/************************************************************************
 * Copyright    (C) 2018-2021, Artak Avetyan (mailto:artak@aregtech.com).
 *              All rights reserved.
 *
 *              This program is distributed to companies and individuals
 *              with opened sources. Optionally, the binaries can be
 *              distributed as well. 
 *
 ************************************************************************/
/************************************************************************
 * \file        areg/appbase/private/Application.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/

#include "areg/appbase/Application.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/trace/NETrace.hpp"

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////

Application & Application::_getApplicationInstance( void )
{
    static Application _theApp;
    return _theApp;
}

Application::Application(void)
    : mStartTracer  ( false )
    , mConfigTracer ( )
    , mStartService ( false )
    , mStartTimer   ( false )
    , mStartRouting ( false )
    , mConfigService( )
    , mAppState     ( Application::AppStateStopped )
    , mAppQuit      (false, false)
    , mLock         ( )
    , mStorage      ( )
{
    ; // do nothing
}

Application::~Application(void)
{
    ; // do nothing
}

void Application::initApplication(  bool startTracing   /*= true */
                                  , bool startServicing /*= true */
                                  , bool startRouting   /*= true */
                                  , bool startTimer     /*= true */
                                  , const char * fileTraceConfig /*= NEApplication::DEFAULT_TRACING_CONFIG_FILE */
                                  , const char * fileRouterConfig/*= NEApplication::DEFAULT_ROUTER_CONFIG_FILE */)
{
    OUTPUT_DBG("Going to initialize application");
    Application::_setAppState(Application::AppStateInitializing);

    Application::setWorkingDirectory( NULL_STRING );
    startTimer = startTimer == false ? startServicing : startTimer;

    if (startTracing)
    {
        Application::startTracer(fileTraceConfig, NEString::isEmpty<char>(fileTraceConfig));
        OUTPUT_DBG("Requested to start tracer with config file [ %s ]", fileTraceConfig != NULL_STRING ? fileTraceConfig : "");
    }
    else if (NEString::isEmpty<char>(fileTraceConfig) == false)
    {
        Application::tracerConfig(fileTraceConfig);
    }

    if ( startTimer )
    {
        OUTPUT_DBG("Starting timer manager");
        Application::startTimerManager();
    }

    if ( startServicing )
    {
        OUTPUT_DBG("Starting service manager");
        Application::startServiceManager();
    }

    if ( startRouting )
    {
        Application::startMessageRouting(fileRouterConfig);
    }
    else if ( NEString::isEmpty<char>( fileRouterConfig ) == false )
    {
        Application::configMessageRouting(fileRouterConfig);
    }

    Application::_getApplicationInstance().mAppQuit.resetEvent();
}

void Application::releaseApplication(void)
{
    Application::_setAppState(Application::AppStateReleasing);

    Application & theApp  = Application::_getApplicationInstance();

    TimerManager::stopTimerManager();
    ComponentLoader::unloadComponentModel( NULL );
    ServiceManager::_stopServiceManager(); // the message routing client is automatically stopped.
    NETrace::stopLogging();

    theApp.mStartTracer     = false;
    theApp.mStartService    = false;
    theApp.mStartTimer      = false;
    theApp.mStartRouting    = false;
    theApp.mConfigTracer    = String::EmptyString;
    theApp.mConfigService   = String::EmptyString;

    Application::_setAppState(Application::AppStateStopped);
}

bool Application::loadModel(const char * modelName /*= NULL */)
{
    return ComponentLoader::loadComponentModel( modelName );
}

void Application::unloadModel(const char * modelName /*= NULL */)
{
    ComponentLoader::unloadComponentModel(modelName);
}

bool Application::isModelLoaded(const char * modelName)
{
    return ComponentLoader::isModelLoaded(modelName);
}

void Application::setWorkingDirectory( const char * dirPath /*= NULL_STRING*/ )
{
    const char * path = NEString::isEmpty<char>(dirPath) ? Process::getInstance().getPath() : dirPath;

#ifdef _DEBUG
    
    if (File::setCurrentDir(path))
    {
        OUTPUT_DBG("Set current directory [ %s ]", path);
    }
    else
    {
        OUTPUT_ERR("No information about current working directory. Ignoring to setup!");
    }

#else
    
    File::setCurrentDir(path);

#endif // _DEBUG
}

bool Application::tracerConfig( const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    Application & theApp  = Application::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? configFile : NEApplication::DEFAULT_TRACING_CONFIG_FILE;

    OUTPUT_DBG("Requested to start tracer configuration [ %s ]", config);

    if ( NETrace::isStarted() == false )
    {
        if (NETrace::configureLoging(config))
        {
            theApp.mConfigTracer = config;
            result = true;
        }
    }
    else
    {
        result = true;
    }

    return result;
}

