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
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerSendThread.hpp"
#include "aregextend/service/private/ClientConnectionPair.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, process_event);

PoolSendThread::PoolSendThread( ClientConnectionPair & owner
                                  , areg::RemoteMessageHandler & remoteService
                                  , ServerConnection & connection
                                  , ServerSendThread & globalStats
                                  , std::string_view threadName )
    : DispatcherThread          ( String(threadName), areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
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
        areg::set_receive_mode(areg::ReceiveMode::MonoCache);
        SendMessageEvent::add_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        SendMessageEvent::remove_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
    }
}

#if 0
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

    // Include the triggering message as slot 0 of the batch.  The trigger is owned
    // by the dispatch chain -- sendEvt is nullptr and must never be destroyed here.
    // Drained events (slots 1..N) are owned by us and are destroyed after send.
    const ExitEvent& exitEvent = ExitEvent::exit_event();
    const RemoteMessage& trigMsg{ data.remote_message() };
    const SOCKETHANDLE trigSocket{ mOwner.socket_by_cookie(trigMsg.target()) };

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (!areg::is_valid_socket(trigSocket))
    {
        DEBUG_LOG_WARN("Trigger message (ID = [ %u ]) for disconnected target [ %u ] will be discarded"
                        , static_cast<uint32_t>(trigMsg.message_id())
                        , static_cast<uint32_t>(trigMsg.target()));
    }
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)

    uint32_t batchCount{ 0u };
    mBatch[batchCount++] = { trigSocket, &trigMsg, nullptr };

    // Drain up to THREAD_DRAIN_LIMIT - 1 additional events.
    for (uint32_t count = 1u; count < areg::THREAD_DRAIN_LIMIT; ++count)
    {
        Event* evt = pick_event();
        if (evt == nullptr)
            break;

        // ExitEvent is a singleton -- compare by pointer, never call destroy() on it.
        if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping pool send thread");
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            trigger_exit();
            return;
        }

        SendMessageEvent* sendEvt = AREG_RUNTIME_CAST(evt, SendMessageEvent);
        if (sendEvt == nullptr)
        {
            evt->destroy();
            continue;
        }

        const SendMessageEventData& evtData = sendEvt->data();
        if (evtData.is_exit_message())
        {
            DEBUG_LOG_DBG("Going to quit pool send message thread");
            sendEvt->destroy();
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            trigger_exit();
            return;
        }

        const RemoteMessage& msg = evtData.remote_message();
        const SOCKETHANDLE hSocket{ mOwner.socket_by_cookie(msg.target()) };
        if (hSocket == areg::InvalidSocketHandle)
        {
            DEBUG_LOG_WARN("Discarding queued message (ID = [ %u ]) for disconnected target [ %u ]"
                            , static_cast<uint32_t>(msg.message_id())
                            , static_cast<uint32_t>(msg.target()));
            sendEvt->destroy();
            continue;
        }

        mBatch[batchCount++] = { hSocket, &msg, sendEvt };
    }

    // Sort by socket and send each same-socket group in one syscall.
    areg::ext::sort_pending_sends(mBatch.data(), batchCount);
    areg::ext::send_pending_groups(mBatch.data(), batchCount, mConnection, mRemoteService,
        [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_sent(bytes, msgs); });

    for (uint32_t i = 0u; i < batchCount; ++i)
    {
        if (mBatch[i].sendEvt != nullptr)
            mBatch[i].sendEvt->destroy();
    }

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (batchCount >= areg::THREAD_DRAIN_LIMIT)
    {
        DEBUG_LOG_WARN("Send drain loop exhausted THREAD_DRAIN_LIMIT (%d) -- outbound queue is building up", areg::THREAD_DRAIN_LIMIT);
    }
#endif  // defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
}

#else

void PoolSendThread::process_event(const SendMessageEventData& data)
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolSendThread, process_event);

    if (data.is_exit_message())
    {
        DEBUG_LOG_DBG("Pool send thread: received exit message, shutting down");
        trigger_exit();
        return;
    }

    if (!data.is_forward_message())
        return;

    const ExitEvent& exitEvent{ ExitEvent::exit_event() };
    const RemoteMessage& trigMsg{ data.remote_message() };

    ITEM_ID  targets[areg::THREAD_DRAIN_LIMIT];
    uint32_t batchCount{ 0u };

    targets[batchCount]  = trigMsg.target();
    mBatch[batchCount++] = { areg::InvalidSocketHandle, &trigMsg, nullptr };

    // Phase 1: drain additional events -- no socket lookup, no lock
    while (batchCount < areg::THREAD_DRAIN_LIMIT)
    {
        Event* evt = pick_event();
        if (evt == nullptr)
            break;

        if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping pool send thread");
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            trigger_exit();
            return;
        }

        ASSERT(AREG_RUNTIME_CAST(evt, SendMessageEvent) != nullptr);
        SendMessageEvent* sendEvt = static_cast<SendMessageEvent*>(evt);
        const SendMessageEventData& evtData = sendEvt->data();
        if (evtData.is_exit_message())
        {
            DEBUG_LOG_DBG("Going to quit pool send message thread");
            sendEvt->destroy();
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            trigger_exit();
            return;
        }

        const RemoteMessage& msg{ evtData.remote_message() };
        targets[batchCount] = msg.target();
        mBatch[batchCount++] = { areg::InvalidSocketHandle, &msg, sendEvt };
    }

    // Phase 2: resolve all cookies in ONE lock window
    SOCKETHANDLE sockets[areg::THREAD_DRAIN_LIMIT];
    mOwner.batch_sockets_by_cookies(targets, sockets, batchCount);

    // Phase 3: compact + insertion-sort by socket handle in one pass.
    // On exit: mBatch[0..validCount) is sorted ascending by socket handle.
    uint32_t validCount{ 0u };
    for (uint32_t i{ 0u }; i < batchCount; ++i)
    {
        if (!areg::is_valid_socket(sockets[i]))
        {
            DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ]"
                            , static_cast<uint32_t>(mBatch[i].msg->message_id())
                            , static_cast<uint32_t>(targets[i]));
            if (mBatch[i].sendEvt != nullptr)
                mBatch[i].sendEvt->destroy();
            continue;
        }

        const areg::PendingSend entry{ sockets[i], mBatch[i].msg, mBatch[i].sendEvt };
        uint32_t lo{ 0u }, hi{ validCount };
        while (lo < hi)
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            mBatch[mid].socket <= entry.socket ? lo = mid + 1u : hi = mid;
        }

        if (lo < validCount)
        {
            ::memmove(&mBatch[lo + 1], &mBatch[lo], (validCount - lo) * sizeof(areg::PendingSend));
        }

        mBatch[lo] = entry;
        ++validCount;
    }

    if (validCount == 0u)
        return;

    // Phase 4: batch is already sorted -- send groups directly
    areg::ext::send_pending_groups(mBatch.data(), validCount, mConnection, mRemoteService,
        [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_sent(bytes, msgs); });

    for (uint32_t i = 0u; i < validCount; ++i)
    {
        if (mBatch[i].sendEvt != nullptr)
            mBatch[i].sendEvt->destroy();
    }
}

#endif

bool PoolSendThread::post_event( Event & eventElem )
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
