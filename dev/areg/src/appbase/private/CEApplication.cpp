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
 * \file        areg/src/appbase/private/CEApplication.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/

#include "areg/src/appbase/CEApplication.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/component/CEComponentLoader.hpp"
#include "areg/src/component/NERegistry.hpp"
#include "areg/src/component/private/CEServiceManager.hpp"
#include "areg/src/component/private/CETimerManager.hpp"
#include "areg/src/trace/NETrace.hpp"

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////

CEApplication & CEApplication::_getApplicationInstance( void )
{
    static CEApplication _theApp;
    return _theApp;
}

CEApplication::CEApplication(void)
    : mStartTracer          ( false )
    , mConfigTracer         ( )
    , mStartService         ( false )
    , mStartTimer           ( false )
    , mStartRouting         ( false )
    , mConfigService        ( )
    , mStorage              ( )
    , mLock                 ( )
{
    ; // do nothing
}

CEApplication::~CEApplication(void)
{
    ; // do nothing
}

void CEApplication::Initialize( bool startTracing   /*= true */
                              , bool startServicing /*= true */
                              , bool startRouting   /*= true */
                              , bool startTimer     /*= true */
                              , const char * fileTraceConfig /*= NEApplication::DEFAULT_TRACING_CONFIG_FILE */
                              , const char * fileRouterConfig/*= NEApplication::DEFAULT_ROUTER_CONFIG_FILE */)
{
    OUTPUT_DBG("Going to initialize application");

    CEApplication::SetupWorkingDirectory( NULL_STRING );
    startTimer = startTimer == false ? startServicing : startTimer;

    if (startTracing)
    {
        CEApplication::StartTracer(fileTraceConfig, NEString::isEmpty<char>(fileTraceConfig));
        OUTPUT_DBG("Requested to start tracer with config file [ %s ]", fileTraceConfig != NULL_STRING ? fileTraceConfig : "");
    }
    else if (NEString::isEmpty<char>(fileTraceConfig) == false)
    {
        CEApplication::ConfigTracer(fileTraceConfig);
    }

    if ( startTimer )
    {
        OUTPUT_DBG("Starting timer manager");
        CEApplication::StartTimerManager();
    }

    if ( startServicing )
    {
        OUTPUT_DBG("Starting service manager");
        CEApplication::StartServiceManager();
    }

    if ( startRouting )
    {
        CEApplication::StartMessageRouterClient(fileRouterConfig);
    }
    else if ( NEString::isEmpty<char>( fileRouterConfig ) == false )
    {
        CEApplication::ConfigMessageRouterClient(fileRouterConfig);
    }
}

void CEApplication::Release(void)
{
    CEApplication & theApp  = CEApplication::_getApplicationInstance();

    CETimerManager::StopTimerManager();
    CEComponentLoader::UnloadComponentModel( NULL );
    CEServiceManager::StopServiceManager(); // the message routing client is automatically stopped.
    NETrace::StopLogging();

    theApp.mStartTracer     = false;
    theApp.mStartService    = false;
    theApp.mStartTimer      = false;
    theApp.mStartRouting    = false;
    theApp.mConfigTracer    = CEString::EmptyString;
    theApp.mConfigService   = CEString::EmptyString;
}

bool CEApplication::StartModel(const char * modelName /*= NULL */)
{
    return CEComponentLoader::LoadComponentModel( modelName );
}

void CEApplication::StopModel(const char * modelName /*= NULL */)
{
    CEComponentLoader::UnloadComponentModel(modelName);
}

bool CEApplication::IsModelLoaded(const char * modelName)
{
    return CEComponentLoader::IsModelLoaded(modelName);
}

void CEApplication::SetupWorkingDirectory( const char * dirPath /*= NULL_STRING*/ )
{
    const char * path = NEString::isEmpty<char>(dirPath) ? CEProcess::GetProcess().GetProcessPath() : dirPath;

#ifdef _DEBUG
    
    if (CEFile::SetCurrentDir(path))
    {
        OUTPUT_DBG("Set current directory [ %s ]", path);
    }
    else
    {
        OUTPUT_ERR("No information about current working directory. Ignoring to setup!");
    }

#else
    
    CEFile::SetCurrentDir(path);

#endif // _DEBUG
}

