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

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread, run_dispatcher);
DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread, _process_connection_event);

ServerReceiveThread::ServerReceiveThread( ConnectionHandler & connectHandler, RemoteMessageHandler & remoteService, ServerConnection & connection )
    : DispatcherThread  ( areg::SERVER_RECEIVE_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::QUEUE_SIZE_MAXIMUM )
    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesReceive     ( 0 )
    , mSaveDataReceive  ( false )
{
}

void ServerReceiveThread::_process_connection_event(SOCKETHANDLE hSocket, const areg::SocketAddress & addrAccepted, areg::RemoteMessage & msgReceived)
{
    LOG_SCOPE( areg_aregextend_service_ServerReceiveThread, _process_connection_event );
    SocketAccepted clientSocket;
    if (!mConnection.is_connection_accepted(hSocket, clientSocket))
    {
        // `addrAccepted` is only populated by server_accept() when ::accept() is called
        // If the address is invalid, this socket was previously accepted but has
        // since been deregistered (e.g. by the service thread processing a disconnect
        // message).  Do NOT re-accept it, that would restart the loop indefinitely.
        // Simply close our fd reference and return.
        if ( !addrAccepted.is_valid() )
        {
            LOG_WARN("Socket [ %u ] is no longer registered, it was deregistered while still in the event queue. Ignoring.", hSocket);
            return;
        }

        clientSocket = SocketAccepted(hSocket, addrAccepted);
        if (mConnectHandler.can_accept_connection(clientSocket))
        {
            LOG_INFO("Accepting new connection of socket [ %u ], client [ %s : %d ]"
                        , hSocket
                        , addrAccepted.host_address().as_string()
                        , addrAccepted.host_port());

            mConnection.accept_connection(clientSocket);

            // Offer the accepted socket to a dedicated per-client thread pair.
            // If on_client_accepted() returns true, the pair took ownership:
            //   - the socket is removed from the multiplexer watch set inside the callback
            //   - this thread must NOT call receive_message() for it
            if ( mConnectHandler.on_client_accepted(clientSocket) )
                return;
        }
        else if (clientSocket.is_alive())
        {
            LOG_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]"
                        , hSocket
                        , addrAccepted.host_address().as_string()
                        , addrAccepted.host_port());

            mConnection.reject_connection(clientSocket);
            clientSocket.close();
            return;
        }
        else
        {
            LOG_WARN("The connection of socket [ %u ] is not alive anymore, client [ %s : %d ], ignore connection."
                        , hSocket
                        , addrAccepted.host_address().as_string()
                        , addrAccepted.host_port());

            mConnection.close_connection(clientSocket);
            return;
        }
    }

    LOG_DBG("Received connection event of socket [ %u ], client [ %s : %d ]"
                , hSocket
                , clientSocket.address().host_address().as_string()
                , clientSocket.address().host_port());
    const int32_t sizeReceived = mConnection.receive_message(msgReceived, clientSocket);

#if AREG_LOGGING
    const areg::SocketAddress& addSocket = clientSocket.address();
    DEBUG_LOG_DBG("Received [ %d ] bytes from socket [ %u ]", sizeReceived, static_cast<uint32_t>(clientSocket.handle()));
#endif // AREG_LOGGING

    if (sizeReceived > 0)
    {
        if (mSaveDataReceive)
        {
            mBytesReceive += static_cast<uint32_t>(sizeReceived);
            mMsgsReceive  += 1u;
        }

        DEBUG_LOG_DBG("Received [ %d ] bytes of message [ %u ] from source [ %u ], client [ %s : %d ], message size [ %u ]"
                    , sizeReceived
                    , static_cast<uint32_t>(msgReceived.message_id())
                    , static_cast<uint32_t>(msgReceived.source())
                    , addSocket.host_address().as_string()
                    , addSocket.host_port()
                    , msgReceived.size_used());

        mRemoteService.process_received_message(msgReceived, clientSocket);
    }
    else
    {
        LOG_WARN("Failed to receive message from client socket [ %s : %d ], socket [ %u ], server valid [ %s ]. Going to close connection"
                    , addSocket.host_address().as_string()
                    , addSocket.host_port()
                    , clientSocket.handle()
                    , mConnection.is_valid() ? "YES" : "NO");

        mRemoteService.failed_receive_message(clientSocket);
    }
}

