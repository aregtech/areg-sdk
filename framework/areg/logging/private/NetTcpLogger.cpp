/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/NetTcpLogger.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, TCP/IP logging object to log message into the
 *              remote object.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/NetTcpLogger.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeController.hpp"

#if AREG_LOGS

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( NEService::COOKIE_LOGGER
                                    , NERemoteService::RemoteServiceKind::Logger
                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)
                                    , NEService::MessageSource::SourceClient
                                    , static_cast<ConnectionConsumer &>(self())
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer   ( )
    , RemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, NECommon::OverlapPolicy::Shift )
{
}

bool NetTcpLogger::open_logger()
{
    Lock lock( mLock );
    bool result{ false };

    if (mClientConnection.is_valid() == false)
    {
        mIsEnabled = false;
        if (mLogConfiguration.is_remote_logging_enabled())
        {
            register_client_commands();
            mRingStack.reserve(mLogConfiguration.stack_size());

            String host{ mLogConfiguration.remote_tcp_address()};
            uint16_t port{ mLogConfiguration.remote_tcp_port() };
            mIsEnabled = true;
            apply_connection_data(host, port);
            mLock.unlock();
            result = connect_service_host();
        }
        else
        {
            mRingStack.release();
        }
    }
    else
    {
        result = is_connect_state();
    }

    return result;
}

void NetTcpLogger::close_logger()
{
    mRingStack.release();
    on_service_exit();
    unregister_client_commands();
}

void NetTcpLogger::log_message(const NELogging::LogEntry& log_message)
{
    if (mIsEnabled)
    {
        if (mChannel.is_valid() && is_connect_state())
        {
            send_message(NELogging::create_log_message(log_message, NELogging::LogDataType::Remote, mChannel.cookie()), Event::EventPriority::NormalPrio);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(NELogging::create_log_message(log_message, NELogging::LogDataType::Remote, mChannel.cookie()));
        }
    }
}

bool NetTcpLogger::is_logger_opened() const
{
    Lock lock( mLock );
    return is_connected_state();
}

void NetTcpLogger::on_service_channel_connected(const Channel & channel)
{
    ASSERT(channel.is_valid());
    ASSERT(channel.cookie() >= NEService::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.is_valid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.cookie();
    while (mRingStack.is_empty() == false)
    {
        RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.set_source(cookie);
        reinterpret_cast<NELogging::LogEntry*>(msgLog.buffer())->logCookie = cookie;
        send_message(msgLog, Event::EventPriority::NormalPrio);
    }
}

void NetTcpLogger::on_service_channel_disconnected(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mIsEnabled = false;
    mClientConnection.set_cookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::on_service_channel_lost(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mClientConnection.set_cookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::failed_send_message(const RemoteMessage & msgFailed, Socket & /* whichTarget */)
{
    ASSERT(mIsEnabled);
    if (mLogConfiguration.stack_size() > 0)
    {
        mRingStack.push(msgFailed);
    }

    send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failed_receive_message(Socket & /* whichSource */)
{
    send_command(ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failed_process_message(const RemoteMessage & /* msgUnprocessed */)
{
}

void NetTcpLogger::process_received_message(const RemoteMessage & msgReceived, Socket & whichSource)
{
    if (msgReceived.is_valid() && whichSource.is_valid())
    {
        NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>(msgReceived.message_id());
        switch (msgId)
        {
        case NEService::FuncIdRange::SystemServiceNotifyConnection:
            service_connection_event(msgReceived);
            break;

        case NEService::FuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                NELogging::ScopeEntry scopeInfo{};
                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeInfo;
                    LogManager::update_scopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                }

                if (scopeCount != 0)
                {
                    const NELogging::ScopeList& scopes{ static_cast<const NELogging::ScopeList&>(mScopeController.scope_list()) };
                    send_message(NELogging::message_scopes_updated(mChannel.cookie(), NEService::COOKIE_LOGGER, scopes));
                }
            }
            break;

        case NEService::FuncIdRange::ServiceLogQueryScopes:
            {
                const NELogging::ScopeList & scopes{ static_cast<const NELogging::ScopeList &>(mScopeController.scope_list()) };
                const ITEM_ID & targetId{ msgReceived.source() };
                send_message(NELogging::message_register_scopes(mChannel.cookie(), targetId, scopes));
            }
            break;

        case NEService::FuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::save_log_config())
            {
                send_message(NELogging::message_configuration_saved());
            }
            break;

        case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
        case NEService::FuncIdRange::ServiceLastId:                    // fall through
        case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::FuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::FuncIdRange::SystemServiceConnect:             // fall through
        case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::FuncIdRange::RequestRegisterService:           // fall through
        case NEService::FuncIdRange::ComponentCleanup:                 // fall through
        case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::FuncIdRange::ServiceLogMessage:                // fall through
        case NEService::FuncIdRange::AttributeLastId:                  // fall through
        case NEService::FuncIdRange::AttributeFirstId:                 // fall through
        case NEService::FuncIdRange::ResponseLastId:                   // fall through
        case NEService::FuncIdRange::ResponseFirstId:                  // fall through
        case NEService::FuncIdRange::RequestLastId:                    // fall through
        case NEService::FuncIdRange::RequestFirstId:                   // fall through
        case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

#endif  // AREG_LOGS
