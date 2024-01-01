#ifndef AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
#define AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/LogObserverApi.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "observer/lib/LogObserverSwitches.h"

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
struct sLogInstance
{
    /* The type of the message source application. Either client, test or simulation */
    unsigned int        liSource;
    /* The bitness of the application, should be either 32 or 64 */
    unsigned int        liBitness;
    /* The cookie ID of the connected client. */
    unsigned long long  liCookie;
    /* The name of the connected application */
    char                liName[LENGTH_NAME];
    /* The name of the file location. */
    char                liLocation[LENGTH_LOCATION];
};

/**
 * \brief   The type of the log message
 **/
enum eLogType
{
    /* The enter scope log message */
      LogScopeEnter     = 1
    /* The log message text */
    , LogMessageText    = 2
    /* The exit scope log message */
    , LogScopeExit      = 4
};

/**
 * \brief   The priority of logs. Can be in combination with PrioScope
 **/
enum eLogPriority
{
    /* No priority, should not log any message */
      PrioNotSet    = 0x0001
    /* Logs scope enter and scope exit messages */
    , PrioScope     = 0x0010
    /* Logs fatal error messages. Can be in combination with PrioScope */
    , PrioFatal     = 0x0020
    /* Logs fatal error and error messages. Can be in combination with PrioScope */
    , PrioError     = 0x0040
    /* Logs fatal error, error and warning messages. Can be in combination with PrioScope */
    , PrioWarning   = 0x0080
    /* Logs fatal error, error, warning and info messages. Can be in combination with PrioScope */
    , PrioInfo      = 0x0100
    /* Logs fatal error, error, warning, info and debug messages. Can be in combination with PrioScope */
    , PrioDebug     = 0x0200
    /* Logs a message without priority. Can be in combination with PrioScope */
    , PrioAny       = 0x0FF0
};

/**
 * \brief   The structure of scopes
 **/
struct sLogScope
{
    /* The ID of the scope. Can be 0 if unknown or if indicates to scope group. */
    unsigned int    lsId;
    /* The priority of the scope to log messages. These are values of eLogPriority. Can be set bitwise. */
    unsigned int    lsPrio;
    /* The name of the scope or scope group. The scope groups are ending with '*'. For example 'areg_base_*' */
    char            lsName[LENGTH_SCOPE];
};

/**
 * \brief   The structure of the logging message.
 **/
struct sLogMessage
{
    /* The type of the message: scope enter, scope exit or message text. */
    eLogType            msgType;
    /* The priority of the message to log. */
    eLogPriority        msgPriority;
    /* The ID of the message source. This value is indicated in the sLogInstance::liCookie */
    unsigned long long  msgSource;
    /* The cookie ID of the message. This value is indicated in the sLogInstance::liCookie */
    unsigned long long  msgCookie;
    /* The ID of the process, running on the local machine. */
    unsigned long long  msgModuleId;
    /* The ID of the thread where the log has been generated. */
    unsigned long long  msgThreadId;
    /* The timestamp where the log was generated. */
    unsigned long long  msgTimestamp;
    /* The ID of the scope that generated message. Same as indicated in sLogScope::lsId. */
    unsigned int        msgScopeId;
    /* The text of generated log message. */
    char                msgLogText[LENGTH_MESSAGE];
    /* The name of the thread, if set, where the message was generated. */
    char                msgThread[LENGTH_NAME];
    /* The name of the application that generated the message. Same as in sLogInstance::liName */
    char                msgModule[LENGTH_NAME];
};

/**
 * \brief   The callback of the event triggered when the observer connects or disconnects from the logger service.
 * \param   isConnected     Flag, indicating whether observer is connected or disconnected.
 * \param   address         The IP address of the logger service to connect or disconnect.
 * \param   port            The IP port number of the logger service to connect or disconnect.
 **/
typedef void (*FuncServiceConnected)(bool /*isConnected*/, const char * /*address*/, unsigned short /*port*/);

/**
 * \brief   The callback of the event triggered when fails to send or receive message.
 **/
typedef void (*FuncMessagingFailed)();

/**
 * \brief   The callback of the event triggered when receive the list of connected instances that log messages.
 * \param   instances   The pointer to the list of instances.
 * \param   count       The number of entries in the list.
 **/
typedef void (*FuncLogInstances)(sLogInstance* /*instances*/, unsigned int /*count*/);

/**
 * \brief   The callback of the event triggered when receive the list of the scopes registered in an application.
 * \param   cookie  The cookie ID of the connected instance / application. Same as sLogInstance::liCookie
 * \param   scopes  The list of the scopes registered in the application. Each entry contains the ID of the scope, message priority and the full name.
 * \param   count   The number of scope entries in the list.
 **/
typedef void (*FuncLogScopes)(unsigned long long /*cookie*/, sLogScope* /*scopes*/, unsigned int /*count*/);

/**
 * \brief   The callback of the event triggered when receive message to log.
 * \param   logMessage  The structure of the message to log.
 **/
typedef void (*FuncLogMessage)(const sLogMessage * /*logMessage*/);

/**
 * \brief   The structure of the callbacks / events to set when send or receive messages.
 **/
struct sObserverEvents
{
    /* The callback to trigger when connect or disconnect logger service. */
    FuncServiceConnected    evtServiceConnected;
    /* The callback to trigger when fails to send or receive message. */
    FuncMessagingFailed     evtMessagingFailed;
    /* The callback to trigger when receive list of connected instances that log messages. */
    FuncLogInstances        evtLogSources;
    /* The callback to trigger when receive list of registered scopes. */
    FuncLogScopes           evtLogScopes;
    /* The callback to trigger when receive a message to log. */
    FuncLogMessage          evtLogMessage;
};

