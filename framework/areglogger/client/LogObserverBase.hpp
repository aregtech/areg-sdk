#ifndef AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
#define AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/LogObserverBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library.
 *              The log observer base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areglogger/client/LogObserverApi.h"

#include "areg/base/SocketDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <map>
#include <string>
#include <vector>

/************************************************************************
 * Dependencies.
 ************************************************************************/

namespace areg {
    class SharedBuffer;
}

namespace areg::logger {

/**
 * \brief   Base class for log observer callbacks. Extend the class and implement virtual methods to
 *          intercept log observer events. If an instance is created, callbacks are triggered on the
 *          instance; otherwise, Log Observer API callbacks are used. This class should be
 *          implemented as a singleton.
 **/
class LOGGER_API LogObserverBase
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class LoggerClient;
    friend class ObserverMessageProcessor;

//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    LogObserverBase();
    virtual ~LogObserverBase();

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the log observer and makes automatic connection to the log collector.
     *          This method should be called before any other operation. Once the operation
     *          succeeded, multiple calls of this method will have no effect. The connection
     *          information is taken from the specified configuration file. If the `configFile`
     *          parameter is empty, it uses default path `./config/areg.init`.
     *
     * \param   configFile      The absolute or relative path to the configuration file. If string
     *                          is empty, uses `./config/areg.init` relative path.
     * \return  Returns true if successfully initialized; false otherwise.
     **/
    bool initialize(const std::string& configFile = String::EmptyString);

    /**
     * \brief   Releases the log observer and disconnects from log collector.
     **/
    void release();

    /**
     * \brief   Connects to the log collector service using the specified IP address and port.
     *
     * \param   address         The IP address of the log collector service to connect to. If empty,
     *                          uses IP address from configuration file.
     * \param   portNr          The port number to connect to. If 0, uses port number from
     *                          configuration file.
     * \param   dbLocation      The path to the logging database file. If empty, uses the path
     *                          specified in the config file. The path may contain masking like
     *                          `./log/log_%time%.db`.
     * \return  Returns true if the connection request was successfully triggered; false otherwise.
     *          The physical connection may not be established yet; it is confirmed via
     *          `on_log_service_connected()` callback.
     **/
    bool connect(const std::string& address, uint16_t portNr, const std::string& dbLocation);

    /**
     * \brief   Disconnects from the log collector service. Does not close the database; use
     *          `stop()` to close the database.
     **/
    void disconnect();

    /**
     * \brief   Pauses the log observer. The observer remains connected, but no logs are written.
     *          When resumed, logs are written to the same database file.
     *
     * \return  Returns true if processed successfully; false otherwise.
     **/
    bool pause();

    /**
     * \brief   Resumes a paused log observer. Logs continue to be written to the same database file.
     *
     * \return  Returns true if processed successfully; false otherwise.
     **/
    bool resume();

    /**
     * \brief   Stops the log observer, disconnects from log collector service, and closes the database file.
     *
     * \return  Returns true if processed successfully; false otherwise.
     **/
    bool stop();

    /**
     * \brief   Restarts the log observer. Establishes connection with the log collector service
     *          using existing connection information and creates a new database file.
     *
     * \param   dbLocation      The relative or absolute path to the logging database file. If
     *                          empty, uses the default path. The path may contain masking like
     *                          `log_%time%.sqlog`.
     * \return  Returns true if processed successfully; false otherwise.
     **/
    bool restart(const std::string& dbLocation = String::EmptyString);

    /**
     * \brief   Returns true if the log observer is initialized.
     **/
    [[nodiscard]]
    bool is_initialized() const noexcept;

    /**
     * \brief   Returns true if the log observer is connected to the log collector service.
     **/
    [[nodiscard]]
    bool is_connected() const noexcept;

    /**
     * \brief   Returns true if the log observer is fully operable and able to collect and write
     *          logs.
     **/
    [[nodiscard]]
    bool is_stated() const noexcept;

    /**
     * \brief   Returns the address of the log collector service.
     **/
    [[nodiscard]]
    const areg::SocketAddress & logger_address() const noexcept;

    /**
     * \brief   Returns the IP address of the log collector service.
     **/
    [[nodiscard]]
    const std::string& logger_ip_address() const noexcept;

