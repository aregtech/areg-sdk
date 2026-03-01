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

namespace areg {

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( areg::COOKIE_LOGGER
                                    , areg::RemoteServiceKind::Logger
                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                    , areg::MessageSource::SourceClient
                                    , static_cast<ConnectionConsumer &>(self())
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer   ( )
    , RemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, areg::OverlapPolicy::Shift )
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

void NetTcpLogger::log_message(const areg::LogEntry& logMessage)
{
    if (mIsEnabled)
    {
        if (mChannel.is_valid() && is_connect_state())
        {
            send_message(areg::create_log_message(logMessage, areg::LogDataType::Remote, mChannel.cookie()), Event::EventPriority::NormalPrio);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(areg::create_log_message(logMessage, areg::LogDataType::Remote, mChannel.cookie()));
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
    ASSERT(channel.cookie() >= areg::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.is_valid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.cookie();
    while (mRingStack.is_empty() == false)
    {
        RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.set_source(cookie);
        reinterpret_cast<areg::LogEntry*>(msgLog.buffer())->logCookie = cookie;
        send_message(msgLog, Event::EventPriority::NormalPrio);
    }
}

void NetTcpLogger::on_service_channel_disconnected(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mIsEnabled = false;
    mClientConnection.set_cookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::on_service_channel_lost(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mClientConnection.set_cookie(areg::COOKIE_UNKNOWN);
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
        areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgReceived.message_id());
        switch (msgId)
        {
        case areg::FuncIdRange::SystemServiceNotifyConnection:
            service_connection_event(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                areg::ScopeEntry scopeInfo{};
                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeInfo;
                    LogManager::update_scopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                }

                if (scopeCount != 0)
                {
                    const areg::ScopeList& scopes{ static_cast<const areg::ScopeList&>(mScopeController.scope_list()) };
                    send_message(areg::message_scopes_updated(mChannel.cookie(), areg::COOKIE_LOGGER, scopes));
                }
            }
            break;

        case areg::FuncIdRange::ServiceLogQueryScopes:
            {
                const areg::ScopeList & scopes{ static_cast<const areg::ScopeList &>(mScopeController.scope_list()) };
                const ITEM_ID & targetId{ msgReceived.source() };
                send_message(areg::message_register_scopes(mChannel.cookie(), targetId, scopes));
            }
            break;

        case areg::FuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::save_log_config())
            {
                send_message(areg::message_configuration_saved());
            }
            break;

        case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
        case areg::FuncIdRange::ServiceLastId:                    // fall through
        case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::FuncIdRange::SystemServiceConnect:             // fall through
        case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::FuncIdRange::RequestRegisterService:           // fall through
        case areg::FuncIdRange::ComponentCleanup:                 // fall through
        case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::FuncIdRange::ServiceLogMessage:                // fall through
        case areg::FuncIdRange::AttributeLastId:                  // fall through
        case areg::FuncIdRange::AttributeFirstId:                 // fall through
        case areg::FuncIdRange::ResponseLastId:                   // fall through
        case areg::FuncIdRange::ResponseFirstId:                  // fall through
        case areg::FuncIdRange::RequestLastId:                    // fall through
        case areg::FuncIdRange::RequestFirstId:                   // fall through
        case areg::FuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

} // namespace areg

#endif  // AREG_LOGS
