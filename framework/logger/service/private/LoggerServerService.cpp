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
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger connection service
 ************************************************************************/
#include "logger/service/LoggerServerService.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"
#include "areg/trace/private/TraceManager.hpp"

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
    : ServiceCommunicatonBase   ( NEService::COOKIE_LOGGER, NERemoteService::eRemoteServices::ServiceLogger, static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip), NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept )
    , mLoggerProcessor          ( self() )
    , mObservers                ( )
{
}

void LoggerServerService::addInstance(const ITEM_ID & cookie, const NEService::sServiceConnectedInstance& instance)
{
    Lock lock(mLock);

    ServiceCommunicatonBase::addInstance(cookie, instance);
    if (LoggerMessageProcessor::isLogSource(instance.ciSource))
    {
        mLoggerProcessor.notifyConnectedInstances(NEService::COOKIE_ANY);
    }
    else if (LoggerMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.addIfUnique(cookie, instance);
    }
}

void LoggerServerService::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);

    NEService::sServiceConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicatonBase::removeInstance(cookie);
    
    if (exists && LoggerMessageProcessor::isLogSource(instance.ciSource))
    {
        mLoggerProcessor.notifyConnectedInstances(NEService::COOKIE_ANY);
    }
    else if (LoggerMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.removeAt(cookie);
    }
}

void LoggerServerService::removeAllInstances(void)
{
    Lock lock(mLock);
    ServiceCommunicatonBase::removeAllInstances();
    mLoggerProcessor.notifyConnectedInstances(NEService::COOKIE_ANY);
    mObservers.clear();
}

void LoggerServerService::dispatchAndForwardLoggerMessage(const RemoteMessage& msgForward)
{
    Lock lock(mLock);

    ASSERT(msgForward.isValid());
    ASSERT(msgForward.getSource() == NEService::COOKIE_LOGGER);
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>(msgForward.getMessageId());
    switch (msgId)
    {
    case NEService::eFuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgForward);
        break;

    case NEService::eFuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgForward);
        break;

    case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgForward);
        break;

    default:
        ASSERT(false);
        break;
    }
}

void LoggerServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageReceived);

    Lock lock(mLock);
    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

    TRACE_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgReceived.getSource())
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch (msgId)
    {
    case NEService::eFuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.queryConnectedInstances(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.registerScopesAtObserver(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogMessage:
        mLoggerProcessor.logMessage(msgReceived);
        NETrace::logMessage(msgReceived);
        break;

    case NEService::eFuncIdRange::SystemServiceConnect:
    case NEService::eFuncIdRange::SystemServiceDisconnect:
        break;

    case NEService::eFuncIdRange::RequestRegisterService:           // fall through
    case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
    case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    default:
        TRACE_ERR("Unexpected logger service message!");
        ASSERT(false);
        break;
    }
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