bool CEApplication::ConfigTracer( const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? configFile : NEApplication::DEFAULT_TRACING_CONFIG_FILE;

    OUTPUT_DBG("Requested to start tracer configuration [ %s ]", config);

    if ( NETrace::IsStarted() == false )
    {
        if (NETrace::LogingConfigure(config))
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

bool CEApplication::StartTracer(const char * configFile /*= NULL_STRING*/, bool force /*= false*/ )
{
    bool result = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_TRACING_CONFIG_FILE : configFile;

    OUTPUT_DBG("Requested to start tracer with config file [ %s ], forcing [ %s ]", config, force ? "TRUE" : "FALSE");

    if (NETrace::IsStarted() == false)
    {
        if ( NETrace::StartLogging(config) )
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
            if ( NETrace::ForceStartLogging() )
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

void CEApplication::StopTracer(void)
{
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    theApp.mStartTracer     = false;
    NETrace::StopLogging();
}

bool CEApplication::IsTracerStarted(void)
{
    return NETrace::IsStarted();
}

bool CEApplication::IsTracerEnabled(void)
{
    return NETrace::IsEnabled();
}

bool CEApplication::IsTracerConfigured(void)
{
    return NETrace::IsConfigured();
}

bool CEApplication::ConfigMessageRouter(const char * configFile /*= NULL_STRING*/ )
{
    return( CEServiceManager::IsRoutingServiceClientStarted() == false ? CEServiceManager::RoutingServiceClientConfigure(configFile) : false );
}

bool CEApplication::StartServiceManager( void )
{
    bool result = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    if ( CEServiceManager::IsServiceManagerStarted( ) == false )
    {
        if (CEServiceManager::StartServiceManager( ))
        {
            CEApplication::StartTimerManager();
            result = true;
            theApp.mStartService = true;
        }
        else
        {
            OUTPUT_ERR("Failed to start service manager!");
        }
    }
    else
    {
        result = true;
        OUTPUT_INFO("The service manager has been started, ignoring to start service manager");
        ASSERT( theApp.mStartService );
        ASSERT( theApp.mStartTimer );
    }

    return result;
}

bool CEApplication::StartTimerManager( void )
{
    bool result             = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );

    if ( CETimerManager::IsTimerManagerStarted() == false )
    {
        OUTPUT_DBG("Starting Timer Manager");
        if ( CETimerManager::StartTimerManager() )
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

bool CEApplication::StartMessageRouterClient(const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_ROUTER_CONFIG_FILE : configFile;

    if (CEApplication::IsServiceManagerStarted())
    {
        if ( CEServiceManager::IsRoutingServiceClientStarted( ) == false )
        {
            if ( CEServiceManager::RoutingServiceClientStart( config ) )
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

bool CEApplication::ConfigMessageRouterClient( const char * configFile /*= NULL_STRING*/ )
{
    bool result = false;
    CEApplication & theApp  = CEApplication::_getApplicationInstance( );
    const char * config     = NEString::isEmpty<char>(configFile) ? NEApplication::DEFAULT_ROUTER_CONFIG_FILE : configFile;

    if ( CEServiceManager::IsRoutingServiceClientStarted( ) == false )
    {
        if ( CEServiceManager::RoutingServiceClientConfigure( config ) )
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

bool CEApplication::StartMessageRouterClient( const char * ipAddress, unsigned short portNr )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance( );
    if ( CEApplication::StartServiceManager() )
    {
        if ( CEServiceManager::IsRoutingServiceClientStarted( ) == false )
        {
            theApp.mStartRouting = CEServiceManager::RoutingServiceClientStart(ipAddress, portNr);
            theApp.mConfigService.Clear();
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

void CEApplication::StopMessageRouterClient( void )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance( );
    CEServiceManager::RoutingServiceClientStop();
    theApp.mStartRouting = false;
}

void CEApplication::EnableMessageRouterClient( bool enable )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance( );
    theApp.mStartRouting = theApp.mStartRouting && enable ? false : theApp.mStartRouting;

    CEServiceManager::RoutingServiceClientEnable(enable);
}

bool CEApplication::IsServiceManagerStarted(void)
{
    return CEServiceManager::IsServiceManagerStarted();
}

bool CEApplication::IsRouterConnected( void )
{
    return CEServiceManager::IsRoutingServiceClientStarted();
}

bool CEApplication::IsRouterEnabled(void)
{
    return CEServiceManager::IsRoutingServiceEnabled();
}

bool CEApplication::IsRouterConfigured(void)
{
    return CEServiceManager::IsRoutingServiceClientConfigured();
}

bool CEApplication::StartRouterService(void)
{
    return CEApplication::_startRouterService();
}

bool CEApplication::IsElementStored( const CEString & elemName )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance();
    CELock lock(theApp.mLock);
    return (theApp.mStorage.Find(elemName) != NULL);
}

NEMemory::uAlign CEApplication::SaveStorageElement( const CEString & elemName, NEMemory::uAlign elem )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance( );
    CELock lock( theApp.mLock );

    MAPPOS pos = theApp.mStorage.Find(elemName);
    NEMemory::uAlign result = NEMemory::InvalidElement;
    if ( pos != NULL )
    {
        result = theApp.mStorage.GetValueAt(pos);
        theApp.mStorage.RemoveAt(pos);
    }
    theApp.mStorage.SetKey(elemName, elem, false);
    return result;
}

NEMemory::uAlign CEApplication::GetStorageElement( const CEString & elemName )
{
    CEApplication & theApp = CEApplication::_getApplicationInstance( );
    CELock lock( theApp.mLock );

    MAPPOS pos = theApp.mStorage.Find( elemName );
    return (pos != NULL ? theApp.mStorage.GetValueAt( pos ) : NEMemory::InvalidElement);
}
