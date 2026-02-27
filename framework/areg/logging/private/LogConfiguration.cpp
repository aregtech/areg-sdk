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
        return Application::getConfigManager().getLoggingStatus();
    }

    bool LogConfiguration::isRemoteLoggingEnabled() const
    {
        ConfigManager& config = Application::getConfigManager();
        return config.getLogEnabled(LogTarget::Remote);
    }

    bool LogConfiguration::isFileLoggingEnabled() const
    {
        ConfigManager& config = Application::getConfigManager();
        return config.getLogEnabled(LogTarget::File);
    }

    bool LogConfiguration::isDatabaseLoggingEnabled() const
    {
        ConfigManager& config = Application::getConfigManager();
        return config.getLogEnabled(LogTarget::Database);
    }

    bool LogConfiguration::isDebugOutputLoggingEnabled() const
    {
        ConfigManager& config = Application::getConfigManager();
        return config.getLogEnabled(LogTarget::Debug);
    }

    void LogConfiguration::setLogEnabled(LogTarget logType, bool isEnabled)
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

    Version LogConfiguration::getVersion() const
    {
        return Application::getConfigManager().getLogVersion();
    }

    String LogConfiguration::getLayoutEnter() const
    {
        return Application::getConfigManager().getLogLayoutEnter();
    }

    void LogConfiguration::setLayoutEnter(const String & prop)
    {
        Application::getConfigManager().setLogLayoutEnter(prop);
    }

    String LogConfiguration::getLayoutMessage() const
    {
        return Application::getConfigManager().getLogLayoutMessage();
    }

    void LogConfiguration::setLayoutMessage(const String & prop)
    {
        Application::getConfigManager().setLogLayoutMessage(prop);
    }

    String LogConfiguration::getLayoutExit() const
    {
        return Application::getConfigManager().getLogLayoutExit();
    }

    void LogConfiguration::setLayoutExit(const String& prop)
    {
        Application::getConfigManager().setLogLayoutExit(prop);
    }

    uint32_t LogConfiguration::getStackSize() const
    {
        return Application::getConfigManager().getLogRemoteQueueSize();
    }

    void LogConfiguration::setStackSize(uint32_t prop)
    {
        Application::getConfigManager().setLogRemoteQueueSize(prop);
    }

    bool LogConfiguration::getStatus() const
    {
        return Application::getConfigManager().getLoggingStatus();
    }

    void LogConfiguration::setStatus(bool prop)
    {
        Application::getConfigManager().setLoggingStatus(prop);
    }

    bool LogConfiguration::getAppendData() const
    {
        return Application::getConfigManager().getLogFileAppend();
    }

    void LogConfiguration::setAppendData(bool prop)
    {
        Application::getConfigManager().setLogFileAppend(prop);
    }

    String LogConfiguration::getLogFile() const
    {
        return Application::getConfigManager().getLogFileLocation();
    }

    void LogConfiguration::setLogFile(const String& prop)
    {
        Application::getConfigManager().setLogFileLocation(prop);
    }

    bool LogConfiguration::getRemoteTcpEnable() const
    {
        return (Application::getConfigManager().getLogEnabled(LogTarget::Remote) &&
                Application::getConfigManager().getRemoteServiceEnable(RemoteServiceKind::Logger, ConnectionType::Tcpip));
    }

    void LogConfiguration::setRemoteTcpEnable(bool prop, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setRemoteServiceEnable(RemoteServiceKind::Logger, ConnectionType::Tcpip, prop, isTemporary);
    }

    String LogConfiguration::getRemoteTcpAddress() const
    {
        return Application::getConfigManager().getRemoteServiceAddress(RemoteServiceKind::Logger, ConnectionType::Tcpip);
    }

    void LogConfiguration::setRemoteTcpAddress(const String & prop, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setRemoteServiceAddress(RemoteServiceKind::Logger, ConnectionType::Tcpip, prop, isTemporary);
    }

    uint16_t LogConfiguration::getRemoteTcpPort() const
    {
        return Application::getConfigManager().getRemoteServicePort(RemoteServiceKind::Logger, ConnectionType::Tcpip);
    }

    void LogConfiguration::setRemoteTcpPort(uint16_t prop, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setRemoteServicePort(RemoteServiceKind::Logger, ConnectionType::Tcpip, prop, isTemporary);
    }

    bool LogConfiguration::getDatabaseEnable() const
    {
        return Application::getConfigManager().getLogEnabled(LogTarget::Database);
    }

    void LogConfiguration::setDatabaseEnable(bool prop, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setLogEnabled(LogTarget::Database, prop, isTemporary);
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

    String LogConfiguration::getDatabaseEngine() const
    {
        return Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseEngine().position);
    }

    void LogConfiguration::setDatabaseEngine(const String& dbEngine, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseEngine().position, dbEngine, isTemporary);
    }

    String LogConfiguration::getDatabaseFullPath() const
    {
        String dbLocation = Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseLocation().position);
        String dbName = Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseName().position);
        return File::makeFileFullPath(dbLocation, dbName);
    }

    void LogConfiguration::setDatabaseFullPath(const String& dbFullPath, bool isTemporary)
    {
        String dbLocation = File::getFileDirectory(dbFullPath.getString());
        String dbName = File::getFileNameWithExtension(dbFullPath.getString());

        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseLocation().position, dbLocation, isTemporary);
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseName().position, dbName, isTemporary);
    }

    String LogConfiguration::getDatabaseName() const
    {
        return Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseName().position);
    }

    void LogConfiguration::setDatabaseName(const String & dbName, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseName().position, dbName, isTemporary);
    }

    String LogConfiguration::getDatabaseLocation() const
    {
        return Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseLocation().position);
    }

    void LogConfiguration::setDatabaseLocation(const String & dbLocation, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseLocation().position, dbLocation, isTemporary);
    }

    String LogConfiguration::getDatabaseDriver() const
    {
        return Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseDriver().position);
    }

    void LogConfiguration::setDatabaseDriver(const String& dbDriver, bool isTemporary /*= false*/)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseDriver().position, dbDriver, isTemporary);
    }

    SocketAddress LogConfiguration::getDatabaseAddress() const
    {
        String address{ Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseAddress().position) };
        uint32_t port{ Application::getConfigManager().getLogDatabaseProperty(getLogDatabasePort().position).toUInt32() };

        return SocketAddress(address, static_cast<uint16_t>(port));
    }

    void LogConfiguration::setDatabaseAddress(const SocketAddress& dbAddress, bool isTemporary)
    {
        setDatabaseAddress(dbAddress.getHostAddress(), dbAddress.getHostPort(), isTemporary);
    }

    void LogConfiguration::setDatabaseAddress(const String& dbAddress, uint16_t dbPort, bool isTemporary)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseAddress().position, dbAddress, isTemporary);
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabasePort().position, String::makeString(static_cast<uint32_t>(dbPort)), isTemporary);
    }

    UserData LogConfiguration::getDatabaseUser() const
    {
        String user{ Application::getConfigManager().getLogDatabaseProperty(getLogDatabaseUser().position) };
        String password{ Application::getConfigManager().getLogDatabaseProperty(getLogDatabasePassword().position) };
        return UserData(user, password);
    }

    void LogConfiguration::setDatabaseUser(const UserData& dbUser, bool isTemporary)
    {
        return setDatabaseUser(dbUser.getUser(), dbUser.getPassword(), isTemporary);
    }

    void LogConfiguration::setDatabaseUser(const String& dbUserName, const String& dbUserPassword, bool isTemporary)
    {
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabaseUser().position, dbUserName, isTemporary);
        Application::getConfigManager().setLogDatabaseProperty(getLogDatabasePassword().position, dbUserPassword, isTemporary);
    }

    void LogConfiguration::saveConfiguration()
    {
        Application::getConfigManager().saveConfig();
    }

    #if AREG_LOGS

    void LogConfiguration::updateScopeConfiguration(const ScopeController& scopeController) const
    {
        const auto& scopeList = scopeController.getScopeList();
        uint32_t key{ 0 };
        LogScope* scope = scopeList.resourceFirstKey(key);
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

    #else   // AREG_LOGS

    void LogConfiguration::updateScopeConfiguration(const ScopeController& /*scopeController*/) const
    {
    }

    #endif  // AREG_LOGS
} // namespace areg
