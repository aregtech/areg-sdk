/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientReceiveThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Receive Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientReceiveThread.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/ipc/private/NEConnection.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);

ClientReceiveThread::ClientReceiveThread(IERemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix)
    : DispatcherThread  (namePrefix + NEConnection::CLIENT_RECEIVE_MESSAGE_THREAD )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesReceive     ( 0 )
{
}

bool ClientReceiveThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientReceiveThread_runDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", getName().getString());
    
    readyForEvents( true );

    IESynchObject* syncObjects[2] {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);
    RemoteMessage msgReceived;
    int whichEvent{ static_cast<int>(EventDispatcherBase::eEventOrder::EventError) };

    do
    {
        whichEvent = multiLock.lock(NECommon::DO_NOT_WAIT, false);
        if ( whichEvent == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue); // escape quit
            int sizeReceive = mConnection.receiveMessage( msgReceived );
            if ( sizeReceive <= 0 )
            {
                msgReceived.invalidate();
                mRemoteService.failedReceiveMessage( mConnection.getSocket() );
                whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
            }
            else
            {
                mBytesReceive += static_cast<uint32_t>(sizeReceive);
                mRemoteService.processReceivedMessage( msgReceived, mConnection.getSocket( ) );
            }

            msgReceived.invalidate();
        }
        else
        {
            Event * eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
            whichEvent = isExitEvent(eventElem) ? static_cast<int>(EventDispatcherBase::eEventOrder::EventExit) : whichEvent;
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue));

    readyForEvents(false);
    removeAllEvents( );

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());
    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]"
                , getName().getString()
                , whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit) ? "SUCCESS" : "FAILURE");

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}