    /**
     * \brief   Returns the host name or IP address of the log collector service.
     **/
    [[nodiscard]]
    const std::string& logger_host_name() const noexcept;

    /**
     * \brief   Returns the TCP port number to connect to the log collector service.
     **/
    [[nodiscard]]
    uint16_t logger_port() const noexcept;

    /**
     * \brief   Returns the logging state set in the configuration.
     **/
    [[nodiscard]]
    bool config_logger_enabled() const noexcept;

    /**
     * \brief   Returns the IP address of log collector service set in the configuration.
     **/
    [[nodiscard]]
    std::string config_logger_address() const;

    /**
     * \brief   Sets the IP address of the log collector service in the configuration.
     *
     * \param   address     The IP address of the log collector service to set.
     **/
    void set_config_logger_address(const std::string& address);

    /**
     * \brief   Returns the TCP port number of log collector service set in the configuration.
     **/
    [[nodiscard]]
    uint16_t config_logger_port() const noexcept;

    /**
     * \brief   Sets the TCP port number of the log collector service in the configuration.
     *
     * \param   portNr      The port number of the log collector service to set.
     **/
    void set_config_logger_port(uint16_t portNr);

    /**
     * \brief   Sets the TCP/IP address and port number of the log collector service in the active
     *          configuration.
     *
     * \param   address     The IP address of the log collector service to set.
     * \param   portNr      The TCP port number of the log collector service to set.
     **/
    void set_config_logger_connection(const std::string& address, uint16_t portNr);

    /**
     * \brief   Returns the database path name set in the configuration.
     **/
    [[nodiscard]]
    std::string config_logger_database() const;

    /**
     * \brief   Sets the database path name in the configuration. The path may contain mask like `log_%time%.sqlog`.
     *
     * \param   dbFilePath      The database path to set.
     **/
    void set_config_logger_database(const std::string& dbFilePath);

    /**
     * \brief   Returns the log database file location directory name. The path may contain mask like `log_%time%`.
     **/
    [[nodiscard]]
    std::string config_logger_database_location() const;

    /**
     * \brief   Sets the log database file location directory name. The path may contain mask like `log_%time%`.
     *
     * \param   dbLocation      The database location to set.
     **/
    void set_config_logger_database_location(const std::string& dbLocation);

    /**
     * \brief   Returns the name of the database file. The name may contain mask like `log_%time%.sqlog`.
     **/
    [[nodiscard]]
    std::string config_logger_database_name() const;

    /**
     * \brief   Sets the name of the database file. The name may contain mask like `log_%time%.sqlog`.
     *
     * \param   dbName      The database name to set.
     **/
    void set_config_logger_database_name(const std::string& dbName);

    /**
     * \brief   Returns the path of the currently active logging database. The returned path does not contain mask.
     **/
    [[nodiscard]]
    std::string active_database_path() const;

    /**
     * \brief   Returns the path of the database set during initialization. The path may contain mask like `log_%time%.sqlog`.
     *
     * \return  Returns the path of the database set during initialization.
     **/
    [[nodiscard]]
    std::string init_database_path() const;

    /**
     * \brief   Queries the log database and returns the list of names of connected instances.
     *
     * \param[out] names       On output, contains the list of names of connected instances.
     **/
    void log_instance_names(std::vector<String>& names);

