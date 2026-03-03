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

bool areg::LogConfiguration::is_logging_enabled() const
{
    return Application::config_manager().logging_status();
}

bool areg::LogConfiguration::is_remote_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(areg::LogTarget::Remote);
}

bool areg::LogConfiguration::is_file_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(areg::LogTarget::File);
}

bool areg::LogConfiguration::is_db_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(areg::LogTarget::Database);
}

bool areg::LogConfiguration::is_debug_logging_enabled() const
{
    ConfigManager& config = Application::config_manager();
    return config.log_enabled(areg::LogTarget::Debug);
}

void areg::LogConfiguration::set_log_enabled(areg::LogTarget logType, bool isEnabled)
{
    ConfigManager& config = Application::config_manager();
    if (isEnabled && config.logging_status() == false)
    {
        config.set_logging_status(true);
    }

    if (config.log_enabled(logType) != isEnabled)
    {
        config.set_log_enabled(logType, isEnabled);
    }
}

areg::Version areg::LogConfiguration::version() const
{
    return Application::config_manager().log_version();
}

areg::String areg::LogConfiguration::layout_enter() const
{
    return Application::config_manager().log_layout_enter();
}

void areg::LogConfiguration::set_layout_enter(const areg::String & prop)
{
    Application::config_manager().set_layout_enter(prop);
}

areg::String areg::LogConfiguration::layout_message() const
{
    return Application::config_manager().log_layout_message();
}

void areg::LogConfiguration::set_layout_message(const areg::String & prop)
{
    Application::config_manager().set_layout_message(prop);
}

areg::String areg::LogConfiguration::layout_exit() const
{
    return Application::config_manager().log_layout_exit();
}

void areg::LogConfiguration::set_layout_exit(const areg::String& prop)
{
    Application::config_manager().set_layout_exit(prop);
}

uint32_t areg::LogConfiguration::stack_size() const
{
    return Application::config_manager().remote_queue_size();
}

void areg::LogConfiguration::set_stack_size(uint32_t prop)
{
    Application::config_manager().set_remote_queue_size(prop);
}

bool areg::LogConfiguration::status() const
{
    return Application::config_manager().logging_status();
}

void areg::LogConfiguration::set_status(bool prop)
{
    Application::config_manager().set_logging_status(prop);
}

bool areg::LogConfiguration::append_data() const
{
    return Application::config_manager().log_file_append();
}

void areg::LogConfiguration::set_append_data(bool prop)
{
    Application::config_manager().set_file_append(prop);
}

areg::String areg::LogConfiguration::log_file() const
{
    return Application::config_manager().log_file_location();
}

void areg::LogConfiguration::set_log_file(const areg::String& prop)
{
    Application::config_manager().set_file_location(prop);
}

bool areg::LogConfiguration::remote_tcp_enable() const
{
    return (Application::config_manager().log_enabled(areg::LogTarget::Remote) &&
            Application::config_manager().remote_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip));
}

