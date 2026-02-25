/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogConfiguration.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Interface of log configuration
 ************************************************************************/

#include "areg/logging/LogConfiguration.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/ScopeController.hpp"
#include "areg/logging/private/ScopeNodes.hpp"

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class implementation
//////////////////////////////////////////////////////////////////////////

bool LogConfiguration::is_logging_enabled() const
{
    return Application::config_manager().logging_status();
}

bool LogConfiguration::is_remote_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(NELogging::LogTarget::Remote);
}

bool LogConfiguration::is_file_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(NELogging::LogTarget::File);
}

bool LogConfiguration::is_db_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(NELogging::LogTarget::Database);
}

bool LogConfiguration::is_debug_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(NELogging::LogTarget::Debug);
}

void LogConfiguration::set_log_enabled(NELogging::LogTarget logType, bool is_enabled)
{
    ConfigManager& config = Application::config_manager();
    if (is_enabled && config.logging_status() == false)
    {
        config.set_logging_status(true);
    }

    if (config.log_enabled(logType) != is_enabled)
    {
        config.set_log_enabled(logType, is_enabled);
    }
}

Version LogConfiguration::version() const
{
    return Application::config_manager().log_version();
}

String LogConfiguration::layout_enter() const
{
    return Application::config_manager().log_layout_enter();
}

void LogConfiguration::set_layout_enter(const String & prop)
{
    Application::config_manager().set_layout_enter(prop);
}

String LogConfiguration::layout_message() const
{
    return Application::config_manager().log_layout_message();
}

void LogConfiguration::set_layout_message(const String & prop)
{
    Application::config_manager().set_layout_message(prop);
}

String LogConfiguration::layout_exit() const
{
    return Application::config_manager().log_layout_exit();
}

void LogConfiguration::set_layout_exit(const String& prop)
{
    Application::config_manager().set_layout_exit(prop);
}

uint32_t LogConfiguration::stack_size() const
{
    return Application::config_manager().remote_queue_size();
}

void LogConfiguration::set_stack_size(uint32_t prop)
{
    Application::config_manager().set_remote_queue_size(prop);
}

bool LogConfiguration::status() const
{
    return Application::config_manager().logging_status();
}

void LogConfiguration::set_status(bool prop)
{
    Application::config_manager().set_logging_status(prop);
}

bool LogConfiguration::append_data() const
{
    return Application::config_manager().log_file_append();
}

void LogConfiguration::set_append_data(bool prop)
{
    Application::config_manager().set_file_append(prop);
}

String LogConfiguration::log_file() const
{
    return Application::config_manager().log_file_location();
}

void LogConfiguration::set_log_file(const String& prop)
{
    Application::config_manager().set_file_location(prop);
}

bool LogConfiguration::remote_tcp_enable() const
{
    return (Application::config_manager().log_enabled(NELogging::LogTarget::Remote) &&
            Application::config_manager().remote_service_enable(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip));
}

void LogConfiguration::set_tcp_enable(bool prop, bool is_temporary /*= false*/)
{
    Application::config_manager().set_service_enable(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip, prop, is_temporary);
}

String LogConfiguration::remote_tcp_address() const
{
    return Application::config_manager().remote_service_address(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip);
}

void LogConfiguration::set_tcp_address(const String & prop, bool is_temporary /*= false*/)
{
    Application::config_manager().set_service_address(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip, prop, is_temporary);
}

uint16_t LogConfiguration::remote_tcp_port() const
{
    return Application::config_manager().remote_service_port(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip);
}

void LogConfiguration::set_tcp_port(uint16_t prop, bool is_temporary /*= false*/)
{
    Application::config_manager().set_service_port(NERemoteService::RemoteServiceKind::Logger, NERemoteService::ConnectionType::Tcpip, prop, is_temporary);
}

bool LogConfiguration::database_enable() const
{
    return Application::config_manager().log_enabled(NELogging::LogTarget::Database);
}

void LogConfiguration::set_database_enable(bool prop, bool is_temporary /*= false*/)
{
    Application::config_manager().set_log_enabled(NELogging::LogTarget::Database, prop, is_temporary);
}

uint32_t LogConfiguration::module_scopes(std::vector<Property>& scopeList)
{
    scopeList = Application::config_manager().module_log_scopes();
    return static_cast<uint32_t>(scopeList.size());
}

void LogConfiguration::set_module_scopes(const std::vector<Property>& scopeList)
{
    Application::config_manager().remove_module_scopes();
    Application::config_manager().add_log_scopes(scopeList, true);
}

