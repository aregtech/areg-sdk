#ifndef AREG_LOGGING_LOGGINGDEFS_HPP
#define AREG_LOGGING_LOGGINGDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/LoggingDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, NELogging namespace contains constants, structures and types.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IOStream.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/String.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class LogDatabaseEngine;
class LogScope;

//////////////////////////////////////////////////////////////////////////
// NELogging namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace is a collection of helper objects like
 *          constants, structures, methods and types used
 *          to provide logging functionality.
 **/
AREG_NAMESPACE_BEGIN

    /**
     * \brief   The supported logging version
     **/
    constexpr std::string_view LOG_VERSION{ "2.0.0" };

    /**
     * \brief   The list of the scopes. It is a pair, where the key is the ID of the scope
     *          and the value is the pointer to the scope.
     **/
    using ScopeList     = HashMap<unsigned int, LogScope*>;

    /**
     * \brief   Alias of the map position.
     **/
    using SCOPEPOS      = ScopeList::MAPPOS;

    /**
     * \brief   areg::sScopeInfo
     *          The structure to keep scope information. It is used to generate scope priority update messages.
     *          The structure contains scope name, scope ID and scope priority values.
     **/
    struct sScopeInfo
    {
        /**
         * \brief   Default constructor
         **/
        inline sScopeInfo();

        /**
         * \brief   Constructor that initializes the scope name, scope ID and the scope priority.
         * \param   name    The name of the scope.
         * \param   id      The scope ID. Can be 0 if the ID is unknown or the name is a scope group.
         * \param   prio    The scope message priority.
         **/
        inline sScopeInfo(const char* name, uint32_t id, uint32_t prio);

        uint32_t    scopeId;    //!< The scope ID, can be 0 (areg::LOG_SCOPE_ID_NONE). For scope group should be 0.
        uint32_t    scopePrio;  //!< The scope priority.
        String      scopeName;  //!< The name of the scope or scope group.
    };

    //!< The list of scope update structure.
    using ScopeNames    = ArrayList<sScopeInfo>;

    /**
     * \brief   areg::LoggingType
     *          The logging types in Areg framework
     **/
    enum LoggingType : unsigned int
    {
          LogTypeUndefined  = 0 //!< Logging is undefined
        , LogTypeRemote     = 1 //!< Logging is to remote log collector
        , LogTypeFile       = 2 //!< Logging is directly in file as plain text
        , LogTypeDebug      = 4 //!< Logging is on debug output console
        , LogTypeDatabase   = 8 //!< Logging is in database, not implemented yet.
    };

    /**
     * \brief   areg::eLogPriority
     *          Log priority definition set when logging message
     **/
    enum eLogPriority : unsigned short
    {
          PrioInvalid       = 0x0000  //!< Invalid priority,          bit set:  0000 0000 0000
        , PrioNotset        = 0x0001  //!< No priority is set,        bit set:  0000 0000 0001
        , PrioScope         = 0x0010  //!< Output scopes priority,    bit set:  0000 0001 0000
        , PrioFatal         = 0x0020  //!< Fatal error log priority,  bit set:  0000 0010 0000
        , PrioError         = 0x0040  //!< Error log priority,        bit set:  0000 0100 0000
        , PrioWarning       = 0x0080  //!< Warning log priority,      bit set:  0000 1000 0000
        , PrioInfo          = 0x0100  //!< Information log priority,  bit set:  0001 0000 0000
        , PrioDebug         = 0x0200  //!< Debug log priority,        bit set:  0010 0000 0000
        , PrioLogs          = 0x03E0  //!< Log is enabled priority,   bit set:  0011 1110 0000
        , PrioScopeLogs     = 0x03F0  //!< Scopes and logs with prio, bit set:  0011 1111 0000
        , PrioValidLogs     = 0x03F1  //!< The valid log priority,    bit set:  0011 1111 0001
        , PrioIgnore        = 0x0400  //!< Ignore logging priority,   bit set:  0100 0000 0000
        , PrioMarker        = 0x0401  //!< Ignore Prio, Marker,       bit set:  0100 0000 0001
        , PrioMarkerError   = 0x0441  //!< Ignore Prio, Marker,       bit set:  0100 0100 0001
        , PrioMarkerWarning = 0x0481  //!< Ignore Prio, Marker,       bit set:  0100 1000 0001
        , PrioMarkerInfo    = 0x0501  //!< Ignore Prio, Marker,       bit set:  0101 0000 0001
        , PrioIgnoreLayout  = 0x0C00  //!< Ignore layout priority,    bit set:  1100 0000 0000
        , PrioAny           = 0x0FF0  //!< Log without priority,      bit set:  1111 1111 0000
        , PrioValid         = 0x0FF1  //!< Valid logging priority     bit set:  1111 1111 0001
        , PrioUndefined     = 0xFFFF  //!< Undefined logging priority bit set:  1111 1111 1111
    };

    /**
     * \brief   areg::getString
     *          Converts areg::eLogPriority values to readable string values
     **/
    inline const char * getString( areg::eLogPriority prio );

    /**
     * \brief   Returns true if the specified log priority value is valid.
     **/
    inline bool isValidLogPriority( areg::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority refers to the logging.
     **/
    inline bool isLogPriority( areg::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables logging, i.e. logs messages..
     **/
    inline bool isEnablingLog( areg::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority makes scope logging.
     **/
    inline bool isLogScope( areg::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority does not log messages.
     **/
    inline bool isDisablingLog( areg::eLogPriority prio );

    /**
     * \brief   Returns the cookie of the log collector (logger).
     **/
    AREG_API const ITEM_ID & getCookie();

    /**
     * \brief   areg::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr unsigned int  HAS_MESSAGE_PRIORITY    { 0x1F }; // 63, bit set 0001 1111

    /**
     * \brief   areg::LOG_SCOPE_ID_NONE
     *          Constant, defines no scope ID. It is used to output message without scope
     **/
    constexpr unsigned int  LOG_SCOPE_ID_NONE     { 0 };

    /**
     * \brief   The string value of no priority
     **/
    const String  PRIO_NOTSET_STR     { "NOTSET" };
    /**
     * \brief   The string value of scope priority
     **/
    const String  PRIO_SCOPE_STR      { "SCOPE" };
    /**
     * \brief   The string value of fatal error priority
     **/
    const String  PRIO_FATAL_STR      { "FATAL" };
    /**
     * \brief   The string value of error priority
     **/
    const String  PRIO_ERROR_STR      { "ERROR" };
    /**
     * \brief   The string value of warning priority
     **/
    const String  PRIO_WARNING_STR    { "WARN" };
    /**
     * \brief   The string value of information priority
     **/
    const String  PRIO_INFO_STR       { "INFO" };
    /**
     * \brief   The string value of debug priority
     **/
    const String  PRIO_DEBUG_STR      { "DEBUG" };
    /**
     * \brief   No priority string
     **/
    const String  PRIO_NO_PRIO        { "" };

    /**
     * \brief   The name of the supported database logging engines.
     **/
    constexpr std::string_view   LOGDB_ENGINE_NAME  { "sqlite3" };

    /**
     * \brief   Returns string value of areg::eLogPriority.
     *          There are following valid string priority values:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority to get string value.
     * \return  Returns string priority value
     **/
    inline const String& logPrioToString(areg::eLogPriority prio);

    /**
     * \brief   From given string value returns log priority value.
     *          The string values should be followings:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority string value to convert.
     *                  The given string is not case sensitive.
     * \return  Returns appropriate logging priority value.
     **/
    inline areg::eLogPriority stringToLogPrio(const String& prio);

    /**
     * \brief   Converts the bitwise set of priority into the human readable string.
     * \param   priorities      The bitwise set of priorities integer value to convert to string.
     * \return  Returns converted string that may contain logical OR ('|') if more than one priority is set.
     **/
    AREG_API String makePrioString(unsigned int priorities);

    /**
     * \brief   Converts the human readable string with priorities separate by logical OR ('|')
     *          into bitwise set of integer value with priorities.
     * \param   prio    The human readable string with priorities separated by logical OR ('|')
     *                  to convert into integer.
     * \return  Returns converted integer value where the priorities are set bitwise.
     **/
    AREG_API unsigned int makePriorities(const String& prio);

    /**
     * \brief   areg::LOG_MESSAGE_IZE
     *          The maximum size of text in log message
     **/
    constexpr uint32_t  LOG_MESSAGE_IZE     { 332 };
    /**
     * \brief   areg::LOG_NAMES_SIZE
     *          The maximum length of the names in logging objects
     **/
    constexpr uint32_t   LOG_NAMES_SIZE     { 48 };

    /**
     * \brief   areg::eLogMessageType
     *          The logging message type.
     **/
    enum class eLogMessageType  : unsigned char
    {
          LogMessageUndefined   = 0 //!< The log origin is undefined.
        , LogMessageScopeEnter  = 1 //!< Indicates the begin of the logging scope.
        , LogMessageText        = 2 //!< Indicates the message to log.
        , LogMessageScopeExit   = 4 //!< Indicates the end of the logging scope.
    };

    /**
     * \brief   areg::eLogDataType
     *          The data type in the message log
     **/
    enum class eLogDataType : unsigned char
    {
          LogDataLocal          = 0 //!< The message data is generated locally.
        , LogDataRemote         = 1 //!< The message data is prepared for remote logging.
    };

    /**
     * \brief   areg::sLogMessage
     *          The structure of logging message object to output on target (log collector or observer).
     **/
    struct AREG_API sLogMessage
    {
        /**
         * \brief   Initializes logging message of specified type.
         * \param   msgType     The logging message type.
         **/
        sLogMessage( areg::eLogMessageType msgType = areg::eLogMessageType::LogMessageUndefined );
        /**
         * \brief   Initializes logging message and sets specified data.
         * \param   msgType     The logging message type.
         * \param   scopeId     The ID of message scope.
         * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
         * \param   scopeStamp  The timestamp of the scope message, which is used to log message. This parameter is used to set duration.
         *                      The duration is ignored and set to 0 if the scopeStamp is 0.
         * \param   msgPrio     The priority of logging message.
         * \param   message     The message text to output on target. Can be empty.
         * \param   msgLen      The length of the message string.
         **/
        sLogMessage(areg::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, areg::eLogPriority msgPrio, const char * message, unsigned int msgLen);
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        sLogMessage( const sLogMessage & src );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        sLogMessage & operator = (const sLogMessage & src);

        areg::eLogDataType     logDataType;    //!< The type of log message data.
        areg::eLogMessageType  logMsgType;     //!< The type of the logging message.
        areg::eLogPriority     logMessagePrio; //!< The log message priority
        ITEM_ID                     logSource;      //!< The ID of the source that generated logging message.
        ITEM_ID                     logTarget;      //!< The ID of the target to send logging message, valid only in case of TCP/IP logging.
        ITEM_ID                     logCookie;      //!< The cookie set by the networking service, i.e. the log collector. Valid only in case of TCP/IP logging.
        ITEM_ID                     logModuleId;    //!< The ID of the process in the local machine.
        ITEM_ID                     logThreadId;    //!< The ID the thread in the local process.
        TIME64                      logTimestamp;   //!< The timestamp of generated log.
        TIME64                      logReceived;    //!< The timestamp when the log message is updated.
        unsigned int                logDuration;    //!< The duration in microseconds after scope message is instantiated in the method call.
        unsigned int                logScopeId;     //!< The ID of log scope that generated log message
        unsigned int                logSessionId;   //!< The session ID of the logging message, valid only in case of remote logging.
        unsigned int                logMessageLen;  //!< The actual length of the log message
        char                        logMessage[LOG_MESSAGE_IZE];//!< The message text to output, with maximum areg::LOG_MESSAGE_IZE characters.
        unsigned int                logThreadLen;               //!< The length of the thread name;
        char                        logThread[LOG_NAMES_SIZE];  //!< The name of the thread that generated the log. Valid only for remote logging
        unsigned int                logModuleLen;               //!< The length of the module name.
        char                        logModule[LOG_NAMES_SIZE];  //!< The name of the module that generated the log. Valid only for remote logging.
    };

    /**
     * \brief   Start logging. If specified file is not nullptr, it configures logging first, then starts logging.
     * \param   fileConfig  The relative or absolute path to logging configuration file.
     *                      If not nullptr, the system configures logging then starts logging.
     *                      If nullptr and logging was not configured, the system configures logging using default configuration path.
     *                      If nullptr and logging was configured, the system immediately starts logging.
     * \return  Returns true if succeeded to configure and start logging.
     **/
    AREG_API bool startLogging( const char * fileConfig = nullptr );

    /**
     * \brief   Sets default configuration values and forces to start logging.
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool forceStartLogging();

    /**
     * \brief   Loads configuration values from specified configuration file and force to start logging, i.e. enables logging.
     *          If configuration file does not exit, it sets default configuration values for logging.
     *          If configuration file is nullptr or empty, it tries to load configuration values from default configuration file.
     * \param   fileConfig  If not nullptr and file exist, loads configuration data from file, enables and starts logging.
     *                      If nullptr or empty, and default configuration file exist, loads configuration data from default file,
     *                      enables and starts logging.
     *                      If neither specified, nor default configuration file exist, it sets logging default values and start logging.
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool initAndStartLogging( const char * fileConfig = nullptr );

    /**
     * \brief   This stops Logging and exits the thread.
     *          If 'waitComplete' is set to true, the calling thread is
     *          blocked until logging completes jobs and cleans resources.
     *          Otherwise, this triggers stop and exit events, and immediately returns.
     * \param   waitComplete    If true, waits for logging to complete the jobs
     *                          and exit threads. Otherwise, it triggers exit and returns.
     **/
    AREG_API void stopLogging( bool waitComplete);

    /**
     * \brief   The calling thread is blocked until logging did not
     *          complete the job and exit. This should be called if previously
     *          it was requested to stop the logging without waiting for completion.
     **/
    AREG_API void waitLoggingEnd();

    /**
     * \brief   Activates the specified log scope, enabling logging for messages within that scope.
     *          By default, the logging priority of a scope is set to areg::PrioNotset.
     *          The method checks the logging configuration for priority settings, and if a priority 
     *          is specified, the scope begins logging messages of that priority or higher.
     * \param   logScope    The log scope instance to activate and set a logging priority for.
     *                      If no priority is found in the configuration, the scope will remain
     *                      unable to log messages.
     * \note    Activating a scope alone does not guarantee message logging. 
     *          If the configuration file lacks priority information for the scope, messages will not be logged.
     *          However, if the configuration specifies a priority other than areg::PrioNotset,
     *          messages within that scope will be logged according to the specified priority.
     **/
    AREG_API void activateScope( LogScope & logScope );

    /**
     * \brief   Returns true if logging has been started.
     **/
    AREG_API bool isStarted();

    /**
     * \brief   Returns true if logging has been configured.
     **/
    AREG_API bool isConfigured();

    /**
     * \brief   Returns true if logging is enabled.
     **/
    AREG_API bool isEnabled();

    /**
     * \brief   Initializes the logging by reading and configuration instructions
     *          from given log configuration file.
     * \param   fileConfig  The relative or absolute path of log configuration file.
     *                      If empty or nullptr, the path to the default configuration file is taken.
     * \return  Returns true if succeeded to open file and configure logging.
     **/
    AREG_API bool initializeLogging( const char * fileConfig );

    /**
     * \brief   Saves the current configuration of the logging state in the configuration file.
     * \param   configFile  The pointer to the configuration file to save.
     *                      If null, uses currently active configuration file.
     * \return  Returns true if succeeded to save the current state of the logging.
     **/
    AREG_API bool saveLogging( const char * configFile = nullptr );

    /**
     * \brief   Returns the ID of given scope name.
     * \param   scopeName   The name of scope.
     * \return  Returns the ID of given scope name.
     **/
    AREG_API unsigned int makeScopeId( const char * scopeName );

    /**
     * \brief   Returns the ID of given scope name.
     *          If the scope name is null, empty or ends with grouping symbol '*', it returns 0.
     * \param   scopeName   The name of scope. If null, empty or ends with grouping symbol '*',
     *                      the return value is zero.
     * \return  Returns the ID of given scope name.
     **/
    AREG_API unsigned int makeScopeIdEx(const char* scopeName);

    /**
     * \brief   Call to change the log scope priority.
     * \param   scopeName   The name of the existing scope. Ignored if scope does not exit.
     * \param   newPrio     The new priority to set. Can be bitwise combination of priorities.
     * \return  Returns true if scope found and priority changed.
     **/
    AREG_API unsigned int setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   If scope with the give name found, returns the combined priority of scope; 
     *          otherwise, returns invalid priority (areg::eLogPriority::PrioInvalid).
     * \param   scopeName   The name of the existing scope.
     * \return  If found the scope, returns the actual priority of the scope.
     *          Otherwise, returns invalid priority.
     **/
    AREG_API unsigned int getScopePriority( const char * scopeName );

    /**
     * \brief   Creates a network message to make a log.
     * \param   logMessage  The logging message structure.
     * \param   dataType    The type of created data to set in the structure.
     * \param   srcCookie   The cookie of the source generated message.
     * \return  Returns message object for network communication.
     **/
    AREG_API RemoteMessage createLogMessage(const areg::sLogMessage& logMessage, areg::eLogDataType dataType, const ITEM_ID & srcCookie);

    /**
     * \brief   Triggers an event to log the message, contained in the buffer.
     * \param   message     The shared buffer with the information to log.
     **/
    AREG_API void logMessage(const RemoteMessage& message);

    /**
     * \brief   Log local custom message ignoring process and thread names.
     * \param   logMessage  The structure that contains information to log a message.
     **/
    AREG_API void logAnyMessageLocal(const areg::sLogMessage& logMessage);

    /**
     * \brief   Log custom message considering process and thread names.
     * \param   logMessage  The structure that contains information to log a message.
     **/
    AREG_API void logAnyMessage(const areg::sLogMessage& logMessage);

    /**
     * \brief   Creates a message for logging service to register scopes with message priority.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message
     * \param   scopeList   The list of scopes to register.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const areg::ScopeList & scopeList);

    /**
     * \brief   Creates a message to update the list of log scopes and priorities. This message can change the priority either
     *          of a certain scope or a scope group. The scope ID in the ScopeNames list can be set or missed.
     *          If a scope name in the ScopeNames is indicating on a scope group, the scope ID can be missed.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message.
     * \param   scopeNames  The list that consists of scope ID, log scope priority and scope name.
     *                      The scope name can indicate on the scope group.
     *                      The scope IDs can be set or missed (set 0).
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageUpdateScopes(const ITEM_ID & source, const ITEM_ID & target, const areg::ScopeNames & scopeNames);

    /**
     * \brief   Creates a message to update the logging priority of a single scope or a single group of scopes.
     *          The group of scopes ends with '*' sign. For example, the scope "areg_*" indicates all scopes that start with "areg_".
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message.
     * \param   scopeName   The scope name that indicates single scope or single group of scopes.
     * \param   scopeId     The ID of the scope. Can be missed (set 0). For scope group it should be 0.
     * \param   scopePrio   The logging priority of the scope.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageUpdateScope(const ITEM_ID& source, const ITEM_ID& target, const String & scopeName, unsigned int scopeId, unsigned int scopePrio);

    /**
     * \brief   Creates a message to query instances connected to the service.
     * \param   source      The source ID that created the query message. Should be the ID of the log observer or the ID of the log collector service.
     * \param   target      The target ID to send the message. The target is either concrete target or areg::TARGET_ALL if message targets all clients.
     * \return  Returns generated message ready to forward to target client(s) via log collector service.
     **/
    AREG_API RemoteMessage messageQueryInstances(const ITEM_ID& source, const ITEM_ID& target);

    /**
     * \brief   Creates a message to query the list of scopes of connected client applications.
     *          After option it should follow either '*' for all connected clients or the list of connected cookies.
     *          The cookie ID is specified, the message is sent to the specified target. Otherwise, the message is sent
     *          to all connected clients. An empty command after option is considered as sending message to all clients.
     *          Only log observers and the log collector service (logger) can generate and send the message.
     *          The message is ignored if the client creates and send the message.
     * \param   source      The ID of the source that generated the message.
     *                      The source should be either log observer or areg::COOKIE_LOGGER.
     * \param   target      The ID of the target to send the message.
     *                      If the ID is areg::TARGET_ALL, the message is sent to all connected clients.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target);

    /**
     * \brief   Creates a message to notify that scopes priority have been changed.
     *          The message is contains the list of all scopes and priorities, similar to methods messageRegisterScopes()
     * \param   source      The source ID that creates the message.
     * \param   target      The target ID that receives the message. Normally, it should be areg::COOKIE_LOGGER, and
     *                      then the log collector forwards the message to all lob observer instances.
     * \param   scopeList   The list of scopes, IDs and priorities to set in the message.
     * \return  Returns generated remote message that contains information of scopes, IDS and priorities to send to log collector.
     * \see     messageRegisterScopes
     **/
    AREG_API RemoteMessage messageScopesUpdated(const ITEM_ID& source, const ITEM_ID& target, const areg::ScopeList& scopeList);

    /**
     * \brief   Creates a message to send request to the connected client target to save configuration.
     * \param   source      The source ID that generated the message. It should be either ID of the log observer application
     *                      or the ID of the log collector service.
     * \param   target      The target ID to receive the message. This target can be either concrete connected client
     *                      or can be areg::TARGET_ALL if should be forwarded to all connected clients.
     * \return  Returns generated message ready to send to client(s) via log collector service.
     **/
    AREG_API RemoteMessage messageSaveConfiguration(const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   Create a message to notify the log collector that the configuration file has been saved.
     *          The message sent immediately after request to save configuration file, and the message
     *          is sent only to the log observer. The source of the message is taken by 
     *          calling areg::getCookie() method, and the target is hard-coded value areg::COOKIE_LOGGER.
     * \return  Returns generate remote message to notify the log collector that the configuration file has been saved.
     * \see     messageSaveConfiguration
     **/
    AREG_API RemoteMessage messageConfigurationSaved();

    /**
     * \brief   Call to set external logging database engine.
     **/
    AREG_API void setLogDatabaseEngine(LogDatabaseEngine* dbEngine);
AREG_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace streamable types
//////////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(areg::eLogPriority)
AREG_IMPLEMENT_STREAMABLE(areg::eLogMessageType)

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace objects
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief   De-serializes log message from the stream.
 * \param   stream  The source of log message data.
 * \param   input   On output this contains structured logging message.
 **/
inline const InStream & operator >> (const InStream& stream, areg::sLogMessage& input)
{
    stream.read(reinterpret_cast<unsigned char *>(&input), offsetof(areg::sLogMessage, logMessage));
    stream.read(reinterpret_cast<unsigned char *>(input.logMessage), input.logMessageLen + 1);
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline OutStream& operator << (OutStream& stream, const areg::sLogMessage& output)
{
    stream.write(reinterpret_cast<const unsigned char *>(&output), offsetof(areg::sLogMessage, logMessage));
    stream.write(reinterpret_cast<const unsigned char *>(output.logMessage), output.logMessageLen + 1);
    return stream;
}

/**
 * \brief   De-serializes a scope update structure from the stream.
 * \param   stream  The source of data that contains scope update structure information.
 * \param   input   On output this contains structured scope update data.
 **/
inline const InStream& operator >> (const InStream& stream, areg::sScopeInfo & input)
{
    stream >> input.scopeId >> input.scopePrio >> input.scopeName;
    return stream;
}

/**
 * \brief   Serializes a scope update structure to the stream.
 * \param   stream  The streaming object to save scope update information.
 * \param   output  The source of scope update structure to serialize message.
 **/
inline OutStream& operator << (OutStream& stream, const areg::sScopeInfo & output)
{
    stream << output.scopeId << output.scopePrio << output.scopeName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace inline methods
//////////////////////////////////////////////////////////////////////////////

inline const char* areg::getString(areg::eLogPriority prio)
{
    switch ( prio )
    {
    case areg::eLogPriority::PrioInvalid:
        return "areg::PrioInvalid";
    case areg::eLogPriority::PrioNotset:
        return "areg::PrioNotset";
    case areg::eLogPriority::PrioFatal:
        return "areg::PrioFatal";
    case areg::eLogPriority::PrioScope:
        return "areg::PrioScope";
    case areg::eLogPriority::PrioError:
        return "areg::PrioError";
    case areg::eLogPriority::PrioWarning:
        return "areg::PrioWarning";
    case areg::eLogPriority::PrioInfo:
        return "areg::PrioInfo";
    case areg::eLogPriority::PrioDebug:
        return "areg::PrioDebug";
    case areg::eLogPriority::PrioLogs:
        return "areg::PrioLogs";
    case areg::eLogPriority::PrioScopeLogs:
        return "areg::PrioScopeLogs";
    case areg::eLogPriority::PrioValidLogs:
        return "areg::PrioValidLogs";
    case areg::eLogPriority::PrioIgnore:
        return "areg::PrioIgnore";
    case areg::eLogPriority::PrioMarker:
        return "areg::PrioMarker";
    case areg::eLogPriority::PrioMarkerError:
        return "areg::PrioMarkerError";
    case areg::eLogPriority::PrioMarkerWarning:
        return "areg::PrioMarkerWarning";
    case areg::eLogPriority::PrioMarkerInfo:
        return "areg::PrioMarkerInfo";
    case areg::eLogPriority::PrioIgnoreLayout:
        return "areg::PrioIgnoreLayout";
    case areg::eLogPriority::PrioAny:
        return "areg::PrioAny";
    case areg::eLogPriority::PrioValid:
        return "areg::PrioValid";
    default:
        ASSERT(false);
        return "ERR: Unexpected areg::eLogPrior value";
    }
}

inline areg::sScopeInfo::sScopeInfo()
    : scopeId   ( 0u )
    , scopePrio ( static_cast<uint32_t>(areg::eLogPriority::PrioInvalid) )
    , scopeName (String::EmptyString)
{
}

inline areg::sScopeInfo::sScopeInfo(const char* name, uint32_t id, uint32_t prio)
    : scopeId   (id)
    , scopePrio (prio)
    , scopeName (name)
{
}

inline bool areg::isValidLogPriority( areg::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(areg::eLogPriority::PrioValid)) != 0;
}

inline bool areg::isLogPriority( areg::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(areg::eLogPriority::PrioValidLogs)) != 0;
}

inline bool areg::isEnablingLog( areg::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(areg::eLogPriority::PrioLogs)) != 0;
}

inline bool areg::isLogScope( areg::eLogPriority prio )
{
    return (prio == areg::eLogPriority::PrioScope);
}

inline bool areg::isDisablingLog( areg::eLogPriority prio )
{
    return (prio == areg::eLogPriority::PrioNotset) || (prio == areg::eLogPriority::PrioInvalid);
}

inline const String& areg::logPrioToString(areg::eLogPriority prio)
{
    switch (prio)
    {
    case areg::eLogPriority::PrioNotset:
        return areg::PRIO_NOTSET_STR;
    case areg::eLogPriority::PrioScope:
        return areg::PRIO_SCOPE_STR;
    case areg::eLogPriority::PrioFatal:
        return areg::PRIO_FATAL_STR;
    case areg::eLogPriority::PrioError:
        return areg::PRIO_ERROR_STR;
    case areg::eLogPriority::PrioWarning:
        return areg::PRIO_WARNING_STR;
    case areg::eLogPriority::PrioInfo:
        return areg::PRIO_INFO_STR;
    case areg::eLogPriority::PrioDebug:
        return areg::PRIO_DEBUG_STR;

    case areg::eLogPriority::PrioInvalid:        // fall through
    case areg::eLogPriority::PrioLogs:           // fall through
    case areg::eLogPriority::PrioValidLogs:      // fall through
    case areg::eLogPriority::PrioIgnore:         // fall through
    case areg::eLogPriority::PrioIgnoreLayout:   // fall through
    case areg::eLogPriority::PrioAny:            // fall through
    case areg::eLogPriority::PrioValid:          // fall through
    default:
        return areg::PRIO_NO_PRIO;
    }
}

inline areg::eLogPriority areg::stringToLogPrio(const String& prio)
{
    if (areg::PRIO_DEBUG_STR == prio)
        return areg::eLogPriority::PrioDebug;
    else if (areg::PRIO_INFO_STR == prio)
        return areg::eLogPriority::PrioInfo;
    else if (areg::PRIO_WARNING_STR == prio)
        return areg::eLogPriority::PrioWarning;
    else if (areg::PRIO_ERROR_STR == prio)
        return areg::eLogPriority::PrioError;
    else if (areg::PRIO_FATAL_STR == prio)
        return areg::eLogPriority::PrioFatal;
    else if (areg::PRIO_SCOPE_STR == prio)
        return areg::eLogPriority::PrioScope;
    else if (areg::PRIO_NOTSET_STR == prio)
        return areg::eLogPriority::PrioNotset;
    else
        return areg::eLogPriority::PrioIgnoreLayout;
}

#endif  // AREG_LOGGING_LOGGINGDEFS_HPP
