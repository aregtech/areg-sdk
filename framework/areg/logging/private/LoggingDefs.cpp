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

#if AREG_LOGS

namespace {
    /**
     * \brief   Returns predefined structure for the logging communication message.
     *          The structure is used as a template to initialize the remote communication message.
     *          The target, source and the message ID should be set before sending the message.
     *          Otherwise, the message ID is an empty function and message will be ignored by any component.
     **/
    const areg::RawMessage& _getLogEmptyMessage()
    {
        static constexpr areg::RawMessage _messageUpdateScpes
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(areg::RawMessage)          // biBufSize
                    , sizeof(uint8_t)                     // biLength
                    , sizeof(areg::MessageHeader)    // biOffset
                    , areg::BufferType::Remote       // biBufType
                    , 0                                         // biUsed
                }
                , areg::COOKIE_LOGGER                      // rbhTarget
                , areg::INVALID_VALUE                       // rbhChecksum
                , areg::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId)   // rbhMessageId
                , areg::MESSAGE_SUCCESS                     // rbhResult
                , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
            }
            , { static_cast<char>(0) }
        };

        return _messageUpdateScpes;
    }

    /**
     * \brief   Returns predefined structure for to log a message on remote log collector or log observer.
     *          The structure is used as a template to initialize remote communication message.
     *          The source of the log should be set before sending the message.
     *          Otherwise, it is ignored by the Log Collector and the message is dropped.
     **/
    const areg::RawMessage & _getLogMessage()
    {
        static constexpr areg::RawMessage _messageServiceLog
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(areg::RawMessage)          // biBufSize
                    , sizeof(uint8_t)                     // biLength
                    , sizeof(areg::MessageHeader)    // biOffset
                    , areg::BufferType::Remote       // biBufType
                    , 0                                         // biUsed
                }
                , areg::COOKIE_LOGGER                      // rbhTarget
                , areg::INVALID_VALUE                       // rbhChecksum
                , areg::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage)   // rbhMessageId
                , areg::MESSAGE_SUCCESS                     // rbhResult
                , areg::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
            }
            , { static_cast<char>(0) }
        };

        return _messageServiceLog;
    }

    void _storeScopeList(areg::RemoteMessage& msgRemote, const areg::ScopeList& scopeList)
    {
        msgRemote << static_cast<uint32_t>(scopeList.size());
        const auto& list{ scopeList.data() };
        for (const auto& entry : list)
        {
            const areg::LogScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgRemote << *scope;
        }
    }
} // namespace
#endif // AREG_LOGS


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
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}

#if AREG_LOGS
areg::LogEntry::LogEntry(areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen)
    : logDataType   { areg::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ msgPrio }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { LogManager::connection_cookie() }
    , logModuleId   { Process::instance().id() }
    , logThreadId   { Thread::current_thread_id() }
    , logTimestamp  { DateTime::now() }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   { scopeStamp != 0u ? static_cast<uint32_t>(logTimestamp - scopeStamp) : 0u }
    , logScopeId    { scopeId }
    , logSessionId  { sessionId }
    , logMessageLen { msgLen }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    uint32_t len = message != nullptr ? areg::mem_copy(logMessage, areg::LOG_MESSAGE_IZE - 1, message, msgLen) : 0u;
    logMessage[len] = String::EmptyChar;
}
#else   // AREG_LOGS
areg::LogEntry::LogEntry(areg::LogMessageType msgType, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : logDataType{ areg::LogDataType::Local }
    , logMsgType{ msgType }
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
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}
#endif  // AREG_LOGS

areg::LogEntry::LogEntry(const areg::LogEntry & src)
    : logDataType   { src.logDataType }
    , logMsgType    { src.logMsgType }
    , logMessagePrio{ src.logMessagePrio }
    , logSource     { src.logSource }
    , logTarget     { src.logTarget }
    , logCookie     { src.logCookie }
    , logModuleId   { src.logModuleId }
    , logThreadId   { src.logThreadId }
    , logTimestamp  { src.logTimestamp }
    , logReceived   { src.logReceived }
    , logDuration   { src.logDuration }
    , logScopeId    { src.logScopeId }
    , logSessionId  { src.logSessionId }
    , logMessageLen { src.logMessageLen }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    areg::mem_copy(logMessage, areg::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
}

