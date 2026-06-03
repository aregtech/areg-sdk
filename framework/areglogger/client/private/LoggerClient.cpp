/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LoggerClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *              The log collector service connection client.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/private/LoggerClient.hpp"

#include "areg/base/File.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/LogObserverBase.hpp"

namespace areg::logger {

LoggerClient& LoggerClient::instance()
{
    static LoggerClient _instance;
    return _instance;
}

LoggerClient::LoggerClient()
    : ServiceClientConnectionBase( LoggerClient::TARGET_ID
                                 , LoggerClient::SERVICE_TYPE
                                 , static_cast<uint32_t>(CONNECT_TYPE)
                                 , LoggerClient::SOURCE_TYPE
                                 , static_cast<ConnectionConsumer &>(self())
                                 , static_cast<RemoteMessageHandler &>(self())
                                 , static_cast<DispatcherThread &>(self())
                                 , LoggerClient::THREAD_PREFIX)
    , ConfigListener    ( )
    , DispatcherThread           ( LoggerClient::THREAD_NAME, areg::DEFAULT_BLOCK_SIZE, areg::QUEUE_SIZE_MAXIMUM )
    , ConnectionConsumer( )
    , RemoteMessageHandler     ( )

    , mCallbacks                 ( nullptr )
    , mMessageProcessor          ( self() )
    , mIsPaused                  ( false )
    , mInstances                 ( )
    , mLogDatabase               ( )
{
}

bool LoggerClient::start_logger_client(const String & address /*= String::EmptyString*/, uint16_t portNr /*= areg::InvalidPort*/)
{
    if ((address.is_empty() == false) && (portNr != areg::InvalidPort))
    {
        Lock lock(mLock);
        mIsPaused = false;
        apply_connection_data(address, portNr);
    }
    else
    {
        Lock lock(mLock);
        mIsPaused = false;
        ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
        if (config.is_configured())
        {
            apply_connection_data(config.connection_address(), config.connection_port());
        }
    }

    return connect_service_host();
}

void LoggerClient::stop_logger_client()
{
    do
    {
        Lock lock(mLock);
        mIsPaused = false;
    } while (false);

    disconnect_service_host();
}

void LoggerClient::set_callbacks(const ObserverEvents* callbacks)
{
    Lock lock(mLock);
    mCallbacks = callbacks;
}

void LoggerClient::set_paused(bool doPause)
{
    FuncObserverStarted callback{ nullptr };
    bool is_started{ false };

    do
    {
        Lock lock(mLock);
        mIsPaused = doPause;
        callback = (mCallbacks != nullptr) && is_connection_started() ? mCallbacks->evtLoggingStarted : nullptr;
        is_started = mIsPaused ? false : is_connection_started();
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_observer_started(is_started);
    }
    else if (callback != nullptr)
    {
        callback(is_started);
    }
}

const areg::SocketAddress& LoggerClient::address() const noexcept
{
    Lock lock(mLock);
    return mClientConnection.address();
}

bool LoggerClient::is_sqlite_engine() const noexcept
{
    LogConfiguration config;
    return (config.is_db_logging_enabled() && (config.database_engine() == areg::LOGDB_ENGINE_NAME));
}

bool LoggerClient::is_config_logger_connect_enabled() const noexcept
{
    ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    return (config.is_configured() && config.connection_enable_flag());
}

String LoggerClient::config_logger_address() const
{
    ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.is_configured())
    {
        return config.connection_address();
    }
    else
    {
        return String::EmptyString;
    }
}

uint16_t LoggerClient::config_logger_port() const
{
    ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.is_configured())
    {
        return config.connection_port();
    }
    else
    {
        return areg::InvalidPort;
    }
}

bool LoggerClient::set_config_logger_connection(const String& address, uint16_t portNr)
{
    bool result{ false };
    ConnectionConfiguration config(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
    if (config.is_configured())
    {
        if (address.is_empty() == false)
            config.set_connection_address(address);
        if (portNr != areg::InvalidPort)
            config.set_connection_port(portNr);

        result = true;
    }

    return result;
}

bool LoggerClient::request_connected_instances()
{
    bool result{ false };
    if (mChannel.cookie() != areg::COOKIE_UNKNOWN)
    {
        result = send_message(areg::message_query_instances(mChannel.cookie(), LoggerClient::TARGET_ID));
    }

    return result;
}

bool LoggerClient::request_scopes(const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.cookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = send_message(areg::message_query_scopes(mChannel.cookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target));
    }

    return result;
}

