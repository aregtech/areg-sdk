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

//////////////////////////////////////////////////////////////////////////
// LoggerClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The observer as a log collector client object to connect to the logger service,
 *          and send-receive messages, including list of connected clients, scopes and messages.
 *          This object is instantiated as a singleton. It as well contains the pointer to the
 *          structure of callbacks to trigger when an event happens.
 **/
class LoggerClient  : public    areg::ServiceClientConnectionBase
                    , public    ConfigListener
                    , protected areg::DispatcherThread
                    , protected areg::ConnectionConsumer
                    , protected areg::RemoteMessageHandler
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
     * \brief   Returns the singleton instance of the LoggerClient
     **/
    static LoggerClient& getInstance();

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
     * \brief   Call to start the send and receive threads, and establish connection to the log collector service.
     *          If failed to connect, it triggers a timer and retries connection.
     * \param   address     The IP address or the host name of the log collector service to connect.
     *                      If the address is empty, it uses the value set in the configuration file.
     *                      If the address is empty, the port number should be areg::InvalidPort.
     * \param   portNr      The port number of the log collector service to connect. If the port number is
     *                      areg::InvalidPort, it uses the port number set in the configuration file.
     *                      If the port number is areg::InvalidPort, the address should be empty.
     * \return  Returns true if succeeded to initialize the threads and trigger the service connection.
     * \note    Either both, the 'address' and 'portNr' should be valid values or both should be invalid / empty.
     **/
    bool startLoggerClient(const areg::String & address = areg::String::EmptyString, uint16_t portNr = areg::InvalidPort);

    /**
     * \brief   Call to stop threads and disconnect log collector service.
     *          The observer will not send and receive messages.
     **/
    void stopLoggerClient();

    /**
     * \brief   Sets the pointer to the callbacks to trigger on messaging events.
     * \param   callbacks   The pointer of the callback structure to set.
     *                      The structure can have either all pointers to the callbacks
     *                      or to only certain callbacks.
     *                      If the parameter is 'nullptr' it resets all callbacks and no
     *                      callback is triggered on the events.
     **/
    void setCallbacks(const ObserverEvents * callbacks);

    /**
     * \brief   Set paused flag true or false. If log collector client is paused, it does not
     *          write logs in the file, but remain connected.
     * \param   doPause     The paused flag to set.
     **/
    void setPaused(bool doPause);

    /**
     * \brief   Returns the socket address (IP address and port number) to connect to the log collector service.
     *          The connection might be not established yet.
     **/
    const areg::SocketAddress& getAddress() const;

    /**
     * \brief   Returns true if the logging database engine is SQLite. Otherwise, returns false.
     **/
    bool isSqliteEngine() const;

    /**
     * \brief   Returns true if the observer is configured and the log collector service connection is enabled.
     **/
    bool isConfigLoggerConnectEnabled() const;

    /**
     * \brief   Returns the IP address of log collector service written in the configuration file.
     **/
    areg::String getConfigLoggerAddress() const;

    /**
     * \brief   Returns the IP port number of log collector service written in the configuration file.
     **/
    uint16_t getConfigLoggerPort() const;

    /**
     * \brief   Sets the IP address and port number of the log collector service to connect.
     * \param   address     The IP address of the log collector service to connect.
     * \param   portNr      The port number of the log collector service to connect.
     * \return  Returns true if succeeded to set the connection data. Otherwise, returns false.
     **/
    bool setConfigLoggerConnection(const areg::String& address, uint16_t portNr);

    /**
     * \brief   Generates and sends the message to query list of connected clients.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestConnectedInstances();

    /**
     * \brief   Generates and sends the message to query list of scopes.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is areg::TARGET_ALL.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is areg::TARGET_ALL.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestScopes(const ITEM_ID& target = areg::TARGET_ALL);

    /**
     * \brief   Generates and sends the message to update the scope priority.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is areg::TARGET_ALL.
     * \param   scopes  The list of scopes or scope group to update the log message priority.
     *                  Each entry contains scope name, scope ID and the scope message priority.
     *                  The ID can be 0 if the name refers to a scope group.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is areg::TARGET_ALL.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestChangeScopePrio(const areg::ScopeNames& scopes, const ITEM_ID& target = areg::TARGET_ALL);

    /**
     * \brief   Generates and sends the message to request to save configuration current state,
     *          so that on the next start the application starts with the configuration state.
     *          Normally, this is used when change scope message priority.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is areg::TARGET_ALL.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is areg::TARGET_ALL.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestSaveConfiguration(const ITEM_ID & target = areg::TARGET_ALL);

    /**
     * \brief   Creates of opens the database for the logging. If specified path is null or empty,
     *          if uses the location specified in the configuration file.
     * \param   dbPath      The relative or absolute path to the logging database file.
     *                      If null or empty, uses the location specified in the configuration file.
     * \return  Returns true if succeeded to initialize the database.
     **/
    bool openLoggingDatabase(const char* dbPath = nullptr);

    /**
     * \brief   Closes previously opened logging database.
     **/
    void closeLoggingDatabase();

    /**
     * \brief   Returns the path to the active logging database.
     **/
    areg::String getActiveDatabasePath() const;

    /**
     * \brief   Returns the path to the initial logging database.
     **/
    areg::String getInitialDatabasePath() const;

    /**
     * \brief   Returns the path specified in configuration file.
     **/
    areg::String getConfigDatabasePath() const;

    /**
     * \brief   Sets the path to the database file. The path can contain mask.
     *          If the path is empty, it uses the path specified in the configuration file.
     *          If the path contains mask, each time it creates a new database.
     * \param   dbPath      The relative or absolute path to the logging database file.
     * \param   enable      The flag to set whether to enable or disable database logging.
     * \return  Returns true if succeeded to set the database path. Otherwise, returns false.
     **/
    bool setConfigDatabasePath(const areg::String& dbPath, bool enable);

    /**
     * \brief   Returns the logging database location path set in configuration file.
     **/
    areg::String getConfigDatabaseLocation() const;

    /**
     * \brief   Sets the logging database location path.
     * \param   dbLocation  The location of logging database.
     * \return  Returns true if operation succeeded.
     **/
    bool setConfigDatabaseLocation(const areg::String& dbLocation);

    /**
     * \brief   Returns the logging database name set in configuration file.
     **/
    areg::String getConfigDatabaseName() const;

    /**
     * \brief   Sets the logging database name in the configuration file.
     * \param   dbName      The name of logging database.
     * \return  Returns true if operation succeeded.
     **/
    bool setConfigDatabaseName(const areg::String& dbName);

    /**
     * \brief   Sets logging connection flag in the configuration file.
     **/
    bool setConfigLoggerConnectEnabled(bool isEnabled);

    /**
     * \brief   Save current configuration of the log observer to the configuration file.
     **/
    void saveConfiguration();

    /**
     * \brief   Call to query and get list of names of connected instances from log database.
     * \param   names   On output, contains the list of names of connected instances.
     **/
    inline void getLogInstanceNames(std::vector<areg::String>& names);

    /**
     * \brief   Call to query and get list of IDs of connected instances from log database
     * \param   ids     On output, contains the list of IDs of connected instances.
     **/
    inline void getLogInstances(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to query and get list of names of threads of the connected instances from log database.
     * \param   names   On output, contains the list of all thread names that sent messages.
     **/
    inline void getLogThreadNames(std::vector<areg::String>& names);

    /**
     * \brief   Call to query and get list of IDs of threads of the connected instances from log database.
     * \param   ids     On output, contains the list of all thread IDs that sent messages.
     **/
    inline void getLogThreads(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to get the list of log priorities.
     * \param   names   On output, contains the names of all priorities.
     **/
    inline void getPriorityNames(std::vector<areg::String>& names);

    /**
     * \brief   Call to query and get information of connected instances from log database.
     *          This query will receive list of all registered instances.
     * \param   infos   On output, contains the list of information of all registered instances in database.
     **/
    inline void getLogInstanceInfos(std::vector< areg::ConnectedInstance>& infos);

    /**
     * \brief   Call to query and get information of log scopes of specified instance from log database.
     *          This query will receive list of all registered scopes.
     * \param   scopes  On output, contains the list of all registered scopes in database related with the specified instance ID.
     * \param   instID  The ID of the instance.
     **/
    inline void getLogInstScopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId);

    /**
     * \brief   Call to get all log messages from log database.
     * \param   messages   On output, contains the list of all log messages.
     **/
    inline void getLogMessages(std::vector<areg::SharedBuffer>& messages);

    /**
     * \brief   Call to get log messages of the specified instance from log database.
     *          If `instId` is `areg::COOKIE_ANY` it receives the list of all instances
     *          similar to the call to `getLogMessages()`.
     * \param   messages    On output, contains the list of log messages of the specified instance.
     * \param   instId  The ID of the instance to get log messages.
     *                  If `areg::COOKIE_ANY` it receives log messages of all instances.
     **/
    inline void getLogInstMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId = areg::COOKIE_ANY);

    /**
     * \brief   Call to get log messages of the specified scope from log database.
     *          If `scopeId` is `0` it receives the list of all scopes
     *          similar to the call to `getLogMessages()`.
     * \param   messages    On output, contains the list of log messages of the specified scope.
     * \param   scopeId     The ID of the scope to get log messages.
     *                      If `0` it receives log messages of all scopes.
     **/
    inline void getLogScopeMessages(std::vector<areg::SharedBuffer>& messages, uint32_t scopeId = 0);

    /**
     * \brief   Call to get log messages of the specified instance and log scope ID from log database.
     *          If `instId` is `areg::COOKIE_ANY` and `scopeId` is `0`, it receives the list of all logs
     *          similar to the call to `getLogMessages()`.
     * \param   messages    On output, contains the list of log messages of the specified instance and scope.
     * \param   instId      The ID of the instance to get log messages.
     *                      If `areg::COOKIE_ANY` it receives log messages of all instances.
     * \param   scopeId     The ID of the scope to get log messages.
     *                      If `0` it receives log messages of all scopes.
     **/
    inline void getLogMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ConfigListener interface overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    void prepareSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    void postSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config  The instance of configuration manager.
     **/
    void prepareReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    void postReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    void onSetupConfiguration(const areg::ListProperties& listReadonly, const areg::ListProperties& listWritable, ConfigManager& config) override;