    /**
     * \brief   Queries the log database and returns the list of IDs of connected instances.
     *
     * \param[out] ids     On output, contains the list of IDs of connected instances.
     **/
    void log_instances(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Queries the log database and returns the list of names of threads of the connected instances.
     *
     * \param[out] names       On output, contains the list of all thread names that sent messages.
     **/
    void log_thread_names(std::vector<String>& names);

    /**
     * \brief   Queries the log database and returns the list of IDs of threads of the connected instances.
     *
     * \param[out] ids     On output, contains the list of all thread IDs that sent messages.
     **/
    void log_threads(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Returns the list of log priorities.
     *
     * \param[out] names       On output, contains the names of all priorities.
     **/
    void log_priority_names(std::vector<String>& names);

    /**
     * \brief   Queries the log database and returns information of all connected instances.
     *
     * \param[out] infos       On output, contains the list of information of all registered
     *                         instances in database.
     **/
    void log_instance_infos(std::vector< areg::ConnectedInstance>& infos);

    /**
     * \brief   Queries the log database and returns information of log scopes of a specified instance.
     *
     * \param[out] scopes   On output, contains the list of all registered scopes in database
     *                      related with the specified instance ID.
     * \param   instId      The ID of the instance.
     **/
    void log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId);

    /**
     * \brief   Returns all log messages from the log database.
     *
     * \param[out] messages    On output, contains the list of all log messages.
     **/
    void log_messages(std::vector<SharedBuffer>& messages);

    /**
     * \brief   Returns log messages of a specified instance from the log database. If `instId` is
     *          `areg::COOKIE_ANY`, returns log messages of all instances.
     *
     * \param[out] messages On output, contains the list of log messages of the specified instance.
     * \param   instId      The ID of the instance to get log messages from. If
     *                      `areg::COOKIE_ANY`, returns log messages of all instances.
     **/
    void log_inst_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId = areg::COOKIE_ANY);

    /**
     * \brief   Returns log messages of a specified scope from the log database. If `scopeId` is
     *          `0`, returns log messages of all scopes.
     *
     * \param[out] messages On output, contains the list of log messages of the specified scope.
     * \param   scopeId     The ID of the scope to get log messages from. If `0`, returns log
     *                      messages of all scopes.
     **/
    void log_scope_messages(std::vector<SharedBuffer>& messages, uint32_t scopeId = 0);

    /**
     * \brief   Returns log messages of a specified instance and scope from the log database. If
     *          `instId` is `areg::COOKIE_ANY` and `scopeId` is `0`, returns all log messages.
     *
     * \param[out] messages On output, contains the list of log messages of the specified instance and scope.
     * \param   instId      The ID of the instance to get log messages from. If
     *                      `areg::COOKIE_ANY`, returns log messages of all instances.
     * \param   scopeId     The ID of the scope to get log messages from. If `0`, returns log
     *                      messages of all scopes.
     **/
    void log_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);

//////////////////////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Requests the list of connected instances that make logs.
     *
     * \return  Returns true if processed with success; false otherwise.
     **/
    bool request_instances();

    /**
     * \brief   Requests the list of registered scopes of a specified connected instance.
     *
     * \param   target      The cookie ID of the target instance to receive the list of registered
     *                      scopes. If the target is `areg::TARGET_ALL` (or 0), receives the
     *                      list of scopes of all connected instances. Otherwise, must be a valid
     *                      cookie ID of a connected log instance.
     * \return  Returns true if processed with success; false otherwise.
     **/
    bool request_scopes(ITEM_ID target = areg::TARGET_ALL);

    /**
     * \brief   Requests to update the priority of logging messages to receive. The indicated scopes
     *          can be a scope group.
     *
     * \param   target      The valid cookie ID of the target instance to update the log message
     *                      priority. This value cannot be `areg::TARGET_ALL` (or 0xFF).
     * \param   scopes      The list of scopes or scope group to update the priority. Scope groups
     *                      should end with `*`. For example `areg_base_*`. In this case the ID of
     *                      the scope can be 0.
     * \param   count       The number of scope entries in the list.
     * \return  Returns true if processed with success; false otherwise.
     **/
    bool request_change_scope_prio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count);

    /**
     * \brief   Requests to save current configuration of a specified target. This is normally
     *          called after updating the log priority of an instance, so that the next start logs
     *          messages of the scopes and priorities currently set.
     *
     * \param   target      The cookie ID of the target instance to save the configuration. If the target
     *                      is `areg::TARGET_ALL` (or 0xFF), the request is sent to  connected instances.
     *                      Otherwise, must be a valid cookie ID of a connected log instance.
     * \return  Returns true if processed with success; false otherwise.
     **/
    bool request_save_config(ITEM_ID target = areg::TARGET_ALL);

    /**
     * \brief   Saves the configuration of the log observer in the configuration file.
     **/
    void save_logger_config();

