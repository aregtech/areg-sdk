/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/PoolSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send thread implementation.
 ************************************************************************/
#include "aregextend/service/private/PoolSendThread.hpp"

#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerSendThread.hpp"
#include "aregextend/service/private/ClientConnectionPair.hpp"

#include <cstring>

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, process_event);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, _do_send);

PoolSendThread::PoolSendThread( ClientConnectionPair & owner
                                  , areg::RemoteMessageHandler & remoteService
                                  , ServerConnection & connection
                                  , ServerSendThread & globalStats
                                  , std::string_view threadName )
    : DispatcherThread          ( String(threadName), areg::SYSTEM_THREAD_STACK_NORMAL, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer  ( )

    , mOwner            (owner)
    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mBatch            ( )
{
}

void PoolSendThread::ready_for_events( bool is_ready )
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
    }
}

bool PoolSendThread::_do_send( const areg::RemoteMessage & msg )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolSendThread, _do_send);
    ASSERT( msg.is_valid() );

    const ITEM_ID target{ msg.target() };
    const SOCKETHANDLE hSocket{ mOwner.socket_by_cookie(target) };
    if ( hSocket == areg::InvalidSocketHandle )
    {
        // Target is no longer connected — silently drop the message.
        DEBUG_LOG_DBG("Pool send thread: target cookie [ %u ] not found, dropping message [ %u ]"
                        , static_cast<uint32_t>(target)
                        , static_cast<uint32_t>(msg.message_id()));
        return true;
    }

    const int32_t sentBytes = mConnection.send_message(msg, hSocket);
    if ( sentBytes > 0 )
    {
        mGlobalStats.accumulate_sent(static_cast<uint64_t>(sentBytes), 1u);
        return true;
    }

    DEBUG_LOG_WARN("Pool send thread: failed to send message [ %u ] to target [ %u ], socket [ %u ], result [ %d ]"
                    , static_cast<uint32_t>(msg.message_id())
                    , static_cast<uint32_t>(target)
                    , static_cast<uint32_t>(hSocket)
                    , sentBytes);

    areg::SocketAccepted clientSocket{ mOwner.client_by_cookie(target) };
    if (!mConnection.is_interrupted())
    {
        mRemoteService.failed_send_message(msg, clientSocket);
    }

    return false;
}

