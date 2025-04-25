#ifndef AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
#define AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LoggerClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log collector service connection client.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areg/component/NEService.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"
#include "areg/persist/IEConfigurationListener.hpp"

#include "areg/logging/NELogging.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"

#include "areglogger/client/private/ObserverMessageProcessor.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
struct sObserverEvents;

//////////////////////////////////////////////////////////////////////////
// LoggerClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The observer as a log collector client object to connect to the logger service,
 *          and send-receive messages, including list of connected clients, scopes and messages.
 *          This object is instantiated as a singleton. It as well contains the pointer to the
 *          structure of callbacks to trigger when an event happens.
 **/
class LoggerClient  : public    ServiceClientConnectionBase
                    , public    IEConfigurationListener
                    , protected DispatcherThread
                    , protected IEServiceConnectionConsumer
                    , protected IERemoteMessageHandler
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
    static constexpr std::string_view                   ThreadName      { "LogObserverThread" };

    //!< The prefix to add to the send and receive threads.
    static constexpr std::string_view                   ThreadPrefix    { "Observer" };

    //!< The ID of the target to send and receive messages. The target is the log collector service cookie ID.
    static constexpr ITEM_ID                            TargetID        { NEService::COOKIE_LOGGER };

    //!< The type of the service. It is a log collector service.
    static constexpr NERemoteService::eRemoteServices   ServiceType     { NERemoteService::eRemoteServices::ServiceLogger };

    //!< The connection type. At the moment only TCP/IP
    static constexpr NERemoteService::eConnectionTypes  ConnectType     { NERemoteService::eConnectionTypes::ConnectTcpip };

    //!< The message source. It is marked as an observer.
    static constexpr NEService::eMessageSource          SourceType      { NEService::eMessageSource::MessageSourceObserver };

//////////////////////////////////////////////////////////////////////////
// Static methods.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of the LoggerClient
     **/
    static LoggerClient& getInstance(void);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor and destructor.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient(void);
    virtual ~LoggerClient(void) = default;

