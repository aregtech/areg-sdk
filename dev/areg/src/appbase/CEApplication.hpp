#ifndef AREG_APPBASE_CEAPPLICATION_HPP
#define AREG_APPBASE_CEAPPLICATION_HPP
/************************************************************************
 * \file        areg/src/appbase/CEApplication.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/appbase/NEApplication.hpp"

#include "areg/src/base/CEString.hpp"
#include "areg/src/base/EContainers.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/ESynchObjects.hpp"

//////////////////////////////////////////////////////////////////////////
// CEApplication class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   An application object containing application start and stop
 *          methods to start Tracer, Service Manager and Router Service.
 *          Call methods to invoke appropriate functions and load modules
 *          required to start and stop asynchronous messaging.
 *          This object is not instantiated in application. Instead,
 *          static methods are called.
 **/
class AREG_API CEApplication
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   The storage type. The application has a named storage where elements are stored
     *          by given names. If a stored element is created by manually allocating memory, the memory
     *          should be as well manually freed.
     **/
    typedef TEStringHashMapImpl<const NEMemory::uAlign>                                     StringHashMapImpl;
    typedef TEStringHashMap<NEMemory::uAlign, const NEMemory::uAlign, StringHashMapImpl>    MapAppStorage;

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to initialize application and start application main modules.
     *          If main modules are already started, the call will be ignored.
     *          The call activates messaging modules, it may start tracing and
     *          message router client, if application requires remote
     *          service interface communication.
     *          Note, no model will be loaded or component started until application is not initialized.
     *
     * \param   startTracing        If true, application will start tracing.
     * \param   startServicing      If true, immediately starts Service Manager.
     *                              This parameter is ignored if startRouter is true.
     * \param   startRouting        If true, application will start message router client for remote messaging.
     * \param   startTimer          If true, application will start timer manager. The timer will also automatically 
     *                              start if the service manager starts.
     * \param   fileTraceConfig     If not NULL, the tracing will be configured, but not started,
     *                              unless startTracing parameter is not true;
     *                              If NULL, startTracing parameter is true and tracing is not configured yet,
     *                              the configuration will be loaded from default config file and tracing will be started;
     *                              If NULL and startTracing parameter is false, no tracing will be configured or started.
     * \param   fileRouterConfig    If not NULL, the Router client will be configured, but not stated,
     *                              unless startRouter parameter is not true;
     *                              If NULL, startRouter parameter is true and Router client is not configured,
     *                              the configuration will be loaded from default config file and Router client will be started;
     *                              If NULL and startRouter is false, no Router client will be configured or started.
     **/
    static void Initialize( bool startTracing   = true
                          , bool startServicing = true
                          , bool startRouting   = true
                          , bool startTimer     = true
                          , const char * fileTraceConfig = NEApplication::DEFAULT_TRACING_CONFIG_FILE
                          , const char * fileRouterConfig= NEApplication::DEFAULT_ROUTER_CONFIG_FILE );

    /**
     * \brief   Call to stop all components and release resources.
     **/
    static void Release( void );

    /**
     * \brief   Call to start particular model registered in application.
     *          The model with specified name should be already registered.
     *          If mode name is NULL or empty, all unloaded models will be loaded.
     * \param   modelName   The unique name of model to load.
     *                      If NULL, all models will be loaded.
     * \return  Returns true could load specified model.
     **/
    static bool StartModel( const char * modelName = NULL_STRING );

    /**
     * \brief   Stops and unloads specified model.
     *          If model name NULL or empty, all models will be unloaded and stopped.
     * \param   modelName   The unique name of model to stop.
     *                      Or NULL if all models should be unloaded and stopped.
     **/
    static void StopModel( const char * modelName = NULL_STRING );

    /**
     * \brief   Checks whether specified model is already loaded and running or not.
     *          The model name should not be NULL or empty.
     * \param   modelName   The unique name of model in application.
     *\return   Returns true is model is loaded. Otherwise, returns false.
     **/
    static bool IsModelLoaded( const char * modelName );

    /**
     * \brief   Sets up the current working directory
     **/
    static void SetupWorkingDirectory( const char * dirPath = NULL_STRING );

    /**
     * \brief   Call to configure tracing. 
     * \param   configFile  Tracing configuration file path. Can also be relative path.
     *                      If NULL, default configuration file will be loaded.
     * \return  Returns true if succeeded to configure tracing.
     **/
    static bool ConfigTracer( const char * configFile = NULL_STRING );

    /**
     * \brief   Call to start tracing.
     * \param   configFile  Absolute or relative path of tracing configuration file.
     *                      If not NULL, the configuration will be loaded and tracing will be started.
     *                      If NULL and tracing is already configured, it will start tracing.
     *                      If NULL and tracing is not configured yet, it will load configuration
     *                      from default configuration file location, configure and start tracing.
     * param    force       If true, if tracing is not configured and/or configuration failed, 
     *                      it will set default values and start tracing without reading
     *                      configuration settings from the file.
     * \return  Returns true if succeeded to start tracing. Starting tracing may fail if
     *          it was not configured or configuration failed.
     **/
    static bool StartTracer( const char * configFile = NULL_STRING, bool force = false );

    /**
     * \brief   Call to stop tracing.
     **/
    static void StopTracer( void );

    /**
     * \brief   Returns true if tracing started with success.
     **/
    static bool IsTracerStarted( void );

    /**
     * \brief   Returns true if tracing is enabled.
     **/
    static bool IsTracerEnabled( void );

    /**
     * \brief   Returns true if tracing is configured.
     **/
    static bool IsTracerConfigured( void );

    /**
     * \brief   Call to configure Router connection.
     * \param   configFile  Absolute or relative path of Router configuration file.
     *                      If NULL, Router configuration will be loaded from default file.
     * \return  Returns true if succeeded to configure Router client.
     **/
    static bool ConfigMessageRouter( const char * configFile = NULL_STRING );

    /**
     * \brief   Call to start Service Manager thread. This will not trigger Router Service client connection.
     * \return  Returns true if succeeded to start Router thread or it is running.
     **/
    static bool StartServiceManager( void );

    /**
     * \brief   Call to start timer manager, so that the components can trigger timers.
     * \return  Returns true if timer manager is running.
     **/
    static bool StartTimerManager( void );

    /**
     * \brief   Call to stop timer manager.
     **/
    static void StopTimerManager( void );

    /**
     * \brief   Configure message routing service.
     * \param   configFile  The message routing configuration file.
     * \return  Returns true if could configure the message routing.
     **/
    static bool ConfigMessageRouterClient( const char * configFile = NULL_STRING );

    /**
     * \brief   Call to start Message Router client. If Service Manager is not started yet, it starts
     *          Service Manager first.
     * \param   configFile  Absolute or relative path of Message Router configuration file.
     *                      If not NULL, the configuration will be loaded and Router client will be started.
     *                      If NULL and Router client is already configured, it will start Router client.
     *                      If NULL and Router client is not configured yet, it will load configuration
     *                      from default configuration file location, configure and start Router client.
     * \return  Returns true if Message Router client successfully started.
     **/
    static bool StartMessageRouterClient( const char * configFile = NULL_STRING );

    /**
     * \brief   Call to start Message Router client. If Service Manager is not started yet, it starts
     *          Service Manager first.
     *          The call overwrites IP-Address and port number set via configuration file.
     * \param   ipAddress   Should be valid IP-Address of remote Router Service.
     * \param   portNr      Should be valid Port Number of remote Router Service.
     * \return  Returns true if Message Router client successfully started.
     **/
    static bool StartMessageRouterClient( const char * ipAddress, unsigned short portNr );

    /**
     * \brief   Call to enable or disable Router Service client.
     *          If Router Service client is already running and it is requested to be disabled,
     *          the service will be stopped. If remote Router Service client is disabled,
     *          no service can be accessed outside of process.
     **/
    static void EnableMessageRouterClient( bool enable );

    /**
     * \brief   Stops Message Router client.
     **/
    static void StopMessageRouterClient( void );

    /**
     * \brief   Returns true, if Message Router client is started.
     **/
    static bool IsServiceManagerStarted( void );

    /**
     * \brief   Returns true, if Message Router client is enabled
     **/
    static bool IsRouterEnabled( void );

    /**
     * \brief   Returns true, if Message Router client is configured
     **/
    static bool IsRouterConfigured( void );

    /**
     * \brief   Call to start routing service on local machine.
     *          To succeed call, the user must have appropriate access rights.
     * \return  Returns true if Message Routing Service successfully started as service.
     **/
    static bool StartRouterService( void );

    /**
     * \brief   Returns true if application successfully connected to Message Routing Service,
     *          and can register or request remote servicing.
     **/
    static bool IsRouterConnected( void );

    /**
     * \brief   Returns true, if it was requested to start tracer.
     *          The tracer starts if any model requests to start it.
     *          Then the flag remains unchanged even if any model requests do not start the tracer.
     **/
    inline static bool IsStartTracerRequested( void );

    /**
     * \brief   Returns true, if it was requested to start Router Service.
     *          The Router Service starts if any model requests to start if.
     *          Then the flag remains unchanged even if any model requests do not start the Router Service.
     **/
    inline static bool IsStartServiceManagerRequested( void );

    /**
     * \brief   Returns the config file name of tracer.
     *          The config file is setup once when any module first requests to setup tracer.
     **/
    inline static const char * GetTracerConfigFile( void );

    /**
     * \brief   Returns the config file name of Router Service.
     *          The config file is setup once when any module first requests to setup Router Service.
     **/
    inline static const char * GetServiceManagerConfigFile( void );

    /**
     * \brief   Returns true if an element exists in the application storage
     * \param   elemName    The name of element stored in application storage.
     **/
    static bool IsElementStored( const CEString & elemName );

    /**
     * \brief   Saves element in application storage. If storage already has element save with specified
     *          name, the function returns stored element. Otherwise, retirns NEMemroy::InvalidElement.
     * \param   elemName    The name of element to save.
     * \param   elem        The element to save in storage.
     * \return  If storage already has an element saved with same name, it returns previously save element.
     *          Otherwise, it returns NEMemory::InvalidElement.
     **/
    static NEMemory::uAlign SaveStorageElement( const CEString & elemName, NEMemory::uAlign elem );

    /**
     * \brief   Returns stored element, which has given name. If element does not exist, returns NEMemory::InvalidElement.
     * \param   elemName    The name of element to search in storage.
     **/
    static NEMemory::uAlign GetStorageElement( const CEString & elemName );

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Requested to start tracer
     **/
    bool            mStartTracer;
    /**
     * \brief   The config file name of Tracer
     **/
    CEString        mConfigTracer;
    /**
     * \brief   Requested to start service manager.
     **/
    bool            mStartService;
    /**
     * \brief   Requested to start timer manager.
     **/
    bool            mStartTimer;
    /**
     * \brief   Requested to start Router Service client. THe service manager should start fits.
     **/
    bool            mStartRouting;
    /**
     * \brief   The config file name of Router Service
     **/
    CEString        mConfigService;
    /**
     * \brief   The application named storage.
     **/
