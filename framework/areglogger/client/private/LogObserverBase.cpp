/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LogObserverBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library.
 *              The log observer base class.
 ************************************************************************/

#include "areglogger/client/LogObserverBase.hpp"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/LogObserverApi.h"

#include "areg/base/String.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/NESocket.hpp"

LogObserverBase* LogObserverBase::_theLogObserver{ nullptr };

LogObserverBase::LogObserverBase(void)
{
    ASSERT(LogObserverBase::_theLogObserver == nullptr);
    LogObserverBase::_theLogObserver = this;
}

LogObserverBase::~LogObserverBase(void)
{
    ASSERT(LogObserverBase::_theLogObserver != nullptr);
    LogObserverBase::_theLogObserver = nullptr;
}

bool LogObserverBase::initialize(const std::string& configFile)
{
    return logObserverInitialize(nullptr, configFile.empty() ? nullptr : configFile.c_str());
}

void LogObserverBase::release(void)
{
    return logObserverRelease();
}

bool LogObserverBase::connect(const std::string& address, uint16_t portNr, const std::string& dbLocation)
{
    return logObserverConnectLogger(  dbLocation.empty() ? nullptr : dbLocation.c_str()
                                    , address.empty() ? nullptr : address.c_str()
                                    , portNr);
}

void LogObserverBase::disconnect(void)
{
    logObserverDisconnectLogger();
}

bool LogObserverBase::pause(void)
{
    return logObserverPauseLogging(true);
}

bool LogObserverBase::resume(void)
{
    return logObserverPauseLogging(false);
}

bool LogObserverBase::stop(void)
{
    return logObserverStopLogging(true, nullptr);
}

bool LogObserverBase::restart(const std::string & dbLocation /*= String::EmptyString*/)
{
    return logObserverStopLogging(false, dbLocation.empty() ? nullptr : dbLocation.c_str());
}

bool LogObserverBase::isInitialized(void) const
{
    return logObserverIsInitialized();
}

bool LogObserverBase::isConnected(void) const
{
    return logObserverIsConnected();
}

bool LogObserverBase::isStated(void) const
{
    return logObserverIsStarted();
}

const NESocket::SocketAddress& LogObserverBase::getLoggerAddress(void) const
{
    return LoggerClient::getInstance().getAddress();
}

const std::string& LogObserverBase::getLoggerIpAddress(void) const
{
    return LoggerClient::getInstance().getAddress().getHostAddress().getData();
}

const std::string& LogObserverBase::getLoggerHostName(void) const
{
    return LoggerClient::getInstance().getAddress().getHostName().getData();
}

uint16_t LogObserverBase::getLoggerPort(void) const
{
    return LoggerClient::getInstance().getAddress().getHostPort();
}

bool LogObserverBase::getConfigLoggerEnabled(void) const
{
    return logObserverConfigLoggerEnabled();
}

const std::string LogObserverBase::getConfigLoggerAddress(void) const
{
    return LoggerClient::getInstance().getConfigLoggerAddress().getData();
}

void LogObserverBase::setConfigLoggerAddress(const std::string& address)
{
    LoggerClient::getInstance().setConfigLoggerConnection(address, NESocket::InvalidPort);
}

uint16_t LogObserverBase::getConfigLoggerPort(void) const
{
    return LoggerClient::getInstance().getConfigLoggerPort();
}

void LogObserverBase::setConfigLoggerPort(uint16_t portNr)
{
    LoggerClient::getInstance().setConfigLoggerConnection(String::EmptyString, portNr);
}

void LogObserverBase::setConfigLoggerConnection(const std::string& address, uint16_t portNr)
{
    LoggerClient::getInstance().setConfigLoggerConnection(address, portNr);
}

std::string LogObserverBase::getConfigLoggerDatabase(void) const
{
    return LoggerClient::getInstance().getConfigDatabasePath().getData();
}

void LogObserverBase::setConfigLoggerDatabase(const std::string& dbFilePath)
{
    LoggerClient::getInstance().setConfigDatabasePath(dbFilePath, true);
}

std::string LogObserverBase::getConfigLoggerDatabaseLocation(void) const
{
    return LoggerClient::getInstance().getConfigDatabaseLocation().getData();
}

void LogObserverBase::setConfigLoggerDatabaseLocation(const std::string& dbLocation)
{
    LoggerClient::getInstance().setConfigDatabaseLocation(dbLocation);
}

std::string LogObserverBase::getConfigLoggerDatabaseName(void) const
{
    return LoggerClient::getInstance().getConfigDatabaseName().getData();
}

void LogObserverBase::setConfigLoggerDatabaseName(const std::string& dbName)
{
    LoggerClient::getInstance().setConfigDatabaseName(dbName);
}

std::string LogObserverBase::getActiveDatabasePath(void) const
{
    return LoggerClient::getInstance().getActiveDatabasePath().getData();
}

std::string LogObserverBase::getInitDatabasePath(void) const
{
    return LoggerClient::getInstance().getInitialDatabasePath().getData();
}

bool LogObserverBase::requestInstances(void)
{
    return logObserverRequestInstances();
}

bool LogObserverBase::requestScopes(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestScopes(target);
}

bool LogObserverBase::requestChangeScopePrio(ITEM_ID target, const sLogScope* scopes, uint32_t count)
{
    return logObserverRequestChangeScopePrio(target, scopes, count);
}

bool LogObserverBase::requestSaveConfig(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestSaveConfig(target);
}

void LogObserverBase::saveLoggerConfig(void)
{
    LoggerClient::getInstance().saveConfiguration();
}
