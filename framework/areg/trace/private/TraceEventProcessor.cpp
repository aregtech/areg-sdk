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
    case TraceEventData::eTraceAction::TraceConfigure:
        _traceConfigure( stream );
        break;

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

    case TraceEventData::eTraceAction::TraceNetConnectService:
        _traceNetConnectService( );
        break;

    case TraceEventData::eTraceAction::TraceNetDisconnectService:
        _traceNetDisconnectService( );
        break;

    case TraceEventData::eTraceAction::TraceNetConnectionLost:
        _traceNetConnectionLost( );
        break;

    case TraceEventData::eTraceAction::TraceNetRegisterScopes:
        _traceNetRegisterScopes( );
        break;

    case TraceEventData::eTraceAction::TraceNetReceivedData:
        _traceNetReceivedData( stream );
        break;

    default:
        break; // ignore, do nothing
    }
}

inline void TraceEventProcessor::_traceConfigure( const SharedBuffer & data )
{
    FileBuffer file( data, "TraceConfigure" );
    mTraceManager.mLogConfig.loadConfig( file );
}

inline void TraceEventProcessor::_traceStartLogs( void )
{
    mTraceManager.traceStartLogs( );
}

inline void TraceEventProcessor::_traceStopLogs( void )
{
    SharedBuffer msg{ NETrace::messageDisconnectLogService( ) };
    mTraceManager.mLoggerTcp.sendData( msg );
    mTraceManager.traceStopLogs( );
}

inline void TraceEventProcessor::_traceSetEnableLogs( bool logsEnable )
{
    static_cast<TracePropertyValue &>(mTraceManager.mLogConfig.getStatus( ).getValue( )) = logsEnable;
}

inline void TraceEventProcessor::_traceSaveScopes( void )
{
    mTraceManager.mLogConfig.saveConfig( );
}

inline void TraceEventProcessor::_traceLogMessage( const SharedBuffer & data )
{
    const NETrace::sLogMessage * logMessage = reinterpret_cast<const NETrace::sLogMessage *>(data.getBuffer( ));
    ASSERT( logMessage != nullptr );
    mTraceManager.writeLogMessage( *logMessage );
}

inline void TraceEventProcessor::_traceNetConnectService( void )
{
    mTraceManager.sendLogData( NETrace::messageConnectLogService( ) );
}

inline void TraceEventProcessor::_traceNetDisconnectService( void )
{
    SharedBuffer msg{  };
    mTraceManager.sendLogData( NETrace::messageDisconnectLogService( ) );
    mTraceManager.disconnectTcpLogService( );
}

inline void TraceEventProcessor::_traceNetConnectionLost( void )
{
    mTraceManager.reconnectTcpLogService( );
}

inline void TraceEventProcessor::_traceNetRegisterScopes( void )
{
    using TraceScopeMap = TEHashMap<unsigned int, TraceScope *>;
    using POS = TraceScopeMap::MAPPOS;
    constexpr unsigned int maxScopes{ 500 };

    const SharedBuffer msgStart{ NETrace::messageRegisterScopesStart( ) };
    const SharedBuffer msgEnd{ NETrace::messageRegisterScopesEnd( ) };

    if ( mTraceManager.sendLogData( msgStart ) == false )
        return;

    const TraceScopeMap & mapScopes = mTraceManager.getScopeList();

    POS end = mapScopes.invalidPosition( );
    POS pos = mapScopes.firstPosition( );

    SharedBuffer msgScopes;

    do
    {
        msgScopes = NETrace::messageRegisterScopes( mapScopes, pos, maxScopes );
        if ( mTraceManager.sendLogData( msgScopes ) == false )
            return;

    } while ( pos != end );

    mTraceManager.sendLogData( msgEnd );
}

inline bool TraceEventProcessor::_traceNetRegisterScopesStart( void )
{
    // sends empty list and action 'set' to reset list on service side.
    return mTraceManager.sendLogData( NETrace::messageRegisterScopesStart( ) );
}

bool TraceEventProcessor::_traceNetRegisterScopesEnd( void )
{
    // sends empty list and action 'no action' to complete appending scopes.
    return mTraceManager.sendLogData( NETrace::messageRegisterScopesEnd( ) );
}

void TraceEventProcessor::_traceNetReceivedData( const SharedBuffer & stream )
{
    const NETrace::sLogHeader * cmdHeader = reinterpret_cast<const NETrace::sLogHeader *>(stream.getBuffer( ));
    const NETrace::sLogCommandData * cmdData = reinterpret_cast<const NETrace::sLogCommandData *>(stream.getBuffer( ) + sizeof( NETrace::sLogHeader ));
    const ITEM_ID cookie{ mTraceManager.mCookie };

    ASSERT( cmdHeader != nullptr );
    ASSERT( cmdData != nullptr );

    if ( cmdHeader->hdrCookieHost == NETrace::COOKIE_SERVICE )
    {
        if ( (cmdHeader->hdrCookieTarget == cookie) || (cookie == NETrace::COOKIE_ANY && cmdData->dataCommand == NETrace::eLogCommands::LogResponseConnect) )
        {
            // Past all checkups to make sure the correctness of the target.
            if ( cmdHeader->hdrLogType == NETrace::eLogType::LogResponse )
            {
                _traceNetResponseData( *cmdData, stream );
            }
            else if ( cmdHeader->hdrLogType == NETrace::eLogType::LogNotify )
            {
                _traceNetNotifyData( *cmdData, stream );
            }
        }
    }
}

void TraceEventProcessor::_traceNetResponseData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & stream )
{
    if ( cmdData.dataCommand == NETrace::eLogCommands::LogResponseConnect )
    {
        const NETrace::sLogResponseConnectData & respData = reinterpret_cast<const NETrace::sLogResponseConnectData &>(cmdData);
        mTraceManager.activateCookie( respData.dataCookie );
        _traceNetRegisterScopes( );
    }
    else if ( cmdData.dataCommand == NETrace::eLogCommands::LogResponseRegisterScopes )
    {
        const NETrace::sLogScopeInfo & scopeInfo = reinterpret_cast<const NETrace::sLogScopeInfo &>(cmdData);
        if ( (scopeInfo.dataBufferLen != 0) && (scopeInfo.dataScopeCount != 0) )
        {
            _changeScopePriority( stream, scopeInfo.dataScopeCount );
        }
    }
}

void TraceEventProcessor::_traceNetNotifyData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & stream )
{
    if ( cmdData.dataCommand == NETrace::eLogCommands::LogNotifyScopePriority )
    {
        const NETrace::sLogScopeInfo & scopeInfo = reinterpret_cast<const NETrace::sLogScopeInfo &>(cmdData);
        if ( (scopeInfo.dataBufferLen != 0) && (scopeInfo.dataScopeCount != 0) )
        {
            _changeScopePriority( stream, scopeInfo.dataScopeCount );
        }
    }
}

inline void TraceEventProcessor::_changeScopePriority( const SharedBuffer & stream, unsigned int scopeCount )
{
    stream.setPosition( sizeof( NETrace::sLogHeader ) + sizeof( NETrace::sLogScopeInfo ), IECursorPosition::eCursorPosition::PositionBegin );
    ASSERT( stream.isEndOfBuffer( ) == false );
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
