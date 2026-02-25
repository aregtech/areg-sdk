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
 * \brief       NELogging namespace contains , structures and types.
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
namespace
{
    /**
     * \brief   Returns predefined structure for the logging communication message.
     *          The structure is used as a template to initialize the remote communication message.
     *          The target, source and the message ID should be set before sending the message.
     *          Otherwise, the message ID is an empty function and message will be ignored by any component.
     **/
    const NEMemory::RawMessage& _getLogEmptyMessage()
    {
        static constexpr NEMemory::RawMessage _messageUpdateScpes
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(NEMemory::RawMessage)          // biBufSize
                    , sizeof(uint8_t)                     // biLength
                    , sizeof(NEMemory::MessageHeader)    // biOffset
                    , NEMemory::BufferType::Remote       // biBufType
                    , 0                                         // biUsed
                }
                , NEService::COOKIE_LOGGER                      // rbhTarget
                , NEMemory::INVALID_VALUE                       // rbhChecksum
                , NEMemory::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(NEService::FuncIdRange::EmptyFunctionId)   // rbhMessageId
                , NEMemory::MESSAGE_SUCCESS                     // rbhResult
                , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
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
    const NEMemory::RawMessage & _getLogMessage()
    {
        static constexpr NEMemory::RawMessage _messageServiceLog
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(NEMemory::RawMessage)          // biBufSize
                    , sizeof(uint8_t)                     // biLength
                    , sizeof(NEMemory::MessageHeader)    // biOffset
                    , NEMemory::BufferType::Remote       // biBufType
                    , 0                                         // biUsed
                }
                , NEService::COOKIE_LOGGER                      // rbhTarget
                , NEMemory::INVALID_VALUE                       // rbhChecksum
                , NEMemory::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogMessage)   // rbhMessageId
                , NEMemory::MESSAGE_SUCCESS                     // rbhResult
                , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
            }
            , { static_cast<char>(0) }
        };

        return _messageServiceLog;
    }

    void _storeScopeList(RemoteMessage& msgRemote, const NELogging::ScopeList& scopeList)
    {
        msgRemote << static_cast<uint32_t>(scopeList.size());
        const auto& list{ scopeList.data() };
        for (const auto& entry : list)
        {
            const LogScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgRemote << *scope;
        }
    }
}
#endif // AREG_LOGS

NELogging::LogEntry::LogEntry(NELogging::LogMessageType msgType)
    : logDataType   { NELogging::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ NELogging::LogPriority::PrioNotset }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { NEService::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   ( 0u )
    , logScopeId    { NELogging::LOG_SCOPE_ID_NONE }
    , logSessionId  { 0u }
    , logMessageLen { 0 }
    , log_message    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}

#if AREG_LOGS
NELogging::LogEntry::LogEntry(NELogging::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, NELogging::LogPriority msgPrio, const char * message, uint32_t msgLen)
    : logDataType   { NELogging::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ msgPrio }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { LogManager::connection_cookie() }
    , logModuleId   { Process::instance().id() }
    , logThreadId   { Thread::current_thread_id() }
    , logTimestamp  { DateTime::now() }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   { scopeStamp != 0u ? static_cast<uint32_t>(logTimestamp - scopeStamp) : 0u }
    , logScopeId    { scopeId }
    , logSessionId  { sessionId }
    , logMessageLen { msgLen }
    , log_message    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    uint32_t len = message != nullptr ? NEMemory::mem_copy(log_message, NELogging::LOG_MESSAGE_IZE - 1, message, msgLen) : 0u;
    log_message[len] = String::EmptyChar;
}
#else   // AREG_LOGS
NELogging::LogEntry::LogEntry(NELogging::LogMessageType msgType, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, NELogging::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : logDataType{ NELogging::LogDataType::Local }
    , logMsgType{ msgType }
    , logMessagePrio{ NELogging::LogPriority::PrioNotset }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { NEService::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   ( 0u )
    , logScopeId    { NELogging::LOG_SCOPE_ID_NONE }
    , logSessionId  { 0u }
    , logMessageLen { 0 }
    , log_message    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}
#endif  // AREG_LOGS

NELogging::LogEntry::LogEntry(const NELogging::LogEntry & src)
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
    , log_message    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    NEMemory::mem_copy(log_message, NELogging::LOG_MESSAGE_IZE, src.log_message, src.logMessageLen + 1);
}

NELogging::LogEntry & NELogging::LogEntry::operator = (const NELogging::LogEntry & src)
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

        if (logDataType == NELogging::LogDataType::Remote)
        {
            logThreadLen = 0;
            logThread[0] = String::EmptyChar;
            logThreadLen = 0;
            logModule[0] = String::EmptyChar;
        }

        NEMemory::mem_copy(log_message, NELogging::LOG_MESSAGE_IZE, src.log_message, src.logMessageLen + 1);
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NELogging::start_logging(const char * fileConfig /*= nullptr */ )
{
    return LogManager::start_logging(fileConfig);
}

