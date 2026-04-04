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

namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class implementation
//////////////////////////////////////////////////////////////////////////

LogConfiguration::LogConfiguration()
    : mConfigMan(Application::config_manager())
{
}

bool LogConfiguration::is_logging_enabled() const noexcept
{
    return mConfigMan.logging_status();
}

bool LogConfiguration::is_remote_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Remote);
}

bool LogConfiguration::is_file_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::File);
}

bool LogConfiguration::is_db_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Database);
}

bool LogConfiguration::is_debug_logging_enabled() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Debug);
}

void LogConfiguration::set_log_enabled(areg::LogTarget logType, bool isEnabled)
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

areg::Version LogConfiguration::version() const noexcept
{
    return mConfigMan.log_version();
}

areg::String LogConfiguration::layout_enter() const
{
    return mConfigMan.log_layout_enter();
}

void LogConfiguration::set_layout_enter(const areg::String & prop)
{
    mConfigMan.set_layout_enter(prop);
}

areg::String LogConfiguration::layout_message() const
{
    return mConfigMan.log_layout_message();
}

void LogConfiguration::set_layout_message(const areg::String & prop)
{
    mConfigMan.set_layout_message(prop);
}

areg::String LogConfiguration::layout_exit() const
{
    return mConfigMan.log_layout_exit();
}

void LogConfiguration::set_layout_exit(const areg::String& prop)
{
    mConfigMan.set_layout_exit(prop);
}

uint32_t LogConfiguration::stack_size() const noexcept
{
    return mConfigMan.remote_queue_size();
}

void LogConfiguration::set_stack_size(uint32_t prop)
{
    mConfigMan.set_remote_queue_size(prop);
}

bool LogConfiguration::status() const noexcept
{
    return mConfigMan.logging_status();
}

void LogConfiguration::set_status(bool prop)
{
    mConfigMan.set_logging_status(prop);
}

bool LogConfiguration::append_data() const noexcept
{
    return mConfigMan.log_file_append();
}

void LogConfiguration::set_append_data(bool prop)
{
    mConfigMan.set_file_append(prop);
}

areg::String LogConfiguration::log_file() const
{
    return mConfigMan.log_file_location();
}

void LogConfiguration::set_log_file(const areg::String& prop)
{
    mConfigMan.set_file_location(prop);
}

bool LogConfiguration::remote_tcp_enable() const
{
    return (mConfigMan.log_enabled(areg::LogTarget::Remote) &&
            mConfigMan.remote_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip));
}