areg::LogEntry & areg::LogEntry::operator = (const areg::LogEntry & src)
{
    if (this != &src)
    {
        logDataType     = src.logDataType;
        logMsgType      = src.logMsgType;
        logMessagePrio  = src.logMessagePrio;
        logSource       = src.logSource;
        logTarget       = src.logTarget;
        logCookie       = src.logCookie;
        logModuleId     = src.logModuleId;
        logThreadId     = src.logThreadId;
        logTimestamp    = src.logTimestamp;
        logReceived     = src.logReceived;
        logDuration     = src.logDuration;
        logScopeId      = src.logScopeId;
        logSessionId    = src.logSessionId;
        logMessageLen   = src.logMessageLen;

        if (logDataType == areg::LogDataType::Remote)
        {
            logThreadLen = 0;
            logThread[0] = String::EmptyChar;
            logThreadLen = 0;
            logModule[0] = String::EmptyChar;
        }

        areg::mem_copy(logMessage, areg::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool areg::start_logging(const char * fileConfig /*= nullptr */ )
{
    return LogManager::start_logging(fileConfig);
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

AREG_API_IMPL uint32_t areg::make_scope_id( const char * scopeName )
{
    return  areg::crc32_calculate(scopeName);
}

AREG_API_IMPL uint32_t areg::make_scope_id_ex(const char* scopeName)
{
    return  (areg::string_ends_with<char>(scopeName, areg::SYNTAX_SCOPE_GROUP, true) ? areg::CHECKSUM_IGNORE : areg::make_scope_id(scopeName));
}

AREG_API_IMPL uint32_t areg::set_scope_priority( const char * scopeName, uint32_t newPrio )
{
    return LogManager::set_scope_priority( scopeName, newPrio );
}

AREG_API_IMPL uint32_t areg::scope_priority( const char * scopeName )
{
    return LogManager::scope_priority( scopeName );
}

AREG_API_IMPL areg::RemoteMessage areg::create_log_message(const areg::LogEntry& logMessage, areg::LogDataType dataType, const ITEM_ID& srcCookie)
{
    RemoteMessage msgLog;
    if (msgLog.init_message(_getLogMessage().rbHeader, sizeof(areg::LogEntry)) != nullptr)
    {
        msgLog << logMessage;
        msgLog.set_size_used(sizeof(areg::LogEntry));
        msgLog.move_to_end();
        msgLog.set_source(srcCookie);
        areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(msgLog.buffer());
        log->logCookie   = srcCookie;
        log->logDataType = dataType;

        const String& module = Process::instance().app_name();
        areg::mem_copy(log->logModule, areg::LOG_NAMES_SIZE, module.as_string(), static_cast<uint32_t>(module.length()) + 1);
        log->logModuleLen   = static_cast<uint32_t>(module.length());

        if (areg::LogDataType::Local != dataType)
        {
            const String& threadName{ Thread::thread_name(static_cast<id_type>(log->logThreadId)) };
            areg::mem_copy(log->logThread, areg::LOG_NAMES_SIZE, threadName.as_string(), static_cast<uint32_t>(threadName.length()) + 1);
            log->logThreadLen   = static_cast<uint32_t>(threadName.length());
        }
        else
        {
            log->logThread[0]   = String::EmptyChar;
            log->logThreadLen   = 0u;
        }
    }

    return msgLog;
}

AREG_API_IMPL void areg::log_message(const RemoteMessage& message)
{
    return LogManager::log_message(message);
}

AREG_API_IMPL areg::RemoteMessage areg::message_register_scopes(const ITEM_ID & source, const ITEM_ID & target, const areg::ScopeList & scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes));
        msgScope.set_target(target != areg::COOKIE_UNKNOWN ? target : areg::COOKIE_LOGGER);
        msgScope.set_source(source != areg::COOKIE_UNKNOWN ? source : areg::cookie());

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void areg::log_local(const areg::LogEntry& logMessage)
{
    LogManager::log_message(logMessage);
}

AREG_API_IMPL areg::RemoteMessage areg::message_update_scopes(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeNames& scopeNames)
{
    RemoteMessage msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(target);
        msgScope.set_source(source);
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
    LogManager::log_message(SharedBuffer(reinterpret_cast<const uint8_t *>(&logMessage), sizeof(areg::LogEntry)));
}

AREG_API_IMPL areg::RemoteMessage areg::message_update_scope(const ITEM_ID& source, const ITEM_ID& target, const String& scopeName, uint32_t scopeId, uint32_t scopePrio)
{
    RemoteMessage msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(target);
        msgScope.set_source(source);
        msgScope << 1u;
        msgScope << scopeName << scopeId << scopePrio;
    }

    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::message_query_instances(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances));
        msgQuery.set_target(target);
        msgQuery.set_source(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::message_query_scopes(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogQueryScopes));
        msgQuery.set_target(target);
        msgQuery.set_source(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::message_scopes_updated(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeList& scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
        msgScope.set_target(target);
        msgScope.set_source(source);

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::message_save_configuration(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgRequest;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgRequest.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgRequest.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceSaveLogConfiguration));
        msgRequest.set_target(target);
        msgRequest.set_source(source);
        msgRequest << target;
    }

    return msgRequest;
}

AREG_API_IMPL areg::RemoteMessage areg::message_configuration_saved()
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
        msgScope.set_target(areg::COOKIE_LOGGER);
        msgScope.set_source(areg::cookie());
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
    return LogManager::force_activate_logging();
}

