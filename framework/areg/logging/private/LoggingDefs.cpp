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
        msgRemote << static_cast<uint32_t>(scopeList.getSize());
        const auto& list{ scopeList.getData() };
        for (const auto& entry : list)
        {
            const areg::LogScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgRemote << *scope;
        }
    }
}
#endif // AREG_LOGS

areg::LogEntry::LogEntry(areg::LogMessageType msgType)
    : logDataType   { areg::LogDataType::Local }
    , logMsgType    { msgType }
    , logMessagePrio{ areg::LogPriority::PrioNotset }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { areg::COOKIE_LOCAL }
    , logModuleId   { areg::Process::CURRENT_PROCESS }
    , logThreadId   { areg::Thread::INVALID_THREAD_ID }
    , logTimestamp  { areg::DateTime::INVALID_TIME }
    , logReceived   { areg::DateTime::INVALID_TIME }
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
    , logCookie     { LogManager::getConnectionCookie() }
    , logModuleId   { areg::Process::getInstance().getId() }
    , logThreadId   { areg::Thread::getCurrentThreadId() }
    , logTimestamp  { areg::DateTime::getNow() }
    , logReceived   { areg::DateTime::INVALID_TIME }
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
    uint32_t len = message != nullptr ? areg::memCopy(logMessage, areg::LOG_MESSAGE_IZE - 1, message, msgLen) : 0u;
    logMessage[len] = areg::String::EmptyChar;
}
#else   // AREG_LOGS
areg::LogEntry::LogEntry(areg::LogMessageType msgType, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : logDataType{ areg::LogDataType::Local }
    , logMsgType{ msgType }
    , logMessagePrio{ areg::LogPriority::PrioNotset }
    , logSource     { areg::COOKIE_LOCAL }
    , logTarget     { areg::COOKIE_LOGGER }
    , logCookie     { areg::COOKIE_LOCAL }
    , logModuleId   { areg::Process::CURRENT_PROCESS }
    , logThreadId   { areg::Thread::INVALID_THREAD_ID }
    , logTimestamp  { areg::DateTime::INVALID_TIME }
    , logReceived   { areg::DateTime::INVALID_TIME }
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
    areg::memCopy(logMessage, areg::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
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
            logThread[0] = areg::String::EmptyChar;
            logThreadLen = 0;
            logModule[0] = areg::String::EmptyChar;
        }

        areg::memCopy(logMessage, areg::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool areg::startLogging(const char * fileConfig /*= nullptr */ )
{
    return LogManager::startLogging(fileConfig);
}

AREG_API_IMPL void areg::stopLogging(bool waitComplete)
{
    LogManager::stopLogging(waitComplete);
}

AREG_API_IMPL void areg::waitLoggingEnd()
{
    LogManager::waitLoggingEnd();
}

AREG_API_IMPL void areg::activateScope(areg::LogScope & logScope)
{
    LogManager::activateLogScope(logScope);
}

AREG_API_IMPL bool areg::isStarted()
{
    return LogManager::isLoggingStarted();
}

AREG_API_IMPL bool areg::isConfigured()
{
    return LogManager::isLoggingConfigured();
}

AREG_API_IMPL bool areg::initializeLogging(const char * fileConfig)
{
    return LogManager::readLogConfig(fileConfig);
}

AREG_API_IMPL bool areg::isEnabled()
{
    return LogManager::isLoggingEnabled();
}

AREG_API_IMPL bool areg::saveLogging( const char * configFile )
{
    return LogManager::saveLogConfig(configFile);
}

AREG_API_IMPL uint32_t areg::makeScopeId( const char * scopeName )
{
    return  areg::crc32Calculate(scopeName);
}

AREG_API_IMPL uint32_t areg::makeScopeIdEx(const char* scopeName)
{
    return  (areg::stringEndsWith<char>(scopeName, areg::SYNTAX_SCOPE_GROUP, true) ? areg::CHECKSUM_IGNORE : areg::makeScopeId(scopeName));
}

AREG_API_IMPL uint32_t areg::setScopePriority( const char * scopeName, uint32_t newPrio )
{
    return LogManager::setScopePriority( scopeName, newPrio );
}

AREG_API_IMPL uint32_t areg::getScopePriority( const char * scopeName )
{
    return LogManager::getScopePriority( scopeName );
}

AREG_API_IMPL areg::RemoteMessage areg::createLogMessage(const areg::LogEntry& logMessage, areg::LogDataType dataType, const ITEM_ID& srcCookie)
{
    areg::RemoteMessage msgLog;
    if (msgLog.initMessage(_getLogMessage().rbHeader, sizeof(areg::LogEntry)) != nullptr)
    {
        msgLog << logMessage;
        msgLog.setSizeUsed(sizeof(areg::LogEntry));
        msgLog.moveToEnd();
        msgLog.setSource(srcCookie);
        areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(msgLog.getBuffer());
        log->logCookie   = srcCookie;
        log->logDataType = dataType;

        const areg::String& module = areg::Process::getInstance().getAppName();
        areg::memCopy(log->logModule, areg::LOG_NAMES_SIZE, module.getString(), static_cast<uint32_t>(module.getLength()) + 1);
        log->logModuleLen   = static_cast<uint32_t>(module.getLength());

        if (areg::LogDataType::Local != dataType)
        {
            const areg::String& threadName{ areg::Thread::getThreadName(static_cast<id_type>(log->logThreadId)) };
            areg::memCopy(log->logThread, areg::LOG_NAMES_SIZE, threadName.getString(), static_cast<uint32_t>(threadName.getLength()) + 1);
            log->logThreadLen   = static_cast<uint32_t>(threadName.getLength());
        }
        else
        {
            log->logThread[0]   = areg::String::EmptyChar;
            log->logThreadLen   = 0u;
        }
    }

    return msgLog;
}

AREG_API_IMPL void areg::logMessage(const areg::RemoteMessage& message)
{
    return LogManager::logMessage(message);
}

AREG_API_IMPL areg::RemoteMessage areg::messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const areg::ScopeList & scopeList)
{
    areg::RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != areg::COOKIE_UNKNOWN ? target : areg::COOKIE_LOGGER);
        msgScope.setSource(source != areg::COOKIE_UNKNOWN ? source : areg::getCookie());

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void areg::logAnyMessageLocal(const areg::LogEntry& logMessage)
{
    LogManager::logMessage(logMessage);
}

AREG_API_IMPL areg::RemoteMessage areg::messageUpdateScopes(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeNames& scopeNames)
{
    areg::RemoteMessage msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.setTarget(target);
        msgScope.setSource(source);
        msgScope << scopeNames.getSize();
        const std::vector<areg::ScopeEntry>& list = scopeNames.getData();
        for (const auto & entry : list)
        {
            msgScope << entry;
        }
    }

    return msgScope;
}