/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    bool postEvent(areg::Event& eventElem) override;

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    void readyForEvents( bool isReady ) override;

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    bool connectServiceHost() override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    void disconnectServiceHost() override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    void onServiceExit() override;

/************************************************************************/
// ConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    void connectedRemoteServiceChannel(const areg::Channel& channel) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    void disconnectedRemoteServiceChannel(const areg::Channel& channel) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    void lostRemoteServiceChannel(const areg::Channel& channel) override;

/************************************************************************/
// RemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    void failedSendMessage( const areg::RemoteMessage & msgFailed, areg::Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    void failedReceiveMessage( areg::Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    void failedProcessMessage( const areg::RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    void processReceivedMessage( const areg::RemoteMessage & msgReceived, areg::Socket & whichSource ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    inline LoggerClient& self();

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The pointer to the callback structure to trigger methods on certain event.
     *          If nullptr, no callback is triggered.
     **/
    const ObserverEvents *     mCallbacks;

    /**
     * \brief   The object that processes received messages.
     **/
    ObserverMessageProcessor    mMessageProcessor;

    /**
     * \brief   The flag, indicating whether the observer was paused or not.
     **/
    bool                        mIsPaused;

    /**
     * \brief   The list of connected instances.
     **/
    areg::MapInstances     mInstances;

    /**
     * \brief   The logging database engine.
     **/
    LogSqliteDatabase           mLogDatabase;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LoggerClient);
};

