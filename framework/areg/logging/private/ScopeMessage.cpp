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
    : mScopeName( logScope.getScopeName() )
    , mScopeId  ( logScope.mScopeId       )
    , mSessionId( logScope.nextSession()  )
    , mTimestamp( areg::DateTime::getNow()      )
    , mScopePrio( logScope.mScopePrio     )
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ areg::LogMessageType::ScopeEnter, mScopeId, mSessionId, 0u, areg::LogPriority::PrioScope, mScopeName };
        LogManager::logMessage(msg);
    }
}

ScopeMessage::~ScopeMessage()
{
    if ( isScopeEnabled() )
    {
        LogMessage msg{ areg::LogMessageType::ScopeExit, mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioScope, mScopeName };
        LogManager::logMessage(msg);
    }
}

void ScopeMessage::logDebug( const char * format, ... ) const
{
    if ( isDbgEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioDebug, format, args);
        va_end(args);
    }
}

void ScopeMessage::logInfo( const char * format, ... ) const
{
    if ( isInfoEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioInfo, format, args);
        va_end(args);
    }
}

void ScopeMessage::logWarning(const char * format, ...) const
{
    if ( isWarnEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioWarning, format, args);
        va_end(args);
    }
}

void ScopeMessage::logError( const char * format, ... ) const
{
    if ( isErrEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioError, format, args);
        va_end(args);
    }
}

void ScopeMessage::logFatal( const char * format, ... ) const
{
    if ( isFatalEnabled() )
    {
        va_list args;
        va_start(args, format);
        ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, areg::LogPriority::PrioFatal, format, args);
        va_end(args);
    }
}

void ScopeMessage::logMessage(areg::LogPriority logPrio, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_sendLog(mScopeId, mSessionId, mTimestamp, logPrio, format, args);
    va_end(args);
}

void ScopeMessage::log( areg::LogPriority logPrio, const char * format, ... ) 
{
    va_list args;
    va_start(args, format);
    ScopeMessage::_sendLog(areg::LOG_SCOPE_ID_NONE, 0u, 0u, logPrio, format, args);
    va_end(args);
}

inline void ScopeMessage::_sendLog( uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * format, va_list args )
{
    LogMessage logData(areg::LogMessageType::MessageText, scopeId, sessionId, scopeStamp, msgPrio, nullptr, 0);
    logData.logMessageLen = static_cast<uint32_t>(areg::String::formatStringList( logData.logMessage, areg::LOG_MESSAGE_IZE, format, args ));
    LogManager::logMessage( logData );
}

#else   // AREG_LOGS

ScopeMessage::ScopeMessage(const LogScope& /*logScope*/)
{
}

ScopeMessage::~ScopeMessage()
{
}

#endif // AREG_LOGS
