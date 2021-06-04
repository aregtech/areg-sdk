/************************************************************************
 * \file        areg/ipc/private/ServerReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Receive Message Thread of Server socket.
 ************************************************************************/
#include "areg/ipc/private/ServerReceiveThread.hpp"

#include "areg/ipc/private/IEServerConnectionHandler.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/ipc/private/ServerConnection.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_ServerReceiveThread_runDispatcher);

ServerReceiveThread::ServerReceiveThread( IEServerConnectionHandler & connectHandler, IERemoteServiceHandler & remoteService, ServerConnection & connection )
    : DispatcherThread    ( NEConnection::SERVER_RECEIVE_MESSAGE_THREAD )
    , mRemoteService        ( remoteService )
    , mConnectHandler       ( connectHandler )
    , mConnection           ( connection )
{
    ; // do nothing
}

ServerReceiveThread::~ServerReceiveThread(void)
{
    ; // do nothing
}

bool ServerReceiveThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ServerReceiveThread_runDispatcher);
    TRACE_DBG("Starting dispatcher [ %s ]", getName().getString());

    mEventStarted.setEvent();

    int whichEvent  = static_cast<int>(EVENT_ERROR);
    if ( mConnection.serverListen( NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        MultiLock multiLock(syncObjects, 2, false);

        RemoteMessage msgReceived;
        do 
        {
            whichEvent = multiLock.lock(IESynchObject::DO_NOT_WAIT, false);
            if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
            {
                whichEvent = static_cast<int>(EVENT_QUEUE); // escape quit
                NESocket::InterlockedValue addrAccepted;
                SOCKETHANDLE hSocket = mConnection.waitForConnectionEvent(addrAccepted);
                if ( hSocket != NESocket::InvalidSocketHandle )
                {
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

                    const NESocket::InterlockedValue & addSocket = clientSocket.getAddress();
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
                        TRACE_DBG("Failed to receive message from client [ %s : %d ], socket. Going to close connection"
                                        , addSocket.getHostAddress().getString()
                                        , addSocket.getHostPort()
                                        , clientSocket.getHandle());

                        mRemoteService.failedReceiveMessage(clientSocket.getHandle());
                        mConnectHandler.connectionLost(clientSocket);
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
                Event * eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? pickEvent() : NULL;
                whichEvent = isExitEvent(eventElem) || (whichEvent == static_cast<int>(EVENT_EXIT)) ? static_cast<int>(EVENT_EXIT) : whichEvent;
            }

        } while (whichEvent == static_cast<int>(EVENT_QUEUE));
    }
    mHasStarted = false;
    removeEvents(false);

    mEventStarted.resetEvent();

    TRACE_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());
    return (whichEvent == static_cast<int>(EVENT_EXIT));
}
