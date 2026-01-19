/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/NELogging.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       NELogging namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/logging/NELogging.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/NEService.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/NELogOptions.hpp"
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
    const NEMemory::sRemoteMessage& _getLogEmptyMessage(void)
    {
        static constexpr NEMemory::sRemoteMessage _messageUpdateScpes
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(NEMemory::sRemoteMessage)          // biBufSize
                    , sizeof(unsigned char)                     // biLength
                    , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
                    , NEMemory::eBufferType::BufferRemote       // biBufType
                    , 0                                         // biUsed
                }
                , NEService::COOKIE_LOGGER                      // rbhTarget
                , NEMemory::INVALID_VALUE                       // rbhChecksum
                , NEMemory::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(NEService::eFuncIdRange::EmptyFunctionId)   // rbhMessageId
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
    const NEMemory::sRemoteMessage & _getLogMessage(void)
    {
        static constexpr NEMemory::sRemoteMessage _messageServiceLog
        {
            {
                {   /*rbhBufHeader*/
                      sizeof(NEMemory::sRemoteMessage)          // biBufSize
                    , sizeof(unsigned char)                     // biLength
                    , sizeof(NEMemory::sRemoteMessageHeader)    // biOffset
                    , NEMemory::eBufferType::BufferRemote       // biBufType
                    , 0                                         // biUsed
                }
                , NEService::COOKIE_LOGGER                      // rbhTarget
                , NEMemory::INVALID_VALUE                       // rbhChecksum
                , NEMemory::INVALID_VALUE                       // rbhSource
                , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogMessage)   // rbhMessageId
                , NEMemory::MESSAGE_SUCCESS                     // rbhResult
                , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
            }
            , { static_cast<char>(0) }
        };

        return _messageServiceLog;
    }

    void _storeScopeList(RemoteMessage& msgRemote, const NELogging::ScopeList& scopeList)
    {
        msgRemote << static_cast<uint32_t>(scopeList.getSize());
        const auto& list{ scopeList.getData() };
        for (const auto& entry : list)
        {
            const LogScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgRemote << *scope;
        }
    }
}
#endif // AREG_LOGS

NELogging::sLogMessage::sLogMessage(NELogging::eLogMessageType msgType)
    : logDataType   { NELogging::eLogDataType::LogDataLocal }
    , logMsgType    { msgType }
    , logMessagePrio{ NELogging::eLogPriority::PrioNotset }
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
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}

#if AREG_LOGS
NELogging::sLogMessage::sLogMessage(NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const char * message, unsigned int msgLen)
    : logDataType   { NELogging::eLogDataType::LogDataLocal }
    , logMsgType    { msgType }
    , logMessagePrio{ msgPrio }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { LogManager::getConnectionCookie() }
    , logModuleId   { Process::getInstance().getId() }
    , logThreadId   { Thread::getCurrentThreadId() }
    , logTimestamp  { DateTime::getNow() }
    , logReceived   { DateTime::INVALID_TIME }
    , logDuration   { scopeStamp != 0u ? static_cast<unsigned int>(logTimestamp - scopeStamp) : 0u }
    , logScopeId    { scopeId }
    , logSessionId  { sessionId }
    , logMessageLen { msgLen }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    uint32_t len = message != nullptr ? NEMemory::memCopy(logMessage, NELogging::LOG_MESSAGE_IZE - 1, message, msgLen) : 0u;
    logMessage[len] = String::EmptyChar;
}
#else   // AREG_LOGS
NELogging::sLogMessage::sLogMessage(NELogging::eLogMessageType msgType, unsigned int /*scopeId*/, unsigned int /*sessionId*/, TIME64 /*scopeStamp*/, NELogging::eLogPriority /*msgPrio*/, const char* /*message*/, unsigned int /*msgLen*/)
    : logDataType{ NELogging::eLogDataType::LogDataLocal }
    , logMsgType{ msgType }
    , logMessagePrio{ NELogging::eLogPriority::PrioNotset }
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
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}
#endif  // AREG_LOGS

NELogging::sLogMessage::sLogMessage(const NELogging::sLogMessage & src)
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
    NEMemory::memCopy(logMessage, NELogging::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
}