void LogConfiguration::set_tcp_enable(bool prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

areg::String LogConfiguration::remote_tcp_address() const
{
    return mConfigMan.remote_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void LogConfiguration::set_tcp_address(const areg::String & prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

uint16_t LogConfiguration::remote_tcp_port() const
{
    return mConfigMan.remote_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
}

void LogConfiguration::set_tcp_port(uint16_t prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
}

bool LogConfiguration::database_enable() const noexcept
{
    return mConfigMan.log_enabled(areg::LogTarget::Database);
}

void LogConfiguration::set_database_enable(bool prop, bool isTemporary /*= false*/)
{
    mConfigMan.set_log_enabled(areg::LogTarget::Database, prop, isTemporary);
}

uint32_t LogConfiguration::module_scopes(std::vector<areg::Property>& scopeList)
{
    scopeList = mConfigMan.module_log_scopes();
    return static_cast<uint32_t>(scopeList.size());
}

void LogConfiguration::set_module_scopes(const std::vector<areg::Property>& scopeList)
{
    mConfigMan.remove_module_scopes();
    mConfigMan.add_log_scopes(scopeList, true);
}

void LogConfiguration::enable_scopes(const std::vector<String>& scopeNames, bool enable, bool isTemporary)
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

areg::String LogConfiguration::database_engine() const
{
    return mConfigMan.log_database_property(areg::log_database_engine().position);
}

void LogConfiguration::set_database_engine(const areg::String& dbEngine, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_engine().position, dbEngine, isTemporary);
}

areg::String LogConfiguration::database_full_path() const
{
    String dbLocation = mConfigMan.log_database_property(areg::log_database_location().position);
    String dbName = mConfigMan.log_database_property(areg::log_database_name().position);
    return File::make_full_path(dbLocation, dbName);
}

void LogConfiguration::set_db_path(const areg::String& dbFullPath, bool isTemporary)
{
    String dbLocation = File::file_directory(dbFullPath.as_string());
    String dbName = File::name_with_extension(dbFullPath.as_string());

    mConfigMan.set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
    mConfigMan.set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String LogConfiguration::database_name() const
{
    return mConfigMan.log_database_property(areg::log_database_name().position);
}

void LogConfiguration::set_database_name(const String & dbName, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_name().position, dbName, isTemporary);
}

areg::String LogConfiguration::database_location() const
{
    return mConfigMan.log_database_property(areg::log_database_location().position);
}

void LogConfiguration::set_database_location(const String & dbLocation, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_location().position, dbLocation, isTemporary);
}

areg::String LogConfiguration::database_driver() const
{
    return mConfigMan.log_database_property(areg::log_database_driver().position);
}

void LogConfiguration::set_database_driver(const String& dbDriver, bool isTemporary /*= false*/)
{
    mConfigMan.set_db_property(areg::log_database_driver().position, dbDriver, isTemporary);
}

areg::SocketAddress LogConfiguration::database_address() const
{
    String address{ mConfigMan.log_database_property(areg::log_database_address().position) };
    uint32_t port{ mConfigMan.log_database_property(areg::log_database_port().position).to_uint32() };

    return areg::SocketAddress(address, static_cast<uint16_t>(port));
}

void LogConfiguration::set_database_address(const areg::SocketAddress& dbAddress, bool isTemporary)
{
    set_database_address(dbAddress.host_address(), dbAddress.host_port(), isTemporary);
}

void LogConfiguration::set_database_address(const String& dbAddress, uint16_t dbPort, bool isTemporary)
{
    mConfigMan.set_db_property(areg::log_database_address().position, dbAddress, isTemporary);
    mConfigMan.set_db_property(areg::log_database_port().position, String::make_string(static_cast<uint32_t>(dbPort)), isTemporary);
}

areg::UserData LogConfiguration::database_user() const
{
    String user{ mConfigMan.log_database_property(areg::log_database_user().position) };
    String password{ mConfigMan.log_database_property(areg::log_database_password().position) };
    return areg::UserData(user, password);
}

void LogConfiguration::set_database_user(const areg::UserData& dbUser, bool isTemporary)
{
    return set_database_user(dbUser.user(), dbUser.password(), isTemporary);
}

void LogConfiguration::set_database_user(const areg::String& dbUserName, const areg::String& dbUserPassword, bool isTemporary)
{
    mConfigMan.set_db_property(areg::log_database_user().position, dbUserName, isTemporary);
    mConfigMan.set_db_property(areg::log_database_password().position, dbUserPassword, isTemporary);
}

void LogConfiguration::save_configuration()
{
    mConfigMan.save_config();
}

#if AREG_LOGGING

void LogConfiguration::update_scope_configuration(const areg::ScopeController& scopeController) const
{
    const auto& scopeList = scopeController.scope_list();
    ScopeRoot root;
    for (auto pos = scopeList.first_position(); scopeList.is_valid_position(pos); pos = scopeList.next_position(pos))
    {
        LogScope* scope = scopeList.value_at(pos);
        ASSERT(scope != nullptr);
        root.add_child_recursive(*scope);
    }

    root.group_recursive();
    mConfigMan.remove_module_scopes();
    root.update_config_node(mConfigMan, String::EmptyString);
}

#else   // AREG_LOGGING

void LogConfiguration::update_scope_configuration(const ScopeController& /*scopeController*/) const
{
}

#endif  // AREG_LOGGING

} // namespace areg
