#ifndef AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
#define AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LoggerClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *              The log collector service connection client.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/persist/ConfigListener.hpp"

#include "areg/logging/LoggingDefs.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"

#include "areglogger/client/private/ObserverMessageProcessor.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
struct ObserverEvents;

namespace areg::logger {

//////////////////////////////////////////////////////////////////////////
// LoggerClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Client for logging framework that manages connection to the log collector service,
 *          handles message synchronization, and maintains database of log entries, scopes, and
 *          connected instances.
 **/
class LoggerClient final    : public    ServiceClientConnectionBase
                            , public    ConfigListener
                            , protected DispatcherThread
                            , protected ConnectionConsumer
                            , protected RemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Friend class
//////////////////////////////////////////////////////////////////////////
    friend class ObserverMessageProcessor;

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The name of the observer dispatcher thread
    static constexpr std::string_view                   THREAD_NAME { "AREG_LogObserverThread" };

    //!< The prefix to add to the send and receive threads.
    static constexpr std::string_view                   THREAD_PREFIX{ "Observer" };

    //!< The ID of the target to send and receive messages. The target is the log collector service cookie ID.
    static constexpr ITEM_ID                            TARGET_ID   { areg::COOKIE_LOGGER };

    //!< The type of the service. It is a log collector service.
    static constexpr areg::RemoteServiceKind   SERVICE_TYPE{ areg::RemoteServiceKind::Logger };

    //!< The connection type. At the moment only TCP/IP
    static constexpr areg::ConnectionType  CONNECT_TYPE{ areg::ConnectionType::Tcpip };

    //!< The message source. It is marked as an observer.
    static constexpr areg::MessageSource          SOURCE_TYPE { areg::MessageSource::SourceObserver };

//////////////////////////////////////////////////////////////////////////
// Static methods.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of the LoggerClient.
     **/
    static LoggerClient& instance();

//////////////////////////////////////////////////////////////////////////
// Hidden constructor and destructor.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient();
    virtual ~LoggerClient() = default;

//////////////////////////////////////////////////////////////////////////
// Public actions.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Starts the send and receive threads and establishes connection to the log collector
     *          service. Retries connection on failure.
     *
     * \param   address     The IP address or hostname of the log collector service. If empty, uses
     *                      the value from configuration file. Must be empty if portNr is
     *                      areg::InvalidPort.
     * \param   portNr      The port number of the log collector service. If areg::InvalidPort,
     *                      uses the port number from configuration file. Must be
     *                      areg::InvalidPort if address is empty.
     * \return  Returns true if threads were initialized and service connection initiated; false
     *          otherwise.
     * \note    Either both address and portNr should be valid values or both should be
     *          invalid/empty.
     **/
    bool start_logger_client(const String & address = String::EmptyString, uint16_t portNr = areg::InvalidPort);

    /**
     * \brief   Stops threads and disconnects from the log collector service.
     **/
    void stop_logger_client();

    /**
     * \brief   Sets the callbacks to invoke on messaging events.
     *
     * \param   callbacks       Pointer to callback structure. May contain all callbacks or only
     *                          selected ones. Pass nullptr to reset and disable all callbacks.
     **/
    void set_callbacks(const ObserverEvents * callbacks);

    /**
     * \brief   Sets or clears the paused flag. When paused, logs are not written but the connection
     *          remains active.
     *
     * \param   doPause     true to pause logging; false to resume.
     **/
    void set_paused(bool doPause);

    /**
     * \brief   Returns the socket address (IP and port) of the log collector service.
     **/
    [[nodiscard]]
    const areg::SocketAddress& address() const noexcept;

    /**
     * \brief   Returns true if the logging database engine is SQLite; false otherwise.
     **/
    [[nodiscard]]
    bool is_sqlite_engine() const noexcept;

    /**
     * \brief   Returns true if the observer is configured and log collector service connection is
     *          enabled.
     **/
    [[nodiscard]]
    bool is_config_logger_connect_enabled() const noexcept;

    /**
     * \brief   Returns the IP address of the log collector service from the configuration file.
     **/
    [[nodiscard]]
    String config_logger_address() const;

    /**
     * \brief   Returns the port number of the log collector service from the configuration file.
     **/
    [[nodiscard]]
    uint16_t config_logger_port() const;

