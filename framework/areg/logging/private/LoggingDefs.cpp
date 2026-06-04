/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LoggingDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       areg namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/logging/LoggingDefs.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogOptions.hpp"
#include "areg/logging/private/LogManager.hpp"

#include <string.h>

#if AREG_LOGGING

namespace {
    inline areg::EventHeader _log_empty_header() noexcept
    {
        areg::EventHeader hdr{};
        hdr.checksum   = areg::CHECKSUM_INVALID;
        hdr.target     = static_cast<uint32_t>(areg::COOKIE_LOGGER);
        hdr.eventType  = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
        hdr.result     = areg::MESSAGE_SUCCESS;
        hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
        return hdr;
    }

    inline areg::EventHeader _log_message_header() noexcept
    {
        areg::EventHeader hdr{};
        hdr.checksum   = areg::CHECKSUM_INVALID;
        hdr.target     = static_cast<uint32_t>(areg::COOKIE_LOGGER);
        hdr.messageId  = static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage);
        hdr.eventType  = static_cast<uint16_t>(areg::EventType::EventRemoteConnection);
        hdr.result     = areg::MESSAGE_SUCCESS;
        hdr.sequenceNr = areg::SEQUENCE_NUMBER_NOTIFY;
        return hdr;
    }

    void _store_scope_list(areg::EventEnvelope& msgEnv, const areg::ScopeList& scopeList)
    {
        msgEnv << static_cast<uint32_t>(scopeList.size());
        const auto& list{ scopeList.data() };
        for (const auto& entry : list)
        {
            const areg::LogScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgEnv << *scope;
        }
    }
} // namespace
#endif // AREG_LOGGING


