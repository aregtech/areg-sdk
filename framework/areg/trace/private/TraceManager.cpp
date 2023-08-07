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
    tracer.sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceLogMessage, logData) );
}

bool TraceManager::startLogging( const char * configFile /*= nullptr*/ )
{
    TraceManager & traceManager = TraceManager::getInstance();

    do
    {
        Lock lock(traceManager.mLock);
        if ( traceManager.isReady( ) == false )
        {
            if ( TraceManager::isLoggingConfigured( ) == false )
            {
                traceManager.loadConfiguration( configFile );
            }

            lock.unlock( );
            VERIFY( traceManager.startLoggingThread( ) );
            lock.lock( );
        }

    } while (false);

    return traceManager.mIsStarted;
}

bool TraceManager::saveLogConfig( const char * configFile )
{
    TraceManager & traceManager = TraceManager::getInstance( );
    if ( NEString::isEmpty( configFile ) )
    {
        Lock lock( traceManager.mLock );
        return traceManager.mLogConfig.saveConfig( );
    }
    else
    {
        Lock lock( traceManager.mLock );
        return traceManager.mLogConfig.saveConfig( configFile );
    }
}

bool TraceManager::saveLogConfig( FileBase & file )
{
    TraceManager & traceManager = TraceManager::getInstance( );
    Lock lock( traceManager.mLock );
    return traceManager.mLogConfig.saveConfig( file );
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

bool TraceManager::setScopePriority( const char * scopeName, unsigned int newPrio )
{
    ScopeController & ctrScope = TraceManager::getInstance( ).mScopeController;
    unsigned int scopeId = NETrace::makeScopeId( scopeName );
    const TraceScope * scope = ctrScope.getScope( scopeId );
    bool result{ scope != nullptr };
    if ( result && (scope->getPriority() != newPrio))
    {
        ctrScope.setScopePriority( scopeId, newPrio );
    }

    return result;
}

unsigned int TraceManager::getScopePriority( const char * scopeName )
{
    ScopeController & ctrScope = TraceManager::getInstance( ).mScopeController;
    unsigned int scopeId = NETrace::makeScopeId( scopeName );
    const TraceScope * scope = ctrScope.getScope( scopeId );
    return (scope != nullptr ? scope->getPriority() : static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid));
}

void TraceManager::netConnectionLost( void )
{
    TraceManager & traceManager = TraceManager::getInstance( );
    traceManager.mCookie = NETrace::COOKIE_LOCAL;
    traceManager.mLoggerTcp.closeConnection( );
    traceManager.sendLogEvent(TraceEventData( TraceEventData::eTraceAction::TraceNetConnectionLost), Event::eEventPriority::EventPriorityHigh);
}

void TraceManager::netReceivedData( const SharedBuffer & data )
{
    TraceManager::getInstance( ).sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetReceivedData, data ), Event::eEventPriority::EventPriorityHigh );
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
    , mEventProcessor   ( self() )

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

void TraceManager::unloadConfiguration( void )
{
    Lock lock( mLock );
    mScopeController.clearConfigScopes( );
    mLogConfig.unloadConfig( );
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

bool TraceManager::startLoggingThread( void )
{
    ASSERT((isRunning() == false) && (isReady() == false));
    mLogStarted.resetEvent( );
    if ( createThread(NECommon::WAIT_INFINITE) )
    {
        if ( waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStartLogs) );
            mLogStarted.lock( NECommon::WAIT_INFINITE );
        }
    }
    else
    {
        ASSERT( false );
        OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", TraceManager::TRACER_THREAD_NAME);
    }

    return mIsStarted;
}

void TraceManager::stopLoggingThread( void )
{
    // RemoveAllEvents(false);
    sendLogEvent( TraceEventData(TraceEventData::eTraceAction::TraceStopLogs) );
    completionWait( NECommon::WAIT_INFINITE );
    mIsStarted = false;
    destroyThread( NECommon::DO_NOT_WAIT );
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
                sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetConnectService ), Event::eEventPriority::EventPriorityHigh );
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

void TraceManager::processEvent( const TraceEventData & data )
{
    mEventProcessor.processLogEvent( data.getTraceAction( ), data.getReadableStream( ) );
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

        connectTcpLogService( );
    }

    mIsStarted = true;
    mLogStarted.setEvent( );
}

void TraceManager::traceStopLogs(void)
{
    mCookie = NETrace::COOKIE_LOCAL;
    mTimerReconnect.stopTimer( );
    mScopeController.changeScopeActivityStatus( false );
    mLogStarted.resetEvent( );

    mIsStarted = false;

    mLoggerDebug.closeLogger( );
    mLoggerFile.closeLogger( );
    mLoggerTcp.closeLogger( );
    triggerExitEvent( );
}

void TraceManager::writeLogMessage( const NETrace::sLogMessage & logMessage )
{
    mLoggerFile.logMessage( logMessage );
    mLoggerDebug.logMessage( logMessage );
    mLoggerTcp.logMessage( logMessage );

    if ( hasMoreEvents() == false )
    {
        mLoggerFile.flushLogs();
    }
}

bool TraceManager::sendLogData( const SharedBuffer & logData )
{
    bool result{ true };
    if ( mLoggerTcp.sendData( logData ) == false )
    {
        result = false;
        reconnectTcpLogService( );
    }

    return result;
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

inline void TraceManager::sendLogEvent( const TraceEventData & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    TraceEvent::sendEvent( data, static_cast<IETraceEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )), eventPrio );
}

void TraceManager::connectTcpLogService( void )
{
    if ( mLoggerTcp.isLoggerOpened( ) == false )
    {
        if ( isNetLoggingEnabled( ) )
        {
            if ( mLoggerTcp.openLogger( ) == false )
            {
                mCookie = NETrace::COOKIE_ANY;
                mTimerReconnect.startTimer( LOG_RECONNECT_TIMEOUT, 1 );
            }
            else if ( mLoggerTcp.isActive( ) == false )
            {
                sendLogEvent( TraceEventData( TraceEventData::eTraceAction::TraceNetConnectService ), Event::eEventPriority::EventPriorityHigh );
            }
            else
            {
                ; // do nothing, just ignore.
            }
        }
    }
}

void TraceManager::reconnectTcpLogService( void )
{
    ASSERT( mLogConfig.isNetLoggingEnabled() );
    mCookie = NETrace::COOKIE_LOCAL;
    mLoggerTcp.closeLogger( );
    mTimerReconnect.startTimer( LOG_RECONNECT_TIMEOUT, static_cast<DispatcherThread &>(self( )), 1 );
}

void TraceManager::disconnectTcpLogService( void )
{
    ASSERT( mLogConfig.isNetLoggingEnabled( ) );
    mCookie = NETrace::COOKIE_LOCAL;
    mLoggerTcp.closeLogger( );
}

void TraceManager::activateCookie( ITEM_ID newCookie )
{
    mCookie = newCookie;
    mLoggerTcp.setActive( true );
}

void TraceManager::changeScopePriority( const String & scopeName, unsigned int scopeId, unsigned int scopePrio )
{
    mScopeController.changeScopeActivityStatus( scopeName, scopeId, scopePrio );
}

