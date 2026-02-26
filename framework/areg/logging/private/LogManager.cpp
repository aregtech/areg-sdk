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

#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogMessage.hpp"

#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////
// LogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LogManager static methods
//////////////////////////////////////////////////////////////////////////

LogManager & LogManager::getInstance()
{
    static LogManager	_theLogManager;
    return _theLogManager;
}

void LogManager::logMessage(const areg::LogEntry& logData )
{
    LogManager::getInstance().sendLogEvent( LoggingEventData(LoggingEventData::LogAction::LogMessage, logData) );
}

void LogManager::logMessage(const areg::SharedBuffer& logData)
{
    LogManager::getInstance().sendLogEvent(LoggingEventData(LoggingEventData::LogAction::LogMessage, logData));
}

void LogManager::logMessage(const areg::RemoteMessage& logData)
{
    LogManager::getInstance().sendLogEvent( LoggingEventData(LoggingEventData::LogAction::LogMessage, logData) );
}

void LogManager::sendCommandMessage(LoggingEventData::LogAction cmd, const areg::SharedBuffer& data)
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

void LogManager::updateScopeConfiguration()
{
    LogManager& logManager = LogManager::getInstance();
    Lock lock(logManager.mLock);

    LogConfiguration config;
    config.updateScopeConfiguration(logManager.mScopeController);
}

bool LogManager::isLoggingEnabled()
{
    return LogManager::getInstance().mLogConfig.isLoggingEnabled();
}

bool LogManager::isLoggingConfigured()
{
    return Application::isConfigured();
}