//////////////////////////////////////////////////////////////////////////
// LoggerClient inline methods.
//////////////////////////////////////////////////////////////////////////

inline void LoggerClient::getLogInstanceNames(std::vector<areg::String>& names)
{
    mLogDatabase.getLogInstanceNames(names);
}

inline void LoggerClient::getLogInstances(std::vector<ITEM_ID>& ids)
{
    mLogDatabase.getLogInstances(ids);
}

inline void LoggerClient::getLogThreadNames(std::vector<areg::String>& names)
{
    mLogDatabase.getLogThreadNames(names);
}

inline void LoggerClient::getLogThreads(std::vector<ITEM_ID>& ids)
{
    mLogDatabase.getLogThreads(ids);
}

inline void LoggerClient::getPriorityNames(std::vector<areg::String>& names)
{
    mLogDatabase.getPriorityNames(names);
}

inline void LoggerClient::getLogInstanceInfos(std::vector< areg::ConnectedInstance>& infos)
{
    mLogDatabase.getLogInstanceInfos(infos);
}

inline void LoggerClient::getLogInstScopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId)
{
    mLogDatabase.getLogInstScopes(scopes, instId);
}

inline void LoggerClient::getLogMessages(std::vector<areg::SharedBuffer>& messages)
{
    mLogDatabase.getLogMessages(messages);
}

inline void LoggerClient::getLogInstMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId /*= areg::COOKIE_ANY*/)
{
    mLogDatabase.getLogInstMessages(messages, instId);
}

inline void LoggerClient::getLogScopeMessages(std::vector<areg::SharedBuffer>& messages, uint32_t scopeId /*= 0*/)
{
    mLogDatabase.getLogScopeMessages(messages, scopeId);
}

inline void LoggerClient::getLogMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
{
    mLogDatabase.getLogMessages(messages, instId, scopeId);
}

inline LoggerClient& LoggerClient::self()
{
    return (*this);
}

#endif // AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