bool LoggerClient::request_change_scope_prio(const areg::ScopeNames & scopes, const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.cookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = send_message(areg::message_update_scopes(mChannel.cookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target, scopes));
    }

    return result;
}

bool LoggerClient::request_save_configuration(const ITEM_ID& target /*= areg::TARGET_ALL*/)
{
    bool result{ false };
    Lock lock(mLock);
    if ((mChannel.cookie() != areg::COOKIE_UNKNOWN) && (target != areg::TARGET_UNKNOWN))
    {
        result = send_message(areg::message_save_configuration(mChannel.cookie(), target == areg::TARGET_ALL ? LoggerClient::TARGET_ID : target));
    }

    return result;
}

bool LoggerClient::open_logging_database(const char* dbPath /*= nullptr*/)
{
    String filePath (dbPath);
    if (filePath.is_empty())
    {
        if (is_sqlite_engine())
        {
            LogConfiguration config;
            mLogDatabase.set_database_logging_enabled(true);
            filePath = File::make_full_path(config.database_location(), config.database_name());
        }
        else
        {
            mLogDatabase.set_database_logging_enabled(false);
        }
    }

    bool result{ mLogDatabase.connect(filePath, false) };
    FuncLogDbCreated callback{ mLogDatabase.is_operable() && (mCallbacks != nullptr) ? mCallbacks->evtLogDbCreated  : nullptr};
    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_db_created(mLogDatabase.database_path().data());
    }
    else if (callback != nullptr)
    {
        callback(mLogDatabase.database_path().as_string());
    }

    return result;
}

void LoggerClient::close_logging_database()
{
    mLogDatabase.disconnect();
}

String LoggerClient::active_database_path() const
{
    return mLogDatabase.database_path();
}

String LoggerClient::initial_database_path() const
{
    return mLogDatabase.initial_database_path();
}

String LoggerClient::config_database_path() const
{
    String result;
    if (is_sqlite_engine())
    {
        LogConfiguration config;
        result = File::make_full_path(config.database_location().as_string(), config.database_name().as_string());
    }

    return result;
}

bool LoggerClient::set_config_database_path(const String& dbPath, bool enable)
{
    bool result{ false };
    LogConfiguration config;
    if (config.database_engine() == areg::LOGDB_ENGINE_NAME)
    {
        String dbLocation = File::file_directory(dbPath.as_string());
        String dbName = File::name_with_extension(dbPath.as_string());
        config.set_database_enable(enable, false);
        config.set_database_location(dbLocation, false);
        config.set_database_name(dbName, false);
        result = true;
    }

    return result;
}

String LoggerClient::config_database_location() const
{
    String result;
    if (is_sqlite_engine())
    {
        LogConfiguration config;
        result = File::file_full_path(config.database_location().as_string());
    }

    return result;
}

bool LoggerClient::set_config_database_location(const String& dbLocation)
{
    bool result{ false };
    if (is_sqlite_engine())
    {
        LogConfiguration config;
        config.set_database_location(dbLocation, false);
        result = true;
    }

    return result;
}

String LoggerClient::config_database_name() const
{
    String result;
    if (is_sqlite_engine())
    {
        LogConfiguration config;
        result = config.database_name().as_string();
    }

    return result;
}

bool LoggerClient::set_config_database_name(const String& dbName)
{
    bool result{ false };
    if (is_sqlite_engine())
    {
        LogConfiguration config;
        config.set_database_name(dbName, false);
        result = true;
    }

    return result;
}

bool LoggerClient::set_config_logger_connect_enabled(bool is_enabled)
{
    bool result{ false };
    LogConfiguration config;
    if (config.database_engine() == areg::LOGDB_ENGINE_NAME)
    {
        config.set_database_enable(is_enabled, false);
        result = true;
    }

    return result;
}

