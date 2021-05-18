#ifndef AREG_APPBASE_APPLICATION_HPP
#define AREG_APPBASE_APPLICATION_HPP
/************************************************************************
 * \file        areg/appbase/Application.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/NEApplication.hpp"

#include "areg/base/String.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/ESynchObjects.hpp"

//////////////////////////////////////////////////////////////////////////
// Application class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is a singleton object in each process that is instantiated
 *          when process starts. It is used to initialize, setup and release
 *          system and resources. Use static methods to enable / disable tracing,
 *          service and communication manager of the system.
 **/
class AREG_API Application
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
     * \brief   Call to initialize application and start application main services such as
     *          Tracing, Service Manager, Communication Manager and Timer Manager. If necessary, 
     *          point the the service configuration file path. The system ignores requests to start
     *          service if it is already running.
     *
     * \param   startTracing        If true, application will start Tracing.
     * \param   startServicing      If true, immediately starts Service Manager.
     *                              This parameter is ignored if startRouting is true.
     * \param   startRouting        If true, application will start message router client for remote messaging.
     *                              This service requires Service Manager to run. So that, it will first start
     *                              Service Manager.
     * \param   startTimer          If true, application will start timer manager. The timer will also automatically 
     *                              start if the service manager starts.
     * \param   fileTraceConfig     If not NULL, configures Tracing from specified configuration file. By default,
     *                              the configuration file location, which is './config/log.init'
     *                              (NEApplication::DEFAULT_TRACING_CONFIG_FILE).
     * \param   fileRouterConfig    If not NULL, the Router client is configured from specified configuration file.
     *                              By default, the router configuration file location is './config/mcrouter.init'.
     * \see     release, loadModel
     *
     * \example     Initialize Application
     *
     *          // This starts all services and configuration reads from default configuration files:
     *          Application::initApplication();
     *
     *          // In this example, start all services and configuration files from user home folder:
     *          Application::initApplication(true, true, true, true, "%user%/log.init", "%user%/mcrouter.init");
     *
     *          // In this example, start all services, even if Service Manager marked 'false', because
     *          // routing requires to start Service Manager first. Read configuration files from default location:
     *          Application::initializ(true, false);
     *
     *          // In this example start no service, only make configuration of system, read configuration files from
     *          // user home folder.The services can start later:
     *          Application::initApplication(false, false, false, false, "%user%/log.init", "%user%/connect.init");
     **/
    static void initApplication(  bool startTracing   = true
                                , bool startServicing = true
                                , bool startRouting   = true
                                , bool startTimer     = true
                                , const char * fileTraceConfig = NEApplication::DEFAULT_TRACING_CONFIG_FILE
                                , const char * fileRouterConfig= NEApplication::DEFAULT_ROUTER_CONFIG_FILE );

    /**
     * \brief   Call to stop all components, unload models, stop services and release resources.
     **/
    static void releaseApplication( void );

    /**
     * \brief   Call to load and start particular registered model in the system.
     *          If mode name is NULL or empty, all registered models are started.
     * \param   modelName   The unique name of model to load. If NULL, all models will be loaded.
     * \return  Returns true could load specified model.
     *
     * \example     Start Model
     *          // In this example the system starts services and starts all models:
     *          Application::initializ();
     *          Application::loadModel();
     **/
    static bool loadModel( const char * modelName = NULL_STRING );

    /**
     * \brief   Stops and unloads specified model. If model name NULL or empty, all models are stopped and unloaded.
     *          The services like Service Manager, Tracing, Routing and Timer Manager are not stopped.
     * \param   modelName   The unique name of model to stop. Or NULL if all models should be unloaded and stopped.
     * 
     * \example     Start and Stop all models
     *          // This example starts all services, loads all models. 1 second later stops and unloads all models,
     *          // stop all services, release all resources:
     *          Application::initiali();
     *          Application::loadModel();
     *          Thread::sleep(1000);
     *          Application::unloadModel();
     *          Application::release();
     **/
    static void unloadModel( const char * modelName = NULL_STRING );

    /**
     * \brief   Checks whether specified model is already loaded and running or not.
     *          The model name should not be NULL or empty.
     * \param   modelName   The unique name of model in application.
     *\return   Returns true is model is loaded. Otherwise, returns false.
     **/
    static bool isModelLoaded( const char * modelName );

    /**
     * \brief   Sets up the current working directory
     **/
    static void setWorkingDirectory( const char * dirPath = NULL_STRING );

    /**
     * \brief   Call to configure tracing. 
     * \param   configFile  Tracing configuration file path. Can also be relative path.
     *                      If NULL, default configuration file will be loaded.
     * \return  Returns true if succeeded to configure tracing.
     **/
    static bool tracerConfig( const char * configFile = NULL_STRING );

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
    static bool startTracer( const char * configFile = NULL_STRING, bool force = false );

    /**
     * \brief   Call to stop tracing.
     **/
    static void stopTracer( void );

    /**
     * \brief   Returns true if tracing started with success.
     **/
    static bool isTracerStarted( void );

    /**
     * \brief   Returns true if tracing is enabled.
     **/
    static bool isTracerEnabled( void );

    /**
     * \brief   Returns true if tracing is configured.
     **/
    static bool isTracerConfigured( void );

    /**
     * \brief   Call to start Service Manager thread. This will not trigger Router Service client connection.
     * \return  Returns true if succeeded to start Router thread or it is running.
     **/
    static bool startServiceManager( void );

    /**
     * \brief   Call to stop Service Manager. When stops, it automatically stops Routing Service connection.
     * \return  Returns true if succeeded to start service manager.
     **/
    static void stopServiceManager( void );

    /**
     * \brief   Call to start timer manager, so that the components can trigger timers.
     * \return  Returns true if timer manager is running.
     **/
    static bool startTimerManager( void );

    /**
     * \brief   Call to stop timer manager.
     **/
    static void stopTimerManager( void );

    /**
     * \brief   Returns true, if Message Router client is started.
     **/
    static bool isServiceManagerStarted( void );

    /**
     * \brief   Configure message routing service.
     * \param   configFile  The message routing configuration file.
     * \return  Returns true if could configure the message routing.
     **/
    static bool configMessageRouting( const char * configFile = NULL_STRING );

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
    static bool startMessageRouting( const char * configFile = NULL_STRING );

    /**
     * \brief   Call to start Message Router client. If Service Manager is not started yet, it starts
     *          Service Manager first.
     *          The call overwrites IP-Address and port number set via configuration file.
     * \param   ipAddress   Should be valid IP-Address of remote Router Service.
     * \param   portNr      Should be valid Port Number of remote Router Service.
     * \return  Returns true if Message Router client successfully started.
     **/
    static bool startMessageRouting( const char * ipAddress, unsigned short portNr );

    /**
     * \brief   Stops Message Router client.
     **/
    static void stopMessageRouting( void );

    /**
     * \brief   Call to enable or disable Router Service client.
     *          If Router Service client is already running and it is requested to be disabled,
     *          the service will be stopped. If remote Router Service client is disabled,
     *          no service can be accessed outside of process.
     **/
    static void enableMessageRouting( bool enable );

    /**
     * \brief   Returns true, if Message Router client is enabled
     **/
    static bool isMessageRoutingEnabled( void );

    /**
     * \brief   Returns true, if Message Router client is configured
     **/
    static bool isMessageRoutingConfigured( void );

    /**
     * \brief   Call to start routing service on local machine.
     *          To succeed call, the user must have appropriate access rights.
     * \return  Returns true if Message Routing Service successfully started as service.
     **/
    static bool startRouterService( void );

    /**
     * \brief   Returns true if application successfully connected to Message Routing Service,
     *          and can register or request remote servicing.
     **/
    static bool isRouterConnected( void );

    /**
     * \brief   Returns true, if it was requested to start tracer.
     *          The tracer starts if any model requests to start it.
     *          Then the flag remains unchanged even if any model requests do not start the tracer.
     **/
    inline static bool startTracerRequested( void );

    /**
     * \brief   Returns true, if it was requested to start Router Service.
     *          The Router Service starts if any model requests to start if.
     *          Then the flag remains unchanged even if any model requests do not start the Router Service.
     **/
    inline static bool startServiceManagerRequested( void );

    /**
     * \brief   Returns the config file name of tracer.
     *          The config file is setup once when any module first requests to setup tracer.
     **/
    inline static const char * getTracerConfigFile( void );

    /**
     * \brief   Returns the config file name of Router Service.
     *          The config file is setup once when any module first requests to setup Router Service.
     **/
    inline static const char * getRoutingConfigFile( void );

    /**
     * \brief   Returns true if an element exists in the application storage
     * \param   elemName    The name of element stored in application storage.
     **/
    static bool isElementStored( const String & elemName );

    /**
     * \brief   Saves element in application storage. If storage already has element save with specified
     *          name, the function returns stored element. Otherwise, retirns NEMemroy::InvalidElement.
     * \param   elemName    The name of element to save.
     * \param   elem        The element to save in storage.
     * \return  If storage already has an element saved with same name, it returns previously save element.
     *          Otherwise, it returns NEMemory::InvalidElement.
     **/
    static NEMemory::uAlign storeElement( const String & elemName, NEMemory::uAlign elem );

    /**
     * \brief   Returns stored element, which has given name. If element does not exist, returns NEMemory::InvalidElement.
     * \param   elemName    The name of element to search in storage.
     **/
    static NEMemory::uAlign getStoredElement( const String & elemName );

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
    String          mConfigTracer;
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
    String          mConfigService;
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
    CriticalSection   mLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden. The object cannot be instantiated.
     **/
    Application( void );
    /**
     * \brief   Destructor. Hidden. The object cannot be manually deleted.
     **/
    ~Application( void );

    /**
     * \brief   Returns instance of singleton Application object.
     **/
    static Application & _getApplicationInstance( void );

    /**
     * \brief   Operating system specific implementation to start router service on machine.
     * \return  Returns true if succeeded to start service.
     **/
    static bool _startRouterService( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    Application( const Application & /*src*/ );
    const Application & operator = ( const Application & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Application class inline methods.
//////////////////////////////////////////////////////////////////////////
inline bool Application::startTracerRequested( void )
{
    return Application::_getApplicationInstance().mStartTracer;
}

inline bool Application::startServiceManagerRequested( void )
{
    return Application::_getApplicationInstance().mStartService;
}

inline const char * Application::getTracerConfigFile( void )
{
    return Application::_getApplicationInstance().mConfigTracer.getString();
}

inline const char * Application::getRoutingConfigFile( void )
{
    return Application::_getApplicationInstance().mConfigService.getString();
}

#endif  // AREG_APPBASE_APPLICATION_HPP
