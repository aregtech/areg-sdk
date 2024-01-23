/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogConfiguration.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Interface of log configuration
 ************************************************************************/

#include "areg/trace/LogConfiguration.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/NELogging.hpp"
#include "areg/trace/private/ScopeController.hpp"
#include "areg/trace/private/ScopeNodes.hpp"

//////////////////////////////////////////////////////////////////////////
// LogConfiguration class implementation
//////////////////////////////////////////////////////////////////////////

bool LogConfiguration::isLoggingEnabled(void) const
{
    return Application::getConfigManager().getLoggingStatus();
}

bool LogConfiguration::isRemoteLoggingEnabled(void) const
{
    ConfigManager& config = Application::getConfigManager();
    return config.getLogEnabled(NETrace::eLogingTypes::LogTypeRemote);
}

bool LogConfiguration::isFileLoggingEnabled(void) const
{
    ConfigManager& config = Application::getConfigManager();
    return config.getLogEnabled(NETrace::eLogingTypes::LogTypeFile);
}

bool LogConfiguration::isDatabaseLoggingEnabled(void) const
{
    ConfigManager& config = Application::getConfigManager();
    return config.getLogEnabled(NETrace::eLogingTypes::LogTypeDatabase);
}

bool LogConfiguration::isDebugOutputLoggingEnabled(void) const
{
    ConfigManager& config = Application::getConfigManager();
    return config.getLogEnabled(NETrace::eLogingTypes::LogTypeDebug);
}

void LogConfiguration::setLogEnabled(NETrace::eLogingTypes logType, bool isEnabled)
{
    ConfigManager& config = Application::getConfigManager();
    if (isEnabled && config.getLoggingStatus() == false)
    {
        config.setLoggingStatus(true);
    }

    if (config.getLogEnabled(logType) != isEnabled)
    {
        config.setLogEnabled(logType, isEnabled);
    }
}

Version LogConfiguration::getVersion(void) const
{
    return Application::getConfigManager().getLogVersion();
}

String LogConfiguration::getLayoutEnter(void) const
{
    return Application::getConfigManager().getLogLayoutEnter();
}

void LogConfiguration::setLayoutEnter(const String & prop)
{
    Application::getConfigManager().setLogLayoutEnter(prop);
}

String LogConfiguration::getLayoutMessage(void) const
{
    return Application::getConfigManager().getLogLayoutMessage();
}

void LogConfiguration::setLayoutMessage(const String & prop)
{
    Application::getConfigManager().setLogLayoutMessage(prop);
}

String LogConfiguration::getLayoutExit(void) const
{
    return Application::getConfigManager().getLogLayoutExit();
}

void LogConfiguration::setLayoutExit(const String& prop)
{
    Application::getConfigManager().setLogLayoutExit(prop);
}

uint32_t LogConfiguration::getStackSize(void) const
{
    return Application::getConfigManager().getLogRemoteQueueSize();
}

void LogConfiguration::setStackSize(uint32_t prop)
{
    Application::getConfigManager().setLogRemoteQueueSize(prop);
}

bool LogConfiguration::getStatus(void) const
{
    return Application::getConfigManager().getLoggingStatus();
}

void LogConfiguration::setStatus(bool prop)
{
    Application::getConfigManager().setLoggingStatus(prop);
}

bool LogConfiguration::getAppendData(void) const
{
    return Application::getConfigManager().getLogFileAppend();
}

void LogConfiguration::setAppendData(bool prop)
{
    Application::getConfigManager().setLogFileAppend(prop);
}

String LogConfiguration::getLogFile(void) const
{
    return Application::getConfigManager().getLogFileLocation();
}

void LogConfiguration::setLogFile(const String& prop)
{
    Application::getConfigManager().setLogFileLocation(prop);
}

bool LogConfiguration::getRemoteTcpEnable(void) const
{
    return (Application::getConfigManager().getLogEnabled(NETrace::eLogingTypes::LogTypeRemote) &&
            Application::getConfigManager().getRemoteServiceEnable(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip));
}

void LogConfiguration::setRemoteTcpEnable(bool prop, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setRemoteServiceEnable(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip, prop, isTemporary);
}

String LogConfiguration::getRemoteTcpAddress(void) const
{
    return Application::getConfigManager().getRemoteServiceAddress(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip);
}

