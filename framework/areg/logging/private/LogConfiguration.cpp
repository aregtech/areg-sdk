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

areg::LogConfiguration::LogConfiguration()
    : mConfigMan(Application::config_manager())
{
}

bool areg::LogConfiguration::is_logging_enabled() const noexcept
{
    return mConfigMan.logging_status();
}

bool areg::LogConfiguration::is_remote_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Remote);
}

bool areg::LogConfiguration::is_file_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::File);
}

bool areg::LogConfiguration::is_db_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Database);
}

bool areg::LogConfiguration::is_debug_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Debug);
}

void areg::LogConfiguration::set_log_enabled(areg::LogTarget logType, bool isEnabled)
{
    if (isEnabled && mConfigMan.logging_status() == false)
    {
        mConfigMan.set_logging_status(true);
    }

    if (mConfigMan.log_enabled(logType) != isEnabled)
    {
        mConfigMan.set_log_enabled(logType, isEnabled);
    }
}

areg::Version areg::LogConfiguration::version() const noexcept
{
    return mConfigMan.log_version();
}

areg::String areg::LogConfiguration::layout_enter() const
{
    return mConfigMan.log_layout_enter();
}

void areg::LogConfiguration::set_layout_enter(const areg::String & prop)
{
    mConfigMan.set_layout_enter(prop);
}

areg::String areg::LogConfiguration::layout_message() const
{
    return mConfigMan.log_layout_message();
}

void areg::LogConfiguration::set_layout_message(const areg::String & prop)
{
    mConfigMan.set_layout_message(prop);
}

areg::String areg::LogConfiguration::layout_exit() const
{
    return mConfigMan.log_layout_exit();
}

void areg::LogConfiguration::set_layout_exit(const areg::String& prop)
{
    mConfigMan.set_layout_exit(prop);
}

uint32_t areg::LogConfiguration::stack_size() const noexcept
{
    return mConfigMan.remote_queue_size();
}

void areg::LogConfiguration::set_stack_size(uint32_t prop)
{
    mConfigMan.set_remote_queue_size(prop);
}

bool areg::LogConfiguration::status() const noexcept
{
    return mConfigMan.logging_status();
}

void areg::LogConfiguration::set_status(bool prop)
{
    mConfigMan.set_logging_status(prop);
}

bool areg::LogConfiguration::append_data() const noexcept
{
    return mConfigMan.log_file_append();
}

void areg::LogConfiguration::set_append_data(bool prop)
{
    mConfigMan.set_file_append(prop);
}

areg::String areg::LogConfiguration::log_file() const
{
    return mConfigMan.log_file_location();
}

void areg::LogConfiguration::set_log_file(const areg::String& prop)
{
    mConfigMan.set_file_location(prop);
}

bool areg::LogConfiguration::remote_tcp_enable() const
{
    return (mConfigMan.log_enabled(areg::LogTarget::Remote) &&
            mConfigMan.remote_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip));
}

