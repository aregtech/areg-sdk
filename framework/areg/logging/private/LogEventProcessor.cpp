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

#if AREG_LOGGING

namespace areg {

LogEventProcessor::LogEventProcessor( LogManager & logManager )
    : mLogManager (logManager)
{
}

void LogEventProcessor::process_log_event( const LoggingEventData & data )
{
    switch ( data.logging_action() )
    {
    case LoggingEventData::LogAction::StartLogs:
        _logging_start_logs( );
        break;

    case LoggingEventData::LogAction::StopLogs:
        _logging_stop_logs( );
        break;

    case LoggingEventData::LogAction::EnableLogs:
        _set_logging_enabled( true );
        break;

    case LoggingEventData::LogAction::DisableLogs:
        _set_logging_enabled( false );
        break;

    case LoggingEventData::LogAction::SaveScopes:
        _logging_save_scopes( );
        break;

    case LoggingEventData::LogAction::LogMessage:
        _logging_log_message( data );
        break;

    case LoggingEventData::LogAction::UpdateScopes:   // fall through
    case LoggingEventData::LogAction::QueryScopes:    // fall through
    case LoggingEventData::LogAction::Undefined:      // fall through
    default:
        break; // ignore, do nothing
    }
}

inline void LogEventProcessor::_logging_start_logs()
{
    mLogManager.start_logs( );
}

inline void LogEventProcessor::_logging_stop_logs()
{
    mLogManager.stop_logs( );
}

inline void LogEventProcessor::_set_logging_enabled( bool logsEnable )
{
    mLogManager.mLogConfig.set_tcp_enable(logsEnable);
}

inline void LogEventProcessor::_logging_save_scopes()
{
    mLogManager.mLogConfig.save_configuration( );
}

inline void LogEventProcessor::_logging_log_message( const LoggingEventData & data )
{
    mLogManager.write_log_message( data.entry() );
}

inline void LogEventProcessor::_change_scope_priority( const SharedBuffer & stream, uint32_t scopeCount )
{
    String scopeName{ };
    uint32_t scopeId{ };
    uint32_t scopePrio{ };

    for ( uint32_t i = 0; i < scopeCount; ++ i )
    {
        stream >> scopeName;
        stream >> scopeId;
        stream >> scopePrio;
        mLogManager.change_scope_priority( scopeName, scopeId, scopePrio );
    }
}

} // namespace areg

#endif  // AREG_LOGGING
