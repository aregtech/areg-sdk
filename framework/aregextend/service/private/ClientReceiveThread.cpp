/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ClientReceiveThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool receive thread implementation.
 ************************************************************************/
#include "aregextend/service/private/ClientReceiveThread.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerReceiveThread.hpp"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ClientReceiveThread, run_dispatcher);

ClientReceiveThread::ClientReceiveThread( areg::ext::ConnectionHandler & connectHandler
                                        , areg::RemoteMessageHandler & remoteService
                                        , ServerConnection & connection
                                        , ServerReceiveThread & globalStats
                                        , std::string_view threadName )
    : DispatcherThread      ( String(threadName), areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_SIZE_MAXIMUM )

    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mMux              ( areg::SocketMultiplexer::DEFAULT_CONNECTIONS )
    , mPendingLock      ( )
    , mPendingAdd       ( )
    , mPendingRemove    ( )
    , mBytesReceive     ( 0u )
    , mMsgsReceive      ( 0u )
    , mSaveDataReceive  ( false )
{
}

void ClientReceiveThread::add_socket( const areg::SocketAccepted & clientSocket )
{
    {
        Lock lock(mPendingLock);
        mPendingAdd.push_back(clientSocket);
    }

    // Interrupt any blocked wait() so the receive thread picks up the new socket
    // at the top of its next loop iteration without a full multiplexer reset.
    mMux.wakeup();
}

void ClientReceiveThread::remove_socket( SOCKETHANDLE hSocket )
{
    {
        Lock lock(mPendingLock);
        mPendingRemove.push_back(hSocket);
    }

    mMux.wakeup();
}

void ClientReceiveThread::request_stop()
{
    // Signal the event-loop exit first so run_dispatcher() sees the exit condition
    // even if the multiplexer is not currently blocking.
    trigger_exit();

    // Hard-reset the multiplexer: unblocks wait() with FailedSocketHandle so the
    // receive thread does not stay blocked until the next client sends data.
    mMux.reset();
}

void ClientReceiveThread::_process_pending_sockets()
{
    Lock lock(mPendingLock);

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

bool ClientReceiveThread::run_dispatcher()
{
    LOG_SCOPE(areg_aregextend_service_ClientReceiveThread, run_dispatcher);
    LOG_DBG("Pool receive thread [ %s ] starting", name().as_string());

    ready_for_events(true);

    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) };
    areg::RemoteMessage msgReceived;

    SyncObject* syncObjs[2] = { &mEventExit, &mEventQueue };
    MultiLock multiLock(syncObjs, 2, false);

    do
    {
        // Apply pending socket registrations / unregistrations before blocking.
        _process_pending_sockets();

        // Non-blocking check: handle any already-signaled exit or queue events
        // without blocking on the multiplexer.
        whichEvent = multiLock.lock(areg::DO_NOT_WAIT, false);
        if ( whichEvent != MultiLock::LOCK_INDEX_TIMEOUT )
        {
            Event * eventElem = ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) )
                                    ? pick_event()
                                    : nullptr;
            whichEvent = is_exit_event(eventElem)
                            ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit)
                            : whichEvent;
            continue;
        }

        // No pending events — block until a socket becomes readable or we are interrupted.
        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue);
        const SOCKETHANDLE hReady = mMux.wait();

        if ( hReady == areg::FailedSocketHandle )
        {
            // Hard stop: request_stop() called mMux.reset() or init failed.
            whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
        }
        else if ( hReady == areg::InvalidSocketHandle )
        {
            // Soft wakeup: wakeup() was called to signal pending socket changes.
            // Loop back to _process_pending_sockets() at the top.
            continue;
        }
        else
        {
            // A socket is readable — receive the message.
            areg::SocketAccepted clientSocket = mConnection.client_by_handle(hReady);
            if ( !clientSocket.is_valid() )
            {
                // Socket no longer tracked in the connection map.
                mMux.unregister_socket(hReady);
                continue;
            }

            const int32_t received = mConnection.receive_message(msgReceived, clientSocket);
            if ( received > 0 )
            {
                if ( mSaveDataReceive )
                {
                    const uint64_t bytes = static_cast<uint64_t>(received);
                    mBytesReceive.fetch_add(bytes, std::memory_order_relaxed);
                    mMsgsReceive.fetch_add(1u, std::memory_order_relaxed);
                    mGlobalStats.accumulate_received(bytes, 1u);
                }

                mRemoteService.process_received_message(msgReceived, clientSocket);
                msgReceived.invalidate();
            }
            else
            {
                // Receive failed: remove socket from the multiplexer.  The
                // connection_lost() callback (via failed_receive_message) will
                // close the connection and send the disconnect notification.
                LOG_WARN("Pool receive thread [ %s ]: receive failed on socket [ %u ], notifying connection_lost"
                            , name().as_string()
                            , static_cast<uint32_t>(hReady));

                mMux.unregister_socket(hReady);
                mRemoteService.failed_receive_message(clientSocket);
            }
        }

    } while ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) );

    ready_for_events(false);
    remove_all_events();

    LOG_DBG("Pool receive thread [ %s ] stopped", name().as_string());

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg::ext
