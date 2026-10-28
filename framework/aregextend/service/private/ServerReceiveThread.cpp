/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/ServerReceiveThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server receive message Thread
 ************************************************************************/
#include "aregextend/service/private/ServerReceiveThread.hpp"

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread, run_dispatcher);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread, _process_connection_event);

ServerReceiveThread::ServerReceiveThread( ConnectionHandler & connectHandler, RemoteMessageHandler & remoteService, ServerConnection & connection )
    : DispatcherThread  ( areg::SERVER_RECEIVE_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_CONTROL_RING_CAPACITY )
    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mRecvStats        ( )
    , mCachedPending    ( )
    , mHandover         ( connection.multiplexer() )
    , mAdded            ( )
    , mReleased         ( )
    , mParts            ( )
    , mPaused           ( connection.multiplexer() )
{
}

void ServerReceiveThread::release_socket(ITEM_ID /*cookie*/, areg::SocketAccepted && socket)
{
    mHandover.release(std::move(socket));
}

void ServerReceiveThread::_release_handed_over()
{
    mHandover.take(mAdded, mReleased);
    for ( const areg::SocketAccepted & sock : mReleased )
    {
        _drop_socket(sock.handle());
    }

    mAdded.clear();
    mReleased.clear();
}

void ServerReceiveThread::_process_connection_event(SOCKETHANDLE hSocket, const areg::SocketAddress & addrAccepted, areg::MessageEnvelope & msgReceived)
{
    DEBUG_LOG_SCOPE( areg_aregextend_service_ServerReceiveThread, _process_connection_event );
    SocketAccepted clientSocket;
    if (!mConnection.is_connection_accepted(hSocket, clientSocket))
    {
        if ( !addrAccepted.is_valid() )
        {
            DEBUG_LOG_WARN("Socket [ %u ] is no longer registered, it was deregistered while still in the event queue. Ignoring.", hSocket);
            return;
        }

        clientSocket = SocketAccepted(hSocket, std::move(addrAccepted));
        if (mConnectHandler.can_accept_connection(clientSocket))
        {
            DEBUG_LOG_INFO("Accepting new connection of socket [ %u ], client [ %s : %d ]"
                        , hSocket
                        , clientSocket.address().host_address().as_string()
                        , clientSocket.address().host_port());

            if ( mConnection.accept_connection(clientSocket, static_cast<SocketRelease &>(*this)) == false )
                return;

            if ( mConnectHandler.on_client_accepted(clientSocket) )
                return;
        }
        else if (clientSocket.is_alive())
        {
            DEBUG_LOG_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]"
                            , hSocket
                            , clientSocket.address().host_address().as_string()
                            , clientSocket.address().host_port());

            mConnection.reject_connection(clientSocket);
            clientSocket.close();
            return;
        }
        else
        {
            DEBUG_LOG_WARN("The connection of socket [ %u ] is not alive anymore, client [ %s : %d ], ignore connection."
                            , hSocket
                            , clientSocket.address().host_address().as_string()
                            , clientSocket.address().host_port());

            mConnection.close_connection(clientSocket);
            return;
        }
    }

    DEBUG_LOG_DBG("Received connection event of socket [ %u ], client [ %s : %d ]"
                , hSocket
                , clientSocket.address().host_address().as_string()
                , clientSocket.address().host_port());

    int32_t received{ 0 };
    {
        AREG_LT_SCOPE(areg::LtStage::RecvNode);  // router inline route+forward
        received = areg::ext::receive_ready_socket(mConnection, mRemoteService, areg::DEFAULT_DRAIN_LIMIT, clientSocket,
                msgReceived, mParts, [this](uint64_t bytes, uint32_t msgs) { accumulate_received(bytes, msgs); });
    }

    if (received < 0)
    {
        PausedSources::discard();
        DEBUG_LOG_WARN("Failed to receive message from client socket [ %s : %d ], socket [ %u ], server valid [ %s ]. Going to close connection"
                        , clientSocket.address().host_address().as_string()
                        , clientSocket.address().host_port()
                        , clientSocket.handle()
                        , mConnection.is_valid() ? "YES" : "NO");

        mRemoteService.failed_receive_message(clientSocket);
        _drop_socket(clientSocket.handle());
    }
    else
    {
        _after_drain(clientSocket.handle(), received == static_cast<int32_t>(areg::DEFAULT_DRAIN_LIMIT));
    }
}

void ServerReceiveThread::_drop_socket(SOCKETHANDLE hSocket)
{
    mConnection.unregister_from_multiplexer(hSocket);
    _forget_cached_socket(hSocket);
    mPaused.drop(hSocket);
    areg::thread_rx_cache_release(hSocket);
    mParts.drop(hSocket);
}

void ServerReceiveThread::_after_drain(SOCKETHANDLE hSocket, bool ceilingHit)
{
    if (is_receive_paused())
    {
        if (mPaused.pause(hSocket))
        {
            mConnection.unregister_from_multiplexer(hSocket);
            _forget_cached_socket(hSocket);
            return;
        }

        // The drain stopped before the cache was empty.
        ceilingHit = true;
    }

    if (ceilingHit)
    {
        _remember_cached_socket(hSocket);
    }
}

void ServerReceiveThread::_resume_paused()
{
    mPaused.resume([this](SOCKETHANDLE hSocket)
    {
        if (mConnection.client_by_handle(hSocket).is_valid() && mConnection.multiplexer().register_socket(hSocket, true))
        {
            _remember_cached_socket(hSocket);
        }
    });
}

