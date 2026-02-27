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

//////////////////////////////////////////////////////////////////////////
// Constants and types
//////////////////////////////////////////////////////////////////////////
Application Application::_theApplication;

Application::Application()
    : mAppState     ( NEApplication::AppState::Stopped )
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
                       , bool start_timer     /*= true */
                       , bool startWatchdog  /*= true */
                       , const char * configFile /*= NEApplication::DEFAULT_CONFIG_FILE */
                       , ConfigListener* configListener /*= nullptr*/)
{
    Application::_set_app_state(NEApplication::AppState::Initializing);
    Application::_os_setup_handlers();
    Application::set_working_directory( nullptr );
    start_timer = start_timer == false ? startServicing : start_timer;

    Application::load_configuration(NEString::is_empty(configFile) ? NEApplication::DEFAULT_CONFIG_FILE.data() : configFile, configListener);

    if (startTracing)
    {
        Application::start_logging(true);
    }

    if ( start_timer )
    {
        Application::start_timer_manager();
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
        Application::start_message_routing(static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip));
    }

    if (Application::instance().mAppState == NEApplication::AppState::Initializing)
    {
        Application::_set_app_state(NEApplication::AppState::Ready);
    }

    Application::instance().mAppQuit.reset();
}

void Application::release()
{
    Application::_set_app_state(NEApplication::AppState::Releasing);

    WatchdogManager::stop_watchdog_manager(false);
    TimerManager::stop_timer_manager(false);
    ComponentLoader::unload_component_model(false, String::EmptyString);
    ServiceManager::_stop_service_manager(false); // the message routing client is automatically stopped.
    NELogging::stop_logging(false);

    WatchdogManager::wait_watchdog_manager();
    TimerManager::wait_timer_manager();
    ComponentLoader::wait_model_unload(String::EmptyString);
    ServiceManager::_wait_service_manager();
    NELogging::wait_logging_end();

    Application::_set_app_state(NEApplication::AppState::Stopped);
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

const NERegistry::Model & Application::find_model( const char * modelName )
{
    return ComponentLoader::find_model( modelName );
}

void Application::set_working_directory( const char * dirPath /*= nullptr*/ )
{
    String path( NEString::is_empty<char>(dirPath) ? Process::instance().path().as_string() : dirPath);
    File::set_current_dir(path);
}

bool Application::start_logging(bool force /*= false*/ )
{
    return NELogging::is_started() || NELogging::start_logging() || (force && NELogging::force_start_logging());
}

void Application::stop_logging()
{
    NELogging::stop_logging(true);
}

void Application::stop_service_manager()
{
    Application::_set_app_state(NEApplication::AppState::Releasing);
    
    if ( ServiceManager::is_manager_started() )
    {
        ServiceManager::_stop_service_manager(true);
    }
    
    Application::_set_app_state(NEApplication::AppState::Stopped);
}

bool Application::start_service_manager()
{
    Application::_set_app_state(NEApplication::AppState::Initializing);

    bool result = false;

    if ( ServiceManager::is_manager_started( ) == false )
    {
        if (ServiceManager::_start_service_manager( ))
        {
            Application::start_timer_manager();
            Application::start_watchdog_manager();
            result = true;
            Application::_set_app_state(NEApplication::AppState::Ready);
        }
        else
        {
            Application::_set_app_state(NEApplication::AppState::Failure);
        }
    }
    else
    {
        result = true;
        Application::_set_app_state(NEApplication::AppState::Ready);
    }

    return result;
}

bool Application::start_timer_manager()
{
    Application::_os_setup_handlers();
    return (TimerManager::is_manager_started() == false ? TimerManager::start_timer_manager() : true);
}

void Application::stop_timer_manager()
{
    Application::_os_release_handlers();
    TimerManager::stop_timer_manager(true);
}

bool Application::start_watchdog_manager()
{
    return (WatchdogManager::is_manager_started() == false ? WatchdogManager::start_watchdog_manager() : true);
}

void Application::stop_watchdog_manager()
{
    WatchdogManager::stop_watchdog_manager(true);
}

bool Application::start_message_routing(uint32_t connectTypes)
{
    bool result{ false };

    if (Application::is_service_manager_started())
    {
        result = (ServiceManager::_is_routing_started() || ServiceManager::_routing_service_start(connectTypes));
    }

    return result;
}

bool Application::config_message_routing()
{
    return (ServiceManager::_is_routing_started() || ServiceManager::_routing_service_configure());
}

bool Application::start_message_routing( const char * ipAddress, uint16_t portNr )
{
    bool result{ false };

    if ( Application::start_service_manager() )
    {
        result = ServiceManager::_is_routing_started() || ServiceManager::_routing_service_start(ipAddress, portNr);
    }

    return result;
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
    return Application::_os_start_local_service(NEApplication::ROUTER_SERVICE_NAME_WIDE, NEApplication::ROUTER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::start_logging_service()
{
    return Application::_os_start_local_service(NEApplication::LOGGER_SERVICE_NAME_WIDE, NEApplication::LOGGER_SERVICE_EXECUTABLE_WIDE);
}

bool Application::is_element_stored( const String & elemName )
{
    Application & theApp = Application::instance();
    Lock lock(theApp.mLock);
    return theApp.mStorage.contains(elemName);
}

NEMemory::Primitive Application::store_element( const String & elemName, NEMemory::Primitive elem )
{
    Application & theApp = Application::instance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find(elemName);
    NEMemory::Primitive result = NEMemory::InvalidElement;
    if (theApp.mStorage.is_valid_position(pos))
    {
        result = theApp.mStorage.value_at_position(pos);
        theApp.mStorage.remove_position(pos);
    }

    theApp.mStorage.set_at(elemName, elem);
    return result;
}

NEMemory::Primitive Application::stored_element( const String & elemName )
{
    Application & theApp = Application::instance( );
    Lock lock( theApp.mLock );

    MapAppStorage::MAPPOS pos = theApp.mStorage.find( elemName );
    return (theApp.mStorage.is_valid_position(pos) ? theApp.mStorage.value_at_position( pos ) : NEMemory::InvalidElement);
}

bool Application::wait_quit(uint32_t waitTimeout /*= NECommon::WAIT_INFINITE*/)
{
    Application & theApp = Application::instance( );
    return theApp.mAppQuit.lock(waitTimeout);
}

void Application::signal_quit()
{
    Application & theApp = Application::instance( );
    theApp.mAppQuit.set_event();
}

bool Application::is_servicing_ready()
{
    Application & theApp = Application::instance();
    return (theApp.mAppState == NEApplication::AppState::Ready);
}

void Application::query_communication_data( uint32_t & sizeSend, uint32_t & sizeReceive )
{
    ServiceManager::query_communication_data( sizeSend, sizeReceive );
}

const String & Application::application_name()
{
    return Process::instance().app_name();
}

const String & Application::machine_name()
{
    return NESocket::hostname();
}

ConfigManager& Application::config_manager()
{
    return Application::instance().mConfigManager;
}

bool Application::load_configuration(const char* fileName /*= nullptr*/, ConfigListener * listener /*= nullptr*/)
{
    Application& theApp = Application::instance();
    bool result{ true };
    if (theApp.mConfigManager.read_config(fileName == nullptr ? NEApplication::DEFAULT_CONFIG_FILE : fileName, listener) == false)
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

    const uint32_t countReadonly{ std::size(NEApplication::DefaultReadonlyProperties) };
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

    theApp.mConfigManager.set_configuration(defReadonly, defWritable, listener);
}

bool Application::is_configured()
{
    return Application::instance().mConfigManager.is_configured();
}

bool Application::_set_app_state(NEApplication::AppState newState)
{
    bool result = false;
    Application & theApp = Application::instance();
    if (newState == NEApplication::AppState::Failure)
    {
        theApp.mAppState = newState;
    }

    switch (theApp.mAppState)
    {
    case NEApplication::AppState::Stopped:
        if (newState == NEApplication::AppState::Initializing)
        {
            theApp.mAppState = NEApplication::AppState::Initializing;
            result = true;
        }
        break;

    case NEApplication::AppState::Initializing:
        if (newState == NEApplication::AppState::Ready)
        {
            theApp.mAppState = NEApplication::AppState::Ready;
            result = true;
        }
        break;

    case NEApplication::AppState::Ready:
        if (newState == NEApplication::AppState::Releasing)
        {
            theApp.mAppState = NEApplication::AppState::Releasing;
            result = true;
        }
        break;

    case NEApplication::AppState::Releasing:
        if (newState == NEApplication::AppState::Stopped)
        {
            theApp.mAppState = NEApplication::AppState::Stopped;
            result = true;
        }
        break;

    case NEApplication::AppState::Failure:
        result = true;
        break; // ignore

    default:
        ASSERT(false);
        break;
    }

    return result;
}
