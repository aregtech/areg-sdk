/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/tcp/private/ServerReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Receive Message Thread of Server socket.
 ************************************************************************/
#include "mcrouter/tcp/private/ServerReceiveThread.hpp"

#include "mcrouter/tcp/private/IEServerConnectionHandler.hpp"
#include "mcrouter/tcp/private/ServerConnection.hpp"
#include "areg/ipc/IERemoteServiceHandler.hpp"
#include "areg/ipc/NEConnection.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ServerReceiveThread_runDispatcher);

ServerReceiveThread::ServerReceiveThread( IEServerConnectionHandler & connectHandler, IERemoteServiceHandler & remoteService, ServerConnection & connection )
    : DispatcherThread  ( NEConnection::SERVER_RECEIVE_MESSAGE_THREAD.data( ) )
    , mRemoteService    ( remoteService )
    , mConnectHandler   ( connectHandler )
    , mConnection       ( connection )
{
}

bool ServerReceiveThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerReceiveThread_runDispatcher);
    TRACE_DBG("Starting dispatcher [ %s ]", getName().getString());

    mEventStarted.setEvent();

    int whichEvent  = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    if ( mConnection.serverListen( NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        MultiLock multiLock(syncObjects, 2, false);

        RemoteMessage msgReceived;
        uint32_t retryCount = 0;
        do 
        {
            whichEvent = multiLock.lock(NECommon::DO_NOT_WAIT, false);
            if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
            {
                whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue); // escape quit
                NESocket::SocketAddress addrAccepted;
                SOCKETHANDLE hSocket = mConnection.waitForConnectionEvent(addrAccepted);

                if (hSocket == NESocket::FailedSocketHandle)
                {
                    TRACE_WARN("Failed selecting server socket, going to retry [ %d ] times before restart.", (RETRY_COUNT - retryCount - 1));
                    if (++retryCount >= RETRY_COUNT)
                    {
                        mConnectHandler.connectionFailure();
                        whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventExit);
                    }
                }
                else if ( hSocket != NESocket::InvalidSocketHandle )
                {
                    retryCount = 0;

                    SocketAccepted clientSocket;
                    if (mConnection.isConnectionAccepted(hSocket) )
                    {
                        clientSocket = mConnection.getClientByHandle( hSocket );
                        TRACE_DBG("Received connection event of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , clientSocket.getAddress().getHostAddress().getString()
                                            , clientSocket.getAddress().getHostPort());
                    }
                    else
                    {
                        clientSocket = SocketAccepted(hSocket, addrAccepted);
                        if ( mConnectHandler.canAcceptConnection(clientSocket)  )
                        {
                            TRACE_DBG("Accepting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.getHostAddress().getString()
                                            , addrAccepted.getHostPort());
                            
                            mConnection.acceptConnection(clientSocket);
                        }
                        else
                        {
                            TRACE_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]"
                                            , hSocket
                                            , addrAccepted.getHostAddress().getString()
                                            , addrAccepted.getHostPort());
                            
                            mConnection.rejectConnection(clientSocket);
                            clientSocket.closeSocket();
                            continue;
                        }
                    }

                    const NESocket::SocketAddress& addSocket = clientSocket.getAddress();
                    if ( mConnection.receiveMessage(msgReceived, clientSocket) > 0 )
                    {
                        TRACE_DBG("Received message [ %p ] from source [ %p ], client [ %s : %d ]"
                                    , static_cast<id_type>(msgReceived.getMessageId())
                                    , static_cast<id_type>(msgReceived.getSource())
                                    , addSocket.getHostAddress().getString()
                                    , addSocket.getHostPort());

                        mRemoteService.processReceivedMessage(msgReceived, addSocket, clientSocket.getHandle());
                    }
                    else
                    {
                        TRACE_DBG("Failed to receive message from client socket [ %s : %d ], socket [ %u ]. Going to close connection"
                                        , addSocket.getHostAddress().getString()
                                        , addSocket.getHostPort()
                                        , clientSocket.getHandle());

                        mRemoteService.failedReceiveMessage(clientSocket.getHandle());
                    }

                    msgReceived.invalidate();
                }
                else
                {
                    OUTPUT_WARN("Client connection lost. Clean master list on next loop or close server socket!");
                }
            }
            else
            {
                Event * eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
                whichEvent = isExitEvent(eventElem) ? static_cast<int>(EventDispatcherBase::eEventOrder::EventExit) : whichEvent;
            }

        } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue));
    }

    mHasStarted = false;
    removeEvents(false);

    mEventStarted.resetEvent();

    TRACE_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());
    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}
