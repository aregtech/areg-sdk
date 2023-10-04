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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/trace/NETrace.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/NEService.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"
#include "areg/ipc/NEConnection.hpp"

#include <string.h>

namespace
{
    const NEMemory::sRemoteMessage & _getLogRegisterScopes(void)
    {
        static constexpr NEMemory::sRemoteMessage _messageRegisterScopes
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
                , static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes)   // rbhMessageId
                , NEMemory::MESSAGE_SUCCESS                     // rbhResult
                , NEService::SEQUENCE_NUMBER_NOTIFY             // rbhSequenceNr
            }
            , { static_cast<char>(0) }
        };

        return _messageRegisterScopes;
    }

    const NEMemory::sRemoteMessage & _getLogMessage(void)
    {
        static constexpr NEMemory::sRemoteMessage _messageRegisterScopes
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

        return _messageRegisterScopes;
    }
}

NETrace::sLogMessage::sLogMessage(NETrace::eMessageType msgType)
    : logMsgType    { msgType}
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { NEService::COOKIE_LOCAL }
    , logModuleId   { Process::CURRENT_PROCESS }
    , logThreadId   { Thread::INVALID_THREAD_ID }
    , logTimestamp  { DateTime::INVALID_TIME }
    , logScopeId    { NETrace::TRACE_SCOPE_ID_NONE }
    , logMessagePrio{ NETrace::eLogPriority::PrioNotset }
    , logMessageLen { 0 }
    , logMessage    {'\0'}
{
}

NETrace::sLogMessage::sLogMessage(NETrace::eMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen)
    : logMsgType    { msgType }
    , logSource     { NEService::COOKIE_LOCAL }
    , logTarget     { NEService::COOKIE_LOGGER }
    , logCookie     { TraceManager::getConnectionCookie() }
    , logModuleId   { Process::getInstance().getId() }
    , logThreadId   { Thread::getCurrentThreadId() }
    , logTimestamp  { DateTime::getNow() }
    , logScopeId    { scopeId }
    , logMessagePrio{ msgPrio }
    , logMessageLen { msgLen }
    , logMessage    { '\0' }
{
    int len = NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, message, msgLen);
    logMessage[len] = String::EmptyChar;
}

NETrace::sLogMessage::sLogMessage(const NETrace::sLogMessage & src)
    : logMsgType    { src.logMsgType }
    , logSource     { src.logSource }
    , logTarget     { src.logTarget }
    , logCookie     { src.logCookie }
    , logModuleId   { src.logModuleId }
    , logThreadId   { src.logThreadId }
    , logTimestamp  { src.logTimestamp }
    , logScopeId    { src.logScopeId }
    , logMessagePrio{ src.logMessagePrio }
    , logMessageLen { src.logMessageLen }
    , logMessage    { '\0' }
{
    int len = NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, src.logMessage, src.logMessageLen);
    logMessage[len] = String::EmptyChar;
}

NETrace::sLogMessage & NETrace::sLogMessage::operator = (const NETrace::sLogMessage & src)
{
    if (this != &src)
    {
        logMsgType    = src.logMsgType;
        logSource     = src.logSource;
        logTarget     = src.logTarget;
        logCookie     = src.logCookie;
        logModuleId   = src.logModuleId;
        logThreadId   = src.logThreadId;
        logTimestamp  = src.logTimestamp;
        logScopeId    = src.logScopeId;
        logMessagePrio= src.logMessagePrio;
        logMessageLen = src.logMessageLen;
        int len = NEMemory::memCopy(logMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, src.logMessage, src.logMessageLen);
        logMessage[len] = String::EmptyChar;
    }

    return (*this);
}


AREG_API_IMPL const String& NETrace::convToString( NETrace::eLogPriority prio )
{
    switch ( prio )
    {
    case NETrace::eLogPriority::PrioNotset:
        return NETrace::PRIO_NOTSET_STR;
    case NETrace::eLogPriority::PrioFatal:
        return NETrace::PRIO_FATAL_STR;
    case NETrace::eLogPriority::PrioError:
        return NETrace::PRIO_ERROR_STR;
    case NETrace::eLogPriority::PrioWarning:
        return NETrace::PRIO_WARNING_STR;
    case NETrace::eLogPriority::PrioInfo:
        return NETrace::PRIO_INFO_STR;
    case NETrace::eLogPriority::PrioDebug:
        return NETrace::PRIO_DEBUG_STR;
    case NETrace::eLogPriority::PrioScope:
        return NETrace::PRIO_SCOPE_STR;
    case NETrace::eLogPriority::PrioInvalid:
    case NETrace::eLogPriority::PrioValidLogs:
    case NETrace::eLogPriority::PrioIgnore:
    case NETrace::eLogPriority::PrioIgnoreLayout:
    case NETrace::eLogPriority::PrioLogs:
    case NETrace::eLogPriority::PrioAny:
    case NETrace::eLogPriority::PrioValid:
        return NETrace::PRIO_NO_PRIO;
    default:
        ASSERT(false);
        return NETrace::PRIO_NO_PRIO;
    }
}

