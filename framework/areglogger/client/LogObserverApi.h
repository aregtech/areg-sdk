#ifndef AREG_AREGLOGGER_CLIENT_LOGOBSERVERAPI_H
#define AREG_AREGLOGGER_CLIENT_LOGOBSERVERAPI_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/LogObserverApi.h
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"

/**
 * \brief   The message source is a client application.
 **/
#define SOURCE_CLIENT       1
/**
 * \brief   The message source is a test application.
 **/
#define SOURCE_TEST         8
/**
 * \brief   The message source is a simulation application.
 **/
#define SOURCE_SIMULATION   16

/**
 * \brief   The bitness of the message source application. These indicate either 32- or 64-bit application.
 **/
#define BITNESS_32BITS      32
#define BITNESS_64BITS      64

/**
 * \brief   The maximum length of the names, including null-terminated character in the name.
 **/
#define LENGTH_NAME         128
/**
 * \brief   The maximum length of the file locations, including null-terminated character in the name.
 **/
#define LENGTH_LOCATION     512
/**
 * \brief   The maximum length of scope name, including null-terminated character in the name.
 **/
#define LENGTH_SCOPE        256
/**
 * \brief   The maximum length log message text, including null-terminated character in the name.
 **/
#define LENGTH_MESSAGE      512

#define ID_IGNORE           0u

/**
 * \brief   The structure of the connected instance.
 **/
struct LogInstance
{
    /* The type of the message source application. Either client, test or simulation */
    uint32_t    liSource;
    /* The bitness of the application, should be either 32 or 64 */
    uint32_t    liBitness;
    /* The cookie ID of the connected client. */
    ITEM_ID     liCookie;
    /* The connection timestamp */
    TIME64      liTimestamp;
    /* The name of the connected application */
    char        liName[LENGTH_NAME];
    /* The name of the file location. */
    char        liLocation[LENGTH_LOCATION];
};

/**
 * \brief   The type of the log message
 **/
enum LogType
{
    /* The enter scope log message */
      LogScopeEnter     = 1
    /* The log message text */
    , MessageText    = 2
    /* The exit scope log message */
    , LogScopeExit      = 4
};

/**
 * \brief   The priority of logs. Can be in combination with PrioScope
 **/
enum LogPriority
{
    /* Invalid priority log message. */
      PrioInvalid   = 0x0000
    /* No priority, should not log any message. */
    , PrioNotSet    = 0x0001
    /* Logs scope enter and scope exit messages. */
    , PrioScope     = 0x0010
    /* Logs fatal error messages. Can be in combination with PrioScope. */
    , PrioFatal     = 0x0020
    /* Logs fatal error and error messages. Can be in combination with PrioScope. */
    , PrioError     = 0x0040
    /* Logs fatal error, error and warning messages. Can be in combination with PrioScope. */
    , PrioWarning   = 0x0080
    /* Logs fatal error, error, warning and info messages. Can be in combination with PrioScope. */
    , PrioInfo      = 0x0100
    /* Logs fatal error, error, warning, info and debug messages. Can be in combination with PrioScope. */
    , PrioDebug     = 0x0200
    /* Ignore the message priority, force to log. */
    , PrioIgnore    = 0x0400
    /* Logs a message without priority. Can be in combination with PrioScope. */
    , PrioAny       = 0x0FF0
};

/**
 * \brief   The structure of scopes
 **/
struct ScopeInfo
{
    /* The ID of the scope. Can be 0 if unknown or if indicates to scope group. */
    uint32_t    lsId;
    /* The priority of the scope to log messages. These are values of LogPriority. Can be set bitwise. */
    uint32_t    lsPrio;
    /* The name of the scope or scope group. The scope groups are ending with '*'. For example 'areg_base_*' */
    char        lsName[LENGTH_SCOPE];
};

/**
 * \brief   The structure of the logging message.
 **/
