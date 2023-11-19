/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceEventProcessor.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace manager. This is a helper class to process
 *              events received by trace manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/TraceEventProcessor.hpp"

#include "areg/base/FileBuffer.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"
#include "areg/trace/private/ScopeNodes.hpp"

TraceEventProcessor::TraceEventProcessor( TraceManager & traceManager )
    : mTraceManager ( traceManager )
{
}

void TraceEventProcessor::processLogEvent( TraceEventData::eTraceAction cmdLog, const SharedBuffer & stream )
{
    stream.moveToBegin( );

    switch ( cmdLog )
    {
    case TraceEventData::eTraceAction::TraceStartLogs:
        _traceStartLogs( );
        break;

    case TraceEventData::eTraceAction::TraceStopLogs:
        _traceStopLogs( );
        break;

    case TraceEventData::eTraceAction::TraceSetEnableLogs:
        _traceSetEnableLogs( true );
        break;

    case TraceEventData::eTraceAction::TraceSetDisableLogs:
        _traceSetEnableLogs( false );
        break;

    case TraceEventData::eTraceAction::TraceSaveScopes:
        _traceSaveScopes( );
        break;

    case TraceEventData::eTraceAction::TraceLogMessage:
        _traceLogMessage( stream );
        break;

    default:
        break; // ignore, do nothing
    }
}

inline void TraceEventProcessor::_traceStartLogs( void )
{
    mTraceManager.traceStartLogs( );
}

inline void TraceEventProcessor::_traceStopLogs( void )
{
    mTraceManager.traceStopLogs( );
}

inline void TraceEventProcessor::_traceSetEnableLogs( bool logsEnable )
{
    mTraceManager.mLogConfig.setRemoteTcpEnable(logsEnable);
}

inline void TraceEventProcessor::_traceSaveScopes( void )
{
    mTraceManager.mLogConfig.saveConfiguration( );
}

inline void TraceEventProcessor::_traceLogMessage( const SharedBuffer & data )
{
    const NETrace::sLogMessage * logMessage = reinterpret_cast<const NETrace::sLogMessage *>(data.getBuffer( ));
    ASSERT( logMessage != nullptr );
    mTraceManager.writeLogMessage( *logMessage );
}

inline void TraceEventProcessor::_changeScopePriority( const SharedBuffer & stream, unsigned int scopeCount )
{
    String scopeName{ };
    uint32_t scopeId{ };
    uint32_t scopePrio{ };

    for ( uint32_t i = 0; i < scopeCount; ++ i )
    {
        stream >> scopeName;
        stream >> scopeId;
        stream >> scopePrio;
        mTraceManager.changeScopePriority( scopeName, scopeId, scopePrio );
    }
}
