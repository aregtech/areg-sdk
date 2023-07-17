/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace manager. The container of scopes, configuring
 *              tracer, starts and stops tracing.
 ************************************************************************/
#include "areg/trace/private/TraceManager.hpp"

#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/LogMessage.hpp"
#include "areg/trace/private/TraceEvent.hpp"
#include "areg/trace/private/TraceProperty.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/Process.hpp"

//////////////////////////////////////////////////////////////////////////
// TraceManager::TraceScopeMap class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class static members
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TraceManager class static methods
//////////////////////////////////////////////////////////////////////////

TraceManager & TraceManager::getInstance( void )
{
    static TraceManager	_theTraceManager;
    return _theTraceManager;
}

void TraceManager::sendLogMessage( LogMessage & logData )
{
    TraceManager & tracer = TraceManager::getInstance();
    tracer._sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceLogMessage, logData) );
}

bool TraceManager::startLogging( const char * configFile /*= nullptr*/, unsigned int waitTimeout /*= NECommon::WAIT_INFINITE*/ )
{
    TraceManager & traceManager = TraceManager::getInstance();
    bool canStart = false;

    do
    {
        Lock lock(traceManager.mLock);
        if (TraceManager::isLoggingConfigured() == false)
        {
        	if (traceManager.isReady( ) == false)
        	{
                canStart = traceManager.loadConfiguration( configFile);
            }
        }
        else
        {
        	canStart = traceManager.isReady( ) == false;
        }

    } while (false);

    return (canStart ? traceManager.startLoggingThread( waitTimeout ) : traceManager.mIsStarted);
}

bool TraceManager::isLoggingConfigured(void)
{
    bool result{ false };
    TraceManager & traceManager = TraceManager::getInstance();
    do 
    {
        Lock lock(traceManager.mLock);
        result = traceManager.mLogConfig.isConfigured();
    } while (false);

    return result;
}

bool TraceManager::isLoggingEnabled(void)
{
    TraceManager & traceManager = TraceManager::getInstance();
    Lock lock(traceManager.mLock);
    return traceManager.mLogConfig.isLoggingEnabled();
}

bool TraceManager::isNetLoggingEnabled(void)
{
    TraceManager& traceManager = TraceManager::getInstance();
    Lock lock(traceManager.mLock);
    return traceManager.mLogConfig.isNetLoggingEnabled();
}

const String& TraceManager::getConfigFile(void)
{
    TraceManager& traceManager = TraceManager::getInstance();
    Lock lock(traceManager.mLock);
    return traceManager.mLogConfig.getConfigFile();
}

bool TraceManager::forceActivateLogging(void)
{
    bool result = false;
    TraceManager & traceManager = TraceManager::getInstance();
    if ( (traceManager.isLoggingConfigured() == false) && (traceManager.isLoggingStarted() == false) )
    {
        Lock lock( traceManager.mLock );

        traceManager.mLogConfig.setDefaultValues( );
        traceManager.mScopeController.activateDefaults( );
        result = traceManager.startLoggingThread( );
    }

    return result;
}

void TraceManager::netConnectionLost( void )
{
    TraceManager & traceManager = TraceManager::getInstance( );
    traceManager.mCookie = NETrace::COOKIE_LOCAL;
    traceManager.mLoggerTcp.closeConnection( );
    traceManager._sendLogEvent(TraceEventData( TraceEventData::eTraceAction::TraceNetConnectionLost), Event::eEventPriority::EventPriorityHigh);
}

void TraceManager::netReceivedData( const SharedBuffer & data )
{
    TraceManager::getInstance( )._sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetReceivedData, data ), Event::eEventPriority::EventPriorityHigh );
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
TraceManager::TraceManager(void)
    : DispatcherThread      ( TraceManager::TRACER_THREAD_NAME.data() )
    , IETraceEventConsumer  ( )
    , IETimerConsumer       ( )

    , mScopeController  ( )
	, mIsStarted		( false )
    , mLogConfig        ( mScopeController )

    , mModuleId         ( Process::getInstance().getId() )
    , mCookie           ( NETrace::COOKIE_LOCAL )

    , mLoggerFile       ( mLogConfig )
    , mLoggerDebug      ( mLogConfig )
    , mLoggerTcp        ( mLogConfig )
    , mTimerReconnect   ( static_cast<IETimerConsumer &>(self()) )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
}

