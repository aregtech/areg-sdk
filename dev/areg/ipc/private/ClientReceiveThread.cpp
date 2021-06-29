/************************************************************************
 * \file        areg/ipc/private/ClientReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Receive Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientReceiveThread.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/private/IERemoteServiceHandler.hpp"

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);

ClientReceiveThread::ClientReceiveThread( IERemoteServiceHandler & remoteService, ClientConnection & connection )
    : DispatcherThread( NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
    ; // do nothing
}

ClientReceiveThread::~ClientReceiveThread( void )
{
    ; // do nothing
}

bool ClientReceiveThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", getName().getString());
    mEventStarted.setEvent();

    IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);

    RemoteMessage msgReceived;
    int whichEvent  = static_cast<int>(EVENT_ERROR);
    do 
    {
        whichEvent = multiLock.lock(IESynchObject::DO_NOT_WAIT, false);
        if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            whichEvent = static_cast<int>(EVENT_QUEUE); // escape quit
            if ( mConnection.receiveMessage(msgReceived) <= 0 )
            {
                msgReceived.invalidate();
                mRemoteService.failedReceiveMessage( mConnection.getSocketHandle() );
                pulseExit();
            }

            mRemoteService.processReceivedMessage(msgReceived, mConnection.getAddress(), mConnection.getSocketHandle());
            msgReceived.invalidate();
        }
        else
        {
            Event * eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? pickEvent() : NULL;
            whichEvent = isExitEvent(eventElem) || (whichEvent == static_cast<int>(EVENT_EXIT)) ? static_cast<int>(EVENT_EXIT) : whichEvent;
        }

    } while (whichEvent == static_cast<int>(EVENT_QUEUE));

    mHasStarted = false;
    removeAllEvents( );

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());

    mEventStarted.resetEvent();

    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", getName().getString(), whichEvent == static_cast<int>(EVENT_EXIT) ? "SUCCESS" : "FAILURE");
    return (whichEvent == static_cast<int>(EVENT_EXIT));
}
