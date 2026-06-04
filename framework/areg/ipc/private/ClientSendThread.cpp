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
#include "areg/base/EventEnvelope.hpp"

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

    areg::IoBuffer  ioBuffer[areg::THREAD_DRAIN_LIMIT];
    uint32_t batchCount{ 0u };
    uint32_t totalSize { 0u };
    uint32_t bufCount  { 0u };

    {
        const uint32_t wireSize{ static_cast<uint32_t>(sizeof(areg::EventHeader)) + hdr0->bufHeader.biUsed };
        ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr0), wireSize };
        totalSize += wireSize;
        ++batchCount;
    }

    // Phase 1: drain additional events from the queue.
    // ZEPHYR-INCOMPATIBLE: ~4KB stack array for batch send draining.
    Event ptrs[areg::THREAD_DRAIN_LIMIT];
    uint32_t evtCount{ 0u };

    for ( ; batchCount < areg::THREAD_DRAIN_LIMIT; ++batchCount )
    {
        Event evt{ pick_event() };
        if ( !evt.is_valid() )
            break;

        if ( evt.is_exit_prio() )
        {
            for ( uint32_t i{ 0u }; i < evtCount; ++i )
                ptrs[i].destroy_event();

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
        ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr), wireSize };
        totalSize += wireSize;

        ptrs[evtCount++] = std::move(evt); // buffer stays alive in ptrs[]; raw pointer hdr remains valid
    }

    // Phase 2: send collected batch as a single writev/scatter-gather call.
    const int32_t sentBytes{ mConnection.send_messages_batch(ioBuffer, bufCount, totalSize) };

    for ( uint32_t i{ 0u }; i < evtCount; ++i )
        ptrs[i].destroy_event();

    if ( sentBytes > 0 )
    {
        accumulate_sent( static_cast<uint64_t>(sentBytes), bufCount );
    }
    else
    {
        mRemoteService.failed_send_message( eventElem.envelope(), mConnection.socket() );
    }
}

bool ClientSendThread::post_event( Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

} // namespace areg
