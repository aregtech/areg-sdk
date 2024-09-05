/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NETrace.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/trace/NETrace.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/NEService.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"

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
     * \brief   Returns predefined structure for to log a message on remote log collector / observer.
     *          The structure is used as a template to initialize remote communication message.
     *          The source of the log should be set before sending the message.
     *          Otherwise, it is ignored by the logger and the message is dropped.
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

    void _storeScopeList(RemoteMessage& msgRemote, const NETrace::ScopeList& scopeList)
    {
        msgRemote << static_cast<uint32_t>(scopeList.getSize());
        const auto& list{ scopeList.getData() };
        for (const auto& entry : list)
        {
            const TraceScope* scope = entry.second;
            ASSERT(scope != nullptr);
            msgRemote << *scope;
        }
    }
}
#endif // AREG_LOGS

NETrace::sLogMessage::sLogMessage(NETrace::eLogMessageType msgType)
    : logDataType   { NETrace::eLogDataType::LogDataLocal }
    , logMsgType    { msgType }
    , logMessagePrio{ NETrace::eLogPriority::PrioNotset }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { NEService::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logScopeId    { NETrace::TRACE_SCOPE_ID_NONE }
    , logMessageLen { 0 }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
}

#if AREG_LOGS
NETrace::sLogMessage::sLogMessage(NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen)
    : logDataType   { NETrace::eLogDataType::LogDataLocal }
    , logMsgType    { msgType }
    , logMessagePrio{ msgPrio }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { TraceManager::getConnectionCookie() }
    , logModuleId   { Process::getInstance().getId() }
    , logThreadId   { Thread::getCurrentThreadId() }
    , logTimestamp  { DateTime::getNow() }
    , logScopeId    { scopeId }
    , logMessageLen { msgLen }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    uint32_t len = message != nullptr ? NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_IZE - 1, message, msgLen) : 0u;
    logMessage[len] = String::EmptyChar;
}
#else   // AREG_LOGS
NETrace::sLogMessage::sLogMessage(NETrace::eLogMessageType msgType, unsigned int /*scopeId*/, NETrace::eLogPriority /*msgPrio*/, const char* /*message*/, unsigned int /*msgLen*/)
    : logDataType{ NETrace::eLogDataType::LogDataLocal }
    , logMsgType{ msgType }
    , logMessagePrio{ NETrace::eLogPriority::PrioNotset }
    , logSource{ NEService::COOKIE_LOCAL }
    , logTarget{ NEService::COOKIE_LOGGER }
    , logCookie{ NEService::COOKIE_LOCAL }
    , logModuleId{ Process::CURRENT_PROCESS }
    , logThreadId{ Thread::INVALID_THREAD_ID }
    , logTimestamp{ DateTime::INVALID_TIME }
    , logScopeId{ NETrace::TRACE_SCOPE_ID_NONE }
    , logMessageLen{ 0 }
    , logMessage{ '\0' }
    , logThreadLen{ 0 }
    , logThread{ '\0' }
    , logModuleLen{ 0 }
    , logModule{ '\0' }
{
}
#endif  // AREG_LOGS

NETrace::sLogMessage::sLogMessage(const NETrace::sLogMessage & src)
    : logDataType   { src.logDataType }
    , logMsgType    { src.logMsgType }
    , logMessagePrio{ src.logMessagePrio }
    , logSource     { src.logSource }
    , logTarget     { src.logTarget }
    , logCookie     { src.logCookie }
    , logModuleId   { src.logModuleId }
    , logThreadId   { src.logThreadId }
    , logTimestamp  { src.logTimestamp }
    , logScopeId    { src.logScopeId }
    , logMessageLen { src.logMessageLen }
    , logMessage    { '\0' }
    , logThreadLen  { 0 }
    , logThread     { '\0' }
    , logModuleLen  { 0 }
    , logModule     { '\0' }
{
    NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
}

NETrace::sLogMessage & NETrace::sLogMessage::operator = (const NETrace::sLogMessage & src)
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
        logScopeId      = src.logScopeId;
        logMessageLen   = src.logMessageLen;

        if (logDataType == NETrace::eLogDataType::LogDataRemote)
        {
            logThreadLen = 0;
            logThread[0] = String::EmptyChar;
            logThreadLen = 0;
            logModule[0] = String::EmptyChar;
        }

        NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_IZE, src.logMessage, src.logMessageLen + 1);
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NETrace::startLogging(const char * fileConfig /*= nullptr */ )
{
    return TraceManager::startLogging(fileConfig);
}

AREG_API_IMPL void NETrace::stopLogging(bool waitComplete)
{
    TraceManager::stopLogging(waitComplete);
}

AREG_API_IMPL void NETrace::waitLoggingEnd(void)
{
    TraceManager::waitLoggingEnd();
}

AREG_API_IMPL void NETrace::activateScope(TraceScope & traceScope)
{
    TraceManager::activateTraceScope(traceScope);
}

AREG_API_IMPL bool NETrace::isStarted( void )
{
    return TraceManager::isLoggingStarted();
}