AREG_API_IMPL void areg::logAnyMessage(const areg::LogEntry& logMessage)
{
    LogManager::logMessage(areg::SharedBuffer(reinterpret_cast<const uint8_t *>(&logMessage), sizeof(areg::LogEntry)));
}

AREG_API_IMPL areg::RemoteMessage areg::messageUpdateScope(const ITEM_ID& source, const ITEM_ID& target, const areg::String& scopeName, uint32_t scopeId, uint32_t scopePrio)
{
    areg::RemoteMessage msgScope;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
        msgScope.setTarget(target);
        msgScope.setSource(source);
        msgScope << 1u;
        msgScope << scopeName << scopeId << scopePrio;
    }

    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::messageQueryInstances(const ITEM_ID& source, const ITEM_ID& target)
{
    areg::RemoteMessage msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances));
        msgQuery.setTarget(target);
        msgQuery.setSource(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target)
{
    areg::RemoteMessage msgQuery;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgQuery.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogQueryScopes));
        msgQuery.setTarget(target);
        msgQuery.setSource(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::messageScopesUpdated(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeList& scopeList)
{
    areg::RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
        msgScope.setTarget(target);
        msgScope.setSource(source);

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::messageSaveConfiguration(const ITEM_ID& source, const ITEM_ID& target)
{
    areg::RemoteMessage msgRequest;
    if ((source != areg::COOKIE_UNKNOWN) &&
        (target != areg::COOKIE_UNKNOWN) &&
        (msgRequest.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgRequest.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceSaveLogConfiguration));
        msgRequest.setTarget(target);
        msgRequest.setSource(source);
        msgRequest << target;
    }

    return msgRequest;
}

AREG_API_IMPL areg::RemoteMessage areg::messageConfigurationSaved()
{
    areg::RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
        msgScope.setTarget(areg::COOKIE_LOGGER);
        msgScope.setSource(areg::getCookie());
    }

    return msgScope;
}