void LoggerClient::save_configuration()
{
    LogConfiguration config;
    config.save_configuration();
}

void LoggerClient::prepare_save_configuration(ConfigManager& /* config */)
{
}

void LoggerClient::post_save_configuration(ConfigManager& /* config */)
{
}

void LoggerClient::prepare_read_configuration(ConfigManager& /* config */)
{
}

void LoggerClient::post_read_configuration(ConfigManager& config)
{
    FuncObserverConfigured callbackConf{ nullptr };
    FuncLogDbConfigured callbackConfDb{ nullptr };
    String address;
    uint16_t port{0};
    String dbName;
    String dbLocation;
    String dbUser;

    config.set_log_enabled(areg::LogTarget::File, true, true);
    config.set_log_enabled(areg::LogTarget::Remote, true, true);

    do
    {
        Lock lock(mLock);
        if (mCallbacks != nullptr)
        {
            callbackConf    = mCallbacks->evtObserverConfigured;
            callbackConfDb  = mCallbacks->evtLogDbConfigured;
            address         = config.remote_service_address(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
            port            = config.remote_service_port(LoggerClient::SERVICE_TYPE, LoggerClient::CONNECT_TYPE);
            dbName          = config.log_database_property(areg::log_database_name().position);
            dbLocation      = config.log_database_property(areg::log_database_location().position);
            dbUser          = config.log_database_property(areg::log_database_user().position);
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_observer_configured(true, address.data(), port);
        LogObserverBase::_theLogObserver->on_log_db_configured(config.log_enabled(areg::LogTarget::Database), dbName.data(), dbLocation.data(), dbUser.data());
    }
    else
    {
        if (callbackConf != nullptr)
            callbackConf(true, address.as_string(), port);
        if (callbackConfDb != nullptr)
            callbackConfDb(config.log_enabled(areg::LogTarget::Database), dbName.as_string(), dbLocation.as_string(), dbUser.as_string());
    }
}

void LoggerClient::on_setup_configuration( const areg::ListProperties&  /* listReadonly */
                                        , const areg::ListProperties& /* listWritable */
                                        , ConfigManager& /* config */)
{
}

bool LoggerClient::post_event(Event& eventElem)
{
    return EventDispatcher::post_event(eventElem);
}

void LoggerClient::ready_for_events(bool is_ready)
{
    if (is_ready)
    {
        register_consumer_commands();
        DispatcherThread::ready_for_events(true);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::DisconnectState);
    }
    else
    {
        DispatcherThread::ready_for_events(false);
        set_connection_state(ServiceClientConnectionBase::ConnectionPhase::ConnectionStopped);
        unregister_consumer_commands();
    }
}

bool LoggerClient::connect_service_host()
{
    bool result{ false };
    if (is_running() == false)
    {
        if (start(areg::WAIT_INFINITE) && wait_start(areg::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connect_service_host();
        }
        else
        {
            shutdown(areg::WAIT_INFINITE);
        }
    }
    else
    {
        result = mClientConnection.is_valid();
    }

    return result;
}

void LoggerClient::disconnect_service_host()
{
    if (is_running())
    {
        FuncInstancesDisconnect callback{ mCallbacks != nullptr ? mCallbacks->evtInstDisconnected : nullptr };
        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->on_log_service_disconnected();
        }
        else if (callback != nullptr)
        {

            for (const auto& entry : mInstances.data())
            {
                callback(&entry.first, 1);
            }
        }

        mInstances.clear();

        ServiceClientConnectionBase::disconnect_service_host();
        wait_completion(areg::WAIT_INFINITE);
        shutdown(areg::DO_NOT_WAIT);
    }
}

void LoggerClient::on_service_exit()
{
    ServiceClientConnectionBase::on_service_exit();
    trigger_exit();
}

void LoggerClient::on_service_channel_connected(const Channel& channel)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    String address;
    uint16_t port{ areg::InvalidPort };
    bool is_started{ false };

    do
    {
        Lock lock(mLock);
        const areg::SocketAddress& addr{ mClientConnection.address() };
        address = addr.host_address();
        port = addr.host_port();
        is_started = mIsPaused ? false : is_connection_started();

        if (mCallbacks != nullptr)
        {
            callbackConnect = mCallbacks->evtServiceConnected;
            callbackStart = mCallbacks->evtLoggingStarted;
        }
    } while (false);

    send_message(areg::message_query_instances(channel.cookie(), LoggerClient::TARGET_ID));

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_service_connected(true, address.data(), port);
        LogObserverBase::_theLogObserver->on_log_observer_started(is_started);
    }
    else
    {
        if (callbackConnect != nullptr)
            callbackConnect(true, address.as_string(), port);
        if (callbackStart != nullptr)
            callbackStart(is_started);
    }
}

