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

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // LogConfiguration class implementation
    //////////////////////////////////////////////////////////////////////////

    bool LogConfiguration::isLoggingEnabled() const
    {
        return areg::Application::getConfigManager().getLoggingStatus();
    }

    bool LogConfiguration::isRemoteLoggingEnabled() const
    {
        areg::ConfigManager& config = areg::Application::getConfigManager();
        return config.getLogEnabled(areg::LogTarget::Remote);
    }

    bool LogConfiguration::isFileLoggingEnabled() const
    {
        areg::ConfigManager& config = areg::Application::getConfigManager();
        return config.getLogEnabled(areg::LogTarget::File);
    }

    bool LogConfiguration::isDatabaseLoggingEnabled() const
    {
        areg::ConfigManager& config = areg::Application::getConfigManager();
        return config.getLogEnabled(areg::LogTarget::Database);
    }

    bool LogConfiguration::isDebugOutputLoggingEnabled() const
    {
        areg::ConfigManager& config = areg::Application::getConfigManager();
        return config.getLogEnabled(areg::LogTarget::Debug);
    }

    void LogConfiguration::setLogEnabled(areg::LogTarget logType, bool isEnabled)
    {
        areg::ConfigManager& config = areg::Application::getConfigManager();
        if (isEnabled && config.getLoggingStatus() == false)
        {
            config.setLoggingStatus(true);
        }

        if (config.getLogEnabled(logType) != isEnabled)
        {
            config.setLogEnabled(logType, isEnabled);
        }
    }

    areg::Version LogConfiguration::getVersion() const
    {
        return areg::Application::getConfigManager().getLogVersion();
    }

    areg::String LogConfiguration::getLayoutEnter() const
    {
        return areg::Application::getConfigManager().getLogLayoutEnter();
    }

    void LogConfiguration::setLayoutEnter(const areg::String & prop)
    {
        areg::Application::getConfigManager().setLogLayoutEnter(prop);
    }

    areg::String LogConfiguration::getLayoutMessage() const
    {
        return areg::Application::getConfigManager().getLogLayoutMessage();
    }

    void LogConfiguration::setLayoutMessage(const areg::String & prop)
    {
        areg::Application::getConfigManager().setLogLayoutMessage(prop);
    }

    areg::String LogConfiguration::getLayoutExit() const
    {
        return areg::Application::getConfigManager().getLogLayoutExit();
    }

    void LogConfiguration::setLayoutExit(const areg::String& prop)
    {
        areg::Application::getConfigManager().setLogLayoutExit(prop);
    }

    uint32_t LogConfiguration::getStackSize() const
    {
        return areg::Application::getConfigManager().getLogRemoteQueueSize();
    }

    void LogConfiguration::setStackSize(uint32_t prop)
    {
        areg::Application::getConfigManager().setLogRemoteQueueSize(prop);
    }

    bool LogConfiguration::getStatus() const
    {
        return areg::Application::getConfigManager().getLoggingStatus();
    }

    void LogConfiguration::setStatus(bool prop)
    {
        areg::Application::getConfigManager().setLoggingStatus(prop);
    }

    bool LogConfiguration::getAppendData() const
    {
        return areg::Application::getConfigManager().getLogFileAppend();
    }

    void LogConfiguration::setAppendData(bool prop)
    {
        areg::Application::getConfigManager().setLogFileAppend(prop);
    }

    areg::String LogConfiguration::getLogFile() const
    {
        return areg::Application::getConfigManager().getLogFileLocation();
    }

    void LogConfiguration::setLogFile(const areg::String& prop)
    {
        areg::Application::getConfigManager().setLogFileLocation(prop);
    }

    bool LogConfiguration::getRemoteTcpEnable() const
    {
        return (areg::Application::getConfigManager().getLogEnabled(areg::LogTarget::Remote) &&
                areg::Application::getConfigManager().getRemoteServiceEnable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip));
    }

    void LogConfiguration::setRemoteTcpEnable(bool prop, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setRemoteServiceEnable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
    }

    areg::String LogConfiguration::getRemoteTcpAddress() const
    {
        return areg::Application::getConfigManager().getRemoteServiceAddress(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
    }

    void LogConfiguration::setRemoteTcpAddress(const areg::String & prop, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setRemoteServiceAddress(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
    }

    uint16_t LogConfiguration::getRemoteTcpPort() const
    {
        return areg::Application::getConfigManager().getRemoteServicePort(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip);
    }

    void LogConfiguration::setRemoteTcpPort(uint16_t prop, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setRemoteServicePort(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, prop, isTemporary);
    }

    bool LogConfiguration::getDatabaseEnable() const
    {
        return areg::Application::getConfigManager().getLogEnabled(areg::LogTarget::Database);
    }

    void LogConfiguration::setDatabaseEnable(bool prop, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setLogEnabled(areg::LogTarget::Database, prop, isTemporary);
    }

    uint32_t LogConfiguration::getModuleScopes(std::vector<areg::Property>& scopeList)
    {
        scopeList = areg::Application::getConfigManager().getModuleLogScopes();
        return static_cast<uint32_t>(scopeList.size());
    }

    void LogConfiguration::setModuleScopes(const std::vector<areg::Property>& scopeList)
    {
        areg::Application::getConfigManager().removeModuleScopes();
        areg::Application::getConfigManager().addModuletLogScopes(scopeList, true);
    }

    areg::String LogConfiguration::getDatabaseEngine() const
    {
        return areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseEngine().position);
    }

    void LogConfiguration::setDatabaseEngine(const areg::String& dbEngine, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseEngine().position, dbEngine, isTemporary);
    }

    areg::String LogConfiguration::getDatabaseFullPath() const
    {
        areg::String dbLocation = areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseLocation().position);
        areg::String dbName = areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseName().position);
        return areg::File::makeFileFullPath(dbLocation, dbName);
    }

    void LogConfiguration::setDatabaseFullPath(const areg::String& dbFullPath, bool isTemporary)
    {
        areg::String dbLocation = areg::File::getFileDirectory(dbFullPath.getString());
        areg::String dbName = areg::File::getFileNameWithExtension(dbFullPath.getString());

        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseLocation().position, dbLocation, isTemporary);
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseName().position, dbName, isTemporary);
    }

    areg::String LogConfiguration::getDatabaseName() const
    {
        return areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseName().position);
    }

    void LogConfiguration::setDatabaseName(const areg::String & dbName, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseName().position, dbName, isTemporary);
    }

    areg::String LogConfiguration::getDatabaseLocation() const
    {
        return areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseLocation().position);
    }

    void LogConfiguration::setDatabaseLocation(const areg::String & dbLocation, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseLocation().position, dbLocation, isTemporary);
    }

    areg::String LogConfiguration::getDatabaseDriver() const
    {
        return areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseDriver().position);
    }

    void LogConfiguration::setDatabaseDriver(const areg::String& dbDriver, bool isTemporary /*= false*/)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseDriver().position, dbDriver, isTemporary);
    }

    areg::SocketAddress LogConfiguration::getDatabaseAddress() const
    {
        areg::String address{ areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseAddress().position) };
        uint32_t port{ areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabasePort().position).toUInt32() };

        return areg::SocketAddress(address, static_cast<uint16_t>(port));
    }

    void LogConfiguration::setDatabaseAddress(const areg::SocketAddress& dbAddress, bool isTemporary)
    {
        setDatabaseAddress(dbAddress.getHostAddress(), dbAddress.getHostPort(), isTemporary);
    }

    void LogConfiguration::setDatabaseAddress(const areg::String& dbAddress, uint16_t dbPort, bool isTemporary)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseAddress().position, dbAddress, isTemporary);
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabasePort().position, areg::String::makeString(static_cast<uint32_t>(dbPort)), isTemporary);
    }

    areg::UserData LogConfiguration::getDatabaseUser() const
    {
        areg::String user{ areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabaseUser().position) };
        areg::String password{ areg::Application::getConfigManager().getLogDatabaseProperty(areg::getLogDatabasePassword().position) };
        return areg::UserData(user, password);
    }

    void LogConfiguration::setDatabaseUser(const areg::UserData& dbUser, bool isTemporary)
    {
        return setDatabaseUser(dbUser.getUser(), dbUser.getPassword(), isTemporary);
    }

    void LogConfiguration::setDatabaseUser(const areg::String& dbUserName, const areg::String& dbUserPassword, bool isTemporary)
    {
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabaseUser().position, dbUserName, isTemporary);
        areg::Application::getConfigManager().setLogDatabaseProperty(areg::getLogDatabasePassword().position, dbUserPassword, isTemporary);
    }

    void LogConfiguration::saveConfiguration()
    {
        areg::Application::getConfigManager().saveConfig();
    }

    #if AREG_LOGS

    void LogConfiguration::updateScopeConfiguration(const ScopeController& scopeController) const
    {
        const auto& scopeList = scopeController.getScopeList();
        uint32_t key{ 0 };
        areg::LogScope* scope = scopeList.resourceFirstKey(key);
        ScopeRoot root;
        while (scope != nullptr)
        {
            root.addChildRecursive(*scope);
            scope = scopeList.resourceNextKey(key);
        }

        root.groupRecursive();
        areg::ConfigManager & config = areg::Application::getConfigManager();
        config.removeModuleScopes();
        root.updateConfigNode(config, areg::String::EmptyString);
    }

    #else   // AREG_LOGS

    void LogConfiguration::updateScopeConfiguration(const ScopeController& /*scopeController*/) const
    {
    }

    #endif  // AREG_LOGS
} // namespace areg