//////////////////////////////////////////////////////////////////////////
// Public actions.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to start the send and receive threads, and establish connection to the log collector service.
     *          If failed to connect, it triggers a timer and retries connection.
     * \param   address     The IP address or the host name of the log collector service to connect.
     *                      If the address is empty, it uses the value set in the configuration file.
     *                      If the address is empty, the port number should be NESocket::InvalidPort.
     * \param   portNr      The port number of the log collector service to connect. If the port number is
     *                      NESocket::InvalidPort, it uses the port number set in the configuration file.
     *                      If the port number is NESocket::InvalidPort, the address should be empty.
     * \return  Returns true if succeeded to initialize the threads and trigger the service connection.
     * \note    Either both, the 'address' and 'portNr' should be valid values or both should be invalid / empty.
     **/
    bool startLoggerClient(const String & address = String::EmptyString, uint16_t portNr = NESocket::InvalidPort);

    /**
     * \brief   Call to stop threads and disconnect log collector service.
     *          The observer will not send and receive messages.
     **/
    void stopLoggerClient(void);

    /**
     * \brief   Sets the pointer to the callbacks to trigger on messaging events.
     * \param   callbacks   The pointer of the callback structure to set.
     *                      The structure can have either all pointers to the callbacks
     *                      or to only certain callbacks.
     *                      If the parameter is 'nullptr' it resets all callbacks and no
     *                      callback is triggered on the events.
     **/
    void setCallbacks(const sObserverEvents * callbacks);

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
    const NESocket::SocketAddress& getAddress(void) const;

    /**
     * \brief   Returns true if the logging database engine is SQLite. Otherwise, returns false.
     **/
    bool isSqliteEngine(void) const;

    /**
     * \brief   Returns true if the observer is configured and the log collector service connection is enabled.
     **/
    bool isConfigLoggerConnectEnabled(void) const;

    /**
     * \brief   Returns the IP address of log collector service written in the configuration file.
     **/
    String getConfigLoggerAddress(void) const;

    /**
     * \brief   Returns the IP port number of log collector service written in the configuration file.
     **/
    uint16_t getConfigLoggerPort(void) const;

    /**
     * \brief   Sets the IP address and port number of the log collector service to connect.
     * \param   address     The IP address of the log collector service to connect.
     * \param   portNr      The port number of the log collector service to connect.
     * \return  Returns true if succeeded to set the connection data. Otherwise, returns false.
     **/
    bool setConfigLoggerConnection(const String& address, uint16_t portNr);

    /**
     * \brief   Generates and sends the message to query list of connected clients.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestConnectedInstances(void);

    /**
     * \brief   Generates and sends the message to query list of scopes.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is NEService::COOKIE_ANY.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is NEService::COOKIE_ANY.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestScopes(const ITEM_ID& target = NEService::COOKIE_ANY);

    /**
     * \brief   Generates and sends the message to update the scope priority.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is NEService::COOKIE_ANY.
     * \param   scopes  The list of scopes or scope group to update the log message priority.
     *                  Each entry contains scope name, scope ID and the scope message priority.
     *                  The ID can be 0 if the name refers to a scope group.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is NEService::COOKIE_ANY.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestChangeScopePrio(const NELogging::ScopeNames& scopes, const ITEM_ID& target = NEService::COOKIE_ANY);

    /**
     * \brief   Generates and sends the message to request to save configuration current state,
     *          so that on the next start the application starts with the configuration state.
     *          Normally, this is used when change scope message priority.
     *          The message is sent either to certain target or to all connected clients
     *          if the target is NEService::COOKIE_ANY.
     * \param   target  The ID of the target to send the message.
     *                  The message is sent to all clients if the target is NEService::COOKIE_ANY.
     * \return  Returns true if processed the request with success. Otherwise, returns false.
     **/
    bool requestSaveConfiguration(const ITEM_ID & target = NEService::COOKIE_ANY);

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
    void closeLoggingDatabase(void);

    /**
     * \brief   Returns the path to the active logging database.
     **/
    String getActiveDatabasePath(void) const;

    /**
     * \brief   Returns the path to the initial logging database.
     **/
    String getInitialDatabasePath(void) const;

    /**
     * \brief   Returns the path specified in configuration file.
     **/
    String getConfigDatabasePath(void) const;

    /**
     * \brief   Sets the path to the database file. The path can contain mask.
     *          If the path is empty, it uses the path specified in the configuration file.
     *          If the path contains mask, each time it creates a new database.
     * \param   dbPath      The relative or absolute path to the logging database file.
     * \param   enable      The flag to set whether to enable or disable database logging.
     * \return  Returns true if succeeded to set the database path. Otherwise, returns false.
     **/
    bool setConfigDatabasePath(const String& dbPath, bool enable);

    /**
     * \brief   Returns the logging database location path set in configuration file.
     **/
    String getConfigDatabaseLocation(void) const;

    /**
     * \brief   Sets the logging database location path.
     * \param   dbLocation  The location of logging database.
     * \return  Returns true if operation succeeded.
     **/
    bool setConfigDatabaseLocation(const String& dbLocation);

    /**
     * \brief   Returns the logging database name set in configuration file.
     **/
    String getConfigDatabaseName(void) const;

    /**
     * \brief   Sets the logging database name in the configuration file.
     * \param   dbName      The name of logging database.
     * \return  Returns true if operation succeeded.
     **/
    bool setConfigDatabaseName(const String& dbName);

    /**
     * \brief   Sets logging connection flag in the configuration file.
     **/
    bool setConfigLoggerConnectEnabled(bool isEnabled);

    /**
     * \brief   Save current configuration of the log observer to the configuration file.
     **/
    void saveConfiguration(void);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEConfigurationListener interface overrides
/************************************************************************/

    /**
     * \brief   Called by configuration manager before the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after the configuration is saved in the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postSaveConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager before the configuration is loaded from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void prepareReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager when configuration is completed to load data from the file.
     * \param   config  The instance of configuration manager.
     **/
    virtual void postReadConfiguration(ConfigManager& config) override;

    /**
     * \brief   Called by configuration manager after setting read-only and writable properties.
     *          For example, when the default configuration is set.
     * \param   listReadonly    The list of read-only properties to set in the configuration.
     * \param   listWritable    The list of module / process specific properties to set in the configuration;
     * \param   config          The instance of configuration manager.
     **/
    virtual void onSetupConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigManager& config) override;

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
    virtual bool postEvent(Event& eventElem) override;

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool connectServiceHost( void ) override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void disconnectServiceHost( void ) override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    virtual void onServiceExit(void) override;

/************************************************************************/
// IEServiceConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel(const Channel& channel) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed, Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    virtual void failedReceiveMessage( Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    inline LoggerClient& self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The pointer to the callback structure to trigger methods on certain event.
     *          If nullptr, no callback is triggered.
     **/
    const sObserverEvents *     mCallbacks;

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
    NEService::MapInstances     mInstances;

    /**
     * \brief   The logging database engine.
     **/
    LogSqliteDatabase           mLogDatabase;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(LoggerClient);
};

//////////////////////////////////////////////////////////////////////////
// LoggerClient inline methods.
//////////////////////////////////////////////////////////////////////////

inline LoggerClient& LoggerClient::self(void)
{
    return (*this);
}

#endif // AREG_AREGLOGGER_CLIENT_PRIVATE_LOGGERCLIENT_HPP
