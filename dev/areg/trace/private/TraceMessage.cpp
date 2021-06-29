/************************************************************************
 * \file        areg/trace/private/TraceMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Logger object, make logging by priority.
 ************************************************************************/

#include "areg/trace/TraceMessage.hpp"

#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/LogMessage.hpp"
#include "areg/trace/private/TraceEvent.hpp"
#include "areg/trace/private/TraceManager.hpp"

#include <stdarg.h>

TraceMessage::TraceMessage( const TraceScope & traceScope )
    : mScopeName    ( traceScope.getScopeName() )
    , mScopeId      ( traceScope.mScopeId )
    , mScopePrio    ( traceScope.mScopePrio )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg(NETrace::LogScopeEnter, mScopeId, NETrace::PrioScope, mScopeName);
        TraceManager::sendLogMessage(msg);
    }
}


TraceMessage::~TraceMessage( void )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg(NETrace::LogScopeExit, mScopeId, NETrace::PrioScope, mScopeName);
        TraceManager::sendLogMessage(msg);
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
    LogMessage logData(NETrace::LogMessage, scopeId, msgPrio, NULL);
    String::formatStringList( logData.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, format, args );
    TraceManager::sendLogMessage( logData );
}
