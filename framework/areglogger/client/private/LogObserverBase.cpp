/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LogObserverBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library.
 *              The log observer base class.
 ************************************************************************/

#include "areglogger/client/LogObserverBase.hpp"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/LogObserverApi.h"

#include "areg/base/String.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/SocketDefs.hpp"

namespace areglogger
{

    LogObserverBase* LogObserverBase::_theLogObserver{ nullptr };

    LogObserverBase::LogObserverBase()
    {
        ASSERT(LogObserverBase::_theLogObserver == nullptr);
        LogObserverBase::_theLogObserver = this;
    }

    LogObserverBase::~LogObserverBase()
    {
        ASSERT(LogObserverBase::_theLogObserver != nullptr);
        LogObserverBase::_theLogObserver = nullptr;
    }

    bool LogObserverBase::initialize(const std::string& configFile)
    {
        return areglogger::logObserverInitialize(nullptr, configFile.empty() ? nullptr : configFile.c_str());
    }

    void LogObserverBase::release()
    {
        return areglogger::logObserverRelease();
    }

    bool LogObserverBase::connect(const std::string& address, uint16_t portNr, const std::string& dbLocation)
    {
        return areglogger::logObserverConnectLogger(  dbLocation.empty() ? nullptr : dbLocation.c_str()
                                        , address.empty() ? nullptr : address.c_str()
                                        , portNr);
    }

    void LogObserverBase::disconnect()
    {
        areglogger::logObserverDisconnectLogger();
    }

    bool LogObserverBase::pause()
    {
        return areglogger::logObserverPauseLogging(true);
    }

    bool LogObserverBase::resume()
    {
        return areglogger::logObserverPauseLogging(false);
    }

    bool LogObserverBase::stop()
    {
        return areglogger::logObserverStopLogging(true, nullptr);
    }

    bool LogObserverBase::restart(const std::string & dbLocation /*= areg::String::EmptyString*/)
    {
        return areglogger::logObserverStopLogging(false, dbLocation.empty() ? nullptr : dbLocation.c_str());
    }

    bool LogObserverBase::isInitialized() const
    {
        return areglogger::logObserverIsInitialized();
    }

    bool LogObserverBase::isConnected() const
    {
        return areglogger::logObserverIsConnected();
    }

    bool LogObserverBase::isStated() const
    {
        return areglogger::logObserverIsStarted();
    }

    const areg::SocketAddress& LogObserverBase::getLoggerAddress() const
    {
        return areglogger::LoggerClient::getInstance().getAddress();
    }

    const std::string& LogObserverBase::getLoggerIpAddress() const
    {
        return areglogger::LoggerClient::getInstance().getAddress().getHostAddress().getData();
    }

    const std::string& LogObserverBase::getLoggerHostName() const
    {
        return areglogger::LoggerClient::getInstance().getAddress().getHostName().getData();
    }

    uint16_t LogObserverBase::getLoggerPort() const
    {
        return areglogger::LoggerClient::getInstance().getAddress().getHostPort();
    }

    bool LogObserverBase::getConfigLoggerEnabled() const
    {
        return areglogger::logObserverConfigLoggerEnabled();
    }

    std::string LogObserverBase::getConfigLoggerAddress() const
    {
        return areglogger::LoggerClient::getInstance().getConfigLoggerAddress().getData();
    }

    void LogObserverBase::setConfigLoggerAddress(const std::string& address)
    {
        areglogger::LoggerClient::getInstance().setConfigLoggerConnection(address, areg::InvalidPort);
    }

    uint16_t LogObserverBase::getConfigLoggerPort() const
    {
        return areglogger::LoggerClient::getInstance().getConfigLoggerPort();
    }

    void LogObserverBase::setConfigLoggerPort(uint16_t portNr)
    {
        areglogger::LoggerClient::getInstance().setConfigLoggerConnection(areg::String::EmptyString, portNr);
    }

    void LogObserverBase::setConfigLoggerConnection(const std::string& address, uint16_t portNr)
    {
        areglogger::LoggerClient::getInstance().setConfigLoggerConnection(address, portNr);
    }

    std::string LogObserverBase::getConfigLoggerDatabase() const
    {
        return areglogger::LoggerClient::getInstance().getConfigDatabasePath().getData();
    }

