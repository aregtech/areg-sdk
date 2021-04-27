#ifndef AREG_TRACE_PRIVATE_CETRACEMANAGER_HPP
#define AREG_TRACE_PRIVATE_CETRACEMANAGER_HPP
/************************************************************************
 * \file        areg/trace/private/CETraceManager.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace manager. The container of scopes, configuring
 *              tracer, starts and stops tracing.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/component/CEDispatcherThread.hpp"
#include "areg/trace/private/CETraceEvent.hpp"
#include "areg/trace/private/IETraceConfiguration.hpp"

#include "areg/base/TEResourceMap.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/CEVersion.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/ESynchObjects.hpp"

#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/CETraceProperty.hpp"
#include "areg/trace/private/CEFileLogger.hpp"
#include "areg/trace/private/CEDebugOutputLogger.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETraceScope;
class CELogMessage;
class CESortedStringList;
class IELogger;

//////////////////////////////////////////////////////////////////////////
// CETraceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The trace manager is a singleton container of all scopes registered
 *          in the system, as well as it is configuring tracing, starts
 *          and stops tracing thread, loading scopes and changes priorities.
 *          Every created scope is registered in Trace Manager and unregistered,
 *          when destroyed. Before system is able to log, the tracing should 
 *          be started (trace thread) and the configuration should be loaded.
 **/
class CETraceManager    : private   CEDispatcherThread
                        , private   IETraceEventConsumer
                        , private   IETraceConfiguration
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

//////////////////////////////////////////////////////////////////////////
// CETraceManager::CETraceScopeMap class declaration
//////////////////////////////////////////////////////////////////////////
    typedef TEHashMapImpl<unsigned int, CETraceScope *>                                                 MapTraceScopeImpl;
    typedef TEHashMap<unsigned int, CETraceScope *, unsigned int, CETraceScope *, MapTraceScopeImpl>    MapTraceScope;
    typedef TEResourceMapImpl<unsigned int, CETraceScope>                                               ImplTraceScope;
    /**
     * \brief   Resource map, container of all logging scopes
     **/
    class CETraceScopeMap   : public TELockResourceMap<unsigned int, CETraceScope, MapTraceScope, ImplTraceScope>
    {
    //////////////////////////////////////////////////////////////////////////
    // CETraceScopeMap friend classes
    //////////////////////////////////////////////////////////////////////////
        friend CETraceManager;
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Constructor.
         **/
        CETraceScopeMap( void );
        /**
         * \brief   Destructor.
         **/
        ~CETraceScopeMap( void );

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        CETraceScopeMap( const CETraceScopeMap & );
        const CETraceScopeMap & operator = ( const CETraceScopeMap & );
    };

    /**
     * \brief   The map of scopes to configure.
     * \tparam  Key     The key is a name of scope
     * \tparam  Value   The value is a digital number of scope.
     **/

    typedef CEStringToIntegerMap                                    ListScopes;
    /**
     * \brief   The list of logging properties
     * \tparam  Value   The value is a property
     **/
    typedef TELinkedList<CETraceProperty, const CETraceProperty &>  ListProperties;

    /**
     * \brief   The thread name of tracer
     **/
    static const char * const   TRACER_THREAD_NAME          /*= "_AREG_TRACER_THREAD_"*/;

    static const char * const   DEFAULT_LOG_VERSION         /*= "log.version = 1.0.0"*/;

    static const char * const   DEFAULT_LOG_ENABLE          /*= "log.enable = true"*/;

    static const char * const   DEFAULT_LOG_FILE            /*= "log.file = ./logs/trace_%time%.log"*/;

    static const char * const   DEFAULT_LOG_APPEND          /*= "log.append = false"*/;

    static const char * const   DEFAULT_LOG_LAYOUT_ENTER    /*= "log.layout.enter = %d: [ %c.%t  %x.%z: Enter --> ]%n"*/;

    static const char * const   DEFAULT_LOG_LAYOUT_MESSAGE  /*= "log.layout.message = %d: [ %c.%t  %p >>> ] %m%n"*/;

    static const char * const   DEFAULT_LOG_LAYOUT_EXIT     /*= "log.layout.exit = %d: [ %c.%t  %x.%z: Exit <-- ]%n"*/;

    static const char * const   DEFAULT_LOG_LAYOUT_DEBUG    /*= "log.debug = true"*/;

    static const char * const   DEFAULT_SCOPES_ENABLED      /*= "scope.* = DEBUG | SCOPE"*/;