/**
 * \brief   Call to initialize the log observer internals. This function should
 *          be called before any other. The multiple call of this function will not
 *          have any effect.
 * \param   configFilePath  Relative or absolute path to the application configuration file.
 *                          If NULL, it uses the default location of the config file './config/areg.init'
 * \returns Returns true, if succeeded to initialize internals. Otherwise, returns false.
 **/
OBSERVER_API bool logObserverInitialize(const sObserverEvents * callbacks, const char * configFilePath);

/**
 * \brief   Call to release the log observer internals and release resources. This function should
 *          be called at the end of application run. Once the observer is uninitialized, it will be
 *          not possible to use any other API and trigger remote methods.
 **/
OBSERVER_API void logObserverRelease();

/**
 * \brief   Call to trigger TCP/IP connection with the logger service. Either specify the IP address and the port number
 *          of the logger service to connect, or pass NULL to use settings indicated in the configuration file.
 * \param   ipAddress   The IP address or the host name of the logger service. If NULL, uses IP address indicated in the config file.
 *                      If NULL, the port number is ignored and the value in the configuration file is used.
 * \param   portNr      The port number to connect. If 0, uses port number indicated in the config file.
 *                      If 0, the IP address is ignore and the value in the configuration file is used.
 * \return  Returns true if succeeded to trigger connection. The physical connection might not be established yet.
 *          The physical connection is established when triggers callback of type FuncServiceConnected.
 * \note    The 'ipAddress' and 'portNr' parameters should be either both valid or they are ignored and the default values
 *          indicated in the configuration file are used.
 **/
OBSERVER_API bool logObserverConnectLogger(const char* ipAddress, unsigned short portNr);

/**
 * \brief   Call to trigger disconnect from logger service.
 *          After disconnecting the callback of FuncServiceConnected type is triggered.
 **/
OBSERVER_API void logObserverDisconnectLogger();

/**
 * \brief   Returns true if observer is initialized and the logger service connection is triggered.
 *          Otherwise, it returns false.
 *          The method does not indicate whether the observer is connected to the logger service or not.
 **/
OBSERVER_API bool logObserverIsInitialized();

/**
 * \brief   Returns true if observer is initialize and connected to the logger service.
 *          Otherwise, it returns false.
 **/
OBSERVER_API bool logObserverIsConnected();

/**
 * \brief   Returns the current IP address of the logger service to connect.
 *          Returns empty string, if not configured or the IP address is unknown
 **/
OBSERVER_API const char * logObserverLoggerAddress();

/**
 * \brief   Returns the current IP port of the logger service to connect.
 *          Return 0 (invalid port), if not configured or the port number is unknown
 **/
OBSERVER_API unsigned short logObserverLoggerPort();

/**
 * \brief   Return true if logger TCP/IP connection is enabled in the configuration file.
 *          Otherwise, returns false.
 **/
OBSERVER_API bool logObserverConfigLoggerEnabled();

/**
 * \brief   On exit, the addrBuffer contains the IP address of the logger registered in the configuration file.
 * \param   addrBuffer  Should be valid pointer of the buffer to write the IP address, the minimum size should be 16 bytes.
 * \param   count       The length of the buffer to write the IP address, inclusive the null-terminated character at the end.
 * \return  Returns true, if succeeds to write the address. Returns false, if the observer is not configured,
 *          or the address is not specified in the configuration, or the buffer is not big enough to write address.
 **/
OBSERVER_API bool logObserverConfigLoggerAddress(char * addrBuffer, int count);

/**
 * \brief   Returns the configured IP port of the logger service to connect,
 *          i.e. the IP port saved in configuration file. Returns 0 (invalid port),
 *          if the observer is not configured or the port number is not specified in configuration.
 **/
OBSERVER_API unsigned short logObserverConfigLoggerPort();

/**
 * \brief   Call to request the list of connected instances.
 *          The callback of FuncLogInstances type is triggered when receive the list of connected instances.
 **/
OBSERVER_API void logObserverRequestInstances();

/**
 * \brief   Call to receive the list of registered scopes of the specified connected instance.
 * \param   target  The cookie ID of the target instance to receive the list of registered scopes.
 *                  If the target is ID_IGNORE (or 0), it receives the list of scopes of all connected instances.
 *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
 **/
OBSERVER_API void logObserverRequestScopes(unsigned long long target);

/**
 * \brief   Call to update the priority of the logging message to receive.
 *          The indicated scopes can be scope group.
 * \param   target  The valid cookie ID of the target to update the log message priority.
 *                  This value cannot be ID_IGNORE (or 0).
 * \param   scopes  The list of scopes of scope group to update the priority.
 *                  The scope group should  end with '*'. For example 'areg_base_*'.
 *                  In this case the ID of the scope can be 0.
 * \param   count   The number of scope entries in the list.
 **/
OBSERVER_API void logObserverRequestChangeScopePrio(unsigned long long target, const sLogScope* scopes, unsigned int count);

/**
 * \brief   Call to save current configuration of the specified target. This is normally called when update the log priority of the instance,
 *          so that on next start the application logs message of the scopes and priorities currently set.
 * \param   target  The cookie ID of the target instance to save the configuration.
 *                  If the target is ID_IGNORE (or 0), the request is sent to all connected instances.
 *                  Otherwise, should be indicated the valid cookie ID of the connected log instance.
 **/
OBSERVER_API void logObserverRequestSaveConfig(unsigned long long target);

#endif  // AREG_OBSERVER_LIB_LOGOBSERVERAPI_H

