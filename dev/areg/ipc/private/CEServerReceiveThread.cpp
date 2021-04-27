/************************************************************************
 * \file        areg/ipc/private/CEServerReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Receive Message Thread of Server socket.
 ************************************************************************/
#include "areg/ipc/private/CEServerReceiveThread.hpp"

#include "areg/ipc/private/IEServerConnectionHandler.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"
#include "areg/ipc/private/CEServerConnection.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/base/CERemoteMessage.hpp"
#include "areg/base/CESocketAccepted.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_CEServerReceiveThread_RunDispatcher);

CEServerReceiveThread::CEServerReceiveThread( IEServerConnectionHandler & connectHandler, IERemoteServiceHandler & remoteService, CEServerConnection & connection )
    : CEDispatcherThread    ( NEConnection::SERVER_RECEIVE_MESSAGE_THREAD )
    , mRemoteService        ( remoteService )
    , mConnectHandler       ( connectHandler )
    , mConnection           ( connection )
{
    ; // do nothing
}

CEServerReceiveThread::~CEServerReceiveThread(void)
{
    ; // do nothing
}

bool CEServerReceiveThread::RunDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_CEServerReceiveThread_RunDispatcher);
    TRACE_DBG("Starting dispatcher [ %s ]", GetThreadName().GetBuffer());

    mEventStarted.SetEvent();

    int whichEvent  = static_cast<int>(EVENT_ERROR);
    if ( mConnection.ServerListen( NESocket::MAXIMUM_LISTEN_QUEUE_SIZE) )
    {
        IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
        CEMultiLock multiLock(syncObjects, 2, false);

        CERemoteMessage msgReceived;
        do 
        {
            whichEvent = multiLock.Lock(IESynchObject::DO_NOT_WAIT, false);
            if ( whichEvent == CEMultiLock::LOCK_INDEX_TIMEOUT )
            {
                whichEvent = static_cast<int>(EVENT_QUEUE); // escape quit
                NESocket::CEInterlockedValue addrAccepted;
                SOCKETHANDLE hSocket = mConnection.WaitForConnectionEvent(addrAccepted);
                if ( hSocket != NESocket::InvalidSocketHandle )
                {
                    CESocketAccepted clientSocket;
                    if (mConnection.IsConnectionAccepted(hSocket) )
                    {
                        clientSocket = mConnection.GetClientByHandle( hSocket );
                        TRACE_DBG("Received connection event of socket [ %u ], client [ %s : %d ]", hSocket, clientSocket.GetAddress().GetHostAddress().GetBuffer(), clientSocket.GetAddress().GetHostPort());
                    }
                    else
                    {
                        clientSocket = CESocketAccepted(hSocket, addrAccepted);
                        if ( mConnectHandler.CanAcceptConnection(clientSocket)  )
                        {
                            TRACE_DBG("Accepting new connection of socket [ %u ], client [ %s : %d ]", hSocket, addrAccepted.GetHostAddress().GetBuffer(), addrAccepted.GetHostPort());
                            mConnection.AcceptConnection(clientSocket);
                        }
                        else
                        {
                            TRACE_WARN("Rejecting new connection of socket [ %u ], client [ %s : %d ]", hSocket, addrAccepted.GetHostAddress().GetBuffer(), addrAccepted.GetHostPort());
                            mConnection.RejectConnection(clientSocket);
                            clientSocket.CloseSocket();
                            continue;
                        }
                    }
                    const NESocket::CEInterlockedValue & addSocket = clientSocket.GetAddress();
                    if ( mConnection.ReceiveMessage(msgReceived, clientSocket) > 0 )
                    {
                        TRACE_DBG("Received message [ %p ] from source [ %p ], client [ %s : %d ]"
                                    , msgReceived.GetMessageId()
                                    , msgReceived.GetSource()
                                    , addSocket.GetHostAddress().GetBuffer()
                                    , addSocket.GetHostPort());

                        mRemoteService.ProcessReceivedMessage(msgReceived, addSocket, clientSocket.GetHandle());
                    }
                    else
                    {
                        TRACE_DBG("Failed to receive message from client [ %s : %d ], socket. Going to close connection"
                                        , addSocket.GetHostAddress().GetBuffer()
                                        , addSocket.GetHostPort()
                                        , clientSocket.GetHandle());

                        mConnectHandler.ConnectionLost(clientSocket);
                    }

                    msgReceived.InvalidateBuffer();

                }
                else
                {
                    OUTPUT_ERR("Failed to accept socket. Make cleanups to clean master list on next loop or server socket is closed!");
                }
            }
            else
            {
                CEEvent * eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? PickEvent() : NULL;
                whichEvent = IsExitEvent(eventElem) || (whichEvent == static_cast<int>(EVENT_EXIT)) ? static_cast<int>(EVENT_EXIT) : whichEvent;
            }
        } while (whichEvent == static_cast<int>(EVENT_QUEUE));
    }
    mHasStarted = false;
    RemoveEvents(false);

    mEventStarted.ResetEvent();

    TRACE_DBG("Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.GetBuffer());
    return (whichEvent == static_cast<int>(EVENT_EXIT));
}
