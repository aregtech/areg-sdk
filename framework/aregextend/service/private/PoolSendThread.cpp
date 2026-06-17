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

#include "areg/base/private/DebugDefs.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_PoolSendThread, start_event_processing);

PoolSendThread::PoolSendThread( ClientConnectionPair & owner
                                , areg::RemoteMessageHandler & remoteService
                                , ServerConnection & connection
                                , ServerSendThread & globalStats
                                , std::string_view threadName )
    : DispatcherThread  ( String(threadName), areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , EventConsumer     ( )

    , mOwner            ( owner )
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
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
    }
}

void PoolSendThread::start_event_processing( areg::Event & eventElem )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_PoolSendThread, start_event_processing);

    if ( eventElem.is_exit_prio() )
    {
        DEBUG_LOG_DBG("Going to quit pool send message thread");
        trigger_exit();
        return;
    }

    // Zero local-only routing fields before wire transmission.
    areg::EventHeader * hdr0{ eventElem.header() };
    ASSERT( hdr0 != nullptr );
    hdr0->internal1 = 0u;
    hdr0->internal2 = 0u;
    hdr0->custom    = 0u;

    uint32_t batchCount{ 0u };

    mTargets[batchCount]      = static_cast<ITEM_ID>(hdr0->target);
    mBatch[batchCount].socket = areg::InvalidSocketHandle;
    mBatch[batchCount].msg    = eventElem.envelope();  // fields already zeroed
    ++batchCount;

    // Phase 1: drain additional queued events straight into mBatch
    while ( batchCount < areg::DEFAULT_DRAIN_LIMIT )
    {
        areg::Event evt{ pick_event() };
        if ( !evt.is_valid() )
            break;

        if ( evt.is_exit_prio() )
        {
            DEBUG_LOG_DBG("Received exit event during batch-drain, stopping pool send thread");
            for ( uint32_t i{ 0u }; i < batchCount; ++i )
                mBatch[i].msg.destroy_event();

            trigger_exit();
            return;
        }

        areg::EventHeader * hdr{ evt.header() };
        ASSERT( hdr != nullptr );
        hdr->internal1 = 0u;
        hdr->internal2 = 0u;
        hdr->custom    = 0u;

        mTargets[batchCount]      = static_cast<ITEM_ID>(hdr->target);
        mBatch[batchCount].socket = areg::InvalidSocketHandle;
        mBatch[batchCount].msg    = evt.envelope();
        ++batchCount;
    }

    // Phase 2: resolve all cookies in one lock window.
    mOwner.batch_sockets_by_cookies(mTargets.data(), mSockets.data(), batchCount);

    // Phase 3: compact + insertion-sort by socket handle in one pass.
    uint32_t validCount{ 0u };
    for ( uint32_t i{ 0u }; i < batchCount; ++i )
    {
        if ( !areg::is_valid_socket(mSockets[i]) )
        {
            DEBUG_LOG_WARN("Discarding message (ID = [ %u ]) for disconnected target [ %u ]"
                            , mBatch[i].msg.message_id()
                            , static_cast<uint32_t>(mTargets[i]));
            continue;
        }

        areg::ext::PendingSend entry{ mSockets[i], std::move(mBatch[i].msg) };
        uint32_t lo{ 0u }, hi{ validCount };
        while ( lo < hi )
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            mBatch[mid].socket <= entry.socket ? lo = mid + 1u : hi = mid;
        }

        // Shift right using move-assignment
        for ( uint32_t j{ validCount }; j > lo; --j )
            mBatch[j] = std::move(mBatch[j - 1]);

        mBatch[lo] = std::move(entry);
        ++validCount;
    }

    if ( validCount != 0u )
    {
        // Phase 4: batch is already sorted -- send groups directly.
        AREG_LT_SCOPE(areg::LtStage::SendSyscall);  // isolate the ::send() from resolve+sort
        areg::ext::send_pending_groups(mBatch.data(), validCount, mConnection, mRemoteService,
            [this](uint64_t bytes, uint32_t msgs) { mGlobalStats.accumulate_sent(bytes, msgs); });
    }

    // Phase 5: release every wire buffer
    for ( uint32_t i{ 0u }; i < batchCount; ++i )
        mBatch[i].msg.destroy_event();
}

bool PoolSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg::ext