AREG_API_IMPL NETrace::eLogPriority NETrace::convFromString( const String& strPrio )
{
    if ( strPrio.isEmpty() == false )
    {
        if ( strPrio.compare( NETrace::PRIO_NOTSET_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioNotset;
        else if (strPrio.compare( NETrace::PRIO_SCOPE_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioScope;
        else if (strPrio.compare( NETrace::PRIO_DEBUG_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioDebug;
        else if (strPrio.compare( NETrace::PRIO_INFO_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioInfo;
        else if (strPrio.compare( NETrace::PRIO_WARNING_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioWarning;
        else if (strPrio.compare( NETrace::PRIO_ERROR_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioError;
        else if (strPrio.compare( NETrace::PRIO_FATAL_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioFatal;
    }

    return NETrace::eLogPriority::PrioInvalid;
}

AREG_API_IMPL String NETrace::makePrioString( unsigned int priorities )
{
    String result;
#if AREG_LOGS
    NETrace::eLogPriority prio1{NETrace::eLogPriority::PrioInvalid};
    NETrace::eLogPriority prio2{NETrace::eLogPriority::PrioInvalid};

    if ( (priorities & static_cast<unsigned int>(NETrace::eLogPriority::PrioValidLogs)) != 0 )
    {
        prio1 = static_cast<NETrace::eLogPriority>(priorities & static_cast<unsigned int>(NETrace::eLogPriority::PrioLogs));
        if ( prio1 != NETrace::eLogPriority::PrioInvalid )
        {
            priorities &= ~static_cast<unsigned int>(NETrace::eLogPriority::PrioLogs);
            prio2 = static_cast<NETrace::eLogPriority>(priorities) == NETrace::eLogPriority::PrioScope ? NETrace::eLogPriority::PrioScope : prio2;
        }
        else
        {
            prio1 = static_cast<NETrace::eLogPriority>(priorities);
        }
    }

    if ( NETrace::isValidLogPriority( prio1 ) && NETrace::isValidLogPriority( prio2 ) )
    {
        if ( NETrace::isLogScope( prio1 ) || NETrace::isLogScope( prio2 ) )
        {
            constexpr int size{ 0xFF + 1 };
            char buffer[ size ];
            String::formatString( buffer, size, "%s | %s", NETrace::convToString( prio1 ).getString( ), NETrace::convToString( prio2 ).getString() );
            result = buffer;
        }
    }
    else if ( NETrace::isValidLogPriority( prio1 ) )
    {
        result = NETrace::convToString( prio1 );
    }
#endif  //     String result;

    return result;
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

AREG_API_IMPL const String& NETrace::getConfigFile(void)
{
#if AREG_LOGS
    return TraceManager::getConfigFile();
#else   // !AREG_LOGS
    return String::getEmptyString();
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::saveLogging( const char * configFile )
{
#if AREG_LOGS
    return TraceManager::saveLogConfig( configFile );
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::readConfiguration( const FileBase & file )
{
#if AREG_LOGS
    return TraceManager::readLogConfig( file );
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::saveConfiguration( FileBase & file )
{
#if AREG_LOGS
    return TraceManager::saveLogConfig( file );
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

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopesStart(unsigned int scopeCount)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogRegisterScopes().rbHeader) != nullptr)
    {
        const ITEM_ID & cookie{ NETrace::getCookie() };
        msgScope.setSource(cookie);
        msgScope << NETrace::eScopeList::ScopeListStart;
        msgScope << scopeCount;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopesEnd( )
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogRegisterScopes().rbHeader) != nullptr)
    {
        const ITEM_ID & cookie{ NETrace::getCookie() };
        msgScope.setSource(cookie);
        msgScope << NETrace::eScopeList::ScopeListEnd;
    }

    return msgScope;
}

AREG_API_IMPL RemoteMessage NETrace::messageLog(const NETrace::sLogMessage & logMessage)
{
    RemoteMessage msgLog;
    if (msgLog.initMessage(_getLogMessage().rbHeader) != nullptr)
    {
        msgLog.setSource(NETrace::getCookie());
        msgLog << logMessage;
    }

    return msgLog;
}

AREG_API_IMPL RemoteMessage NETrace::messageRegisterScopes(const ScopeList & scopeList, SCOPEPOS & startAt, unsigned int maxEntries /*= 0xFFFFFFFF*/)
{
    RemoteMessage msgScope;
    if (msgScope.initMessage(_getLogRegisterScopes().rbHeader) != nullptr)
    {
        const ITEM_ID & cookie{ NETrace::getCookie() };
        msgScope.setSource(cookie);
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

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
#if AREG_LOGS
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