void PoolSendThread::process_event( const SendMessageEventData & data )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolSendThread, process_event);

    if ( data.is_exit_message() )
    {
        DEBUG_LOG_DBG("Pool send thread: received exit message, shutting down");
        trigger_exit();
        return;
    }

    ASSERT(data.is_forward_message());
    if ( !_do_send( data.remote_message() ) )
        return;

    // Batch drain: collect up to areg::THREAD_DRAIN_LIMIT additional events and send each individually.
    // Draining in bulk reduces the number of times the dispatcher re-enters the kernel wait path,
    // which is the dominant overhead under burst load.
    const ExitEvent& exitEvent = ExitEvent::exit_event();

    uint32_t batchCount{ 0 };
    bool    exitRequested{ false };

    // --- Phase 1: drain events and resolve socket handles ---
    for (uint32_t count = 0; count < areg::THREAD_DRAIN_LIMIT; ++count)
    {
        Event* evt = pick_event();
        if (evt == nullptr)
            break;

        // ExitEvent is a singleton -- compare by pointer, never call destroy() on it.
        if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping send thread");
            exitRequested = true;
            break;
        }

        SendMessageEvent* sendEvt = AREG_RUNTIME_CAST(evt, SendMessageEvent);
        if (sendEvt == nullptr)
        {
            evt->destroy();
            continue;   // skip unknown event types; do not stop draining
        }

        const SendMessageEventData& evtData = sendEvt->data();
        if (evtData.is_exit_message())
        {
            DEBUG_LOG_DBG("Going to quit send message thread");
            sendEvt->destroy();
            exitRequested = true;
            break;
        }

        const RemoteMessage& msg = evtData.remote_message();
        const SOCKETHANDLE hSocket{ mOwner.socket_by_cookie(msg.target()) };
        if (hSocket == areg::InvalidSocketHandle)
        {
            DEBUG_LOG_WARN("Discarding queued message (ID = [ %u ]) for disconnected target [ %u ]", static_cast<uint32_t>(msg.message_id()), static_cast<uint32_t>(msg.target()));
            sendEvt->destroy();
            continue;
        }

        mBatch[batchCount++] = { hSocket, sendEvt };
    }

    // --- Phase 2: binary insertion sort by socket handle to enable grouping ---
    // PendingSend is trivially copyable (SOCKETHANDLE + 2 pointers = 24 bytes on 64-bit).
    // Fast path (1:1 / same-socket): the >= guard fires every iteration — O(n) comparisons,
    //   zero memmove calls.
    // Slow path (m:n / mixed targets): binary search finds the insertion point in O(log n),
    //   then memmove shifts the run in one SIMD-optimized call.
    for (uint32_t i{1}; i < batchCount; ++i)
    {
        if (mBatch[i].socket >= mBatch[i-1].socket)
            continue;   // already in order — fast path

        const PendingSend key{ mBatch[i] };
        // Binary search for insertion index in [0, i).
        uint32_t lo{0}, hi{i};
        while (lo < hi)
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            (mBatch[mid].socket <= key.socket) ? lo = mid + 1 : hi = mid;
        }
        // Shift [lo .. i-1] one position right, then place key at lo.
        memmove( &mBatch[lo + 1], &mBatch[lo], static_cast<size_t>(i - lo) * sizeof(PendingSend) );
        mBatch[lo] = key;
    }

    // --- Phase 3: send each same-socket group with one syscall ---
    uint32_t i{ 0 };
    while (i < batchCount)
    {
        const SOCKETHANDLE hSocket{ mBatch[i].socket };
        uint32_t j{ i + 1 };
        while ((j < batchCount) && (mBatch[j].socket == hSocket))
            ++j;

        const uint32_t groupSize{ j - i };

        if (groupSize == 1)
        {
            // Single message: existing path (avoids the batch overhead for the common case).
            const int32_t sent = mConnection.send_message(mBatch[i].sendEvt->data().remote_message(), hSocket);
            if (sent > 0)
            {
                mGlobalStats.accumulate_sent(static_cast<uint64_t>(sent), 1u);
            }
            else
            {
                DEBUG_LOG_WARN("Failed to send message to target [ %u ]", static_cast<uint32_t>(mBatch[i].sendEvt->data().message_target()));
                if (!mConnection.is_interrupted())
                {
                    areg::SocketAccepted client{ mOwner.client_by_cookie(mBatch[i].sendEvt->data().message_target()) };
                    mRemoteService.failed_send_message(mBatch[i].sendEvt->data().remote_message(), client);
                }
            }
        }
        else
        {
            // Multiple messages for the same socket: scatter/gather send.
            const RemoteMessage* msgPtrs[areg::THREAD_DRAIN_LIMIT];
            for (uint32_t k{ 0 }; k < groupSize; ++k)
                msgPtrs[k] = &(mBatch[i + k].sendEvt->data().remote_message());

            const int32_t sent = mConnection.send_messages_batch(msgPtrs, static_cast<uint32_t>(groupSize), hSocket);
            if (sent > 0)
            {
                mGlobalStats.accumulate_sent(static_cast<uint64_t>(sent), static_cast<uint32_t>(groupSize));
            }
            else
            {
                DEBUG_LOG_WARN("Failed batch-send of %d messages to target [ %u ]", groupSize, static_cast<uint32_t>(mBatch[i].sendEvt->data().message_target()));
                if (!mConnection.is_interrupted())
                {
                    areg::SocketAccepted client{ mOwner.client_by_cookie(mBatch[i].sendEvt->data().message_target()) };
                    mRemoteService.failed_send_message(mBatch[i].sendEvt->data().remote_message(), client);
                }
            }
        }

        // Destroy all events in this group regardless of send outcome.
        for (uint32_t k{ i }; k < j; ++k)
            mBatch[k].sendEvt->destroy();

        i = j;
    }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (batchCount >= areg::THREAD_DRAIN_LIMIT)
    {
        DEBUG_LOG_WARN("Send drain loop exhausted THREAD_DRAIN_LIMIT (%d) — outbound queue is building up", areg::THREAD_DRAIN_LIMIT);
    }
#endif   // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

    if (exitRequested)
    {
        // Pool send thread: do NOT close connections or the server socket here.
        // Global shutdown is managed by ServiceCommunicationBase::stop_connection().
        trigger_exit();
    }
}

bool PoolSendThread::post_event( Event & eventElem )
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
