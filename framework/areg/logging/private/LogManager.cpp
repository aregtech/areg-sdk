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

#if AREG_LOGGING
namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LogManager static methods
//////////////////////////////////////////////////////////////////////////

LogManager & LogManager::instance()
{
    static LogManager	_theLogManager;
    return _theLogManager;
}

void LogManager::log_message(const areg::LogEntry& logData )
{
    LogManager::instance().send_log_event( LoggingEventData(LoggingEventData::LogAction::LogMessage, logData) );
}

void LogManager::log_message(const SharedBuffer& logData)
{
    LogManager::instance().send_log_event(LoggingEventData(LoggingEventData::LogAction::LogMessage, logData));
}

void LogManager::log_message(const RemoteMessage& logData)
{
    LogManager::instance().send_log_event( LoggingEventData(LoggingEventData::LogAction::LogMessage, logData) );
}

void LogManager::send_command_message(LoggingEventData::LogAction cmd, const SharedBuffer& data)
{
    LogManager::instance().send_log_event(LoggingEventData(cmd, data));
}

bool LogManager::read_log_config( const char* configFile /*= nullptr*/ )
{
    return Application::load_configuration(configFile);
}

bool LogManager::start_logging(const char* configFile /*= nullptr*/ )
{
    Application::load_configuration(configFile);

    LogManager& logManager = LogManager::instance();
    Lock lock(logManager.mLock);
    if (logManager.is_ready() == false)
    {
        lock.unlock();
        VERIFY(logManager.start_logging_thread());
        lock.lock();
    }

    return logManager.mIsStarted;
}

bool LogManager::save_log_config(const char* configFile /*= nullptr*/ )
{
    LogManager::update_scope_configuration();
    return Application::save_configuration(configFile);
}

void LogManager::update_scope_configuration()
{
    LogManager& logManager = LogManager::instance();
    Lock lock(logManager.mLock);

    LogConfiguration config;
    config.update_scope_configuration(logManager.mScopeController);
}

bool LogManager::is_logging_enabled()
{
    return LogManager::instance().mLogConfig.is_logging_enabled();
}

bool LogManager::is_logging_configured()
{
    return Application::is_configured();
}

bool LogManager::force_activate_logging()
{
    bool result = false;
    LogManager & logManager = LogManager::instance();
    if ( logManager.is_logging_started() == false )
    {
        Lock lock( logManager.mLock );
        logManager.mLogConfig.set_status(true);
        logManager.mLogConfig.set_log_enabled(areg::LogTarget::File, true);
        logManager.mScopeController.activate_defaults( );
        result = logManager.start_logging_thread( );
    }

    return result;
}

void LogManager::set_default_configuration(bool overwriteExisting)
{
    if (overwriteExisting || Application::is_configured() == false)
    {
        Application::setup_default_configuration();
    }
}

bool LogManager::set_scope_priority( const char * scopeName, uint32_t newPrio )
{
    ScopeController & ctrScope = LogManager::instance( ).mScopeController;
    uint32_t scopeId = areg::make_scope_id( scopeName );
    const LogScope * scope = ctrScope.scope( scopeId );
    bool result{ scope != nullptr };
    if ( result && (scope->priority() != newPrio))
    {
        ctrScope.set_scope_priority( scopeId, newPrio );
    }

    return result;
}

void LogManager::update_scopes(const String & scopeName, uint32_t scopeId, uint32_t newPrio)
{
    ScopeController & ctrScope = LogManager::instance().mScopeController;
    ctrScope.clear_config_scopes();
    ctrScope.set_scope_activity(scopeName, scopeId, newPrio);
}

uint32_t LogManager::scope_priority( const char * scopeName )
{
    ScopeController & ctrScope = LogManager::instance( ).mScopeController;
    uint32_t scopeId = areg::make_scope_id( scopeName );
    const LogScope * scope = ctrScope.scope( scopeId );
    return (scope != nullptr ? scope->priority() : static_cast<uint32_t>(areg::LogPriority::PrioInvalid));
}

void LogManager::set_db_engine(LogDatabaseEngine * dbEngine)
{
    LogManager::instance().mLoggerDatabase.set_database_engine(dbEngine);
}

bool LogManager::is_db_initialized()
{
    return LogManager::instance().mLoggerDatabase.is_valid();
}

bool LogManager::is_db_enabled()
{
    return LogManager::instance().mLogConfig.database_enable();
}

void LogManager::force_enable_logging()
{
    LogManager& logManager = LogManager::instance();
    logManager.mLogConfig.set_status(true);
    logManager.mLogConfig.set_log_enabled(areg::LogTarget::File, true);
}