    /**
     * \brief   Sets the IP address and port number of the log collector service to connect.
     *
     * \param   address     The IP address of the log collector service.
     * \param   portNr      The port number of the log collector service.
     * \return  Returns true if connection data was set; false otherwise.
     **/
    bool set_config_logger_connection(const String& address, uint16_t portNr);

    /**
     * \brief   Sends a request to query the list of connected clients.
     *
     * \return  Returns true if request was processed; false otherwise.
     **/
    bool request_connected_instances();

    /**
     * \brief   Sends a request to query the list of log scopes.
     *
     * \param   target      The ID of the target to send the message. If areg::TARGET_ALL,
     *                      sends to all connected clients.
     * \return  Returns true if request was processed; false otherwise.
     **/
    bool request_scopes(const ITEM_ID& target = areg::TARGET_ALL);

    /**
     * \brief   Sends a request to update the priority of log scopes.
     *
     * \param   scopes      List of scopes to update. Each entry contains scope name, ID, and
     *                      message priority. ID can be 0 if the name refers to a scope group.
     * \param   target      The ID of the target to send the message. If areg::TARGET_ALL,
     *                      sends to all connected clients.
     * \return  Returns true if request was processed; false otherwise.
     **/
    bool request_change_scope_prio(const areg::ScopeNames& scopes, const ITEM_ID& target = areg::TARGET_ALL);

    /**
     * \brief   Sends a request to save configuration state so it persists across application
     *          restarts.
     *
     * \param   target      The ID of the target to send the message. If areg::TARGET_ALL,
     *                      sends to all connected clients.
     * \return  Returns true if request was processed; false otherwise.
     **/
    bool request_save_configuration(const ITEM_ID & target = areg::TARGET_ALL);

    /**
     * \brief   Creates or opens the logging database. If no path is specified, uses the location
     *          from configuration file.
     *
     * \param   dbPath      Relative or absolute path to the logging database file. If null or
     *                      empty, uses location from configuration file.
     * \return  Returns true if database was initialized; false otherwise.
     **/
    bool open_logging_database(const char* dbPath = nullptr);

    /**
     * \brief   Closes the logging database.
     **/
    void close_logging_database();

    /**
     * \brief   Returns the path to the currently active logging database.
     **/
    [[nodiscard]]
    String active_database_path() const;

    /**
     * \brief   Returns the initial path to the logging database.
     **/
    [[nodiscard]]
    String initial_database_path() const;

    /**
     * \brief   Returns the database path specified in the configuration file.
     **/
    [[nodiscard]]
    String config_database_path() const;

    /**
     * \brief   Sets the logging database path and enables or disables database logging. Path can
     *          contain a mask to create new databases.
     *
     * \param   dbPath      Relative or absolute path to the logging database file. If empty, uses
     *                      path from configuration file.
     * \param   enable      true to enable database logging; false to disable.
     * \return  Returns true if database path was set; false otherwise.
     **/
    bool set_config_database_path(const String& dbPath, bool enable);

    /**
     * \brief   Returns the logging database location path from the configuration file.
     **/
    [[nodiscard]]
    String config_database_location() const;

    /**
     * \brief   Sets the logging database location path.
     *
     * \param   dbLocation      The location path for the logging database.
     * \return  Returns true if operation succeeded; false otherwise.
     **/
    bool set_config_database_location(const String& dbLocation);

    /**
     * \brief   Returns the logging database name from the configuration file.
     **/
    [[nodiscard]]
    String config_database_name() const;

    /**
     * \brief   Sets the logging database name in the configuration file.
     *
     * \param   dbName      The name of the logging database.
     * \return  Returns true if operation succeeded; false otherwise.
     **/
    bool set_config_database_name(const String& dbName);

    /**
     * \brief   Sets the logging connection flag in the configuration file.
     *
     * \param   is_enabled      true to enable logging connection; false to disable.
     * \return  Returns true if operation succeeded; false otherwise.
     **/
    bool set_config_logger_connect_enabled(bool is_enabled);

    /**
     * \brief   Saves the current log observer configuration to the configuration file.
     **/
    void save_configuration();

    /**
     * \brief   Retrieves the list of names of connected instances from the log database.
     *
     * \param[out] names       On output, contains the list of names of connected instances.
     **/
    inline void log_instance_names(std::vector<String>& names);

