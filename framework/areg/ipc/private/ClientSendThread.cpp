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
    : DispatcherThread        ( namePrefix + areg::CLIENT_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_BIG, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mSendStats        ( )
{
}

void ClientSendThread::ready_for_events( bool is_ready )
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
        const ExitEvent & exitEvent = ExitEvent::exit_event();

        SendMessageEvent* evtPtrs[areg::THREAD_DRAIN_LIMIT];
        areg::IoBuffer    ioBuffer[areg::THREAD_DRAIN_LIMIT];
        uint32_t batchCount { 0u };
        uint32_t totalSize  { 0u };
        uint32_t bufCount   { 0u };

        evtPtrs[batchCount++] = nullptr;
        const RemoteMessage& msg1 = data.remote_message();
        const areg::MessageHeader* hdr = msg1.header();
        if (hdr != nullptr)
        {
            msg1.buffer_completion_fix();
            const uint32_t bufSize = sizeof(areg::MessageHeader) + hdr->rbhBufHeader.biUsed;
            ioBuffer[bufCount ++]  = { reinterpret_cast<const uint8_t*>(hdr), bufSize };
            totalSize += bufSize;
        }

        // --- Phase 1: drain additional queued messages ---
        for (uint32_t count{ batchCount }; count < areg::THREAD_DRAIN_LIMIT; ++count)
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

            ASSERT(AREG_RUNTIME_CAST(evt, SendMessageEvent) != nullptr);
            SendMessageEvent* sendEvt = static_cast<SendMessageEvent *>(evt);
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

            evtPtrs[batchCount++] = sendEvt;
            const RemoteMessage& msg = evtData.remote_message();
            hdr = msg.header();
            if (hdr != nullptr)
            {
                msg.buffer_completion_fix();
                const uint32_t bufSize = sizeof(areg::MessageHeader) + hdr->rbhBufHeader.biUsed;
                ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr), bufSize };
                totalSize += bufSize;
            }
        }

        // --- Phase 2: send the collected batch ---

        const int32_t sentBytes = mConnection.send_messages_batch(ioBuffer, bufCount, totalSize);
        for (uint32_t i = 1; i < batchCount; ++i)
            evtPtrs[i]->destroy();

        if (sentBytes > 0)
        {
            accumulate_sent(static_cast<uint64_t>(sentBytes), bufCount);
        }
        else
        {
            mRemoteService.failed_send_message(msg1, mConnection.socket());
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
