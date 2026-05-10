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
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include <array>

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, _do_send);

ServerSendThread::ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread        ( areg::SERVER_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBatch            ( )
    , mSendStats        ( )
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
    const SOCKETHANDLE hSocket{ mConnection.client_handle_by_cookie(target) };

    // Target cookie has no connected socket -- the client disconnected while this message
    // was still queued.  Discard silently and return true so the drain loop continues
    // processing the remaining batch instead of stopping after the first stale entry.
    if ( hSocket == areg::InvalidSocketHandle )
    {
        DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ], no socket found"
                        , static_cast<uint32_t>(msg.message_id())
                        , static_cast<uint32_t>(target));
        return true;
    }

    DEBUG_LOG_DBG("Sending message [ %s ] (ID = [ %u ]) to socket [ %u ], source [ %u ] to target [ %u ], data length [ %u ]"
                    , areg::as_string(static_cast<areg::FuncIdRange>(msg.message_id()))
                    , static_cast<uint32_t>(msg.message_id())
                    , static_cast<uint32_t>(hSocket)
                    , static_cast<uint32_t>(msg.source())
                    , static_cast<uint32_t>(msg.target())
                    , static_cast<uint32_t>(msg.size_used()));

    const int32_t sentBytes = mConnection.send_message(msg, hSocket);
    if ( sentBytes > 0 )
    {
        accumulate_sent(static_cast<uint32_t>(sentBytes), 1u);
        return true;
    }
    else
    {
        DEBUG_LOG_WARN( "Failed to send message [ %u ] to target [ %u ], socket [ %u ], result [ %d ]"
                       , msg.message_id()
                       , static_cast<uint32_t>(msg.target())
                       , static_cast<uint32_t>(hSocket)
                       , sentBytes);

        SocketAccepted client{ mConnection.client_by_handle(hSocket) };
        if (!mConnection.is_interrupted())
        {
            mRemoteService.failed_send_message(msg, client);
        }

        return false;
    }
}

void ServerSendThread::process_event( const SendMessageEventData & data )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);
    if (data.is_exit_message())
    {
        DEBUG_LOG_DBG("Going to quit send message thread");
        mConnection.close_all_connections();
        mConnection.close_socket();
        trigger_exit();
        return;
    }

    if (!data.is_forward_message() || !_do_send(data.remote_message()))
        return;
    
    if (mExternalEvents.is_empty())
        return;

    // Batch drain: collect up to areg::THREAD_DRAIN_LIMIT additional events and send each individually.
    // Draining in bulk reduces the number of times the dispatcher re-enters the kernel wait path,
    // which is the dominant overhead under burst load.
    const ExitEvent & exitEvent = ExitEvent::exit_event();

    uint32_t batchCount{ 0 };
    bool    exitRequested{ false };

    // --- Phase 1: drain events and resolve sockets ---
    for ( uint32_t count = 0; count < areg::THREAD_DRAIN_LIMIT; ++count )
    {
        Event * evt = pick_event();
        if ( evt == nullptr )
            break;

        // ExitEvent is a singleton -- compare by pointer, never call destroy() on it.
        if ( static_cast<const Event *>( evt ) == static_cast<const Event *>( &exitEvent ) )
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping send thread");
            exitRequested = true;
            break;
        }

        SendMessageEvent * sendEvt = AREG_RUNTIME_CAST( evt, SendMessageEvent );
        if ( sendEvt == nullptr )
        {
            evt->destroy();
            continue;
        }

        const SendMessageEventData & evtData = sendEvt->data();
        if ( evtData.is_exit_message() )
        {
            DEBUG_LOG_DBG("Going to quit send message thread");
            sendEvt->destroy();
            exitRequested = true;
            break;
        }

        const RemoteMessage & msg = evtData.remote_message();
        const SOCKETHANDLE hSocket{ mConnection.client_handle_by_cookie( msg.target() ) };
        if ( hSocket == areg::InvalidSocketHandle )
        {
            DEBUG_LOG_WARN("Discarding queued message (ID = [ %u ]) for disconnected target [ %u ]", msg.message_id(), static_cast<uint32_t>(msg.target()));
            sendEvt->destroy();
            continue;
        }

        mBatch[batchCount++] = { hSocket, sendEvt };
    }

    if (exitRequested)
    {
        mConnection.close_all_connections();
        mConnection.close_socket();
        trigger_exit();
        for (uint32_t i =0 ; i < batchCount; ++i)
        {
            if (mBatch[i].sendEvt != nullptr)
                mBatch[i].sendEvt->destroy();
        }

        return;
    }

    // --- Phase 2+3: sort by socket and send each same-socket group ---
    areg::ext::sort_pending_sends(mBatch.data(), batchCount);
    areg::ext::send_pending_groups(mBatch.data(), batchCount, mConnection, mRemoteService,
        [this](uint64_t bytes, uint32_t msgs) { accumulate_sent(bytes, msgs); });

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (batchCount >= areg::THREAD_DRAIN_LIMIT)
    {
        DEBUG_LOG_WARN("Send drain loop exhausted THREAD_DRAIN_LIMIT (%d) � outbound queue is building up", areg::THREAD_DRAIN_LIMIT);
    }
#endif   // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
}

bool ServerSendThread::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