void LogConfiguration::setRemoteTcpAddress(const String & prop, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setRemoteServiceAddress(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip, prop, isTemporary);
}

uint16_t LogConfiguration::getRemoteTcpPort(void) const
{
    return Application::getConfigManager().getRemoteServicePort(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip);
}

void LogConfiguration::setRemoteTcpPort(uint16_t prop, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setRemoteServicePort(NERemoteService::eRemoteServices::ServiceLogger, NERemoteService::eConnectionTypes::ConnectTcpip, prop, isTemporary);
}

bool LogConfiguration::getDatabaseEnable(void) const
{
    return Application::getConfigManager().getLogEnabled(NETrace::eLogingTypes::LogTypeDatabase);
}

void LogConfiguration::setDatabaseEnable(bool prop, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setLogEnabled(NETrace::eLogingTypes::LogTypeDatabase, prop, isTemporary);
}

uint32_t LogConfiguration::getModuleScopes(std::vector<Property>& scopeList)
{
    scopeList = Application::getConfigManager().getModuleLogScopes();
    return static_cast<uint32_t>(scopeList.size());
}

void LogConfiguration::setModuleScopes(const std::vector<Property>& scopeList)
{
    Application::getConfigManager().removeModuleScopes();
    Application::getConfigManager().addModuletLogScopes(scopeList, true);
}

String LogConfiguration::getDatabaseName(void) const
{
    return Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabaseName().position);
}

void LogConfiguration::setDatabaseName(const String dbName, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabaseName().position, dbName, isTemporary);
}

String LogConfiguration::getDatabaseLocation(void) const
{
    return Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabaseLocation().position);
}

void LogConfiguration::setDatabaseLocation(const String& dbLocation, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabaseLocation().position, dbLocation, isTemporary);
}

String LogConfiguration::getDatabaseDriver(void) const
{
    return Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabaseDriver().position);
}

void LogConfiguration::setDatabaseDriver(const String& dbDriver, bool isTemporary /*= false*/)
{
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabaseDriver().position, dbDriver, isTemporary);
}

NESocket::SocketAddress LogConfiguration::getDatabaseAddress(void) const
{
    String address{ Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabaseAddress().position) };
    uint32_t port{ Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabasePort().position).toUInt32() };

    return NESocket::SocketAddress(address, static_cast<uint16_t>(port));
}

void LogConfiguration::setDatabaseAddress(const NESocket::SocketAddress& dbAddress, bool isTemporary)
{
    setDatabaseAddress(dbAddress.getHostAddress(), dbAddress.getHostPort(), isTemporary);
}

void LogConfiguration::setDatabaseAddress(const String& dbAddress, uint16_t dbPort, bool isTemporary)
{
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabaseAddress().position, dbAddress, isTemporary);
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabasePort().position, String::makeString(static_cast<uint32_t>(dbPort)), isTemporary);
}

NESocket::UserData LogConfiguration::getDatabaseUser(void) const
{
    String user{ Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabaseUser().position) };
    String password{ Application::getConfigManager().getLogDatabaseProperty(NEPersistence::getLogDatabasePassword().position) };
    return NESocket::UserData(user, password);
}

void LogConfiguration::setDatabaseUser(const NESocket::UserData& dbUser, bool isTemporary)
{
    return setDatabaseUser(dbUser.getUser(), dbUser.getPassword(), isTemporary);
}

void LogConfiguration::setDatabaseUser(const String& dbUserName, const String& dbUserPassword, bool isTemporary)
{
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabaseUser().position, dbUserName, isTemporary);
    Application::getConfigManager().setLogDatabaseProperty(NEPersistence::getLogDatabasePassword().position, dbUserPassword, isTemporary);
}

void LogConfiguration::saveConfiguration(void)
{
    Application::getConfigManager().saveConfig();
}

void LogConfiguration::updateScopeConfiguration(const ScopeController& scopeController) const
{
    const auto& scopeList = scopeController.getScopeList();
    unsigned int key{ 0 };
    TraceScope* scope = scopeList.resourceFirstKey(key);
    ScopeRoot root;
    while (scope != nullptr)
    {
        root.addChildRecursive(*scope);
        scope = scopeList.resourceNextKey(key);
    }

    root.groupRecursive();
    ConfigManager & config = Application::getConfigManager();
    config.removeModuleScopes();
    root.updateConfigNode(config, String::EmptyString);
}
