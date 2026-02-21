/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/service/private/LogCollectorServerService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector connection service
 ************************************************************************/
#include "logcollector/service/LogCollectorServerService.hpp"

#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageReceived);
DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageSend);

//////////////////////////////////////////////////////////////////////////
// LogCollectorServerService class implementation
//////////////////////////////////////////////////////////////////////////

LogCollectorServerService::LogCollectorServerService()
    : ServiceCommunicationBase   ( areg::COOKIE_LOGGER
                                , areg::eRemoteServices::ServiceLogger
                                , static_cast<uint32_t>(areg::eConnectionTypes::ConnectTcpip)
                                , areg::SERVER_DISPATCH_MESSAGE_THREAD
                                , ServiceCommunicationBase::eConnectionBehavior::DefaultAccept )
    , TimerConsumer           ( )

    , mLoggerProcessor          ( self() )
    , mObservers                ( )
    , mSaveTimer                ( static_cast<TimerConsumer &>(self()), "ConfigSaveTimer", LogCollectorServerService::TIMEOUT_SAVE_CONFIG)
{
}

void LogCollectorServerService::addInstance(const ITEM_ID& cookie, const areg::sServiceConnectedInstance& instance)
{
    Lock lock(mLock);

    ServiceCommunicationBase::addInstance(cookie, instance);
    if (LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        areg::sLogMessage logMsgHello(areg::eLogMessageType::LogMessageText, 0u, 0u, 0u, areg::eLogPriority::PrioAny, nullptr, 0);
        String::formatString( logMsgHello.logMessage
                            , areg::LOG_MESSAGE_IZE
                            , "CONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        areg::logAnyMessageLocal(logMsgHello);

        mLoggerProcessor.notifyConnectedInstances(getInstances(), areg::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.addIfUnique(cookie, instance);
    }
}

void LogCollectorServerService::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);

    ArrayList<ITEM_ID> listIds;
    areg::sServiceConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicationBase::removeInstance(cookie);
   
    mLoggerProcessor.clientDisconnected(cookie);
    if (exists && LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        areg::sLogMessage logMsgBye(areg::eLogMessageType::LogMessageText, 0u, 0u, 0u, areg::eLogPriority::PrioAny, nullptr, 0);
        String::formatString(logMsgBye.logMessage
                            , areg::LOG_MESSAGE_IZE
                            , "DISCONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        areg::logAnyMessageLocal(logMsgBye);

        listIds.add(instance.ciCookie);
        mLoggerProcessor.notifyDisconnectedInstances(listIds, areg::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::isLogObserver(instance.ciSource))
    {
        mObservers.removeAt(cookie);
    }
}

void LogCollectorServerService::removeAllInstances()
{
    Lock lock(mLock);
    
    mSaveTimer.stopTimer();

    if (mInstanceMap.getSize() != 0)
    {
        ArrayList<ITEM_ID> listIds;
        for (const auto& entry : getInstances().getData())
        {
            if (LogCollectorMessageProcessor::isLogSource(entry.second.ciSource))
            {
                listIds.add(entry.second.ciCookie);
            }
        }

        areg::sLogMessage logMsgClose(areg::eLogMessageType::LogMessageText, 0u, 0u, 0u, areg::eLogPriority::PrioAny, nullptr, 0);
        String::formatString(logMsgClose.logMessage, areg::LOG_MESSAGE_IZE, "Disconnecting and removing [ %u ] instances.", mInstanceMap.getSize());
        areg::logAnyMessageLocal(logMsgClose);
        ServiceCommunicationBase::removeAllInstances();

        if (listIds.isEmpty() == false)
        {
            mLoggerProcessor.notifyDisconnectedInstances(listIds, areg::TARGET_ALL);
        }
    }

    mObservers.clear();
}

void LogCollectorServerService::dispatchAndForwardLoggerMessage(const RemoteMessage& msgForward)
{
    Lock lock(mLock);

    ASSERT(msgForward.isValid());
    ASSERT(msgForward.getSource() == areg::COOKIE_LOGGER);
    areg::eFuncIdRange msgId = static_cast<areg::eFuncIdRange>(msgForward.getMessageId());
    switch (msgId)
    {
    case areg::eFuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgForward);
        break;

    case areg::eFuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgForward);
        break;

    case areg::eFuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgForward);
        break;

    case areg::eFuncIdRange::EmptyFunctionId:                  // fall through
    case areg::eFuncIdRange::ComponentCleanup:                 // fall through
    case areg::eFuncIdRange::RequestRegisterService:           // fall through
    case areg::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::eFuncIdRange::SystemServiceConnect:             // fall through
    case areg::eFuncIdRange::SystemServiceDisconnect:          // fall through
    case areg::eFuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::eFuncIdRange::SystemServiceQueryInstances:      // fall through
    case areg::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
    case areg::eFuncIdRange::SystemServiceRequestRegister:     // fall through
    case areg::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
    case areg::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
    case areg::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case areg::eFuncIdRange::ServiceLogMessage:                // fall through
    case areg::eFuncIdRange::RequestFirstId:                   // fall through
    case areg::eFuncIdRange::ResponseFirstId:                  // fall through
    case areg::eFuncIdRange::AttributeFirstId:                 // fall through
    case areg::eFuncIdRange::RequestLastId:                    // fall through
    case areg::eFuncIdRange::ResponseLastId:                   // fall through
    case areg::eFuncIdRange::AttributeLastId:                  // fall through
    case areg::eFuncIdRange::ServiceLastId:                    // fall through
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
    areg::eFuncIdRange msgId = static_cast<areg::eFuncIdRange>( msgReceived.getMessageId() );

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , areg::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgReceived.getSource())
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch (msgId)
    {
    case areg::eFuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.queryConnectedInstances(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.registerScopesAtObserver(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogScopesUpdated:
        mLoggerProcessor.logSourceScopesUpadated(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogConfigurationSaved:
        mLoggerProcessor.logSourceConfigurationSaved(msgReceived);
        break;

    case areg::eFuncIdRange::ServiceLogMessage:
        mLoggerProcessor.logMessage(msgReceived);
        areg::logMessage(msgReceived);
        break;

    case areg::eFuncIdRange::SystemServiceConnect:
    case areg::eFuncIdRange::SystemServiceDisconnect:
        break;

    case areg::eFuncIdRange::SystemServiceNotifyInstances:

    case areg::eFuncIdRange::RequestRegisterService:           // fall through
    case areg::eFuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::eFuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::eFuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::eFuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::eFuncIdRange::SystemServiceRequestRegister:     // fall through
    case areg::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::eFuncIdRange::EmptyFunctionId:                  // fall through
    case areg::eFuncIdRange::ComponentCleanup:                 // fall through
    case areg::eFuncIdRange::RequestFirstId:                   // fall through
    case areg::eFuncIdRange::ResponseFirstId:                  // fall through
    case areg::eFuncIdRange::AttributeFirstId:                 // fall through
    case areg::eFuncIdRange::RequestLastId:                    // fall through
    case areg::eFuncIdRange::ResponseLastId:                   // fall through
    case areg::eFuncIdRange::AttributeLastId:                  // fall through
    case areg::eFuncIdRange::ServiceLastId:                    // fall through
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

    areg::eFuncIdRange msgId = static_cast<areg::eFuncIdRange>( msgSend.getMessageId() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , areg::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( areg::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != areg::TARGET_UNKNOWN )
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

void LogCollectorServerService::onServiceConnectionStarted()
{
}

void LogCollectorServerService::onServiceConnectionStopped()
{
}

void LogCollectorServerService::onServiceConnectionLost()
{
}
