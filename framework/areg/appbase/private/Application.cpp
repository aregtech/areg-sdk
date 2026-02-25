/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/appbase/private/Application.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/

#include "areg/appbase/Application.hpp"

#include "areg/base/File.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"

#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/private/LogManager.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////
Application Application::_theApplication;

Application::Application()
    : mAppState     ( areg::AppState::Stopped )
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
                                  , const char * configFile /*= areg::DEFAULT_CONFIG_FILE */
                                  , ConfigListener* configListener /*= nullptr*/)
{
    Application::_setAppState(areg::AppState::Initializing);
    Application::_osSetupHandlers();
    Application::setWorkingDirectory( nullptr );
    startTimer = startTimer == false ? startServicing : startTimer;

    Application::loadConfiguration(areg::isEmpty(configFile) ? areg::DEFAULT_CONFIG_FILE.data() : configFile, configListener);

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
        Application::startMessageRouting(static_cast<uint32_t>(areg::ConnectionType::Tcpip));
    }

    if (Application::getInstance().mAppState == areg::AppState::Initializing)
    {
        Application::_setAppState(areg::AppState::Ready);
    }

    Application::getInstance().mAppQuit.resetEvent();
}

void Application::releaseApplication()
{
    Application::_setAppState(areg::AppState::Releasing);

    areg::WatchdogManager::stopWatchdogManager(false);
    TimerManager::stopTimerManager(false);
    ComponentLoader::unloadComponentModel(false, areg::String::EmptyString);
    ServiceManager::_stopServiceManager(false); // the message routing client is automatically stopped.
    areg::stopLogging(false);

    areg::WatchdogManager::waitWatchdogManager();
    TimerManager::waitTimerManager();
    ComponentLoader::waitModelUnload(areg::String::EmptyString);
    ServiceManager::_waitServiceManager();
    areg::waitLoggingEnd();

    Application::_setAppState(areg::AppState::Stopped);
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

const areg::Model & Application::findModel( const char * modelName )
{
    return ComponentLoader::findModel( modelName );
}

void Application::setWorkingDirectory( const char * dirPath /*= nullptr*/ )
{
    areg::String path( areg::isEmpty<char>(dirPath) ? areg::Process::getInstance().getPath().getString() : dirPath);
    areg::File::setCurrentDir(path);
}

bool Application::startLogging(bool force /*= false*/ )
{
    return areg::isStarted() || areg::startLogging() || (force && areg::forceStartLogging());
}

void Application::stopLogging()
{
    areg::stopLogging(true);
}

void Application::stopServiceManager()
{
    Application::_setAppState(areg::AppState::Releasing);
    
    if ( ServiceManager::isServiceManagerStarted() )
    {
        ServiceManager::_stopServiceManager(true);
    }
    
    Application::_setAppState(areg::AppState::Stopped);
}

bool Application::startServiceManager()
{
    Application::_setAppState(areg::AppState::Initializing);

    bool result = false;

    if ( ServiceManager::isServiceManagerStarted( ) == false )
    {
        if (ServiceManager::_startServiceManager( ))
        {
            Application::startTimerManager();
            Application::startWatchdogManager();
            result = true;
            Application::_setAppState(areg::AppState::Ready);
        }
        else
        {
            Application::_setAppState(areg::AppState::Failure);
        }
    }
    else
    {
        result = true;
        Application::_setAppState(areg::AppState::Ready);
    }

    return result;
}

bool Application::startTimerManager()
{
    Application::_osSetupHandlers();
    return (TimerManager::isTimerManagerStarted() == false ? TimerManager::startTimerManager() : true);
}

void Application::stopTimerManager()
{
    Application::_osReleaseHandlers();
    TimerManager::stopTimerManager(true);
}

bool Application::startWatchdogManager()
{
    return (areg::WatchdogManager::isWatchdogManagerStarted() == false ? areg::WatchdogManager::startWatchdogManager() : true);
}

void Application::stopWatchdogManager()
{
    areg::WatchdogManager::stopWatchdogManager(true);
}

bool Application::startMessageRouting(uint32_t connectTypes)
{
    bool result{ false };

    if (Application::isServiceManagerStarted())
    {
        result = (ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceStart(connectTypes));
    }

    return result;
}

bool Application::configMessageRouting()
{
    return (ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceConfigure());
}

bool Application::startMessageRouting( const char * ipAddress, uint16_t portNr )
{
    bool result{ false };

    if ( Application::startServiceManager() )
    {
        result = ServiceManager::_isRoutingServiceStarted() || ServiceManager::_routingServiceStart(ipAddress, portNr);
    }

    return result;
}

void Application::stopMessageRouting()
{
    ServiceManager::_routingServiceStop();
}

bool Application::isServiceManagerStarted()
{
    return ServiceManager::isServiceManagerStarted();
}

bool Application::isRouterConnected()
{
    return ServiceManager::_isRoutingServiceStarted();
}

bool Application::isRouterConnectionPending()
{
    return ServiceManager::_isRoutingServicePending();
}

bool Application::isMessageRoutingConfigured()
{
    return ServiceManager::_isRoutingServiceConfigured();
}

bool Application::startRouterService()
{
    return Application::_osStartLocalService(areg::ROUTER_SERVICE_NAME_WIDE, areg::ROUTER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::startLoggingService()
{
    return Application::_osStartLocalService(areg::LOGGER_SERVICE_NAME_WIDE, areg::LOGGER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::isElementStored( const areg::String & elemName )
{
    Application & theApp = Application::getInstance();
    Lock lock(theApp.mLock);
    return theApp.mStorage.contains(elemName);
}

areg::Primitive Application::storeElement( const areg::String & elemName, areg::Primitive elem )
{
    Application & theApp = Application::getInstance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find(elemName);
    areg::Primitive result = areg::InvalidElement;
    if (theApp.mStorage.isValidPosition(pos))
    {
        result = theApp.mStorage.valueAtPosition(pos);
        theApp.mStorage.removePosition(pos);
    }

    theApp.mStorage.setAt(elemName, elem);
    return result;
}

areg::Primitive Application::getStoredElement( const areg::String & elemName )
{
    Application & theApp = Application::getInstance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find( elemName );
    return (theApp.mStorage.isValidPosition(pos) ? theApp.mStorage.valueAtPosition( pos ) : areg::InvalidElement);
}

bool Application::waitAppQuit(uint32_t waitTimeout /*= areg::WAIT_INFINITE*/)
{
    Application & theApp = Application::getInstance( );
    return theApp.mAppQuit.lock(waitTimeout);
}

void Application::signalAppQuit()
{
    Application & theApp = Application::getInstance( );
    theApp.mAppQuit.setEvent();
}

bool Application::isServicingReady()
{
    Application & theApp = Application::getInstance();
    return (theApp.mAppState == areg::AppState::Ready);
}

void Application::queryCommunicationData( uint32_t & sizeSend, uint32_t & sizeReceive )
{
    ServiceManager::queryCommunicationData( sizeSend, sizeReceive );
}

const areg::String & Application::getApplicationName()
{
    return areg::Process::getInstance().getAppName();
}

const areg::String & Application::getMachineName()
{
    return areg::getHostname();
}

ConfigManager& Application::getConfigManager()
{
    return Application::getInstance().mConfigManager;
}

bool Application::loadConfiguration(const char* fileName /*= nullptr*/, ConfigListener * listener /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    bool result{ true };
    if (theApp.mConfigManager.readConfig(fileName == nullptr ? areg::DEFAULT_CONFIG_FILE : fileName, listener) == false)
    {
        result = false;
        Application::setupDefaultConfiguration(listener);
    }

    return result;
}

bool Application::saveConfiguration(const char* fileName /*= nullptr*/, ConfigListener * /*listener*/ /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    return theApp.mConfigManager.saveConfig(fileName);
}

void Application::setupDefaultConfiguration(ConfigListener * listener /*= nullptr*/)
{
    Application& theApp = Application::getInstance();
    const areg::String& module = areg::Process::getInstance().getAppName();

    const uint32_t countReadonly{ std::size(areg::DefaultReadonlyProperties) };
    areg::ListProperties defReadonly(countReadonly);
    for (const auto & entry : areg::DefaultReadonlyProperties)
    {
        defReadonly.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, areg::String::EmptyString));
    }

    areg::ListProperties defWritable;
    for (const auto& entry : areg::DefaultLogScopesConfig)
    {
        if (module == entry.configKey.module)
        {
            defWritable.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, areg::String::EmptyString, false));
        }
    }

    theApp.mConfigManager.setConfiguration(defReadonly, defWritable, listener);
}

bool Application::isConfigured()
{
    return Application::getInstance().mConfigManager.isConfigured();
}

bool Application::_setAppState(areg::AppState newState)
{
    bool result = false;
    Application & theApp = Application::getInstance();
    if (newState == areg::AppState::Failure)
    {
        theApp.mAppState = newState;
    }

    switch (theApp.mAppState)
    {
    case areg::AppState::Stopped:
        if (newState == areg::AppState::Initializing)
        {
            theApp.mAppState = areg::AppState::Initializing;
            result = true;
        }
        break;

    case areg::AppState::Initializing:
        if (newState == areg::AppState::Ready)
        {
            theApp.mAppState = areg::AppState::Ready;
            result = true;
        }
        break;

    case areg::AppState::Ready:
        if (newState == areg::AppState::Releasing)
        {
            theApp.mAppState = areg::AppState::Releasing;
            result = true;
        }
        break;

    case areg::AppState::Releasing:
        if (newState == areg::AppState::Stopped)
        {
            theApp.mAppState = areg::AppState::Stopped;
            result = true;
        }
        break;

    case areg::AppState::Failure:
        result = true;
        break; // ignore

    default:
        ASSERT(false);
        break;
    }

    return result;
}