AREG_API_IMPL bool NETrace::isConfigured(void)
{
    return TraceManager::isLoggingConfigured();
}

AREG_API_IMPL bool NETrace::initializeLogging(const char * fileConfig)
{
    return TraceManager::readLogConfig(fileConfig);
}

AREG_API_IMPL bool NETrace::isEnabled(void)
{
    return TraceManager::isLoggingEnabled();
}

AREG_API_IMPL bool NETrace::saveLogging( const char * configFile )
{
    return TraceManager::saveLogConfig(configFile);
}

AREG_API_IMPL unsigned int NETrace::makeScopeId( const char * scopeName )
{
    return  NEMath::crc32Calculate(scopeName);
}

AREG_API_IMPL unsigned int NETrace::makeScopeIdEx(const char* scopeName)
{
    return  (NEString::stringEndsWith<char>(scopeName, NELogging::SYNTAX_SCOPE_GROUP, true) ? NEMath::CHECKSUM_IGNORE : NETrace::makeScopeId(scopeName));
}

AREG_API_IMPL unsigned int NETrace::setScopePriority( const char * scopeName, unsigned int newPrio )
{
    return TraceManager::setScopePriority( scopeName, newPrio );
}

AREG_API_IMPL unsigned int NETrace::getScopePriority( const char * scopeName )
{
    return TraceManager::getScopePriority( scopeName );
}

AREG_API_IMPL RemoteMessage NETrace::createLogMessage(const NETrace::sLogMessage& logMessage, NETrace::eLogDataType dataType, const ITEM_ID& srcCookie)
{
    RemoteMessage msgLog;
    if (msgLog.initMessage(_getLogMessage().rbHeader, sizeof(NETrace::sLogMessage)) != nullptr)
    {
        msgLog << logMessage;
        msgLog.setSizeUsed(sizeof(NETrace::sLogMessage));
        msgLog.moveToEnd();
        msgLog.setSource(srcCookie);
        NETrace::sLogMessage* log = reinterpret_cast<NETrace::sLogMessage*>(msgLog.getBuffer());
        log->logCookie  = srcCookie;
        log->logDataType = dataType;

        if (NETrace::eLogDataType::LogDataLocal != dataType)
        {
            const String& threadName{ Thread::getThreadName(static_cast<id_type>(log->logThreadId)) };
            NEMemory::memCopy(log->logThread, NETrace::LOG_NAMES_SIZE, threadName.getString(), static_cast<uint32_t>(threadName.getLength()) + 1);
            log->logThreadLen   = static_cast<uint32_t>(threadName.getLength());

            const String& module = Process::getInstance().getAppName();
            NEMemory::memCopy(log->logModule, NETrace::LOG_NAMES_SIZE, module.getString(), static_cast<uint32_t>(module.getLength()) + 1);
            log->logModuleLen   = static_cast<uint32_t>(module.getLength());
        }
    }

    return msgLog;
}

AREG_API_IMPL void NETrace::logMessage(const RemoteMessage& message)
{
    return TraceManager::logMessage(message);
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const NETrace::ScopeList & scopeList)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.setSource(source != NEService::COOKIE_UNKNOWN ? source : NETrace::getCookie());

        _storeScopeList(msgScope, scopeList);
    }

    return msgScope;
}

AREG_API_IMPL void NETrace::logAnyMessageLocal(const NETrace::sLogMessage& logMessage)
{
    TraceManager::logMessage(logMessage);
}

AREG_API_IMPL RemoteMessage NETrace::messageUpdateScopes(const ITEM_ID& source, const ITEM_ID& target, const NETrace::ScopeNames& scopeNames)
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
        const std::vector<NETrace::sScopeInfo>& list = scopeNames.getData();
        for (const auto & entry : list)
        {
            msgScope << entry;
        }
    }

    return msgScope;
}

AREG_API_IMPL void NETrace::logAnyMessage(const NETrace::sLogMessage& logMessage)
{
    TraceManager::logMessage(SharedBuffer(reinterpret_cast<const unsigned char *>(&logMessage), sizeof(NETrace::sLogMessage)));
}

AREG_API_IMPL RemoteMessage NETrace::messageUpdateScope(const ITEM_ID& source, const ITEM_ID& target, const String& scopeName, unsigned int scopeId, unsigned int scopePrio)
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

AREG_API_IMPL RemoteMessage NETrace::messageQueryInstances(const ITEM_ID& source, const ITEM_ID& target)
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

AREG_API_IMPL RemoteMessage NETrace::messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target)
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

AREG_API_IMPL RemoteMessage NETrace::messageScopesUpdated(const ITEM_ID& source, const ITEM_ID& target, const NETrace::ScopeList& scopeList)
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

AREG_API_IMPL RemoteMessage NETrace::messageSaveConfiguration(const ITEM_ID& source, const ITEM_ID& target)
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

AREG_API_IMPL RemoteMessage NETrace::messageConfigurationSaved(void)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogConfigurationSaved));
        msgScope.setTarget(NEService::COOKIE_LOGGER);
        msgScope.setSource(NETrace::getCookie());
    }

    return msgScope;
}

