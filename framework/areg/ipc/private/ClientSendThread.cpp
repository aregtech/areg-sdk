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
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/base/MessageEnvelope.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg {

ClientSendThread::ClientSendThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String& namePrefix )
    : DispatcherThread  ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , EventConsumer     ( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mSendStats        ( )
{
}

void ClientSendThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        areg::set_receive_mode(areg::ReceiveMode::MonoCache);
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        mConnection.close_socket( );
    }
}

void ClientSendThread::start_event_processing( Event & eventElem )
{
    AREG_LT_SCOPE(areg::LtStage::SendNode);     // serialize routing fields + batch + writev

    if ( eventElem.is_exit_prio() )
    {
        mConnection.close_socket();
        trigger_exit();
        return;
    }

    // Zero local-only routing fields before wire transmission.
    areg::EventHeader * hdr0{ eventElem.header() };
    ASSERT( hdr0 != nullptr );
    hdr0->internal1 = 0u;
    hdr0->internal2 = 0u;
    hdr0->custom    = 0u;
    eventElem.envelope().buffer_completion_fix(); // compute checksum now; zeroed fields are not covered by checksum

    // Drained buffers are retained in mDrain[] (a reused member array) so the raw header pointers held
    // in ioBuffer stay valid through the writev. We retain only the shared_ptr (one atomic increment),
    // never a full Event. mDrain is reset, not reconstructed, each call. Slot 0 is the triggering event
    // (kept alive by the caller); drained events occupy slots [1 .. bufCount).
    // ZEPHYR-INCOMPATIBLE: ~4 KB stack IoBuffer array for batch send draining.
    areg::IoBuffer ioBuffer[areg::THREAD_DRAIN_LIMIT];
    uint32_t totalSize { 0u };
    uint32_t bufCount  { 0u };

    {
        const uint32_t wireSize{ static_cast<uint32_t>(sizeof(areg::EventHeader)) + hdr0->bufHeader.biUsed };
        ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr0), wireSize };
        totalSize += wireSize;
    }

    // Drain further queued events into the same batch (one OS send). The single-message ping-pong case
    // finds the queue empty and skips the loop, touching no mDrain slot.
    for ( ; bufCount < areg::THREAD_DRAIN_LIMIT; ++bufCount )
    {
        Event evt{ pick_event() };
        if ( !evt.is_valid() )
            break;

        if ( evt.is_exit_prio() )
        {
            for ( uint32_t i{ 1u }; i < bufCount; ++i )
                mDrain[i].reset();

            mConnection.close_socket();
            trigger_exit();
            return;
        }

        areg::EventHeader * hdr{ evt.header() };
        ASSERT( hdr != nullptr );
        hdr->internal1 = 0u;
        hdr->internal2 = 0u;
        hdr->custom    = 0u;
        evt.envelope().buffer_completion_fix(); // compute checksum now; zeroed fields are not covered by checksum

        const uint32_t wireSize{ static_cast<uint32_t>(sizeof(areg::EventHeader)) + hdr->bufHeader.biUsed };
        ioBuffer[bufCount] = { reinterpret_cast<const uint8_t*>(hdr), wireSize };
        totalSize += wireSize;
        mDrain[bufCount] = evt.envelope().share_buffer();  // retain buffer; raw pointer hdr stays valid through writev
    }

    int32_t sentBytes{ 0 };
    {
        AREG_LT_SCOPE(areg::LtStage::SendSyscall);  // isolates the raw send()/writev syscall cost
        sentBytes = mConnection.send_messages_batch(ioBuffer, bufCount, totalSize);
    }

    if ( sentBytes > 0 )
        accumulate_sent( static_cast<uint64_t>(sentBytes), bufCount );
    else
        mRemoteService.failed_send_message( eventElem.envelope(), mConnection.socket() );

    // Release retained drained buffers (slot 0 is owned by the caller). Keeps mDrain allocated for reuse.
    for ( uint32_t i{ 1u }; i < bufCount; ++i )
        mDrain[i].reset();
}

bool ClientSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg
