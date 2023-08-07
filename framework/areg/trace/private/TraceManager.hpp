#ifndef AREG_TRACE_PRIVATE_TRACEMANAGER_HPP
#define AREG_TRACE_PRIVATE_TRACEMANAGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceManager.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace manager. The container of scopes, configuring
 *              tracer, starts and stops tracing.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/trace/private/TraceEvent.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

#include "areg/component/Timer.hpp"

#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/ScopeController.hpp"
#include "areg/trace/private/FileLogger.hpp"
#include "areg/trace/private/DebugOutputLogger.hpp"
#include "areg/trace/private/NetTcpLogger.hpp"
#include "areg/trace/private/LogConfiguration.hpp"
#include "areg/trace/private/TraceEventProcessor.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;
class LogMessage;
class IELogger;

//////////////////////////////////////////////////////////////////////////
// TraceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The trace manager is a singleton container of all scopes registered
 *          in the system, as well as it is configuring tracing, starts
 *          and stops tracing thread, loading scopes and changes priorities.
 *          Every created scope is registered in Trace Manager and unregistered,
 *          when destroyed. Before system is able to log, the tracing should 
 *          be started (trace thread) and the configuration should be loaded.
 **/
class TraceManager  : private   DispatcherThread
                    , private   IETraceEventConsumer
                    , private   IETimerConsumer
{
    friend class TraceEventProcessor;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    //!< The thread name of tracer
    static constexpr std::string_view   TRACER_THREAD_NAME          { "_AREG_TRACER_THREAD_" };

    //!< Logging activation waiting maximum timeout
    static constexpr unsigned int       LOG_START_WAITING_TIME      { NECommon::WAIT_10_SECONDS };

    //!< Reconnect timeout in milliseconds
    static constexpr unsigned int       LOG_RECONNECT_TIMEOUT       { NECommon::TIMEOUT_1_SEC * 5 };

public:

    /**
     * \brief   Sends logging message for logging.
     * \param   logData The logging message object, which will be sent to all loggers.
     **/
    static void sendLogMessage( LogMessage & logData );

    /**
     * \brief   Call to configure logging. The passed configuration file name should be either
     *          full or relative path to configuration file. If passed nullptr,
     *          the default configuration file will be loaded.
     **/
    inline static bool readLogConfig( const char * configFile = nullptr );

    /**
     * \brief   Call to read the configuration instructions from the given file opened for reading.
     * \param   file    The file with configuration instructions opened for reading.
     * \return  Returns true if succeeded to read and configure the logging.
     **/
    inline static bool readLogConfig( const FileBase & file );

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
    static bool startLogging( const char * configFile = nullptr);

    /**
     * \brief   Saves the current logging state in the configuration file.
     * \param   configFile  Relative of absolute path to the configuration file.
     *                      If nullptr, it uses the file used to configure the logs.
     * \return  Returns true if succeeded to save the current logging state in the configuration file.
     **/
    static bool saveLogConfig( const char * configFile = nullptr );

    /**
     * \brief   Saves the current logging state in the given file opened to write.
     * \param   file    The opened file to save configuration.
     * \return  Returns true if succeeded to save the current logging state in the file.
     **/
    static bool saveLogConfig( FileBase & file );

    /**
     * \brief   Call to stop logging. This call will stop all loggers and exit thread.
     *          The call will be blocked until logging thread is stopped.
     **/
    inline static void stopLogging( void );

    /**
     * \brief   Registers instance of trace scope object in trace manager.
     *          The trace scope should have unique ID.
     * \param   scope   The instance of trace scope object to register
     **/
    inline static void registerTraceScope( TraceScope & scope );

    /**
     * \brief   Unregisters instance of trace scope object in trace manager.
     * \param   scope   The instance of trace scope to unregister
     **/
    inline static void unregisterTraceScope( TraceScope & scope );

    /**
     * \brief   Activates trace scope. Finds priority in priority list
     *          and sets scope priority.
     * \param   scope   The instance of trace scope object to activate
     *                  and set logging priority.
     **/
    inline static void activateTraceScope( TraceScope & scope );

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
     * \brief   Returns true if logging via network is enabled.
     **/
    static bool isNetLoggingEnabled(void);

    /**
     * \brief   Returns the logging config file name set in the system.
     **/
    static const String& getConfigFile( void );

    /**
     * \brief   Call to force to activate logging with default settings.
     *          The logging will be activated only if logging is not running and
     *          only in debug build. For release, please use real logging configuration.
     * \return  Returns true if could activate logging. For non-debug builds, the function always returns false.
     **/
    static bool forceActivateLogging( void );

    /**
     * \brief   Call to change the scope log priority.
     * \param   scopeName   The name of the existing scope. Ignored if scope does not exit.
     * \param   newPrio     The new priority to set. Can be bitwise combination with scopes.
     * \return  Returns true if scope found and priority changed.
     **/
    static bool setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   Returns the scope priority if found. Otherwise, returns invalid priority.
     * \param   scopeName   The name of the existing scope.
     * \return  Is found the scope, returns the actual priority of the scope.
     *          Otherwise, returns invalid priority (NETrace::eLogPriority::PrioInvalid).
     **/
    static unsigned int getScopePriority( const char * scopeName );

    /**
     * \brief   Call if connection lost.
     **/
    static void netConnectionLost( void );

    /**
     * \brief   Call if received data from logging service.
     **/
    static void netReceivedData( const SharedBuffer & data );

    /**
     * \brief   Forces to enable logging.
     **/
    inline static void forceEnableLogging( void );

    /**
     * \brief   Returns the cookie ID. If NETrace::COOKIE_LOCAL there is no remote logging.
     **/
    inline static const ITEM_ID & getCookie(void);

    /**
     * \brief   Returns the module ID set by the system. Normally, it is the process ID.
     **/
    inline static const ITEM_ID & getModuleId(void);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns singleton instance of trace scope manager
     **/
    static TraceManager& getInstance(void);

    /**
     * \brief   Protected default constructor.
     **/
    TraceManager( void );

    /**
     * \brief   Protected destructor.
     **/
    virtual ~TraceManager( void ) = default;

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
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool runDispatcher( void ) override;

/************************************************************************/
// IETraceEventConsumer interface overrides
/************************************************************************/
    virtual void processEvent( const TraceEventData & data ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

    /**
     * \brief   Automatically triggered when event is dispatched by thread.
     * \param   data    The Timer Event Data object containing Timer object.
     **/
    virtual void processEvent( const TimerEventData & data ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

/************************************************************************/
// Logging configuration, start / stop
/************************************************************************/

    /**
     * \brief   Loads specified logging configuration file. If specified file is nullptr or empty,
     *          the system will use default path to load configuration and scopes.
     * \param   filePath    Relative or absolute path of configuration file.
     * \return  Returns true if succeeded to load configuration file. Otherwise, returns false.
     *          The valid configuration should contain at least one tracing, even if it is disabled.
     **/
    bool loadConfiguration( const char * filePath = nullptr );

    /**
     * \brief   Unloads the configuration, resets all logging.
     **/
    void unloadConfiguration( void );

    /**
     * \brief   Starts logging thread, loads scopes and sets up all tracers.
     *          The configuration should be already loaded.
     * \return  Returns true if started with success.
     **/
    bool startLoggingThread( void );

    /**
     * \brief   Stops logging thread. If needed, it will wait for completion.
     * \param   waitTimeout     Timeout to wait in milliseconds. If zero, it will not wait
     *                          and immediately returns from method. If INFINITE (value 0xFFFFFFFF),
     *                          it will wait until thread completes job and exits.
     **/
    void stopLoggingThread( void );

    /**
     * \brief   Returns true, if settings to log traces on remote host are valid.
     **/
    bool isNetConfigValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in database are valid.
     **/
    bool isDatabaseValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in file are valid.
     **/
    bool isFileValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in debugging output window are valid.
     **/
    bool isDebugOutputValid( void ) const;


    /**
     * \brief   Clears logging configuration data.
     **/
    void clearConfigData( void );

    /**
     * \brief   Loads scopes and sets priorities specified in configuration.
     * \return  Returns true, if loading of scopes succeeded.
     **/
    void traceStartLogs( void );

    /**
     * \brief   Deactivates all scopes to stop logging.
     **/
    void traceStopLogs( void );

    /**
     * \brief   Writes a log message to the existing loggers.
     * \param   logMessage  The message to log.
     **/
    void writeLogMessage( const NETrace::sLogMessage & logMessage );

    /**
     * \brief   Sends a binary data to the remote log service.
     * \param   logData     The binary log data to send to remote log service.
     * \return  Returns true if succeeded to send. Otherwise, returns false.
     **/
    bool sendLogData( const SharedBuffer & logData );

    /**
     * \brief   Sends log event with the preferred priority.
     *          By default, it the priority is Normal.
     **/
    void sendLogEvent( const TraceEventData & data, Event::eEventPriority eventPrio = Event::eEventPriority::EventPriorityNormal);

    /**
     * \brief   Establishes TCP/IP connection with the remote log service.
     **/
    void connectTcpLogService( void );

    /**
     * \brief   Triggers a timer to reconnect with the TCP/IP remote logging service.
     **/
    void reconnectTcpLogService( void );

    /**
     * \brief   Disconnects from the TCP/IP remote logging service.
     **/
    void disconnectTcpLogService( void );

    /**
     * \brief   Sets and activates new Cookie set by remote logging service.
     **/
    void activateCookie( ITEM_ID newCookie );

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
    inline const TEHashMap<unsigned int, TraceScope *> & getScopeList( void ) const;

    /**
     * \brief   Returns instance of trace manager.
     **/
    inline TraceManager & self( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The trace control object to activate / deactivate and change priority of the scopes.
     **/
    ScopeController     mScopeController;
    /**
     * \brief   The flag, indicating whether logger is started or not
     **/
    bool                mIsStarted;
    /**
     * \brief   The logging configuration
     **/
    LogConfiguration    mLogConfig;
    /**
     * \brief   The unique ID of the module.
     **/
    ITEM_ID             mModuleId;
    /**
     * \brief   The cookie set by logger service, if there is any.
     **/
    ITEM_ID             mCookie;
    /**
     * \brief   The file logger object, to output logs in the file.
     **/
    FileLogger          mLoggerFile;
    /**
     * \brief   The debug output logger to output logs in the output device (window).
     **/
    DebugOutputLogger   mLoggerDebug;
    /**
     * \brief   The remote TCP/IP logging service.
     **/
    NetTcpLogger        mLoggerTcp;
    /**
     * \brief   The timer to reconnect with remote logging service
     **/
    Timer               mTimerReconnect;
    /**
     * \brief   The log event processor helper object.
     **/
    TraceEventProcessor mEventProcessor;
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
    DECLARE_NOCOPY_NOMOVE( TraceManager );
};

//////////////////////////////////////////////////////////////////////////
// TraceManager class inline functions
//////////////////////////////////////////////////////////////////////////

inline void TraceManager::stopLogging( void )
{
    getInstance().stopLoggingThread( );
    getInstance( ).unloadConfiguration( );
}

inline void TraceManager::registerTraceScope(TraceScope& scope)
{
    getInstance().mScopeController.registerScope(scope);
}

inline void TraceManager::unregisterTraceScope( TraceScope & scope )
{
    getInstance( ).mScopeController.unregisterScope( scope );
}

inline void TraceManager::activateTraceScope(TraceScope& scope)
{
    getInstance().mScopeController.activateScope(scope);
}

inline bool TraceManager::readLogConfig(const char* configFile /*= nullptr */)
{
    return TraceManager::getInstance().loadConfiguration(configFile);
}

inline bool TraceManager::readLogConfig( const FileBase & file )
{
    return TraceManager::getInstance( ).mLogConfig.loadConfig( file );
}

inline const ITEM_ID & TraceManager::getCookie(void)
{
    return TraceManager::getInstance().mCookie;
}

inline const ITEM_ID& TraceManager::getModuleId(void)
{
    return TraceManager::getInstance().mModuleId;
}

inline void TraceManager::forceEnableLogging(void)
{
    TraceManager::getInstance().mLogConfig.getStatus().parseProperty(NELogConfig::DEFAULT_LOG_ENABLE.data());
}

inline const TEHashMap<unsigned int, TraceScope *> & TraceManager::getScopeList( void ) const
{
    return mScopeController.getScopeList( );
}

inline TraceManager & TraceManager::self( void )
{
    return (*this);
}

inline bool TraceManager::isLoggingStarted( void )
{
    Lock lock(getInstance().mLock);
    return getInstance().mIsStarted;
}

#endif  // AREG_TRACE_PRIVATE_TRACEMANAGER_HPP