public:
    /**
     * \brief   Returns singleton instance of trace scope manager
     **/
    static CETraceManager & GetTraceManager( void );

    /**
     * \brief   Returns the ID of given scope name.
     *          If scope name is NULL or empty, it returns zero.
     * \param   scopeName   The name of scope. If NULL or empty,
     *                      the return value is zero.
     * \return  Returns the ID of given scope name.
     **/
    static unsigned int MakeScopeId( const char * scopeName );

    /**
     * \brief   Sends logging message for logging.
     * \param   logData The logging message object, which will be sent to all loggers.
     **/
    static void SendLogMessage( CELogMessage & logData );

    /**
     * \brief   Call to configure logging. The passed configuration file name should be either
     *          full or relative path to configuration file. If passed NULL,
     *          the default configuration file will be loaded.
     **/
    static bool LoggingConfigure( const char * configFile = NULL );

    /**
     * \brief   Call to initialize and start logging.
     *          The initialization data is read out from specified log configuration file.
     *          If specified file is NULL, the configuration will be read out from
     *          default log configuration.
     * \param   configFile  The full or relative path to configuration file.
     *                      If NULL, the log configuration will be read out
     *                      from default configuration file.
     * \return  Returns true if could read configuration and start logging thread.
     *          If logging was already started, the call will be ignored and return true.
     *          If starting fails, returns false.
     **/
    static bool StartLogging( const char * configFile = NULL );

    /**
     * \brief   Call to stop logging. This call will stop all loggers and exit thread.
     *          The call will be blocked until either logging thread is not stopped,
     *          or the waiting timeout is not expired.
     **/
    static void StopLogging( unsigned int waitTimeout = CEThread::WAIT_INFINITE );

    /**
     * \brief   Registers instance of trace scope object in trace manager.
     *          The trace scope should have unique ID.
     * \param   scope   The instance of trace scope object to register
     **/
    static void RegisterTraceScope( CETraceScope & scope );

    /**
     * \brief   Unregisters instance of trace scope object in trace manager.
     * \param   scope   The instance of trace scope to unregister
     **/
    static void UnregisterTraceScope( CETraceScope & scope );

    /**
     * \brief   Activates trace scope. Finds priority in priority list
     *          and sets scope priority.
     * \param   scope   The instance of trace scope object to activate
     *                  and set logging priority.
     **/
    static void ActivateTraceScope( CETraceScope & scope );

    /**
     * \brief   Returns true if logging has started
     **/
    static inline bool IsLoggingStarted( void );

    /**
     * \brief   Returns true if logging is configured and ready to start
     **/
    static bool IsLoggingConfigured( void );

    /**
     * \brief   Returns true if logging is enabled.
     **/
    static bool IsLoggingEnabled( void );

    /**
     * \brief   Call to force to activate logging with default settings.
     *          The logging will be activated only if logging is not running and
     *          only in debug build. For release, please use real logging configuration.
     * \return  Returns true if could activate logging. For non-debug builds, the function always returns false.
     **/
    static bool ForceActivateLogging( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Protected default constructor.
     **/
    CETraceManager( void );

    /**
     * \brief   Protected destructor.
     **/
    ~CETraceManager( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Registers instance of trace scope object in trace manager.
     *          The trace scope should have unique ID.
     * \param   scopeRegister   The instance trace scope object to register
     **/
    void RegisterScope( CETraceScope & scopeRegister );

    /**
     * \brief   Unregisters instance of trace scope object in trace manager.
     * \param   scopeUnregister The instance of trace scope to unregister
     **/
    void UnregisterScope( CETraceScope & scopeUnregister );

    /**
     * \brief   By given unique scope ID it returns instance of
     *          Trace Scope object. Returns NULL if there is no
     *          trace scope object with specified ID registered 
     *          in the system.
     * \param   scopeId     The unique trace scope ID to lookup in the system
     * \return  Returns valid pointer to Trace Scope object if it is
     *          registered. Otherwise, it returns NULL.
     **/
    inline CETraceScope * GetScope( unsigned int scopeId ) const;

    /**
     * \brief   By given unique scope name it returns instance of
     *          Trace Scope object. Returns NULL if there is no
     *          trace scope object with specified name registered 
     *          in the system.
     * \param   scopeName   The unique trace scope name to lookup in the system
     * \return  Returns valid pointer to Trace Scope object if it is
     *          registered. Otherwise, it returns NULL.
     **/
    inline CETraceScope * GetScope( const char * scopeName ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system with specified unique ID.
     * \param   scopeId     The unique trace scope ID to check in the system.
     * \return  Returns true if there is trace scope with specified unique ID
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool IsScopeRegistered( unsigned int scopeId ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system with specified unique name.
     * \param   scopeName   The unique trace scope name to check in the system.
     * \return  Returns true if there is trace scope with specified unique name
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool IsScopeRegistered( const char * scopeName ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system.
     * \param   scope       The trace scope to check in the system.
     * \return  Returns true if there is specified trace scope
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool IsScopeRegistered( const CETraceScope & scope ) const;

    /**
     * \brief   Returns true if configuration of trace manager is valid.
     *          The trace manager is valid if at least one logger is specified,
     *          even if tracing is disabled or the system has no traces.
     **/
    bool IsValid( void ) const;

    /**
     * \brief   Returns true if logging is enabled.
     **/
    bool IsEnabled( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Trace Scope functionalities
/************************************************************************/

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   newPrio     The priority value to set for trace scope.
     **/
    void SetScopePriority( unsigned int scopeId, unsigned int newPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   newPrio     The name of priority value to set for trace scope.
     **/
    inline void SetScopePriority( unsigned int scopeId, const char * newPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   newPrio     The priority value to set for trace scope.
     **/
    inline void SetScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   newPrio     The name of priority value to set for trace scope.
     **/
    inline void SetScopePriority( const char * scopeName, const char * newPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   addPrio     The priority value to add for trace scope.
     **/
    void AddScopePriority( unsigned int scopeId, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   addPrio     The name of priority value to add for trace scope.
     **/
    inline void AddScopePriority( unsigned int scopeId, const char * addPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   addPrio     The priority value to add for trace scope.
     **/
    inline void AddScopePriority( const char * scopeName, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   addPrio     The name of priority value to add for trace scope.
     **/
    inline void AddScopePriority( const char * scopeName, const char * addPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   remPrio     The priority value to remove for trace scope.
     **/
    void RemoveScopePriority( unsigned int scopeId, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   remPrio     The name of priority value to remove for trace scope.
     **/
    inline void RemoveScopePriority( unsigned int scopeId, const char * remPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   remPrio     The priority value to remove for trace scope.
     **/
    inline void RemoveScopePriority( const char * scopeName, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   remPrio     The name of priority value to remove for trace scope.
     **/
    inline void RemoveScopePriority( const char * scopeName, const char * remPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, sets the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   newPrio         The priority value to set for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int SetScopeGroupPriority( const char * scopeGroupName, unsigned int newPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, sets the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   newPrio         The name of priority value to set for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int SetScopeGroupPriority( const char * scopeGroupName, const char * newPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, adds the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   addPrio         The priority value to add for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int AddScopeGroupPriority( const char * scopeGroupName, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, adds the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   addPrio         The name of priority value to add for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int AddScopeGroupPriority( const char * scopeGroupName, const char * addPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, removes the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   remPrio         The priority value to remove for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int RemoveScopeGroupPriority( const char * scopeGroupName, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, removes the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   remPrio         The name of priority value to remove for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int RemoveScopeGroupPriority( const char * scopeGroupName, const char * remPrio );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:

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
    virtual bool PostEvent( CEEvent & eventElem );

/************************************************************************/
// CEDispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

/************************************************************************/
// IETraceEventConsumer interface overrides
/************************************************************************/
    virtual void ProcessEvent( const CETraceEventData & data );

/************************************************************************/
// IETraceConfiguration interface overrides
/************************************************************************/

    /**
     * \brief   Returns property value of logging version.
     **/
    virtual const CETraceProperty & PropertyVersion( void ) const;

    /**
     * \brief   Returns property value of layout format of scope enter.
     **/
    virtual const CETraceProperty & PropertyLayoutEnter( void ) const;

    /**
     * \brief   Returns property value of layout format form message text output.
     **/
    virtual const CETraceProperty & PropertyLayoutMessage( void ) const;

    /**
     * \brief   Returns property value of layout format of scope exit.
     **/
    virtual const CETraceProperty & PropertyLayoutExit( void ) const;

    /**
     * \brief   Returns property value of stack size.
     **/
    virtual const CETraceProperty & PropertyStackSize( void ) const;

    /**
     * \brief   Returns property value of debug output settings.
     **/
    virtual const CETraceProperty & PropertyDebugOutput( void ) const;

    /**
     * \brief   Returns property value of logging status (enabled / disabled).
     **/
    virtual const CETraceProperty & PropertyStatus( void ) const;

    /**
     * \brief   Returns property value of append data status.
     **/
    virtual const CETraceProperty & PropertyAppendData( void ) const;

    /**
     * \brief   Returns property value of file logging settings.
     **/
    virtual const CETraceProperty & PropertyLogFile( void ) const;

    /**
     * \brief   Returns property value of remote host name logging settings.
     **/
    virtual const CETraceProperty & PropertyRemoteHost( void ) const;

    /**
     * \brief   Returns property value of remote host port logging settings.
     **/
    virtual const CETraceProperty & PropertyRemotePort( void ) const;

    /**
     * \brief   Returns property value of database host name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseHost( void ) const;

    /**
     * \brief   Returns property value of database name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseName( void ) const;

    /**
     * \brief   Returns property value of database driver name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseDriver( void ) const;

    /**
     * \brief   Returns property value of database user name logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabaseUser( void ) const;

    /**
     * \brief   Returns property value of database user password logging settings.
     **/
    virtual const CETraceProperty & PropertyDatabasePassword( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of trace manager.
     **/
    inline CETraceManager & self( void );

    /**
     * \brief   Send log event, which contains specified logging event data
     **/
    void SendLogEvent( const CETraceEventData & data );

/************************************************************************/
// Logging configuration, start / stop
/************************************************************************/

    /**
     * \brief   Loads specified logging configuration file. If specified file is NULL or empty,
     *          the system will use default path to load configuration and scopes.
     * \param   filePath    Relative or absolute path of configuration file.
     * \return  Returns true if succeeded to load configuration file. Otherwise, returns false.
     *          The valid configuration should contain at least one tracing, even if it is disabled.
     **/
    bool LoadConfiguration( const char * filePath = NULL );
    /**
     * \brief   Loads specified logging configuration file. The specified file should be already
     *          opened for reading. If file is not opened for reading, the system will not automatically open file.
     * \param   configFile  The instance of valid configuration file opened for reading.
     * \return  Returns true if succeeded to load configuration file. Otherwise, returns false.
     *          The valid configuration should contain at least one tracing, even if it is disabled.
     **/
    bool LoadConfiguration( const CEFileBase & configFile );

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
    void stopLoggingThread( unsigned int waitTimeout );

    /**
     * \brief   Call if a logging property of specified scopes or scope group has been changed.
     *          For example, if a logging priority is changed, enabled or disabled, or the SCOPE
     *          flag is removed and the state should be saved in configuration file, call method to
     *          update current property to save in configuration file.
     * \param   scopeList   The list of scopes, which are changed property.
     **/
    void TraceChangeScopes( CESortedStringList & scopeList );

    /**
     * \brief   Activates specified scope. The system cannot log if a scope is inactive.
     *          No property of specified scope is changed, the priority remains unchanged.
     * \param   traceScope  The scope to activate.
     **/
    void ActivateScope( CETraceScope & traceScope );

    /**
     * \brief   Activates specified scope and sets specified logging priority. 
     *          The system cannot log if a scope is inactive. The method as well changes logging priority.
     * \param   traceScope  The scope to activate and update logging priority.
     **/
    void ActivateScope( CETraceScope & traceScope, unsigned int defaultPriority );

    /**
     * \brief   Changes the logging priority of specified scope. If found a scope with specified name,
     *          the system changes logging priority
     * \param   scopeName   The name of scope to search in the system to change priority.
     * \param   logPriority The logging priority to set for scope.
     **/
    void SetScopesPriority( const char * scopeName, unsigned int logPriority );

    /**
     * \brief   Returns true, if settings to log traces on remote host are valid.
     **/
    bool IsHostValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in database are valid.
     **/
    bool IsDatabaseValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in file are valid.
     **/
    bool IsFileValid( void ) const;

    /**
     * \brief   Returns true, if settings to log traces in debugging output window are valid.
     **/
    bool IsDebugOutputValid( void ) const;

    /**
     * \brief   Called after loading configuration to initialize configuration objects.
     *          Returns true, if at least one logger is initialized.
     **/
    bool InitializeConfiguration( void );

    /**
     * \brief   Clears logging configuration data.
     **/
    void ClearConfigurationData( void );

    /**
     * \brief   Loads scopes and sets priorities specified in configuration.
     * \return   Returns true, if loading of scopes succeeded.
     **/
    void TraceStartLogs( void );

    /**
     * \brief   Deactivates all scopes to stop logging.
     **/
    void TraceStopLogs( void );

    /**
     * \brief   Logs (traces) the specified message.
     *          The method passes message to every logger to output message.
     * \param   logMessage  The message to log.
     * \note    Currently only logging in output window and file system works.
     *          There should be slight refactoring to be able to register any logger.
     **/
    void TraceMessage( const CELogMessage & logMessage );

    /**
     * \brief   Sets activity flag of every registered scope.
     *          If scopes are inactive, they cannot log anymore.
     * \param   makeActive  The flag, indicating whether the scopes should be active or inactive.
     **/
    void SetScopesActivity( bool makeActive );

    /**
     * \brief   Activated the tracing default settings without reading out the setting from configuration file.
     *          It will enable all scopes and logs of current module using default layout format.
     **/
    bool ActivateTracingDefaults( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The map of registered trace scope objects.
     **/
    CETraceScopeMap mMapTraceScope;
    /**
     * \brief   The flag, indicating whether the scopes are activated or not.
     **/
    bool            mScopesActivated;
    /**
     * \brief   The flag, indicating whether logger is started or not
     **/
    bool            mIsStarted;
    /**
     * \brief   The file path of configuration file.
     **/
    CEString        mConfigFile;
    /**
     * \brief   The property of supported logging version.
     **/
    CETraceProperty mLogVersion;
    /**
     * \brief   The property of enter scope layout
     **/
    CETraceProperty mLogLayoutEnter;
    /**
     * \brief   The property of logging message layout
     **/
    CETraceProperty mLogLayoutMessage;
    /**
     * \brief   The property of exit scope layout
     **/
    CETraceProperty mLogLayoutExit;
    /**
     * \brief   The property of logging stack size, which can be cached by application.
     **/
    CETraceProperty mLogStackSize;
    /**
     * \brief   The property of debug output logger
     **/
    CETraceProperty mLogDebugOutput;
    /**
     * \brief   The property of logging status
     **/
    CETraceProperty mLogStatus;
    /**
     * \brief   The property of appending logs or added to new file
     **/
    CETraceProperty mLogAppendData;
    /**
     * \brief   The property of logging file name format
     **/
    CETraceProperty mLogFile;
    /**
     * \brief   The property of remote logging service host name
     **/
    CETraceProperty mLogRemoteHost;
    /**
     * \brief   The property of remote logging service port number
     **/
    CETraceProperty mLogRemotePort;
    /**
     * \brief   The property of logging database host.
     **/
    CETraceProperty mLogDbHost;
    /**
     * \brief   The property of logging database name
     **/
    CETraceProperty mLogDbName;
    /**
     * \brief   The property of logging driver (engine) name
     **/
    CETraceProperty mLogDbDriver;
    /**
     * \brief   The property of logging database user name
     **/
    CETraceProperty mLogDbUser;
    /**
     * \brief   The property of logging database user password
     **/
    CETraceProperty mLogDbPassword;
    /**
     * \brief   The list of scopes read out from configuration file.
     **/
    ListScopes      mConfigScopeList;
    /**
     * \brief   The list of scope groups read out from configuration file
     **/
    ListScopes      mConfigScopeGroup;
    /**
     * \brief   The list of all properties read out from configuration file.
     **/
    ListProperties  mPropertyList;
    /**
     * \brief   The unique ID of the module.
     **/
    ITEM_ID         mModuleId;
    /**
     * \brief   The file logger object, to output logs in the file.
     **/
    CEFileLogger    mLoggerFile;
    /**
     * \brief   The debug output logger to output logs in the output device (window).
     **/
    CEDebugOutputLogger mLoggerDebug;
    /**
     * \brief   An event, indicating that the logging has been started.
     */
    CESynchEvent    mLogStarted;
    /**
     * \brief   Synchronization object used to synchronize data access.
     **/
    mutable CECriticalSection   mLock;

private:
//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    CETraceManager( const CETraceManager & );
    const CETraceManager & operator = ( const CETraceManager & );
};

//////////////////////////////////////////////////////////////////////////
// CETraceManager class inline functions
//////////////////////////////////////////////////////////////////////////

inline CETraceManager & CETraceManager::self( void )
{   return (*this);                                                                                 }

inline bool CETraceManager::IsLoggingStarted( void )
{   CELock lock(GetTraceManager().mLock); return GetTraceManager().mIsStarted;                      }

inline CETraceScope * CETraceManager::GetScope( unsigned int scopeId ) const
{   return mMapTraceScope.FindResourceObject( scopeId );                                            }

inline CETraceScope * CETraceManager::GetScope(const char * scopeName) const
{   return GetScope( CETraceManager::MakeScopeId(scopeName) );                                      }

inline void CETraceManager::SetScopePriority(unsigned int scopeId, const char * newPrio)
{   SetScopePriority( scopeId, NETrace::FromString(newPrio) );                                      }

inline void CETraceManager::SetScopePriority(const char * scopeName, unsigned int newPrio)
{   SetScopePriority( CETraceManager::MakeScopeId(scopeName), newPrio );                            }

inline void CETraceManager::SetScopePriority(const char * scopeName, const char * newPrio)
{   SetScopePriority( CETraceManager::MakeScopeId(scopeName), NETrace::FromString(newPrio) );       }

inline void CETraceManager::AddScopePriority(unsigned int scopeId, const char * addPrio)
{   AddScopePriority( scopeId, NETrace::FromString(addPrio) );                                      }

inline void CETraceManager::AddScopePriority(const char * scopeName, NETrace::eLogPriority addPrio)
{   AddScopePriority( CETraceManager::MakeScopeId(scopeName), addPrio );                            }

inline void CETraceManager::AddScopePriority(const char * scopeName, const char * addPrio)
{   AddScopePriority( CETraceManager::MakeScopeId(scopeName), NETrace::FromString(addPrio) );       }

inline void CETraceManager::RemoveScopePriority(unsigned int scopeId, const char * remPrio)
{   RemoveScopePriority( scopeId, NETrace::FromString(remPrio) );                                   }

inline void CETraceManager::RemoveScopePriority(const char * scopeName, NETrace::eLogPriority remPrio)
{   RemoveScopePriority( CETraceManager::MakeScopeId(scopeName), remPrio );                         }

inline void CETraceManager::RemoveScopePriority(const char * scopeName, const char * remPrio)
{   RemoveScopePriority( CETraceManager::MakeScopeId(scopeName), NETrace::FromString(remPrio) );    }

inline int CETraceManager::SetScopeGroupPriority(const char * scopeGroupName, const char * newPrio)
{   return SetScopeGroupPriority( scopeGroupName, NETrace::FromString(newPrio) );                   }

inline int CETraceManager::AddScopeGroupPriority(const char * scopeGroupName, const char * addPrio)
{   return AddScopeGroupPriority( scopeGroupName, NETrace::FromString(addPrio) );                   }

inline int CETraceManager::RemoveScopeGroupPriority(const char * scopeGroupName, const char * remPrio)
{   return RemoveScopeGroupPriority(scopeGroupName, NETrace::FromString(remPrio) );                 }

#endif  // AREG_TRACE_PRIVATE_CETRACEMANAGER_HPP
