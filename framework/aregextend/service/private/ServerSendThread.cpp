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

#include <array>
#include <cstring>

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, _do_send);

ServerSendThread::ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread        ( areg::SERVER_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_NORMAL, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBatch            ( )
    , mBytesSend        ( 0u )
    , mMsgsSend         ( 0u )
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

    // --- Phase 2: binary insertion sort by socket handle to enable grouping ---
    // PendingSend is trivially copyable (SOCKETHANDLE + 2 pointers = 24 bytes on 64-bit).
    // Fast path (1:1 / same-socket fan-out): the >= guard fires every iteration — O(n)
    //   comparisons, zero memmove calls.
    // Slow path (m:n / mixed targets): binary search finds the insertion point in O(log n),
    //   then memmove shifts the run in one SIMD-optimized call instead of n scalar copies.
    //   For n=32 worst case: max shift = 31 × 24 = 744 bytes; AVX-256 handles that in ~3
    //   iterations vs ~93 scalar register-move ops in a plain shift loop.
    for ( uint32_t i{ 1 }; i < batchCount; ++i )
    {
        if ( mBatch[i].socket >= mBatch[i - 1].socket )
            continue;   // already in order — fast path (1:1 / same-socket groups)

        const PendingSend key{ mBatch[i] };
        // Binary search for the correct insertion index in [0, i).
        uint32_t lo{ 0 }, hi{ i };
        while ( lo < hi )
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            (mBatch[mid].socket <= key.socket) ? lo = mid + 1 : hi = mid;
        }
        // Shift [lo .. i-1] one position right, then place key at lo.
        memmove( &mBatch[lo + 1], &mBatch[lo], static_cast<size_t>(i - lo) * sizeof(PendingSend) );
        mBatch[lo] = key;
    }

    // --- Phase 3: send each same-socket group with one syscall ---
    for ( uint32_t i{ 0 }; i < batchCount; )
    {
        const SOCKETHANDLE hSocket{ mBatch[i].socket };
        uint32_t j{ i + 1 };
        while ( (j < batchCount) && (mBatch[j].socket == hSocket) )
            ++j;

        const uint32_t groupSize{ j - i };
        if ( groupSize == 1 )
        {
            const RemoteMessage& message{ mBatch[i].sendEvt->data().remote_message() };
            const int32_t sent{ mConnection.send_message(message, hSocket) };
            if ( sent > 0 )
            {
                accumulate_sent(static_cast<uint32_t>(sent), 1u);
            }
            else
            {
                DEBUG_LOG_WARN("Failed to send message to target [ %u ]", static_cast<uint32_t>(message.target()));
                if (!mConnection.is_interrupted())
                {
                    SocketAccepted client{ mConnection.client_by_handle(hSocket) };
                    mRemoteService.failed_send_message(message, client);
                }
            }
        }
        else
        {
            // Multiple messages for the same socket: scatter/gather send.
            const RemoteMessage* msgPtrs[areg::THREAD_DRAIN_LIMIT];
            for ( uint32_t k{ 0 }; k < groupSize; ++k )
                msgPtrs[k] = &(mBatch[i + k].sendEvt->data().remote_message());

            const int32_t sent{ mConnection.send_messages_batch(msgPtrs, groupSize, hSocket) };
            if ( sent > 0 )
            {
                accumulate_sent(static_cast<uint32_t>(sent), groupSize);
            }
            else
            {
                DEBUG_LOG_WARN("Failed batch-send of %u messages to target [ %u ]", groupSize, static_cast<uint32_t>(mBatch[i].sendEvt->data().message_target()));
                if (!mConnection.is_interrupted())
                {
                    SocketAccepted client{ mConnection.client_by_handle(hSocket) };
                    mRemoteService.failed_send_message(mBatch[i].sendEvt->data().remote_message(), client);
                }
            }
        }

        // Destroy all events in this group regardless of send outcome.
        for ( uint32_t k{ i }; k < j; ++k )
            mBatch[k].sendEvt->destroy();

        i = j;
    }

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
