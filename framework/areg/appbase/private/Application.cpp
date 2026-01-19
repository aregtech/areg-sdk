/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/appbase/private/Application.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/

#include "areg/appbase/Application.hpp"

#include "areg/base/File.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Process.hpp"

#include "areg/component/ComponentLoader.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/logging/NELogging.hpp"
#include "areg/logging/private/LogManager.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////
Application Application::_theApplication;

Application::Application(void)
    : mAppState     ( NEApplication::eApplicationState::AppStateStopped )
    , mSetup        ( false )
    , mConfigManager( )
    , mAppQuit      (false, false)
    , mLock         ( )
    , mStorage      ( )
{
}

void Application::initApplication(  bool startTracing   /*= true */
                                  , bool startServicing /*= true */
                                  , bool startRouting   /*= true */
                                  , bool startTimer     /*= true */
                                  , bool startWatchdog  /*= true */
                                  , const char * configFile /*= NEApplication::DEFAULT_CONFIG_FILE */
                                  , IEConfigurationListener* configListener /*= nullptr*/)
{
    Application::_setAppState(NEApplication::eApplicationState::AppStateInitializing);
    Application::_osSetupHandlers();
    Application::setWorkingDirectory( nullptr );
    startTimer = startTimer == false ? startServicing : startTimer;

    Application::loadConfiguration(NEString::isEmpty(configFile) ? NEApplication::DEFAULT_CONFIG_FILE.data() : configFile, configListener);

    if (startTracing)
    {
        Application::startLogging(true);
    }

    if ( startTimer )
    {
        Application::startTimerManager();
    }

    if ( startWatchdog )
    {
        Application::startWatchdogManager();
    }

    if ( startServicing )
    {
        Application::startServiceManager();
    }

    if (startRouting)
    {
        Application::startMessageRouting(static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectTcpip));
    }

    if (Application::getInstance().mAppState == NEApplication::eApplicationState::AppStateInitializing)
    {
        Application::_setAppState(NEApplication::eApplicationState::AppStateReady);
    }

    Application::getInstance().mAppQuit.resetEvent();
}

void Application::releaseApplication(void)
{
    Application::_setAppState(NEApplication::eApplicationState::AppStateReleasing);

    WatchdogManager::stopWatchdogManager(false);
    TimerManager::stopTimerManager(false);
    ComponentLoader::unloadComponentModel(false, String::EmptyString);
    ServiceManager::_stopServiceManager(false); // the message routing client is automatically stopped.
    NELogging::stopLogging(false);

    WatchdogManager::waitWatchdogManager();
    TimerManager::waitTimerManager();
    ComponentLoader::waitModelUnload(String::EmptyString);
    ServiceManager::_waitServiceManager();
    NELogging::waitLoggingEnd();

    Application::_setAppState(NEApplication::eApplicationState::AppStateStopped);
    Application::_osReleaseHandlers();
}

bool Application::loadModel(const char * modelName /*= nullptr */)
{
    return ComponentLoader::loadComponentModel( modelName );
}

void Application::unloadModel(const char * modelName /*= nullptr */)
{
    ComponentLoader::unloadComponentModel(true, modelName);
}

bool Application::isModelLoaded(const char * modelName)
{
    return ComponentLoader::isModelLoaded(modelName);
}

const NERegistry::Model & Application::findModel( const char * modelName )
{
    return ComponentLoader::findModel( modelName );
}

void Application::setWorkingDirectory( const char * dirPath /*= nullptr*/ )
{
    String path( NEString::isEmpty<char>(dirPath) ? Process::getInstance().getPath().getString() : dirPath);
    File::setCurrentDir(path);
}

bool Application::startLogging(bool force /*= false*/ )
{
    return NELogging::isStarted() || NELogging::startLogging() || (force && NELogging::forceStartLogging());
}

void Application::stopLogging(void)
{
    NELogging::stopLogging(true);
}

