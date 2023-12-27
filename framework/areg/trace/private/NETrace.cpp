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
#include "areg/ipc/private/NEConnection.hpp"

#include <string.h>

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
     * \brief   Returns predefined structure for to log a message on remote collector / observer.
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
}

NETrace::sLogMessage::sLogMessage(NETrace::eLogMessageType msgType)
    : logMsgType    { msgType }
    , logDataType   { NETrace::eLogDataType::LogDataLocal }
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

NETrace::sLogMessage::sLogMessage(NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen)
    : logMsgType    { msgType }
    , logDataType   { NETrace::eLogDataType::LogDataLocal }
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
    uint32_t len = NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_IZE - 1, message, msgLen);
    logMessage[len] = String::EmptyChar;
}

NETrace::sLogMessage::sLogMessage(const NETrace::sLogMessage & src)
    : logMsgType    { src.logMsgType }
    , logDataType   { src.logDataType }
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
        logMsgType      = src.logMsgType;
        logDataType     = src.logDataType;
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

AREG_API_IMPL bool NETrace::startLogging(const char * fileConfig /*= nullptr */ )
{
#if AREG_LOGS
    return TraceManager::startLogging(fileConfig);
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL void NETrace::stopLogging(bool waitComplete)
{
#if AREG_LOGS
    TraceManager::stopLogging(waitComplete);
#endif  // AREG_LOGS
}

AREG_API_IMPL void NETrace::waitLoggingEnd(void)
{
#if AREG_LOGS
    TraceManager::waitLoggingEnd();
#endif  // AREG_LOGS
}

AREG_API_IMPL void NETrace::activateScope(TraceScope & traceScope)
{
#if AREG_LOGS
    TraceManager::activateTraceScope(traceScope);
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isStarted( void )
{
#if AREG_LOGS
    return TraceManager::isLoggingStarted();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isConfigured(void)
{
#if AREG_LOGS
    return TraceManager::isLoggingConfigured();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::initializeLogging(const char * fileConfig)
{
#if AREG_LOGS
    return TraceManager::readLogConfig(fileConfig);
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isEnabled(void)
{
#if AREG_LOGS
    return TraceManager::isLoggingEnabled();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::saveLogging( const char * configFile )
{
#if AREG_LOGS
    return TraceManager::saveLogConfig(configFile);
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL unsigned int NETrace::makeScopeId( const char * scopeName )
{
#if AREG_LOGS
    return  NEMath::crc32Calculate( scopeName );
#else   // !AREG_LOGS
    return 0;
#endif  // AREG_LOGS
}

AREG_API_IMPL unsigned int NETrace::setScopePriority( const char * scopeName, unsigned int newPrio )
{
#if AREG_LOGS
    return TraceManager::setScopePriority( scopeName, newPrio );
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL unsigned int NETrace::getScopePriority( const char * scopeName )
{
#if AREG_LOGS
    return TraceManager::getScopePriority( scopeName );
#else   // !AREG_LOGS
    return static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid);
#endif  // AREG_LOGS
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
            const String& threadName{ Thread::getThreadName(log->logThreadId) };
            NEMemory::memCopy(log->logThread, NETrace::LOG_NAMES_SIZE, threadName.getString(), threadName.getLength() + 1);
            log->logThreadLen   = threadName.getLength();

            const String& module = Process::getInstance().getAppName();
            NEMemory::memCopy(log->logModule, NETrace::LOG_NAMES_SIZE, module.getString(), module.getLength() + 1);
            log->logModuleLen   = module.getLength();
        }
    }

    return msgLog;
}

AREG_API_IMPL void NETrace::logMessage(const RemoteMessage& message)
{
    return TraceManager::logMessage(message);
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopesStart(const ITEM_ID & source, const ITEM_ID & target, unsigned int scopeCount)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.setSource(source != NEService::COOKIE_UNKNOWN ? source : NETrace::getCookie());
        msgScope << NETrace::eScopeList::ScopeListStart;
        msgScope << scopeCount;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopesEnd(const ITEM_ID & source, const ITEM_ID & target)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.setSource(source != NEService::COOKIE_UNKNOWN ? source : NETrace::getCookie());
        msgScope << NETrace::eScopeList::ScopeListEnd;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const NETrace::ScopeList & scopeList, NETrace::SCOPEPOS & startAt, unsigned int maxEntries /*= 0xFFFFFFFF*/)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogEmptyMessage().rbHeader) != nullptr)
    {
        msgScope.setMessageId(static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
        msgScope.setTarget(target != NEService::COOKIE_UNKNOWN ? target : NEService::COOKIE_LOGGER);
        msgScope.setSource(source != NEService::COOKIE_UNKNOWN ? source : NETrace::getCookie());
        msgScope << NETrace::eScopeList::ScopeListContinue;

        SCOPEPOS end = scopeList.invalidPosition();
        SCOPEPOS pos = startAt == end ? scopeList.firstPosition() : startAt;

        if (maxEntries == 0xFFFFFFFF)
        {
            maxEntries = scopeList.getSize();
        }

        unsigned int numPos = msgScope.getPosition();
        msgScope << maxEntries; // initially, the number of scopes is the complete list.

        unsigned int count{ 0 };
        for (; (count < maxEntries) && (pos != end); ++ count)
        {
            TraceScopePair tracePair;
            scopeList.getAtPosition(pos, tracePair);
            ASSERT(tracePair.second != nullptr);
            msgScope << (*tracePair.second);
            pos = scopeList.nextPosition(pos);
        }

        if (count != maxEntries)
        {
            count = MACRO_MIN(count, maxEntries);
            msgScope.setPosition(static_cast<int>(numPos), IECursorPosition::eCursorPosition::PositionBegin);
            msgScope << count;
            msgScope.moveToEnd();
        }
    }

    return msgScope;
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

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
#if AREG_LOGS
    TraceManager::setDefaultConfiguration(false);
    TraceManager::forceEnableLogging();
    return TraceManager::forceActivateLogging();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::initAndStartLogging(const char * fileConfig /*= nullptr */)
{
#if AREG_LOGS
    if (TraceManager::readLogConfig(fileConfig))
    {
        TraceManager::forceEnableLogging();
        return TraceManager::startLogging(nullptr);
    }
    else
    {
        return TraceManager::forceActivateLogging();
    }
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL const ITEM_ID & NETrace::getCookie(void)
{
#if AREG_LOGS
    return TraceManager::getConnectionCookie();
#else   // !AREG_LOGS
    return NEService::COOKIE_UNKNOWN;
#endif  // AREG_LOGS
}

AREG_API_IMPL String NETrace::makePrioString(unsigned int priorities)
{
    return Identifier::convToString(priorities, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset));
}

AREG_API_IMPL unsigned int NETrace::makePriorities(const String& prioString)
{
    return static_cast<NETrace::eLogPriority>(Identifier::convFromString(prioString, NEApplication::LogScopePriorityIndentifiers, static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid)));
}