void LoggerClient::on_service_channel_disconnected(const Channel& /* channel */)
{
    FuncServiceConnected callbackConnect{ nullptr };
    FuncObserverStarted callbackStart{ nullptr };
    String address;
    uint16_t port{ areg::InvalidPort };

    do
    {
        Lock lock(mLock);
        const areg::SocketAddress& addr{ mClientConnection.address() };
        address = addr.host_address();
        port = addr.host_port();

        if (mCallbacks != nullptr)
        {
            callbackConnect = mCallbacks->evtServiceConnected;
            callbackStart = mCallbacks->evtLoggingStarted;
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_observer_started(false);
        LogObserverBase::_theLogObserver->on_log_service_connected(false, address.data(), port);
    }
    else
    {
        if (callbackStart != nullptr)
            callbackStart(false);
        if (callbackConnect != nullptr)
            callbackConnect(false, address.as_string(), port);
    }
}

void LoggerClient::on_service_channel_lost(const Channel& /* channel */)
{
    FuncObserverStarted callback{ nullptr };

    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtLoggingStarted : nullptr;
        mInstances.clear();
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_observer_started(false);
    }
    else if (callback != nullptr)
    {
        callback(false);
    }
}

void LoggerClient::failed_send_message(const EventEnvelope& /* msgFailed */, Socket& /* whichTarget */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtMessagingFailed : nullptr;
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_messaging_failed();
    }
    else if (callback != nullptr)
    {
        callback();
    }
}

void LoggerClient::failed_receive_message(Socket& /* whichSource */)
{
    FuncMessagingFailed callback{ nullptr };
    do
    {
        Lock lock(mLock);
        callback = mCallbacks != nullptr ? mCallbacks->evtMessagingFailed : nullptr;
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_messaging_failed();
    }
    else if (callback != nullptr)
    {
        callback();
    }
}

void LoggerClient::failed_process_message(const EventEnvelope& /* msgUnprocessed */)
{
}

void LoggerClient::process_received_message(EventEnvelope& msgReceived, Socket& whichSource)
{
    if (msgReceived.is_valid() && whichSource.is_valid())
    {
        areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgReceived.message_id());
        switch (msgId)
        {
        case areg::FuncIdRange::SystemServiceNotifyConnection:
            mMessageProcessor.notify_service_connection(msgReceived);
            service_connection_event(msgReceived);
            break;

        case areg::FuncIdRange::SystemServiceNotifyInstances:
            mMessageProcessor.notify_connected_clients(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogRegisterScopes:
            mMessageProcessor.notify_log_register_scopes(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogScopesUpdated:
            mMessageProcessor.notify_log_update_scopes(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogMessage:
            if (mIsPaused == false)
            {
                mMessageProcessor.notify_log_message(msgReceived);
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
        case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::FuncIdRange::AttributeLastId:                  // fall through
        case areg::FuncIdRange::AttributeFirstId:                 // fall through
        case areg::FuncIdRange::ResponseLastId:                   // fall through
        case areg::FuncIdRange::ResponseFirstId:                  // fall through
        case areg::FuncIdRange::RequestLastId:                    // fall through
        case areg::FuncIdRange::RequestFirstId:                   // fall through
        case areg::FuncIdRange::EmptyFunctionId:                  // fall through
        case areg::FuncIdRange::ServiceLogUpdateScopes:           // fall through
        case areg::FuncIdRange::ServiceLogQueryScopes:            // fall through
        case areg::FuncIdRange::ServiceSaveLogConfiguration:      // fall through
        default:
            ASSERT(false);
        }
    }
}

} // namespace areg::logger