//////////////////////////////////////////////////////////////////////////
// TraceManager class methods
//////////////////////////////////////////////////////////////////////////
bool TraceManager::loadConfiguration( const char * filePath /*= nullptr */ )
{
    Lock lock(mLock);
    return mLogConfig.loadConfig( filePath );
}

void TraceManager::clearConfigData( void )
{
    Lock lock(mLock);

    mLogConfig.clearProperties();
    mScopeController.clearConfigScopes( );
}

bool TraceManager::isNetConfigValid(void) const
{
    return (mLogConfig.getRemoteTcpHost().isValid() && mLogConfig.getRemoteTcpPort().isValid());
}

bool TraceManager::isDatabaseValid(void) const
{
    return (mLogConfig.getDatabaseHost().isValid() && mLogConfig.getDatabaseName().isValid() && mLogConfig.getDatabaseDriver().isValid() );
}

bool TraceManager::isFileValid(void) const
{
    return mLogConfig.getLogFile().isValid();
}

bool TraceManager::isDebugOutputValid(void) const
{
    return mLogConfig.getDebugOutput().isValid();
}

bool TraceManager::startLoggingThread( unsigned int waitTimeout /* = NECommon::WAIT_INFINITE */ )
{
    bool result = false;
    ASSERT((isRunning() == false) && (isReady() == false));
    if ( createThread(NECommon::WAIT_INFINITE) )
    {
        // SetScopesActivity(true);
        if ( waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            mLogStarted.resetEvent();

            _sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStartLogs) );

            result = mLogStarted.lock(waitTimeout);
        }
    }
    else
    {
        OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", TraceManager::TRACER_THREAD_NAME);
    }

    return result;
}

void TraceManager::stopLoggingThread( unsigned int waitTimeout /* = NECommon::WAIT_INFINITE */ )
{
    // RemoveAllEvents(false);
    _sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStopLogs) );
    completionWait( isRunning() ? waitTimeout : NECommon::DO_NOT_WAIT );
}

void TraceManager::processEvent(const TraceEventData & data)
{
    const SharedBuffer & stream = data.getReadableStream();
    stream.moveToBegin();

    switch ( data.getTraceAction() )
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

void TraceManager::processTimer( Timer & timer )
{
    if ( &timer == &mTimerReconnect )
    {
        mTimerReconnect.stopTimer( );
        if ( (mLoggerTcp.isLoggerOpened( ) == false) && isNetLoggingEnabled( ))
        {
            if ( mLoggerTcp.openLogger( ) )
            {
                mCookie = NETrace::COOKIE_ANY;
                _sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetConnectService ), Event::eEventPriority::EventPriorityHigh );
            }
            else
            {
                mTimerReconnect.startTimer( LOG_RECONNECT_TIMEOUT, 1 );
            }
        }
    }
}

void TraceManager::processEvent( const TimerEventData & data )
{
    IETimerConsumer::processEvent( data );
}

bool TraceManager::runDispatcher(void)
{
    TraceEvent::addListener(static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));
    bool result = DispatcherThread::runDispatcher();

    mLoggerFile.closeLogger();
    mLoggerDebug.closeLogger();
    mLoggerTcp.closeLogger();

    TraceEvent::removeListener(static_cast<IETraceEventConsumer &>(self()), static_cast<DispatcherThread &>(self()));

    return result;
}

void TraceManager::traceStartLogs( void )
{
    if ( mLogConfig.isLoggingEnabled() )
    {
        mScopeController.changeScopeActivityStatus( true );
        if (mLoggerFile.isLoggerOpened() == false)
        {
            mLoggerFile.openLogger();
        }

#if defined(OUTPUT_DEBUG)
        if (mLoggerDebug.isLoggerOpened() == false)
        {
            mLoggerDebug.openLogger();
        }
#endif // !defined(OUTPUT_DEBUG)

        if (mLoggerTcp.isLoggerOpened() == false)
        {
            if ( isNetLoggingEnabled( ) )
            {
                if ( mLoggerTcp.openLogger( ) == false)
                {
                    mCookie = NETrace::COOKIE_ANY;
                    mTimerReconnect.startTimer( LOG_RECONNECT_TIMEOUT, 1 );
                }
                else if (mLoggerTcp.isActive() == false )
                {
                    _sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetConnectService ), Event::eEventPriority::EventPriorityHigh );
                }
                else
                {
                    ; // do nothing, just ignore.
                }
            }
        }

        mIsStarted = true;
    }
}

