/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeMessage.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Scope messaging to make logging by priority.
 ************************************************************************/

#include "areg/logging/ScopeMessage.hpp"

#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogMessage.hpp"
#include "areg/logging/private/LoggingEvent.hpp"
#include "areg/logging/private/LogManager.hpp"

#include <stdarg.h>

#if AREG_LOGS

ScopeMessage::ScopeMessage( const LogScope & logScope )
    : mScopeName( logScope.scope_name() )
    , mScopeId  ( logScope.mScopeId       )
    , mSessionId( logScope.next_session()  )
    , mTimestamp( DateTime::now()      )
    , mScopePrio( logScope.mScopePrio     )
{
    if ( is_scope_enabled() )
    {
        LogMessage msg{ NELogging::LogMessageType::ScopeEnter, mScopeId, mSessionId, 0u, NELogging::LogPriority::PrioScope, mScopeName };
        LogManager::log_message(msg);
    }
}

ScopeMessage::~ScopeMessage()
{
    if ( is_scope_enabled() )
    {
        LogMessage msg{ NELogging::LogMessageType::ScopeExit, mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioScope, mScopeName };
        LogManager::log_message(msg);
    }
}

void ScopeMessage::log_debug( const char * format, ... ) const
{
    if ( is_dbg_enabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioDebug, format, args);
        va_end(args);
    }
}

void ScopeMessage::log_info( const char * format, ... ) const
{
    if ( is_info_enabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioInfo, format, args);
        va_end(args);
    }
}

void ScopeMessage::log_warning(const char * format, ...) const
{
    if ( is_warn_enabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioWarning, format, args);
        va_end(args);
    }
}

void ScopeMessage::log_error( const char * format, ... ) const
{
    if ( is_err_enabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioError, format, args);
        va_end(args);
    }
}

void ScopeMessage::log_fatal( const char * format, ... ) const
{
    if ( is_fatal_enabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, NELogging::LogPriority::PrioFatal, format, args);
        va_end(args);
    }
}

void ScopeMessage::log_message(NELogging::LogPriority logPrio, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_send_log(mScopeId, mSessionId, mTimestamp, logPrio, format, args);
    va_end(args);
}

void ScopeMessage::log( NELogging::LogPriority logPrio, const char * format, ... ) 
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_send_log(NELogging::LOG_SCOPE_ID_NONE, 0u, 0u, logPrio, format, args);
    va_end(args);
}

inline void ScopeMessage::_send_log( uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, NELogging::LogPriority msgPrio, const char * format, va_list args )
{
    LogMessage logData(NELogging::LogMessageType::MessageText, scopeId, sessionId, scopeStamp, msgPrio, nullptr, 0);
    logData.logMessageLen = static_cast<uint32_t>(String::format_string_list( logData.log_message, NELogging::LOG_MESSAGE_IZE, format, args ));
    LogManager::log_message( logData );
}

#else   // AREG_LOGS

ScopeMessage::ScopeMessage(const LogScope& /*logScope*/)
{
}

ScopeMessage::~ScopeMessage()
{
}

#endif // AREG_LOGS
