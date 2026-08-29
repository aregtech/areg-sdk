/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeController.hpp"

#if AREG_LOGGING

namespace areg {

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( areg::COOKIE_LOGGER
                                    , areg::RemoteServiceKind::Logger
                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                    , areg::MessageSource::SourceClient
                                    , static_cast<ConnectionConsumer &>(*this)
                                    , static_cast<RemoteMessageHandler &>(*this)
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer    ( )
    , RemoteMessageHandler  ( )

    , mScopeController  ( scopeController )
    , mFlags            ( NetTcpLogger::FLAG_NONE )
    , mRingStack        ( 0, areg::OverlapPolicy::Shift )
{
}

bool NetTcpLogger::open_logger()
{
    do
    {
        Lock lock(mLock);

        if (mClientConnection.is_valid())
            return is_connect_state();

        mFlags.store(NetTcpLogger::FLAG_NONE, std::memory_order_release);
        if (!mLogConfiguration.is_remote_logging_enabled())
        {
            mRingStack.release();
            return false;
        }

        register_consumer_commands();
        mRingStack.reserve(mLogConfiguration.stack_size());

        String host{ mLogConfiguration.remote_tcp_address() };
        uint16_t port{ mLogConfiguration.remote_tcp_port() };
        // A new connection is a new state: the source comes back enabled and never paused.
        mFlags.store(NetTcpLogger::FLAG_ENABLED, std::memory_order_release);
        apply_connection_data(host, port);
    } while (false);

    return connect_service_host();
}

void NetTcpLogger::close_logger()
{
    mRingStack.release();
    on_service_exit();
    unregister_consumer_commands();
}

void NetTcpLogger::log_message(const areg::LogEntry& logMessage)
{
    // A paused source drops what it produced. Nothing is queued, so nothing is replayed on resume.
    if (!is_sending())
        return;

    if (mChannel.is_valid() && is_connected_state())
    {
        send_message(areg::create_log_message(logMessage, areg::LogDataType::Remote, mChannel.cookie()), areg::EventPriority::NormalPrio);
    }
    else if (mRingStack.capacity() != 0)
    {
        mRingStack.push(areg::create_log_message(logMessage, areg::LogDataType::Remote, mChannel.cookie()));
    }
}

void NetTcpLogger::forward_message(const areg::MessageEnvelope& msg)
{
    // A paused source drops what it produced. Nothing is queued, so nothing is replayed on resume.
    if (!is_sending())
        return;

    if (mChannel.is_valid() && is_connected_state())
    {
        send_message(msg, areg::EventPriority::NormalPrio);
    }
    else if (mRingStack.capacity() != 0)
    {
        mRingStack.push(msg);
    }
}

void NetTcpLogger::forward_message(areg::MessageEnvelope&& msg)
{
    // A paused source drops what it produced. Nothing is queued, so nothing is replayed on resume.
    if (!is_sending())
        return;

    if (mChannel.is_valid() && is_connected_state())
    {
        send_message(std::move(msg), areg::EventPriority::NormalPrio);
    }
    else if (mRingStack.capacity() != 0)
    {
        mRingStack.push(std::move(msg));
    }
}

bool NetTcpLogger::is_logger_opened() const noexcept
{
    Lock lock( mLock );
    return is_connected_state();
}

void NetTcpLogger::on_service_channel_connected(const Channel & channel)
{
    ASSERT(channel.is_valid());
    ASSERT(channel.cookie() >= areg::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.is_valid());

    // A new connection is a new state: the source comes back enabled and never paused.
    mFlags.store(NetTcpLogger::FLAG_ENABLED, std::memory_order_release);
    const ITEM_ID& cookie = channel.cookie();

    const areg::ScopeList& scopes{ static_cast<const areg::ScopeList&>(mScopeController.scope_list()) };
    send_message(areg::message_register_scopes(cookie, areg::COOKIE_LOGGER, scopes));

    while (mRingStack.is_empty() == false)
    {
        areg::MessageEnvelope msgLog{ mRingStack.pop() };
        msgLog.set_source(static_cast<uint32_t>(cookie));
        reinterpret_cast<areg::LogEntry*>(msgLog.buffer())->logCookie = cookie;
        send_message(std::move(msgLog), areg::EventPriority::NormalPrio);
    }
}

void NetTcpLogger::on_service_channel_disconnected(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mFlags.store(NetTcpLogger::FLAG_NONE, std::memory_order_release);
    mClientConnection.set_cookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::on_service_channel_lost(const Channel & /* channel */)
{
    ASSERT(mChannel.is_valid() == false);
    mClientConnection.set_cookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::failed_send_message(const MessageEnvelope & msgFailed, Socket & /* whichTarget */)
{
    if (connection_state() == ConnectionPhase::ConnectionStopping)
        return;

    ASSERT(is_enabled());
    if (mLogConfiguration.stack_size() > 0)
    {
        mRingStack.push(msgFailed);
    }

    notify_connection_lost();
}

void NetTcpLogger::failed_receive_message(Socket & /* whichSource */)
{
    if (connection_state() == ConnectionPhase::ConnectionStopping)
        return;

    notify_connection_lost();
}

void NetTcpLogger::failed_process_message(const MessageEnvelope & /* msgUnprocessed */)
{
}

void NetTcpLogger::process_received_message(MessageEnvelope & msgReceived, Socket & whichSource)
{
    if (!msgReceived.is_valid() || !whichSource.is_valid())
        return;
    
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
            const ITEM_ID targetId{ static_cast<ITEM_ID>(msgReceived.source()) };
            send_message(areg::message_register_scopes(mChannel.cookie(), targetId, scopes));
        }
        break;

    case areg::FuncIdRange::ServiceSaveLogConfiguration:
        if (LogManager::save_log_config())
        {
            send_message(areg::message_configuration_saved());
        }
        break;

    case areg::FuncIdRange::ServiceLogRestoreConfiguration:
        {
            LogManager::restore_log_config();
            const areg::ScopeList& scopes{ static_cast<const areg::ScopeList&>(mScopeController.scope_list()) };
            send_message(areg::message_configuration_restored());
            // The observers hold the priorities, so they are sent the list the file restored.
            send_message(areg::message_scopes_updated(mChannel.cookie(), areg::COOKIE_LOGGER, scopes));
        }
        break;

    case areg::FuncIdRange::ServiceLogUpdateSourceState:
        {
            ITEM_ID  target{ areg::COOKIE_UNKNOWN };
            uint8_t  wanted{ static_cast<uint8_t>(areg::LogSourceState::Undefined) };
            msgReceived >> target;
            msgReceived >> wanted;

            // The byte arrives over the network, so only the two defined states are accepted.
            const areg::LogSourceState state{ static_cast<areg::LogSourceState>(wanted) };
            if ((state == areg::LogSourceState::Active) || (state == areg::LogSourceState::Paused))
            {
                const ITEM_ID byObserver{ static_cast<ITEM_ID>(msgReceived.source()) };
                set_paused(state == areg::LogSourceState::Paused);
                send_message(areg::message_source_state_updated(mChannel.cookie(), areg::COOKIE_LOGGER, state, byObserver));
            }
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
    case areg::FuncIdRange::ServiceLogSourceStateUpdated:     // fall through
    case areg::FuncIdRange::ServiceLogConfigurationRestored:  // fall through
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

} // namespace areg

#endif  // AREG_LOGGING