void TraceManager::traceStopLogs(void)
{
    _traceNetDisconnectService( );
    mScopeController.changeScopeActivityStatus( false );
}

void TraceManager::dataMessage( const LogMessage & logMessage )
{
    mLoggerFile.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerDebug.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerTcp.logMessage(static_cast<const NETrace::sLogMessage&>(logMessage));

    if ( hasMoreEvents() == false )
    {
        mLoggerFile.flushLogs();
    }
}

bool TraceManager::postEvent(Event & eventElem)
{
    bool result = false;
    if (RUNTIME_CAST(&eventElem, TraceEvent) != nullptr)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Not a TraceEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName().getString());
        eventElem.destroy();
    }
    return result;
}

inline void TraceManager::_sendLogEvent( const TraceEventData & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    TraceEvent::sendEvent( data, static_cast<IETraceEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )), eventPrio );
}

inline void TraceManager::_reconnectTcpLogService( void )
{
    ASSERT( mLogConfig.isNetLoggingEnabled() );
    mCookie = NETrace::COOKIE_LOCAL;
    mLoggerTcp.closeConnection( );
    mTimerReconnect.startTimer( LOG_RECONNECT_TIMEOUT, static_cast<DispatcherThread &>(self( )), 1 );
}

inline void TraceManager::_traceConfigure( const SharedBuffer & data )
{
    FileBuffer file( data, "TraceConfigure" );
    mLogConfig.loadConfig( file );
}

inline void TraceManager::_traceStartLogs( void )
{
    traceStartLogs( );
    mLogStarted.setEvent( );
}

inline void TraceManager::_traceStopLogs( void )
{
    mTimerReconnect.stopTimer( );
    traceStopLogs( );
    triggerExitEvent( );
    mLogStarted.resetEvent( );
}

inline void TraceManager::_traceSetEnableLogs( bool logsEnable )
{
    static_cast<TracePropertyValue &>(mLogConfig.getStatus( ).getValue( )) = logsEnable;
}

inline void TraceManager::_traceSaveScopes( void )
{
    // not implemented yet
}

inline void TraceManager::_traceLogMessage( const SharedBuffer & data )
{
    LogMessage logMessage( static_cast<const IEInStream &>(data) );
    mLoggerFile.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerDebug.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );
    mLoggerTcp.logMessage( static_cast<const NETrace::sLogMessage &>(logMessage) );

    if ( hasMoreEvents( ) == false )
    {
        mLoggerFile.flushLogs( );
    }
}

inline void TraceManager::_traceNetConnectService( void )
{
    const NETrace::sLogRequestConnect reqConnect;
    SharedBuffer data;
    data << reqConnect;
    if ( mLoggerTcp.sendData( data ) == false )
    {
        _reconnectTcpLogService( );
    }
}

inline void TraceManager::_traceNetDisconnectService( void )
{
    mTimerReconnect.stopTimer( );

    if ( mLoggerTcp.isLoggerOpened( ) )
    {
        const NETrace::sLogRequestDisconnect reqDisconnect;
        SharedBuffer data;
        data << reqDisconnect;
        mLoggerTcp.sendData( data );
        mLoggerTcp.closeLogger( );
    }

    mCookie = NETrace::COOKIE_LOCAL;
}

inline void TraceManager::_traceNetConnectionLost( void )
{
    _reconnectTcpLogService( );
}

inline void TraceManager::_traceNetRegisterScopes( void )
{
    if ( mLoggerTcp.isLoggerOpened( ) )
    {
        using POS = MapTraceScope::MAPPOS;

        if ( _traceNetRegisterScopesStart( ) == false )
        {
            _reconnectTcpLogService( );
            return; // do not send anymore
        }

        const TraceScopeMap & mapScopes = mScopeController.getScopeList( );
        POS end = mapScopes.invalidPosition( );
        POS pos = mapScopes.firstPosition( );

        NETrace::sLogRequestRegisterScopes regScopes;
        regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesAppend;
        while( pos != end )
        {
            constexpr int maxCount{ 500 };
            SharedBuffer data;
            data << regScopes.reqScopeHeader;
            data << regScopes.reqScopeInfo;

            int i{ 0 };
            for ( ; (i < maxCount) && pos != end; ++ i )
            {
                TraceScopePair tracePair;
                mapScopes.getAtPosition( pos, tracePair );
                ASSERT( tracePair.second != nullptr );
                data << (*tracePair.second);
                pos = mapScopes.nextPosition( pos );
            }

            int sizeHeader{ sizeof( NETrace::sLogHeader ) };
            int sizeInfo{ sizeof( NETrace::sLogScopeInfo ) };

            NETrace::sLogHeader * logHeader = reinterpret_cast<NETrace::sLogHeader *>(data.getBuffer( ));
            logHeader->hdrDataLen = data.getSizeUsed( ) - sizeHeader;
            NETrace::sLogScopeInfo * scopeInfo  = reinterpret_cast<NETrace::sLogScopeInfo *>(data.getBuffer( ) + sizeHeader);
            scopeInfo->dataBufferLen = data.getSizeUsed( ) - (sizeHeader + sizeInfo);
            scopeInfo->dataScopeCount = i;

            if ( mLoggerTcp.sendData( data ) == false )
            {
                _reconnectTcpLogService( );
                return; // do not send anymore
            }
        }

        if ( _traceNetRegisterScopesEnd( ) == false )
        {
            _reconnectTcpLogService( );
            return; // do not send anymore
        }
    }
}

