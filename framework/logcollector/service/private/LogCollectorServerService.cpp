/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/service/private/LogCollectorServerService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector connection service
 ************************************************************************/
#include "logcollector/service/LogCollectorServerService.hpp"

#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService, on_message_received);
DEF_LOG_SCOPE(logcollector_service_LogCollectorServerService, on_message_send);

//////////////////////////////////////////////////////////////////////////
// LogCollectorServerService class implementation
//////////////////////////////////////////////////////////////////////////

LogCollectorServerService::LogCollectorServerService()
    : areg::ext::ServiceCommunicationBase   ( areg::COOKIE_LOGGER
                                            , areg::RemoteServiceKind::Logger
                                            , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                            , areg::SYSTEM_THREAD_STACK_NORMAL
                                            , areg::SERVER_DISPATCH_MESSAGE_THREAD
                                            , areg::ext::ServiceCommunicationBase::ConnectionPolicy::Accept )
    , areg::TimerConsumer       ( )

    , mLoggerProcessor          ( self() )
    , mObservers                ( )
    , mSaveTimer                ( static_cast<areg::TimerConsumer &>(self()), "ConfigSaveTimer", LogCollectorServerService::TIMEOUT_SAVE_CONFIG)
{
}

void LogCollectorServerService::add_instance(const ITEM_ID& cookie, const areg::ConnectedInstance& instance)
{
    areg::Lock lock(mLock);

    areg::ext::ServiceCommunicationBase::add_instance(cookie, instance);
    if (LogCollectorMessageProcessor::is_log_source(instance.ciSource))
    {
        areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::format_string( logMsgHello.logMessage
                            , areg::LOG_MSG_SIZE
                            , "CONNECTED the %u-bit instance [ %s ] with cookie [ %u ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        areg::log_local(logMsgHello);

        mLoggerProcessor.notify_connected_instances(instances(), areg::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::is_log_observer(instance.ciSource))
    {
        mObservers.add_if_unique(cookie, instance);
    }
}

void LogCollectorServerService::remove_instance(const ITEM_ID & cookie)
{
    areg::Lock lock(mLock);

    areg::ArrayList<ITEM_ID> listIds;
    areg::ConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    areg::ext::ServiceCommunicationBase::remove_instance(cookie);
   
    mLoggerProcessor.client_disconnected(cookie);
    if (exists && LogCollectorMessageProcessor::is_log_source(instance.ciSource))
    {
        areg::LogEntry logMsgBye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::format_string(logMsgBye.logMessage
                            , areg::LOG_MSG_SIZE
                            , "DISCONNECTED the %u-bit instance [ %s ] with cookie [ %u ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        areg::log_local(logMsgBye);

        listIds.add(instance.ciCookie);
        mLoggerProcessor.notify_disconnected_instances(listIds, areg::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::is_log_observer(instance.ciSource))
    {
        mObservers.remove_at(cookie);
    }
}

void LogCollectorServerService::remove_all_instances()
{
    areg::Lock lock(mLock);

    mSaveTimer.stop_timer();

    if (mInstanceMap.size() != 0)
    {
        areg::ArrayList<ITEM_ID> listIds;
        for (const auto& entry : instances().data())
        {
            if (LogCollectorMessageProcessor::is_log_source(entry.second.ciSource))
            {
                listIds.add(entry.second.ciCookie);
            }
        }

        areg::LogEntry logMsgClose(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioAny, nullptr, 0);
        areg::String::format_string(logMsgClose.logMessage, areg::LOG_MSG_SIZE, "Disconnecting and removing [ %u ] instances.", mInstanceMap.size());
        areg::log_local(logMsgClose);
        areg::ext::ServiceCommunicationBase::remove_all_instances();

        if (listIds.is_empty() == false)
        {
            mLoggerProcessor.notify_disconnected_instances(listIds, areg::TARGET_ALL);
        }
    }

    mObservers.clear();
}

void LogCollectorServerService::dispatch_and_forward_logger_message(const areg::MessageEnvelope& msgForward)
{
    areg::Lock lock(mLock);

    ASSERT(msgForward.is_valid());
    ASSERT(msgForward.source() == static_cast<uint32_t>(areg::COOKIE_LOGGER));
    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgForward.message_id());
    switch (msgId)
    {
    case areg::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.update_log_source_scopes(msgForward);
        break;

    case areg::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.query_log_source_scopes(msgForward);
        break;

    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.save_log_source_configuration(msgForward);
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

void LogCollectorServerService::on_message_received(const areg::MessageEnvelope &msgReceived)
{
    LOG_SCOPE( logcollector_service_LogCollectorServerService, on_message_received );

    areg::Lock lock(mLock);
    ASSERT( msgReceived.is_valid() );
    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgReceived.message_id() );

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , msgReceived.source()
                    , msgReceived.target());

    switch (msgId)
    {
    case areg::FuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.query_connected_instances(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.register_scopes_at_observer(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.update_log_source_scopes(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.query_log_source_scopes(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogScopesUpdated:
        mLoggerProcessor.log_source_scopes_updated(msgReceived);
        break;

    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.save_log_source_configuration(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogConfigurationSaved:
        mLoggerProcessor.log_source_configuration_saved(msgReceived);
        break;

    case areg::FuncIdRange::ServiceLogMessage:
        mLoggerProcessor.log_message(msgReceived);
        areg::log_message(msgReceived);
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

void LogCollectorServerService::process_timer(areg::Timer& /* timer */ )
{
}

void LogCollectorServerService::on_message_send(const areg::MessageEnvelope &msgSend)
{
    LOG_SCOPE( logcollector_service_LogCollectorServerService, on_message_send );

    areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>( msgSend.message_id() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , areg::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , msgSend.target()
                    , msgSend.source());

    if ( areg::is_executable_id( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.target( ) != static_cast<uint32_t>(areg::TARGET_UNKNOWN) )
        {
            send_message( msgSend );
        }
    }
    else
    {
        LOG_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void LogCollectorServerService::on_service_channel_connected(const areg::Channel & /* channel */)
{

}

void LogCollectorServerService::on_service_channel_disconnected(const areg::Channel & /* channel */)
{

}

void LogCollectorServerService::on_service_channel_lost(const areg::Channel & /* channel */)
{
}

void LogCollectorServerService::failed_process_message(const areg::MessageEnvelope & /* msgUnprocessed */)
{
}

void LogCollectorServerService::on_connection_started()
{
}

void LogCollectorServerService::on_connection_stopped()
{
}

void LogCollectorServerService::on_connection_lost()
{
}