AREG_API_IMPL void NELogging::stop_logging(bool waitComplete)
{
    LogManager::stop_logging(waitComplete);
}

AREG_API_IMPL void NELogging::wait_logging_end()
{
    LogManager::wait_logging_end();
}

AREG_API_IMPL void NELogging::activate_scope(LogScope & logScope)
{
    LogManager::activate_log_scope(logScope);
}

AREG_API_IMPL bool NELogging::is_started()
{
    return LogManager::is_logging_started();
}

AREG_API_IMPL bool NELogging::is_configured()
{
    return LogManager::is_logging_configured();
}

AREG_API_IMPL bool NELogging::initialize_logging(const char * fileConfig)
{
    return LogManager::read_log_config(fileConfig);
}

AREG_API_IMPL bool NELogging::is_enabled()
{
    return LogManager::is_logging_enabled();
}

AREG_API_IMPL bool NELogging::save_logging( const char * configFile )
{
    return LogManager::save_log_config(configFile);
}

AREG_API_IMPL uint32_t NELogging::make_scope_id( const char * scopeName )
{
    return  NEMath::crc32_calculate(scopeName);
}

AREG_API_IMPL uint32_t NELogging::make_scope_id_ex(const char* scopeName)
{
    return  (NEString::string_ends_with<char>(scopeName, NELogOptions::SYNTAX_SCOPE_GROUP, true) ? NEMath::CHECKSUM_IGNORE : NELogging::make_scope_id(scopeName));
}

AREG_API_IMPL uint32_t NELogging::set_scope_priority( const char * scopeName, uint32_t newPrio )
{
    return LogManager::set_scope_priority( scopeName, newPrio );
}

AREG_API_IMPL uint32_t NELogging::scope_priority( const char * scopeName )
{
    return LogManager::scope_priority( scopeName );
}