    void LogObserverBase::setConfigLoggerDatabase(const std::string& dbFilePath)
    {
        areglogger::LoggerClient::getInstance().setConfigDatabasePath(dbFilePath, true);
    }

    std::string LogObserverBase::getConfigLoggerDatabaseLocation() const
    {
        return areglogger::LoggerClient::getInstance().getConfigDatabaseLocation().getData();
    }

    void LogObserverBase::setConfigLoggerDatabaseLocation(const std::string& dbLocation)
    {
        areglogger::LoggerClient::getInstance().setConfigDatabaseLocation(dbLocation);
    }

    std::string LogObserverBase::getConfigLoggerDatabaseName() const
    {
        return areglogger::LoggerClient::getInstance().getConfigDatabaseName().getData();
    }

    void LogObserverBase::setConfigLoggerDatabaseName(const std::string& dbName)
    {
        areglogger::LoggerClient::getInstance().setConfigDatabaseName(dbName);
    }

    std::string LogObserverBase::getActiveDatabasePath() const
    {
        return areglogger::LoggerClient::getInstance().getActiveDatabasePath().getData();
    }

    std::string LogObserverBase::getInitDatabasePath() const
    {
        return areglogger::LoggerClient::getInstance().getInitialDatabasePath().getData();
    }

    bool LogObserverBase::requestInstances()
    {
        return areglogger::logObserverRequestInstances();
    }

    bool LogObserverBase::requestScopes(ITEM_ID target /*= areg::TARGET_ALL*/)
    {
        return areglogger::logObserverRequestScopes(target);
    }

    bool LogObserverBase::requestChangeScopePrio(ITEM_ID target, const areglogger::ScopeInfo* scopes, uint32_t count)
    {
        return areglogger::logObserverRequestChangeScopePrio(target, scopes, count);
    }

    bool LogObserverBase::requestSaveConfig(ITEM_ID target /*= areg::TARGET_ALL*/)
    {
        return areglogger::logObserverRequestSaveConfig(target);
    }

    void LogObserverBase::saveLoggerConfig()
    {
        areglogger::LoggerClient::getInstance().saveConfiguration();
    }

    void LogObserverBase::getLogInstanceNames(std::vector<areg::String>& names)
    {
        areglogger::LoggerClient::getInstance().getLogInstanceNames(names);
    }

    void LogObserverBase::getLogInstances(std::vector<ITEM_ID>& ids)
    {
        areglogger::LoggerClient::getInstance().getLogInstances(ids);
    }

    void LogObserverBase::getLogThreadNames(std::vector<areg::String>& names)
    {
        areglogger::LoggerClient::getInstance().getLogThreadNames(names);
    }

    void LogObserverBase::getLogThreads(std::vector<ITEM_ID>& ids)
    {
        areglogger::LoggerClient::getInstance().getLogThreads(ids);
    }

    void LogObserverBase::getPriorityNames(std::vector<areg::String>& names)
    {
        areglogger::LoggerClient::getInstance().getPriorityNames(names);
    }

    void LogObserverBase::getLogInstanceInfos(std::vector< areg::ConnectedInstance>& infos)
    {
        areglogger::LoggerClient::getInstance().getLogInstanceInfos(infos);
    }

    void LogObserverBase::getLogInstScopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId)
    {
        areglogger::LoggerClient::getInstance().getLogInstScopes(scopes, instId);
    }

    void LogObserverBase::getLogMessages(std::vector<areg::SharedBuffer>& messages)
    {
        areglogger::LoggerClient::getInstance().getLogMessages(messages);
    }

    void LogObserverBase::getLogInstMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId /*= areg::COOKIE_ANY*/)
    {
        areglogger::LoggerClient::getInstance().getLogInstMessages(messages, instId);
    }

    void LogObserverBase::getLogScopeMessages(std::vector<areg::SharedBuffer>& messages, uint32_t scopeId /*= 0*/)
    {
        areglogger::LoggerClient::getInstance().getLogScopeMessages(messages, scopeId);
    }

    void LogObserverBase::getLogMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
    {
        areglogger::LoggerClient::getInstance().getLogMessages(messages, instId, scopeId);
    }
} // namespace areglogger
