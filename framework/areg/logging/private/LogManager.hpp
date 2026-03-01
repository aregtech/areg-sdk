#ifndef AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
#define AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogManager.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Logging manager: container of scopes, configures,
 *              starts and stops logging.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/logging/private/LoggingEvent.hpp"

#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/private/ScopeController.hpp"
#include "areg/logging/private/FileLogger.hpp"
#include "areg/logging/private/DebugOutputLogger.hpp"
#include "areg/logging/private/NetTcpLogger.hpp"
#include "areg/logging/private/DatabaseLogger.hpp"
#include "areg/logging/private/LogEventProcessor.hpp"

#include <string_view>

#if AREG_LOGS

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class LogDatabaseEngine;
    class LogScope;
    class LogMessage;
    struct LogEntry;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Singleton container for all registered log scopes. Manages logging initialization,
 *          configuration, scope registration, and priority changes. Every scope is registered on
 *          creation and unregistered on destruction.
 **/
class LogManager    : public    DispatcherThread
                    , private   LoggingEventConsumer
{
    friend class LogEventProcessor;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    //!< The thread name of logging thread
    static constexpr std::string_view   LOGGING_THREAD_NAME          { "_AREG_LOGGING_THREAD_" };

    //!< Logging activation waiting maximum timeout
    static constexpr uint32_t       LOG_START_WAITING_TIME      { areg::WAIT_10_SECONDS };

    //!< Reconnect timeout in milliseconds
    static constexpr uint32_t       LOG_RECONNECT_TIMEOUT       { areg::TIMEOUT_1_SEC * 5 };

public:

    /**
     * \brief   Triggers an event to log a message created locally.
     *
     * \param   logData     The logging message object to send to all loggers.
     **/
    static void log_message( const areg::LogEntry & logData );

    /**
     * \brief   Triggers an event to log a message contained in a shared buffer.
     *
     * \param   logData     The message in the shared buffer to log.
     **/
    static void log_message(const SharedBuffer& logData);

    /**
     * \brief   Triggers an event to log a remote message.
     *
     * \param   logData     The remote message buffer containing a log message from another process.
     **/
    static void log_message( const RemoteMessage& logData );

    /**
     * \brief   Generates and queues an internal command message.
     *
     * \param   cmd     The command to execute.
     * \param   data    The binary data to pass in the command.
     **/
    static void send_command_message(LoggingEventData::LogAction cmd, const SharedBuffer& data);

    /**
     * \brief   Reads logging configuration from a file.
     *
     * \param   configFile      Path to the configuration file (full or relative). If nullptr, loads
     *                          the default configuration file.
     * \return  Returns true if configuration was loaded successfully.
     **/
    static bool read_log_config( const char * configFile = nullptr );

    /**
     * \brief   Initializes and starts the logging thread.
     *
     * \param   configFile      Path to the logging configuration file (full or relative). If
     *                          nullptr, uses the default configuration file.
     * \return  Returns true if configuration was read and logging thread started. Returns true if
     *          logging was already running. Returns false on failure.
     **/
    static bool start_logging( const char * configFile = nullptr );

    /**
     * \brief   Saves the current logging configuration state to a file.
     *
     * \param   configFile      Path to the configuration file (relative or absolute). If nullptr,
     *                          uses the file used to configure logging.
     * \return  Returns true if the configuration was saved successfully.
     **/
    static bool save_log_config( const char * configFile = nullptr );

    /**
     * \brief   Updates the list of scopes and logging priorities in the application configuration.
     **/
    static void update_scope_configuration();

    /**
     * \brief   Stops the logging manager and exits the thread.
     *
     * \param   waitComplete    If true, blocks until logging completes and cleans resources.
     *                          Otherwise, triggers exit and returns immediately.
     **/
    inline static void stop_logging(bool waitComplete);

    /**
     * \brief   Blocks until the logging manager completes and exits.
     **/
    inline static void wait_logging_end();

    /**
     * \brief   Registers a log scope object in the log manager.
     *
     * \param   scope       The log scope object to register.
     **/
    inline static void register_log_scope( LogScope & scope );

    /**
     * \brief   Unregisters a log scope object from the log manager.
     *
     * \param   scope       The log scope object to unregister.
     **/
    inline static void unregister_log_scope( LogScope & scope );

    /**
     * \brief   Activates a log scope and sets its logging priority from the configuration.
     *
     * \param   scope       The log scope object to activate and set priority for.
     **/
    inline static void activate_log_scope( LogScope & scope );

    /**
     * \brief   Returns true if logging has started.
     **/
    inline static bool is_logging_started();

    /**
     * \brief   Returns true if logging is configured and ready to start.
     **/
    static bool is_logging_configured();

    /**
     * \brief   Returns true if logging is enabled.
     **/
    static bool is_logging_enabled();

    /**
     * \brief   Forces logging to activate with default settings for debug builds only.
     *
     * \return  Returns true if logging was activated. Always returns false for release builds.
     **/
    static bool force_activate_logging();

    /**
     * \brief   Enables logging with default configuration or existing configuration if already
     *          configured.
     **/
    static void force_enable_logging();

    /**
     * \brief   Sets default logging configuration or overwrites existing configuration if
     *          requested.
     *
     * \param   overwriteExisting       If true, resets and overwrites existing configuration with
     *                                  defaults regardless of prior state. If false, only applies
     *                                  defaults if not previously configured. After calling with
     *                                  true, configuration will not be loaded from file until
     *                                  reset.
     **/
    static void set_default_configuration(bool overwriteExisting);

    /**
     * \brief   Sets the logging priority for a scope.
     *
     * \param   scopeName       The name of the existing scope. Ignored if scope does not exist.
     * \param   newPrio         The new priority to set. Can be a bitwise combination of priorities.
     * \return  Returns true if the scope was found and priority was changed.
     **/
    static bool set_scope_priority( const char * scopeName, uint32_t newPrio );

    /**
     * \brief   Updates the logging priority of a scope or scope group.
     *
     * \param   scopeName       The name of a single scope or scope group ending with '*'.
     * \param   scopeId         The ID of the scope; ignored for scope groups.
     * \param   newPrio         The new priority to set. Can be a bitwise combination of priorities.
     **/
    static void update_scopes(const String & scopeName, uint32_t scopeId, uint32_t newPrio);

    /**
     * \brief   Returns the logging priority of a scope.
     *
     * \param   scopeName       The name of the existing scope.
     * \return  The priority of the scope if found; otherwise, returns
     *          areg::LogPriority::PrioInvalid.
     **/
    static uint32_t scope_priority( const char * scopeName );

    /**
     * \brief   Sets the log database engine instance for handling database storage.
     *
     * \param   dbEngine    The log database engine instance. If nullptr, disables database logging.
     **/
    static void set_db_engine(LogDatabaseEngine * dbEngine);

    /**
     * \brief   Returns true if the logging database and tables are initialized and ready.
     **/
    static bool is_db_initialized();

    /**
     * \brief   Returns true if logging to the database is enabled in the configuration.
     **/
    static bool is_db_enabled();

    /**
     * \brief   Returns the connection cookie of the log collector service.
     **/
    inline static const ITEM_ID & connection_cookie();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the singleton instance of the log manager.
     **/
    static LogManager& instance();

    LogManager();

    /**
     * \brief   Protected destructor.
     **/
    virtual ~LogManager() = default;

protected:
//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts an event and delivers it to its target.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true.
     **/
    bool post_event( Event & eventElem ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables the dispatcher to receive events.
     *
     * \param   is_ready     True to enable event dispatching, false to disable.
     **/
    void ready_for_events( bool is_ready ) override;

/************************************************************************/
// LoggingEventConsumer interface overrides
/************************************************************************/
    /**
     * \brief   Processes a logging event.
     *
     * \param   data    The logging event data to process.
     **/
    void process_event( const LoggingEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// Logging initialization, start / stop
/************************************************************************/

    /**
     * \brief   Starts the logging thread and loads scopes with configured priorities.
     *
     * \return  Returns true if started successfully.
     **/
    bool start_logging_thread();
     
    /**
     * \brief   Stops the logging manager and exits the thread.
     *
     * \param   waitComplete    If true, blocks until logging completes and cleans resources.
     *                          Otherwise, triggers exit and returns immediately.
     **/
    void stop_logging_thread(bool waitComplete);

    /**
     * \brief   Blocks until the logging manager completes and exits.
     **/
    void wait_thread_end();

    /**
     * \brief   Returns true if remote logging is enabled in the configuration.
     **/
    bool is_remote_logging_enabled() const;

    /**
     * \brief   Returns true if database logging is enabled in the configuration.
     **/
    bool is_db_logging_enabled() const;

    /**
     * \brief   Returns true if file logging is enabled in the configuration.
     **/
    bool is_file_logging_enabled() const;

    /**
     * \brief   Returns true if debug output window logging is enabled. Only relevant for Visual
     *          Studio.
     **/
    bool is_debug_logging_enabled() const;

    /**
     * \brief   Clears all logging configuration data.
     **/
    void clear_config_data();

    /**
     * \brief   Resets and deactivates all scopes.
     **/
    void reset();

    /**
     * \brief   Activates scopes and sets their priorities from the configuration.
     **/
    void start_logs();

    /**
     * \brief   Deactivates all scopes to prevent logging.
     **/
    void stop_logs();

    /**
     * \brief   Writes a log message to all registered loggers.
     *
     * \param   logMessage     The message to log.
     **/
    void write_log_message( const areg::LogEntry & logMessage);

    /**
     * \brief   Sends a logging event with the specified priority.
     *
     * \param   data            The logging event data.
     * \param   eventPrio       The priority of the event. Defaults to NormalPrio.
     **/
    void send_log_event( const LoggingEventData & data, Event::EventPriority eventPrio = Event::EventPriority::NormalPrio);

    /**
     * \brief   Changes the logging priority of a scope or scope group.
     *
     * \param   scopeName       The name of a single scope or scope group ending with '*'.
     * \param   scopeId         The ID of the scope; ignored for scope groups.
     * \param   scopePrio       The new logging priority to set.
     **/
    void change_scope_priority( const String & scopeName, uint32_t scopeId, uint32_t scopePrio );

    /**
     * \brief   Returns a read-only list of registered scopes.
     **/
    inline const HashMap<uint32_t, LogScope *> & scope_list() const;

    /**
     * \brief   Returns the log manager instance.
     **/
    inline LogManager & self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Scope controller object to activate / deactivate and change priority of the scopes.
     **/
    ScopeController     mScopeController;
    /**
     * \brief   Flag, indicating whether the logging is started or not
     **/
    bool                mIsStarted;
    /**
     * \brief   Logging configuration
     **/
    LogConfiguration    mLogConfig;
    /**
     * \brief   File logging  object, to output logs in the file.
     **/
    FileLogger          mLoggerFile;
    /**
     * \brief   The debug output logging to output logs in the output device (window).
     **/
    DebugOutputLogger   mLoggerDebug;
    /**
     * \brief   The remote TCP/IP logging service.
     **/
    NetTcpLogger        mLoggerTcp;
    /**
     * \brief   The database logging object to write logs in the DB.
     **/
    DatabaseLogger      mLoggerDatabase;
    /**
     * \brief   The log event processor helper object.
     **/
    LogEventProcessor   mEventProcessor;
    /**
     * \brief   An event, indicating that the logging has been started.
     */
    SyncEvent           mLogStarted;
    /**
     * \brief   Synchronization object used to synchronize data access.
     **/
    mutable ResourceLock    mLock;

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    AREG_NOCOPY_NOMOVE( LogManager );
};

//////////////////////////////////////////////////////////////////////////
// LogManager class inline functions
//////////////////////////////////////////////////////////////////////////

inline void LogManager::stop_logging(bool waitComplete)
{
    instance().stop_logging_thread(waitComplete);
}

inline void LogManager::wait_logging_end()
{
    instance().wait_thread_end();
}

inline void LogManager::register_log_scope(LogScope& scope)
{
    instance().mScopeController.register_scope(scope);
}

inline void LogManager::unregister_log_scope( LogScope & scope )
{
    instance( ).mScopeController.unregister_scope( scope );
}

inline void LogManager::activate_log_scope(LogScope& scope)
{
    instance().mScopeController.activate_scope(scope);
}

inline const ITEM_ID & LogManager::connection_cookie()
{
    return LogManager::instance().mLoggerTcp.connection_cookie();
}

inline const HashMap<uint32_t, LogScope *> & LogManager::scope_list() const
{
    return mScopeController.scope_list( );
}

inline LogManager & LogManager::self()
{
    return (*this);
}

inline bool LogManager::is_logging_started()
{
    Lock lock(instance().mLock);
    return instance().mIsStarted;
}

} // namespace areg

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
