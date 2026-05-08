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
    : DispatcherThread        ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_NORMAL, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mBytesSend        ( 0u )
    , mMsgsSend         ( 0u )
    , mSaveDataSend     ( false )
#if defined(__linux__)
    , mZerocopyRing     ( )
    , mZerocopyRingHead ( 0u )
    , mZerocopyRingTail ( 0u )
    , mZerocopyRingCount( 0u )
    , mZerocopyIdNext   ( 0u )
    , mZerocopyConfirmed( UINT32_MAX )
#endif  // defined(__linux__)
{
}

void ClientSendThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        SendMessageEvent::add_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
#if defined(__linux__)
        mZerocopyRingHead  = 0u;
        mZerocopyRingTail  = 0u;
        mZerocopyRingCount = 0u;
        mZerocopyIdNext    = 0u;
        mZerocopyConfirmed = UINT32_MAX;
#endif  // defined(__linux__)
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        SendMessageEvent::remove_listener( static_cast<SendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
#if defined(__linux__)
        // Flush ring BEFORE closing socket: once the socket is closed the kernel
        // will not deliver further ERRQUEUE completions, so blocking drains would
        // stall forever.  Force-release all slots immediately.
        _flush_ring();
#endif  // defined(__linux__)
        mConnection.close_socket( );
    }
}

void ClientSendThread::process_event( const SendMessageEventData & data )
{
    if ( data.is_forward_message() )
    {
        // Include the triggering message as element [0] of the batch, then drain more from
        // the queue. This removes the extra solo send syscall that previously preceded the
        // drain loop, halving the number of send syscalls at full load.
        //
        // evtPtrs[0] is nullptr: the triggering event is owned by the dispatch chain —
        // never call destroy() on it.  Drained events (indices 1..N) are owned by us.
        constexpr uint32_t DRAIN_LIMIT{ areg::THREAD_DRAIN_LIMIT };
        const ExitEvent & exitEvent = ExitEvent::exit_event();

        const RemoteMessage*    msgPtrs[DRAIN_LIMIT + 1];
        SendMessageEvent*       evtPtrs[DRAIN_LIMIT + 1];
        uint32_t batchCount{ 0 };

        msgPtrs[batchCount] = &data.remote_message();
        evtPtrs[batchCount] = nullptr;
        ++batchCount;

        // --- Phase 1: drain additional queued messages ---
        for ( uint32_t count = 0; count < DRAIN_LIMIT; ++count )
        {
            Event * evt = pick_event();
            if ( evt == nullptr )
                break;

            // ExitEvent is a singleton — compare by pointer, never call destroy() on it.
            if ( static_cast<const Event *>( evt ) == static_cast<const Event *>( &exitEvent ) )
            {
                for ( uint32_t i = 1; i < batchCount; ++i )
                    evtPtrs[i]->destroy();

                mConnection.close_socket();
                trigger_exit();
                return;
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
                sendEvt->destroy();
                for ( uint32_t i = 1; i < batchCount; ++i )
                    evtPtrs[i]->destroy();

                mConnection.close_socket();
                trigger_exit();
                return;
            }

            msgPtrs[batchCount] = &evtData.remote_message();
            evtPtrs[batchCount] = sendEvt;
            ++batchCount;
        }

        // --- Phase 2: send the collected batch ---

#if defined(__linux__)
        if (mConnection.is_zerocopy_enabled())
        {
            // Non-blocking drain of ERRQUEUE: updates mZerocopyConfirmed and
            // releases any ring slots that have been confirmed by the kernel.
            // Skip if ring is empty — avoids a syscall on every batch.
            if (mZerocopyRingCount > 0u)
                _drain_available();

            // If the ring is full, block until the oldest slot is confirmed
            // so we can reuse its slot.  This happens at most ZEROCOPY_RING_SIZE
            // times per THREAD_DRAIN_LIMIT messages, so amortised overhead is low.
            while (mZerocopyRingCount == ZEROCOPY_RING_SIZE)
                _drain_oldest_blocking();

            // Reset per-batch zerocopy counter before sending.
            areg::socket_take_zerocopy_count();

            int32_t sent_bytes{ 0 };
            for (uint32_t i = 0u; i < batchCount; ++i)
            {
                const int32_t bytes = mConnection.send_message_zerocopy(*msgPtrs[i]);
                if (bytes > 0)
                    sent_bytes += bytes;
                else if (bytes < 0)
                {
                    // Hard send error: flush ring and report failure.
                    _flush_ring();
                    for (uint32_t k = 1u; k < batchCount; ++k)
                        evtPtrs[k]->destroy();

                    mRemoteService.failed_send_message(*msgPtrs[0], mConnection.socket());
                    return;
                }
                // bytes == 0 means message not valid — skip silently
            }

            const uint32_t sends_made = areg::socket_take_zerocopy_count();

            if (sends_made == 0u)
            {
                // All sends fell back to regular copy (ENOBUFS or no MSG_ZEROCOPY support).
                // No ERRQUEUE notifications will arrive — safe to destroy events now.
                for (uint32_t i = 1u; i < batchCount; ++i)
                    evtPtrs[i]->destroy();

                accumulate_sent(static_cast<uint64_t>(sent_bytes), batchCount);
            }
            else
            {
                // At least one MSG_ZEROCOPY send succeeded.  The kernel is DMA-ing
                // directly from the SharedBuffer pages.  We must keep the buffers alive
                // until the ERRQUEUE confirms their hi_id.  Store in the ring slot.
                const uint32_t hi_id = mZerocopyIdNext + sends_made - 1u;
                mZerocopyIdNext += sends_made;

                ZerocopyEntry& slot = mZerocopyRing[mZerocopyRingHead];
                slot.hi_id       = hi_id;
                slot.trigger_msg = data.remote_message();   // copy --> ref +1
                slot.event_count = batchCount - 1u;
                for (uint32_t i = 1u; i < batchCount; ++i)
                    slot.events[i - 1u] = evtPtrs[i];      // ownership transferred

                mZerocopyRingHead = (mZerocopyRingHead + 1u) % ZEROCOPY_RING_SIZE;
                ++mZerocopyRingCount;

                // Do NOT destroy drained events here — ownership moved to ring slot.
                accumulate_sent(static_cast<uint64_t>(sent_bytes), batchCount);
            }

            return;
        }
#endif  // defined(__linux__)

        // Non-zerocopy path: original batch send.
        const int32_t sentBytes = batchCount == 1u
                                    ? mConnection.send_message(*msgPtrs[0])
                                    : mConnection.send_messages_batch(msgPtrs, batchCount);

        for (uint32_t i = 1; i < batchCount; ++i)
            evtPtrs[i]->destroy();

        if (sentBytes > 0)
        {
            accumulate_sent(static_cast<uint64_t>(sentBytes), batchCount);
        }
        else
        {
            mRemoteService.failed_send_message(*msgPtrs[0], mConnection.socket());
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
