/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientSendThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/component/NEService.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/ipc/private/NEConnection.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

ClientSendThread::ClientSendThread(IERemoteMessageHandler& remoteService, ClientConnection & connection, const String& namePrefix )
    : DispatcherThread  ( namePrefix + NEConnection::CLIENT_SEND_MESSAGE_THREAD )
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0 )
{
}

void ClientSendThread::readyForEvents( bool isReady )
{
    TRACE_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

    if ( isReady )
    {
        TRACE_DBG( "Starting client service dispatcher thread [ %s ]", getName( ).getString( ) );
        SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::readyForEvents( true );
    }
    else
    {
        DispatcherThread::readyForEvents( false );
        SendMessageEvent::removeListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        mConnection.closeSocket( );
        TRACE_DBG( "Exiting client service dispatcher thread [ %s ], stopping receiving events", getName( ).getString( ) );
    }
}

void ClientSendThread::processEvent( const SendMessageEventData & data )
{
    if ( data.isForwardMessage() )
    {
        const RemoteMessage & msg = data.getRemoteMessage( );
        int sizeSend = mConnection.sendMessage( msg );
        if ( sizeSend > 0 )
        {
            mBytesSend += static_cast<uint32_t>(sizeSend);
        }
        else
        {
            mRemoteService.failedSendMessage( msg, mConnection.getSocket( ) );
        }
    }
    else if (data.isExitThreadMessage() )
    {
        mConnection.closeSocket( );
        triggerExit( );
    }
}

bool ClientSendThread::postEvent(Event & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr ? EventDispatcher::postEvent(eventElem) : false );
}
