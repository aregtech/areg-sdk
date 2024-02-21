/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceMessage.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Logger object, make logging by priority.
 ************************************************************************/

#include "areg/trace/TraceMessage.hpp"

#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/LogMessage.hpp"
#include "areg/trace/private/TraceEvent.hpp"
#include "areg/trace/private/TraceManager.hpp"

#include <stdarg.h>

#if AREG_LOGS

TraceMessage::TraceMessage( const TraceScope & traceScope )
    : mScopeName( traceScope.getScopeName() )
    , mScopeId  ( traceScope.mScopeId       )
    , mScopePrio( traceScope.mScopePrio     )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ NETrace::eLogMessageType::LogMessageScopeEnter, mScopeId, NETrace::PrioScope, mScopeName };
        TraceManager::logMessage(msg);
    }
}

TraceMessage::~TraceMessage( void )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ NETrace::eLogMessageType::LogMessageScopeExit, mScopeId, NETrace::PrioScope, mScopeName };
        TraceManager::logMessage(msg);
    }
}

void TraceMessage::logDebug( const char * format, ... ) const
{
    if ( isDbgEnabled() )
    {
        va_list args;
        va_start(args, format);
        TraceMessage::_sendLog(mScopeId, NETrace::PrioDebug, format, args);
        va_end(args);
    }
}

void TraceMessage::logInfo( const char * format, ... ) const
{
    if ( isInfoEnabled() )
    {
        va_list args;
        va_start(args, format);
        TraceMessage::_sendLog(mScopeId, NETrace::PrioInfo, format, args);
        va_end(args);
    }
}

void TraceMessage::logWarning(const char * format, ...) const
{
    if ( isWarnEnabled() )
    {
        va_list args;
        va_start(args, format);
        TraceMessage::_sendLog(mScopeId, NETrace::PrioWarning, format, args);
        va_end(args);
    }
}

void TraceMessage::logError( const char * format, ... ) const
{
    if ( isErrEnabled() )
    {
        va_list args;
        va_start(args, format);
        TraceMessage::_sendLog(mScopeId, NETrace::PrioError, format, args);
        va_end(args);
    }
}

void TraceMessage::logFatal( const char * format, ... ) const
{
    if ( isFatalEnabled() )
    {
        va_list args;
        va_start(args, format);
        TraceMessage::_sendLog(mScopeId, NETrace::PrioFatal, format, args);
        va_end(args);
    }
}

void TraceMessage::logMessage(NETrace::eLogPriority logPrio, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    TraceMessage::_sendLog(mScopeId, logPrio, format, args);
    va_end(args);
}

void TraceMessage::logTrace( NETrace::eLogPriority logPrio, const char * format, ... ) 
{
    va_list args;
    va_start(args, format);
    TraceMessage::_sendLog(NETrace::TRACE_SCOPE_ID_NONE, logPrio, format, args);
    va_end(args);
}

inline void TraceMessage::_sendLog( unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * format, va_list args )
{
    LogMessage logData(NETrace::eLogMessageType::LogMessageText, scopeId, msgPrio, nullptr, 0);
    logData.logMessageLen = String::formatStringList( logData.logMessage, NETrace::LOG_MESSAGE_IZE - 1, format, args );
    TraceManager::logMessage( logData );
}

#else   // AREG_LOGS

TraceMessage::TraceMessage(const TraceScope& /*traceScope*/)
{
}

TraceMessage::~TraceMessage(void)
{
}

#endif // AREG_LOGS
