/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ClientSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Send Message Thread
 ************************************************************************/
#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

ClientSendThread::ClientSendThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String& namePrefix )
    : DispatcherThread  ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0 )
    , mSaveDataSend     ( false )
{
}

void ClientSendThread::ready_for_events( bool is_ready )
{
    LOG_SCOPE(areg_ipc_private_ClientSendThread_readyForEvents);

    if ( is_ready )
    {
        LOG_DBG( "Starting client service dispatcher thread [ %s ]", name( ).as_string( ) );
        SendMessageEvent::add_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        SendMessageEvent::remove_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        mConnection.close_socket( );
        LOG_DBG( "Exiting client service dispatcher thread [ %s ], stopping receiving events", name( ).as_string( ) );
    }
}

void ClientSendThread::process_event( const SendMessageEventData & data )
{
    if ( data.is_forward_message() )
    {
        const RemoteMessage & msg = data.remote_message( );
        int32_t sizeSend = mConnection.send_message( msg );
        if ( sizeSend > 0 )
        {
            if (mSaveDataSend)
            {
                mBytesSend += static_cast<uint32_t>(sizeSend);
            }
        }
        else
        {
            mRemoteService.failed_send_message( msg, mConnection.socket( ) );
        }
    }
    else if (data.is_exit_message() )
    {
        mConnection.close_socket( );
        trigger_exit( );
    }
}

bool ClientSendThread::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg
