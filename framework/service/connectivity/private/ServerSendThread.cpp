/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        service/connectivity/private/ServerSendThread.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service connectivity server send message thread
 ************************************************************************/
#include "service/connectivity/ServerSendThread.hpp"

#include "service/connectivity/ServerConnection.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/component/NEService.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_service_connectivity_ServerSendThread_processEvent);

ServerSendThread::ServerSendThread(IERemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread          ( NEConnection::SERVER_SEND_MESSAGE_THREAD )
    , IESendMessageEventConsumer( )
    , mRemoteService            ( remoteService )
    , mConnection               ( connection )
    , mBytesSend                ( 0 )
{
}

bool ServerSendThread::runDispatcher( void )
{
    removeAllEvents();
    SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this));
    bool result = DispatcherThread::runDispatcher();
    SendMessageEvent::removeListener(static_cast<IESendMessageEventConsumer&>(*this), static_cast<DispatcherThread&>(*this));

    mConnection.disableReceive();
    mConnection.closeAllConnections();

    return result;
}

void ServerSendThread::processEvent( const SendMessageEventData & data )
{
    TRACE_SCOPE(areg_service_connectivity_ServerSendThread_processEvent);
    const RemoteMessage& msgSend = data.getRemoteMessage();
    if (msgSend.isValid())
    {
        ITEM_ID target = static_cast<ITEM_ID>(msgSend.getTarget());
        SocketAccepted client = mConnection.getClientByCookie(target);

        TRACE_DBG("Sending message [ %s ] (ID = [ %u ]) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %u ] to target [ %u ]"
                    , NEService::getString(static_cast<NEService::eFuncIdRange>(msgSend.getMessageId()))
                    , static_cast<unsigned int>(msgSend.getMessageId())
                    , client.getAddress().getHostAddress().getString()
                    , client.getAddress().getHostPort()
                    , ((unsigned int)(client.getHandle()))
                    , static_cast<unsigned int>(msgSend.getSource())
                    , static_cast<unsigned int>(msgSend.getTarget()));

        int sentBytes = 0;
        if ((client.isAlive() == false) || ((sentBytes = mConnection.sendMessage(msgSend, client)) <= 0))
        {
            TRACE_WARN("Failed to send message [ %u ] to target [ %u ], client is [ %s ]"
                        , msgSend.getMessageId()
                        , static_cast<unsigned int>(msgSend.getTarget())
                        , client.isAlive() ? "IS ALIVE" : "IS NOT ALIVE");

            mRemoteService.failedSendMessage(msgSend, client);
        }
        else
        {
            mBytesSend += sentBytes;
            TRACE_DBG("Succeeded to send message [ %u ] to target [ %p ]", msgSend.getMessageId(), static_cast<id_type>(msgSend.getTarget()));
        }
    }
    else
    {
        TRACE_DBG("Message [ %s ] ( ID = %p ) from source [ %p ] to target [ %p ], ignoring to send message"
                    , NEService::getString(static_cast<NEService::eFuncIdRange>(msgSend.getMessageId()))
                    , static_cast<id_type>(msgSend.getMessageId())
                    , static_cast<id_type>(msgSend.getSource())
                    , static_cast<id_type>(msgSend.getTarget()));
    }
}

bool ServerSendThread::postEvent(Event & eventElem)
{
    return ( RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr ? EventDispatcher::postEvent(eventElem) : false );
}
