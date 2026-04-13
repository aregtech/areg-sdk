/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ClientSendThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send thread implementation.
 ************************************************************************/
#include "aregextend/service/private/ClientSendThread.hpp"

#include "areg/component/ExitEvent.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/private/ServerSendThread.hpp"

namespace areg::ext {

DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ClientSendThread, process_event);
DEBUG_DEF_LOG_SCOPE(areg_aregextend_service_ClientSendThread, _do_send);

ClientSendThread::ClientSendThread( areg::RemoteMessageHandler & remoteService
                                  , ServerConnection & connection
                                  , ServerSendThread & globalStats
                                  , std::string_view threadName )
    : DispatcherThread          ( String(threadName), areg::SYSTEM_THREAD_STACK_NORMAL, areg::SEND_THREAD_QUEUE_LIMIT )
    , SendMessageEventConsumer  ( )

    , mRemoteService    ( remoteService )
    , mConnection       ( connection )
    , mGlobalStats      ( globalStats )
    , mBytesSend        ( 0u )
    , mMsgsSend         ( 0u )
    , mSaveDataSend     ( false )
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
    }
}

bool ClientSendThread::_do_send( const areg::RemoteMessage & msg )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ClientSendThread, _do_send);
    ASSERT( msg.is_valid() );

    const ITEM_ID target{ msg.target() };
    areg::SocketAccepted clientSocket{ mConnection.client_by_cookie(target) };
    if ( !clientSocket.is_valid() )
    {
        // Target is no longer connected — silently drop the message.
        DEBUG_LOG_DBG("Pool send thread: target cookie [ %u ] not found, dropping message [ %u ]"
                        , static_cast<uint32_t>(target)
                        , static_cast<uint32_t>(msg.message_id()));
        return true;
    }

    const int32_t sentBytes = mConnection.send_message(msg, clientSocket);
    if ( sentBytes > 0 )
    {
        if ( mSaveDataSend )
        {
            const uint64_t bytes = static_cast<uint64_t>(sentBytes);
            mBytesSend.fetch_add(bytes, std::memory_order_relaxed);
            mMsgsSend.fetch_add(1u, std::memory_order_relaxed);
            mGlobalStats.accumulate_sent(bytes, 1u);
        }

        return true;
    }

    DEBUG_LOG_WARN("Pool send thread: failed to send message [ %u ] to target [ %u ], socket [ %u ], result [ %d ]"
                    , static_cast<uint32_t>(msg.message_id())
                    , static_cast<uint32_t>(target)
                    , static_cast<uint32_t>(clientSocket.handle())
                    , sentBytes);

    mRemoteService.failed_send_message(msg, clientSocket);
    return false;
}

void ClientSendThread::process_event( const SendMessageEventData & data )
{
    DEBUG_LOG_SCOPE(areg_aregextend_service_ClientSendThread, process_event);

    if ( data.is_exit_message() )
    {
        DEBUG_LOG_DBG("Pool send thread: received exit message, shutting down");
        trigger_exit();
        return;
    }

    if ( !data.is_forward_message() )
        return;

    if ( !_do_send( data.remote_message() ) )
        return;

    // Batch drain: pull up to DRAIN_LIMIT additional queued events without
    // returning to the blocking wait. Reduces kernel wake-up overhead under
    // burst load.
    constexpr int32_t DRAIN_LIMIT{ areg::THREAD_DRAIN_LIMIT };
    const ExitEvent & exitEvent = ExitEvent::exit_event();

    int32_t drainCount{ 0 };
    for ( int32_t i = 0; i < DRAIN_LIMIT; ++i )
    {
        Event * evt = pick_event();
        if ( evt == nullptr )
            break;

        if ( static_cast<const Event *>( evt ) == static_cast<const Event *>( &exitEvent ) )
        {
            DEBUG_LOG_DBG("Pool send thread: exit event during drain, shutting down");
            trigger_exit();
            return;
        }

        SendMessageEvent * sendEvt = AREG_RUNTIME_CAST( evt, SendMessageEvent );
        if ( sendEvt == nullptr )
        {
            evt->destroy();
            break;
        }

        const SendMessageEventData & evtData = sendEvt->data();
        if ( evtData.is_exit_message() )
        {
            DEBUG_LOG_DBG("Pool send thread: exit data during drain, shutting down");
            sendEvt->destroy();
            trigger_exit();
            return;
        }

        if ( evtData.is_forward_message() )
        {
            const areg::RemoteMessage & drainMsg = evtData.remote_message();
            const ITEM_ID drainTarget{ drainMsg.target() };
            areg::SocketAccepted drainSocket{ mConnection.client_by_cookie(drainTarget) };
            if ( drainSocket.is_valid() )
            {
                const int32_t sent = mConnection.send_message(drainMsg, drainSocket);
                if ( sent > 0 )
                {
                    if ( mSaveDataSend )
                    {
                        const uint64_t bytes = static_cast<uint64_t>(sent);
                        mBytesSend.fetch_add(bytes, std::memory_order_relaxed);
                        mMsgsSend.fetch_add(1u, std::memory_order_relaxed);
                        mGlobalStats.accumulate_sent(bytes, 1u);
                    }

                    ++drainCount;
                }
                else
                {
                    DEBUG_LOG_WARN("Pool send thread: drain send failed for target [ %u ]", static_cast<uint32_t>(drainTarget));
                    mRemoteService.failed_send_message(drainMsg, drainSocket);
                    sendEvt->destroy();
                    return;
                }
            }
            // else: target gone — silently drop and continue drain
        }

        sendEvt->destroy();
    }

    if ( drainCount >= DRAIN_LIMIT )
    {
        DEBUG_LOG_WARN("Pool send thread: drain limit (%d) exhausted", DRAIN_LIMIT);
    }
}

bool ClientSendThread::post_event( Event & eventElem )
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
