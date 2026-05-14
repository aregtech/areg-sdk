/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/PoolReceiveThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool receive thread implementation.
 ************************************************************************/
#include "aregextend/service/private/PoolReceiveThread.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerReceiveThread.hpp"
#include "aregextend/service/private/ClientConnectionPair.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, run_dispatcher);

PoolReceiveThread::PoolReceiveThread( areg::RemoteMessageHandler & remoteService
                                    , ServerConnection & connection
                                    , ServerReceiveThread & globalStats
                                    , std::string_view threadName )
    : DispatcherThread  ( String(threadName), areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_SIZE_MAXIMUM )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mMux              ( areg::DEFAULT_CONNECTIONS )
    , mHasPending       ( false )
    , mPendingLock      ( )
    , mPendingAdd       ( )
    , mPendingRemove    ( )
{
}

void PoolReceiveThread::add_socket( const areg::SocketAccepted & clientSocket )
{
    {
        Lock lock(mPendingLock);
        mPendingAdd.push_back(clientSocket);
        mHasPending.store(true, std::memory_order_release);
    }

    // Interrupt any blocked wait() to pick up new socket.
    mMux.wakeup();
}

void PoolReceiveThread::remove_socket( SOCKETHANDLE hSocket )
{
    {
        Lock lock(mPendingLock);
        mPendingRemove.push_back(hSocket);
        mHasPending.store(true, std::memory_order_release);
    }

    mMux.wakeup();
}

void PoolReceiveThread::request_stop()
{
    trigger_exit();
    mMux.reset();
}

void PoolReceiveThread::_process_pending_sockets()
{
    if ( !mHasPending.load(std::memory_order_acquire) )
        return;

    Lock lock(mPendingLock);
    mHasPending.store(false, std::memory_order_relaxed);

    for ( SOCKETHANDLE hRemove : mPendingRemove )
        mMux.unregister_socket(hRemove);

    mPendingRemove.clear();
    for ( const areg::SocketAccepted & sock : mPendingAdd )
    {
        if ( sock.is_valid() )
            mMux.register_socket(sock.handle(), true);
    }

    mPendingAdd.clear();
}

bool PoolReceiveThread::run_dispatcher()
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, run_dispatcher);
    DEBUG_LOG_DBG("Pool receive thread [ %s ] starting", name().as_string());

    areg::set_receive_mode(areg::ReceiveMode::MultiCache);
    ready_for_events(true);

    areg::RemoteMessage msgReceived;
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) };
    SyncObject* syncObjs[2] = { &mEventExit, &mEventQueue };
    MultiLock multiLock(syncObjs, 2, false);

    do
    {
        _process_pending_sockets();

        whichEvent = multiLock.lock(areg::DO_NOT_WAIT, false);
        if ( whichEvent != MultiLock::LOCK_INDEX_TIMEOUT )
        {
            Event * eventElem = ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ) ? pick_event() : nullptr;
            whichEvent = is_exit_event(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
            continue;
        }

        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue);
        const SOCKETHANDLE hReady = mMux.wait();

        if ( hReady == areg::FailedSocketHandle )
        {
            whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
        }
        else if ( hReady == areg::InvalidSocketHandle )
        {
            // Soft wakeup.
            continue;
        }
        else
        {
            areg::SocketAccepted clientSocket = mConnection.client_by_handle(hReady);
            if ( !clientSocket.is_valid() )
            {
                mMux.unregister_socket(hReady);
                continue;
            }

            const int32_t received = mConnection.receive_message(msgReceived, clientSocket);
            if ( received > 0 )
            {
                mGlobalStats.accumulate_received(static_cast<uint64_t>(received), 1u);
                mRemoteService.process_received_message(msgReceived, clientSocket);

                // Drain bytes cached by _os_recv_data read-ahead.
                if (!areg::ext::drain_recv_cache(mConnection, mRemoteService, areg::THREAD_DRAIN_LIMIT - 1u, clientSocket,
                        msgReceived, [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); }))
                {
                    mMux.unregister_socket(hReady);
                    mRemoteService.failed_receive_message(clientSocket);
                }
            }
            else
            {
                DEBUG_LOG_WARN("Pool receive thread [ %s ]: receive failed on socket [ %u ], notifying connection_lost"
                                , name().as_string()
                                , static_cast<uint32_t>(hReady));

                mMux.unregister_socket(hReady);
                mRemoteService.failed_receive_message(clientSocket);
            }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
            int32_t drainCount{ 0 };
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
            for ( uint32_t drain = 0; drain < areg::THREAD_DRAIN_LIMIT; ++drain )
            {
                const SOCKETHANDLE hDrain = mMux.wait(0);
                if ( (hDrain == areg::InvalidSocketHandle) || (hDrain == areg::FailedSocketHandle) )
                    break;

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                ++drainCount;
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                areg::SocketAccepted drainSocket = mConnection.client_by_handle(hDrain);
                if ( !drainSocket.is_valid() )
                {
                    mMux.unregister_socket(hDrain);
                    continue;
                }

                const int32_t drainReceived = mConnection.receive_message(msgReceived, drainSocket);
                if ( drainReceived > 0 )
                {
                    mGlobalStats.accumulate_received(static_cast<uint64_t>(drainReceived), 1u);
                    mRemoteService.process_received_message(msgReceived, drainSocket);

                    // Drain read-ahead cache for this socket too.
                    if (!areg::ext::drain_recv_cache(mConnection, mRemoteService, areg::THREAD_DRAIN_LIMIT - 1u, drainSocket,
                            msgReceived, [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); }))
                    {
                        mMux.unregister_socket(hDrain);
                        mRemoteService.failed_receive_message(drainSocket);
                    }
                }
                else
                {
                    DEBUG_LOG_WARN("Pool receive thread [ %s ]: receive failed on drain socket [ %u ], notifying connection_lost", name().as_string(), static_cast<uint32_t>(hDrain));
                    mMux.unregister_socket(hDrain);
                    mRemoteService.failed_receive_message(drainSocket);
                }
            }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
            if (drainCount >= areg::THREAD_DRAIN_LIMIT)
            {
                DEBUG_LOG_WARN("Receive drain loop exhausted thread drain limit (%d) -- inbound event queue is growing", areg::THREAD_DRAIN_LIMIT);
            }
#endif   // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
        }

    } while ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) );

    ready_for_events(false);
    remove_all_events();

    DEBUG_LOG_DBG("Pool receive thread [ %s ] stopped", name().as_string());

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg::ext
