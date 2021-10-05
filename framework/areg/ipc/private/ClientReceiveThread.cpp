/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientReceiveThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Receive Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientReceiveThread.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/IERemoteServiceHandler.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);

ClientReceiveThread::ClientReceiveThread( IERemoteServiceHandler & remoteService, ClientConnection & connection )
    : DispatcherThread  ( NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD.data() )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
}

ClientReceiveThread::~ClientReceiveThread( void )
{
}

bool ClientReceiveThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", getName().getString());
    mEventStarted.setEvent();

    IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);

    RemoteMessage msgReceived;
    int whichEvent  = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    do 
    {
        whichEvent = multiLock.lock(NECommon::DO_NOT_WAIT, false);
        if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue); // escape quit
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
            Event * eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
            whichEvent = isExitEvent(eventElem) ? static_cast<int>(EventDispatcherBase::eEventOrder::EventExit) : whichEvent;
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue));

    mHasStarted = false;
    removeAllEvents( );

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());

    mEventStarted.resetEvent();

    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", getName().getString()
               , whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit) ? "SUCCESS" : "FAILURE");

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}
