/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/PoolReceiveThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool receive thread implementation.
 ************************************************************************/
#include "aregextend/service/private/PoolReceiveThread.hpp"

#include "areg/base/MessageEnvelope.hpp"
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

#include "areg/base/private/DebugDefs.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, run_dispatcher);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, _receive_ready_socket);

PoolReceiveThread::PoolReceiveThread( areg::RemoteMessageHandler & remoteService
                                    , ServerConnection & connection
                                    , ServerReceiveThread & globalStats
                                    , std::string_view threadName )
    : DispatcherThread  ( String(threadName), areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_CONTROL_RING_CAPACITY )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mMux              ( areg::DEFAULT_CONNECTIONS )
    , mHandover         ( mMux )
    , mAdded            ( )
    , mReleased         ( )
    , mCachedPending    ( )
    , mParts            ( )
{
}

void PoolReceiveThread::add_socket( const areg::SocketAccepted & clientSocket )
{
    mHandover.add(clientSocket);
}

void PoolReceiveThread::release_socket( areg::SocketAccepted && clientSocket )
{
    mHandover.release(std::move(clientSocket));
}

void PoolReceiveThread::request_stop()
{
    trigger_exit();
    mMux.reset();
}

void PoolReceiveThread::_process_pending_sockets()
{
    if ( mHandover.has_pending() == false )
        return;

    mHandover.take(mAdded, mReleased);
    for ( const areg::SocketAccepted & sock : mAdded )
    {
        if ( sock.is_valid() )
            mMux.register_socket(sock.handle(), true);
    }

    for ( const areg::SocketAccepted & sock : mReleased )
    {
        _drop_socket(sock.handle());
    }

    mAdded.clear();
    mReleased.clear();
}

void PoolReceiveThread::_receive_ready_socket( SOCKETHANDLE hSocket, areg::MessageEnvelope & msgReceived )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, _receive_ready_socket);
    areg::SocketAccepted clientSocket{ mConnection.client_by_handle(hSocket) };
    if ( !clientSocket.is_valid() )
    {
        _drop_socket(hSocket);
        return;
    }

    int32_t received{ 0 };
    {
        AREG_LT_SCOPE(areg::LtStage::RecvNode);  // router inline route+forward
        received = areg::ext::receive_ready_socket(mConnection, mRemoteService, areg::DEFAULT_DRAIN_LIMIT, clientSocket,
                msgReceived, mParts, [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); });
    }

    if ( received < 0 )
    {
        DEBUG_LOG_WARN("Pool receive thread [ %s ]: receive failed on socket [ %u ], notifying connection_lost"
                        , name().as_string()
                        , static_cast<uint32_t>(hSocket));

        mRemoteService.failed_receive_message(clientSocket);
        _drop_socket(hSocket);
    }
    else if ( received == static_cast<int32_t>(areg::DEFAULT_DRAIN_LIMIT) )
    {
        // The ceiling stopped the drain, so the cache may still hold messages.
        _remember_cached_socket(hSocket);
    }
}

void PoolReceiveThread::_drop_socket( SOCKETHANDLE hSocket )
{
    mMux.unregister_socket(hSocket);
    _forget_cached_socket(hSocket);
    areg::thread_rx_cache_release(hSocket);
    mParts.drop(hSocket);
}

void PoolReceiveThread::_remember_cached_socket( SOCKETHANDLE hSocket )
{
    for ( SOCKETHANDLE known : mCachedPending )
    {
        if ( known == hSocket )
            return;
    }

    mCachedPending.push_back(hSocket);
}

void PoolReceiveThread::_forget_cached_socket( SOCKETHANDLE hSocket )
{
    for ( auto pos = mCachedPending.begin(); pos != mCachedPending.end(); ++pos )
    {
        if ( *pos == hSocket )
        {
            mCachedPending.erase(pos);
            return;
        }
    }
}

void PoolReceiveThread::_service_cached_socket( areg::MessageEnvelope & msgReceived )
{
    const SOCKETHANDLE hSocket{ mCachedPending.front() };
    mCachedPending.erase(mCachedPending.begin());

    // A socket this thread no longer monitors may already belong to another pool
    // thread under the same handle value. Only the multiplexer says who owns it.
    if ( !mMux.is_registered(hSocket) || (areg::recv_data_available(hSocket) == 0u) )
        return;

    areg::SocketAccepted clientSocket{ mConnection.client_by_handle(hSocket) };
    if ( !clientSocket.is_valid() )
        return;

    constexpr uint32_t MAX_DRAIN{ areg::DEFAULT_DRAIN_LIMIT - 1u };
    const int32_t drained{ areg::ext::drain_recv_cache(mConnection, mRemoteService, MAX_DRAIN, clientSocket,
            msgReceived, [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_received(bytes, msgs); }) };

    if ( drained < 0 )
    {
        mRemoteService.failed_receive_message(clientSocket);
        _drop_socket(hSocket);
    }
    else if ( drained == static_cast<int32_t>(MAX_DRAIN) )
    {
        mCachedPending.push_back(hSocket);
    }
}

bool PoolReceiveThread::run_dispatcher()
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolReceiveThread, run_dispatcher);
    DEBUG_LOG_DBG("Pool receive thread [ %s ] starting", name().as_string());

    areg::set_receive_mode(areg::ReceiveMode::MultiCache);
    mCachedPending.clear();
    mParts.clear();
    ready_for_events(true);

    areg::MessageEnvelope msgReceived;
    bool isExit{ false };   // set on a clean ExitEvent or a fatal multiplexer wait failure

    do
    {
        _process_pending_sockets();

        if ( mExternalEvents.has_pending() )
        {
            Event eventElem{ pick_event() };
            if ( eventElem.is_exit_prio() )
                isExit = true;
            continue;
        }

        // A socket whose cache still holds messages is invisible to the
        // multiplexer, so it is serviced here and the wait never blocks
        // while one is outstanding.
        SOCKETHANDLE hReady{ areg::InvalidSocketHandle };
        if ( mCachedPending.empty() )
        {
            hReady = mMux.wait();
        }
        else
        {
            _service_cached_socket(msgReceived);
            hReady = mMux.wait(0);
        }

        if ( hReady == areg::FailedSocketHandle )
        {
            isExit = true;
        }
        else if ( hReady == areg::InvalidSocketHandle )
        {
            // Soft wakeup.
            continue;
        }
        else
        {
            _receive_ready_socket(hReady, msgReceived);
            for ( uint32_t drain = 0; drain < areg::DEFAULT_DRAIN_LIMIT; ++drain )
            {
                const SOCKETHANDLE hDrain = mMux.wait(0);
                if ( (hDrain == areg::InvalidSocketHandle) || (hDrain == areg::FailedSocketHandle) )
                    break;

                _receive_ready_socket(hDrain, msgReceived);
            }
        }

    } while ( !isExit );

    ready_for_events(false);
    remove_all_events();

    DEBUG_LOG_DBG("Pool receive thread [ %s ] stopped", name().as_string());

    return isExit;
}

bool PoolReceiveThread::post_event( areg::Event & eventElem )
{
    return areg::EventDispatcher::post_event( eventElem );
}

} // namespace areg::ext
