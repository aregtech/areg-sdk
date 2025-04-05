#ifndef AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
#define AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/LogObserverBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library.
 *              The log observer base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"
#include "areglogger/client/LogObserverApi.h"

#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * Dependencies.
 ************************************************************************/
class String;
class SharedBuffer;

/**
 * \brief   The log observer base class.
 *          The class is used to trigger callbacks on log observer events.
 *          Extend the class and implement virtual methods.
 *          If the class is instantiated, the instance is used to trigger callbacks
 *          instead of triggering Log Observer API callbacks.
 *          If the class is not instantiated, the system will try to trigger Log Observer API callbacks.
 *          The LogObserverBase class should be singleton.
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
    LogObserverBase(void);
    virtual ~LogObserverBase(void);

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the log observer and makes automatic connection to the log collector.
     *          This method should be called before any other operation. Once the operation succeeded,
     *          multiple calls of this method will have no effect.
     *          The connection information is taken from the specified configuration file.
     *          If the `configFile` parameter is empty, it uses default path `./config/areg.init`.
     * \param   configFile  The absolute or relative path to the configuration file.
     *                      If string is empty, uses `./config/areg.init` relative path.
     * \return  Returns true if successfully initialized.
     **/
    bool initialize(const String& configFile = String::EmptyString);

    /**
     * \brief   Releases the log observer and disconnects from log collector.
     **/
    void release(void);

    /**
     * \brief   Connects to the log collector service.
     *          The connection information is taken from the specified configuration file.
     *          If the `dbLocation` parameter is empty, it uses default path `./log/log.db`.
     * \param   address     The IP address of the log collector service to connect.
     *                      If string is empty, uses IP address indicated in the configuration file.
     * \param   portNr      The port number to connect. If 0, uses port number indicated in the config file.
     *                      If 0, the IP address is ignore and the value in the configuration file is used.
     * \param   dbLocation  The path to the logging database file. If NULL, uses the path specified in the config file.
     *                      The file path may as well contain masking like "./log/log_%time%.db".
     * \return  Returns true if succeeded to trigger connection. The physical connection might not be established yet.
     *          The physical connection is established when triggers callback of type FuncServiceConnected.
     **/
    bool connect(const String& address, uint16_t portNr, const String& dbLocation);

    /**
     * \brief   Disconnects from the log collector service.
     *          The method is called to disconnect from the log collector service.
     *          The method does not close the database. To close the database, use `stop()` method.
     **/
    void disconnect(void);

    /**
     * \brief   Pauses the log observer. The log observer remains connected, but no logs are written.
     *          If log observer is resume, the logs are written in the same database file.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool pause(void);

    /**
     * \brief   Resumes paused log observer, continues receiving logs. The logs are written in the same database file.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool resume(void);

    /**
     * \brief   Stops the log observer, disconnects from log collector service and closes the database file.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool stop(void);

    /**
     * \brief   Restarts the log observer, establishes the connection with the log collector service using existing connection information
     *          New database is created and the logs are written in the new database.
     * \param   dbLocation  The relative of absolute path to the logging database file.
     *                      If the path is empty, it uses the default path.
     *                      The path with mask like `log_%time%.sqlog` are allowed.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool restart(const String& dbLocation = String::EmptyString);

    /**
     * \brief   Returns true if the log observer is initialized.
     **/
    bool isInitialized(void) const;

    /**
     * \brief   Returns true if the log observer is connected to the log collector service.
     **/
    bool isConnected(void) const;

    /**
     * \brief   Returns true if the log observer is fully operable, and is able to collect and write logs.
     **/
    bool isStated(void) const;

    /**
     * \brief   Returns the IP address to connect to the log collector service.
     **/
    const String& getLoggerAddress(void) const;

    /**
     * \brief   Returns the TCP port number to connect to the log collector service.
     **/
    uint16_t getLoggerPort(void) const;

    /**
     * \brief   Return the logging state set in the configuration.
     **/
    bool getConfigLoggerEnabled(void) const;

    /**
     * \brief   Return the IP address of log collector service set in the configuration.
     **/
    String getConfigLoggerAddress(void) const;

    /**
     * \brief   Sets the IP address of the log collector service in the configuration.
     * \param   address     The IP address of the log collector service to set.
     **/
    void setConfigLoggerAddress(const String& address);

    /**
     * \brief   Return the TCP port number of log collector service set in the configuration.
     **/
    uint16_t getConfigLoggerPort(void) const;

    /**
     * \brief   Sets the TCP port number of the log collector service in the configuration.
     * \param   portNr  The port number of the log collector service to set.
     **/
    void setConfigLoggerPort(uint16_t portNr);

    /**
     * \brief   Sets the TCP/IP address and port number of the log collector service in the active configuration.
     * \param   address     The IP address of the log collector service to set.
     * \param   portNr      The TCP port number of the log collector service to set.
     **/
    void setConfigLoggerConnection(const String& address, uint16_t portNr);

    /**
     * \brief   Returns the database path name set in the configuration.
     **/
    String getConfigLoggerDatabase(void) const;

    /**
     * \brief   Sets the database path name in the configuration.
     *          The path may contain mask like `log_%time%.sqlog`.
     * \param   dbLocation  The database path to set.
     **/
    void setConfigLoggerDatabase(const String& dbLocation);

    /**
     * \brief   Returns the path of the currently active logging database. The returned path cannot contain mask.
     **/
    String getActiveDatabasePath(void) const;

    /**
     * \brief   Returns the path of the database set during initialization.
     *          The path may contain mask like `log_%time%.sqlog`.
     * \return  Returns the path of the database set during initialization.
     **/
    String getInitDatabasePath(void) const;