bool Application::startTracer(const char * configFile /*= NULL_STRING*/, bool force /*= false*/ )
{
    bool result = false;
    Application & theApp  = Application::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_TRACING_CONFIG_FILE : configFile;

    OUTPUT_DBG("Requested to start tracer with config file [ %s ], forcing [ %s ]", config, force ? "TRUE" : "FALSE");

    if (NETrace::isStarted() == false)
    {
        if ( NETrace::startLogging(config) )
        {
            OUTPUT_DBG("Succeeded to start tracer, setting flags and config file name [ %s ]", config);
            theApp.mStartTracer     = true;
            theApp.mConfigTracer    = configFile;
            result = true;
        }
        else if (force)
        {
            OUTPUT_WARN("The tracing is enabled, but there is neither configuration file speficied, nor default exists.");
            OUTPUT_WARN("Forcing to start logging with system default values.");
            if ( NETrace::forceStartLogging() )
            {
                OUTPUT_INFO("Succeeded to start forced tracer!!!");
                theApp.mStartTracer     = true;
                theApp.mConfigTracer    = "";
                result = true;
            }
        }
        else
        {
            OUTPUT_ERR("Failed to start tracer!");
        }
    }
    else
    {
        OUTPUT_INFO("The tracer is already started, ignoring starting");
        result = true;
    }

    return result;
}

void Application::stopTracer(void)
{
    Application & theApp  = Application::_getApplicationInstance( );
    theApp.mStartTracer     = false;
    NETrace::stopLogging();
}

bool Application::isTracerStarted(void)
{
    return NETrace::isStarted();
}

bool Application::isTracerEnabled(void)
{
    return NETrace::isEnabled();
}

bool Application::isTracerConfigured(void)
{
    return NETrace::isConfigured();
}

void Application::stopServiceManager( void )
{
    Application::_setAppState(Application::AppStateReleasing);
    
    if ( ServiceManager::isServiceManagerStarted() )
    {
        ServiceManager::_stopServiceManager();
    }
    
    Application::_setAppState(Application::AppStateStopped);
}

bool Application::startServiceManager( void )
{
    Application::_setAppState(Application::AppStateInitializing);

    bool result = false;
    Application & theApp  = Application::_getApplicationInstance( );
    if ( ServiceManager::isServiceManagerStarted( ) == false )
    {
        if (ServiceManager::_startServiceManager( ))
        {
            Application::startTimerManager();
            result = true;
            theApp.mStartService = true;
            Application::_setAppState(Application::AppStateReady);
        }
        else
        {
            OUTPUT_ERR("Failed to start service manager!");
        }
    }
    else
    {
        result = true;
        Application::_setAppState(Application::AppStateReady);

        OUTPUT_INFO("The service manager has been started, ignoring to start service manager");
        ASSERT( theApp.mStartService );
        ASSERT( theApp.mStartTimer );
    }

    return result;
}

bool Application::startTimerManager( void )
{
    bool result             = false;
    Application & theApp  = Application::_getApplicationInstance( );

    if ( TimerManager::isTimerManagerStarted() == false )
    {
        OUTPUT_DBG("Starting Timer Manager");
        if ( TimerManager::startTimerManager() )
        {
            theApp.mStartTimer  = true;
            result = true;
        }
        else
        {
            OUTPUT_ERR("Failed to start timer manager!");
        }
    }
    else
    {
        result = true;
        OUTPUT_INFO("The timer manager is running, ignoring to start timer manager");
    }

    return result;
}

void Application::stopTimerManager(void)
{
    TimerManager::stopTimerManager();
}

bool Application::startMessageRouting(const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    Application & theApp  = Application::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_ROUTER_CONFIG_FILE : configFile;

    if (Application::isServiceManagerStarted())
    {
        if ( ServiceManager::_isRoutingServiceStarted( ) == false )
        {
            if ( ServiceManager::_routingServiceStart( config ) )
            {
                result = true;
                theApp.mStartRouting    = true;
                theApp.mConfigService   = config;
            }
            else
            {
                OUTPUT_ERR("Failed to start routing service client");
            }
        }
        else
        {
            result = true;
            OUTPUT_INFO("The Router Service client is already started, ignoring start request");
        }
    }
    else
    {
        OUTPUT_WARN("The service manager was not started, cannot start the message routing client!");
    }

    return result;
}