struct LogRecord
{
    /* The type of the message: scope enter, scope exit or message text. */
    LogType         msgType;
    /* The priority of the message to log. */
    LogPriority     msgPriority;
    /* The ID of the message source. This value is indicated in the LogInstance::liCookie */
    ITEM_ID         msgSource;
    /* The cookie ID of the message. This value is indicated in the LogInstance::liCookie */
    ITEM_ID         msgCookie;
    /* The ID of the process, running on the local machine. */
    ITEM_ID         msgModuleId;
    /* The ID of the thread where the log has been generated. */
    ITEM_ID         msgThreadId;
    /* The timestamp where the log was generated. */
    TIME64          msgTimestamp;
    /* The timestamp when the log data received */
    TIME64          msgReceived;
    /* The duration in microseconds since the scope message was instantiated */
    uint32_t        msgDuration;
    /* The ID of the scope that generated message. Same as indicated in ScopeInfo::lsId. */
    uint32_t        msgScopeId;
    /* The ID of the session, which is used to differentiate messages of the same scope. */
    uint32_t        msgSessionId;
    /* The text of generated log message. */
    char            msgLogText[LENGTH_MESSAGE];
    /* The name of the thread, if set, where the message was generated. */
    char            msgThread[LENGTH_NAME];
    /* The name of the application that generated the message. Same as in LogInstance::liName */
    char            msgModule[LENGTH_NAME];
};

/**
 * \brief   The states of the log observer.
 **/
enum ObserverState
{
    /* The lob observer is uninitialized */
      ObserverUninitialized = 0
    /* The log observer is initialized, but disconnected / stopped. */
    , ObserverDisconnected  = 1
    /* The log observer is connected and started. */
    , ObserverConnected     = 3
    /* The log observer is connected, but paused, i.e. receives, but does not write or forward logs. */
    , ObserverPaused        = 7
};

/**
 * \brief   The callback of the event triggered when initializing and configuring the observer.
 *          The callback indicates the IP address and port number of the log collector service set
 *          in the configuration file.
 *  isEnabled       The flag, indicating whether the logging service is enabled or not.
 *  address         The null-terminated string of the IP address of the log collector service set in the configuration file.
 *  port            The IP port number of the log collector service set in the configuration file.
 **/
typedef void (*FuncObserverConfigured)(bool /*isEnabled*/, const char* /*address*/, uint16_t /*port*/);

/**
 * \brief   The callback of the event triggered when initializing and configuring the observer.
 *          The callback indicates the supported database, the database location or URI and
 *          the database user name.
 *  isEnabled       The flag, indicating whether the logging in the database is enabler or not.
 *  dbName          The name of the  supported database.
 *  dbLocation      The relative or absolute path the database. The path may contain a mask.
 *  dbUser          The database user to use when log in. If null or empty, the database may not require the user name.
 **/
typedef void (*FuncLogDbConfigured)(bool /*isEnabled*/, const char* /*dbName*/, const char* /*dbLocation*/, const char* /*dbUser*/);

/**
 * \brief   The callback of the event triggered when the observer connects or disconnects from the log collector service.
 *  isConnected     Flag, indicating whether observer is connected or disconnected.
 *  address         The IP address of the log collector service to connect or disconnect.
 *  port            The IP port number of the log collector service to connect or disconnect.
 **/
typedef void (*FuncServiceConnected)(bool /*isConnected*/, const char * /*address*/, uint16_t /*port*/);

/**
 * \brief   The callback of the event trigger when starting or pausing the log observer.
 *          If the log observer is paused, on start it continues to write logs in the same file.
 *          If the log observer is stopped (disconnected is called), on start it creates new file.
 *  isStarted       The flag indicating whether the lob observer is started or paused.
 **/
typedef void (*FuncObserverStarted)(bool /*isStarted*/);

/**
 * \brief   The callback of the event triggered when the logging database is created.
 *  dbLocation      The relative or absolute path to the logging database.
 **/
typedef void (*FuncLogDbCreated)(const char* /*dbLocation*/);

/**
 * \brief   The callback of the event triggered when fails to send or receive message.
 **/
typedef void (*FuncMessagingFailed)();

/**
 * \brief   The callback of the event triggered when receive the list of connected instances that make logs.
 *  instances   The pointer to the list of the connected instances.
 *  count       The number of entries in the list.
 **/
typedef void (*FuncInstancesConnect)(const LogInstance* /*instances*/, uint32_t /*count*/);

/**
 * \brief   The callback of the event triggered when receive the list of disconnected instances that make logs.
 *  instances   The pointer to the list of IDs of the disconnected instances.
 *  count       The number of entries in the list.
 **/
typedef void (*FuncInstancesDisconnect)(const ITEM_ID * /*instances*/, uint32_t /*count*/);

/**
 * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
 *  cookie  The cookie ID of the connected instance / application. Same as LogInstance::liCookie
 *  scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
 *  count   The number of scope entries in the list.
 **/
typedef void (*FuncLogRegisterScopes)(ITEM_ID /*cookie*/, const ScopeInfo* /*scopes*/, uint32_t /*count*/);