void areg::LogConfiguration::set_tcp_enable(bool prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

areg::String areg::LogConfiguration::remote_tcp_address() const
{
    return mConfigMan.remote_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void areg::LogConfiguration::set_tcp_address(const areg::String & prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

uint16_t areg::LogConfiguration::remote_tcp_port() const
{
    return mConfigMan.remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void areg::LogConfiguration::set_tcp_port(uint16_t prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

bool areg::LogConfiguration::database_enable() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Database);
}

void areg::LogConfiguration::set_database_enable(bool prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_log_enabled(areg::LogTarget::Database, prop, isTemporary);
}

uint32_t areg::LogConfiguration::module_scopes(std::vector<areg::Property>& scopeList)
{
    scopeList = mConfigMan.module_log_scopes();
    return static_cast<uint32_t>(scopeList.size());
}

void areg::LogConfiguration::set_module_scopes(const std::vector<areg::Property>& scopeList)
{
    mConfigMan.remove_module_scopes();
    mConfigMan.add_log_scopes(scopeList, true);
}

void areg::LogConfiguration::enable_scopes(const std::vector<String>& scopeNames, bool enable, bool isTemporary)
{
    constexpr areg::ConfigEntry confKey{ areg::ConfigEntry::LogScope };
    const uint32_t prios{enable 
                        ? static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug) 
                        : static_cast<uint32_t>(areg::LogPriority::PrioNotset) };
    const areg::ConfigKey& key{ areg::log_scope() };

    for (const auto& scope : scopeNames)
    {
        StringArray scopes = areg::priority_to_string_list(prios);
        String prioNames;
        for (const auto& name : scopes.data())
        {
            if (!prioNames.is_empty())
                prioNames += areg::SYNTAX_VALUE_LIST_DELIMITER;
            prioNames += name;
        }

        mConfigMan.set_module_property(key.section, key.property, scope, prioNames, confKey, isTemporary);
    }
}

areg::String areg::LogConfiguration::database_engine() const
{
    return mConfigMan.log_database_property(areg::log_database_engine().position);
}

void areg::LogConfiguration::set_database_engine(const areg::String& dbEngine, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_engine().position, dbEngine, isTemporary);
}

areg::String areg::LogConfiguration::database_full_path() const
{
    String dbLocation = mConfigMan.log_database_property(areg::log_database_location().position);
    String dbName = mConfigMan.log_database_property(areg::log_database_name().position);
    return File::make_full_path(dbLocation, dbName);
}

void areg::LogConfiguration::set_db_path(const areg::String& dbFullPath, bool isTemporary)
{
    String dbLocation = File::file_directory(dbFullPath.as_string());
    String dbName = File::name_with_extension(dbFullPath.as_string());

    mConfigMan.set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
    mConfigMan.set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String areg::LogConfiguration::database_name() const
{
    return mConfigMan.log_database_property(areg::log_database_name().position);
}

void areg::LogConfiguration::set_database_name(const String & dbName, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String areg::LogConfiguration::database_location() const
{
    return mConfigMan.log_database_property(areg::log_database_location().position);
}

void areg::LogConfiguration::set_database_location(const String & dbLocation, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
}

areg::String areg::LogConfiguration::database_driver() const
{
    return mConfigMan.log_database_property(areg::log_database_driver().position);
}

void areg::LogConfiguration::set_database_driver(const String& dbDriver, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_driver().position, dbDriver, isTemporary);
}

areg::SocketAddress areg::LogConfiguration::database_address() const
{
    String address{ mConfigMan.log_database_property(areg::log_database_address().position) };
    uint32_t port{ mConfigMan.log_database_property(areg::log_database_port().position).to_uint32() };

    return areg::SocketAddress(address, static_cast<uint16_t>(port));
}

void areg::LogConfiguration::set_database_address(const areg::SocketAddress& dbAddress, bool isTemporary)
{
    set_database_address(dbAddress.host_address(), dbAddress.host_port(), isTemporary);
}

void areg::LogConfiguration::set_database_address(const String& dbAddress, uint16_t dbPort, bool isTemporary)
{
    mConfigMan.set_db_property(areg::log_database_address().position, dbAddress, isTemporary);
    mConfigMan.set_db_property(areg::log_database_port().position, String::make_string(static_cast<uint32_t>(dbPort)), isTemporary);
}

areg::UserData areg::LogConfiguration::database_user() const
{
    String user{ mConfigMan.log_database_property(areg::log_database_user().position) };
    String password{ mConfigMan.log_database_property(areg::log_database_password().position) };
    return areg::UserData(user, password);
}

void areg::LogConfiguration::set_database_user(const areg::UserData& dbUser, bool isTemporary)
{
    return set_database_user(dbUser.user(), dbUser.password(), isTemporary);
}

void areg::LogConfiguration::set_database_user(const areg::String& dbUserName, const areg::String& dbUserPassword, bool isTemporary)
{
    mConfigMan.set_db_property(areg::log_database_user().position, dbUserName, isTemporary);
    mConfigMan.set_db_property(areg::log_database_password().position, dbUserPassword, isTemporary);
}

void areg::LogConfiguration::save_configuration()
{
    mConfigMan.save_config();
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
    mConfigMan.remove_module_scopes();
    root.update_config_node(mConfigMan, String::EmptyString);
}

#else   // AREG_LOGGING

void areg::LogConfiguration::update_scope_configuration(const ScopeController& /*scopeController*/) const
{
}

#endif  // AREG_LOGGING