//////////////////////////////////////////////////////////////////////////
// Protected Overrides / Callbacks
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************
 * LogObserverBase overrides
 ************************************************************************/

    /**
     * \brief   Callback triggered when initializing and configuring the observer. Indicates the IP
     *          address and port number of the log collector service set in the configuration file.
     *
     * \param   isEnabled   The flag indicating whether the logging service is enabled or not.
     * \param   address     The null-terminated string of the IP address of the log collector service set in the configuration file.
     * \param   port        The IP port number of the log collector service set in the configuration file.
     **/
    virtual void on_log_observer_configured(bool isEnabled, const std::string & address, uint16_t port) = 0;

    /**
     * \brief   Callback triggered when initializing and configuring the observer. Indicates the
     *          supported database, the database location or URI, and the database user name.
     *
     * \param   isEnabled   The flag indicating whether logging in the database is enabled or not.
     * \param   dbName      The name of the supported database.
     * \param   dbLocation  The relative or absolute path to the database. The path may contain a mask.
     * \param   dbUser      The database user to use when logging in. If null or empty, the database may not require the user name.
     **/
    virtual void on_log_db_configured(bool isEnabled, const std::string & dbName, const std::string & dbLocation, const std::string & dbUser) = 0;

    /**
     * \brief   Callback triggered when the observer connects or disconnects from the log collector service.
     *
     * \param   isConnected     Flag indicating whether observer is connected or disconnected.
     * \param   address         The IP address of the log collector service to connect or disconnect from.
     * \param   port            The IP port number of the log collector service to connect or disconnect from.
     **/
    virtual void on_log_service_connected(bool isConnected, const std::string & address, uint16_t port) = 0;

    /**
     * \brief   Callback triggered when starting or pausing the log observer. If paused, on resume
     *          logs continue in the same file. If stopped, on restart a new file is created.
     *
     * \param   isStarted   The flag indicating whether the log observer is started or paused.
     **/
    virtual void on_log_observer_started(bool isStarted) = 0;

    /**
     * \brief   Callback triggered when the logging database is created.
     *
     * \param   dbLocation      The relative or absolute path to the logging database.
     **/
    virtual void on_log_db_created(const std::string & dbLocation) = 0;

    /**
     * \brief   Callback triggered when fails to send or receive a message.
     **/
    virtual void on_log_messaging_failed() = 0;

    /**
     * \brief   Callback triggered when receiving the list of connected instances that make logs.
     *
     * \param   instances       The list of the connected instances.
     **/
    virtual void on_log_instances_connect(const std::vector< areg::ConnectedInstance > & instances) = 0;

    /**
     * \brief   Callback triggered when receiving the list of disconnected instances that make logs.
     *
     * \param   instances       The list of the disconnected instances.
     **/
    virtual void on_log_instances_disconnect(const std::vector< areg::ConnectedInstance > & instances) = 0;

    /**
     * \brief   Callback triggered when the connection with the log collector service is lost.
     **/
    virtual void on_log_service_disconnected() = 0;

    /**
     * \brief   Callback triggered when receiving the list of scopes registered in an application.
     *
     * \param   cookie      The cookie ID of the connected instance/application. Same as
     *                      `LogInstance::liCookie`.
     * \param   scopes      The list of scopes registered in the application. Each entry contains
     *                      the ID of the scope, message priority, and the full name.
     * \param   count       The number of scope entries in the list.
     **/
    virtual void on_log_register_scopes(ITEM_ID cookie, const ScopeInfo* scopes, int32_t count) = 0;

    /**
     * \brief   Callback triggered when receiving the list of previously registered scopes with new priorities.
     *
     * \param   cookie      The cookie ID of the connected instance/application. Same as
     *                      `LogInstance::liCookie`.
     * \param   scopes      The list of previously registered scopes. Each entry contains the ID of
     *                      the scope, message priority, and the full name.
     * \param   count       The number of scope entries in the list.
     **/
    virtual void on_log_update_scopes(ITEM_ID cookie, const ScopeInfo* scopes, int32_t count) = 0;

    /**
     * \brief   Callback triggered when receiving a message to log.
     *
     * \param   logMessage  The structure of the message to log.
     **/
    virtual void on_log_message(const SharedBuffer & logMessage) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    static  LogObserverBase* _theLogObserver; //!< The instance of the log observer base class.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LogObserverBase);
};

} // namespace areg::logger

#endif // AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