/**
 * \brief   The callback of the event triggered when receive the list of previously registered scopes with new priorities.
 *  cookie  The cookie ID of the connected instance / application. Same as LogInstance::liCookie
 *  scopes  The list of previously registered scopes. Each entry contains the ID of the scope, message priority and the full name.
 *  count   The number of scope entries in the list.
 **/
typedef void (*FuncLogUpdateScopes)(ITEM_ID /*cookie*/, const ScopeInfo* /*scopes*/, uint32_t /*count*/);

/**
 * \brief   The callback of the event triggered when receive message to log.
 *  log_message  The structure of the message to log.
 **/
typedef void (*FuncLogMessage)(const LogRecord * /*log_message*/);

/**
 * \brief   The callback of the event triggered when receive remote message to log.
 *          The buffer indicates to the areg::LogEntry structure.
 *  logBuffer   The pointer to the areg::LogEntry structure to log messages.
 *  size        The size of the buffer with log message.
 **/
typedef void (*FuncLogMessageEx)(const unsigned char* /*logBuffer*/, uint32_t /*size*/);

/**
 * \brief   The structure of the callbacks / events to set when send or receive messages.
 **/
struct ObserverEvents
{
    /* The callback to the trigger when the observer is initialized and configured, and the information of the log collector is available. */
    FuncObserverConfigured  evtObserverConfigured;
    /* The callback to trigger when the observer is initialized and configured, and the information of logging database is available. */
    FuncLogDbConfigured     evtLogDbConfigured;
    /* The callback to trigger when connect or disconnect log collector service. */
    FuncServiceConnected    evtServiceConnected;
    /* The callback to trigger when log observer is started or paused. */
    FuncObserverStarted     evtLoggingStarted;
    /* The callback to trigger when the logging database is created. */
    FuncLogDbCreated        evtLogDbCreated;
    /* The callback to trigger when fails to send or receive message. */
    FuncMessagingFailed     evtMessagingFailed;
    /* The callback to trigger when receive list of connected instances that make logs. */
    FuncInstancesConnect    evtInstConnected;
    /* The callback to trigger when receive list of IDs of the disconnected instances that make logs. */
    FuncInstancesDisconnect evtInstDisconnected;
    /* The callback to trigger when receive list of registered scopes. */
    FuncLogRegisterScopes   evtLogRegisterScopes;
    /* The callback to trigger when receive list of updated scope priorities */
    FuncLogUpdateScopes     evtLogUpdatedScopes;
    /* The callback to trigger when receive a message to log. If is set and not null, the 'evtLogMessageEx' callback is ignored. */
    FuncLogMessage          evtLogMessage;
    /* The callback to trigger when receive remote message to log. To use, set the 'evtLogMessage' callback null. */
    FuncLogMessageEx        evtLogMessageEx;
};

/**
 * \brief   Call to initialize the log observer internals. This function should
 *          be called before any other. The multiple call of this function will not
 *          have any effect.
 * \param   configFilePath  Relative or absolute path to the application configuration file.
 *                          If NULL, it uses the default location of the config file './config/areg.init'
 * \returns Returns true, if succeeded to initialize internals. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_initialize(const ObserverEvents * callbacks, const char * configFilePath /* = NULL */);

/**
 * \brief   Call to release the log observer internals and release resources. This function should
 *          be called at the end of application run. Once the observer is uninitialized, it will be
 *          not possible to use any other API and trigger remote methods.
 **/
LOGGER_API void log_observer_release();

/**
 * \brief   Call to trigger TCP/IP connection with the log collector service. Either specify the IP address and the port number
 *          of the log collector service to connect, or pass NULL to use settings indicated in the LOGconfiguration file.
 * \param   dbPath      The path to the logging database file. If NULL, uses the path specified in the config file.
 *                      The file path may as well contain masking like "./log/log_%time%.db".
 * \param   ipAddress   The IP address or the host name of the log collector service. If NULL, uses IP address indicated in the config file.
 *                      If NULL, the port number is ignored and the value in the configuration file is used.
 * \param   portNr      The port number to connect. If 0, uses port number indicated in the config file.
 *                      If 0, the IP address is ignore and the value in the configuration file is used.
 * \return  Returns true if succeeded to trigger connection. The physical connection might not be established yet.
 *          The physical connection is established when triggers callback of type FuncServiceConnected.
 * \note    The 'ipAddress' and 'portNr' parameters should be either both valid or they are ignored and the default values
 *          indicated in the configuration file are used.
 **/
LOGGER_API bool log_observer_connect_logger(const char * dbPath /* = NULL */, const char* ipAddress /* = NULL */, uint16_t portNr /* = NULL */);