bool LogManager::forceActivateLogging()
{
    bool result = false;
    LogManager & logManager = LogManager::getInstance();
    if ( logManager.isLoggingStarted() == false )
    {
        Lock lock( logManager.mLock );
        logManager.mLogConfig.setStatus(true);
        logManager.mLogConfig.setLogEnabled(areg::LogTarget::File, true);
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

bool LogManager::setScopePriority( const char * scopeName, uint32_t newPrio )
{
    ScopeController & ctrScope = LogManager::getInstance( ).mScopeController;
    uint32_t scopeId = areg::makeScopeId( scopeName );
    const areg::LogScope * scope = ctrScope.getScope( scopeId );
    bool result{ scope != nullptr };
    if ( result && (scope->getPriority() != newPrio))
    {
        ctrScope.setScopePriority( scopeId, newPrio );
    }

    return result;
}

void LogManager::updateScopes(const areg::String & scopeName, uint32_t scopeId, uint32_t newPrio)
{
    ScopeController & ctrScope = LogManager::getInstance().mScopeController;
    ctrScope.clearConfigScopes();
    ctrScope.changeScopeActivityStatus(scopeName, scopeId, newPrio);
}

uint32_t LogManager::getScopePriority( const char * scopeName )
{
    ScopeController & ctrScope = LogManager::getInstance( ).mScopeController;
    uint32_t scopeId = areg::makeScopeId( scopeName );
    const areg::LogScope * scope = ctrScope.getScope( scopeId );
    return (scope != nullptr ? scope->getPriority() : static_cast<uint32_t>(areg::LogPriority::PrioInvalid));
}

void LogManager::setLogDatabaseEngine(LogDatabaseEngine * dbEngine)
{
    LogManager::getInstance().mLoggerDatabase.setDatabaseEngine(dbEngine);
}

bool LogManager::isLogDabaseEngineInitialized()
{
    return LogManager::getInstance().mLoggerDatabase.isValid();
}

bool LogManager::isLogDatabaseEnabled()
{
    return LogManager::getInstance().mLogConfig.getDatabaseEnable();
}

void LogManager::forceEnableLogging()
{
    LogManager& logManager = LogManager::getInstance();
    logManager.mLogConfig.setStatus(true);
    logManager.mLogConfig.setLogEnabled(areg::LogTarget::File, true);
}

//////////////////////////////////////////////////////////////////////////
// LogManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
LogManager::LogManager()
    : areg::DispatcherThread      ( LogManager::LOGGING_THREAD_NAME.data(), areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM )
    , LoggingEventConsumer  ( )

    , mScopeController  ( )
	, mIsStarted		( false )
    , mLogConfig        ( )

    , mLoggerFile       ( mLogConfig )
    , mLoggerDebug      ( mLogConfig )
    , mLoggerTcp        ( mLogConfig, mScopeController, static_cast<areg::DispatcherThread &>(self()) )
    , mLoggerDatabase   ( mLogConfig )
    , mEventProcessor   ( self() )

    , mLogStarted       ( false, false )
    , mLock             ( )
{
}

//////////////////////////////////////////////////////////////////////////
// LogManager class methods
//////////////////////////////////////////////////////////////////////////
void LogManager::clearConfigData()
{
    Lock lock(mLock);
    mScopeController.clearConfigScopes( );
}

void LogManager::resetScopes()
{
    Lock lock(mLock);
    mScopeController.resetScopes();
}

bool LogManager::isRemoteLoggingEnabled() const
{
    return mLogConfig.isRemoteLoggingEnabled();
}

bool LogManager::isDatabaseLoggingEnabled() const
{
    return mLogConfig.isDatabaseLoggingEnabled();
}

bool LogManager::isFileLoggingEnabled() const
{
    return mLogConfig.isFileLoggingEnabled();
}

bool LogManager::isDebugOutputLoggingEnabled() const
{
    return mLogConfig.isDebugOutputLoggingEnabled();
}

bool LogManager::startLoggingThread()
{
    ASSERT((isRunning() == false) && (isReady() == false));
    mLogStarted.resetEvent( );
    if ( createThread(areg::WAIT_INFINITE) )
    {
        if ( waitForDispatcherStart(areg::WAIT_INFINITE) )
        {
            sendLogEvent( LoggingEventData(LoggingEventData::LogAction::StartLogs) );
            mLogStarted.lock( areg::WAIT_INFINITE );
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
    sendLogEvent( LoggingEventData(LoggingEventData::LogAction::StopLogs) );
    mIsStarted = false;

    if (waitComplete)
    {
        completionWait(areg::WAIT_INFINITE);
        shutdownThread(areg::DO_NOT_WAIT);
    }
}

void LogManager::waitLoggingThreadEnd()
{
    mIsStarted = false;
    completionWait(areg::WAIT_INFINITE);
    shutdownThread(areg::DO_NOT_WAIT);
}

void LogManager::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        LoggingEvent::addListener( static_cast<LoggingEventConsumer &>(self( )), static_cast<areg::DispatcherThread &>(self( )) );
        areg::DispatcherThread::readyForEvents( true );
    }
    else
    {
        areg::DispatcherThread::readyForEvents( false );
        LoggingEvent::removeListener( static_cast<LoggingEventConsumer &>(self( )), static_cast<areg::DispatcherThread &>(self( )) );

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

void LogManager::startLogs()
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

void LogManager::stopLogs()
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

void LogManager::writeLogMessage( const areg::LogEntry & logMessage )
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

bool LogManager::postEvent(areg::Event & eventElem)
{
    return areg::EventDispatcher::postEvent(eventElem);
}

inline void LogManager::sendLogEvent( const LoggingEventData & data, areg::Event::EventPriority eventPrio /*= Event::EventPriority::NormalPrio*/ )
{
    LoggingEvent::sendEvent( data, static_cast<LoggingEventConsumer &>(self( )), static_cast<areg::DispatcherThread &>(self( )), eventPrio );
}

void LogManager::changeScopePriority( const areg::String & scopeName, uint32_t scopeId, uint32_t scopePrio )
{
    mScopeController.changeScopeActivityStatus( scopeName, scopeId, scopePrio );
}

#endif  // AREG_LOGS
