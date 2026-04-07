/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server send message thread
 ************************************************************************/
#include "aregextend/service/private/ServerSendThread.hpp"

#include "areg/component/ServiceDefs.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/service/ServerConnection.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, _do_send);

ServerSendThread::ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread        ( areg::SERVER_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_NORMAL, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0 )
    , mSaveDataSend     ( false )
{
}

void ServerSendThread::ready_for_events( bool is_ready )
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
        mConnection.close_all_connections( );
        mConnection.disable_send( );
    }
}

bool ServerSendThread::_do_send( const RemoteMessage & msg )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServerSendThread, _do_send);
    ASSERT( msg.is_valid() );

    const ITEM_ID & target{ msg.target() };
    SocketAccepted client{ mConnection.client_by_cookie(target) };

    // Target cookie has no connected socket — the client disconnected while this message
    // was still queued.  Discard silently and return true so the drain loop continues
    // processing the remaining batch instead of stopping after the first stale entry.
    if ( !client.is_valid() )
    {
        DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ], no socket found"
                        , static_cast<uint32_t>(msg.message_id())
                        , static_cast<uint32_t>(target));
        return true;
    }

    DEBUG_LOG_DBG("Sending message [ %s ] (ID = [ %u ]) to client [ %s : %d ] of socket [ %u ], source [ %u ] to target [ %u ], data length [ %u ]"
                    , areg::as_string(static_cast<areg::FuncIdRange>(msg.message_id()))
                    , static_cast<uint32_t>(msg.message_id())
                    , client.address().host_address().as_string()
                    , client.address().host_port()
                    , static_cast<uint32_t>(client.handle())
                    , static_cast<uint32_t>(msg.source())
                    , static_cast<uint32_t>(msg.target())
                    , static_cast<uint32_t>(msg.size_used()));

    const int32_t sentBytes = mConnection.send_message(msg, client);
    if ( sentBytes > 0 )
    {
        if ( mSaveDataSend )
            mBytesSend += static_cast<uint32_t>(sentBytes);

        return true;
    }

    DEBUG_LOG_WARN("Failed to send message [ %u ] to target [ %u ], socket [ %u ], result [ %d ], socket valid [ %s ]"
                    , msg.message_id()
                    , static_cast<uint32_t>(msg.target())
                    , static_cast<uint32_t>(client.handle())
                    , sentBytes
                    , client.is_valid() ? "VALID" : "INVALID");

    mRemoteService.failed_send_message(msg, client);
    return false;
}

void ServerSendThread::process_event( const SendMessageEventData & data )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);
    if ( data.is_forward_message() )
    {
        if ( !_do_send( data.remote_message() ) )
            return;

        // Drain additional queued messages without returning to the dispatcher overhead.
        constexpr int32_t DRAIN_LIMIT{ 32 };
        const ExitEvent & exitEvent = ExitEvent::exit_event();
        int32_t drainCount{ 0 };
        for ( int32_t count = 0; count < DRAIN_LIMIT; ++count )
        {
            Event * evt = pick_event();
            if ( evt == nullptr )
                break;

            ++drainCount;

            // ExitEvent is a singleton — compare by pointer, never call destroy() on it.
            if ( static_cast<const Event *>( evt ) == static_cast<const Event *>( &exitEvent ) )
            {
                DEBUG_LOG_DBG("Received exit event during batch-drain, stopping send thread");
                mConnection.close_all_connections();
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
                DEBUG_LOG_DBG("Going to quit send message thread");
                sendEvt->destroy();
                mConnection.close_all_connections();
                mConnection.close_socket();
                trigger_exit();
                return;
            }

            const bool ok = _do_send(evtData.remote_message());
            sendEvt->destroy();
            if ( !ok )
                return;
        }

        // If the drain loop saturated the limit, the send queue is building up faster
        // than events are consumed.  A persistent warning here is a strong signal that
        // the outbound queue is growing unboundedly.
        if (drainCount >= DRAIN_LIMIT)
        {
            DEBUG_LOG_WARN("Send drain loop exhausted DRAIN_LIMIT (%d) — outbound queue is building up", DRAIN_LIMIT);
        }
    }
    else if ( data.is_exit_message() )
    {
        DEBUG_LOG_DBG("Going to quit send message thread");
        mConnection.close_all_connections( );
        mConnection.close_socket( );
        trigger_exit( );
    }
}

bool ServerSendThread::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
