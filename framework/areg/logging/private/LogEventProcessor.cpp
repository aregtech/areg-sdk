/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogEventProcessor.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, log manager. This is a helper class to process
 *              events received by log manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/LogEventProcessor.hpp"

#include "areg/base/FileBuffer.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeNodes.hpp"

namespace areg
{

    #if AREG_LOGS

    LogEventProcessor::LogEventProcessor( LogManager & logManager )
        : mLogManager (logManager)
    {
    }

    void LogEventProcessor::processLogEvent( LoggingEventData::LogAction cmdLog, const SharedBuffer & stream )
    {
        stream.moveToBegin( );

        switch ( cmdLog )
        {
        case LoggingEventData::LogAction::StartLogs:
            _loggingStartLogs( );
            break;

        case LoggingEventData::LogAction::StopLogs:
            _loggingStopLogs( );
            break;

        case LoggingEventData::LogAction::EnableLogs:
            _loggingSetEnableLogs( true );
            break;

        case LoggingEventData::LogAction::DisableLogs:
            _loggingSetEnableLogs( false );
            break;

        case LoggingEventData::LogAction::SaveScopes:
            _loggingSaveScopes( );
            break;

        case LoggingEventData::LogAction::LogMessage:
            _loggingLogMessage( stream );
            break;

        case LoggingEventData::LogAction::UpdateScopes:   // fall through
        case LoggingEventData::LogAction::QueryScopes:    // fall through
        case LoggingEventData::LogAction::Undefined:      // fall through
        default:
            break; // ignore, do nothing
        }
    }

    inline void LogEventProcessor::_loggingStartLogs()
    {
        mLogManager.startLogs( );
    }

    inline void LogEventProcessor::_loggingStopLogs()
    {
        mLogManager.stopLogs( );
    }

    inline void LogEventProcessor::_loggingSetEnableLogs( bool logsEnable )
    {
        mLogManager.mLogConfig.setRemoteTcpEnable(logsEnable);
    }

    inline void LogEventProcessor::_loggingSaveScopes()
    {
        mLogManager.mLogConfig.saveConfiguration( );
    }

    inline void LogEventProcessor::_loggingLogMessage( const SharedBuffer & data )
    {
        const LogEntry * logMessage = reinterpret_cast<const LogEntry *>(data.getBuffer( ));
        ASSERT( logMessage != nullptr );
        mLogManager.writeLogMessage( *logMessage );
    }

    inline void LogEventProcessor::_changeScopePriority( const SharedBuffer & stream, uint32_t scopeCount )
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
} // namespace areg