AREG_API_IMPL bool areg::init_logging(const char * fileConfig /*= nullptr */)
{
    if (LogManager::read_log_config(fileConfig))
    {
        LogManager::force_enable_logging();
        return LogManager::start_logging(nullptr);
    }
    else
    {
        return LogManager::force_activate_logging();
    }
}

AREG_API_IMPL const ITEM_ID & areg::cookie()
{
    return LogManager::connection_cookie();
}

AREG_API_IMPL areg::String areg::make_prio_string(uint32_t priorities)
{
    return Identifier::to_string(priorities, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

AREG_API_IMPL uint32_t areg::make_priorities(const String& prioString)
{
    uint16_t id = static_cast<uint16_t>(Identifier::conv_from_string(prioString, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioInvalid)));
    return static_cast<uint32_t>(static_cast<areg::LogPriority>(id));
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGS
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

AREG_API_IMPL uint32_t areg::make_scope_id( const char * /*scopeName*/ )
{
    return 0;
}

AREG_API_IMPL uint32_t areg::make_scope_id_ex(const char* /*scopeName*/)
{
    return 0;
}

AREG_API_IMPL uint32_t areg::set_scope_priority( const char * /*scopeName*/, uint32_t /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL uint32_t areg::scope_priority( const char * /*scopeName*/ )
{
    return static_cast<uint32_t>(areg::LogPriority::PrioInvalid);
}

AREG_API_IMPL areg::RemoteMessage areg::create_log_message(const areg::LogEntry & /*logMessage*/, areg::LogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    RemoteMessage msgLog;
    return msgLog;
}

AREG_API_IMPL void areg::log_message(const areg::RemoteMessage& /*message*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::message_register_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::log_local(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::message_update_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeNames & /*scopeNames*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::log_any_message(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::message_update_scope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const String & /*scopeName*/, uint32_t /*scopeId*/, uint32_t /*scopePrio*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::message_query_instances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::message_query_scopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::message_scopes_updated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::message_save_configuration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgRequest;
    return msgRequest;
}

AREG_API_IMPL areg::RemoteMessage areg::message_configuration_saved()
{
    areg::  RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::set_db_engine(areg::LogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool areg::force_start_logging()
{
    return true;
}

AREG_API_IMPL bool areg::init_logging(const char * /*fileConfig*/ /*= nullptr */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID & areg::cookie()
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
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