void Application::stopServiceManager( void )
{
    Application::_setAppState(NEApplication::eApplicationState::AppStateReleasing);
    
    if ( ServiceManager::isServiceManagerStarted() )
    {
        ServiceManager::_stopServiceManager(true);
    }
    
    Application::_setAppState(NEApplication::eApplicationState::AppStateStopped);
}

bool Application::startServiceManager( void )
{
    Application::_setAppState(NEApplication::eApplicationState::AppStateInitializing);

    bool result = false;

    if ( ServiceManager::isServiceManagerStarted( ) == false )
    {
        if (ServiceManager::_startServiceManager( ))
        {
            Application::startTimerManager();
            Application::startWatchdogManager();
            result = true;
            Application::_setAppState(NEApplication::eApplicationState::AppStateReady);
        }
        else
        {
            Application::_setAppState(NEApplication::eApplicationState::AppStateFailure);
        }
    }
    else
    {
        result = true;
        Application::_setAppState(NEApplication::eApplicationState::AppStateReady);
    }

    return result;
}

bool Application::startTimerManager( void )
{
    Application::_osSetupHandlers();
    return (TimerManager::isTimerManagerStarted() == false ? TimerManager::startTimerManager() : true);
}

void Application::stopTimerManager(void)
{
    Application::_osReleaseHandlers();
    TimerManager::stopTimerManager(true);
}

bool Application::startWatchdogManager(void)
{
    return (WatchdogManager::isWatchdogManagerStarted() == false ? WatchdogManager::startWatchdogManager() : true);
}

void Application::stopWatchdogManager(void)
{
    WatchdogManager::stopWatchdogManager(true);
}

bool Application::startMessageRouting(unsigned int connectTypes)
{
    bool result{ false };

    if (Application::isServiceManagerStarted())
    {
        result = (ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceStart(connectTypes));
    }

    return result;
}

bool Application::configMessageRouting( void )
{
    return (ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceConfigure());
}

bool Application::startMessageRouting( const char * ipAddress, unsigned short portNr )
{
    bool result{ false };

    if ( Application::startServiceManager() )
    {
        result = ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceStart(ipAddress, portNr);
    }

    return result;
}

void Application::stopMessageRouting( void )
{
    ServiceManager::_routingServiceStop();
}

bool Application::isServiceManagerStarted(void)
{
    return ServiceManager::isServiceManagerStarted();
}

bool Application::isRouterConnected( void )
{
    return ServiceManager::_isRoutingServiceStarted();
}

bool Application::isRouterConnectionPending(void)
{
    return ServiceManager::_isRoutingServicePending();
}

bool Application::isMessageRoutingConfigured(void)
{
    return ServiceManager::_isRoutingServiceConfigured();
}