//////////////////////////////////////////////////////////////////////////
// LogManager class constructor / destructor
//////////////////////////////////////////////////////////////////////////
LogManager::LogManager()
    : DispatcherThread      ( LogManager::LOGGING_THREAD_NAME.data(), areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM )
    , LoggingEventConsumer  ( )

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
void LogManager::clear_config_data()
{
    Lock lock(mLock);
    mScopeController.clear_config_scopes( );
}

void LogManager::reset()
{
    Lock lock(mLock);
    mScopeController.reset();
}

bool LogManager::is_remote_logging_enabled() const noexcept
{
    return mLogConfig.is_remote_logging_enabled();
}

bool LogManager::is_db_logging_enabled() const noexcept
{
    return mLogConfig.is_db_logging_enabled();
}

bool LogManager::is_file_logging_enabled() const noexcept
{
    return mLogConfig.is_file_logging_enabled();
}

bool LogManager::is_debug_logging_enabled() const noexcept
{
    return mLogConfig.is_debug_logging_enabled();
}

bool LogManager::start_logging_thread()
{
    ASSERT((is_running() == false) && (is_ready() == false));
    mLogStarted.reset( );
    if ( start(areg::WAIT_INFINITE) )
    {
        if ( wait_start(areg::WAIT_INFINITE) )
        {
            send_log_event( LoggingEventData(LoggingEventData::LogAction::StartLogs) );
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

void LogManager::stop_logging_thread(bool waitComplete)
{
    send_log_event( LoggingEventData(LoggingEventData::LogAction::StopLogs) );
    mIsStarted = false;

    if (waitComplete)
    {
        wait_completion(areg::WAIT_INFINITE);
        shutdown(areg::DO_NOT_WAIT);
    }
}

void LogManager::wait_thread_end()
{
    mIsStarted = false;
    wait_completion(areg::WAIT_INFINITE);
    shutdown(areg::DO_NOT_WAIT);
}

void LogManager::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        LoggingEvent::add_listener( static_cast<LoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );
        DispatcherThread::ready_for_events( true );
    }
    else
    {
        DispatcherThread::ready_for_events( false );
        LoggingEvent::remove_listener( static_cast<LoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )) );

        // When we are here, all loggers should be already closed.
        ASSERT(mLoggerFile.is_logger_opened() == false);
        ASSERT(mLoggerDebug.is_logger_opened() == false);
        ASSERT(mLoggerTcp.is_logger_opened() == false);
        ASSERT(mLoggerDatabase.is_logger_opened() == false);
    }
}

void LogManager::process_event( const LoggingEventData & data )
{
    mEventProcessor.process_log_event( data.logging_action( ), data.readable_stream( ) );
}

void LogManager::start_logs()
{
    if ( mLogConfig.is_logging_enabled() )
    {
        mScopeController.configure_scopes();
        mScopeController.set_scope_activity( true );
        if (mLoggerFile.is_logger_opened() == false)
        {
            mLoggerFile.open_logger();
        }

#if defined(OUTPUT_DEBUG)
        if (mLoggerDebug.is_logger_opened() == false)
        {
            mLoggerDebug.open_logger();
        }
#endif // !defined(OUTPUT_DEBUG)

        if (mLoggerTcp.is_logger_opened() == false)
        {
            mLoggerTcp.open_logger();
        }

        if (mLoggerDatabase.is_logger_opened() == false)
        {
            mLoggerDatabase.open_logger();
        }
    }

    mIsStarted = true;
    mLogStarted.set_event( );
}

void LogManager::stop_logs()
{
    mScopeController.set_scope_activity( false );
    mLogStarted.reset( );

    mIsStarted = false;

    mLoggerDebug.close_logger( );
    mLoggerFile.close_logger( );
    mLoggerTcp.close_logger( );
    mLoggerDatabase.close_logger();
    trigger_exit( );
}

void LogManager::write_log_message( const areg::LogEntry & logMessage)
{
    mLoggerFile.log_message(logMessage);
    mLoggerDebug.log_message(logMessage);
    mLoggerTcp.log_message(logMessage);
    mLoggerDatabase.log_message(logMessage);

    if ( has_more_events() == false )
    {
        mLoggerFile.flush_logs();
        mLoggerDatabase.flush_logs();
    }
}

bool LogManager::post_event(Event & eventElem)
{
    return EventDispatcher::post_event(eventElem);
}

inline void LogManager::send_log_event( const LoggingEventData & data, Event::EventPriority eventPrio /*= Event::EventPriority::NormalPrio*/ )
{
    LoggingEvent::send_event( data, static_cast<LoggingEventConsumer &>(self( )), static_cast<DispatcherThread &>(self( )), eventPrio );
}

void LogManager::change_scope_priority( const String & scopeName, uint32_t scopeId, uint32_t scopePrio )
{
    mScopeController.set_scope_activity( scopeName, scopeId, scopePrio );
}

} // namespace areg

#endif  // AREG_LOGGING
