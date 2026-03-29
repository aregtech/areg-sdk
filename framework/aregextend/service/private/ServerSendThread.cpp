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
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/service/ServerConnection.hpp"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread, process_event);

ServerSendThread::ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread          ( areg::SERVER_SEND_MESSAGE_THREAD, areg::SYSTEM_THREAD_STACK_NORMAL, areg::QUEUE_SIZE_MAXIMUM )
    , SendMessageEventConsumer( )
    , mRemoteService            ( remoteService )
    , mConnection               ( connection )
    , mBytesSend                ( 0 )
    , mSaveDataSend             ( false )
{
}

void ServerSendThread::ready_for_events( bool is_ready )
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
        mConnection.close_all_connections( );
        mConnection.disable_send( );
    }
}

void ServerSendThread::process_event( const SendMessageEventData & data )
{
    LOG_SCOPE( areg_aregextend_service_ServerSendThread, process_event );
    if (data.is_forward_message())
    {
        const RemoteMessage & msgSend = data.remote_message( );
        ASSERT( msgSend.is_valid( ) );

        const ITEM_ID & target{ msgSend.target() };
        SocketAccepted client{ mConnection.client_by_cookie(target) };

        LOG_DBG("Sending message [ %s ] (ID = [ %u ]) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %u ] to target [ %u ]"
                    , areg::as_string(static_cast<areg::FuncIdRange>(msgSend.message_id()))
                    , static_cast<uint32_t>(msgSend.message_id())
                    , client.address().host_address().as_string()
                    , client.address().host_port()
                    , ((uint32_t)(client.handle()))
                    , static_cast<uint32_t>(msgSend.source())
                    , static_cast<uint32_t>(msgSend.target()));

        int32_t sentBytes = 0;
        if ((client.is_alive() == false) || ((sentBytes = mConnection.send_message(msgSend, client)) <= 0))
        {
            LOG_WARN("Failed to send message [ %u ] to target [ %u ], client is [ %s ]"
                        , msgSend.message_id()
                        , static_cast<uint32_t>(msgSend.target())
                        , client.is_alive() ? "ALIVE" : "DEAD");

            mRemoteService.failed_send_message(msgSend, client);
        }
        else
        {
            if (mSaveDataSend)
            {
                mBytesSend += static_cast<uint32_t>(sentBytes);
            }

            LOG_DBG("Succeeded to send message [ %u ] to target [ %p ]", msgSend.message_id(), static_cast<id_type>(msgSend.target()));
        }
    }
    else if (data.is_exit_message() )
    {
        LOG_DBG("Going to quite send message thread");
        mConnection.close_all_connections( );
        mConnection.close_socket( );
        trigger_exit( );
    }
}

bool ServerSendThread::post_event(Event & eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

} // namespace areg::ext
