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
        areg::set_receive_mode(areg::ReceiveMode::Cached);
        SendMessageEvent::add_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        SendMessageEvent::remove_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
    }
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
    const RemoteMessage& trigMsg{ data.remote_message() };
    const SOCKETHANDLE trigSocket{ mOwner.socket_by_cookie(trigMsg.target()) };

    // Batch drain: collect up to THREAD_DRAIN_LIMIT - 1 additional events.
    // Slot 0 of the IoBuffer is reserved for the triggering message; draining
    // THREAD_DRAIN_LIMIT - 1 events keeps the combined batch at THREAD_DRAIN_LIMIT.
    const ExitEvent& exitEvent = ExitEvent::exit_event();

    uint32_t batchCount{ 0u };
    bool exitRequested{ false };

    // --- Phase 1: drain events and resolve socket handles ---
    for (uint32_t count = 0; count < areg::THREAD_DRAIN_LIMIT - 1u; ++count)
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
            DEBUG_LOG_WARN("Discarding queued message (ID = [ %u ]) for disconnected target [ %u ]"
                            , static_cast<uint32_t>(msg.message_id())
                            , static_cast<uint32_t>(msg.target()));
            sendEvt->destroy();
            continue;
        }

        mBatch[batchCount++] = { hSocket, sendEvt };
    }

    // --- Phase 2+3: sort by socket and send each same-socket group, including trigger ---
    areg::ext::sort_pending_sends(mBatch.data(), batchCount);
    areg::ext::send_pending_groups_with_trigger(mBatch.data(), batchCount, trigSocket, trigMsg, mConnection, mRemoteService,
        [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_sent(bytes, msgs); });

#if defined(AREG_LOG_DEBUG) && (AREG_LOG_DEBUG != 0)
    if (batchCount >= areg::THREAD_DRAIN_LIMIT - 1u)
    {
        DEBUG_LOG_WARN("Send drain loop exhausted THREAD_DRAIN_LIMIT - 1 (%d) -- outbound queue is building up", areg::THREAD_DRAIN_LIMIT - 1u);
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
