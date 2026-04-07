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

#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/private/LogManager.hpp"

#include <vector>
namespace areg {

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

void Application::setup( bool startTracing   /*= true */
                       , bool startServicing /*= true */
                       , bool startRouting   /*= true */
                       , bool startTimer     /*= true */
                       , bool startWatchdog  /*= true */
                       , const char * configFile /*= areg::DEFAULT_CONFIG_FILE */
                       , ConfigListener* configListener /*= nullptr*/)
{
    Application::_set_app_state(areg::AppState::Initializing);
    Application::_os_setup_handlers();
    Application::set_working_directory( nullptr );
    startTimer = startTimer == false ? startServicing : startTimer;

    Application::load_configuration(areg::is_empty(configFile) ? areg::DEFAULT_CONFIG_FILE.data() : configFile, configListener);

    if (startTimer)
    {
        Application::start_timer_manager();
    }

    if (startTracing)
    {
        Application::start_logging(true);
    }

    if ( startWatchdog )
    {
        Application::start_watchdog_manager();
    }

    if ( startServicing )
    {
        Application::start_service_manager();
    }

    if (startRouting)
    {
        Application::start_message_routing(static_cast<uint32_t>(areg::ConnectionType::Tcpip));
    }

    if (Application::instance().mAppState == areg::AppState::Initializing)
    {
        Application::_set_app_state(areg::AppState::Ready);
    }

    Application::instance().mAppQuit.reset();
}

void Application::release()
{
    Application::_set_app_state(areg::AppState::Releasing);

    WatchdogManager::stop_watchdog_manager(false);
    TimerManager::stop_timer_manager(false);
    ComponentLoader::unload_component_model(false, String::EmptyString);
    ServiceManager::_stop_service_manager(false); // the message routing client is automatically stopped.

    WatchdogManager::wait_watchdog_manager();
    TimerManager::wait_timer_manager();
    ComponentLoader::wait_model_unload(String::EmptyString);
    ServiceManager::_wait_service_manager();
    areg::stop_logging(false);
    areg::wait_logging_end();

    Application::_set_app_state(areg::AppState::Stopped);
    Application::_os_release_handlers();
}

bool Application::load_model(const char * modelName /*= nullptr */)
{
    return ComponentLoader::load_component_model( modelName );
}

void Application::unload_model(const char * modelName /*= nullptr */)
{
    ComponentLoader::unload_component_model(true, modelName);
}

bool Application::is_model_loaded(const char * modelName)
{
    return ComponentLoader::is_model_loaded(modelName);
}

const areg::Model & Application::find_model( const char * modelName )
{
    return ComponentLoader::find_model( modelName );
}

void Application::set_working_directory( const char * dirPath /*= nullptr*/ )
{
    String path(areg::is_empty<char>(dirPath) ? Process::instance().path().as_string() : dirPath);
    File::set_current_dir(path);
}

bool Application::start_logging(bool force /*= false*/ )
{
    return areg::is_started() || areg::start_logging() || (force && areg::force_start_logging());
}

void Application::stop_logging()
{
    areg::stop_logging(true);
}

void Application::stop_service_manager()
{
    Application::_set_app_state(areg::AppState::Releasing);
    
    if ( ServiceManager::is_manager_started() )
    {
        ServiceManager::_stop_service_manager(true);
    }
    
    Application::_set_app_state(areg::AppState::Stopped);
}

bool Application::start_service_manager()
{
    Application::_set_app_state(areg::AppState::Initializing);

    bool result = false;

    if ( ServiceManager::is_manager_started( ) == false )
    {
        if (ServiceManager::_start_service_manager( ))
        {
            Application::start_timer_manager();
            Application::start_watchdog_manager();
            result = true;
            Application::_set_app_state(areg::AppState::Ready);
        }
        else
        {
            Application::_set_app_state(areg::AppState::Failure);
        }
    }
    else
    {
        result = true;
        Application::_set_app_state(areg::AppState::Ready);
    }

    return result;
}

bool Application::start_timer_manager()
{
    Application::_os_setup_handlers();
    return (TimerManager::is_manager_started() || TimerManager::start_timer_manager());
}

void Application::stop_timer_manager()
{
    Application::_os_release_handlers();
    TimerManager::stop_timer_manager(true);
}

bool Application::start_watchdog_manager()
{
    return (WatchdogManager::is_manager_started() || WatchdogManager::start_watchdog_manager());
}

void Application::stop_watchdog_manager()
{
    WatchdogManager::stop_watchdog_manager(true);
}

bool Application::start_message_routing(uint32_t connectTypes)
{
    return  Application::start_timer_manager() &&
            Application::is_service_manager_started() && 
            (ServiceManager::_is_routing_started() || ServiceManager::_routing_service_start(connectTypes));
}

bool Application::config_message_routing()
{
    return (ServiceManager::_is_routing_started() || ServiceManager::_routing_service_configure());
}

bool Application::start_message_routing( const char * ipAddress, uint16_t portNr )
{
    return  Application::start_timer_manager() && 
            Application::start_service_manager() && 
            (ServiceManager::_is_routing_started() || ServiceManager::_routing_service_start(ipAddress, portNr));
}

void Application::stop_message_routing()
{
    ServiceManager::_routing_service_stop();
}

bool Application::is_service_manager_started()
{
    return ServiceManager::is_manager_started();
}

bool Application::is_router_connected()
{
    return ServiceManager::_is_routing_started();
}

bool Application::is_router_connection_pending()
{
    return ServiceManager::_is_routing_pending();
}

bool Application::is_message_routing_configured()
{
    return ServiceManager::_is_routing_configured();
}

bool Application::start_router_service()
{
    return Application::_os_start_local_service(areg::ROUTER_SERVICE_NAME_WIDE, areg::ROUTER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::start_logging_service()
{
    return Application::_os_start_local_service(areg::LOGGER_SERVICE_NAME_WIDE, areg::LOGGER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::is_element_stored( const String & elemName )
{
    Application & theApp = Application::instance();
    Lock lock(theApp.mLock);
    return theApp.mStorage.contains(elemName);
}

areg::Primitive Application::store_element( const String & elemName, areg::Primitive elem )
{
    Application & theApp = Application::instance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find(elemName);
    areg::Primitive result = areg::InvalidElement;
    if (theApp.mStorage.is_valid_position(pos))
    {
        result = theApp.mStorage.value_at(pos);
        theApp.mStorage.remove_at(pos);
    }

    theApp.mStorage.set_value_at(elemName, elem);
    return result;
}

areg::Primitive Application::stored_element( const String & elemName )
{
    Application & theApp = Application::instance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find( elemName );
    return (theApp.mStorage.is_valid_position(pos) ? theApp.mStorage.value_at( pos ) : areg::InvalidElement);
}

bool Application::wait_quit(uint32_t waitTimeout /*= areg::WAIT_INFINITE*/)
{
    Application & theApp = Application::instance( );
    return theApp.mAppQuit.lock(waitTimeout);
}

void Application::signal_quit()
{
    Application & theApp = Application::instance( );
    theApp.mAppQuit.set_signaled();
}

bool Application::is_servicing_ready()
{
    Application & theApp = Application::instance();
    return (theApp.mAppState == areg::AppState::Ready);
}

void Application::query_communication_data( uint64_t & sizeSend, uint64_t & sizeReceive )
{
    ServiceManager::query_communication_data( sizeSend, sizeReceive );
}

const String & Application::application_name()
{
    return Process::instance().app_name();
}

const String & Application::machine_name()
{
    return areg::hostname();
}

ConfigManager& Application::config_manager()
{
    return Application::instance().mConfigManager;
}

bool Application::load_configuration(const char* fileName /*= nullptr*/, ConfigListener * listener /*= nullptr*/)
{
    Application& theApp = Application::instance();
    bool result{ true };
    if (theApp.mConfigManager.read_config(fileName == nullptr ? areg::DEFAULT_CONFIG_FILE : fileName, listener) == false)
    {
        result = false;
        Application::setup_default_configuration(listener);
    }

    return result;
}

bool Application::save_configuration(const char* fileName /*= nullptr*/, ConfigListener * /*listener*/ /*= nullptr*/)
{
    Application& theApp = Application::instance();
    return theApp.mConfigManager.save_config(fileName);
}

void Application::setup_default_configuration(ConfigListener * listener /*= nullptr*/)
{
    Application& theApp = Application::instance();
    const String& module = Process::instance().app_name();

    const uint32_t countReadonly{ std::size(areg::DefaultReadonlyProperties) };
    areg::ListProperties defReadonly(countReadonly);
    for (const auto & entry : areg::DefaultReadonlyProperties)
    {
        defReadonly.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, String::EmptyString));
    }

    areg::ListProperties defWritable;
    for (const auto& entry : areg::DefaultLogScopesConfig)
    {
        if (module == entry.configKey.module)
        {
            defWritable.add(Property(entry.configKey.section, entry.configKey.module, entry.configKey.property, entry.configKey.position, entry.configValue, String::EmptyString, false));
        }
    }

    theApp.mConfigManager.set_configuration(defReadonly, defWritable, listener);
}

bool Application::is_configured()
{
    return Application::instance().mConfigManager.is_configured();
}

bool Application::_set_app_state(areg::AppState newState)
{
    bool result = false;
    Application & theApp = Application::instance();
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

} // namespace areg
