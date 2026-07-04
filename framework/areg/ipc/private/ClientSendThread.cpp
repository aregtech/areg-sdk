/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
    , mDrain            ( )
    , mIoBuffer         ( )
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

    uint32_t bufCount  { 0u };
    uint64_t totalSize { 0u };  // 64-bit: sum of up to DEFAULT_DRAIN_LIMIT wire sizes cannot overflow.
    {
        const uint32_t wireSize{ static_cast<uint32_t>(sizeof(areg::EventHeader)) + hdr0->bufHeader.biUsed };
        mIoBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr0), wireSize };
        totalSize += wireSize;
    }

    // Drain further queued events into the same batch (one OS send) via a single dequeue window.
    // The single-message ping-pong case drains nothing and touches no mDrain slot.
    const uint32_t drained{ pop_events(mEvents, areg::DEFAULT_DRAIN_LIMIT - bufCount) };
    for ( uint32_t k{ 0u }; k < drained; ++k, ++bufCount )
    {
        Event& evt{ mEvents[k] };
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
        mIoBuffer[bufCount] = { reinterpret_cast<const uint8_t*>(hdr), wireSize };
        totalSize += wireSize;
        mDrain[bufCount] = evt.envelope().share_buffer();  // retain buffer; raw pointer hdr stays valid through writev
        evt.destroy_event();                               // release the mEvents slot; mDrain keeps the buffer alive
    }

    if ( totalSize <= areg::MAX_SEND_BATCH_BYTES )
    {
        int32_t sentBytes{ 0 };
        {
            AREG_LT_SCOPE(areg::LtStage::SendSyscall);  // isolates the raw send()/writev syscall cost
            sentBytes = mConnection.send_messages_batch(mIoBuffer, bufCount, static_cast<uint32_t>(totalSize) );
        }

        if ( sentBytes > 0 )
            accumulate_sent( static_cast<uint64_t>(sentBytes), bufCount );
        else
            mRemoteService.failed_send_message( eventElem.envelope(), mConnection.socket() );
    }
    else
    {
        for ( uint32_t start{ 0u }; start < bufCount; )
        {
            uint32_t end       { start };
            uint32_t batchBytes{ 0u };
            do
            {
                batchBytes += static_cast<uint32_t>(mIoBuffer[end].size);
                ++end;
            } while ( (end < bufCount) && (static_cast<uint32_t>(mIoBuffer[end].size) <= (areg::MAX_SEND_BATCH_BYTES - batchBytes)) );

            int32_t sentBytes{ 0 };
            {
                AREG_LT_SCOPE(areg::LtStage::SendSyscall);
                sentBytes = mConnection.send_messages_batch(mIoBuffer + start, end - start, batchBytes );
            }

            if ( sentBytes > 0 )
                accumulate_sent( static_cast<uint64_t>(sentBytes), end - start );
            else
                mRemoteService.failed_send_message( eventElem.envelope(), mConnection.socket() );

            start = end;
        }
    }

    // Release retained drained buffers (slot 0 is owned by the caller). Keeps mDrain allocated for reuse.
    for ( uint32_t i{ 1u }; i < bufCount; ++i )
        mDrain[i].reset();
}

bool ClientSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg
