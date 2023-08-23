/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/service/private/LoggerServerService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger connection service
 ************************************************************************/
#include "logger/service/LoggerServerService.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageReceived);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageSend);

DEF_TRACE_SCOPE(logger_service_LoggerServerService_startConnection);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_restartConnection);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_stopConnection);

DEF_TRACE_SCOPE(logger_service_LoggerServerService_failedSendMessage);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// LoggerServerService class implementation
//////////////////////////////////////////////////////////////////////////

LoggerServerService::LoggerServerService( void )
    : ServiceCommunicatonBase   ( NEService::COOKIE_LOGGER, NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept )
{
}

void LoggerServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
#if AREG_LOGS

    TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );
    ITEM_ID source= static_cast<ITEM_ID>(msgReceived.getSource());

    TRACE_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

#endif // AREG_LOGS
}

void LoggerServerService::onServiceMessageSend(const RemoteMessage &msgSend)
{
    TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageSend);

    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
    TRACE_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( NEService::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != NEService::TARGET_UNKNOWN )
        {
            sendMessage( msgSend );
        }
    }
    else
    {
        TRACE_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void LoggerServerService::disconnectServices(void)
{
}

void LoggerServerService::connectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LoggerServerService::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LoggerServerService::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void LoggerServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{
}

void LoggerServerService::onServiceConnectionStarted( void )
{
}

void LoggerServerService::onServiceConnectionStopped( void )
{
}

void LoggerServerService::onServiceConnectionLost( void )
{
}