    /**
     * \brief   Retrieves the list of IDs of connected instances from the log database.
     *
     * \param[out] ids     On output, contains the list of IDs of connected instances.
     **/
    inline void log_instances(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Retrieves the list of names of threads from connected instances from the log
     *          database.
     *
     * \param[out] names       On output, contains the list of all thread names that sent messages.
     **/
    inline void log_thread_names(std::vector<String>& names);

    /**
     * \brief   Retrieves the list of IDs of threads from connected instances from the log database.
     *
     * \param[out] ids     On output, contains the list of all thread IDs that sent messages.
     **/
    inline void log_threads(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Retrieves the list of log priority names.
     *
     * \param[out] names       On output, contains the names of all priorities.
     **/
    inline void log_priority_names(std::vector<String>& names);

    /**
     * \brief   Retrieves information about all registered instances from the log database.
     *
     * \param[out] infos       On output, contains the list of information of all registered
     *                         instances in database.
     **/
    inline void log_instance_infos(std::vector< areg::ConnectedInstance>& infos);

    /**
     * \brief   Retrieves information about log scopes of a specified instance from the log
     *          database.
     *
     * \param[out] scopes      On output, contains the list of all registered scopes in database
     *                         related to the specified instance ID.
     * \param   instId      The ID of the instance.
     **/
    inline void log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId);

    /**
     * \brief   Retrieves all log messages from the log database.
     *
     * \param[out] messages    On output, contains the list of all log messages.
     **/
    inline void log_messages(std::vector<SharedBuffer>& messages);

    /**
     * \brief   Retrieves log messages from a specified instance. If instId is
     *          areg::COOKIE_ANY, retrieves messages from all instances.
     *
     * \param[out] messages    On output, contains the list of log messages of the specified
     *                         instance.
     * \param   instId      The ID of the instance to retrieve log messages from. If
     *                      areg::COOKIE_ANY, retrieves from all instances.
     **/
    inline void log_inst_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId = areg::COOKIE_ANY);

    /**
     * \brief   Retrieves log messages from a specified scope. If scopeId is 0, retrieves messages
     *          from all scopes.
     *
     * \param[out] messages    On output, contains the list of log messages of the specified scope.
     * \param   scopeId     The ID of the scope to retrieve log messages from. If 0, retrieves from
     *                      all scopes.
     **/
    inline void log_scope_messages(std::vector<SharedBuffer>& messages, uint32_t scopeId = 0);

    /**
     * \brief   Retrieves log messages from a specified instance and scope. If both are 'any',
     *          retrieves all messages.
     *
     * \param[out] messages    On output, contains the list of log messages of the specified
     *                         instance and scope.
     * \param   instId      The ID of the instance to retrieve log messages from. If
     *                      areg::COOKIE_ANY, includes all instances.
     * \param   scopeId     The ID of the scope to retrieve log messages from. If 0, includes all
     *                      scopes.
     **/
    inline void log_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ConfigListener interface overrides
/************************************************************************/

    /**
     * \brief   Called before the configuration is saved to prepare data.
     *
     * \param   config      The configuration manager instance.
     **/
    void prepare_save_configuration(ConfigManager& config) final;

    /**
     * \brief   Called after the configuration is saved to perform post-save operations.
     *
     * \param   config      The configuration manager instance.
     **/
    void post_save_configuration(ConfigManager& config) final;

    /**
     * \brief   Called before the configuration is loaded to prepare for reading.
     *
     * \param   config      The configuration manager instance.
     **/
    void prepare_read_configuration(ConfigManager& config) final;

    /**
     * \brief   Called after configuration data is loaded to complete initialization.
     *
     * \param   config      The configuration manager instance.
     **/
    void post_read_configuration(ConfigManager& config) final;

    /**
     * \brief   Called after setting read-only and writable properties in the configuration.
     *
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module/process-specific properties to set in the
     *                          configuration.
     * \param   config          The configuration manager instance.
     **/
    void on_setup_configuration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigManager& config) final;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Posts an event for delivery to its target. This base implementation destroys the
     *          event without processing.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true.
     **/
    [[nodiscard]]
    bool post_event(Event& eventElem) final;

    /**
     * \brief   Enables or disables event dispatching. Override to perform event dispatcher
     *          preparation.
     *
     * \param   is_ready    true to enable event dispatching; false to disable.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Initiates remote service connection. The host and port must be already configured.
     *
     * \return  Returns true if service connection was initiated; false otherwise.
     **/
    bool connect_service_host() final;

    /**
     * \brief   Disconnects from the remote service and stops communication.
     **/
    void disconnect_service_host() final;

    /**
     * \brief   Called when the service should exit.
     **/
    void on_service_exit() final;