NELogging::sLogMessage & NELogging::sLogMessage::operator = (const NELogging::sLogMessage & src)
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

        if (logDataType == NELogging::eLogDataType::LogDataRemote)
        {
            logThreadLen = 0;
            logThread[0] = String::EmptyChar;
            logThreadLen = 0;
            logModule[0] = String::EmptyChar;
        }

        NEMemory::memCopy(logMessage, NELogging::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NELogging::startLogging(const char * fileConfig /*= nullptr */ )
{
    return LogManager::startLogging(fileConfig);
}

AREG_API_IMPL void NELogging::stopLogging(bool waitComplete)
{
    LogManager::stopLogging(waitComplete);
}

AREG_API_IMPL void NELogging::waitLoggingEnd(void)
{
    LogManager::waitLoggingEnd();
}

AREG_API_IMPL void NELogging::activateScope(LogScope & logScope)
{
    LogManager::activateLogScope(logScope);
}

AREG_API_IMPL bool NELogging::isStarted( void )
{
    return LogManager::isLoggingStarted();
}

AREG_API_IMPL bool NELogging::isConfigured(void)
{
    return LogManager::isLoggingConfigured();
}

AREG_API_IMPL bool NELogging::initializeLogging(const char * fileConfig)
{
    return LogManager::readLogConfig(fileConfig);
}

AREG_API_IMPL bool NELogging::isEnabled(void)
{
    return LogManager::isLoggingEnabled();
}

AREG_API_IMPL bool NELogging::saveLogging( const char * configFile )
{
    return LogManager::saveLogConfig(configFile);
}

AREG_API_IMPL unsigned int NELogging::makeScopeId( const char * scopeName )
{
    return  NEMath::crc32Calculate(scopeName);
}

AREG_API_IMPL unsigned int NELogging::makeScopeIdEx(const char* scopeName)
{
    return  (NEString::stringEndsWith<char>(scopeName, NELogOptions::SYNTAX_SCOPE_GROUP, true) ? NEMath::CHECKSUM_IGNORE : NELogging::makeScopeId(scopeName));
}

AREG_API_IMPL unsigned int NELogging::setScopePriority( const char * scopeName, unsigned int newPrio )
{
    return LogManager::setScopePriority( scopeName, newPrio );
}

AREG_API_IMPL unsigned int NELogging::getScopePriority( const char * scopeName )
{
    return LogManager::getScopePriority( scopeName );
}

AREG_API_IMPL RemoteMessage NELogging::createLogMessage(const NELogging::sLogMessage& logMessage, NELogging::eLogDataType dataType, const ITEM_ID& srcCookie)
{
    RemoteMessage msgLog;
    if (msgLog.initMessage(_getLogMessage().rbHeader, sizeof(NELogging::sLogMessage)) != nullptr)
    {
        msgLog << logMessage;
        msgLog.setSizeUsed(sizeof(NELogging::sLogMessage));
        msgLog.moveToEnd();
        msgLog.setSource(srcCookie);
        NELogging::sLogMessage* log = reinterpret_cast<NELogging::sLogMessage*>(msgLog.getBuffer());
        log->logCookie   = srcCookie;
        log->logDataType = dataType;

        const String& module = Process::getInstance().getAppName();
        NEMemory::memCopy(log->logModule, NELogging::LOG_NAMES_SIZE, module.getString(), static_cast<uint32_t>(module.getLength()) + 1);
        log->logModuleLen   = static_cast<uint32_t>(module.getLength());

        if (NELogging::eLogDataType::LogDataLocal != dataType)
        {
            const String& threadName{ Thread::getThreadName(static_cast<id_type>(log->logThreadId)) };
            NEMemory::memCopy(log->logThread, NELogging::LOG_NAMES_SIZE, threadName.getString(), static_cast<uint32_t>(threadName.getLength()) + 1);
            log->logThreadLen   = static_cast<uint32_t>(threadName.getLength());
        }
        else
        {
            log->logThread[0]   = String::EmptyChar;
            log->logThreadLen   = 0u;
        }
    }

    return msgLog;
}

AREG_API_IMPL void NELogging::logMessage(const RemoteMessage& message)
{
    return LogManager::logMessage(message);
}

AREG_API_IMPL RemoteMessage NELogging::messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeList & scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.setSource(source != NEService::COOKIE_UNKNOWN ? source : NELogging::getCookie());

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::logAnyMessageLocal(const NELogging::sLogMessage& logMessage)
{
    LogManager::logMessage(logMessage);
}

AREG_API_IMPL RemoteMessage NELogging::messageUpdateScopes(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeNames& scopeNames)
{
    RemoteMessage msgScope;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogUpdateScopes));
        msgScope.setTarget(target);
        msgScope.setSource(source);
        msgScope << scopeNames.getSize();
        const std::vector<NELogging::sScopeInfo>& list = scopeNames.getData();
        for (const auto & entry : list)
        {
            msgScope << entry;
        }
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::logAnyMessage(const NELogging::sLogMessage& logMessage)
{
    LogManager::logMessage(SharedBuffer(reinterpret_cast<const unsigned char *>(&logMessage), sizeof(NELogging::sLogMessage)));
}

AREG_API_IMPL RemoteMessage NELogging::messageUpdateScope(const ITEM_ID& source, const ITEM_ID& target, const String& scopeName, unsigned int scopeId, unsigned int scopePrio)
{
    RemoteMessage msgScope;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogUpdateScopes));
        msgScope.setTarget(target);
        msgScope.setSource(source);
        msgScope << 1u;
        msgScope << scopeName << scopeId << scopePrio;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::messageQueryInstances(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgQuery.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceQueryInstances));
        msgQuery.setTarget(target);
        msgQuery.setSource(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgQuery;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgQuery.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgQuery.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogQueryScopes));
        msgQuery.setTarget(target);
        msgQuery.setSource(source);
        msgQuery << target;
    }

    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::messageScopesUpdated(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeList& scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogScopesUpdated));
        msgScope.setTarget(target);
        msgScope.setSource(source);

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::messageSaveConfiguration(const ITEM_ID& source, const ITEM_ID& target)
{
    RemoteMessage msgRequest;
    if ((source != NEService::COOKIE_UNKNOWN) &&
        (target != NEService::COOKIE_UNKNOWN) &&
        (msgRequest.initMessage(_getLogEmptyMessage().rbHeader) != nullptr))
    {
        msgRequest.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceSaveLogConfiguration));
        msgRequest.setTarget(target);
        msgRequest.setSource(source);
        msgRequest << target;
    }

    return msgRequest;
}

