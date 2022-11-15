/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/private/ClientConnection.hpp"
#include "areg/ipc/IERemoteServiceHandler.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ClientSendThread_runDispatcher);

ClientSendThread::ClientSendThread( IERemoteServiceHandler & remoteService, ClientConnection & connection )
    : DispatcherThread  ( NEConnection::CLIENT_SEND_MESSAGE_THREAD )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
{
}

bool ClientSendThread::runDispatcher(void)
{
    TRACE_SCOPE(areg_ipc_private_ClientSendThread_runDispatcher);
    TRACE_DBG("Starting client service dispatcher thread [ %s ]", getName().getString());

    SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));

    bool result = DispatcherThread::runDispatcher();

    SendMessageEvent::removeListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));
    TRACE_DBG("Exiting client service dispatcher thread [ %s ] with result [ %s ]", getName().getString(), result ? "SUCCESS" : "FAILURE");
    return result;
}

void ClientSendThread::processEvent( const SendMessageEventData & data )
{
    const RemoteMessage & msg = data.getRemoteMessage();
    if ( msg.isValid())
    {
        if ( mConnection.sendMessage( msg ) <= 0 )
        {
            mRemoteService.failedSendMessage( msg );
        }
    }
}

bool ClientSendThread::postEvent(Event & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr ? EventDispatcher::postEvent(eventElem) : false );
}