/**
 * \brief   Call to trigger disconnect from log collector service.
 *          After disconnecting the callback of FuncServiceConnected type is triggered.
 **/
LOGGER_API void log_observer_disconnect_logger();

/**
 * \brief   Call to pause started or start paused log observer. By default, when log observer is connected, it is in started state.
 *          When call log_observer_pause_logging(true) it pauses the log observer and no messages are logged.
 *          When call log_observer_pause_logging(false) it resumes the log observer and continues logging messages in the same file.
 *          On start, the log observer resumes and continues writing logs.
 * \param   doPause     The flag to set to indicate whether the log observer should pause or resume writing logs.
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_pause_logging(bool doPause);

/**
 * \brief   Call to stop or restart logging messages to log messages in the new database.
 *          By default, when log observer is connected, it creates a database and starts logging.
 *          When call log_observer_stop_logging(true) it stops the log observer and closes the database. The database path can be empty.
 *          When call log_observer_stop_logging(false) it creates new database. If database path is specified, it creates a new database in the specified path.
 *          If the database path is empty, it creates a new database in the location specified by previous start.
 *          If the database path contains mask like "./log/log_%time%.db", each time it will create a new database.
 * \param   doStop      The flag to set to indicate whether the log observer should stop or start writing logs.
 * \param   dbPath      The path to the logging database file. If NULL, uses the path specified in the config file.
 *                      The file path may as well contain masking like "./log/log_%time%.db".
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_stop_logging(bool doStop, const char* dbPath /* = NULL*/);

/**
 * \brief   Call to get the current state of the log observer.
 **/
LOGGER_API ObserverState log_observer_current_state();

/**
 * \brief   Returns true if log observer is initialized and the log collector service connection is triggered.
 *          Otherwise, it returns false.
 *          The method does not indicate whether the observer is connected to the log collector service or not.
 **/
LOGGER_API bool log_observer_is_initialized();

/**
 * \brief   Returns true if log observer is initialize and connected to the log collector service.
 *          Otherwise, it returns false.
 **/
LOGGER_API bool log_observer_is_connected();

/**
 * \brief   Returns true, if log observer is initialized, connected, receives and writes or forwards logs.
 *          In all other cases it returns false.
 **/
LOGGER_API bool log_observer_is_started();

/**
 * \brief   Returns the current IP address of the log collector service to connect.
 *          Returns empty string, if not configured or the IP address is unknown
 **/
LOGGER_API const char * log_observer_logger_address();

/**
 * \brief   Returns the current IP port of the log collector service to connect.
 *          Return 0 (invalid port), if not configured or the port number is unknown
 **/
LOGGER_API uint16_t log_observer_logger_port();

/**
 * \brief   Return true if log collector TCP/IP connection is enabled in the configuration file.
 *          Otherwise, returns false.
 **/
LOGGER_API bool log_observer_config_logger_enabled();

/**
 * \brief   On exit, the addrBuffer contains the IP address of the log collector registered in the configuration file.
 * \param   addrBuffer  Should be valid pointer of the buffer to write the IP address, the minimum size should be 16 bytes.
 * \param   space       The length of the buffer to write the IP address, inclusive the null-terminated character at the end.
 * \return  Returns true, if succeeds to write the address. Returns false, if the log observer is not configured,
 *          or the address is not specified in the configuration, or the buffer is not big enough to write address.
 **/
LOGGER_API bool log_observer_config_logger_address(char * addrBuffer, uint32_t space);

/**
 * \brief   Returns the configured IP port of the log collector service to connect,
 *          i.e. the IP port saved in configuration file. Returns 0 (invalid port),
 *          if the log observer is not configured or the port number is not specified in configuration.
 **/
LOGGER_API unsigned short log_observer_config_logger_port();