#if _MSC_VER
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    MapAppStorage   mStorage;
#if _MSC_VER
    #pragma warning(default: 4251)
#endif  // _MSC_VER
        /**
     * \brief   Synchronization object
     **/
    CECriticalSection   mLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden. The object cannot be instantiated.
     **/
    CEApplication( void );
    /**
     * \brief   Destructor. Hidden. The object cannot be manually deleted.
     **/
    ~CEApplication( void );

    static CEApplication & _getApplicationInstance( void );

    /**
     * \brief   Operating system specific implementation to start router service on machine.
     * \return  Returns true if succeeded to start service.
     **/
    static bool _startRouterService( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    CEApplication( const CEApplication & /*src*/ );
    const CEApplication & operator = ( const CEApplication & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEApplication class inline methods.
//////////////////////////////////////////////////////////////////////////
inline bool CEApplication::IsStartTracerRequested( void )
{   return CEApplication::_getApplicationInstance().mStartTracer;                       }

inline bool CEApplication::IsStartServiceManagerRequested( void )
{   return CEApplication::_getApplicationInstance().mStartService;                }

inline const char * CEApplication::GetTracerConfigFile( void )
{   return CEApplication::_getApplicationInstance().mConfigTracer.GetBuffer();          }

inline const char * CEApplication::GetServiceManagerConfigFile( void )
{   return CEApplication::_getApplicationInstance().mConfigService.GetBuffer();   }

#endif  // AREG_APPBASE_CEAPPLICATION_HPP
