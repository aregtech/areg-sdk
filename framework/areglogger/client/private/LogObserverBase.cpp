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

bool LogObserverBase::is_initialized() const
{
    return logObserverIsInitialized();
}

bool LogObserverBase::is_connected() const
{
    return logObserverIsConnected();
}

bool LogObserverBase::is_stated() const
{
    return logObserverIsStarted();
}

const NESocket::SocketAddress& LogObserverBase::logger_address() const
{
    return LoggerClient::instance().address();
}

const std::string& LogObserverBase::logger_ip_address() const
{
    return LoggerClient::instance().address().host_address().data();
}

const std::string& LogObserverBase::logger_host_name() const
{
    return LoggerClient::instance().address().host_name().data();
}

uint16_t LogObserverBase::logger_port() const
{
    return LoggerClient::instance().address().host_port();
}

bool LogObserverBase::config_logger_enabled() const
{
    return logObserverConfigLoggerEnabled();
}

std::string LogObserverBase::config_logger_address() const
{
    return LoggerClient::instance().config_logger_address().data();
}

void LogObserverBase::set_config_logger_address(const std::string& address)
{
    LoggerClient::instance().set_config_logger_connection(address, NESocket::InvalidPort);
}

uint16_t LogObserverBase::config_logger_port() const
{
    return LoggerClient::instance().config_logger_port();
}

void LogObserverBase::set_config_logger_port(uint16_t portNr)
{
    LoggerClient::instance().set_config_logger_connection(String::EmptyString, portNr);
}

void LogObserverBase::set_config_logger_connection(const std::string& address, uint16_t portNr)
{
    LoggerClient::instance().set_config_logger_connection(address, portNr);
}

std::string LogObserverBase::config_logger_database() const
{
    return LoggerClient::instance().config_database_path().data();
}

void LogObserverBase::set_config_logger_database(const std::string& dbFilePath)
{
    LoggerClient::instance().set_config_database_path(dbFilePath, true);
}

std::string LogObserverBase::config_logger_database_location() const
{
    return LoggerClient::instance().config_database_location().data();
}

void LogObserverBase::set_config_logger_database_location(const std::string& dbLocation)
{
    LoggerClient::instance().set_config_database_location(dbLocation);
}

std::string LogObserverBase::config_logger_database_name() const
{
    return LoggerClient::instance().config_database_name().data();
}

void LogObserverBase::set_config_logger_database_name(const std::string& dbName)
{
    LoggerClient::instance().set_config_database_name(dbName);
}

std::string LogObserverBase::active_database_path() const
{
    return LoggerClient::instance().active_database_path().data();
}

std::string LogObserverBase::init_database_path() const
{
    return LoggerClient::instance().initial_database_path().data();
}

bool LogObserverBase::request_instances()
{
    return logObserverRequestInstances();
}

bool LogObserverBase::request_scopes(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestScopes(target);
}

bool LogObserverBase::request_change_scope_prio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count)
{
    return logObserverRequestChangeScopePrio(target, scopes, count);
}

bool LogObserverBase::request_save_config(ITEM_ID target /*= NEService::TARGET_ALL*/)
{
    return logObserverRequestSaveConfig(target);
}

void LogObserverBase::save_logger_config()
{
    LoggerClient::instance().save_configuration();
}

void LogObserverBase::log_instance_names(std::vector<String>& names)
{
    LoggerClient::instance().log_instance_names(names);
}

void LogObserverBase::log_instances(std::vector<ITEM_ID>& ids)
{
    LoggerClient::instance().log_instances(ids);
}

void LogObserverBase::log_thread_names(std::vector<String>& names)
{
    LoggerClient::instance().log_thread_names(names);
}

void LogObserverBase::log_threads(std::vector<ITEM_ID>& ids)
{
    LoggerClient::instance().log_threads(ids);
}

void LogObserverBase::priority_names(std::vector<String>& names)
{
    LoggerClient::instance().priority_names(names);
}

void LogObserverBase::log_instance_infos(std::vector< NEService::ConnectedInstance>& infos)
{
    LoggerClient::instance().log_instance_infos(infos);
}

void LogObserverBase::log_inst_scopes(std::vector<NELogging::ScopeEntry>& scopes, ITEM_ID instId)
{
    LoggerClient::instance().log_inst_scopes(scopes, instId);
}

void LogObserverBase::log_messages(std::vector<SharedBuffer>& messages)
{
    LoggerClient::instance().log_messages(messages);
}

void LogObserverBase::log_inst_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId /*= NEService::COOKIE_ANY*/)
{
    LoggerClient::instance().log_inst_messages(messages, instId);
}

void LogObserverBase::log_scope_messages(std::vector<SharedBuffer>& messages, uint32_t scopeId /*= 0*/)
{
    LoggerClient::instance().log_scope_messages(messages, scopeId);
}

void LogObserverBase::log_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
{
    LoggerClient::instance().log_messages(messages, instId, scopeId);
}