String LogConfiguration::database_engine() const
{
    return Application::config_manager().log_database_property(NEPersistence::log_database_engine().position);
}

void LogConfiguration::set_database_engine(const String& dbEngine, bool is_temporary /*= false*/)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_engine().position, dbEngine, is_temporary);
}

String LogConfiguration::database_full_path() const
{
    String dbLocation = Application::config_manager().log_database_property(NEPersistence::log_database_location().position);
    String dbName = Application::config_manager().log_database_property(NEPersistence::log_database_name().position);
    return File::make_full_path(dbLocation, dbName);
}

void LogConfiguration::set_db_path(const String& dbFullPath, bool is_temporary)
{
    String dbLocation = File::file_directory(dbFullPath.as_string());
    String dbName = File::name_with_extension(dbFullPath.as_string());

    Application::config_manager().set_db_property(NEPersistence::log_database_location().position, dbLocation, is_temporary);
    Application::config_manager().set_db_property(NEPersistence::log_database_name().position, dbName, is_temporary);
}

String LogConfiguration::database_name() const
{
    return Application::config_manager().log_database_property(NEPersistence::log_database_name().position);
}

void LogConfiguration::set_database_name(const String & dbName, bool is_temporary /*= false*/)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_name().position, dbName, is_temporary);
}

String LogConfiguration::database_location() const
{
    return Application::config_manager().log_database_property(NEPersistence::log_database_location().position);
}

void LogConfiguration::set_database_location(const String & dbLocation, bool is_temporary /*= false*/)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_location().position, dbLocation, is_temporary);
}

String LogConfiguration::database_driver() const
{
    return Application::config_manager().log_database_property(NEPersistence::log_database_driver().position);
}

void LogConfiguration::set_database_driver(const String& dbDriver, bool is_temporary /*= false*/)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_driver().position, dbDriver, is_temporary);
}

NESocket::SocketAddress LogConfiguration::database_address() const
{
    String address{ Application::config_manager().log_database_property(NEPersistence::log_database_address().position) };
    uint32_t port{ Application::config_manager().log_database_property(NEPersistence::log_database_port().position).to_uint32() };

    return NESocket::SocketAddress(address, static_cast<uint16_t>(port));
}

void LogConfiguration::set_database_address(const NESocket::SocketAddress& dbAddress, bool is_temporary)
{
    set_database_address(dbAddress.host_address(), dbAddress.host_port(), is_temporary);
}

void LogConfiguration::set_database_address(const String& dbAddress, uint16_t dbPort, bool is_temporary)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_address().position, dbAddress, is_temporary);
    Application::config_manager().set_db_property(NEPersistence::log_database_port().position, String::make_string(static_cast<uint32_t>(dbPort)), is_temporary);
}

NESocket::UserData LogConfiguration::database_user() const
{
    String user{ Application::config_manager().log_database_property(NEPersistence::log_database_user().position) };
    String password{ Application::config_manager().log_database_property(NEPersistence::log_database_password().position) };
    return NESocket::UserData(user, password);
}

void LogConfiguration::set_database_user(const NESocket::UserData& dbUser, bool is_temporary)
{
    return set_database_user(dbUser.user(), dbUser.password(), is_temporary);
}

void LogConfiguration::set_database_user(const String& dbUserName, const String& dbUserPassword, bool is_temporary)
{
    Application::config_manager().set_db_property(NEPersistence::log_database_user().position, dbUserName, is_temporary);
    Application::config_manager().set_db_property(NEPersistence::log_database_password().position, dbUserPassword, is_temporary);
}

void LogConfiguration::save_configuration()
{
    Application::config_manager().save_config();
}

#if AREG_LOGS

void LogConfiguration::update_scope_configuration(const ScopeController& scopeController) const
{
    const auto& scopeList = scopeController.scope_list();
    uint32_t key{ 0 };
    LogScope* scope = scopeList.resource_first_key(key);
    ScopeRoot root;
    while (scope != nullptr)
    {
        root.add_child_recursive(*scope);
        scope = scopeList.resource_next_key(key);
    }

    root.group_recursive();
    ConfigManager & config = Application::config_manager();
    config.remove_module_scopes();
    root.update_config_node(config, String::EmptyString);
}

#else   // AREG_LOGS

void LogConfiguration::update_scope_configuration(const ScopeController& /*scopeController*/) const
{
}

#endif  // AREG_LOGS
