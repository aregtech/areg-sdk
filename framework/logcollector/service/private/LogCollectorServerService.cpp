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
    : ServiceCommunicationBase   ( NEService::COOKIE_LOGGER
                                , NERemoteService::RemoteServiceKind::Logger
                                , static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)
                                , NEConnection::SERVER_DISPATCH_MESSAGE_THREAD
                                , ServiceCommunicationBase::ConnectionPolicy::Accept )
    , TimerConsumer           ( )

    , mLoggerProcessor          ( self() )
    , mObservers                ( )
    , mSaveTimer                ( static_cast<TimerConsumer &>(self()), "ConfigSaveTimer", LogCollectorServerService::TIMEOUT_SAVE_CONFIG)
{
}

void LogCollectorServerService::add_instance(const ITEM_ID& cookie, const NEService::ConnectedInstance& instance)
{
    Lock lock(mLock);

    ServiceCommunicationBase::add_instance(cookie, instance);
    if (LogCollectorMessageProcessor::is_log_source(instance.ciSource))
    {
        NELogging::LogEntry logMsgHello(NELogging::LogMessageType::MessageText, 0u, 0u, 0u, NELogging::LogPriority::PrioAny, nullptr, 0);
        String::format_string( logMsgHello.log_message
                            , NELogging::LOG_MESSAGE_IZE
                            , "CONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        NELogging::log_local(logMsgHello);

        mLoggerProcessor.notify_connected_instances(getInstances(), NEService::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::is_log_observer(instance.ciSource))
    {
        mObservers.add_if_unique(cookie, instance);
    }
}

void LogCollectorServerService::remove_instance(const ITEM_ID & cookie)
{
    Lock lock(mLock);

    ArrayList<ITEM_ID> listIds;
    NEService::ConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicationBase::remove_instance(cookie);
   
    mLoggerProcessor.client_disconnected(cookie);
    if (exists && LogCollectorMessageProcessor::is_log_source(instance.ciSource))
    {
        NELogging::LogEntry logMsgBye(NELogging::LogMessageType::MessageText, 0u, 0u, 0u, NELogging::LogPriority::PrioAny, nullptr, 0);
        String::format_string(logMsgBye.log_message
                            , NELogging::LOG_MESSAGE_IZE
                            , "DISCONNECTED the %u-bit instance [ %s ] with cookie [ %llu ] and location [ %s ]"
                            , static_cast<uint32_t>(instance.ciBitness)
                            , instance.ciInstance.c_str()
                            , instance.ciCookie
                            , instance.ciLocation.c_str());
        NELogging::log_local(logMsgBye);

        listIds.add(instance.ciCookie);
        mLoggerProcessor.notify_disconnected_instances(listIds, NEService::TARGET_ALL);
    }
    else if (LogCollectorMessageProcessor::is_log_observer(instance.ciSource))
    {
        mObservers.remove_at(cookie);
    }
}

void LogCollectorServerService::remove_all_instances()
{
    Lock lock(mLock);
    
    mSaveTimer.stop_timer();

    if (mInstanceMap.size() != 0)
    {
        ArrayList<ITEM_ID> listIds;
        for (const auto& entry : getInstances().data())
        {
            if (LogCollectorMessageProcessor::is_log_source(entry.second.ciSource))
            {
                listIds.add(entry.second.ciCookie);
            }
        }

        NELogging::LogEntry logMsgClose(NELogging::LogMessageType::MessageText, 0u, 0u, 0u, NELogging::LogPriority::PrioAny, nullptr, 0);
        String::format_string(logMsgClose.log_message, NELogging::LOG_MESSAGE_IZE, "Disconnecting and removing [ %u ] instances.", mInstanceMap.size());
        NELogging::log_local(logMsgClose);
        ServiceCommunicationBase::remove_all_instances();

        if (listIds.is_empty() == false)
        {
            mLoggerProcessor.notify_disconnected_instances(listIds, NEService::TARGET_ALL);
        }
    }

    mObservers.clear();
}

void LogCollectorServerService::dispatch_and_forward_logger_message(const RemoteMessage& msgForward)
{
    Lock lock(mLock);

    ASSERT(msgForward.is_valid());
    ASSERT(msgForward.source() == NEService::COOKIE_LOGGER);
    NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>(msgForward.message_id());
    switch (msgId)
    {
    case NEService::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.update_log_source_scopes(msgForward);
        break;

    case NEService::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.query_log_source_scopes(msgForward);
        break;

    case NEService::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.save_log_source_configuration(msgForward);
        break;

    case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
    case NEService::FuncIdRange::ComponentCleanup:                 // fall through
    case NEService::FuncIdRange::RequestRegisterService:           // fall through
    case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::FuncIdRange::SystemServiceConnect:             // fall through
    case NEService::FuncIdRange::SystemServiceDisconnect:          // fall through
    case NEService::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
    case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
    case NEService::FuncIdRange::SystemServiceRequestRegister:     // fall through
    case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
    case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
    case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
    case NEService::FuncIdRange::ServiceLogMessage:                // fall through
    case NEService::FuncIdRange::RequestFirstId:                   // fall through
    case NEService::FuncIdRange::ResponseFirstId:                  // fall through
    case NEService::FuncIdRange::AttributeFirstId:                 // fall through
    case NEService::FuncIdRange::RequestLastId:                    // fall through
    case NEService::FuncIdRange::ResponseLastId:                   // fall through
    case NEService::FuncIdRange::AttributeLastId:                  // fall through
    case NEService::FuncIdRange::ServiceLastId:                    // fall through
    default:
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::on_message_received(const RemoteMessage &msgReceived)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageReceived);

    Lock lock(mLock);
    ASSERT( msgReceived.is_valid() );
    NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>( msgReceived.message_id() );

    LOG_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgReceived.source())
                    , static_cast<uint32_t>(msgReceived.target()));

    switch (msgId)
    {
    case NEService::FuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.query_connected_instances(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.register_scopes_at_observer(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.update_log_source_scopes(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.query_log_source_scopes(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogScopesUpdated:
        mLoggerProcessor.log_source_scopes_updated(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceSaveLogConfiguration:
        mLoggerProcessor.save_log_source_configuration(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogConfigurationSaved:
        mLoggerProcessor.log_source_configuration_saved(msgReceived);
        break;

    case NEService::FuncIdRange::ServiceLogMessage:
        mLoggerProcessor.log_message(msgReceived);
        NELogging::log_message(msgReceived);
        break;

    case NEService::FuncIdRange::SystemServiceConnect:
    case NEService::FuncIdRange::SystemServiceDisconnect:
        break;

    case NEService::FuncIdRange::SystemServiceNotifyInstances:

    case NEService::FuncIdRange::RequestRegisterService:           // fall through
    case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
    case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
    case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
    case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
    case NEService::FuncIdRange::SystemServiceNotifyConnection:    // fall through
    case NEService::FuncIdRange::SystemServiceRequestRegister:     // fall through
    case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
    case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
    case NEService::FuncIdRange::ComponentCleanup:                 // fall through
    case NEService::FuncIdRange::RequestFirstId:                   // fall through
    case NEService::FuncIdRange::ResponseFirstId:                  // fall through
    case NEService::FuncIdRange::AttributeFirstId:                 // fall through
    case NEService::FuncIdRange::RequestLastId:                    // fall through
    case NEService::FuncIdRange::ResponseLastId:                   // fall through
    case NEService::FuncIdRange::AttributeLastId:                  // fall through
    case NEService::FuncIdRange::ServiceLastId:                    // fall through
    default:
        LOG_ERR("Unexpected Log Collector service message!");
        ASSERT(false);
        break;
    }
}

void LogCollectorServerService::process_timer(Timer& /* timer */ )
{
}

void LogCollectorServerService::on_message_send(const RemoteMessage &msgSend)
{
    LOG_SCOPE(logcollector_service_LogCollectorServerService_onServiceMessageSend);

    NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>( msgSend.message_id() );
    LOG_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::as_string(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.target())
                    , static_cast<uint32_t>(msgSend.source()));

    if ( NEService::is_executable_id( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.target( ) != NEService::TARGET_UNKNOWN )
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

void LogCollectorServerService::on_service_channel_connected(const Channel & /* channel */)
{

}

void LogCollectorServerService::on_service_channel_disconnected(const Channel & /* channel */)
{

}

void LogCollectorServerService::on_service_channel_lost(const Channel & /* channel */)
{
}

void LogCollectorServerService::failed_process_message(const RemoteMessage & /* msgUnprocessed */)
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
