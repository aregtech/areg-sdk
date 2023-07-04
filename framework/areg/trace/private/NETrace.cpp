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
#include "areg/trace/private/TraceManager.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/DateTime.hpp"

#include <string.h>

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
    case NETrace::eLogPriority::PrioIgnore:
    case NETrace::eLogPriority::PrioIgnoreLayout:
    case NETrace::eLogPriority::PrioAny:
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

    return NETrace::eLogPriority::PrioNotset;
}


NETrace::S_LogHeader::S_LogHeader( NETrace::eLogType logType /*= NETrace::LogUndefined*/ )
    : logLength     ( sizeof(NETrace::S_LogMessage) )
    , logType       ( logType                       )
    , logModuleId   ( 0                             )
    , logCookie     ( NETrace::COOKIE_LOCAL         )
{
}

NETrace::S_LogHeader::S_LogHeader(const NETrace::S_LogHeader & src)
    : logLength     ( src.logLength     )
    , logType       ( src.logType       )
    , logModuleId   ( src.logModuleId   )
    , logCookie     ( src.logCookie     )
{
}

NETrace::S_LogHeader & NETrace::S_LogHeader::operator = (const NETrace::S_LogHeader & src)
{
    if ( static_cast<const NETrace::S_LogHeader *>(this) != &src )
    {
        logLength   = src.logLength;
        logType     = src.logType;
        logModuleId = src.logModuleId;
    }

    return (*this);
}

NETrace::S_LogData::S_LogData(void)
    : traceThreadId     ( Thread::getCurrentThreadId()  )
    , traceScopeId      ( NETrace::TRACE_SCOPE_ID_NONE  )
    , traceTimestamp    ( DateTime::getNow()            )
    , traceMessagePrio  ( NETrace::PrioNotset           )
    , traceMessageLen   ( 0                             )
{
    traceMessage[0] = String::EmptyChar;
}

NETrace::S_LogData::S_LogData(const S_LogData & source)
    : traceThreadId     ( source.traceThreadId      )
    , traceScopeId      ( source.traceScopeId       )
    , traceTimestamp    ( source.traceTimestamp     )
    , traceMessagePrio  ( source.traceMessagePrio   )
    , traceMessageLen   ( source.traceMessageLen    )
{
    int len = NEMemory::memCopy( traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, source.traceMessage, source.traceMessageLen );
    traceMessage[len] = String::EmptyChar;
}

NETrace::S_LogData::S_LogData(unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : traceThreadId     ( Thread::getCurrentThreadId()  )
    , traceScopeId      ( scopeId                       )
    , traceTimestamp    ( DateTime::getNow()            )
    , traceMessagePrio  ( msgPrio                       )
    , traceMessageLen   ( MACRO_MIN( LOG_MESSAGE_BUFFER_SIZE - 1, msgLen) )
{
    int len = NEMemory::memCopy(traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, message, msgLen);
    traceMessageLen = len;
    traceMessage[len] = String::EmptyChar;
}

NETrace::S_LogData & NETrace::S_LogData::operator = (const NETrace::S_LogData & source )
{
    if (static_cast<NETrace::S_LogData *>(this) != &source )
    {
        traceThreadId   = source.traceThreadId;
        traceScopeId    = source.traceScopeId;
        traceTimestamp  = source.traceTimestamp;
        traceMessagePrio= source.traceMessagePrio;
        traceMessageLen = source.traceMessageLen;

        int len = NEMemory::memCopy( traceMessage, LOG_MESSAGE_BUFFER_SIZE - 1, source.traceMessage, source.traceMessageLen );
        traceMessageLen     = len;
        traceMessage[len]   = String::EmptyChar;
    }

    return (*this);
}

NETrace::S_LogMessage::S_LogMessage(void)
    : lmHeader  ( NETrace::LogUndefined )
    , lmTrace   ( )
{
}

NETrace::S_LogMessage::S_LogMessage(const NETrace::S_LogMessage & src)
    : lmHeader  ( src.lmHeader )
    , lmTrace   ( src.lmTrace )
{
}

NETrace::S_LogMessage::S_LogMessage(NETrace::eLogType logType)
    : lmHeader  ( logType )
    , lmTrace   ( )
{
}

NETrace::S_LogMessage::S_LogMessage(NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : lmHeader  ( logType )
    , lmTrace   ( scopeId, msgPrio, message, msgLen )
{
}


NETrace::S_LogMessage & NETrace::S_LogMessage::operator = (const NETrace::S_LogMessage & src)
{
    if ( static_cast<const NETrace::S_LogMessage *>(this) != &src )
    {
        lmHeader= src.lmHeader;
        lmTrace = src.lmTrace;
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

AREG_API_IMPL void NETrace::stopLogging(void)
{
#if AREG_LOGS
    TraceManager::stopLogging();
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
    return TraceManager::isLoggingEnabled();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::configureLoging(const char * fileConfig)
{
#if AREG_LOGS
    return TraceManager::configureLogging(fileConfig);
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

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
#if AREG_LOGS
    return TraceManager::forceActivateLogging();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::configAndStart(const char * fileConfig /*= nullptr */)
{
#if AREG_LOGS
    if (TraceManager::configureLogging(fileConfig))
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
