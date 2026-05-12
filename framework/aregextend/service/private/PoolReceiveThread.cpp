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

    // Interrupt any blocked wait() so the receive thread picks up the new socket
    // at the top of its next loop iteration without a full multiplexer reset.
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
    // Signal the event-loop exit first so run_dispatcher() sees the exit condition
    // even if the multiplexer is not currently blocking.
    trigger_exit();

    // Hard-reset the multiplexer: unblocks wait() with FailedSocketHandle so the
    // receive thread does not stay blocked until the next client sends data.
    mMux.reset();
}

void PoolReceiveThread::_process_pending_sockets()
{
    // Fast path: skip the lock entirely when no socket changes are queued.
    // At steady state (no connects/disconnects) this eliminates one ResourceLock
    // acquisition per loop iteration (~100–150 ns).
    if ( !mHasPending.load(std::memory_order_acquire) )
        return;

    Lock lock(mPendingLock);
    // Reset the flag inside the lock so a racing add_socket()/remove_socket() that
    // stores true after this load but before the lock is taken will be processed in
    // the next iteration (not silently lost).
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

    // Per-socket RX cache is safe for multi-socket threads: thread_rx_cache(hSocket)
    // gives each socket handle its own isolated ThreadCache.
    areg::set_receive_mode(areg::ReceiveMode::Cached);

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
            Event * eventElem = ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ) ? pick_event() : nullptr;
            whichEvent = is_exit_event(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
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
                mGlobalStats.accumulate_received(static_cast<uint64_t>(received), 1u);
                mRemoteService.process_received_message(msgReceived, clientSocket);

                // Drain bytes cached by _os_recv_data read-ahead.
                if (!areg::ext::drain_recv_cache(mConnection, mRemoteService, msgReceived, clientSocket,
                        [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); }))
                {
                    mMux.unregister_socket(hReady);
                    mRemoteService.failed_receive_message(clientSocket);
                }
            }
            else
            {
                // Receive failed: remove socket from the multiplexer.  The
                // connection_lost() callback (via failed_receive_message) will
                // close the connection and send the disconnect notification.
                DEBUG_LOG_WARN("Pool receive thread [ %s ]: receive failed on socket [ %u ], notifying connection_lost"
                                , name().as_string()
                                , static_cast<uint32_t>(hReady));

                mMux.unregister_socket(hReady);
                mRemoteService.failed_receive_message(clientSocket);
            }

            // Drain additional ready sockets without re-entering the full loop overhead.
            // Under burst load (C1: 3 MB msgs, C3: 1:N fan-out) multiple sockets can
            // become readable simultaneously.  mMux.wait(0) polls without blocking:
            // it serves from the internal batch cache first (no syscall), then does one
            // non-blocking epoll_wait/kevent/WSAPoll if the cache is empty.
            // This avoids the per-message cost of _process_pending_sockets() + multiLock.lock().
            constexpr int32_t DRAIN_LIMIT{ areg::THREAD_DRAIN_LIMIT };
            for ( int32_t drain = 0; drain < DRAIN_LIMIT; ++drain )
            {
                const SOCKETHANDLE hDrain = mMux.wait(0);
                if ( (hDrain == areg::InvalidSocketHandle) || (hDrain == areg::FailedSocketHandle) )
                    break;

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
                    if (!areg::ext::drain_recv_cache(mConnection, mRemoteService, msgReceived, drainSocket,
                            [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); }))
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
        }

    } while ( whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) );

    ready_for_events(false);
    remove_all_events();

    DEBUG_LOG_DBG("Pool receive thread [ %s ] stopped", name().as_string());

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg::ext