/************************************************************************/
// ConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Called when the remote service connection and communication channel is established.
     *
     * \param   channel     The connection and communication channel of the remote service.
     **/
    void on_service_channel_connected(const Channel& channel) final;

    /**
     * \brief   Called when the remote service connection and communication channel is disconnected.
     *
     * \param   channel     The connection and communication channel of the remote service.
     **/
    void on_service_channel_disconnected(const Channel& channel) final;

    /**
     * \brief   Called when the remote service connection is lost unexpectedly. The connection is
     *          lost if data cannot be read or received, excluding intentional API-initiated
     *          disconnects.
     *
     * \param   channel     The connection and communication channel of the remote service.
     **/
    void on_service_channel_lost(const Channel& channel) final;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Called when a message fails to send.
     *
     * \param   msgFailed       The message that failed to send.
     * \param   whichTarget     The target socket where the send failed.
     **/
    void failed_send_message( const EventEnvelope & msgFailed, Socket & whichTarget ) final;

    /**
     * \brief   Called when message reception fails.
     *
     * \param   whichSource     The source socket where reception failed.
     **/
    void failed_receive_message( Socket & whichSource ) final;

    /**
     * \brief   Called when message processing fails because the target was not found. For request
     *          messages, an error notification is sent to the source.
     *
     * \param   msgUnprocessed      The unprocessed message data.
     **/
    void failed_process_message( const EventEnvelope & msgUnprocessed ) final;

    /**
     * \brief   Processes a received message.
     *
     * \param   msgReceived     The received message to process.
     * \param   whichSource     The source socket that received the message.
     **/
    void process_received_message( EventEnvelope & msgReceived, Socket & whichSource ) final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns a reference to self.
     **/
    [[nodiscard]]
    inline LoggerClient& self();

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The pointer to the callback structure to trigger methods on certain event.
     *          If nullptr, no callback is triggered.
     **/
    const ObserverEvents *          mCallbacks;

    /**
     * \brief   The object that processes received messages.
     **/
    ObserverMessageProcessor        mMessageProcessor;

    /**
     * \brief   The flag, indicating whether the observer was paused or not.
     **/
    bool                            mIsPaused;

    /**
     * \brief   The list of connected instances.
     **/
    areg::MapInstances              mInstances;

    /**
     * \brief   The logging database engine.
     **/
    areg::ext::LogSqliteDatabase    mLogDatabase;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LoggerClient);
};

//////////////////////////////////////////////////////////////////////////
// LoggerClient inline methods.
//////////////////////////////////////////////////////////////////////////

inline void LoggerClient::log_instance_names(std::vector<String>& names)
{
    mLogDatabase.log_instance_names(names);
}

inline void LoggerClient::log_instances(std::vector<ITEM_ID>& ids)
{
    mLogDatabase.log_instances(ids);
}

inline void LoggerClient::log_thread_names(std::vector<String>& names)
{
    mLogDatabase.log_thread_names(names);
}

inline void LoggerClient::log_threads(std::vector<ITEM_ID>& ids)
{
    mLogDatabase.log_threads(ids);
}

inline void LoggerClient::log_priority_names(std::vector<String>& names)
{
    mLogDatabase.log_priority_names(names);
}

inline void LoggerClient::log_instance_infos(std::vector< areg::ConnectedInstance>& infos)
{
    mLogDatabase.log_instance_infos(infos);
}

inline void LoggerClient::log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId)
{
    mLogDatabase.log_inst_scopes(scopes, instId);
}

inline void LoggerClient::log_messages(std::vector<SharedBuffer>& messages)
{
    mLogDatabase.log_messages(messages);
}

inline void LoggerClient::log_inst_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId /*= areg::COOKIE_ANY*/)
{
    mLogDatabase.log_inst_messages(messages, instId);
}

inline void LoggerClient::log_scope_messages(std::vector<SharedBuffer>& messages, uint32_t scopeId /*= 0*/)
{
    mLogDatabase.log_scope_messages(messages, scopeId);
}

inline void LoggerClient::log_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
{
    mLogDatabase.log_messages(messages, instId, scopeId);
}

inline LoggerClient& LoggerClient::self()
{
    return (*this);
}

} // namespace areg::logger

#endif // AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