void ServerReceiveThread::_remember_cached_socket(SOCKETHANDLE hSocket)
{
    for (SOCKETHANDLE known : mCachedPending)
    {
        if (known == hSocket)
            return;
    }

    mCachedPending.push_back(hSocket);
}

void ServerReceiveThread::_forget_cached_socket(SOCKETHANDLE hSocket)
{
    for (auto pos = mCachedPending.begin(); pos != mCachedPending.end(); ++pos)
    {
        if (*pos == hSocket)
        {
            mCachedPending.erase(pos);
            return;
        }
    }
}

void ServerReceiveThread::_service_cached_socket(areg::MessageEnvelope & msgReceived)
{
    const SOCKETHANDLE hSocket{ mCachedPending.front() };
    mCachedPending.erase(mCachedPending.begin());

    if (areg::recv_data_available(hSocket) == 0u)
        return;

    areg::SocketAccepted clientSocket{ mConnection.client_by_handle(hSocket) };
    if (!clientSocket.is_valid())
        return;

    constexpr uint32_t MAX_DRAIN{ areg::DEFAULT_DRAIN_LIMIT - 1u };
    const int32_t drained{ areg::ext::drain_recv_cache(mConnection, mRemoteService, MAX_DRAIN, clientSocket,
            msgReceived, [this](uint64_t bytes, uint32_t msgs) { accumulate_received(bytes, msgs); }) };

    if (drained < 0)
    {
        PausedSources::discard();
        mRemoteService.failed_receive_message(clientSocket);
        _drop_socket(hSocket);
    }
    else
    {
        _after_drain(hSocket, drained == static_cast<int32_t>(MAX_DRAIN));
    }
}

bool ServerReceiveThread::run_dispatcher()
{
    DEBUG_LOG_SCOPE( areg_aregextend_service_ServerReceiveThread, run_dispatcher );
    DEBUG_LOG_DBG("Starting dispatcher [ %s ]", name().as_string());

    areg::set_receive_mode(areg::ReceiveMode::MultiCache);

    // This thread object outlives a run and is started again on a reconnect.
    mCachedPending.clear();
    mParts.clear();
    mPaused.clear();

    ready_for_events(true);
    bool isExit{ false };   // stays false if server_listen() fails (failure return); set on a clean ExitEvent
    if ( mConnection.server_listen( areg::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        areg::MessageEnvelope msgReceived;
        areg::SocketAddress addrAccepted;
        areg::SocketAddress addrDrain;
        uint32_t retryCount = 0;
        do
        {
            if ( mHandover.has_pending() )
            {
                _release_handed_over();
            }

            if ( mPaused.has_released() )
            {
                _resume_paused();
            }

            if ( !mExternalEvents.has_pending() )
            {
                // A socket whose cache still holds messages is invisible to the
                // multiplexer, so it is serviced here and the wait never blocks
                // while one is outstanding.
                SOCKETHANDLE hSocket{ areg::InvalidSocketHandle };
                if ( mCachedPending.empty() )
                {
                    hSocket = mConnection.wait_connection(addrAccepted);
                }
                else
                {
                    _service_cached_socket(msgReceived);
                    hSocket = mConnection.wait_connection_nowait(addrAccepted);
                }

                if (!mConnection.is_valid())
                {
                    DEBUG_LOG_WARN("The server socket is not valid anymore, should quit receive thread!");
                    if (areg::is_valid_socket(hSocket))
                    {
                        areg::socket_close(hSocket);
                    }

                    isExit = true;
                }
                else if (hSocket == areg::FailedSocketHandle)
                {
                    if (++retryCount >= RETRY_COUNT)
                    {
                        mConnectHandler.connection_failure();
                        isExit = true;
                    }

                    DEBUG_LOG_WARN("Failed selecting server socket, going to retry [ %d ] times before restart.", (RETRY_COUNT - retryCount));
                }
                else if ( hSocket != areg::InvalidSocketHandle )
                {
                    retryCount = 0;
                    _process_connection_event(hSocket, addrAccepted, msgReceived);
                    // msgReceived.invalidate();

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                    uint32_t drainCount{ 0 };
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                    for (uint32_t drain = 0; (drain < areg::DEFAULT_DRAIN_LIMIT) && mConnection.has_ready_connection(); ++drain)
                    {
                        const SOCKETHANDLE hDrain = mConnection.wait_connection_nowait(addrDrain);
                        if ( !areg::is_valid_socket(hDrain) )
                        {
                            break;
                        }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                        ++drainCount;
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                        _process_connection_event(hDrain, addrDrain, msgReceived);
                        // msgReceived.invalidate();
                    }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                    if (drainCount >= areg::DEFAULT_DRAIN_LIMIT)
                    {
                        DEBUG_LOG_WARN("Receive drain loop exhausted thread drain limit (%d) -- inbound event queue is growing", areg::DEFAULT_DRAIN_LIMIT);
                    }
#endif   // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
                }
            }
            else
            {
                Event eventElem{ pick_event() };
                if ( eventElem.is_exit_prio() )
                    isExit = true;
            }

        } while ( !isExit );
    }

    mPaused.clear();
    ready_for_events(false);
    remove_all_events();

    DEBUG_LOG_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.as_string());
    return isExit;
}

bool ServerReceiveThread::post_event( areg::Event & eventElem )
{
    return areg::EventDispatcher::post_event( eventElem );
}

} // namespace areg::ext
