#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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
#include "areg/trace/private/TraceEvent.hpp"

#include "areg/trace/private/LogConfiguration.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/TraceProperty.hpp"
#include "areg/trace/private/FileLogger.hpp"
#include "areg/trace/private/DebugOutputLogger.hpp"

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
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

//////////////////////////////////////////////////////////////////////////
// TraceManager::TraceScopeMap class declaration
//////////////////////////////////////////////////////////////////////////
    //!< Scope hash map helper
    using ImplMapTraceScope	= TEHashMapImpl<unsigned int, TraceScope *>;
    //!< Scope hash map
    using MapTraceScope     = TEHashMap<unsigned int, TraceScope *, ImplMapTraceScope>;
    //!< Scope resource map helper
    using ImplTraceScope    = TEResourceMapImpl<unsigned int, TraceScope>;
    /**
     * \brief   Resource map, container of all logging scopes
     **/
    class TraceScopeMap   : public TELockResourceMap<unsigned int, TraceScope, MapTraceScope, ImplTraceScope>
    {
    //////////////////////////////////////////////////////////////////////////
    // TraceScopeMap friend classes
    //////////////////////////////////////////////////////////////////////////
        friend TraceManager;
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Constructor.
         **/
        TraceScopeMap( void ) = default;
        /**
         * \brief   Destructor.
         **/
        ~TraceScopeMap( void ) = default;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        DECLARE_NOCOPY_NOMOVE( TraceScopeMap );
    };

    /**
     * \brief   The map of scopes to configure.
     * \tparam  Key     The key is a name of scope
     * \tparam  Value   The value is a digital number of scope.
     **/

    using ListScopes        = StringToIntegerMap;
    /**
     * \brief   The list of logging properties
     * \tparam  Value   The value is a property
     **/
    using ListProperties    = TELinkedList<TraceProperty>;

    //!< The thread name of tracer
    static constexpr std::string_view   TRACER_THREAD_NAME          { "_AREG_TRACER_THREAD_" };

    //!< All scopes.
    static constexpr std::string_view   LOG_SCOPES_ALL              { "*" };

    //!< Scope indicating AREG SDK internal logs.
    static constexpr std::string_view   LOG_SCOPES_SELF             { "areg_*" };

    //!< Logging activation waiting maximum timeout
    static constexpr unsigned int       LOG_START_WAITING_TIME      { NECommon::WAIT_10_SECONDS };

    //!< Structure of default enabled scopes and priorities.
    typedef struct S_LogEnabling
    {
        const char * const  logScope;   //!< Name of the scope or scope group
        const unsigned int  logPrio;    //!< The logging priority for that scope or scope group
    } sLogEnabling;

    //!< The list scopes or group of scopes and enabled logging priority.
    //!< The last entry in the list must have nullptr instead of name.
    static const sLogEnabling   DEFAULT_LOG_ENABLED_SCOPES[];

