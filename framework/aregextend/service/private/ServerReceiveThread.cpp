/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
    : DispatcherThread  ( areg::SERVER_RECEIVE_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_SIZE_MAXIMUM )
    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mRecvStats        ( )
{
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

            mConnection.accept_connection(clientSocket);
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

    const int32_t sizeReceived = mConnection.receive_message(msgReceived, clientSocket);

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    const areg::SocketAddress& addSocket = clientSocket.address();
    DEBUG_LOG_DBG("Received [ %d ] bytes from socket [ %u ]", sizeReceived, static_cast<uint32_t>(clientSocket.handle()));
#endif   // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

    if (sizeReceived > 0)
    {
        accumulate_received(static_cast<uint64_t>(sizeReceived), 1u);

        DEBUG_LOG_DBG("Received [ %d ] bytes of message [ %u ] from source [ %u ], client [ %s : %d ], message size [ %u ]"
                    , sizeReceived
                    , static_cast<uint32_t>(msgReceived.message_id())
                    , static_cast<uint32_t>(msgReceived.source())
                    , addSocket.host_address().as_string()
                    , addSocket.host_port()
                    , msgReceived.size_used());

        {
            AREG_LT_SCOPE(areg::LtStage::RecvNode);  // router inline route+forward
            mRemoteService.process_received_message(msgReceived, clientSocket);
        }

        if (!areg::ext::drain_recv_cache(mConnection, mRemoteService, areg::DEFAULT_DRAIN_LIMIT - 1u, clientSocket,
                msgReceived, [this](uint64_t bytes, uint32_t msgs) { accumulate_received(bytes, msgs); }))
        {
            mRemoteService.failed_receive_message(clientSocket);
        }
    }
    else
    {
        DEBUG_LOG_WARN("Failed to receive message from client socket [ %s : %d ], socket [ %u ], server valid [ %s ]. Going to close connection"
                        , addSocket.host_address().as_string()
                        , addSocket.host_port()
                        , clientSocket.handle()
                        , mConnection.is_valid() ? "YES" : "NO");

        mRemoteService.failed_receive_message(clientSocket);
    }
}

bool ServerReceiveThread::run_dispatcher()
{
    DEBUG_LOG_SCOPE( areg_aregextend_service_ServerReceiveThread, run_dispatcher );
    DEBUG_LOG_DBG("Starting dispatcher [ %s ]", name().as_string());

    areg::set_receive_mode(areg::ReceiveMode::MultiCache);

    ready_for_events(true);
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };
    if ( mConnection.server_listen( areg::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        SyncEvent* events[2] { &mEventExit, &mEventQueue };
        areg::MessageEnvelope msgReceived;
        areg::SocketAddress addrDrain;
        uint32_t retryCount = 0;
        do
        {
            whichEvent = SyncEvent::wait_any(events, 2, areg::DO_NOT_WAIT);
            if ( whichEvent == SyncEvent::WAIT_ANY_TIMEOUT )
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue); // escape quit
                areg::SocketAddress addrAccepted;
                SOCKETHANDLE hSocket = mConnection.wait_connection(addrAccepted);

                if (!mConnection.is_valid())
                {
                    DEBUG_LOG_WARN("The server socket is not valid anymore, should quit receive thread!");
                    if (areg::is_valid_socket(hSocket))
                    {
                        areg::socket_close(hSocket);
                    }

                    whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                }
                else if (hSocket == areg::FailedSocketHandle)
                {
                    if (++retryCount >= RETRY_COUNT)
                    {
                        mConnectHandler.connection_failure();
                        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
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
                    for (uint32_t drain = 0; drain < areg::DEFAULT_DRAIN_LIMIT; ++drain)
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
                if (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue))
                {
                    Event eventElem{ pick_event() };
                    if (eventElem.is_exit_prio())
                        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                }
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue));
    }

    ready_for_events(false);
    remove_all_events();

    DEBUG_LOG_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.as_string());
    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg::ext