void areg::LogConfiguration::set_tcp_enable(bool prop, bool isTemporary /*= false*/)
{
    Application::config_manager().set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

areg::String areg::LogConfiguration::remote_tcp_address() const
{
    return Application::config_manager().remote_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void areg::LogConfiguration::set_tcp_address(const areg::String & prop, bool isTemporary /*= false*/)
{
    Application::config_manager().set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

uint16_t areg::LogConfiguration::remote_tcp_port() const
{
    return Application::config_manager().remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void areg::LogConfiguration::set_tcp_port(uint16_t prop, bool isTemporary /*= false*/)
{
    Application::config_manager().set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

bool areg::LogConfiguration::database_enable() const
{
    return Application::config_manager().log_enabled(areg::LogTarget::Database);
}

void areg::LogConfiguration::set_database_enable(bool prop, bool isTemporary /*= false*/)
{
    Application::config_manager().set_log_enabled(areg::LogTarget::Database, prop, isTemporary);
}

uint32_t areg::LogConfiguration::module_scopes(std::vector<areg::Property>& scopeList)
{
    scopeList = Application::config_manager().module_log_scopes();
    return static_cast<uint32_t>(scopeList.size());
}

void areg::LogConfiguration::set_module_scopes(const std::vector<areg::Property>& scopeList)
{
    Application::config_manager().remove_module_scopes();
    Application::config_manager().add_log_scopes(scopeList, true);
}

areg::String areg::LogConfiguration::database_engine() const
{
    return Application::config_manager().log_database_property(areg::log_database_engine().position);
}

void areg::LogConfiguration::set_database_engine(const areg::String& dbEngine, bool isTemporary /*= false*/)
{
    Application::config_manager().set_db_property(areg::log_database_engine().position, dbEngine, isTemporary);
}

areg::String areg::LogConfiguration::database_full_path() const
{
    String dbLocation = Application::config_manager().log_database_property(areg::log_database_location().position);
    String dbName = Application::config_manager().log_database_property(areg::log_database_name().position);
    return File::make_full_path(dbLocation, dbName);
}

void areg::LogConfiguration::set_db_path(const areg::String& dbFullPath, bool isTemporary)
{
    String dbLocation = File::file_directory(dbFullPath.as_string());
    String dbName = File::name_with_extension(dbFullPath.as_string());

    Application::config_manager().set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
    Application::config_manager().set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String areg::LogConfiguration::database_name() const
{
    return Application::config_manager().log_database_property(areg::log_database_name().position);
}

void areg::LogConfiguration::set_database_name(const String & dbName, bool isTemporary /*= false*/)
{
    Application::config_manager().set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String areg::LogConfiguration::database_location() const
{
    return Application::config_manager().log_database_property(areg::log_database_location().position);
}

void areg::LogConfiguration::set_database_location(const String & dbLocation, bool isTemporary /*= false*/)
{
    Application::config_manager().set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
}

areg::String areg::LogConfiguration::database_driver() const
{
    return Application::config_manager().log_database_property(areg::log_database_driver().position);
}

void areg::LogConfiguration::set_database_driver(const String& dbDriver, bool isTemporary /*= false*/)
{
    Application::config_manager().set_db_property(areg::log_database_driver().position, dbDriver, isTemporary);
}

areg::SocketAddress areg::LogConfiguration::database_address() const
{
    String address{ Application::config_manager().log_database_property(areg::log_database_address().position) };
    uint32_t port{ Application::config_manager().log_database_property(areg::log_database_port().position).to_uint32() };

    return areg::SocketAddress(address, static_cast<uint16_t>(port));
}

void areg::LogConfiguration::set_database_address(const areg::SocketAddress& dbAddress, bool isTemporary)
{
    set_database_address(dbAddress.host_address(), dbAddress.host_port(), isTemporary);
}

void areg::LogConfiguration::set_database_address(const String& dbAddress, uint16_t dbPort, bool isTemporary)
{
    Application::config_manager().set_db_property(areg::log_database_address().position, dbAddress, isTemporary);
    Application::config_manager().set_db_property(areg::log_database_port().position, String::make_string(static_cast<uint32_t>(dbPort)), isTemporary);
}

areg::UserData areg::LogConfiguration::database_user() const
{
    String user{ Application::config_manager().log_database_property(areg::log_database_user().position) };
    String password{ Application::config_manager().log_database_property(areg::log_database_password().position) };
    return areg::UserData(user, password);
}

void areg::LogConfiguration::set_database_user(const areg::UserData& dbUser, bool isTemporary)
{
    return set_database_user(dbUser.user(), dbUser.password(), isTemporary);
}

void areg::LogConfiguration::set_database_user(const areg::String& dbUserName, const areg::String& dbUserPassword, bool isTemporary)
{
    Application::config_manager().set_db_property(areg::log_database_user().position, dbUserName, isTemporary);
    Application::config_manager().set_db_property(areg::log_database_password().position, dbUserPassword, isTemporary);
}

void areg::LogConfiguration::save_configuration()
{
    Application::config_manager().save_config();
}

#if AREG_LOGGING

void areg::LogConfiguration::update_scope_configuration(const areg::ScopeController& scopeController) const
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

#else   // AREG_LOGGING

void areg::LogConfiguration::update_scope_configuration(const ScopeController& /*scopeController*/) const
{
}

#endif  // AREG_LOGGING
