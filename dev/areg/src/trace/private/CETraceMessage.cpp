/************************************************************************
 * \file        areg/src/trace/private/CETraceMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Logger object, make logging by priority.
 ************************************************************************/

#include "areg/src/trace/CETraceMessage.hpp"

#include "areg/src/trace/CETraceScope.hpp"
#include "areg/src/trace/private/CELogMessage.hpp"
#include "areg/src/trace/private/CETraceEvent.hpp"
#include "areg/src/trace/private/CETraceManager.hpp"

#include <stdarg.h>

#define IS_ENABLED_SCOPE(scopePrio)     (scopePrio &  static_cast<unsigned int>(NETrace::PrioScope))
#define IS_ENABLED_DEBUG(scopePrio)     (scopePrio >= static_cast<unsigned int>(NETrace::PrioDebug))
#define IS_ENABLED_INFO(scopePrio)      (scopePrio >= static_cast<unsigned int>(NETrace::PrioInfo))
#define IS_ENABLED_WARNING(scopePrio)   (scopePrio >= static_cast<unsigned int>(NETrace::PrioWarning))
#define IS_ENABLED_ERROR(scopePrio)     (scopePrio >= static_cast<unsigned int>(NETrace::PrioError))
#define IS_ENABLED_FATAL(scopePrio)     (scopePrio >= static_cast<unsigned int>(NETrace::PrioFatal))
#define IS_ENABLED_LOG(scopePrio)       (scopePrio != static_cast<unsigned int>(NETrace::PrioNotset))

CETraceMessage::CETraceMessage( const CETraceScope & traceScope )
    : mScopeName    ( traceScope.GetScopeName() )
    , mScopeId      ( traceScope.mScopeId )
    , mScopePrio    ( traceScope.mScopePrio )
{
    if ( IS_ENABLED_SCOPE(mScopePrio) )
    {
        CELogMessage msg(NETrace::LogScopeEnter, mScopeId, NETrace::PrioScope, mScopeName);
        CETraceManager::SendLogMessage(msg);
    }
}


CETraceMessage::~CETraceMessage( void )
{
    if ( IS_ENABLED_SCOPE(mScopePrio) )
    {
        CELogMessage msg(NETrace::LogScopeExit, mScopeId, NETrace::PrioScope, mScopeName);
        CETraceManager::SendLogMessage(msg);
    }
}

void CETraceMessage::DebugLog( const char * format, ... ) const
{
    if ( IS_ENABLED_DEBUG(mScopePrio) )
    {
        va_list args;
        va_start(args, format);
        CETraceMessage::sendLog(mScopeId, NETrace::PrioDebug, format, args);
        va_end(args);
    }
}

void CETraceMessage::InfoLog( const char * format, ... ) const
{
    if ( IS_ENABLED_INFO(mScopePrio) )
    {
        va_list args;
        va_start(args, format);
        CETraceMessage::sendLog(mScopeId, NETrace::PrioInfo, format, args);
        va_end(args);
    }
}

void CETraceMessage::WarningLog(const char * format, ...) const
{
    if ( IS_ENABLED_WARNING(mScopePrio) )
    {
        va_list args;
        va_start(args, format);
        CETraceMessage::sendLog(mScopeId, NETrace::PrioWarning, format, args);
        va_end(args);
    }
}

void CETraceMessage::ErrorLog( const char * format, ... ) const
{
    if ( IS_ENABLED_ERROR(mScopePrio) )
    {
        va_list args;
        va_start(args, format);
        CETraceMessage::sendLog(mScopeId, NETrace::PrioError, format, args);
        va_end(args);
    }
}

void CETraceMessage::FatalLog( const char * format, ... ) const
{
    if ( IS_ENABLED_FATAL(mScopePrio) )
    {
        va_list args;
        va_start(args, format);
        CETraceMessage::sendLog(mScopeId, NETrace::PrioFatal, format, args);
        va_end(args);
    }
}

void CETraceMessage::LogTrace( NETrace::eLogPriority logPrio, const char * format, ... ) 
{
    va_list args;
    va_start(args, format);
    CETraceMessage::sendLog(NETrace::TRACE_SCOPE_ID_NONE, logPrio, format, args);
    va_end(args);
}

inline void CETraceMessage::sendLog( unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * format, va_list args )
{
    CELogMessage logData(NETrace::LogMessage, scopeId, msgPrio, NULL);
    CEString::PrintStringList( logData.lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, format, args );
    CETraceManager::SendLogMessage( logData );
}
