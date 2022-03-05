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
 * \file        areg/appbase/Application.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/NEApplication.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/SynchObjects.hpp"

#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Dependencies
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Application::eAppState
     *          Describes the application states.
     *          -   Initially, the application is in undefined state.
     *          -   The application is in initialization state, when Service Manager did not started yet and it
     *              initializes any module like tracing, timer, etc.
     *          -   Application is in ready state only when Service Manager is started.
     *          -   Application is in release state when it is going to stop Service Manager.
     **/
    typedef enum class E_AppState
    {
          AppStateStopped       //!< Application state is undefined
        , AppStateInitializing  //!< Application is initializing
        , AppStateReady         //!< Application is ready. The application is ready only when Service Manager runs.
        , AppStateReleasing     //!< Application is releasing.
    } eAppState;

    /**
     * \brief   Call to initialize application and start main services. If necessary, point the service config file path.
     *          The system ignores requests to start service if it is already running.
     *
     * \param   startTracing        If true, application starts Tracing.
     * \param   startServicing      If true, application starts Service Manager. This parameter is ignored if 'startRouting' is true.
     * \param   startRouting        If true, application starts multi-cast router client and Service Manager (if not started).
     * \param   startTimer          If true, application starts timer manager. If Service Managers, Timer Manager automatically starts.
     * \param   fileTraceConfig     If nullptr or empty, configures Tracing from specified file. Default location is './config/log.init' (NEApplication::DEFAULT_TRACING_CONFIG_FILE)
     * \param   fileRouterConfig    If nullptr or empty, configures  Router client from specified file. Default location is './config/router.init' (NEApplication::DEFAULT_ROUTER_CONFIG_FILE).
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
     *          Application::initApplication(true, false);
     *
     *          // In this example start no service, only make configuration of system, read configuration files from
     *          // user home folder.The services can start later:
     *          Application::initApplication(false, false, false, false, "%user%/log.init", "%user%/connect.init");
     **/
    static void initApplication(  bool startTracing   = true
                                , bool startServicing = true
                                , bool startRouting   = true
                                , bool startTimer     = true
                                , const char * fileTraceConfig = NEApplication::DEFAULT_TRACING_CONFIG_FILE.data()
                                , const char * fileRouterConfig= NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() );

    /**
     * \brief   Call to stop all components, unload models, stop services and release resources.
     **/
    static void releaseApplication( void );

    /**
     * \brief   Setup application handlers. In Linux, it sets up the signal handlers.
     **/
    static void setupHandlers( void );

    /**
     * \brief   Call to load and start particular registered model in the system.
     *          If mode name is nullptr or empty, all registered models are started.
     * \param   modelName   The unique name of model to load. If nullptr, all models will be loaded.
     * \return  Returns true could load specified model.
     *
     * \example     Start Model
     *          // In this example the system starts services and starts all models:
     *          Application::initializ();
     *          Application::loadModel();
     **/
    static bool loadModel( const char * modelName = nullptr );

    /**
     * \brief   Stops and unloads specified model. If model name nullptr or empty, all models are stopped and unloaded.
     *          The services like Service Manager, Tracing, Routing and Timer Manager are not stopped.
     * \param   modelName   The unique name of model to stop. Or nullptr if all models should be unloaded and stopped.
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
    static void unloadModel( const char * modelName = nullptr );

    /**
     * \brief   Checks whether specified model is already loaded and running or not.
     *          The model name should not be nullptr or empty.
     * \param   modelName   The unique name of model in application.
     *\return   Returns true is model is loaded. Otherwise, returns false.
     **/
    static bool isModelLoaded( const char * modelName );

    /**
     * \brief   Sets up the current working directory.
     * \param   dirPath     The absolute or relative path to the folder to set
     *                      as a working directory. If nullptr or empty, the
     *                      working directory is the folder of executable.
     **/
    static void setWorkingDirectory( const char * dirPath = nullptr );

    /**
     * \brief   Call to configure tracing. 
     * \param   configFile  Tracing configuration file path. Can also be relative path.
     *                      If nullptr or empty, default configuration file will be 
     *                      loaded (NEApplication::DEFAULT_TRACING_CONFIG_FILE).
     * \return  Returns true if succeeded to configure tracing.
     **/
    static bool tracerConfig( const char * configFile = nullptr );

    /**
     * \brief   Call to start tracing.
     * \param   configFile  Absolute or relative path of tracing configuration file.
     *                      -   If nullptr or empty, and logging is configured, it will start tracing;
     *                      -   If nullptr or empty, and logging is not configured, it will first load
     *                          the default configuration (NEApplication::DEFAULT_TRACING_CONFIG_FILE)
     *                      -   If not nullptr and not empty, it will first load the specified configuration files.
     *                      The loading of configuration is ignored if logging started.
     * param    force       If true, forces load default configuration file (NEApplication::DEFAULT_TRACING_CONFIG_FILE)
      *                     or use logging default settings.
     * \return  Returns true if succeeded to start tracing. Starting tracing may fail if
     *          it was not configured or configuration failed.
     **/
    static bool startTracer( const char * configFile = nullptr, bool force = false );

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
    static bool configMessageRouting( const char * configFile = nullptr );

    /**
     * \brief   Call to start Message Router client. If Service Manager is not started yet, it starts
     *          Service Manager first.
     * \param   configFile  Absolute or relative path of Message Router configuration file.
     *                      If not nullptr, the configuration will be loaded and Router client will be started.
     *                      If nullptr and Router client is already configured, it will start Router client.
     *                      If nullptr and Router client is not configured yet, it will load configuration
     *                      from default configuration file location, configure and start Router client.
     * \return  Returns true if Message Router client successfully started.
     **/
    static bool startMessageRouting( const char * configFile = nullptr );

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
    static bool isElementStored( const std::string& elemName );

    /**
     * \brief   Saves element in application storage. If storage already has element save with specified
     *          name, the function returns stored element. Otherwise, retirns NEMemroy::InvalidElement.
     * \param   elemName    The name of element to save.
     * \param   elem        The element to save in storage.
     * \return  If storage already has an element saved with same name, it returns previously save element.
     *          Otherwise, it returns NEMemory::InvalidElement.
     **/
    static NEMemory::uAlign storeElement( const std::string& elemName, NEMemory::uAlign elem );

    /**
     * \brief   Returns stored element, which has given name. If element does not exist, returns NEMemory::InvalidElement.
     * \param   elemName    The name of element to search in storage.
     **/
    static NEMemory::uAlign getStoredElement( const std::string& elemName );

    /**
     * \brief   Locks the calling thread until either application quit signal is set, or waiting timeout is expired.
     * \param   waitTimeout The waiting timeout for application quit signal.
     *                      If NECommon::WAIT_INFINITE, waits until signal is set.
     * \return  Returns true, if application quit event signal is set. If timeout expired, returns false.
     **/
    static bool waitAppQuit( unsigned int waitTimeout = NECommon::WAIT_INFINITE);

    /**
     * \brief   Sets application quit signal event. So that, the waiting signal thread can be released
     *          to make further clean-ups and exit application.
     **/
    static void signalAppQuit( void );

    /**
     * \brief   Returns true if the Service Manager of application runs.
     **/
    static bool isServicingReady( void );

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating application basic handling is setup. In Linux setup signal handling.
     */
    bool            mSetup;
    /**
     * \brief   Requested to start tracer
     **/
    bool            mStartTracer;
    /**
     * \brief   The config file name of Tracer
     **/
    std::string     mConfigTracer;
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
    std::string     mConfigService;
    /**
     * \brief   The state of application.
     **/
    eAppState       mAppState;
    /**
     * \brief   Exit application event.
     **/
    SynchEvent      mAppQuit;
    /**
     * \brief   Synchronization object
     **/
    ResourceLock    mLock;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The application named storage.
     **/
    std::map<std::string, NEMemory::uAlign> mStorage;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief	The singleton instance of Application object.
     **/
    static Application	_theApplication;

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
    ~Application( void ) = default;

    /**
     * \brief   Returns instance of singleton Application object.
     **/
    static inline Application & getInstance( void );

    /**
     * \brief   Operating system specific implementation to start router service on machine.
     * \return  Returns true if succeeded to start service.
     **/
    static bool _startRouterService( void );

    /**
     * \brief   Sets new state of application. The state can be changed in following sequence:
     *          AppStateUndefined => AppStateInitializing => AppStateReady => AppStateReleasing => AppStateUndefined
     * \param   newState    The new sate of application to set.
     * \return  Returns true if succeeded to change the application state.
     **/
    static bool _setAppState( eAppState newState );

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Application );
};

//////////////////////////////////////////////////////////////////////////
// Application class inline methods.
//////////////////////////////////////////////////////////////////////////
inline bool Application::startTracerRequested( void )
{
    return Application::getInstance().mStartTracer;
}

inline bool Application::startServiceManagerRequested( void )
{
    return Application::getInstance().mStartService;
}

inline const char * Application::getTracerConfigFile( void )
{
    return Application::getInstance().mConfigTracer.c_str();
}

inline const char * Application::getRoutingConfigFile( void )
{
    return Application::getInstance().mConfigService.c_str();
}

inline Application & Application::getInstance( void )
{
    return Application::_theApplication;
}
