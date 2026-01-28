/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Logging manager: container of scopes, configures,
 *              starts and stops logging.
 ************************************************************************/
#include "areg/logging/private/LogManager.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"

#include "areg/logging/IELogDatabaseEngine.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogMessage.hpp"

#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////
// LogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LogManager static methods
//////////////////////////////////////////////////////////////////////////

LogManager & LogManager::getInstance( void )
{
    static LogManager	_theLogManager;
    return _theLogManager;
}

void LogManager::logMessage(const NELogging::sLogMessage& logData )
{
    LogManager::getInstance().sendLogEvent( LoggingEventData(LoggingEventData::eLoggingAction::LoggingLogMessage, logData) );
}

void LogManager::logMessage(const SharedBuffer& logData)
{
    LogManager::getInstance().sendLogEvent(LoggingEventData(LoggingEventData::eLoggingAction::LoggingLogMessage, logData));
}

void LogManager::logMessage(const RemoteMessage& logData)
{
    LogManager::getInstance().sendLogEvent( LoggingEventData(LoggingEventData::eLoggingAction::LoggingLogMessage, logData) );
}

void LogManager::sendCommandMessage(LoggingEventData::eLoggingAction cmd, const SharedBuffer& data)
{
    LogManager::getInstance().sendLogEvent(LoggingEventData(cmd, data));
}

bool LogManager::readLogConfig( const char* configFile /*= nullptr*/ )
{
    return Application::loadConfiguration(configFile);
}

bool LogManager::startLogging(const char* configFile /*= nullptr*/ )
{
    Application::loadConfiguration(configFile);

    LogManager& logManager = LogManager::getInstance();
    Lock lock(logManager.mLock);
    if (logManager.isReady() == false)
    {
        lock.unlock();
        VERIFY(logManager.startLoggingThread());
        lock.lock();
    }

    return logManager.mIsStarted;
}

bool LogManager::saveLogConfig(const char* configFile /*= nullptr*/ )
{
    LogManager::updateScopeConfiguration();
    return Application::saveConfiguration(configFile);
}

void LogManager::updateScopeConfiguration(void)
{
    LogManager& logManager = LogManager::getInstance();
    Lock lock(logManager.mLock);

    LogConfiguration config;
    config.updateScopeConfiguration(logManager.mScopeController);
}

bool LogManager::isLoggingEnabled(void)
{
    return LogManager::getInstance().mLogConfig.isLoggingEnabled();
}

bool LogManager::isLoggingConfigured(void)
{
    return Application::isConfigured();
}

bool LogManager::forceActivateLogging(void)
{
    bool result = false;
    LogManager & logManager = LogManager::getInstance();
    if ( logManager.isLoggingStarted() == false )
    {
        Lock lock( logManager.mLock );
        logManager.mLogConfig.setStatus(true);
        logManager.mLogConfig.setLogEnabled(NELogging::eLogingTypes::LogTypeFile, true);
        logManager.mScopeController.activateDefaults( );
        result = logManager.startLoggingThread( );
    }

    return result;
}

void LogManager::setDefaultConfiguration(bool overwriteExisting)
{
    if (overwriteExisting || Application::isConfigured() == false)
    {
        Application::setupDefaultConfiguration();
    }
}

bool LogManager::setScopePriority( const char * scopeName, unsigned int newPrio )
{
    ScopeController & ctrScope = LogManager::getInstance( ).mScopeController;
    unsigned int scopeId = NELogging::makeScopeId( scopeName );
    const LogScope * scope = ctrScope.getScope( scopeId );
    bool result{ scope != nullptr };
    if ( result && (scope->getPriority() != newPrio))
    {
        ctrScope.setScopePriority( scopeId, newPrio );
    }

    return result;
}

void LogManager::updateScopes(const String & scopeName, unsigned int scopeId, unsigned int newPrio)
{
    ScopeController & ctrScope = LogManager::getInstance().mScopeController;
    ctrScope.clearConfigScopes();
    ctrScope.changeScopeActivityStatus(scopeName, scopeId, newPrio);
}

unsigned int LogManager::getScopePriority( const char * scopeName )
{
    ScopeController & ctrScope = LogManager::getInstance( ).mScopeController;
    unsigned int scopeId = NELogging::makeScopeId( scopeName );
    const LogScope * scope = ctrScope.getScope( scopeId );
    return (scope != nullptr ? scope->getPriority() : static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid));
}

void LogManager::setLogDatabaseEngine(IELogDatabaseEngine * dbEngine)
{
    LogManager::getInstance().mLoggerDatabase.setDatabaseEngine(dbEngine);
}

bool LogManager::isLogDabaseEngineInitialized(void)
{
    return LogManager::getInstance().mLoggerDatabase.isValid();
}

bool LogManager::isLogDatabaseEnabled(void)
{
    return LogManager::getInstance().mLogConfig.getDatabaseEnable();
}

void LogManager::forceEnableLogging(void)
{
    LogManager& logManager = LogManager::getInstance();
    logManager.mLogConfig.setStatus(true);
    logManager.mLogConfig.setLogEnabled(NELogging::eLogingTypes::LogTypeFile, true);
}