AREG_API_IMPL RemoteMessage NELogging::create_log_message(const NELogging::LogEntry& log_message, NELogging::LogDataType dataType, const ITEM_ID& srcCookie)
{
    RemoteMessage msgLog;
    if (msgLog.init_message(_getLogMessage().rbHeader, sizeof(NELogging::LogEntry)) != nullptr)
    {
        msgLog << log_message;
        msgLog.set_size_used(sizeof(NELogging::LogEntry));
        msgLog.move_to_end();
        msgLog.set_source(srcCookie);
        NELogging::LogEntry* log = reinterpret_cast<NELogging::LogEntry*>(msgLog.buffer());
        log->logCookie   = srcCookie;
        log->logDataType = dataType;

        const String& module = Process::instance().app_name();
        NEMemory::mem_copy(log->logModule, NELogging::LOG_NAMES_SIZE, module.as_string(), static_cast<uint32_t>(module.length()) + 1);
        log->logModuleLen   = static_cast<uint32_t>(module.length());

        if (NELogging::LogDataType::Local != dataType)
        {
            const String& threadName{ Thread::thread_name(static_cast<id_type>(log->logThreadId)) };
            NEMemory::mem_copy(log->logThread, NELogging::LOG_NAMES_SIZE, threadName.as_string(), static_cast<uint32_t>(threadName.length()) + 1);
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

AREG_API_IMPL void NELogging::log_message(const RemoteMessage& message)
{
    return LogManager::log_message(message);
}

AREG_API_IMPL RemoteMessage NELogging::message_register_scopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeList & scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogRegisterScopes));
        msgScope.set_target(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.set_source(source != NEService::COOKIE_UNKNOWN ? source : NELogging::cookie());

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::log_local(const NELogging::LogEntry& log_message)
{
    LogManager::log_message(log_message);
}

AREG_API_IMPL RemoteMessage NELogging::message_update_scopes(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeNames& scopeNames)
{
    RemoteMessage msgScope;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(target);
        msgScope.set_source(source);
        msgScope << scopeNames.size();
        const std::vector<NELogging::ScopeEntry>& list = scopeNames.data();
        for (const auto & entry : list)
        {
            msgScope << entry;
        }
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::log_any_message(const NELogging::LogEntry& log_message)
{
    LogManager::log_message(SharedBuffer(reinterpret_cast<const uint8_t *>(&log_message), sizeof(NELogging::LogEntry)));
}

AREG_API_IMPL RemoteMessage NELogging::message_update_scope(const ITEM_ID& source, const ITEM_ID& target, const String& scopeName, uint32_t scopeId, uint32_t scopePrio)
{
    RemoteMessage msgScope;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.set_target(target);
        msgScope.set_source(source);
        msgScope << 1u;
        msgScope << scopeName << scopeId << scopePrio;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::message_query_instances(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgQuery.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceQueryInstances));
        msgQuery.set_target(target);
        msgQuery.set_source(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::message_query_scopes(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgQuery.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogQueryScopes));
        msgQuery.set_target(target);
        msgQuery.set_source(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::message_scopes_updated(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeList& scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogScopesUpdated));
        msgScope.set_target(target);
        msgScope.set_source(source);

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::message_save_configuration(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgRequest;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgRequest.init_message(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgRequest.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceSaveLogConfiguration));
        msgRequest.set_target(target);
        msgRequest.set_source(source);
        msgRequest << target;
    }

    return msgRequest;
}

AREG_API_IMPL RemoteMessage NELogging::message_configuration_saved()
{
    RemoteMessage msgScope;
    if (msgScope.init_message(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.set_message_id(static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogConfigurationSaved));
        msgScope.set_target(NEService::COOKIE_LOGGER);
        msgScope.set_source(NELogging::cookie());
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::set_db_engine(LogDatabaseEngine * dbEngine)
{
    LogManager::set_db_engine(dbEngine);
}

AREG_API_IMPL bool NELogging::force_start_logging()
{
    LogManager::set_default_configuration(false);
    LogManager::force_enable_logging();
    return LogManager::force_activate_logging();
}

AREG_API_IMPL bool NELogging::init_logging(const char * fileConfig /*= nullptr */)
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

AREG_API_IMPL const ITEM_ID & NELogging::cookie()
{
    return LogManager::connection_cookie();
}

AREG_API_IMPL String NELogging::make_prio_string(uint32_t priorities)
{
    return Identifier::to_string(priorities, NEApplication::LogScopePriorityIndentifiers, static_cast<uint32_t>(NELogging::LogPriority::PrioNotset));
}

AREG_API_IMPL uint32_t NELogging::make_priorities(const String& prioString)
{
    uint16_t id = static_cast<uint16_t>(Identifier::conv_from_string(prioString, NEApplication::LogScopePriorityIndentifiers, static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid)));
    return static_cast<uint32_t>(static_cast<NELogging::LogPriority>(id));
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NELogging::start_logging(const char * /*fileConfig*/ /*= nullptr */ )
{
    return true;
}

AREG_API_IMPL void NELogging::stop_logging(bool /*waitComplete*/)
{
}

AREG_API_IMPL void NELogging::wait_logging_end()
{
}

AREG_API_IMPL void NELogging::activate_scope(LogScope & /*logScope*/)
{
}

AREG_API_IMPL bool NELogging::is_started()
{
    return true;
}

AREG_API_IMPL bool NELogging::is_configured()
{
    return true;
}

AREG_API_IMPL bool NELogging::initialize_logging(const char * /*fileConfig*/)
{
    return true;
}

AREG_API_IMPL bool NELogging::is_enabled()
{
    return true;
}

AREG_API_IMPL bool NELogging::save_logging( const char * /*configFile*/ )
{
    return true;
}

AREG_API_IMPL uint32_t NELogging::make_scope_id( const char * /*scopeName*/ )
{
    return 0;
}

AREG_API_IMPL uint32_t NELogging::make_scope_id_ex(const char* /*scopeName*/)
{
    return 0;
}

AREG_API_IMPL uint32_t NELogging::set_scope_priority( const char * /*scopeName*/, uint32_t /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL uint32_t NELogging::scope_priority( const char * /*scopeName*/ )
{
    return static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid);
}

AREG_API_IMPL RemoteMessage NELogging::create_log_message(const NELogging::LogEntry & /*log_message*/, NELogging::LogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    RemoteMessage msgLog;
    return msgLog;
}

AREG_API_IMPL void NELogging::log_message(const RemoteMessage& /*message*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::message_register_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::log_local(const NELogging::LogEntry & /*log_message*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::message_update_scopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeNames & /*scopeNames*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::log_any_message(const NELogging::LogEntry & /*log_message*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::message_update_scope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const String & /*scopeName*/, uint32_t /*scopeId*/, uint32_t /*scopePrio*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::message_query_instances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::message_query_scopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::message_scopes_updated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::message_save_configuration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgRequest;
    return msgRequest;
}

AREG_API_IMPL RemoteMessage NELogging::message_configuration_saved()
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::set_db_engine(LogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool NELogging::force_start_logging()
{
    return true;
}

AREG_API_IMPL bool NELogging::init_logging(const char * /*fileConfig*/ /*= nullptr */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID & NELogging::cookie()
{
    return NEService::COOKIE_UNKNOWN;
}

AREG_API_IMPL String NELogging::make_prio_string(uint32_t /*priorities*/)
{
    return NELogging::PRIO_NOTSET_STR;
}

AREG_API_IMPL uint32_t NELogging::make_priorities(const String& /*prioString*/)
{
    return static_cast<uint32_t>(NELogging::LogPriority::PrioNotset);
}

//////////////////////////////////////////////////////////////////////////
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