AREG_API_IMPL void areg::setLogDatabaseEngine(LogDatabaseEngine * dbEngine)
{
    LogManager::setLogDatabaseEngine(dbEngine);
}

AREG_API_IMPL bool areg::forceStartLogging()
{
    LogManager::setDefaultConfiguration(false);
    LogManager::forceEnableLogging();
    return LogManager::forceActivateLogging();
}

AREG_API_IMPL bool areg::initAndStartLogging(const char * fileConfig /*= nullptr */)
{
    if (LogManager::readLogConfig(fileConfig))
    {
        LogManager::forceEnableLogging();
        return LogManager::startLogging(nullptr);
    }
    else
    {
        return LogManager::forceActivateLogging();
    }
}

AREG_API_IMPL const ITEM_ID & areg::getCookie()
{
    return LogManager::getConnectionCookie();
}

AREG_API_IMPL areg::String areg::makePrioString(uint32_t priorities)
{
    return areg::Identifier::convToString(priorities, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

AREG_API_IMPL uint32_t areg::makePriorities(const areg::String& prioString)
{
    uint16_t id = static_cast<uint16_t>(areg::Identifier::convFromString(prioString, areg::LogScopePriorityIndentifiers, static_cast<uint32_t>(areg::LogPriority::PrioInvalid)));
    return static_cast<uint32_t>(static_cast<areg::LogPriority>(id));
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool areg::startLogging(const char * /*fileConfig*/ /*= nullptr */ )
{
    return true;
}

AREG_API_IMPL void areg::stopLogging(bool /*waitComplete*/)
{
}

AREG_API_IMPL void areg::waitLoggingEnd()
{
}

AREG_API_IMPL void areg::activateScope(areg::LogScope & /*logScope*/)
{
}

AREG_API_IMPL bool areg::isStarted()
{
    return true;
}

AREG_API_IMPL bool areg::isConfigured()
{
    return true;
}

AREG_API_IMPL bool areg::initializeLogging(const char * /*fileConfig*/)
{
    return true;
}

AREG_API_IMPL bool areg::isEnabled()
{
    return true;
}

AREG_API_IMPL bool areg::saveLogging( const char * /*configFile*/ )
{
    return true;
}

AREG_API_IMPL uint32_t areg::makeScopeId( const char * /*scopeName*/ )
{
    return 0;
}

AREG_API_IMPL uint32_t areg::makeScopeIdEx(const char* /*scopeName*/)
{
    return 0;
}

AREG_API_IMPL uint32_t areg::setScopePriority( const char * /*scopeName*/, uint32_t /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL uint32_t areg::getScopePriority( const char * /*scopeName*/ )
{
    return static_cast<uint32_t>(areg::LogPriority::PrioInvalid);
}

AREG_API_IMPL areg::RemoteMessage areg::createLogMessage(const areg::LogEntry & /*logMessage*/, areg::LogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    areg::RemoteMessage msgLog;
    return msgLog;
}

AREG_API_IMPL void areg::logMessage(const areg::RemoteMessage& /*message*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::messageRegisterScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::logAnyMessageLocal(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::messageUpdateScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeNames & /*scopeNames*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::logAnyMessage(const areg::LogEntry & /*logMessage*/)
{
}

AREG_API_IMPL areg::RemoteMessage areg::messageUpdateScope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::String & /*scopeName*/, uint32_t /*scopeId*/, uint32_t /*scopePrio*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::messageQueryInstances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::messageQueryScopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL areg::RemoteMessage areg::messageScopesUpdated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const areg::ScopeList & /*scopeList*/)
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL areg::RemoteMessage areg::messageSaveConfiguration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    areg::RemoteMessage msgRequest;
    return msgRequest;
}

AREG_API_IMPL areg::RemoteMessage areg::messageConfigurationSaved()
{
    areg::RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void areg::setLogDatabaseEngine(LogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool areg::forceStartLogging()
{
    return true;
}

AREG_API_IMPL bool areg::initAndStartLogging(const char * /*fileConfig*/ /*= nullptr */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID & areg::getCookie()
{
    return areg::COOKIE_UNKNOWN;
}

AREG_API_IMPL areg::String areg::makePrioString(uint32_t /*priorities*/)
{
    return areg::PRIO_NOTSET_STR;
}

AREG_API_IMPL uint32_t areg::makePriorities(const areg::String& /*prioString*/)
{
    return static_cast<uint32_t>(areg::LogPriority::PrioNotset);
}

//////////////////////////////////////////////////////////////////////////
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
