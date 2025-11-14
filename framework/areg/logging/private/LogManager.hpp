#ifndef AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
#define AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogManager.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logging manager: container of scopes, configures,
 *              starts and stops logging.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/logging/private/LoggingEvent.hpp"

#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areg/logging/NELogging.hpp"
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
class IELogDatabaseEngine;
class LogScope;
class LogMessage;
class IELogger;
namespace NELogging
{
    struct sLogMessage;
}

//////////////////////////////////////////////////////////////////////////
// LogManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The log manager is a singleton container of all scopes registered
 *          in the system. It configures, starts and stops logging, loads scopes
 *          and changes priorities. Every created scope is registered in the
 *          Log Manager and unregistered when destroyed. Before logging, it should 
 *          be started and the configuration should be loaded.
 **/
class LogManager    : public    DispatcherThread
                    , private   IELoggingEventConsumer
{
    friend class LogEventProcessor;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    //!< The thread name of logging thread
    static constexpr std::string_view   LOGGING_THREAD_NAME          { "_AREG_LOGGING_THREAD_" };

    //!< Logging activation waiting maximum timeout
    static constexpr unsigned int       LOG_START_WAITING_TIME      { NECommon::WAIT_10_SECONDS };

    //!< Reconnect timeout in milliseconds
    static constexpr unsigned int       LOG_RECONNECT_TIMEOUT       { NECommon::TIMEOUT_1_SEC * 5 };

public:

    /**
     * \brief   Triggers an event to log message created locally in the same process.
     * \param   logData The logging message object, which will be sent to all loggers.
     **/
    static void logMessage( const NELogging::sLogMessage & logData );

    /**
     * \brief   Triggers an event to log message contained in the shared buffer.
     * \param   logData     The instance of message in shared buffer to log.
     **/
    static void logMessage(const SharedBuffer& logData);

    /**
     * \brief   Triggers an event to log remote message.
     * \param   logData     The instance of remote message buffer, which contains the
     *                      log message from another process.
     **/
    static void logMessage( const RemoteMessage& logData );

    /**
     * \brief   Generates and queues a message to execute internal command.
     * \param   cmd     The command to execute.
     * \param   data    The binary data to pass in the command.
     **/
    static void sendCommandMessage(LoggingEventData::eLoggingAction cmd, const SharedBuffer& data);

    /**
     * \brief   Call to configure logging. The passed configuration file name should be either
     *          full or relative path to configuration file. If passed nullptr,
     *          the default configuration file will be loaded.
     **/
    static bool readLogConfig( const char * configFile = nullptr );

    /**
     * \brief   Call to initialize and start logging.
     *          The initialization data is read out from specified log configuration file.
     *          If specified file is nullptr, the configuration will be read out from
     *          default log configuration.
     * \param   configFile  The full or relative path to configuration file.
     *                      If nullptr, the log configuration will be read out
     *                      from default configuration file.
     * \return  Returns true if could read configuration and start logging thread.
     *          If logging was already started, the call will be ignored and return true.
     *          If starting fails, returns false.
     **/
    static bool startLogging( const char * configFile = nullptr );

    /**
     * \brief   Saves the current logging state in the configuration file.
     * \param   configFile  Relative of absolute path to the configuration file.
     *                      If nullptr, it uses the file used to configure the logs.
     * \return  Returns true if succeeded to save the current logging state in the configuration file.
     **/
    static bool saveLogConfig( const char * configFile = nullptr );

    /**
     * \brief   Updates the list of scopes and log priorities in the application configuration.
     **/
    static void updateScopeConfiguration( void );

    /**
     * \brief   Call to stop Logging Manager and exits the thread.
     *          If 'waitComplete' is set to true, the calling thread is
     *          blocked until logging Manager completes jobs and cleans resources.
     *          Otherwise, this triggers stop and exit events, and immediately returns.
     * \param   waitComplete    If true, waits for Logging Manager to complete the jobs
     *                          and exit threads. Otherwise, it triggers exit and returns.
     **/
    inline static void stopLogging(bool waitComplete);

    /**
     * \brief   The calling thread is blocked until Logging Manager did not
     *          complete the job and exit. This should be called if previously
     *          it was requested to stop the Logging Manager without waiting for completion.
     **/
    inline static void waitLoggingEnd(void);

    /**
     * \brief   Registers instance of log scope object in log manager.
     * \param   scope   The instance of log scope object to register.
     **/
    inline static void registerLogScope( LogScope & scope );

    /**
     * \brief   Unregisters instance of log scope object from log manager.
     * \param   scope   The instance of log scope to unregister.
     **/
    inline static void unregisterLogScope( LogScope & scope );

    /**
     * \brief   Activates log scope. Finds priority in priority list
     *          and sets scope message priority.
     * \param   scope   The instance of log scope object to activate
     *                  and set logging priority.
     **/
    inline static void activateLogScope( LogScope & scope );

    /**
     * \brief   Returns true if logging has started
     **/
    inline static bool isLoggingStarted( void );

    /**
     * \brief   Returns true if logging is configured and ready to start
     **/
    static bool isLoggingConfigured( void );

    /**
     * \brief   Returns true if logging is enabled.
     **/
    static bool isLoggingEnabled( void );

    /**
     * \brief   Call to force to activate logging with default settings.
     *          The logging will be activated only if logging is not running and
     *          only in debug build. For release, please use real logging configuration.
     * \return  Returns true if could activate logging. For non-debug builds, the function always returns false.
     **/
    static bool forceActivateLogging( void );

    /**
     * \brief   Forces to enable logging.
     *          If logging is not configured, it will set default configuration,
     *          then it will enable and start logging. If logging is configured,
     *          it will enable and start logging with the existing configuration.
     *          To overwrite the existing configuration and use default, call
     *          method setDefaultConfiguration()
     * \see     setDefaultConfiguration
     **/
    static void forceEnableLogging(void);

    /**
     * \brief   Call to set the default configuration of logging if it is not
     *          configured or overwrite the existing configuration, depending
     *          on the passed overwriteExisting parameter.
     * \param   overwriteExisting   Flag, indicating whether the existing configuration
     *                              should be overwritten or not.
     *                              If the parameter is true, then independent whether
     *                              whether the configuration was already loaded or not
     *                              the function will reset the existing configuration
     *                              and set default parameters. Otherwise, the default
     *                              configuration is used only if not configured.
     *                              Note, that after calling this method, the application
     *                              ignores to load configuration from the file.
     *                              Reset configuration to load from file.
     **/
    static void setDefaultConfiguration(bool overwriteExisting);

    /**
     * \brief   Call to change the scope log priority.
     * \param   scopeName   The name of the existing scope. Ignored if scope does not exit.
     * \param   newPrio     The new priority to set. Can be bitwise combination of priorities.
     * \return  Returns true if scope found and priority changed.
     **/
    static bool setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   Call to change the scope log priority.
     * \param   scopeName   The name of a single scope or scope group ended with '*' to change priority.
     * \param   scopeId     The ID of the scope, ignored in case of scope group.
     * \param   newPrio     The new priority to set. Can be bitwise combination of priorities.
     **/
    static void updateScopes(const String & scopeName, unsigned int scopeId, unsigned int newPrio);

    /**
     * \brief   Returns the scope priority if found. Otherwise, returns invalid priority.
     * \param   scopeName   The name of the existing scope.
     * \return  Is found the scope, returns the actual priority of the scope.
     *          Otherwise, returns invalid priority (NELogging::eLogPriority::PrioInvalid).
     **/
    static unsigned int getScopePriority( const char * scopeName );

    /**
     * \brief   Call to set the instance of log database engine responsible to handle the database.
     *          If not null, the log messages are forwarded to the database engine.
     *          Otherwise, ignores to write the log messages.
     * \param   dbEngine    The pointer to the log database engine to set.
     **/
    static void setLogDatabaseEngine(IELogDatabaseEngine * dbEngine);

    /**
     * \brief   Returns true if database and data tables are initialized and the
     *          logging database is ready to save messages.
     **/
    static bool isLogDabaseEngineInitialized(void);

    /**
     * \brief   Returns true if logging in the database is enabled. The value is read from configuration file.
     **/
    static bool isLogDatabaseEnabled(void);

    /**
     * \brief   Returns the Log Collector service connection cookie.
     */
    inline static const ITEM_ID & getConnectionCookie(void);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns singleton instance of Log Manager.
     **/
    static LogManager& getInstance(void);

    /**
     * \brief   Protected default constructor.
     **/
    LogManager( void );

    /**
     * \brief   Protected destructor.
     **/
    virtual ~LogManager( void ) = default;

protected:
//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem ) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// IELoggingEventConsumer interface overrides
/************************************************************************/
    /**
     * \brief   Called by event dispatcher when processes the logging event data.
     * \param   data    The logging event data to process.
     **/
    virtual void processEvent( const LoggingEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// Logging initialization, start / stop
/************************************************************************/

    /**
     * \brief   Starts logging thread, loads scopes and sets up all logging objects.
     *          The configuration should be already loaded.
     * \return  Returns true if started with success.
     **/
    bool startLoggingThread( void );
     
     /**
      * \brief   Call to stop Logging Manager and exits the thread.
      *          If 'waitComplete' is set to true, the calling thread is
      *          blocked until logging Manager completes jobs and cleans resources.
      *          Otherwise, this triggers stop and exit events, and immediately returns.
      * \param   waitComplete    If true, waits for Logging Manager to complete the jobs
      *                          and exit threads. Otherwise, it triggers exit and returns.
      **/
    void stopLoggingThread(bool waitComplete);

    /**
     * \brief   The calling thread is blocked until Logging Manager did not
     *          complete the job and exit. This should be called if previously
     *          it was requested to stop the Logging Manager without waiting for completion.
     **/
    void waitLoggingThreadEnd(void);

    /**
     * \brief   Returns true, if in configuration the remote logging is enabled.
     **/
    bool isRemoteLoggingEnabled( void ) const;

    /**
     * \brief   Returns true, if in configuration the database logging is enabled.
     **/
    bool isDatabaseLoggingEnabled( void ) const;

    /**
     * \brief   Returns true, if in configuration the logging in file is enabled.
     **/
    bool isFileLoggingEnabled( void ) const;

    /**
     * \brief   Returns true, if in configuration the logging in debug output window is enable.
     *          This setting is relevant only to Microsoft Visual Studio.
     **/
    bool isDebugOutputLoggingEnabled( void ) const;

    /**
     * \brief   Clears logging configuration data.
     **/
    void clearConfigData( void );

    /**
     * \brief   Resets the scopes.
     **/
    void resetScopes(void);

    /**
     * \brief   Loads scopes and sets priorities specified in configuration.
     **/
    void startLogs( void );

    /**
     * \brief   Deactivates all scopes to stop logging.
     **/
    void stopLogs( void );

    /**
     * \brief   Writes a log message to the existing loggers.
     * \param   logMessage  The message to log.
     **/
    void writeLogMessage( const NELogging::sLogMessage & logMessage );

    /**
     * \brief   Sends log event with the preferred priority.
     *          By default, it the priority is Normal.
     **/
    void sendLogEvent( const LoggingEventData & data, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal);

    /**
     * \brief   Changes the scope priority. It can be either a single scope or scope group.
     * \param   scopeName   The name of a single scope or group of scopes ending with '*'.
     * \param   scopeId     The ID of the scope. If it is a scope group, the value is ignored.
     * \param   scopePrio   The new priority to set to the scope or scope group.
     **/
    void changeScopePriority( const String & scopeName, unsigned int scopeId, unsigned int scopePrio );

    /**
     * \brief   Returns read-only list of registered scopes.
     **/
    inline const TEHashMap<unsigned int, LogScope *> & getScopeList( void ) const;

    /**
     * \brief   Returns instance of log manager.
     **/
    inline LogManager & self( void );

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
    LogEventProcessor mEventProcessor;
    /**
     * \brief   An event, indicating that the logging has been started.
     */
    SynchEvent          mLogStarted;
    /**
     * \brief   Synchronization object used to synchronize data access.
     **/
    mutable ResourceLock    mLock;

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    DECLARE_NOCOPY_NOMOVE( LogManager );
};

//////////////////////////////////////////////////////////////////////////
// LogManager class inline functions
//////////////////////////////////////////////////////////////////////////

inline void LogManager::stopLogging(bool waitComplete)
{
    getInstance().stopLoggingThread(waitComplete);
}

inline void LogManager::waitLoggingEnd(void)
{
    getInstance().waitLoggingThreadEnd();
}

inline void LogManager::registerLogScope(LogScope& scope)
{
    getInstance().mScopeController.registerScope(scope);
}

inline void LogManager::unregisterLogScope( LogScope & scope )
{
    getInstance( ).mScopeController.unregisterScope( scope );
}

inline void LogManager::activateLogScope(LogScope& scope)
{
    getInstance().mScopeController.activateScope(scope);
}

inline const ITEM_ID & LogManager::getConnectionCookie(void)
{
    return LogManager::getInstance().mLoggerTcp.getConnectionCookie();
}

inline const TEHashMap<unsigned int, LogScope *> & LogManager::getScopeList( void ) const
{
    return mScopeController.getScopeList( );
}

inline LogManager & LogManager::self( void )
{
    return (*this);
}

inline bool LogManager::isLoggingStarted( void )
{
    Lock lock(getInstance().mLock);
    return getInstance().mIsStarted;
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGMANAGER_HPP