//////////////////////////////////////////////////////////////////////////
// LogManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
LogManager::LogManager(void)
    : DispatcherThread      ( LogManager::LOGGING_THREAD_NAME.data(), NECommon::STACK_SIZE_DEFAULT, NECommon::QUEUE_SIZE_MAXIMUM )
    , IELoggingEventConsumer  ( )

    , mScopeController  ( )
	, mIsStarted		( false )
    , mLogConfig        ( )

    , mLoggerFile       ( mLogConfig )
    , mLoggerDebug      ( mLogConfig )
    , mLoggerTcp        ( mLogConfig, mScopeController, static_cast<DispatcherThread &>(self()) )
    , mLoggerDatabase   ( mLogConfig )
    , mEventProcessor   ( self() )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
}

//////////////////////////////////////////////////////////////////////////
// LogManager class methods
//////////////////////////////////////////////////////////////////////////
void LogManager::clearConfigData( void )
{
    Lock lock(mLock);
    mScopeController.clearConfigScopes( );
}

void LogManager::resetScopes(void)
{
    Lock lock(mLock);
    mScopeController.resetScopes();
}

bool LogManager::isRemoteLoggingEnabled(void) const
{
    return mLogConfig.isRemoteLoggingEnabled();
}

bool LogManager::isDatabaseLoggingEnabled(void) const
{
    return mLogConfig.isDatabaseLoggingEnabled();
}

bool LogManager::isFileLoggingEnabled(void) const
{
    return mLogConfig.isFileLoggingEnabled();
}

bool LogManager::isDebugOutputLoggingEnabled(void) const
{
    return mLogConfig.isDebugOutputLoggingEnabled();
}

bool LogManager::startLoggingThread( void )
{
    ASSERT((isRunning() == false) && (isReady() == false));
    mLogStarted.resetEvent( );
    if ( createThread(NECommon::WAIT_INFINITE) )
    {
        if ( waitForDispatcherStart(NECommon::WAIT_INFINITE) )
        {
            sendLogEvent( LoggingEventData(LoggingEventData::eLoggingAction::LoggingStartLogs) );
            mLogStarted.lock( NECommon::WAIT_INFINITE );
        }
    }
#ifdef  DEBUG
    else
    {
        ASSERT( false );
    }
#endif  // DEBUG

    return mIsStarted;
}

void LogManager::stopLoggingThread(bool waitComplete)
{
    sendLogEvent( LoggingEventData(LoggingEventData::eLoggingAction::LoggingStopLogs) );
    mIsStarted = false;

    if (waitComplete)
    {
        completionWait(NECommon::WAIT_INFINITE);
        shutdownThread(NECommon::DO_NOT_WAIT);
    }
}

void LogManager::waitLoggingThreadEnd(void)
{
    mIsStarted = false;
    completionWait(NECommon::WAIT_INFINITE);
    shutdownThread(NECommon::DO_NOT_WAIT);
}

void LogManager::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        LoggingEvent::addListener( static_cast<IELoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
        DispatcherThread::readyForEvents( true );
    }
    else
    {
        DispatcherThread::readyForEvents( false );
        LoggingEvent::removeListener( static_cast<IELoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );

        // When we are here, all loggers should be already closed.
        ASSERT(mLoggerFile.isLoggerOpened() == false);
        ASSERT(mLoggerDebug.isLoggerOpened() == false);
        ASSERT(mLoggerTcp.isLoggerOpened() == false);
        ASSERT(mLoggerDatabase.isLoggerOpened() == false);
    }
}

void LogManager::processEvent( const LoggingEventData & data )
{
    mEventProcessor.processLogEvent( data.getLoggingAction( ), data.getReadableStream( ) );
}

void LogManager::startLogs( void )
{
    if ( mLogConfig.isLoggingEnabled() )
    {
        mScopeController.configureScopes();
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
            mLoggerTcp.openLogger();
        }

        if (mLoggerDatabase.isLoggerOpened() == false)
        {
            mLoggerDatabase.openLogger();
        }
    }

    mIsStarted = true;
    mLogStarted.setEvent( );
}

void LogManager::stopLogs(void)
{
    mScopeController.changeScopeActivityStatus( false );
    mLogStarted.resetEvent( );

    mIsStarted = false;

    mLoggerDebug.closeLogger( );
    mLoggerFile.closeLogger( );
    mLoggerTcp.closeLogger( );
    mLoggerDatabase.closeLogger();
    triggerExit( );
}

void LogManager::writeLogMessage( const NELogging::sLogMessage & logMessage )
{
    mLoggerFile.logMessage( logMessage );
    mLoggerDebug.logMessage( logMessage );
    mLoggerTcp.logMessage( logMessage );
    mLoggerDatabase.logMessage(logMessage);

    if ( hasMoreEvents() == false )
    {
        mLoggerFile.flushLogs();
        mLoggerDatabase.flushLogs();
    }
}

bool LogManager::postEvent(Event & eventElem)
{
    return EventDispatcher::postEvent(eventElem);
}

inline void LogManager::sendLogEvent( const LoggingEventData & data, Event::eEventPriority eventPrio /*= Event::eEventPriority::EventPriorityNormal*/ )
{
    LoggingEvent::sendEvent( data, static_cast<IELoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )), eventPrio );
}

void LogManager::changeScopePriority( const String & scopeName, unsigned int scopeId, unsigned int scopePrio )
{
    mScopeController.changeScopeActivityStatus( scopeName, scopeId, scopePrio );
}

#endif  // AREG_LOGS