bool TraceManager::_traceNetRegisterScopesStart( void )
{
    // sends empty list and action 'set' to reset list on service side.
    NETrace::sLogRequestRegisterScopes regScopes;
    regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesSet;
    SharedBuffer data;
    data << regScopes.reqScopeHeader;
    data << regScopes.reqScopeInfo;
    return mLoggerTcp.sendData( data );
}

bool TraceManager::_traceNetRegisterScopesEnd( void )
{
    // sends empty list and action 'no action' to complete appending scopes.
    NETrace::sLogRequestRegisterScopes regScopes;
    regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesNoAction;
    SharedBuffer data;
    data << regScopes.reqScopeHeader;
    data << regScopes.reqScopeInfo;
    return mLoggerTcp.sendData( data );
}

void TraceManager::_traceNetReceivedData( const SharedBuffer & stream )
{
    const NETrace::sLogHeader * cmdHeader = reinterpret_cast<const NETrace::sLogHeader *>(stream.getBuffer( ));
    const NETrace::sLogCommandData * cmdData = reinterpret_cast<const NETrace::sLogCommandData *>(stream.getBuffer( ) + sizeof( NETrace::sLogHeader ));
    ASSERT( cmdHeader != nullptr );
    ASSERT( cmdData != nullptr );

    if ( cmdHeader->hdrCookieHost == NETrace::COOKIE_SERVICE )
    {
        if ( (cmdHeader->hdrCookieTarget == mCookie) || (mCookie == NETrace::COOKIE_ANY && cmdData->dataCommand == NETrace::eLogCommands::LogResponseConnect) )
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

void TraceManager::_traceNetResponseData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & stream )
{
    if ( cmdData.dataCommand == NETrace::eLogCommands::LogResponseConnect )
    {
        const NETrace::sLogResponseConnectData & respData = reinterpret_cast<const NETrace::sLogResponseConnectData &>(cmdData);
        mCookie = respData.dataCookie;
        mLoggerTcp.setActive( true );
        _sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetRegisterScopes ), Event::eEventPriority::EventPriorityHigh );
    }
    else if ( cmdData.dataCommand == NETrace::eLogCommands::LogResponseRegisterScopes )
    {
        const NETrace::sLogScopeInfo & scopeInfo = reinterpret_cast<const NETrace::sLogScopeInfo &>(cmdData);
        if ( (scopeInfo.dataBufferLen != 0) && (scopeInfo.dataScopeCount != 0) )
        {
            stream.setPosition( sizeof( NETrace::sLogHeader ) + sizeof( NETrace::sLogScopeInfo ), IECursorPosition::eCursorPosition::PositionBegin );
            ASSERT( stream.isEndOfBuffer( ) == false );
            String scopeName{ };
            uint32_t scopeId{ };
            uint32_t scopePrio{ };

            for ( uint32_t i = 0; i < scopeInfo.dataScopeCount; ++ i )
            {
                stream >> scopeName;
                stream >> scopeId;
                stream >> scopePrio;

                // TraceScope * scope = mMapTraceScope.getAt( scopeId );
                // ASSERT( scope != nullptr );
                // ASSERT( scope->getScopeName( ) == scopeName );

                // TODO: move to separate function to be used for notification and response.
                // TODO: check whether it is a scope name or scope group. In case of group, ignore scopeId.
            }
        }
    }
}

void TraceManager::_traceNetNotifyData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & data )
{
}