AREG_API_IMPL void NETrace::setLogDatabaseEngine(IELogDatabaseEngine * dbEngine)
{
    TraceManager::setLogDatabaseEngine(dbEngine);
}

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
    TraceManager::setDefaultConfiguration(false);
    TraceManager::forceEnableLogging();
    return TraceManager::forceActivateLogging();
}

AREG_API_IMPL bool NETrace::initAndStartLogging(const char * fileConfig /*= nullptr */)
{
    if (TraceManager::readLogConfig(fileConfig))
    {
        TraceManager::forceEnableLogging();
        return TraceManager::startLogging(nullptr);
    }
    else
    {
        return TraceManager::forceActivateLogging();
    }
}

AREG_API_IMPL const ITEM_ID & NETrace::getCookie(void)
{
    return TraceManager::getConnectionCookie();
}

AREG_API_IMPL String NETrace::makePrioString(unsigned int priorities)
{
    return Identifier::convToString(priorities, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset));
}

AREG_API_IMPL unsigned int NETrace::makePriorities(const String& prioString)
{
    return static_cast<NETrace::eLogPriority>(Identifier::convFromString(prioString, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid)));
}

//////////////////////////////////////////////////////////////////////////
#else   // !AREG_LOGS
//////////////////////////////////////////////////////////////////////////

AREG_API_IMPL bool NETrace::startLogging(const char * /*fileConfig*/ /*= nullptr */ )
{
    return true;
}

AREG_API_IMPL void NETrace::stopLogging(bool /*waitComplete*/)
{
}

AREG_API_IMPL void NETrace::waitLoggingEnd(void)
{
}

AREG_API_IMPL void NETrace::activateScope(TraceScope & /*traceScope*/)
{
}

AREG_API_IMPL bool NETrace::isStarted( void )
{
    return true;
}

AREG_API_IMPL bool NETrace::isConfigured(void)
{
    return true;
}

AREG_API_IMPL bool NETrace::initializeLogging(const char * /*fileConfig*/)
{
    return true;
}

AREG_API_IMPL bool NETrace::isEnabled(void)
{
    return true;
}

AREG_API_IMPL bool NETrace::saveLogging( const char * /*configFile*/ )
{
    return true;
}

AREG_API_IMPL unsigned int NETrace::makeScopeId( const char * /*scopeName*/ )
{
    return 0;
}

AREG_API_IMPL unsigned int NETrace::makeScopeIdEx(const char* /*scopeName*/)
{
    return 0;
}

AREG_API_IMPL unsigned int NETrace::setScopePriority( const char * /*scopeName*/, unsigned int /*newPrio*/ )
{
    return true;
}

AREG_API_IMPL unsigned int NETrace::getScopePriority( const char * /*scopeName*/ )
{
    return static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid);
}

AREG_API_IMPL RemoteMessage NETrace::createLogMessage(const NETrace::sLogMessage & /*logMessage*/, NETrace::eLogDataType /*dataType*/, const ITEM_ID & /*srcCookie*/)
{
    RemoteMessage msgLog;
    return msgLog;
}

AREG_API_IMPL void NETrace::logMessage(const RemoteMessage& /*message*/)
{
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NETrace::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NETrace::logAnyMessageLocal(const NETrace::sLogMessage & /*logMessage*/)
{
}

AREG_API_IMPL RemoteMessage NETrace::messageUpdateScopes(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NETrace::ScopeNames & /*scopeNames*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NETrace::logAnyMessage(const NETrace::sLogMessage & /*logMessage*/)
{
}

AREG_API_IMPL RemoteMessage NETrace::messageUpdateScope(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const String & /*scopeName*/, unsigned int /*scopeId*/, unsigned int /*scopePrio*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageQueryInstances(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NETrace::messageQueryScopes(const ITEM_ID& /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgQuery;
    return msgQuery;
}

AREG_API_IMPL RemoteMessage NETrace::messageScopesUpdated(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/, const NETrace::ScopeList & /*scopeList*/)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageSaveConfiguration(const ITEM_ID & /*source*/, const ITEM_ID & /*target*/)
{
    RemoteMessage msgRequest;
    return msgRequest;
}

AREG_API_IMPL RemoteMessage NETrace::messageConfigurationSaved(void)
{
    RemoteMessage msgScope;
    return msgScope;
}

AREG_API_IMPL void NETrace::setLogDatabaseEngine(IELogDatabaseEngine * /*dbEngine*/)
{
}

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
    return true;
}

AREG_API_IMPL bool NETrace::initAndStartLogging(const char * /*fileConfig*/ /*= nullptr */)
{
    return true;
}

AREG_API_IMPL const ITEM_ID & NETrace::getCookie(void)
{
    return NEService::COOKIE_UNKNOWN;
}

AREG_API_IMPL String NETrace::makePrioString(unsigned int /*priorities*/)
{
    return NETrace::PRIO_NOTSET_STR;
}

AREG_API_IMPL unsigned int NETrace::makePriorities(const String& /*prioString*/)
{
    return static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset);
}

//////////////////////////////////////////////////////////////////////////
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
