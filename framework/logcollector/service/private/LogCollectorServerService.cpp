/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logcollector/service/private/LogCollectorServerService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector connection service
 ************************************************************************/
#include "logcollector/service/LogCollectorServerService.hpp"

#include "areg/ipc/private/NEConnection.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageReceived);
DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageSend);

//////////////////////////////////////////////////////////////////////////
// LogCollectorServerService class implementation
//////////////////////////////////////////////////////////////////////////

LogCollectorServerService::LogCollectorServerService( void )
    : ServiceCommunicatonBase   ( NEService::COOKIE_LOGGER
                                , NERemoteService::eRemoteServices::ServiceLogger
                                , static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)
                                , NEConnection::SERVER_DISPATCH_MESSAGE_THREAD
                                , ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept )
    , IETimerConsumer           ( )

    , mLoggerProcessor          ( self() )
    , mObservers                ( )
    , mSaveTimer                ( static_cast<IETimerConsumer &>(self()), "ConfigSaveTimer", LogCollectorServerService::TIMEOUT_SAVE_CONFIG)
{
}

void LogCollectorServerService::addInstance(const ITEM_ID& cookie, const NEService::sServiceConnectedInstance& instance)
{
    Lock lock(mLock);

    ServiceCommunicatonBase::addInstance(cookie, instance);
    if (LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        NELogging::sLogMessage logMsgHello(NELogging::eLogMessageType::LogMessageText, 0, NELogging::eLogPriority::PrioAny, nullptr, 0);
        String::formatString( logMsgHello.logMessage
                            , NELogging::LOG_MESSAGE_IZE
                            , "CONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        NELogging::logAnyMessageLocal(logMsgHello);

        mLoggerProcessor.notifyConnectedInstances(getInstances(), NEService::COOKIE_ANY);
    }
    else if (LogCollectorMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.addIfUnique(cookie, instance);
    }
}

void LogCollectorServerService::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);

    TEArrayList<ITEM_ID> listIds;
    NEService::sServiceConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicatonBase::removeInstance(cookie);
   
    mLoggerProcessor.clientDisconnected(cookie);
    if (exists && LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        NELogging::sLogMessage logMsgBye(NELogging::eLogMessageType::LogMessageText, 0, NELogging::eLogPriority::PrioAny, nullptr, 0);
        String::formatString(logMsgBye.logMessage
                            , NELogging::LOG_MESSAGE_IZE
                            , "DISCONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        NELogging::logAnyMessageLocal(logMsgBye);

        listIds.add(instance.ciCookie);
        mLoggerProcessor.notifyDisconnectedInstances(listIds, NEService::COOKIE_ANY);
    }
    else if (LogCollectorMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.removeAt(cookie);
    }
}

void LogCollectorServerService::removeAllInstances(void)
{
    Lock lock(mLock);
    
    mSaveTimer.stopTimer();

    if (mInstanceMap.getSize() != 0)
    {
        TEArrayList<ITEM_ID> listIds;
        for (const auto& entry : getInstances().getData())
        {
            if (LogCollectorMessageProcessor::isLogSource(entry.second.ciSource))
            {
                listIds.add(entry.second.ciCookie);
            }
        }

        NELogging::sLogMessage logMsgClose(NELogging::eLogMessageType::LogMessageText, 0, NELogging::eLogPriority::PrioAny, nullptr, 0);
        String::formatString(logMsgClose.logMessage, NELogging::LOG_MESSAGE_IZE, "Disconnecting and removing [ %u ] instances.", mInstanceMap.getSize());
        NELogging::logAnyMessageLocal(logMsgClose);
        ServiceCommunicatonBase::removeAllInstances();

        if (listIds.isEmpty() == false)
        {
            mLoggerProcessor.notifyDisconnectedInstances(listIds, NEService::COOKIE_ANY);
        }
    }

    mObservers.clear();
}

void LogCollectorServerService::dispatchAndForwardLoggerMessage(const RemoteMessage& msgForward)
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

    case NEService::eFuncIdRange::EmptyFunctionId:                  // fall through
    case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
    case NEService::eFuncIdRange::RequestRegisterService:           // fall through
    case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::eFuncIdRange::SystemServiceConnect:             // fall through
    case NEService::eFuncIdRange::SystemServiceDisconnect:          // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::eFuncIdRange::SystemServiceQueryInstances:      // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
    case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
    case NEService::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
    case NEService::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case NEService::eFuncIdRange::ServiceLogMessage:                // fall through
    case NEService::eFuncIdRange::RequestFirstId:                   // fall through
    case NEService::eFuncIdRange::ResponseFirstId:                  // fall through
    case NEService::eFuncIdRange::AttributeFirstId:                 // fall through
    case NEService::eFuncIdRange::RequestLastId:                    // fall through
    case NEService::eFuncIdRange::ResponseLastId:                   // fall through
    case NEService::eFuncIdRange::AttributeLastId:                  // fall through
    case NEService::eFuncIdRange::ServiceLastId:                    // fall through
    default:
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageReceived);

    Lock lock(mLock);
    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
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

    case NEService::eFuncIdRange::ServiceLogScopesUpdated:
        mLoggerProcessor.logSourceScopesUpadated(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogConfigurationSaved:
        mLoggerProcessor.logSourceConfigurationSaved(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogMessage:
        mLoggerProcessor.logMessage(msgReceived);
        NELogging::logMessage(msgReceived);
        break;

    case NEService::eFuncIdRange::SystemServiceConnect:
    case NEService::eFuncIdRange::SystemServiceDisconnect:
        break;

    case NEService::eFuncIdRange::SystemServiceNotifyInstances:

    case NEService::eFuncIdRange::RequestRegisterService:           // fall through
    case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::eFuncIdRange::EmptyFunctionId:                  // fall through
    case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
    case NEService::eFuncIdRange::RequestFirstId:                   // fall through
    case NEService::eFuncIdRange::ResponseFirstId:                  // fall through
    case NEService::eFuncIdRange::AttributeFirstId:                 // fall through
    case NEService::eFuncIdRange::RequestLastId:                    // fall through
    case NEService::eFuncIdRange::ResponseLastId:                   // fall through
    case NEService::eFuncIdRange::AttributeLastId:                  // fall through
    case NEService::eFuncIdRange::ServiceLastId:                    // fall through
    default:
        LOG_ERR("Unexpected Log Collector service message!");
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::processTimer(Timer& /* timer */ )
{
}

void LogCollectorServerService::onServiceMessageSend(const RemoteMessage &msgSend)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageSend);

    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
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
        LOG_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void LogCollectorServerService::connectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LogCollectorServerService::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LogCollectorServerService::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void LogCollectorServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{
}

void LogCollectorServerService::onServiceConnectionStarted( void )
{
}

void LogCollectorServerService::onServiceConnectionStopped( void )
{
}

void LogCollectorServerService::onServiceConnectionLost( void )
{
}