//////////////////////////////////////////////////////////////////////////
// Actions
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Requests the list of connected instances that make logs.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool requestInstances(void);

    /**
     * \brief   Requests the list of registered scopes of the specified connected instance.
     * \param   target  The cookie ID of the target instance to receive the list of registered scopes.
     *                  If the target is NEService::TARGET_ALL (or 0), it receives the list of scopes of all connected instances.
     *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool requestScopes(ITEM_ID target = NEService::TARGET_ALL);

    /**
     * \brief   Requests to update the priority of the logging message to receive.
     *          The indicated scopes can be scope group.
     * \param   target  The valid cookie ID of the target to update the log message priority.
     *                  This value cannot be NEService::TARGET_ALL (or 0).
     * \param   scopes  The list of scopes of scope group to update the priority.
     *                  The scope group should  end with '*'. For example 'areg_base_*'.
     *                  In this case the ID of the scope can be 0.
     * \param   count   The number of scope entries in the list.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool requestChangeScopePrio(ITEM_ID target, const sLogScope* scopes, uint32_t count);

    /**
     * \brief   Requests to save current configuration of the specified target. This is normally called when update the log priority of the instance,
     *          so that on next start the application logs message of the scopes and priorities currently set.
     * \param   target  The cookie ID of the target instance to save the configuration.
     *                  If the target is NEService::TARGET_ALL (or 0), the request is sent to all connected instances.
     *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
     * \return  Returns true if processed with success. Otherwise, returns false.
     **/
    bool requestSaveConfig(ITEM_ID target = NEService::TARGET_ALL);

    /**
     * \brief   Saves the configuration of the log observer in the configuration file.
     **/
    void saveLoggerConfig(void);

//////////////////////////////////////////////////////////////////////////
// Protected Overrides / Callbacks
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************
 * LogObserverBase overrides
 ************************************************************************/

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the IP address and port number of the log collector service set
     *          in the configuration file.
     * \param   isEnabled       The flag, indicating whether the logging service is enabled or not.
     * \param   address         The null-terminated string of the IP address of the log collector service set in the configuration file.
     * \param   port            The IP port number of the log collector service set in the configuration file.
     **/
    virtual void onLogObserverConfigured(bool isEnabled, const String & address, uint16_t port) = 0;

    /**
     * \brief   The callback of the event triggered when initializing and configuring the observer.
     *          The callback indicates the supported database, the database location or URI and
     *          the database user name.
     * \param   isEnabled       The flag, indicating whether the logging in the database is enabler or not.
     * \param   dbName          The name of the  supported database.
     * \param   dbLocation      The relative or absolute path the database. The path may contain a mask.
     * \param   dbUser          The database user to use when log in. If null or empty, the database may not require the user name.
     **/
    virtual void onLogDbConfigured(bool isEnabled, const String & dbName, const String & dbLocation, const String & dbUser) = 0;

    /**
     * \brief   The callback of the event triggered when the observer connects or disconnects from the log collector service.
     * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
     * \param   address         The IP address of the log collector service to connect or disconnect.
     * \param   port            The IP port number of the log collector service to connect or disconnect.
     **/
    virtual void onLogServiceConnected(bool isConnected, const String & address, uint16_t port) = 0;

    /**
     * \brief   The callback of the event trigger when starting or pausing the log observer.
     *          If the log observer is paused, on start it continues to write logs in the same file.
     *          If the log observer is stopped (disconnected is called), on start it creates new file.
     * \param   isStarted       The flag indicating whether the lob observer is started or paused.
     **/
    virtual void onLogObserverStarted(bool isStarted) = 0;

    /**
     * \brief   The callback of the event triggered when the logging database is created.
     * \param   dbLocation      The relative or absolute path to the logging database.
     **/
    virtual void onLogDbCreated(const String & dbLocation) = 0;

    /**
     * \brief   The callback of the event triggered when fails to send or receive message.
     **/
    virtual void onLogMessagingFailed(void) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of connected instances that make logs.
     * \param   instances   The list of the connected instances.
     **/
    virtual void onLogInstancesConnect(const TEArrayList< NEService::sServiceConnectedInstance > & instances) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of disconnected instances that make logs.
     * \param   instances   The list of IDs of the disconnected instances.
     * \param   count       The number of entries in the list.
     **/
    virtual void onLogInstancesDisconnect(const TEArrayList< NEService::sServiceConnectedInstance > & instances) = 0;

    /**
     * \brief   The callback of the event triggered when connection with the log collector service is lost.
     * \param   instances   The list of disconnected instances.
     **/
    virtual void onLogServiceDisconnected(const NEService::MapInstances& instances) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    virtual void onLogRegisterScopes(ITEM_ID cookie, const sLogScope* scopes, int count) = 0;

    /**
     * \brief   The callback of the event triggered when receive the list of previously registered scopes with new priorities.
     * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
     * \param   scopes  The list of previously registered scopes. Each entry contains the ID of the scope, message priority and the full name.
     * \param   count   The number of scope entries in the list.
     **/
    virtual void onLogUpdateScopes(ITEM_ID cookie, const sLogScope* scopes, int count) = 0;

    /**
     * \brief   The callback of the event triggered when receive message to log.
     * \param   logMessage  The structure of the message to log.
     **/
    virtual void onLogMessage(const SharedBuffer & logMessage) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    static  LogObserverBase* _theLogObserver; //!< The instance of the log observer base class.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(LogObserverBase);
};

#endif // AREG_AREGLOGGER_CLIENT_LOGOBSERVERBASE_HPP
