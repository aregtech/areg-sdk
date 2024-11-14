#ifndef AREG_APPBASE_APPLICATION_HPP
#define AREG_APPBASE_APPLICATION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/appbase/Application.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/NEApplication.hpp"

#include "areg/base/String.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/persist/ConfigManager.hpp"

 /************************************************************************
  * Dependencies.
  ************************************************************************/
class IEConfigurationListener;

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
    using MapAppStorage     = TEStringHashMap<NEMemory::uAlign>;

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to initialize application and start main services. If necessary, point the service config file path.
     *          The system ignores requests to start service if it is already running.
     *
     * \param   startTracing    If true, application starts Tracing.
     * \param   startServicing  If true, application starts Service Manager. This parameter is ignored if 'startRouting' is true.
     * \param   startRouting    If true, application starts multi-cast router client and Service Manager (if not started).
     * \param   startTimer      If true, application starts timer manager. If Service Managers, Timer Manager automatically starts.
     * \param   startWatchdog   If true, application starts watchdog manager, so that it can track the component threads.
     * \param   configFile      If nullptr or empty, configures Tracing from specified file. Default location is 
     *                          './config/areg.init' (NEApplication::DEFAULT_CONFIG_FILE)
     * \param   configListener  A pointer to the configuration listener. If the pointer is valid, the listener is notified
     *                          before and after loading configuration, as well as if loading configuration fails and the 
     *                          default configuration is set.
     *                          By default, the pointer to listener is null, so that no notification is triggered.
     *                          
     * \see     release, loadModel
     *
     * \example     Initialize Application
     *
     *          // This starts all services and configuration reads from default configuration files:
     *          Application::initApplication();
     *
     *          // In this example, start all services and configuration files from user home folder:
     *          Application::initApplication(true, true, true, true, true, "%user%/areg.init");
     *
     *          // In this example, start all services, even if Service Manager marked 'false', because
     *          // routing requires to start Service Manager first. Read configuration files from default location:
     *          Application::initApplication(true, false);
     *
     *          // In this example start no service, only make configuration of system, read configuration files from
     *          // user home folder.The services can start later:
     *          Application::initApplication(false, false, false, false, false, "%user%/areg.init");
     **/
    static void initApplication(  bool startTracing   = true
                                , bool startServicing = true
                                , bool startRouting   = true
                                , bool startTimer     = true
                                , bool startWatchdog  = false
                                , const char * configFile = NEApplication::DEFAULT_CONFIG_FILE.data()
                                , IEConfigurationListener * configListener = nullptr);

    /**
     * \brief   Call to stop all components, unload models, stop services and release resources.
     **/
    static void releaseApplication( void );

    /**
     * \brief   Call to load and start particular registered model in the system.
     *          If mode name is nullptr or empty, all registered models are started.
     * \param   modelName   The unique name of model to load. If nullptr, all models will be loaded.
     * \return  Returns true could load specified model.
     *
     * \example     Start Model
     *          // In this example the system starts services and starts all models:
     *          Application::initialize();
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
     *          Application::initialize();
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
     * \brief   Searches model by name.
     * \param   modelName   The name of model to search.
     * \return  If found, returns valid model. Otherwise, returns invalid model.
     *          The invalid model has empty name.
     **/
    static const NERegistry::Model & findModel( const char * modelName );

    /**
     * \brief   Sets up the current working directory.
     * \param   dirPath     The absolute or relative path to the folder to set
     *                      as a working directory. If nullptr or empty, the
     *                      working directory is the folder of executable.
     **/
    static void setWorkingDirectory( const char * dirPath = nullptr );

    /**
     * \brief   Call to start logging.
     * param    force       If true, forces load default configuration file (NEApplication::DEFAULT_CONFIG_FILE)
      *                     or use logging default settings.
     * \return  Returns true if succeeded to start logging. Start logging may fail if
     *          it was not configured or configuration failed.
     **/
    static bool startLogging( bool force = false );

    /**
     * \brief   Call to stop tracing.
     **/
    static void stopLogging( void );

    /**
     * \brief   Call to start Service Manager thread. This will not trigger Router Service client connection.
     * \return  Returns true if succeeded to start Router thread or it is running.
     **/
    static bool startServiceManager( void );

    /**
     * \brief   Call to stop Service Manager. When stops, it automatically stops Routing Service connection.
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
     * \brief   Call to start timer manager, so that the components can trigger timers.
     * \return  Returns true if timer manager is running.
     **/
    static bool startWatchdogManager(void);

    /**
     * \brief   Call to stop timer manager.
     **/
    static void stopWatchdogManager(void);

    /**
     * \brief   Returns true, if Message Router client is started.
     **/
    static bool isServiceManagerStarted( void );

    /**
     * \brief   Configure message routing service.
     * \return  Returns true if could configure the message routing.
     **/
    static bool configMessageRouting( void );

    /**
     * \brief   Call to start Message Router client. If Service Manager is not started yet, it starts
     *          Service Manager first.
     * \param   connectTypes    The bitwise set of connections to establish.
     *                          At the moment, only TCP/IP is possible.
     * \return  Returns true if Message Router client successfully started.
     **/
    static bool startMessageRouting(unsigned int connectTypes);

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
     * \brief   Call to start routing service on local machine.
     *          To succeed call, the user must have appropriate access rights.
     * \return  Returns true if Message Routing Service successfully started as service.
     **/
    static bool startLoggingService(void);

    /**
     * \brief   Returns true if application successfully connected to Message Routing Service,
     *          and can register or request remote servicing.
     **/
    static bool isRouterConnected( void );

    /**
     * \brief   Returns true if an element exists in the application storage
     * \param   elemName    The name of element stored in application storage.
     **/
    static bool isElementStored( const String & elemName );

    /**
     * \brief   Saves element in application storage. If storage already has element save with specified
     *          name, the function returns stored element. Otherwise, returns NEMemroy::InvalidElement.
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

    /**
     * \brief   Call to query the amount of send and receive data size in bytes.
     *          The call extracts the send and receive sizes, and resets them to zero.
     *          On output 'sizeSend' and 'sizeReceive' parameters contain the size
     *          since the last call of the method.
     *          If need to measure the total amount of data, accumulate calls.
     *          If need to measure the data rate per second, call this method each second.
     * 
     * \param   sizeSend    On output this parameter contains the size of data in bytes
     *                      sent since the last call of the method.
     * \param   sizeReceive On output this parameter contains the size of data in bytes
     *                      received since the last call of the method.
     **/
    static void queryCommunicationData( unsigned int & OUT sizeSend, unsigned int & OUT sizeReceive );

    /**
     * \brief   Returns the name of the executable process.
     **/
    static const String & getApplicationName(void);

    /**
     * \brief   Returns the name of system host.
     **/
    static const String & getMachineName(void);

    /**
     * \brief   Returns the instance of application configuration initializer object
     *          to read or write configuration properties
     **/
    static ConfigManager& getConfigManager(void);

    /**
     * \brief   Loads the configuration from the given file.
     * \param   fileName    The relative or absolute path to the file to read configurations.
     *                      If nullptr, loads configuration from default file './config/areg.init' (NEApplication::DEFAULT_CONFIG_FILE).
     * \param   listener    The pointer to the configuration listener. If valid, the notifications are triggered before and after
     *                      reading configuration from file. If listener is null or loading configuration failed, no notification is triggered.
     * \return  Returns true if succeeded to load configuration.
     *          If fails, loads default configuration and returns false.
     **/
    static bool loadConfiguration(const char * fileName = nullptr, IEConfigurationListener * listener = nullptr);

    /**
     * \brief   Saves current configuration in the given file.
     *          Note that only writable configuration properties are saved.
     *          The read-only part (global configuration) remains untouched.
     * \param   fileName    The relative or absolute path to the file to write configurations.
     *                      If nullptr, saves configuration in the same file that was read.
     *                      If default configuration was loaded, saves all configuration in the
     *                      default file './config/areg.init' (NEApplication::DEFAULT_CONFIG_FILE).
     * \param   listener    The pointer to the configuration listener. If valid, the notifications are triggered before and after
     *                      saving configuration to file. If listener is null or saving configuration failed, no notification is triggered.
     * \return  Returns true if succeeded to save configuration.
     *          Otherwise, returns false.
     **/
    static bool saveConfiguration(const char * fileName = nullptr, IEConfigurationListener * listener = nullptr);

    /**
     * \brief   Loads default configuration properties defined in NEApplication::DefaultReadonlyProperties
     *          and in the NEApplication::DefaultLogScopesConfig. This will discard the previous configuration
     *          if the application was already configured.
     * \param   listener    The pointer to the configuration listener. If valid, the notifications are triggered if default
     *                      configuration is set. If listener is null, no notification is triggered.
     **/
    static void setupDefaultConfiguration(IEConfigurationListener * listener = nullptr);

    /**
     * \brief   Returns true if the application is already configured.
     **/
    static bool isConfigured(void);

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The state of application.
     **/
    NEApplication::eApplicationState    mAppState;
    /**
     * \brief   Flag, indicating application basic handling is setup. In Linux setup signal handling.
     */
    bool            mSetup;
    /**
     * \brief   The object to read and save application configuration properties.
     **/
    ConfigManager   mConfigManager;
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
    MapAppStorage   mStorage;
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
     * \brief   Sets new state of application. The state can be changed in following sequence:
     *          AppStateUndefined => AppStateInitializing => AppStateReady => AppStateReleasing => AppStateUndefined
     * \param   newState    The new sate of application to set.
     * \return  Returns true if succeeded to change the application state.
     **/
    static bool _setAppState(NEApplication::eApplicationState newState);

    /**
     * \brief   The OS specific implementation of start a service.
     *          If service is not running, the application should have enough access rights
     *          to start a service. Nothing happens if the service is running.
     * \param   serviceName         The name of the service to check and if needed to start.
     * \param   serviceExecutable   The name of the service executable to check.
     * \return  Returns true, if service is running. Otherwise, returns false.
     **/
    static bool _osStartLocalService( const wchar_t * serviceName, const wchar_t * serviceExecutable );

    /**
     * \brief   OS specific implementation to make setups.
     *          In Linux it sets up signal handlers.
     *          In Windows it sets up time period.
     **/
    static void _osSetupHandlers(void);

    /**
     * \brief   OS specific implementation to release resources.
     **/
    static void _osReleaseHandlers(void);

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Application );
};

//////////////////////////////////////////////////////////////////////////
// Application class inline methods.
//////////////////////////////////////////////////////////////////////////

inline Application & Application::getInstance( void )
{
    return Application::_theApplication;
}

#endif  // AREG_APPBASE_APPLICATION_HPP