AREG_API_IMPL RemoteMessage NELogging::messageConfigurationSaved(void)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogConfigurationSaved));
        msgScope.setTarget(NEService::COOKIE_LOGGER);
        msgScope.setSource(NELogging::getCookie());
    }

    return msgScope;
}

AREG_API_IMPL void NELogging::setLogDatabaseEngine(IELogDatabaseEngine * dbEngine)
{
    LogManager::setLogDatabaseEngine(dbEngine);
}

AREG_API_IMPL bool NELogging::forceStartLogging(void)
{
    LogManager::setDefaultConfiguration(false);
    LogManager::forceEnableLogging();
    return LogManager::forceActivateLogging();
}

AREG_API_IMPL bool NELogging::initAndStartLogging(const char * fileConfig /*= nullptr */)
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

AREG_API_IMPL const ITEM_ID & NELogging::getCookie(void)
{
    return LogManager::getConnectionCookie();
}

AREG_API_IMPL String NELogging::makePrioString(unsigned int priorities)
{
    return Identifier::convToString(priorities, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset));
}

AREG_API_IMPL unsigned int NELogging::makePriorities(const String& prioString)
{
    uint16_t id = static_cast<uint16_t>(Identifier::convFromString(prioString, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid)));
    return static_cast<NELogging::eLogPriority>(id);
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NELogging::startLogging(const char * /*fileConfig*/ /*= nullptr */ )
{
    return true;
}

AREG_API_IMPL void NELogging::stopLogging(bool /*waitComplete*/)
{
}

AREG_API_IMPL void NELogging::waitLoggingEnd(void)
{
}

AREG_API_IMPL void NELogging::activateScope(LogScope & /*logScope*/)
{
}

AREG_API_IMPL bool NELogging::isStarted( void )
{
    return true;
}

AREG_API_IMPL bool NELogging::isConfigured(void)
{
    return true;
}

AREG_API_IMPL bool NELogging::initializeLogging(const char * /*fileConfig*/)
{
    return true;
}

AREG_API_IMPL bool NELogging::isEnabled(void)
{
    return true;
}

AREG_API_IMPL bool NELogging::saveLogging( const char * /*configFile*/ )
{
    return true;
}

AREG_API_IMPL unsigned int NELogging::makeScopeId( const char * /*scopeName*/ )
{
    return 0;
}

AREG_API_IMPL unsigned int NELogging::makeScopeIdEx(const char* /*scopeName*/)
{
    return 0;
}

AREG_API_IMPL unsigned int NELogging::setScopePriority( const char * /*scopeName*/, unsigned int /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL unsigned int NELogging::getScopePriority( const char * /*scopeName*/ )
{
    return static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid);
}

AREG_API_IMPL RemoteMessage NELogging::createLogMessage(const NELogging::sLogMessage & /*logMessage*/, NELogging::eLogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    RemoteMessage msgLog;
    return msgLog;
}

AREG_API_IMPL void NELogging::logMessage(const RemoteMessage& /*message*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::messageRegisterScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::logAnyMessageLocal(const NELogging::sLogMessage & /*logMessage*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::messageUpdateScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeNames & /*scopeNames*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::logAnyMessage(const NELogging::sLogMessage & /*logMessage*/)
{
}

AREG_API_IMPL RemoteMessage NELogging::messageUpdateScope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const String & /*scopeName*/, unsigned int /*scopeId*/, unsigned int /*scopePrio*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::messageQueryInstances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::messageQueryScopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NELogging::messageScopesUpdated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NELogging::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NELogging::messageSaveConfiguration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgRequest;
    return msgRequest;
}

AREG_API_IMPL RemoteMessage NELogging::messageConfigurationSaved(void)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NELogging::setLogDatabaseEngine(IELogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool NELogging::forceStartLogging(void)
{
    return true;
}

AREG_API_IMPL bool NELogging::initAndStartLogging(const char * /*fileConfig*/ /*= nullptr */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID & NELogging::getCookie(void)
{
    return NEService::COOKIE_UNKNOWN;
}

AREG_API_IMPL String NELogging::makePrioString(unsigned int /*priorities*/)
{
    return NELogging::PRIO_NOTSET_STR;
}

AREG_API_IMPL unsigned int NELogging::makePriorities(const String& /*prioString*/)
{
    return static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset);
}

//////////////////////////////////////////////////////////////////////////
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
