/************************************************************************
 * \file        areg/src/ipc/private/CEClientReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Receive Message Thread
 ************************************************************************/
#include "areg/src/ipc/private/CEClientReceiveThread.hpp"

#include "areg/src/ipc/private/NEConnection.hpp"
#include "areg/src/ipc/private/CEClientConnection.hpp"
#include "areg/src/ipc/private/IERemoteServiceHandler.hpp"

#include "areg/src/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_CEClientReceiveThread_RunDispatcher);

CEClientReceiveThread::CEClientReceiveThread( IERemoteServiceHandler & remoteService, CEClientConnection & connection )
    : CEDispatcherThread( NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
    ; // do nothing
}

CEClientReceiveThread::~CEClientReceiveThread( void )
{
    ; // do nothing
}

bool CEClientReceiveThread::RunDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_CEClientReceiveThread_RunDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", GetThreadName().GetBuffer());
    mEventStarted.SetEvent();

    IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
    CEMultiLock multiLock(syncObjects, 2, false);

    CERemoteMessage msgReceived;
    int whichEvent  = static_cast<int>(EVENT_ERROR);
    do 
    {
        whichEvent = multiLock.Lock(IESynchObject::DO_NOT_WAIT, false);
        if ( whichEvent == CEMultiLock::LOCK_INDEX_TIMEOUT )
        {
            whichEvent = static_cast<int>(EVENT_QUEUE); // escape quit
            if ( mConnection.ReceiveMessage(msgReceived) < 0 )
            {
                msgReceived.InvalidateBuffer();
                mRemoteService.FailedReceiveMessage( mConnection.GetSocketHandle() );
                PulseExit();
            }

            mRemoteService.ProcessReceivedMessage(msgReceived, mConnection.GetAddress(), mConnection.GetSocketHandle());
            msgReceived.InvalidateBuffer();
        }
        else
        {
            CEEvent * eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? PickEvent() : NULL;
            whichEvent = IsExitEvent(eventElem) || (whichEvent == static_cast<int>(EVENT_EXIT)) ? static_cast<int>(EVENT_EXIT) : whichEvent;
        }

    } while (whichEvent == static_cast<int>(EVENT_QUEUE));

    mHasStarted = false;
    RemoveAllEvents( );

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.GetBuffer());

    mEventStarted.ResetEvent();

    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", GetThreadName().GetBuffer(), whichEvent == static_cast<int>(EVENT_EXIT) ? "SUCCESS" : "FAILURE");
    return (whichEvent == static_cast<int>(EVENT_EXIT));
}
