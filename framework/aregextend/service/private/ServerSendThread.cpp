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

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);

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
        areg::set_receive_mode(areg::ReceiveMode::MonoCache);
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

#if 0  // old method
void ServerSendThread::process_event(const SendMessageEventData& data)
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

    if (!data.is_forward_message())
        return;

    const ExitEvent& exitEvent{ ExitEvent::exit_event() };
    const RemoteMessage& trigMsg{ data.remote_message() };

    ITEM_ID  targets[areg::THREAD_DRAIN_LIMIT];
    uint32_t batchCount{ 0u };

    targets[batchCount] = trigMsg.target();
    mBatch[batchCount++] = { areg::InvalidSocketHandle, &trigMsg, nullptr };

    // Phase 1: drain up to THREAD_DRAIN_LIMIT - 1 additional events (no lock)
    while (batchCount < areg::THREAD_DRAIN_LIMIT)
    {
        Event* evt = pick_event();
        if (evt == nullptr)
            break;

        if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping send thread");
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            mConnection.close_all_connections();
            mConnection.close_socket();
            trigger_exit();
            return;
        }

        ASSERT(AREG_RUNTIME_CAST(evt, SendMessageEvent) != nullptr);
        SendMessageEvent* sendEvt = static_cast<SendMessageEvent*>(evt);
        const SendMessageEventData& evtData = sendEvt->data();
        if (evtData.is_exit_message())
        {
            DEBUG_LOG_DBG("Going to quit send message thread");
            sendEvt->destroy();
            for (uint32_t i = 1u; i < batchCount; ++i)
                mBatch[i].sendEvt->destroy();

            mConnection.close_all_connections();
            mConnection.close_socket();
            trigger_exit();
            return;
        }

        const RemoteMessage& msg = evtData.remote_message();
        targets[batchCount] = msg.target();
        mBatch[batchCount++] = { areg::InvalidSocketHandle, &msg, sendEvt };
    }

    // Phase 2: resolve all cookies, handles in one shared_lock window ---
    SOCKETHANDLE sockets[areg::THREAD_DRAIN_LIMIT];
    mConnection.batch_handles_by_cookies(targets, sockets, batchCount);

    // Phase 3: fill socket handles, drop disconnected targets, compact in-place
    uint32_t validCount{ 0u };
    for (uint32_t i{ 0u }; i < batchCount; ++i)
    {
        if (!areg::is_valid_socket(sockets[i]))
        {
            DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ]"
                            , static_cast<uint32_t>(mBatch[i].msg->message_id())
                            , static_cast<uint32_t>(targets[i]));
            if (mBatch[i].sendEvt != nullptr)   // slot 0 sendEvt is always nullptr
                mBatch[i].sendEvt->destroy();

            continue;
        }

        if (i != validCount)
            mBatch[validCount] = mBatch[i];

        mBatch[validCount].socket = sockets[i];
        ++validCount;
    }

    if (validCount == 0u)
        return;

    // Phase 4: sort by socket and send by group
    areg::ext::sort_pending_sends(mBatch.data(), validCount);
    areg::ext::send_pending_groups(mBatch.data(), validCount, mConnection, mRemoteService,
        [this](uint64_t bytes, uint32_t msgs) { accumulate_sent(bytes, msgs); });

    for (uint32_t i = 0u; i < validCount; ++i)
    {
        if (mBatch[i].sendEvt != nullptr)
            mBatch[i].sendEvt->destroy();
    }
}

#else // new method

void ServerSendThread::process_event(const SendMessageEventData& data)
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

    if (!data.is_forward_message())
        return;

    const ExitEvent& exitEvent{ ExitEvent::exit_event() };
    const RemoteMessage& trigMsg{ data.remote_message() };

    ITEM_ID  targets[areg::THREAD_DRAIN_LIMIT];
    uint32_t batchCount{ 0u };

    targets[batchCount] = trigMsg.target();
    mBatch[batchCount++] = { areg::InvalidSocketHandle, &trigMsg, nullptr };

    // Phase 1: drain additional events -- no socket lookup, no lock
    Event* events[areg::THREAD_DRAIN_LIMIT - 1u];
    uint32_t evtCount = pop_events(events, areg::THREAD_DRAIN_LIMIT - 1u);
    if (evtCount != 0)
    {
        for (uint32_t e = 0; e < evtCount; ++e)
        {
            Event* evt = events[e];
            ASSERT(evt != nullptr);
            if (static_cast<const Event*>(evt) == static_cast<const Event*>(&exitEvent))
            {
                DEBUG_LOG_DBG("Received exit event during batch-drain, stopping send thread");
                // ExitEvent is a singleton -- skip it when destroying the batch.
                for (uint32_t i = 0u; i < evtCount; ++i)
                    events[i]->destroy();

                mConnection.close_all_connections();
                mConnection.close_socket();
                trigger_exit();
                return;
            }

            ASSERT(AREG_RUNTIME_CAST(evt, SendMessageEvent) != nullptr);
            SendMessageEvent* sendEvt = static_cast<SendMessageEvent*>(evt);
            const SendMessageEventData& evtData = sendEvt->data();
            if (evtData.is_exit_message())
            {
                DEBUG_LOG_DBG("Going to quit send message thread");
                for (uint32_t i = 0u; i < evtCount; ++i)
                    events[i]->destroy();

                mConnection.close_all_connections();
                mConnection.close_socket();
                trigger_exit();
                return;
            }

            const RemoteMessage& msg{ evtData.remote_message() };
            targets[batchCount] = msg.target();
            mBatch[batchCount++] = { areg::InvalidSocketHandle, &msg, sendEvt };
        }
    }

    // Phase 2: resolve all cookies in ONE lock window
    SOCKETHANDLE sockets[areg::THREAD_DRAIN_LIMIT];
    mConnection.batch_handles_by_cookies(targets, sockets, batchCount);

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
            continue;
        }

        areg::PendingSend entry{ sockets[i], mBatch[i].msg, mBatch[i].sendEvt };
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

        mBatch[lo] = std::move(entry);
        ++validCount;
    }

    if (validCount != 0u)
    {
        // Phase 4: batch is already sorted -- send groups directly
        areg::ext::send_pending_groups(mBatch.data(), validCount, mConnection, mRemoteService,
            [this](uint64_t bytes, uint32_t msgs) { accumulate_sent(bytes, msgs); });
    }

    for (uint32_t i = 0u; i < evtCount; ++i)
        events[i]->destroy();
}

#endif

bool ServerSendThread::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
