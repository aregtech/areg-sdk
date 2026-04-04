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
#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ClientConnection.hpp"

namespace areg {

ClientSendThread::ClientSendThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String& namePrefix )
    : DispatcherThread        ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_SIZE_MAXIMUM )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0 )
    , mSaveDataSend     ( false )
{
}

void ClientSendThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        SendMessageEvent::add_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        SendMessageEvent::remove_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        mConnection.close_socket( );
    }
}

bool ClientSendThread::_do_send( const RemoteMessage & msg )
{
    ASSERT( msg.is_valid() );
    const int32_t sentBytes = mConnection.send_message(msg);
    if ( sentBytes > 0 )
    {
        if ( mSaveDataSend )
            mBytesSend += static_cast<uint32_t>(sentBytes);

        return true;
    }

    mRemoteService.failed_send_message( msg, mConnection.socket( ) );
    return false;
}

void ClientSendThread::process_event( const SendMessageEventData & data )
{
    if ( data.is_forward_message() )
    {
        if ( !_do_send( data.remote_message() ) )
            return;

        // Drain additional queued messages without returning to the dispatcher overhead.
        // Bounded by DRAIN_LIMIT so that a shutdown command is never delayed more than
        // DRAIN_LIMIT sends.
        constexpr int32_t DRAIN_LIMIT{ 32 };
        const ExitEvent & exitEvent = ExitEvent::exit_event();
        for ( int32_t count = 0; count < DRAIN_LIMIT; ++count )
        {
            Event * evt = pick_event();
            if ( evt == nullptr )
                break;

            // ExitEvent is a singleton — compare by pointer, never call destroy() on it.
            if ( static_cast<const Event *>( evt ) == static_cast<const Event *>( &exitEvent ) )
            {
                mConnection.close_socket();
                trigger_exit();
                return;
            }

            SendMessageEvent * sendEvt = AREG_RUNTIME_CAST( evt, SendMessageEvent );
            if ( sendEvt == nullptr )
            {
                // Unexpected event type — destroy it and let the normal dispatch loop continue.
                evt->destroy();
                break;
            }

            const SendMessageEventData & evtData = sendEvt->data();
            if ( evtData.is_exit_message() )
            {
                sendEvt->destroy();
                mConnection.close_socket();
                trigger_exit();
                return;
            }

            const bool ok = _do_send(evtData.remote_message());
            sendEvt->destroy();
            if ( !ok )
                return;
        }
    }
    else if ( data.is_exit_message() )
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