bool Application::configMessageRouting( const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    Application & theApp  = Application::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_ROUTER_CONFIG_FILE : configFile;

    if ( ServiceManager::_isRoutingServiceStarted( ) == false )
    {
        if ( ServiceManager::_routingServiceConfigure( config ) )
        {
            result = true;
            theApp.mConfigService   = config;
        }
        else
        {
            OUTPUT_ERR("Failed to start routing service client");
        }
    }
    else
    {
        result = true;
        OUTPUT_INFO("The Router Service client is already started, ignoring start request");
    }

    return result;
}

bool Application::startMessageRouting( const char * ipAddress, unsigned short portNr )
{
    Application & theApp = Application::_getApplicationInstance( );
    if ( Application::startServiceManager() )
    {
        if ( ServiceManager::_isRoutingServiceStarted( ) == false )
        {
            theApp.mStartRouting = ServiceManager::_routingServiceStart(ipAddress, portNr);
            theApp.mConfigService.clear();
        }
#ifdef _DEBUG
        else
        {
            ASSERT( theApp.mStartRouting );
        }
#endif // _DEBUG
    }

    return theApp.mStartRouting;
}

void Application::stopMessageRouting( void )
{
    Application & theApp = Application::_getApplicationInstance( );
    ServiceManager::_routingServiceStop();
    theApp.mStartRouting = false;
}

void Application::enableMessageRouting( bool enable )
{
    Application & theApp = Application::_getApplicationInstance( );
    theApp.mStartRouting = theApp.mStartRouting && enable ? false : theApp.mStartRouting;

    ServiceManager::_routingServiceEnable(enable);
}

bool Application::isServiceManagerStarted(void)
{
    return ServiceManager::isServiceManagerStarted();
}

bool Application::isRouterConnected( void )
{
    return ServiceManager::_isRoutingServiceStarted();
}

bool Application::isMessageRoutingEnabled(void)
{
    return ServiceManager::_isRoutingServiceEnabled();
}

bool Application::isMessageRoutingConfigured(void)
{
    return ServiceManager::_isRoutingServiceConfigured();
}

bool Application::startRouterService(void)
{
    return Application::_startRouterService();
}

bool Application::isElementStored( const String & elemName )
{
    Application & theApp = Application::_getApplicationInstance();
    Lock lock(theApp.mLock);
    return (theApp.mStorage.find(elemName) != NULL);
}

NEMemory::uAlign Application::storeElement( const String & elemName, NEMemory::uAlign elem )
{
    Application & theApp = Application::_getApplicationInstance( );
    Lock lock( theApp.mLock );

    MAPPOS pos = theApp.mStorage.find(elemName);
    NEMemory::uAlign result = NEMemory::InvalidElement;
    if ( pos != NULL )
    {
        result = theApp.mStorage.valueAtPosition(pos);
        theApp.mStorage.removePosition(pos);
    }
    theApp.mStorage.setAt(elemName, elem, false);
    return result;
}

NEMemory::uAlign Application::getStoredElement( const String & elemName )
{
    Application & theApp = Application::_getApplicationInstance( );
    Lock lock( theApp.mLock );

    MAPPOS pos = theApp.mStorage.find( elemName );
    return (pos != NULL ? theApp.mStorage.valueAtPosition( pos ) : NEMemory::InvalidElement);
}

bool Application::waitAppQuit(unsigned int waitTimeout /*= IESynchObject::WAIT_INFINITE*/)
{
    Application & theApp = Application::_getApplicationInstance( );
    return theApp.mAppQuit.lock(waitTimeout);
}

void Application::signalAppQuit(void)
{
    Application & theApp = Application::_getApplicationInstance( );
    theApp.mAppQuit.setEvent();
}

bool Application::isServicingReady(void)
{
    Application & theApp = Application::_getApplicationInstance();
    return (theApp.mAppState == Application::AppStateReady);
}

bool Application::_setAppState(eAppState newState)
{
    bool result = false;
    Application & theApp = Application::_getApplicationInstance();
    switch (theApp.mAppState)
    {
    case Application::AppStateStopped:
        if (newState == Application::AppStateInitializing)
        {
            theApp.mAppState = Application::AppStateInitializing;
            result = true;
        }
        break;

    case Application::AppStateInitializing:
        if (newState == Application::AppStateReady)
        {
            theApp.mAppState = Application::AppStateReady;
            result = true;
        }
        break;

    case Application::AppStateReady:
        if (newState == Application::AppStateReleasing)
        {
            theApp.mAppState = Application::AppStateReleasing;
            result = true;
        }
        break;

    case Application::AppStateReleasing:
        if (newState == Application::AppStateStopped)
        {
            theApp.mAppState = Application::AppStateStopped;
            result = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    }

    return result;
}