public:
    /**
     * \brief   Returns singleton instance of trace scope manager
     **/
    static TraceManager & getInstance( void );

    /**
     * \brief   Returns the ID of given scope name.
     *          If scope name is nullptr or empty, it returns zero.
     * \param   scopeName   The name of scope. If nullptr or empty,
     *                      the return value is zero.
     * \return  Returns the ID of given scope name.
     **/
    static unsigned int makeScopeId( const char * scopeName );

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
    static bool configureLogging( const char * configFile = nullptr );

    /**
     * \brief   Call to initialize and start logging.
     *          The initialization data is read out from specified log configuration file.
     *          If specified file is nullptr, the configuration will be read out from
     *          default log configuration.
     * \param   configFile  The full or relative path to configuration file.
     *                      If nullptr, the log configuration will be read out
     *                      from default configuration file.
     * \param   waitTimeout The timeout in milliseconds to wait until logging starts.
     * \return  Returns true if could read configuration and start logging thread.
     *          If logging was already started, the call will be ignored and return true.
     *          If starting fails, returns false.
     **/
    static bool startLogging( const char * configFile = nullptr, unsigned int waitTimeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Call to stop logging. This call will stop all loggers and exit thread.
     *          The call will be blocked until either logging thread is not stopped,
     *          or the waiting timeout is not expired.
     **/
    static void stopLogging( unsigned int waitTimeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Registers instance of trace scope object in trace manager.
     *          The trace scope should have unique ID.
     * \param   scope   The instance of trace scope object to register
     **/
    static void registerTraceScope( TraceScope & scope );

    /**
     * \brief   Unregisters instance of trace scope object in trace manager.
     * \param   scope   The instance of trace scope to unregister
     **/
    static void unregisterTraceScope( TraceScope & scope );

    /**
     * \brief   Activates trace scope. Finds priority in priority list
     *          and sets scope priority.
     * \param   scope   The instance of trace scope object to activate
     *                  and set logging priority.
     **/
    static void activateTraceScope( TraceScope & scope );

    /**
     * \brief   Returns true if logging has started
     **/
    static inline bool isLoggingStarted( void );

    /**
     * \brief   Returns true if logging is configured and ready to start
     **/
    static bool isLoggingConfigured( void );

    /**
     * \brief   Returns true if logging is enabled.
     **/
    static bool isLoggingEnabled( void );

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
     * \brief   Forces to enable logging.
     **/
    static void forceEnableLogging( void );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Protected default constructor.
     **/
    TraceManager( void );

    /**
     * \brief   Protected destructor.
     **/
    virtual ~TraceManager( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Registers instance of trace scope object in trace manager.
     *          The trace scope should have unique ID.
     * \param   scopeRegister   The instance trace scope object to register
     **/
    void _registerScope( TraceScope & scopeRegister );

    /**
     * \brief   Unregisters instance of trace scope object in trace manager.
     * \param   scopeUnregister The instance of trace scope to unregister
     **/
    void _unregisterScope( TraceScope & scopeUnregister );

    /**
     * \brief   By given unique scope ID it returns instance of
     *          Trace Scope object. Returns nullptr if there is no
     *          trace scope object with specified ID registered 
     *          in the system.
     * \param   scopeId     The unique trace scope ID to lookup in the system
     * \return  Returns valid pointer to Trace Scope object if it is
     *          registered. Otherwise, it returns nullptr.
     **/
    inline TraceScope * _getScope( unsigned int scopeId ) const;

    /**
     * \brief   By given unique scope name it returns instance of
     *          Trace Scope object. Returns nullptr if there is no
     *          trace scope object with specified name registered 
     *          in the system.
     * \param   scopeName   The unique trace scope name to lookup in the system
     * \return  Returns valid pointer to Trace Scope object if it is
     *          registered. Otherwise, it returns nullptr.
     **/
    inline TraceScope * _getScope( const char * scopeName ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system with specified unique ID.
     * \param   scopeId     The unique trace scope ID to check in the system.
     * \return  Returns true if there is trace scope with specified unique ID
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool _isScopeRegistered( unsigned int scopeId ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system with specified unique name.
     * \param   scopeName   The unique trace scope name to check in the system.
     * \return  Returns true if there is trace scope with specified unique name
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool _isScopeRegistered( const char * scopeName ) const;

    /**
     * \brief   Checks whether there is trace scope object registered
     *          in the system.
     * \param   scope       The trace scope to check in the system.
     * \return  Returns true if there is specified trace scope
     *          is registered in the system. Otherwise, it returns false.
     **/
    bool _isScopeRegistered( const TraceScope & scope ) const;

    /**
     * \brief   Returns true if configuration of trace manager is valid.
     *          The trace manager is valid if at least one logger is specified,
     *          even if tracing is disabled or the system has no traces.
     **/
    bool _isValid( void ) const;

    /**
     * \brief   Returns true if logging is enabled.
     **/
    bool _isEnabled( void ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Trace Scope functionalities
/************************************************************************/

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   newPrio     The priority value to set for trace scope.
     **/
    void setScopePriority( unsigned int scopeId, unsigned int newPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   newPrio     The name of priority value to set for trace scope.
     **/
    inline void setScopePriority( unsigned int scopeId, const char * newPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   newPrio     The priority value to set for trace scope.
     **/
    inline void setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          sets the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   newPrio     The name of priority value to set for trace scope.
     **/
    inline void setScopePriority( const char * scopeName, const char * newPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   addPrio     The priority value to add for trace scope.
     **/
    void addScopePriority( unsigned int scopeId, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   addPrio     The name of priority value to add for trace scope.
     **/
    inline void addScopePriority( unsigned int scopeId, const char * addPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   addPrio     The priority value to add for trace scope.
     **/
    inline void addScopePriority( const char * scopeName, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          adds the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   addPrio     The name of priority value to add for trace scope.
     **/
    inline void addScopePriority( const char * scopeName, const char * addPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   remPrio     The priority value to remove for trace scope.
     **/
    void removeScopePriority( unsigned int scopeId, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given unique ID searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeId     The unique ID of trace scope to search in the system.
     * \param   remPrio     The name of priority value to remove for trace scope.
     **/
    inline void removeScopePriority( unsigned int scopeId, const char * remPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   remPrio     The priority value to remove for trace scope.
     **/
    inline void removeScopePriority( const char * scopeName, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given unique name searches trace scope object in the map and if found, 
     *          removes the specified scope priority.
     * \param   scopeName   The unique name of trace scope to search in the system.
     * \param   remPrio     The name of priority value to remove for trace scope.
     **/
    inline void removeScopePriority( const char * scopeName, const char * remPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, sets the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   newPrio         The priority value to set for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int setScopeGroupPriority( const char * scopeGroupName, unsigned int newPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, sets the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   newPrio         The name of priority value to set for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int setScopeGroupPriority( const char * scopeGroupName, const char * newPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, adds the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   addPrio         The priority value to add for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int addScopeGroupPriority( const char * scopeGroupName, NETrace::eLogPriority addPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, adds the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   addPrio         The name of priority value to add for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int addScopeGroupPriority( const char * scopeGroupName, const char * addPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, removes the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   remPrio         The priority value to remove for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    int removeScopeGroupPriority( const char * scopeGroupName, NETrace::eLogPriority remPrio );

    /**
     * \brief   By given name of scope group searches trace scope object in the map and for every
     *          found scope object, removes the specified scope priority.
     * \param   scopeGroupName  The name of trace scope group to search in the system.
     * \param   remPrio         The name of priority value to remove for every trace scope.
     * \return  Returns number of trace scope object, which priority has been changed.
     *          Returns zero, if could not find any trace scope within specified group.
     **/
    inline int removeScopeGroupPriority( const char * scopeGroupName, const char * remPrio );

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

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns instance of trace manager.
     **/
    inline TraceManager & self( void );

    /**
     * \brief   Send log event, which contains specified logging event data
     **/
    void _sendLogEvent( const TraceEventData & data );

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
     * \brief   Loads specified logging configuration file. The specified file should be already
     *          opened for reading. If file is not opened for reading, the system will not automatically open file.
     * \param   configFile  The instance of valid configuration file opened for reading.
     * \return  Returns true if succeeded to load configuration file. Otherwise, returns false.
     *          The valid configuration should contain at least one tracing, even if it is disabled.
     **/
    bool loadConfiguration( const FileBase & configFile );

    /**
     * \brief   Starts logging thread, loads scopes and sets up all tracers.
     *          The configuration should be already loaded.
     * \param   waitTimeout     The timeout in milliseconds to wait until logging starts.
     * \return  Returns true if started with success.
     **/
    bool startLoggingThread( unsigned int waitTimeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Stops logging thread. If needed, it will wait for completion.
     * \param   waitTimeout     Timeout to wait in milliseconds. If zero, it will not wait
     *                          and immediately returns from method. If INFINITE (value 0xFFFFFFFF),
     *                          it will wait until thread completes job and exits.
     **/
    void stopLoggingThread( unsigned int waitTimeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Call if a logging property of specified scopes or scope group has been changed.
     *          For example, if a logging priority is changed, enabled or disabled, or the SCOPE
     *          flag is removed and the state should be saved in configuration file, call method to
     *          update current property to save in configuration file.
     * \param   scopeList   The list of scopes, which are changed property.
     **/
    void onUpdateScopes( SortedStringList & scopeList );

    /**
     * \brief   Activates specified scope. The system cannot log if a scope is inactive.
     *          No property of specified scope is changed, the priority remains unchanged.
     * \param   traceScope  The scope to activate.
     **/
    void activateScope( TraceScope & traceScope );

    /**
     * \brief   Activates specified scope and sets specified logging priority. 
     *          The system cannot log if a scope is inactive. The method as well changes logging priority.
     * \param   traceScope  The scope to activate and update logging priority.
     **/
    void activateScope( TraceScope & traceScope, unsigned int defaultPriority );

    /**
     * \brief   Changes the logging priority of specified scope. If found a scope with specified name,
     *          the system changes logging priority
     * \param   scopeName   The name of scope to search in the system to change priority.
     * \param   logPriority The logging priority to set for scope.
     **/
    void setScopesPriority( const char * scopeName, unsigned int logPriority );

    /**
     * \brief   Returns true, if settings to log traces on remote host are valid.
     **/
    bool isHostValid( void ) const;

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
     * \brief   Called after loading configuration to initialize configuration objects.
     *          Returns true, if at least one logger is initialized.
     **/
    bool initializeConfig( void );

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
     * \brief   Logs (traces) the specified message.
     *          The method passes message to every logger to output message.
     * \param   logMessage  The message to log.
     * \note    Currently only logging in output window and file system works.
     *          There should be slight refactoring to be able to register any logger.
     **/
    void traceMessage( const LogMessage & logMessage );

    /**
     * \brief   Sets activity flag of every registered scope.
     *          If scopes are inactive, they cannot log anymore.
     * \param   makeActive  The flag, indicating whether the scopes should be active or inactive.
     **/
    void setScopesActivity( bool makeActive );

    /**
     * \brief   Activated the tracing default settings without reading out the setting from configuration file.
     *          It will enable all scopes and logs of current module using default layout format.
     **/
    bool activateTracingDefaults( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The map of registered trace scope objects.
     **/
    TraceScopeMap       mMapTraceScope;
    /**
     * \brief   The flag, indicating whether the scopes are activated or not.
     **/
    bool                mScopesActivated;
    /**
     * \brief   The flag, indicating whether logger is started or not
     **/
    bool                mIsStarted;
    /**
     * \brief   The file path of configuration file.
     **/
    String              mConfigFile;
    /**
     * \brief   The logging configuration
     **/
    LogConfiguration    mLogConfig;
    /**
     * \brief   The list of scopes read out from configuration file.
     **/
    ListScopes          mConfigScopeList;
    /**
     * \brief   The list of scope groups read out from configuration file
     **/
    ListScopes          mConfigScopeGroup;
    /**
     * \brief   The list of all properties read out from configuration file.
     **/
    ListProperties      mPropertyList;
    /**
     * \brief   The unique ID of the module.
     **/
    ITEM_ID             mModuleId;
    /**
     * \brief   The file logger object, to output logs in the file.
     **/
    FileLogger          mLoggerFile;
    /**
     * \brief   The debug output logger to output logs in the output device (window).
     **/
    DebugOutputLogger   mLoggerDebug;
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

inline TraceManager & TraceManager::self( void )
{
    return (*this);
}

inline bool TraceManager::isLoggingStarted( void )
{
    Lock lock(getInstance().mLock);
    return getInstance().mIsStarted;
}

inline TraceScope * TraceManager::_getScope( unsigned int scopeId ) const
{
    return mMapTraceScope.findResourceObject( scopeId );
}

inline TraceScope * TraceManager::_getScope(const char * scopeName) const
{
    return _getScope( TraceManager::makeScopeId(scopeName) );
}

inline void TraceManager::setScopePriority(unsigned int scopeId, const char * newPrio)
{
    setScopePriority( scopeId, NETrace::convFromString(newPrio) );
}

inline void TraceManager::setScopePriority(const char * scopeName, unsigned int newPrio)
{
    setScopePriority( TraceManager::makeScopeId(scopeName), newPrio );
}

inline void TraceManager::setScopePriority(const char * scopeName, const char * newPrio)
{
    setScopePriority( TraceManager::makeScopeId(scopeName), NETrace::convFromString(newPrio) );
}

inline void TraceManager::addScopePriority(unsigned int scopeId, const char * addPrio)
{
    addScopePriority( scopeId, NETrace::convFromString(addPrio) );
}

inline void TraceManager::addScopePriority(const char * scopeName, NETrace::eLogPriority addPrio)
{
    addScopePriority( TraceManager::makeScopeId(scopeName), addPrio );
}

inline void TraceManager::addScopePriority(const char * scopeName, const char * addPrio)
{
    addScopePriority( TraceManager::makeScopeId(scopeName), NETrace::convFromString(addPrio) );
}

inline void TraceManager::removeScopePriority(unsigned int scopeId, const char * remPrio)
{
    removeScopePriority( scopeId, NETrace::convFromString(remPrio) );
}

inline void TraceManager::removeScopePriority(const char * scopeName, NETrace::eLogPriority remPrio)
{
    removeScopePriority( TraceManager::makeScopeId(scopeName), remPrio );
}

inline void TraceManager::removeScopePriority(const char * scopeName, const char * remPrio)
{
    removeScopePriority( TraceManager::makeScopeId(scopeName), NETrace::convFromString(remPrio) );
}

inline int TraceManager::setScopeGroupPriority(const char * scopeGroupName, const char * newPrio)
{
    return setScopeGroupPriority( scopeGroupName, NETrace::convFromString(newPrio) );
}

inline int TraceManager::addScopeGroupPriority(const char * scopeGroupName, const char * addPrio)
{
    return addScopeGroupPriority( scopeGroupName, NETrace::convFromString(addPrio) );
}

inline int TraceManager::removeScopeGroupPriority(const char * scopeGroupName, const char * remPrio)
{
    return removeScopeGroupPriority(scopeGroupName, NETrace::convFromString(remPrio) );
}