bool Application::startRouterService(void)
{
    return Application::_osStartLocalService(NEApplication::ROUTER_SERVICE_NAME_WIDE, NEApplication::ROUTER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::startLoggingService(void)
{
    return Application::_osStartLocalService(NEApplication::LOGGER_SERVICE_NAME_WIDE, NEApplication::LOGGER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::isElementStored( const String & elemName )
{
    Application & theApp = Application::getInstance();
    Lock lock(theApp.mLock);
    return theApp.mStorage.contains(elemName);
}

NEMemory::uAlign Application::storeElement( const String & elemName, NEMemory::uAlign elem )
{
    Application & theApp = Application::getInstance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find(elemName);
    NEMemory::uAlign result = NEMemory::InvalidElement;
    if (theApp.mStorage.isValidPosition(pos))
    {
        result = theApp.mStorage.valueAtPosition(pos);
        theApp.mStorage.removePosition(pos);
    }

    theApp.mStorage.setAt(elemName, elem);
    return result;
}

NEMemory::uAlign Application::getStoredElement( const String & elemName )
{
    Application & theApp = Application::getInstance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find( elemName );
    return (theApp.mStorage.isValidPosition(pos) ? theApp.mStorage.valueAtPosition( pos ) : NEMemory::InvalidElement);
}

bool Application::waitAppQuit(unsigned int waitTimeout /*= NECommon::WAIT_INFINITE*/)
{
    Application & theApp = Application::getInstance( );
    return theApp.mAppQuit.lock(waitTimeout);
}

void Application::signalAppQuit(void)
{
    Application & theApp = Application::getInstance( );
    theApp.mAppQuit.setEvent();
}

bool Application::isServicingReady(void)
{
    Application & theApp = Application::getInstance();
    return (theApp.mAppState == NEApplication::eApplicationState::AppStateReady);
}

void Application::queryCommunicationData( unsigned int & OUT sizeSend, unsigned int & OUT sizeReceive )
{
    ServiceManager::queryCommunicationData( sizeSend, sizeReceive );
}

const String & Application::getApplicationName(void)
{
    return Process::getInstance().getAppName();
}

const String & Application::getMachineName(void)
{
    return NESocket::getHostname();
}

ConfigManager& Application::getConfigManager(void)
{
    return Application::getInstance().mConfigManager;
}

bool Application::loadConfiguration(const char* fileName /*= nullptr*/, IEConfigurationListener * listener /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    bool result{ true };
    if (theApp.mConfigManager.readConfig(fileName == nullptr ? NEApplication::DEFAULT_CONFIG_FILE : fileName, listener) == false)
    {
        result = false;
        Application::setupDefaultConfiguration(listener);
    }

    return result;
}

bool Application::saveConfiguration(const char* fileName /*= nullptr*/, IEConfigurationListener * /*listener*/ /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    return theApp.mConfigManager.saveConfig(fileName);
}

void Application::setupDefaultConfiguration(IEConfigurationListener * listener /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    const String& module = Process::getInstance().getAppName();

    const uint32_t countReadonly{ MACRO_ARRAYLEN(NEApplication::DefaultReadonlyProperties) };
    NEPersistence::ListProperties defReadonly(countReadonly);
    for (const auto & entry : NEApplication::DefaultReadonlyProperties)
    {
        defReadonly.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, String::EmptyString));
    }

    NEPersistence::ListProperties defWritable;
    for (const auto& entry : NEApplication::DefaultLogScopesConfig)
    {
        if (module == entry.configKey.module)
        {
            defWritable.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, String::EmptyString, false));
        }
    }

    theApp.mConfigManager.setConfiguration(defReadonly, defWritable, listener);
}

bool Application::isConfigured(void)
{
    return Application::getInstance().mConfigManager.isConfigured();
}

bool Application::_setAppState(NEApplication::eApplicationState newState)
{
    bool result = false;
    Application & theApp = Application::getInstance();
    if (newState == NEApplication::eApplicationState::AppStateFailure)
    {
        theApp.mAppState = newState;
    }

    switch (theApp.mAppState)
    {
    case NEApplication::eApplicationState::AppStateStopped:
        if (newState == NEApplication::eApplicationState::AppStateInitializing)
        {
            theApp.mAppState = NEApplication::eApplicationState::AppStateInitializing;
            result = true;
        }
        break;

    case NEApplication::eApplicationState::AppStateInitializing:
        if (newState == NEApplication::eApplicationState::AppStateReady)
        {
            theApp.mAppState = NEApplication::eApplicationState::AppStateReady;
            result = true;
        }
        break;

    case NEApplication::eApplicationState::AppStateReady:
        if (newState == NEApplication::eApplicationState::AppStateReleasing)
        {
            theApp.mAppState = NEApplication::eApplicationState::AppStateReleasing;
            result = true;
        }
        break;

    case NEApplication::eApplicationState::AppStateReleasing:
        if (newState == NEApplication::eApplicationState::AppStateStopped)
        {
            theApp.mAppState = NEApplication::eApplicationState::AppStateStopped;
            result = true;
        }
        break;

    case NEApplication::eApplicationState::AppStateFailure:
        result = true;
        break; // ignore

    default:
        ASSERT(false);
        break;
    }

    return result;
}
