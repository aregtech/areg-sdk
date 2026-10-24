#ifndef AREG_APPBASE_APPLICATION_HPP
#define AREG_APPBASE_APPLICATION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/SyncPrimitives.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <atomic>

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg {
    class ConfigListener;
    class Model;
    enum class ModelState : uint8_t;
    enum class ThreadState : uint8_t;
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
    friend class LogManager;

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
    static void setup( bool startTracing   = true
                     , bool startServicing = true
                     , bool startRouting   = true
                     , bool startTimer     = true
                     , bool startWatchdog  = false
                     , const char * configFile = areg::DEFAULT_CONFIG_FILE.data()
                     , ConfigListener * configListener = nullptr);

    /**
     * \brief   Stops all components, unloads models, stops services, and releases resources.
     **/
    static void release();

    /**
     * \brief   Loads and starts a registered model. If modelName is nullptr or empty, loads all
     *          registered models. Returns once the threads of the model started, or, if
     *          waitComponents is true, once their components started.
     *
     * \param   modelName       Unique name of the model to load, or nullptr to load all models.
     * \param   waitComponents  If true, waits until the components started; otherwise until the threads started.
     * \return  Returns true if the specified model was loaded successfully.
      *
      * \example     Start Model
      *          // Start all services and all registered models:
      *          Application::setup();
      *          Application::load_model();
     **/
    static bool load_model( const char * modelName = nullptr, bool waitComponents = false );

    /**
     * \brief   Stops and unloads a model. If modelName is nullptr or empty, unloads all models.
     *          Service Manager, Tracing, Routing, and Timer Manager are not stopped.
     *
     * \param   modelName       Unique name of the model to unload, or nullptr to unload all models.
     *
     * \example     Start and Stop all models
     * 
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
     * \brief   Returns the state of the model: Unloaded, Loading, Loaded or Unloading. Only areg
     *          changes it; the value is a snapshot. Include "areg/component/Model.hpp" to compare it.
     *
     * \param   modelName       Unique name of the model.
     * \return  The state of the model; areg::ModelState::Unloaded if there is no model of the name.
     **/
    [[nodiscard]]
    static areg::ModelState model_state( const char * modelName );

    /**
     * \brief   Returns the state of the component thread: Starting, Running, Stopping or
     *          Abandoned. Only areg changes it; the value is a snapshot. Include
     *          "areg/component/Model.hpp" to compare it.
     *
     * \param   threadName      Unique name of the component thread.
     * \return  The state of the thread; areg::ThreadState::Unknown if no component thread has the name.
     **/
    [[nodiscard]]
    static areg::ThreadState thread_state( const char * threadName );

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
     *          logging defaults. Can be called at any time while the application runs, also
     *          after stop_logging().
     *
     * \param   force       If true, forces loading the default configuration file
     *                      (areg::DEFAULT_CONFIG_FILE) or uses logging default settings.
     * \return  Returns true if logging started successfully; false if not configured or
     *          configuration failed.
     **/
    static bool start_logging( bool force = false );

    /**
     * \brief   Stops logging. Can be called at any time; start_logging() starts it again.
     **/
    static void stop_logging();

    /**
     * \brief   Starts the watchdog manager, enabling components to monitor thread health.
     *          Can be called at any time while the application runs, also after
     *          stop_watchdog_manager().
     *
     * \return  Returns true if the watchdog manager is running.
     **/
    static bool start_watchdog_manager();

    /**
     * \brief   Stops the watchdog manager. Can be called at any time, for example while
     *          debugging; no watchdog timeout is checked until start_watchdog_manager().
     **/
    static void stop_watchdog_manager();

    /**
     * \brief   Connects the application to the message router (mtrouter), with the router address
     *          and port the configuration holds at the moment of the call. To use another address,
     *          set it first through config_manager(), for example as a temporary value.
     *          Requires setup() with servicing. A connection already started is kept.
     * \return  Returns true if the connection is requested or already started; false if no router
     *          is configured or the service manager does not run.
     **/
    static bool connect_router();

    /**
     * \brief   Disconnects the application from the message router. The remote services become
     *          unavailable; local services go on.
     **/
    static void disconnect_router();

    /**
     * \brief   Returns the state of the router connection: NotConfigured, Disconnected,
     *          Connecting or Connected. The value is a snapshot.
     **/
    [[nodiscard]]
    static areg::RouterState router_state();

    /**
     * \brief   Returns the state of the system: Stopped, Initializing, Ready, Releasing or Failure.
     *          Only areg changes it; the value is a snapshot.
     **/
    [[nodiscard]]
    static areg::AppState system_state();

    /**
     * \brief   Sets the user state, a value the application defines (for example a pause). areg
     *          stores it and calls Component::on_user_state(previous, current) of every component
     *          on its own thread; it changes nothing else. The call does not wait.
     *
     * \param   userState   The new user state. The initial value is 0.
     **/
    static void set_user_state( uint32_t userState );

    /**
     * \brief   Returns the user state; 0 until set_user_state() sets one.
     **/
    [[nodiscard]]
    static uint32_t user_state();

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
     * \brief   Returns true while servicing can start or retry connections, i.e. the application
     *          is initializing or ready (not releasing, stopped or in failure).
     *
     * \return  Returns true if the application state is Initializing or Ready.
     **/
    [[nodiscard]]
    static bool is_servicing_available();

    /**
     * \brief   Queries the amount of sent data in bytes and sent messages since the last call, and resets counters.
     *
     * \param[out] sizeSend     On output, contains the size of data in bytes sent since the last call.
     * \param[out] msgSent      On output, contains the size of messages sent since the last call.
     * \note    Accumulate calls to measure total data. Call each second to measure data rate.
     **/
    static void query_data_sent(uint64_t& sizeSent, uint32_t& msgSent) noexcept;

    /**
     * \brief   Queries the amount of received data in bytes and received messages since the last call, and resets counters.
     *
     * \param[out] sizeSend     On output, contains the size of data in bytes received since the last call.
     * \param[out] msgSent      On output, contains the size of messages received since the last call.
     * \note    Accumulate calls to measure total data. Call each second to measure data rate.
     **/
    static void query_data_received(uint64_t& sizeRecv, uint32_t& msgRecv) noexcept;

    /**
     * \brief   Enables or disables data and message rate verbosity.
     **/
    static void enable_data_rate(bool enable) noexcept;

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
     * \brief   Loads default configuration. Discards previous configuration.
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
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The state of application.
     **/
    std::atomic<areg::AppState> mAppState;
    /**
     * \brief   The application named storage.
     **/
    MapAppStorage   mStorage;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

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
    static Application & instance();

    /**
     * \brief   Sets the application state. Allowed transitions: AppStateUndefined =>
     *          AppStateInitializing => AppStateReady => AppStateReleasing => AppStateUndefined.
     *
     * \param   newState    The new application state to set.
     * \return  Returns true if the application state was changed successfully.
     **/
    static bool _set_app_state(areg::AppState newState);

    /**
     * \brief   Starts the service manager, and with it the timer and watchdog managers.
     * \return  Returns true if the service manager runs.
     **/
    static bool _start_service_manager();

    /**
     * \brief   Starts the timer manager and the OS signal handlers.
     * \return  Returns true if the timer manager runs.
     **/
    static bool _start_timer_manager();

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
     * \brief   OS-specific initialization. On Linux, sets up signal handlers; on Windows, sets up time period.
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

} // namespace areg
#endif  // AREG_APPBASE_APPLICATION_HPP
