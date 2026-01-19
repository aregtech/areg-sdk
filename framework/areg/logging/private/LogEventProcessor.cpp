/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogEventProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, log manager. This is a helper class to process
 *              events received by log manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/LogEventProcessor.hpp"

#include "areg/base/FileBuffer.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeNodes.hpp"

#if AREG_LOGS

LogEventProcessor::LogEventProcessor( LogManager & logManager )
    : mLogManager (logManager)
{
}

void LogEventProcessor::processLogEvent( LoggingEventData::eLoggingAction cmdLog, const SharedBuffer & stream )
{
    stream.moveToBegin( );

    switch ( cmdLog )
    {
    case LoggingEventData::eLoggingAction::LoggingStartLogs:
        _loggingStartLogs( );
        break;

    case LoggingEventData::eLoggingAction::LoggingStopLogs:
        _loggingStopLogs( );
        break;

    case LoggingEventData::eLoggingAction::LoggingSetEnableLogs:
        _loggingSetEnableLogs( true );
        break;

    case LoggingEventData::eLoggingAction::LoggingSetDisableLogs:
        _loggingSetEnableLogs( false );
        break;

    case LoggingEventData::eLoggingAction::LoggingSaveScopes:
        _loggingSaveScopes( );
        break;

    case LoggingEventData::eLoggingAction::LoggingLogMessage:
        _loggingLogMessage( stream );
        break;

    case LoggingEventData::eLoggingAction::LoggingUpdateScopes:   // fall through
    case LoggingEventData::eLoggingAction::LoggingQueryScopes:    // fall through
    case LoggingEventData::eLoggingAction::LoggingUndefined:      // fall through
    default:
        break; // ignore, do nothing
    }
}

inline void LogEventProcessor::_loggingStartLogs( void )
{
    mLogManager.startLogs( );
}

inline void LogEventProcessor::_loggingStopLogs( void )
{
    mLogManager.stopLogs( );
}

inline void LogEventProcessor::_loggingSetEnableLogs( bool logsEnable )
{
    mLogManager.mLogConfig.setRemoteTcpEnable(logsEnable);
}

inline void LogEventProcessor::_loggingSaveScopes( void )
{
    mLogManager.mLogConfig.saveConfiguration( );
}

inline void LogEventProcessor::_loggingLogMessage( const SharedBuffer & data )
{
    const NELogging::sLogMessage * logMessage = reinterpret_cast<const NELogging::sLogMessage *>(data.getBuffer( ));
    ASSERT( logMessage != nullptr );
    mLogManager.writeLogMessage( *logMessage );
}

inline void LogEventProcessor::_changeScopePriority( const SharedBuffer & stream, unsigned int scopeCount )
{
    String scopeName{ };
    uint32_t scopeId{ };
    uint32_t scopePrio{ };

    for ( uint32_t i = 0; i < scopeCount; ++ i )
    {
        stream >> scopeName;
        stream >> scopeId;
        stream >> scopePrio;
        mLogManager.changeScopePriority( scopeName, scopeId, scopePrio );
    }
}

#endif  // AREG_LOGS
