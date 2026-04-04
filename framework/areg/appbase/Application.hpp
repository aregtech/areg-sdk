#ifndef AREG_APPBASE_APPLICATION_HPP
#define AREG_APPBASE_APPLICATION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/appbase/Application.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Singleton application object,
 *              which is responsible to initialize and start components
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/AppDefs.hpp"

#include "areg/base/String.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Model.hpp"
#include "areg/persist/ConfigManager.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg {
    class ConfigListener;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Dependencies
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Application class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Singleton for application lifecycle management, system service initialization,
 *          configuration loading, and component model management.
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
    using MapAppStorage     = StringHashMap<areg::Primitive>;

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes application and starts specified system services. The system ignores
     *          requests to start a service if it is already running.
     *
     * \param   startTracing        If true, starts the tracing service.
     * \param   startServicing      If true, starts the Service Manager. Ignored if startRouting is
     *                              true.
     * \param   startRouting        If true, starts the message routing client and Service Manager
     *                              (if not already started).
     * \param   startTimer          If true, starts the timer manager. Automatically started if
     *                              Service Manager is running.
     * \param   startWatchdog       If true, starts the watchdog manager to monitor component
     *                              threads.
     * \param   configFile          Path to configuration file. If nullptr or empty, uses default
     *                              location './config/areg.init'.
     * \param   configListener      Optional listener notified before and after configuration
     *                              loading, or on failure. Null by default.
     **/
    static void setup( bool startTracing   = true
                     , bool startServicing = true
                     , bool startRouting   = true
                     , bool startTimer     = true
                     , bool startWatchdog  = false
                     , const char * configFile = areg::DEFAULT_CONFIG_FILE.data()
                     , ConfigListener * configListener = nullptr);

    /**
     * \brief   Stops all components, unloads models, stops services, and releases resources.
      *
      * \see     release, load_model
      *
      * \example     Initialize Application
      *
      *          // This starts all services and reads configuration from default location:
      *          Application::setup();
      *
      *          // Start all services, read configuration from user home folder:
      *          Application::setup(true, true, true, true, true, "%user%/areg.init");
      *
      *          // Start all services; startRouting forces Service Manager to start even if
      *          // startServicing is false. Read config from default location:
      *          Application::setup(true, false);
      *
      *          // Start no services, only configure the system from a custom config file.
      *          // Services can be started later:
      *          Application::setup(false, false, false, false, false, "%user%/areg.init");
     **/
    static void release();

    /**
     * \brief   Loads and starts a registered model. If modelName is nullptr or empty, loads all
     *          registered models.
     *
     * \param   modelName       Unique name of the model to load, or nullptr to load all models.
     * \return  Returns true if the specified model was loaded successfully.
      *
      * \example     Start Model
      *          // Start all services and all registered models:
      *          Application::setup();
      *          Application::load_model();
     **/
    static bool load_model( const char * modelName = nullptr );

    /**
     * \brief   Stops and unloads a model. If modelName is nullptr or empty, unloads all models.
     *          Service Manager, Tracing, Routing, and Timer Manager are not stopped.
     *
     * \param   modelName       Unique name of the model to unload, or nullptr to unload all models.
      *
      * \example     Start and Stop all models
      *          // Start all services, load all models, sleep 1 s, then unload and release:
      *          Application::setup();
      *          Application::load_model();
      *          Thread::sleep(1000);
      *          Application::unload_model();
      *          Application::release();
     **/
    static void unload_model( const char * modelName = nullptr );

    /**
     * \brief   Returns true if the specified model is loaded and running.
     *
     * \param   modelName       Unique name of the model to check. Must not be nullptr or empty.
     * \return  Returns true if the model is loaded; false otherwise.
     **/
    [[nodiscard]]
    static bool is_model_loaded( const char * modelName );

    /**
     * \brief   Searches for a model by name.
     *
     * \param   modelName       Name of the model to search for.
     * \return  Returns the model if found; otherwise returns an invalid model with an empty name.
     **/
    static const areg::Model & find_model( const char * modelName );

    /**
     * \brief   Sets the current working directory.
     *
     * \param   dirPath     Absolute or relative path to the folder to set as working directory. If
     *                      nullptr or empty, uses the folder of the executable.
     **/
    static void set_working_directory( const char * dirPath = nullptr );

    /**
     * \brief   Starts logging. If force is true, loads the default configuration file or uses
     *          logging defaults.
     *
     * \param   force       If true, forces loading the default configuration file
     *                      (areg::DEFAULT_CONFIG_FILE) or uses logging default settings.
     * \return  Returns true if logging started successfully; false if not configured or
     *          configuration failed.
     **/
    static bool start_logging( bool force = false );

    /**
     * \brief   Stops logging.
     **/
    static void stop_logging();

    /**
     * \brief   Starts the Service Manager thread. Does not trigger Router Service client
     *          connection.
     *
     * \return  Returns true if Service Manager started successfully or is already running.
     **/
    static bool start_service_manager();

    /**
     * \brief   Stops the Service Manager. Automatically stops the Routing Service connection.
     **/
    static void stop_service_manager();

    /**
     * \brief   Starts the timer manager, enabling components to trigger timers.
     *
     * \return  Returns true if the timer manager is running.
     **/
    static bool start_timer_manager();

    /**
     * \brief   Stops the timer manager.
     **/
    static void stop_timer_manager();

    /**
     * \brief   Starts the watchdog manager, enabling components to monitor thread health.
     *
     * \return  Returns true if the watchdog manager is running.
     **/
    static bool start_watchdog_manager();

    /**
     * \brief   Stops the watchdog manager.
     **/
    static void stop_watchdog_manager();

    /**
     * \brief   Returns true if the Service Manager is started.
     *
     * \return  Returns true if the Service Manager is started; false otherwise.
     **/
    [[nodiscard]]
    static bool is_service_manager_started();

    /**
     * \brief   Configures the message routing service.
     *
     * \return  Returns true if message routing was configured successfully.
     **/
    static bool config_message_routing();

    /**
     * \brief   Starts the Message Router client. Starts the Service Manager first if not already
     *          running.
     *
     * \param   connectTypes    Bitwise set of connection types to establish. Currently only TCP/IP
     *                          is supported.
     * \return  Returns true if the Message Router client started successfully.
     **/
    static bool start_message_routing(uint32_t connectTypes);

    /**
     * \brief   Starts the Message Router client with specified remote server address and port.
     *          Starts the Service Manager first if not already running. Overwrites IP address and
     *          port from configuration file.
     *
     * \param   ipAddress       Valid IP address of the remote Router Service.
     * \param   portNr          Valid port number of the remote Router Service.
     * \return  Returns true if the Message Router client started successfully.
     * \note    Move overload. Overwrites configuration file settings.
     **/
    static bool start_message_routing( const char * ipAddress, uint16_t portNr );

    /**
     * \brief   Stops the Message Router client.
     **/
    static void stop_message_routing();

    /**
     * \brief   Returns true if the Message Router client is configured.
     *
     * \return  Returns true if the Message Router client is configured; false otherwise.
     **/
    [[nodiscard]]
    static bool is_message_routing_configured();

    /**
     * \brief   Starts the routing service on the local machine. Requires appropriate access rights.
     *
     * \return  Returns true if the Message Routing Service started successfully.
     **/
    static bool start_router_service();

    /**
     * \brief   Starts the logging service on the local machine. Requires appropriate access rights.
     *
     * \return  Returns true if the logging service started successfully.
     **/
    static bool start_logging_service();

    /**
     * \brief   Returns true if the application is connected to the Message Routing Service and can
     *          register or request remote services.
     *
     * \return  Returns true if connected to the Message Routing Service; false otherwise.
     **/
    [[nodiscard]]
    static bool is_router_connected();


    /**
     * \brief   Returns true if the application has triggered connection to the Message Routing
     *          Service but the connection is not yet established.
     *
     * \return  Returns true if connection is pending; false otherwise.
     **/
    [[nodiscard]]
    static bool is_router_connection_pending();

    /**
     * \brief   Returns true if an element with the given name exists in application storage.
     *
     * \param   elemName    Name of the element to check in application storage.
     * \return  Returns true if the element exists; false otherwise.
     **/
    [[nodiscard]]
    static bool is_element_stored( const String & elemName );

    /**
     * \brief   Saves an element in application storage.
     *
     * \param   elemName    Name of the element to save.
     * \param   elem        Element to save in storage.
     * \return  Returns the previously saved element if an element with the same name exists;
     *          otherwise returns areg::InvalidElement.
     **/
    static areg::Primitive store_element( const String & elemName, areg::Primitive elem );

    /**
     * \brief   Returns the stored element with the given name.
     *
     * \param   elemName    Name of the element to retrieve from storage.
     * \return  Returns the stored element if found; otherwise returns areg::InvalidElement.
     **/
    static areg::Primitive stored_element( const String & elemName );

    /**
     * \brief   Blocks the calling thread until the application quit signal is set or the waiting
     *          timeout expires.
     *
     * \param   waitTimeout     Timeout in milliseconds to wait for the application quit signal.
     *                          areg::WAIT_INFINITE waits indefinitely.
     * \return  Returns true if the application quit signal was set; false if the timeout expired.
     **/
    static bool wait_quit( uint32_t waitTimeout = areg::WAIT_INFINITE);

    /**
     * \brief   Sets the application quit signal event, allowing waiting threads to release and
     *          perform cleanup.
     **/
    static void signal_quit();

    /**
     * \brief   Returns true if the Service Manager is running.
     *
     * \return  Returns true if the Service Manager is running; false otherwise.
     **/
    [[nodiscard]]
    static bool is_servicing_ready();

    /**
     * \brief   Queries the amount of sent and received data in bytes since the last call, and
     *          resets counters.
     *
     * \param[out] sizeSend        On output, contains the size of data in bytes sent since the last
     *                             call.
     * \param[out] sizeReceive     On output, contains the size of data in bytes received since the
     *                             last call.
     * \note    Accumulate calls to measure total data. Call each second to measure data rate.
     **/
    static void query_communication_data( uint32_t & sizeSend, uint32_t & sizeReceive );

    /**
     * \brief   Returns the name of the executable process.
     *
     * \return  The name of the executable process.
     **/
    static const String & application_name();

    /**
     * \brief   Returns the name of the system host.
     *
     * \return  The name of the system host.
     **/
    static const String & machine_name();

    /**
     * \brief   Returns the application configuration manager instance for reading or writing
     *          configuration properties.
     *
     * \return  Reference to the ConfigManager instance.
     **/
    static ConfigManager& config_manager();

    /**
     * \brief   Loads configuration from the specified file.
     *
     * \param   fileName    Relative or absolute path to the configuration file. If nullptr, loads
     *                      from default './config/areg.init'.
     * \param   listener    Optional listener notified before and after loading, or on failure. No
     *                      notifications if nullptr.
     * \return  Returns true if configuration was loaded successfully; false if loading failed
     *          (default configuration is loaded).
     **/
    static bool load_configuration(const char * fileName = nullptr, ConfigListener * listener = nullptr);

    /**
     * \brief   Saves current configuration to the specified file. Only writable properties are
     *          saved; the read-only global configuration is unchanged.
     *
     * \param   fileName    Relative or absolute path to the configuration file. If nullptr, saves
     *                      to the previously loaded file or default './config/areg.init'.
     * \param   listener    Optional listener notified before and after saving, or on failure. No
     *                      notifications if nullptr.
     * \return  Returns true if configuration was saved successfully; false otherwise.
     **/
    static bool save_configuration(const char * fileName = nullptr, ConfigListener * listener = nullptr);

    /**
     * \brief   Loads default configuration from areg::DefaultReadonlyProperties and
     *          areg::DefaultLogScopesConfig. Discards previous configuration.
     *
     * \param   listener    Optional listener notified when default configuration is set. No
     *                      notifications if nullptr.
     **/
    static void setup_default_configuration(ConfigListener * listener = nullptr);

    /**
     * \brief   Returns true if the application is already configured.
     *
     * \return  Returns true if configured; false otherwise.
     **/
    [[nodiscard]]
    static bool is_configured();

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The state of application.
     **/
    areg::AppState    mAppState;
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
    SyncEvent       mAppQuit;
    /**
     * \brief   Synchronization object
     **/
    ResourceLock    mLock;
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The application named storage.
     **/
    MapAppStorage   mStorage;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief	The singleton instance of Application object.
     **/
    static Application	_theApplication;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    Application();
    ~Application() = default;

    /**
     * \brief   Returns a reference to the singleton Application instance.
     *
     * \return  Reference to the singleton Application instance.
     **/
    static inline Application & instance();

    /**
     * \brief   Sets the application state. Allowed transitions: AppStateUndefined =>
     *          AppStateInitializing => AppStateReady => AppStateReleasing => AppStateUndefined.
     *
     * \param   newState    The new application state to set.
     * \return  Returns true if the application state was changed successfully.
     **/
    static bool _set_app_state(areg::AppState newState);

    /**
     * \brief   OS-specific implementation to start a local service. Does nothing if the service is
     *          already running. Requires appropriate access rights.
     *
     * \param   serviceName             Name of the service to start.
     * \param   serviceExecutable       Name of the service executable to verify.
     * \return  Returns true if the service is running; false otherwise.
     **/
    static bool _os_start_local_service( const wchar_t * serviceName, const wchar_t * serviceExecutable );

    /**
     * \brief   OS-specific initialization. On Linux, sets up signal handlers; on Windows, sets up
     *          time period.
     **/
    static void _os_setup_handlers();

    /**
     * \brief   OS-specific cleanup to release resources.
     **/
    static void _os_release_handlers();

//////////////////////////////////////////////////////////////////////////
// Forbidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Application );
};

//////////////////////////////////////////////////////////////////////////
// Application class inline methods.
//////////////////////////////////////////////////////////////////////////

inline Application & Application::instance()
{
    return Application::_theApplication;
}

} // namespace areg
#endif  // AREG_APPBASE_APPLICATION_HPP
