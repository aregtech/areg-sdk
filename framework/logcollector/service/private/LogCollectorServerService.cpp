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
                                , areg::RemoteServiceKind::Logger
                                , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                , areg::SERVER_DISPATCH_MESSAGE_THREAD
                                , ServiceCommunicationBase::ConnectionPolicy::Accept )
    , areg::TimerConsumer           ( )

    , mLoggerProcessor          ( self() )
    , mObservers                ( )
    , mSaveTimer                ( static_cast<areg::TimerConsumer &>(self()), "ConfigSaveTimer", LogCollectorServerService::TIMEOUT_SAVE_CONFIG)
{
}

void LogCollectorServerService::addInstance(const ITEM_ID& cookie, const areg::ConnectedInstance& instance)
{
    areg::Lock lock(mLock);

    ServiceCommunicationBase::addInstance(cookie, instance);
    if (LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::formatString( logMsgHello.logMessage
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
    areg::Lock lock(mLock);

    areg::ArrayList<ITEM_ID> listIds;
    areg::ConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicationBase::removeInstance(cookie);
   
    mLoggerProcessor.clientDisconnected(cookie);
    if (exists && LogCollectorMessageProcessor::isLogSource(instance.ciSource))
    {
        areg::LogEntry logMsgBye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::formatString(logMsgBye.logMessage
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
    areg::Lock lock(mLock);
    
    mSaveTimer.stopTimer();

    if (mInstanceMap.getSize() != 0)
    {
        areg::ArrayList<ITEM_ID> listIds;
        for (const auto& entry : getInstances().getData())
        {
            if (LogCollectorMessageProcessor::isLogSource(entry.second.ciSource))
            {
                listIds.add(entry.second.ciCookie);
            }
        }

        areg::LogEntry logMsgClose(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::formatString(logMsgClose.logMessage, areg::LOG_MESSAGE_IZE, "Disconnecting and removing [ %u ] instances.", mInstanceMap.getSize());
        areg::logAnyMessageLocal(logMsgClose);
        ServiceCommunicationBase::removeAllInstances();

        if (listIds.isEmpty() == false)
        {
            mLoggerProcessor.notifyDisconnectedInstances(listIds, areg::TARGET_ALL);
        }
    }

    mObservers.clear();
}

void LogCollectorServerService::dispatchAndForwardLoggerMessage(const areg::RemoteMessage& msgForward)
{
    areg::Lock lock(mLock);

    ASSERT(msgForward.isValid());
    ASSERT(msgForward.getSource() == areg::COOKIE_LOGGER);
    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgForward.getMessageId());
    switch (msgId)
    {
    case areg::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgForward);
        break;

    case areg::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgForward);
        break;

    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgForward);
        break;

    case areg::FuncIdRange::EmptyFunctionId:                  // fall through
    case areg::FuncIdRange::ComponentCleanup:                 // fall through
    case areg::FuncIdRange::RequestRegisterService:           // fall through
    case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::FuncIdRange::SystemServiceConnect:             // fall through
    case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
    case areg::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
    case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case areg::FuncIdRange::ServiceLogMessage:                // fall through
    case areg::FuncIdRange::RequestFirstId:                   // fall through
    case areg::FuncIdRange::ResponseFirstId:                  // fall through
    case areg::FuncIdRange::AttributeFirstId:                 // fall through
    case areg::FuncIdRange::RequestLastId:                    // fall through
    case areg::FuncIdRange::ResponseLastId:                   // fall through
    case areg::FuncIdRange::AttributeLastId:                  // fall through
    case areg::FuncIdRange::ServiceLastId:                    // fall through
    default:
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::onServiceMessageReceived(const areg::RemoteMessage &msgReceived)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageReceived);

    areg::Lock lock(mLock);
    ASSERT( msgReceived.isValid() );
    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgReceived.getMessageId() );

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , areg::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgReceived.getSource())
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch (msgId)
    {
    case areg::FuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.queryConnectedInstances(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.registerScopesAtObserver(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateLogSourceScopes(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryLogSourceScopes(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogScopesUpdated:
        mLoggerProcessor.logSourceScopesUpadated(msgReceived);
        break;

    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.saveLogSourceConfiguration(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogConfigurationSaved:
        mLoggerProcessor.logSourceConfigurationSaved(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogMessage:
        mLoggerProcessor.logMessage(msgReceived);
        areg::logMessage(msgReceived);
        break;

    case areg::FuncIdRange::SystemServiceConnect:
    case areg::FuncIdRange::SystemServiceDisconnect:
        break;

    case areg::FuncIdRange::SystemServiceNotifyInstances:

    case areg::FuncIdRange::RequestRegisterService:           // fall through
    case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
    case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case areg::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
    case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case areg::FuncIdRange::EmptyFunctionId:                  // fall through
    case areg::FuncIdRange::ComponentCleanup:                 // fall through
    case areg::FuncIdRange::RequestFirstId:                   // fall through
    case areg::FuncIdRange::ResponseFirstId:                  // fall through
    case areg::FuncIdRange::AttributeFirstId:                 // fall through
    case areg::FuncIdRange::RequestLastId:                    // fall through
    case areg::FuncIdRange::ResponseLastId:                   // fall through
    case areg::FuncIdRange::AttributeLastId:                  // fall through
    case areg::FuncIdRange::ServiceLastId:                    // fall through
    default:
        LOG_ERR("Unexpected Log Collector service message!");
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::processTimer(areg::Timer& /* timer */ )
{
}

void LogCollectorServerService::onServiceMessageSend(const areg::RemoteMessage &msgSend)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageSend);

    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgSend.getMessageId() );
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

void LogCollectorServerService::connectedRemoteServiceChannel(const areg::Channel & /* channel */)
{

}

void LogCollectorServerService::disconnectedRemoteServiceChannel(const areg::Channel & /* channel */)
{

}

void LogCollectorServerService::lostRemoteServiceChannel(const areg::Channel & /* channel */)
{
}

void LogCollectorServerService::failedProcessMessage(const areg::RemoteMessage & /* msgUnprocessed */)
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
