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
#include "areg/logging/GELog.h"

#include "aregextend/service/ConnectionHandler.hpp"
#include "aregextend/service/ServerConnection.hpp"


DEF_LOG_SCOPE(areg_aregextend_service_ServerReceiveThread_runDispatcher);

ServerReceiveThread::ServerReceiveThread( ConnectionHandler & connectHandler, RemoteMessageHandler & remoteService, ServerConnection & connection )
    : areg::DispatcherThread  ( areg::SERVER_RECEIVE_MESSAGE_THREAD, areg::DEFAULT_BLOCK_SIZE, areg::QUEUE_SIZE_MAXIMUM )
    , mConnectHandler   ( connectHandler )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesReceive     ( 0 )
    , mSaveDataReceive  ( false )
{
}

bool ServerReceiveThread::runDispatcher()
{
    LOG_SCOPE( areg_aregextend_service_ServerReceiveThread_runDispatcher );
    LOG_DBG("Starting dispatcher [ %s ]", getName().getString());

    readyForEvents(true);
    int32_t whichEvent{ static_cast<int32_t>(EventDispatcherBase::EventSignal::Error) };
    if ( mConnection.serverListen( areg::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        areg::SyncObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        MultiLock multiLock(syncObjects, 2, false);

        areg::RemoteMessage msgReceived;
        uint32_t retryCount = 0;
        do 
        {
            whichEvent = multiLock.lock(areg::DO_NOT_WAIT, false);
            if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue); // escape quit
                areg::SocketAddress addrAccepted;
                SOCKETHANDLE hSocket = mConnection.waitForConnectionEvent(addrAccepted);

                if (mConnection.isValid() == false)
                {
                    LOG_WARN("The server socket is not valid anymore, should quit receive thread!");
                    if (areg::isSocketHandleValid(hSocket))
                    {
                        areg::socketClose(hSocket);
                    }

                    whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                }
                else if (hSocket == areg::FailedSocketHandle)
                {
                    LOG_WARN("Failed selecting server socket, going to retry [ %d ] times before restart.", (RETRY_COUNT - retryCount - 1));
                    if (++retryCount >= RETRY_COUNT)
                    {
                        mConnectHandler.connectionFailure();
                        whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                    }
                }
                else if ( hSocket != areg::InvalidSocketHandle )
                {
                    retryCount = 0;

                    areg::SocketAccepted clientSocket;
                    if (mConnection.isConnectionAccepted(hSocket) )
                    {
                        clientSocket = mConnection.getClientByHandle( hSocket );
                        LOG_DBG("Received connection event of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , clientSocket.getAddress().getHostAddress().getString()
                                            , clientSocket.getAddress().getHostPort());
                    }
                    else
                    {
                        clientSocket = areg::SocketAccepted(hSocket, addrAccepted);
                        if ( mConnectHandler.canAcceptConnection(clientSocket)  )
                        {
                            LOG_DBG("Accepting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.getHostAddress().getString()
                                            , addrAccepted.getHostPort());
                            
                            mConnection.acceptConnection(clientSocket);
                        }
                        else if ( clientSocket.isAlive() )
                        {
                            LOG_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.getHostAddress().getString()
                                            , addrAccepted.getHostPort());
                            
                            mConnection.rejectConnection(clientSocket);
                            clientSocket.closeSocket();
                            continue;
                        }
                        else
                        {
                            LOG_WARN( "The connection of socket [ %u ] is not alive anymore, client [ %s : %d ], ignore connection."
                                        , hSocket
                                        , addrAccepted.getHostAddress( ).getString( )
                                        , addrAccepted.getHostPort( ) );
                            mConnection.closeConnection( clientSocket );
                            continue;
                        }
                    }

#if AREG_LOGS
                    const areg::SocketAddress& addSocket = clientSocket.getAddress();
#endif // AREG_LOGS
                    int32_t sizeReceived = mConnection.receiveMessage(msgReceived, clientSocket);
                    if (sizeReceived > 0 )
                    {
                        if (mSaveDataReceive)
                        {
                            mBytesReceive += static_cast<uint32_t>(sizeReceived);
                        }

                        LOG_DBG("Received message [ %p ] from source [ %p ], client [ %s : %d ]"
                                    , static_cast<id_type>(msgReceived.getMessageId())
                                    , static_cast<id_type>(msgReceived.getSource())
                                    , addSocket.getHostAddress().getString()
                                    , addSocket.getHostPort());

                        mRemoteService.processReceivedMessage(msgReceived, clientSocket);
                    }
                    else
                    {
                        LOG_DBG("Failed to receive message from client socket [ %s : %d ], socket [ %u ]. Going to close connection"
                                        , addSocket.getHostAddress().getString()
                                        , addSocket.getHostPort()
                                        , clientSocket.getHandle());

                        mRemoteService.failedReceiveMessage(clientSocket);
                    }

                    msgReceived.invalidate();
                }
            }
            else
            {
                areg::Event * eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pickEvent() : nullptr;
                whichEvent = isExitEvent(eventElem) ? static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit) : whichEvent;
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue));
    }

    readyForEvents(false);
    removeAllEvents();

    LOG_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());
    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}
