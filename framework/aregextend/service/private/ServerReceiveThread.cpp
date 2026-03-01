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
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread_runDispatcher);

ServerReceiveThread::ServerReceiveThread( ConnectionHandler & connectHandler, RemoteMessageHandler & remoteService, ServerConnection & connection )
    : DispatcherThread  ( areg::SERVER_RECEIVE_MESSAGE_THREAD, areg::DEFAULT_BLOCK_SIZE, areg::QUEUE_SIZE_MAXIMUM )
    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesReceive     ( 0 )
    , mSaveDataReceive  ( false )
{
}

bool ServerReceiveThread::run_dispatcher()
{
    LOG_SCOPE( areg_aregextend_service_ServerReceiveThread_runDispatcher );
    LOG_DBG("Starting dispatcher [ %s ]", name().as_string());

    ready_for_events(true);
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };
    if ( mConnection.server_listen( areg::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        SyncObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        MultiLock multiLock(syncObjects, 2, false);

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
                    if (areg::is_handle_valid(hSocket))
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

                    SocketAccepted clientSocket;
                    if (mConnection.is_connection_accepted(hSocket) )
                    {
                        clientSocket = mConnection.client_by_handle( hSocket );
                        LOG_DBG("Received connection event of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , clientSocket.address().host_address().as_string()
                                            , clientSocket.address().host_port());
                    }
                    else
                    {
                        clientSocket = SocketAccepted(hSocket, addrAccepted);
                        if ( mConnectHandler.can_accept_connection(clientSocket)  )
                        {
                            LOG_DBG("Accepting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.host_address().as_string()
                                            , addrAccepted.host_port());
                            
                            mConnection.accept_connection(clientSocket);
                        }
                        else if ( clientSocket.is_alive() )
                        {
                            LOG_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.host_address().as_string()
                                            , addrAccepted.host_port());
                            
                            mConnection.reject_connection(clientSocket);
                            clientSocket.close_socket();
                            continue;
                        }
                        else
                        {
                            LOG_WARN( "The connection of socket [ %u ] is not alive anymore, client [ %s : %d ], ignore connection."
                                        , hSocket
                                        , addrAccepted.host_address( ).as_string( )
                                        , addrAccepted.host_port( ) );
                            mConnection.close_connection( clientSocket );
                            continue;
                        }
                    }

#if AREG_LOGS
                    const areg::SocketAddress& addSocket = clientSocket.address();
#endif // AREG_LOGS
                    int32_t sizeReceived = mConnection.receive_message(msgReceived, clientSocket);
                    if (sizeReceived > 0 )
                    {
                        if (mSaveDataReceive)
                        {
                            mBytesReceive += static_cast<uint32_t>(sizeReceived);
                        }

                        LOG_DBG("Received message [ %p ] from source [ %p ], client [ %s : %d ]"
                                    , static_cast<id_type>(msgReceived.message_id())
                                    , static_cast<id_type>(msgReceived.source())
                                    , addSocket.host_address().as_string()
                                    , addSocket.host_port());

                        mRemoteService.process_received_message(msgReceived, clientSocket);
                    }
                    else
                    {
                        LOG_DBG("Failed to receive message from client socket [ %s : %d ], socket [ %u ]. Going to close connection"
                                        , addSocket.host_address().as_string()
                                        , addSocket.host_port()
                                        , clientSocket.handle());

                        mRemoteService.failed_receive_message(clientSocket);
                    }

                    msgReceived.invalidate();
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
