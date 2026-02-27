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
#include "areg/logging/GELog.h"
#include "aregextend/service/ServerConnection.hpp"


DEF_LOG_SCOPE(areg_aregextend_service_ServerSendThread_processEvent);

namespace aregext
{

    ServerSendThread::ServerSendThread(areg::RemoteMessageHandler& remoteService, ServerConnection & connection)
        : areg::DispatcherThread          ( areg::SERVER_SEND_MESSAGE_THREAD, areg::DEFAULT_BLOCK_SIZE, areg::QUEUE_SIZE_MAXIMUM )
        , areg::SendMessageEventConsumer( )
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
            areg::SendMessageEvent::addListener( static_cast<areg::SendMessageEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(*this) );
            areg::DispatcherThread::readyForEvents( true );
        }
        else
        {
            areg::DispatcherThread::readyForEvents( false );
            areg::SendMessageEvent::removeListener( static_cast<areg::SendMessageEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(*this) );
            mConnection.closeAllConnections( );
            mConnection.disableSend( );
        }
    }

    void ServerSendThread::processEvent( const areg::SendMessageEventData & data )
    {
        LOG_SCOPE( areg_aregextend_service_ServerSendThread_processEvent );
        if (data.isForwardMessage())
        {
            const areg::RemoteMessage & msgSend = data.getRemoteMessage( );
            ASSERT( msgSend.isValid( ) );

            const ITEM_ID & target{ msgSend.getTarget() };
            areg::SocketAccepted client{ mConnection.getClientByCookie(target) };

            LOG_DBG("Sending message [ %s ] (ID = [ %u ]) to client [ %s : %d ] of socket [ %u ]. The message sent from source [ %u ] to target [ %u ]"
                        , areg::getString(static_cast<areg::FuncIdRange>(msgSend.getMessageId()))
                        , static_cast<uint32_t>(msgSend.getMessageId())
                        , client.getAddress().getHostAddress().getString()
                        , client.getAddress().getHostPort()
                        , ((uint32_t)(client.getHandle()))
                        , static_cast<uint32_t>(msgSend.getSource())
                        , static_cast<uint32_t>(msgSend.getTarget()));

            int32_t sentBytes = 0;
            if ((client.isAlive() == false) || ((sentBytes = mConnection.sendMessage(msgSend, client)) <= 0))
            {
                LOG_WARN("Failed to send message [ %u ] to target [ %u ], client is [ %s ]"
                            , msgSend.getMessageId()
                            , static_cast<uint32_t>(msgSend.getTarget())
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

    bool ServerSendThread::postEvent(areg::Event & eventElem)
    {
        return (AREG_RUNTIME_CAST(&eventElem, areg::SendMessageEvent) != nullptr) && areg::EventDispatcher::postEvent(eventElem);
    }
} // namespace aregext