areg::LogEntry::LogEntry(areg::LogMessageType msgType)
    : logDataType   { LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ areg::LogPriority::PrioNotset }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { areg::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   ( 0u )
    , logScopeId    { areg::LOG_SCOPE_ID_NONE }
    , logSessionId  { 0u }
    , logMessageLen { 0 }
    , logThreadLen  { 0 }
    , logModuleLen  { 0 }
    , logMessage    { '\0' }
    , logThread     { '\0' }
    , logModule     { '\0' }
{
}

#if AREG_LOGGING
areg::LogEntry::LogEntry(areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen)
    : logDataType   { areg::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ msgPrio }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { LogManager::connection_cookie() }
    , logModuleId   { static_cast<ITEM_ID>(Process::instance().id()) }
    , logThreadId   { static_cast<ITEM_ID>(Thread::current_thread_id()) }
    , logTimestamp  { DateTime::now() }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   { scopeStamp != 0u ? static_cast<uint32_t>(logTimestamp - scopeStamp) : 0u }
    , logScopeId    { scopeId }
    , logSessionId  { sessionId }
    , logMessageLen { msgLen }
    , logThreadLen  { 0 }
    , logModuleLen  { 0 }
    , logMessage    { '\0' }
    , logThread     { '\0' }
    , logModule     { '\0' }
{
    uint32_t len = message != nullptr ? areg::mem_copy(logMessage, areg::LOG_MSG_SIZE - 1, message, msgLen) : 0u;
    logMessage[len] = String::EmptyChar;
}
#else   // AREG_LOGGING
areg::LogEntry::LogEntry(areg::LogMessageType msgType, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : logDataType   { areg::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ areg::LogPriority::PrioNotset }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { areg::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   ( 0u )
    , logScopeId    { areg::LOG_SCOPE_ID_NONE }
    , logSessionId  { 0u }
    , logMessageLen { 0 }
    , logThreadLen  { 0 }
    , logModuleLen  { 0 }
    , logMessage    { '\0' }
    , logThread     { '\0' }
    , logModule     { '\0' }
{
}
#endif  // AREG_LOGGING

areg::LogEntry::LogEntry(const areg::LogEntry & src)
    : logDataType   { }
    , logMsgType    { }
    , logMessagePrio{ }
    , logSource     { }
    , logTarget     { }
    , logCookie     { }
    , logModuleId   { }
    , logThreadId   { }
    , logTimestamp  { }
    , logReceived   { }
    , logDuration   { }
    , logScopeId    { }
    , logSessionId  { }
    , logMessageLen { }
    , logThreadLen  { }
    , logModuleLen  { }
    , logMessage    { }
    , logThread     { }
    , logModule     { }
{
    areg::mem_copy(this, sizeof(areg::LogEntry), &src, sizeof(areg::LogEntry));
}

areg::LogEntry & areg::LogEntry::operator = (const areg::LogEntry & src)
{
    if (this != &src)
    {
        areg::mem_copy(this, sizeof(areg::LogEntry), &src, sizeof(areg::LogEntry));
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGGING
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool areg::start_logging(const char * fileConfig /*= nullptr */ )
{
    return (Application::start_timer_manager() && LogManager::start_logging(fileConfig));
}

AREG_API_IMPL void areg::stop_logging(bool waitComplete)
{
    LogManager::stop_logging(waitComplete);
}

AREG_API_IMPL void areg::wait_logging_end()
{
    LogManager::wait_logging_end();
}

AREG_API_IMPL void areg::activate_scope(LogScope & logScope)
{
    LogManager::activate_log_scope(logScope);
}

AREG_API_IMPL bool areg::is_started()
{
    return LogManager::is_logging_started();
}

AREG_API_IMPL bool areg::is_configured()
{
    return LogManager::is_logging_configured();
}

AREG_API_IMPL bool areg::initialize_logging(const char * fileConfig)
{
    return LogManager::read_log_config(fileConfig);
}

AREG_API_IMPL bool areg::is_enabled()
{
    return LogManager::is_logging_enabled();
}

AREG_API_IMPL bool areg::save_logging( const char * configFile )
{
    return LogManager::save_log_config(configFile);
}

AREG_API_IMPL uint32_t areg::set_scope_priority( const char * scopeName, uint32_t newPrio )
{
    return LogManager::set_scope_priority( scopeName, newPrio );
}

AREG_API_IMPL uint32_t areg::scope_priority( const char * scopeName )
{
    return LogManager::scope_priority( scopeName );
}

AREG_API_IMPL areg::EventEnvelope areg::make_log_message( areg::LogMessageType msgType
                                                        , uint32_t scopeId
                                                        , uint32_t sessionId
                                                        , TIME64 scopeStamp
                                                        , areg::LogPriority msgPrio
                                                        , const char* message
                                                        , uint32_t msgLen)
{
    EventEnvelope msg;
    areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(msg.init_envelope(_log_message_header(), sizeof(areg::LogEntry)));
    if (log != nullptr)
    {
        const TIME64 now    = DateTime::now();

        log->logDataType    = areg::LogDataType::Local;
        log->logMsgType     = msgType;
        log->logMessagePrio = msgPrio;
        log->logSource      = areg::COOKIE_LOCAL;
        log->logTarget      = areg::COOKIE_LOGGER;
        log->logCookie      = areg::COOKIE_LOCAL;
        log->logThreadId    = static_cast<ITEM_ID>(Thread::current_thread_id());
        log->logTimestamp   = now;
        log->logReceived    = DateTime::INVALID_TIME;
        log->logDuration    = scopeStamp != 0u ? static_cast<uint32_t>(now - scopeStamp) : 0u;
        log->logScopeId     = scopeId;
        log->logSessionId   = sessionId;
        log->logThreadLen   = 0u;
        log->logModuleLen   = 0u;
        log->logModuleId    = 0u;

        uint32_t len = areg::mem_copy(log->logMessage, areg::LOG_MSG_SIZE - 1u, message, msgLen);
        log->logMessage[len] = String::EmptyChar;
        log->logMessageLen   = len;

        msg.set_size_used(sizeof(areg::LogEntry));
        msg.move_to_end();
    }

    return msg;
}

AREG_API_IMPL void areg::finalize_log_message(areg::EventEnvelope& msg, areg::LogDataType dataType, const ITEM_ID& srcCookie)
{
    if (!msg.is_valid())
        return;

    areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(msg.buffer());
    log->logDataType = dataType;

    if (areg::LogDataType::Local == dataType)
    {
        msg.set_source(static_cast<uint32_t>(areg::COOKIE_LOCAL));
        log->logCookie    = areg::COOKIE_LOCAL;
        log->logModuleId  = 0u;
        log->logModuleLen = 0u;
        log->logThreadLen = 0u;
        log->logModule[0] = String::EmptyChar;
        log->logThread[0] = String::EmptyChar;
    }
    else
    {
        const String& moduleName{ Process::instance().app_name() };
        const String threadName{ Thread::thread_name(static_cast<id_type>(log->logThreadId)) };

        msg.set_source(static_cast<uint32_t>(srcCookie));
        log->logCookie    = srcCookie;
        log->logModuleId  = static_cast<ITEM_ID>(Process::instance().id());
        log->logModuleLen = areg::mem_copy(log->logModule, areg::LOG_NAME_SIZE - 1u, moduleName.as_string(), static_cast<uint32_t>(moduleName.length()));
        log->logThreadLen = areg::mem_copy(log->logThread, areg::LOG_NAME_SIZE - 1u, threadName.as_string(), static_cast<uint32_t>(threadName.length()));

        log->logModule[log->logModuleLen] = String::EmptyChar;
        log->logThread[log->logThreadLen] = String::EmptyChar;
    }
}

AREG_API_IMPL areg::EventEnvelope areg::create_log_message(const areg::LogEntry& logMessage, areg::LogDataType dataType, const ITEM_ID& srcCookie)
{
    EventEnvelope msgLog;
    areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(msgLog.init_envelope(_log_message_header(), sizeof(areg::LogEntry)));
    if (log != nullptr)
    {
        constexpr uint32_t NAME_LENGTH {areg::LOG_NAME_SIZE - 1};

        areg::mem_copy(log, sizeof(areg::LogEntry), &logMessage, sizeof(areg::LogEntry));
        msgLog.set_size_used(sizeof(areg::LogEntry));
        msgLog.move_to_end();
        msgLog.set_source(static_cast<uint32_t>(srcCookie));
        log->logCookie   = srcCookie;
        log->logDataType = dataType;

        const String& module = Process::instance().app_name();
        uint32_t lenModule  = areg::mem_copy(log->logModule, NAME_LENGTH, module.as_string(), static_cast<uint32_t>(module.length()));
        ASSERT(lenModule < areg::LOG_NAME_SIZE);
        log->logModule[lenModule]   = String::EmptyChar;
        log->logModuleLen           = lenModule;

        if (areg::LogDataType::Local != dataType)
        {
            const String& threadName{ Thread::thread_name(static_cast<id_type>(log->logThreadId)) };
            uint32_t lenThread  = areg::mem_copy(log->logThread, NAME_LENGTH, threadName.as_string(), static_cast<uint32_t>(threadName.length()));
            ASSERT(lenThread < areg::LOG_NAME_SIZE);
            log->logThread[lenThread]   = String::EmptyChar;
            log->logThreadLen           = lenThread;
        }
        else
        {
            log->logThread[0]   = String::EmptyChar;
            log->logThreadLen   = 0u;
        }
    }

    return msgLog;
}

AREG_API_IMPL void areg::log_message(const EventEnvelope& message)
{
    return LogManager::log_message(message);
}

AREG_API_IMPL areg::EventEnvelope areg::message_register_scopes(const ITEM_ID & source, const ITEM_ID & target, const areg::ScopeList & scopeList)
{
    EventEnvelope msgScope;
    if (msgScope.init_envelope(_log_empty_header()) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes));
        msgScope.set_target(static_cast<uint32_t>(target != areg::COOKIE_UNKNOWN ? target : areg::COOKIE_LOGGER));
        msgScope.set_source(static_cast<uint32_t>(source != areg::COOKIE_UNKNOWN ? source : areg::cookie()));

        _store_scope_list(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void areg::log_local(const areg::LogEntry& logMessage)
{
    LogManager::log_message(logMessage);
}

AREG_API_IMPL areg::EventEnvelope areg::message_update_scopes(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeNames& scopeNames)
{
    EventEnvelope msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.init_envelope(_log_empty_header()) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(static_cast<uint32_t>(target));
        msgScope.set_source(static_cast<uint32_t>(source));
        msgScope << scopeNames.size();
        const std::vector<areg::ScopeEntry>& list = scopeNames.data();
        for (const auto & entry : list)
        {
            msgScope << entry;
        }
    }

    return msgScope;
}

AREG_API_IMPL void areg::log_any_message(const areg::LogEntry& logMessage)
{
    LogManager::log_message(logMessage);
}

AREG_API_IMPL areg::EventEnvelope areg::message_update_scope(const ITEM_ID& source, const ITEM_ID& target, const String& scopeName, uint32_t scopeId, uint32_t scopePrio)
{
    EventEnvelope msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.init_envelope(_log_empty_header()) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(static_cast<uint32_t>(target));
        msgScope.set_source(static_cast<uint32_t>(source));
        msgScope << 1u;
        msgScope << scopeName << scopeId << scopePrio;
    }

    return msgScope;
}

AREG_API_IMPL areg::EventEnvelope areg::message_query_instances(const ITEM_ID& source, const ITEM_ID& target)
{
    EventEnvelope msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.init_envelope(_log_empty_header()) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances));
        msgQuery.set_target(static_cast<uint32_t>(target));
        msgQuery.set_source(static_cast<uint32_t>(source));
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::EventEnvelope areg::message_query_scopes(const ITEM_ID& source, const ITEM_ID& target)
{
    EventEnvelope msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.init_envelope(_log_empty_header()) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogQueryScopes));
        msgQuery.set_target(static_cast<uint32_t>(target));
        msgQuery.set_source(static_cast<uint32_t>(source));
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::EventEnvelope areg::message_scopes_updated(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeList& scopeList)
{
    EventEnvelope msgScope;
    if (msgScope.init_envelope(_log_empty_header()) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
        msgScope.set_target(static_cast<uint32_t>(target));
        msgScope.set_source(static_cast<uint32_t>(source));

        _store_scope_list(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL areg::EventEnvelope areg::message_save_configuration(const ITEM_ID& source, const ITEM_ID& target)
{
    EventEnvelope msgRequest;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgRequest.init_envelope(_log_empty_header()) != nullptr))
    {
        msgRequest.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceSaveLogConfiguration));
        msgRequest.set_target(static_cast<uint32_t>(target));
        msgRequest.set_source(static_cast<uint32_t>(source));
        msgRequest << target;
    }

    return msgRequest;
}

AREG_API_IMPL areg::EventEnvelope areg::message_configuration_saved()
{
    EventEnvelope msgScope;
    if (msgScope.init_envelope(_log_empty_header()) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
        msgScope.set_target(static_cast<uint32_t>(areg::COOKIE_LOGGER));
        msgScope.set_source(static_cast<uint32_t>(areg::cookie()));
    }

    return msgScope;
}

AREG_API_IMPL void areg::set_db_engine(LogDatabaseEngine * dbEngine)
{
    LogManager::set_db_engine(dbEngine);
}

AREG_API_IMPL bool areg::force_start_logging()
{
    LogManager::set_default_configuration(false);
    LogManager::force_enable_logging();
    return (Application::start_timer_manager() && LogManager::force_activate_logging());
}

AREG_API_IMPL bool areg::init_logging(const char * fileConfig /*= nullptr */, bool force /*= false */)
{
    Application::start_timer_manager();

    if (LogManager::read_log_config(fileConfig) && !force)
    {
        return LogManager::start_logging(nullptr);
    }
    else if (force)
    {
        return LogManager::force_activate_logging();
    }

    return false;
}

AREG_API_IMPL const ITEM_ID & areg::cookie() noexcept
{
    return LogManager::connection_cookie();
}

AREG_API_IMPL areg::String areg::make_prio_string(uint32_t priorities)
{
    return Identifier::to_string(priorities, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

AREG_API_IMPL uint32_t areg::make_priorities(const String& prioString)
{
    uint16_t id = static_cast<uint16_t>(Identifier::from_string(prioString, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioInvalid)));
    return static_cast<uint32_t>(static_cast<areg::LogPriority>(id));
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGGING
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool areg::start_logging(const char * /*fileConfig*/ /*= nullptr */ )
{
    return true;
}

AREG_API_IMPL void areg::stop_logging(bool /*waitComplete*/)
{
}

AREG_API_IMPL void areg::wait_logging_end()
{
}

AREG_API_IMPL void areg::activate_scope(LogScope & /*logScope*/)
{
}

AREG_API_IMPL bool areg::is_started()
{
    return true;
}

AREG_API_IMPL bool areg::is_configured()
{
    return true;
}

AREG_API_IMPL bool areg::initialize_logging(const char * /*fileConfig*/)
{
    return true;
}

AREG_API_IMPL bool areg::is_enabled()
{
    return true;
}

AREG_API_IMPL bool areg::save_logging( const char * /*configFile*/ )
{
    return true;
}

AREG_API_IMPL uint32_t areg::set_scope_priority( const char * /*scopeName*/, uint32_t /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL uint32_t areg::scope_priority( const char * /*scopeName*/ )
{
    return static_cast<uint32_t>(areg::LogPriority::PrioInvalid);
}

AREG_API_IMPL areg::EventEnvelope areg::make_log_message(areg::LogMessageType, uint32_t, uint32_t, TIME64, areg::LogPriority, const char*, uint32_t)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL void areg::finalize_log_message(areg::EventEnvelope&, areg::LogDataType, const ITEM_ID&)
{
}

AREG_API_IMPL areg::EventEnvelope areg::create_log_message(const areg::LogEntry & /*logMessage*/, areg::LogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL void areg::log_message(const areg::EventEnvelope& /*message*/)
{
}

AREG_API_IMPL areg::EventEnvelope areg::message_register_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL void areg::log_local(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::EventEnvelope areg::message_update_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeNames & /*scopeNames*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL void areg::log_any_message(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::EventEnvelope areg::message_update_scope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const String & /*scopeName*/, uint32_t /*scopeId*/, uint32_t /*scopePrio*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL areg::EventEnvelope areg::message_query_instances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL areg::EventEnvelope areg::message_query_scopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL areg::EventEnvelope areg::message_scopes_updated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL areg::EventEnvelope areg::message_save_configuration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL areg::EventEnvelope areg::message_configuration_saved()
{
    return areg::EventEnvelope{};
}

AREG_API_IMPL void areg::set_db_engine(areg::LogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool areg::force_start_logging()
{
    return true;
}

AREG_API_IMPL bool areg::init_logging(const char * /*fileConfig = nullptr */, bool /*force = false */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID& areg::cookie() noexcept
{
    return areg::COOKIE_UNKNOWN;
}

AREG_API_IMPL areg::String areg::make_prio_string(uint32_t /*priorities*/)
{
    return areg::PRIO_NOTSET_STR;
}

AREG_API_IMPL uint32_t areg::make_priorities(const areg::String& /*prioString*/)
{
    return static_cast<uint32_t>(areg::LogPriority::PrioNotset);
}

//////////////////////////////////////////////////////////////////////////
#endif  // AREG_LOGGING
//////////////////////////////////////////////////////////////////////////