bool ServerReceiveThread::run_dispatcher()
{
    LOG_SCOPE( areg_aregextend_service_ServerReceiveThread, run_dispatcher );
    LOG_DBG("Starting dispatcher [ %s ]", name().as_string());

    ready_for_events(true);
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };
    if ( mConnection.server_listen( areg::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        SyncObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        MultiLock multiLock(syncObjects, 2, false);

        // Maximum number of additional sockets to drain per blocking wait() wakeup.
        // Limits how long the inner loop holds the receive thread before checking
        // for exit events, ensuring shutdown is never delayed more than DRAIN_LIMIT
        // message-receive operations.
        constexpr int32_t DRAIN_LIMIT{ areg::THREAD_DRAIN_LIMIT };

        RemoteMessage msgReceived;
        uint32_t retryCount = 0;
        do
        {
            whichEvent = multiLock.lock(areg::DO_NOT_WAIT, false);
            if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue); // escape quit
                areg::SocketAddress addrAccepted;
                SOCKETHANDLE hSocket = mConnection.wait_connection(addrAccepted);

                if (mConnection.is_valid() == false)
                {
                    LOG_WARN("The server socket is not valid anymore, should quit receive thread!");
                    if (areg::is_valid_socket(hSocket))
                    {
                        areg::socket_close(hSocket);
                    }

                    whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                }
                else if (hSocket == areg::FailedSocketHandle)
                {
                    LOG_WARN("Failed selecting server socket, going to retry [ %d ] times before restart.", (RETRY_COUNT - retryCount - 1));
                    if (++retryCount >= RETRY_COUNT)
                    {
                        mConnectHandler.connection_failure();
                        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                    }
                }
                else if ( hSocket != areg::InvalidSocketHandle )
                {
                    retryCount = 0;
                    _process_connection_event(hSocket, addrAccepted, msgReceived);
                    msgReceived.invalidate();

                    // Drain additional ready sockets without re-entering the blocking wait().
                    // Under burst load, multiple clients may be readable simultaneously.
                    // One non-blocking poll per extra socket avoids a full epoll/kqueue/WSAPoll
                    // syscall roundtrip per message, significantly improving throughput.
                    int32_t drainCount{ 0 };
                    for (int32_t drain = 0; drain < DRAIN_LIMIT; ++drain)
                    {
                        areg::SocketAddress addrDrain;
                        const SOCKETHANDLE hDrain = mConnection.wait_connection_nowait(addrDrain);
                        if (    (hDrain == areg::InvalidSocketHandle)   // nothing ready
                             || (hDrain == areg::FailedSocketHandle) )  // error / reset
                        {
                            break;
                        }

                        ++drainCount;
                        _process_connection_event(hDrain, addrDrain, msgReceived);
                        // msgReceived.invalidate();
                    }

                    // If the drain loop saturated its limit, there are still more
                    // sockets ready.  Under normal load this should not happen
                    // continuously — a persistent warning here means the receive
                    // thread cannot keep up and the inbound queue is growing.
                    if (drainCount >= DRAIN_LIMIT)
                    {
                        LOG_WARN("Receive drain loop exhausted DRAIN_LIMIT (%d) — inbound event queue is growing", DRAIN_LIMIT);
                    }
                }
            }
            else
            {
                Event * eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pick_event() : nullptr;
                whichEvent = is_exit_event(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue));
    }

    ready_for_events(false);
    remove_all_events();

    LOG_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.as_string());
    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

} // namespace areg::ext
