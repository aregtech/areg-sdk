/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServerSendThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service connectivity server send message thread
 ************************************************************************/
#include "aregextend/service/private/ServerSendThread.hpp"

#include "areg/component/NEService.hpp"
#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/service/ServerConnection.hpp"


DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread_processEvent);

ServerSendThread::ServerSendThread(IERemoteMessageHandler& remoteService, ServerConnection & connection)
    : DispatcherThread          ( NEConnection::SERVER_SEND_MESSAGE_THREAD, NECommon::DEFAULT_BLOCK_SIZE, NECommon::QUEUE_SIZE_MAXIMUM )
    , IESendMessageEventConsumer( )
    , mRemoteService            ( remoteService )
    , mConnection               ( connection )
    , mBytesSend                ( 0 )
    , mSaveDataSend             ( false )
{
}

void ServerSendThread::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        SendMessageEvent::addListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        DispatcherThread::readyForEvents( true );
    }
    else
    {
        DispatcherThread::readyForEvents( false );
        SendMessageEvent::removeListener( static_cast<IESendMessageEventConsumer &>(*this), static_cast<DispatcherThread &>(*this) );
        mConnection.closeAllConnections( );
        mConnection.disableSend( );
    }
}

void ServerSendThread::processEvent( const SendMessageEventData & data )
{
    LOG_SCOPE( areg_aregextend_service_ServerSendThread_processEvent );
    if (data.isForwardMessage())
    {
        const RemoteMessage & msgSend = data.getRemoteMessage( );
        ASSERT( msgSend.isValid( ) );

        const ITEM_ID & target{ msgSend.getTarget() };
        SocketAccepted client{ mConnection.getClientByCookie(target) };

        LOG_DBG("Sending message [ %s ] (ID = [ %u ]) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %u ] to target [ %u ]"
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
            LOG_WARN("Failed to send message [ %u ] to target [ %u ], client is [ %s ]"
                        , msgSend.getMessageId()
                        , static_cast<unsigned int>(msgSend.getTarget())
                        , client.isAlive() ? "ALIVE" : "DEAD");

            mRemoteService.failedSendMessage(msgSend, client);
        }
        else
        {
            if (mSaveDataSend)
            {
                mBytesSend += static_cast<uint32_t>(sentBytes);
            }

            LOG_DBG("Succeeded to send message [ %u ] to target [ %p ]", msgSend.getMessageId(), static_cast<id_type>(msgSend.getTarget()));
        }
    }
    else if (data.isExitThreadMessage() )
    {
        LOG_DBG("Going to quite send message thread");
        mConnection.closeAllConnections( );
        mConnection.closeSocket( );
        triggerExit( );
    }
}

bool ServerSendThread::postEvent(Event & eventElem)
{
    return (RUNTIME_CAST(&eventElem, SendMessageEvent) != nullptr) && EventDispatcher::postEvent(eventElem);
}
