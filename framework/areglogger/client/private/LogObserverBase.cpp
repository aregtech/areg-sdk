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
    return logObserverInitialize(nullptr, configFile.empty() ? nullptr : configFile.c_str());
}

void LogObserverBase::release()
{
    return logObserverRelease();
}

bool LogObserverBase::connect(const std::string& address, uint16_t portNr, const std::string& dbLocation)
{
    return logObserverConnectLogger(  dbLocation.empty() ? nullptr : dbLocation.c_str()
                                    , address.empty() ? nullptr : address.c_str()
                                    , portNr);
}

void LogObserverBase::disconnect()
{
    logObserverDisconnectLogger();
}

bool LogObserverBase::pause()
{
    return logObserverPauseLogging(true);
}

bool LogObserverBase::resume()
{
    return logObserverPauseLogging(false);
}

bool LogObserverBase::stop()
{
    return logObserverStopLogging(true, nullptr);
}

bool LogObserverBase::restart(const std::string & dbLocation /*= String::EmptyString*/)
{
    return logObserverStopLogging(false, dbLocation.empty() ? nullptr : dbLocation.c_str());
}

bool LogObserverBase::isInitialized() const
{
    return logObserverIsInitialized();
}

bool LogObserverBase::is_connected() const
{
    return logObserverIsConnected();
}

bool LogObserverBase::isStated() const
{
    return logObserverIsStarted();
}

const NESocket::SocketAddress& LogObserverBase::getLoggerAddress() const
{
    return LoggerClient::instance().address();
}

const std::string& LogObserverBase::getLoggerIpAddress() const
{
    return LoggerClient::instance().address().host_address().data();
}

const std::string& LogObserverBase::getLoggerHostName() const
{
    return LoggerClient::instance().address().host_name().data();
}

uint16_t LogObserverBase::getLoggerPort() const
{
    return LoggerClient::instance().address().host_port();
}

bool LogObserverBase::getConfigLoggerEnabled() const
{
    return logObserverConfigLoggerEnabled();
}

std::string LogObserverBase::getConfigLoggerAddress() const
{
    return LoggerClient::instance().getConfigLoggerAddress().data();
}

void LogObserverBase::setConfigLoggerAddress(const std::string& address)
{
    LoggerClient::instance().setConfigLoggerConnection(address, NESocket::InvalidPort);
}

uint16_t LogObserverBase::getConfigLoggerPort() const
{
    return LoggerClient::instance().getConfigLoggerPort();
}

void LogObserverBase::setConfigLoggerPort(uint16_t portNr)
{
    LoggerClient::instance().setConfigLoggerConnection(String::EmptyString, portNr);
}

void LogObserverBase::setConfigLoggerConnection(const std::string& address, uint16_t portNr)
{
    LoggerClient::instance().setConfigLoggerConnection(address, portNr);
}

std::string LogObserverBase::getConfigLoggerDatabase() const
{
    return LoggerClient::instance().getConfigDatabasePath().data();
}

void LogObserverBase::setConfigLoggerDatabase(const std::string& dbFilePath)
{
    LoggerClient::instance().setConfigDatabasePath(dbFilePath, true);
}

std::string LogObserverBase::getConfigLoggerDatabaseLocation() const
{
    return LoggerClient::instance().getConfigDatabaseLocation().data();
}

void LogObserverBase::setConfigLoggerDatabaseLocation(const std::string& dbLocation)
{
    LoggerClient::instance().setConfigDatabaseLocation(dbLocation);
}

std::string LogObserverBase::getConfigLoggerDatabaseName() const
{
    return LoggerClient::instance().getConfigDatabaseName().data();
}

void LogObserverBase::setConfigLoggerDatabaseName(const std::string& dbName)
{
    LoggerClient::instance().setConfigDatabaseName(dbName);
}

std::string LogObserverBase::getActiveDatabasePath() const
{
    return LoggerClient::instance().getActiveDatabasePath().data();
}

std::string LogObserverBase::getInitDatabasePath() const
{
    return LoggerClient::instance().getInitialDatabasePath().data();
}

bool LogObserverBase::requestInstances()
{
    return logObserverRequestInstances();
}

bool LogObserverBase::requestScopes(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestScopes(target);
}

bool LogObserverBase::requestChangeScopePrio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count)
{
    return logObserverRequestChangeScopePrio(target, scopes, count);
}

bool LogObserverBase::requestSaveConfig(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestSaveConfig(target);
}

void LogObserverBase::saveLoggerConfig()
{
    LoggerClient::instance().save_configuration();
}

void LogObserverBase::getLogInstanceNames(std::vector<String>& names)
{
    LoggerClient::instance().getLogInstanceNames(names);
}

void LogObserverBase::getLogInstances(std::vector<ITEM_ID>& ids)
{
    LoggerClient::instance().getLogInstances(ids);
}

void LogObserverBase::getLogThreadNames(std::vector<String>& names)
{
    LoggerClient::instance().getLogThreadNames(names);
}

void LogObserverBase::getLogThreads(std::vector<ITEM_ID>& ids)
{
    LoggerClient::instance().getLogThreads(ids);
}

void LogObserverBase::getPriorityNames(std::vector<String>& names)
{
    LoggerClient::instance().getPriorityNames(names);
}

void LogObserverBase::getLogInstanceInfos(std::vector< NEService::ConnectedInstance>& infos)
{
    LoggerClient::instance().getLogInstanceInfos(infos);
}

void LogObserverBase::getLogInstScopes(std::vector<NELogging::ScopeEntry>& scopes, ITEM_ID instId)
{
    LoggerClient::instance().getLogInstScopes(scopes, instId);
}

void LogObserverBase::getLogMessages(std::vector<SharedBuffer>& messages)
{
    LoggerClient::instance().getLogMessages(messages);
}

void LogObserverBase::getLogInstMessages(std::vector<SharedBuffer>& messages, ITEM_ID instId /*= NEService::COOKIE_ANY*/)
{
    LoggerClient::instance().getLogInstMessages(messages, instId);
}

void LogObserverBase::getLogScopeMessages(std::vector<SharedBuffer>& messages, uint32_t scopeId /*= 0*/)
{
    LoggerClient::instance().getLogScopeMessages(messages, scopeId);
}

void LogObserverBase::getLogMessages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
{
    LoggerClient::instance().getLogMessages(messages, instId, scopeId);
}