/**
 * \brief   Call to request the list of connected instances.
 *          The callback of FuncLogInstances type is triggered when receive the list of connected instances.
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_request_instances();

/**
 * \brief   Call to receive the list of registered scopes of the specified connected instance.
 * \param   target  The cookie ID of the target instance to receive the list of registered scopes.
 *                  If the target is ID_IGNORE (or 0), it receives the list of scopes of all connected instances.
 *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_request_scopes(ITEM_ID target);

/**
 * \brief   Call to update the priority of the logging message to receive.
 *          The indicated scopes can be scope group.
 * \param   target  The valid cookie ID of the target to update the log message priority.
 *                  This value cannot be ID_IGNORE (or 0).
 * \param   scopes  The list of scopes of scope group to update the priority.
 *                  The scope group should  end with '*'. For example 'areg_base_*'.
 *                  In this case the ID of the scope can be 0.
 * \param   count   The number of scope entries in the list.
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_request_change_scope_prio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count);

/**
 * \brief   Call to save current configuration of the specified target. This is normally called when update the log priority of the instance,
 *          so that on next start the application logs message of the scopes and priorities currently set.
 * \param   target  The cookie ID of the target instance to save the configuration.
 *                  If the target is ID_IGNORE (or 0), the request is sent to all connected instances.
 *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
 * \return  Returns true if processed with success. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_request_save_config(ITEM_ID target);

/**
 * \brief   Call to get active database full path.
 * \param   dbPath  The buffer to write the full path of the active database.
 * \param   space   The length of the buffer to write the database path, inclusive the null-terminated character at the end.
 * \return  Returns number of characters copied in the `dbPath`. It `dbPath` is null or `space` is smaller than the length of database path,
 *          the returned value is the space required to allocate to get the path.
 **/
LOGGER_API int log_observer_active_database_path(char* dbPath, int space);

/**
 * \brief   Call to get initial database full path.
 * \param   dbPath  The buffer to write the full path of the initial database.
 * \param   space   The length of the buffer to write the database path, inclusive the null-terminated character at the end.
 * \return  Returns number of characters copied in the `dbPath`. It `dbPath` is null or `space` is smaller than the length of database path,
 *          the returned value is the space required to allocate to get the path.
 **/
LOGGER_API int log_observer_initial_database_path(char* dbPath, int space);

/**
 * \brief   Call to get the database path specified in the configuration file.
 * \param   dbPath  The buffer to write the full path of the database specified in the configuration file.
 * \param   space   The length of the buffer to write the database path, inclusive the null-terminated character at the end.
 * \return  Returns number of characters copied in the `dbPath`. It `dbPath` is null or `space` is smaller than the length of database path,
 *          the returned value is the space required to allocate to get the path.
 **/
LOGGER_API int log_observer_config_database_path(char* dbPath, int space);

/**
 * \brief   Call to get the database location specified in the configuration file.
 * \param   dbLocation  The buffer to write the log database location specified in the configuration file. The path may contain mask like `log_%time`
 * \param   space       The length of the buffer to write the database path, inclusive the null-terminated character at the end.
 * \return  Returns number of characters copied in the `dbLocation`. It `dbLocation` is null or `space` is smaller than the length of database path,
 *          the returned value is the space required to allocate to get the path.
 **/
LOGGER_API int log_observer_config_database_location(char* dbLocation, int space);

/**
 * \brief   Call to set the database location in the configuration file.
 * \param   dbLocation  The location to save logging database files. The path may contain mask like `log_%time`.
 *                      If empty or null, it is not modified and ignored.
 * \return  Returns true if operation succeeded. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_set_config_database_location(const char* dbLocation);

/**
 * \brief   Call to get the database name specified in the configuration file.
 * \param   dbName  The buffer to write the database name specified in the configuration file. The name may contain mask like `log_%time%.sqlog`.
 * \param   space   The length of the buffer to write the database name, inclusive the null-terminated character at the end.
 * \return  Returns number of characters copied in the `dbName`. It `dbName` is null or `space` is smaller than the length of database name,
 *          the returned value is the space required to allocate to get the name.
 **/
LOGGER_API int log_observer_config_database_name(char* dbName, int space);

/**
 * \brief   Call to set the database name in the configuration file.
 * \param   dbName  The name to save logging database files. The name may contain mask like `log_%time%.sqlog`.
 *                  If empty or null, it is not modified and ignored.
 * \return  Returns true if operation succeeded. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_set_config_database_name(const char* dbName);

/**
 * \brief   Call to change the IP-address and TCP port of the log collector service. If required changes are written in configuration file.
 * \param   address     The IP-address of the log collector service to set in configuration. If empty or null, it is not modified and ignored.
 * \param   portNr      The TCP port number of the log collector service to set in configuration. If Invalid port (0), it is not modified and ignored.
 * \param   dbFilePath  The path to the logging database file to set in configuration. If empty or null, it is not modified and ignored.
 * \param   makeSave    If set true, the modified configuration is saved in the configuration file. 
 * \return  Returns true if operation succeeded. Otherwise, returns false.
 **/
LOGGER_API bool log_observer_config_update(const char* address, uint16_t portNr, const char * dbFilePath, bool makeSave);

#endif  // AREG_AREGLOGGER_CLIENT_LOGOBSERVERAPI_H
