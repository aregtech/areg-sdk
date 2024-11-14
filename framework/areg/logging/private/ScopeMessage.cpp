/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeMessage.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
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
    : mScopeName( logScope.getScopeName() )
    , mScopeId  ( logScope.mScopeId       )
    , mScopePrio( logScope.mScopePrio     )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ NELogging::eLogMessageType::LogMessageScopeEnter, mScopeId, NELogging::PrioScope, mScopeName };
        LogManager::logMessage(msg);
    }
}

ScopeMessage::~ScopeMessage( void )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ NELogging::eLogMessageType::LogMessageScopeExit, mScopeId, NELogging::PrioScope, mScopeName };
        LogManager::logMessage(msg);
    }
}

void ScopeMessage::logDebug( const char * format, ... ) const
{
    if ( isDbgEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, NELogging::PrioDebug, format, args);
        va_end(args);
    }
}

void ScopeMessage::logInfo( const char * format, ... ) const
{
    if ( isInfoEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, NELogging::PrioInfo, format, args);
        va_end(args);
    }
}

void ScopeMessage::logWarning(const char * format, ...) const
{
    if ( isWarnEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, NELogging::PrioWarning, format, args);
        va_end(args);
    }
}

void ScopeMessage::logError( const char * format, ... ) const
{
    if ( isErrEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, NELogging::PrioError, format, args);
        va_end(args);
    }
}

void ScopeMessage::logFatal( const char * format, ... ) const
{
    if ( isFatalEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, NELogging::PrioFatal, format, args);
        va_end(args);
    }
}

void ScopeMessage::logMessage(NELogging::eLogPriority logPrio, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_sendLog(mScopeId, logPrio, format, args);
    va_end(args);
}

void ScopeMessage::log( NELogging::eLogPriority logPrio, const char * format, ... ) 
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_sendLog(NELogging::LOG_SCOPE_ID_NONE, logPrio, format, args);
    va_end(args);
}

inline void ScopeMessage::_sendLog( unsigned int scopeId, NELogging::eLogPriority msgPrio, const char * format, va_list args )
{
    LogMessage logData(NELogging::eLogMessageType::LogMessageText, scopeId, msgPrio, nullptr, 0);
    logData.logMessageLen = static_cast<uint32_t>(String::formatStringList( logData.logMessage, NELogging::LOG_MESSAGE_IZE, format, args ));
    LogManager::logMessage( logData );
}

#else   // AREG_LOGS

ScopeMessage::ScopeMessage(const LogScope& /*logScope*/)
{
}

ScopeMessage::~ScopeMessage(void)
{
}

#endif // AREG_LOGS
