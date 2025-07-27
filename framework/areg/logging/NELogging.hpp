#ifndef AREG_LOGGING_NELOGGING_HPP
#define AREG_LOGGING_NELOGGING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/NELogging.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, NELogging namespace contains constants, structures and types.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/component/NEService.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IELogDatabaseEngine;
class LogScope;

//////////////////////////////////////////////////////////////////////////
// NELogging namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace is a collection of helper objects like
 *          constants, structures, methods and types used
 *          to provide logging functionality.
 **/
namespace NELogging
{
    /**
     * \brief   The supported logging version
     **/
    constexpr std::string_view LOG_VERSION{ "2.0.0" };

    /**
     * \brief   The list of the scopes. It is a pair, where the key is the ID of the scope
     *          and the value is the pointer to the scope.
     **/
    using ScopeList     = TEHashMap<unsigned int, LogScope*>;

    /**
     * \brief   Alias of the map position.
     **/
    using SCOPEPOS      = ScopeList::MAPPOS;

    /**
     * \brief   NELogging::sScopeInfo
     *          The structure to keep scope information. It is used to generate scope priority update messages.
     *          The structure contains scope name, scope ID and scope priority values.
     **/
    struct sScopeInfo
    {
        /**
         * \brief   Default constructor
         **/
        inline sScopeInfo(void);

        /**
         * \brief   Constructor that initializes the scope name, scope ID and the scope priority.
         * \param   name    The name of the scope.
         * \param   id      The scope ID. Can be 0 if the ID is unknown or the name is a scope group.
         * \param   prio    The scope message priority.
         **/
        inline sScopeInfo(const char* name, uint32_t id, uint32_t prio);

        uint32_t    scopeId;    //!< The scope ID, can be 0 (NELogging::LOG_SCOPE_ID_NONE). For scope group should be 0.
        uint32_t    scopePrio;  //!< The scope priority.
        String      scopeName;  //!< The name of the scope or scope group.
    };

    //!< The list of scope update structure.
    using ScopeNames    = TEArrayList<sScopeInfo>;

    /**
     * \brief   NELogging::eLogingTypes
     *          The logging types in AREG framework
     **/
    enum eLogingTypes : unsigned int
    {
          LogTypeUndefined  = 0 //!< Logging is undefined
        , LogTypeRemote     = 1 //!< Logging is to remote log collector
        , LogTypeFile       = 2 //!< Logging is directly in file as plain text
        , LogTypeDebug      = 4 //!< Logging is on debug output console
        , LogTypeDatabase   = 8 //!< Logging is in database, not implemented yet.
    };

    /**
     * \brief   NELogging::eLogPriority
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
        , PrioValidLogs     = 0x03F1  //!< The valid log priority,    bit set:  0011 1111 0001
        , PrioIgnore        = 0x0400  //!< Ignore logging priority,   bit set:  0100 0000 0000
        , PrioIgnoreLayout  = 0x0C00  //!< Ignore layout priority,    bit set:  1100 0000 0000
        , PrioAny           = 0x0FF0  //!< Log without priority,      bit set:  1111 1111 0000
        , PrioValid         = 0x0FF1  //!< Valid logging priority     bit set:  1111 1111 0001
        , PrioUndefined     = 0xFFFF  //!< Undefined lottint priority bit set:  1111 1111 1111
    };

    /**
     * \brief   NELogging::getString
     *          Converts NELogging::eLogPriority values to readable string values
     **/
    inline const char * getString( NELogging::eLogPriority prio );

    /**
     * \brief   Returns true if the specified log priority value is valid.
     **/
    inline bool isValidLogPriority( NELogging::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority refers to the logging.
     **/
    inline bool isLogPriority( NELogging::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables logging, i.e. logs messages..
     **/
    inline bool isEnablingLog( NELogging::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority makes scope logging.
     **/
    inline bool isLogScope( NELogging::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority does not log messages.
     **/
    inline bool isDisablingLog( NELogging::eLogPriority prio );

    /**
     * \brief   Returns the cookie of the log collector (logger).
     **/
    AREG_API const ITEM_ID & getCookie(void);

    /**
     * \brief   NELogging::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr unsigned int  HAS_MESSAGE_PRIORITY    { 0x1F }; // 63, bit set 0001 1111

    /**
     * \brief   NELogging::LOG_SCOPE_ID_NONE
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
     * \brief   Returns string value of NELogging::eLogPriority.
     *          There are following valid string priority values:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority to get string value.
     * \return  Returns string priority value
     **/
    inline const String& logPrioToString(NELogging::eLogPriority prio);

    /**
     * \brief   From given string value returns log priority value.
     *          The string values should be followings:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority string value to convert.
     *                  The given string is not case sensitive.
     * \return  Returns appropriate logging priority value.
     **/
    inline NELogging::eLogPriority stringToLogPrio(const String& prio);

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
     * \brief   NELogging::LOG_MESSAGE_IZE
     *          The maximum size of text in log message
     **/
    constexpr uint32_t  LOG_MESSAGE_IZE     { 512 };
    /**
     * \brief   NELogging::LOG_NAMES_SIZE
     *          The maximum length of the names in logging objects
     **/
    constexpr uint32_t   LOG_NAMES_SIZE     { 128 };

    /**
     * \brief   NELogging::eLogMessageType
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
     * \brief   NELogging::eLogDataType
     *          The data type in the message log
     **/
    enum class eLogDataType : unsigned char
    {
          LogDataLocal          = 0 //!< The message data is generated locally.
        , LogDataRemote         = 1 //!< The message data is prepared for remote logging.
    };

    /**
     * \brief   NELogging::sLogMessage
     *          The structure of logging message object to output on target (log collector or observer).
     **/
    struct AREG_API sLogMessage
    {
        /**
         * \brief   Initializes logging message of specified type.
         * \param   msgType     The logging message type.
         **/
        sLogMessage( NELogging::eLogMessageType msgType = NELogging::eLogMessageType::LogMessageUndefined );
        /**
         * \brief   Initializes logging message and sets specified data.
         * \param   msgType     The logging message type.
         * \param   scopeId     The ID of message scope.
         * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
         * \param   msgPrio     The priority of logging message.
         * \param   message     The message text to output on target. Can be empty.
         * \param   msgLen      The length of the message string.
         **/
        sLogMessage(NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, NELogging::eLogPriority msgPrio, const char * message, unsigned int msgLen);
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

        NELogging::eLogDataType     logDataType;    //!< The type of log message data.
        NELogging::eLogMessageType  logMsgType;     //!< The type of the logging message.
        NELogging::eLogPriority     logMessagePrio; //!< The log message priority
        ITEM_ID                     logSource;      //!< The ID of the source that generated logging message.
        ITEM_ID                     logTarget;      //!< The ID of the target to send logging message, valid only in case of TCP/IP logging.
        ITEM_ID                     logCookie;      //!< The cookie set by the networking service, i.e. the log collector. Valid only in case of TCP/IP logging.
        ITEM_ID                     logModuleId;    //!< The ID of the process in the local machine.
        ITEM_ID                     logThreadId;    //!< The ID the thread in the local process.
        TIME64                      logTimestamp;   //!< The timestamp of generated log.
        unsigned int                logScopeId;     //!< The ID of log scope that generated log message
        unsigned int                logSessionId;   //!< The session ID of the logging message, valid only in case of remote logging.
        unsigned int                logMessageLen;  //!< The actual length of the log message
        char                        logMessage[LOG_MESSAGE_IZE];//!< The message text to output, with maximum NELogging::LOG_MESSAGE_IZE characters.
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
    AREG_API bool forceStartLogging( void );

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
    AREG_API void waitLoggingEnd(void);

    /**
     * \brief   Activates the specified log scope, enabling logging for messages within that scope.
     *          By default, the logging priority of a scope is set to NELogging::PrioNotset.
     *          The method checks the logging configuration for priority settings, and if a priority 
     *          is specified, the scope begins logging messages of that priority or higher.
     * \param   logScope    The log scope instance to activate and set a logging priority for.
     *                      If no priority is found in the configuration, the scope will remain
     *                      unable to log messages.
     * \note    Activating a scope alone does not guarantee message logging. 
     *          If the configuration file lacks priority information for the scope, messages will not be logged.
     *          However, if the configuration specifies a priority other than NELogging::PrioNotset,
     *          messages within that scope will be logged according to the specified priority.
     **/
    AREG_API void activateScope( LogScope & logScope );

    /**
     * \brief   Returns true if logging has been started.
     **/
    AREG_API bool isStarted( void );

    /**
     * \brief   Returns true if logging has been configured.
     **/
    AREG_API bool isConfigured( void );

    /**
     * \brief   Returns true if logging is enabled.
     **/
    AREG_API bool isEnabled( void );

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
     *          otherwise, returns invalid priority (NELogging::eLogPriority::PrioInvalid).
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
    AREG_API RemoteMessage createLogMessage(const NELogging::sLogMessage& logMessage, NELogging::eLogDataType dataType, const ITEM_ID & srcCookie);

    /**
     * \brief   Triggers an event to log the message, contained in the buffer.
     * \param   message     The shared buffer with the information to log.
     **/
    AREG_API void logMessage(const RemoteMessage& message);

    /**
     * \brief   Log local custom message ignoring process and thread names.
     * \param   logMessage  The structure that contains information to log a message.
     **/
    AREG_API void logAnyMessageLocal(const NELogging::sLogMessage& logMessage);

    /**
     * \brief   Log custom message considering process and thread names.
     * \param   logMessage  The structure that contains information to log a message.
     **/
    AREG_API void logAnyMessage(const NELogging::sLogMessage& logMessage);

    /**
     * \brief   Creates a message for logging service to register scopes with message priority.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message
     * \param   scopeList   The list of scopes to register.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeList & scopeList);

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
    AREG_API RemoteMessage messageUpdateScopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeNames & scopeNames);

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
     * \param   target      The target ID to send the message. The target is either concrete target or NEService::TARGET_ALL if message targets all clients.
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
     *                      The source should be either log observer or NEService::COOKIE_LOGGER.
     * \param   target      The ID of the target to send the message.
     *                      If the ID is NEService::TARGET_ALL, the message is sent to all connected clients.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target);

    /**
     * \brief   Creates a message to notify that scopes priority have been changed.
     *          The message is contains the list of all scopes and priorities, similar to methods messageRegisterScopes()
     * \param   source      The source ID that creates the message.
     * \param   target      The target ID that receives the message. Normally, it should be NEService::COOKIE_LOGGER, and
     *                      then the log collector forwards the message to all lob observer instances.
     * \param   scopeList   The list of scopes, IDs and priorities to set in the message.
     * \return  Returns generated remote message that contains information of scopes, IDS and priorities to send to log collector.
     * \see     messageRegisterScopes
     **/
    AREG_API RemoteMessage messageScopesUpdated(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeList& scopeList);

    /**
     * \brief   Creates a message to send request to the connected client target to save configuration.
     * \param   source      The source ID that generated the message. It should be either ID of the log observer application
     *                      or the ID of the log collector service.
     * \param   target      The target ID to receive the message. This target can be either concrete connected client
     *                      or can be NEService::TARGET_ALL if should be forwarded to all connected clients.
     * \return  Returns generated message ready to send to client(s) via log collector service.
     **/
    AREG_API RemoteMessage messageSaveConfiguration(const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   Create a message to notify the log collector that the configuration file has been saved.
     *          The message sent immediately after request to save configuration file, and the message
     *          is sent only to the log observer. The source of the message is taken by 
     *          calling NELogging::getCookie() method, and the target is hard-coded value NEService::COOKIE_LOGGER.
     * \return  Returns generate remote message to notify the log collector that the configuration file has been saved.
     * \see     messageSaveConfiguration
     **/
    AREG_API RemoteMessage messageConfigurationSaved(void);

    /**
     * \brief   Call to set external logging database engine.
     **/
    AREG_API void setLogDatabaseEngine(IELogDatabaseEngine* dbEngine);
}

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace streamable types
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NELogging::eLogPriority)
IMPLEMENT_STREAMABLE(NELogging::eLogMessageType)

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
inline const IEInStream & operator >> (const IEInStream& stream, NELogging::sLogMessage& input)
{
    stream.read(reinterpret_cast<unsigned char *>(&input), offsetof(NELogging::sLogMessage, logMessage));
    stream.read(reinterpret_cast<unsigned char *>(input.logMessage), input.logMessageLen + 1);
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NELogging::sLogMessage& output)
{
    stream.write(reinterpret_cast<const unsigned char *>(&output), offsetof(NELogging::sLogMessage, logMessage));
    stream.write(reinterpret_cast<const unsigned char *>(output.logMessage), output.logMessageLen + 1);
    return stream;
}

/**
 * \brief   De-serializes a scope update structure from the stream.
 * \param   stream  The source of data that contains scope update structure information.
 * \param   input   On output this contains structured scope update data.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NELogging::sScopeInfo & input)
{
    stream >> input.scopeId >> input.scopePrio >> input.scopeName;
    return stream;
}

/**
 * \brief   Serializes a scope update structure to the stream.
 * \param   stream  The streaming object to save scope update information.
 * \param   output  The source of scope update structure to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NELogging::sScopeInfo & output)
{
    stream << output.scopeId << output.scopePrio << output.scopeName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace inline methods
//////////////////////////////////////////////////////////////////////////////

inline const char* NELogging::getString(NELogging::eLogPriority prio)
{
    switch ( prio )
    {
    case NELogging::eLogPriority::PrioInvalid:
        return "NELogging::PrioInvalid";
    case NELogging::eLogPriority::PrioNotset:
        return "NELogging::PrioNotset";
    case NELogging::eLogPriority::PrioFatal:
        return "NELogging::PrioFatal";
    case NELogging::eLogPriority::PrioScope:
        return "NELogging::PrioScope";
    case NELogging::eLogPriority::PrioError:
        return "NELogging::PrioError";
    case NELogging::eLogPriority::PrioWarning:
        return "NELogging::PrioWarning";
    case NELogging::eLogPriority::PrioInfo:
        return "NELogging::PrioInfo";
    case NELogging::eLogPriority::PrioDebug:
        return "NELogging::PrioDebug";
    case NELogging::eLogPriority::PrioLogs:
        return "NELogging::PrioLogs";
    case NELogging::eLogPriority::PrioValidLogs:
        return "NELogging::PrioValidLogs";
    case NELogging::eLogPriority::PrioIgnore:
        return "NELogging::PrioIgnore";
    case NELogging::eLogPriority::PrioIgnoreLayout:
        return "NELogging::PrioIgnoreLayout";
    case NELogging::eLogPriority::PrioAny:
        return "NELogging::PrioAny";
    case NELogging::eLogPriority::PrioValid:
        return "NELogging::PrioValid";
    default:
        ASSERT(false);
        return "ERR: Unexpected NELogging::eLogPrior value";
    }
}

inline NELogging::sScopeInfo::sScopeInfo(void)
    : scopeId   ( 0u )
    , scopePrio ( static_cast<uint32_t>(NELogging::eLogPriority::PrioInvalid) )
    , scopeName (String::EmptyString)
{
}

inline NELogging::sScopeInfo::sScopeInfo(const char* name, uint32_t id, uint32_t prio)
    : scopeId   (id)
    , scopePrio (prio)
    , scopeName (name)
{
}

inline bool NELogging::isValidLogPriority( NELogging::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NELogging::eLogPriority::PrioValid)) != 0;
}

inline bool NELogging::isLogPriority( NELogging::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NELogging::eLogPriority::PrioValidLogs)) != 0;
}

inline bool NELogging::isEnablingLog( NELogging::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NELogging::eLogPriority::PrioLogs)) != 0;
}

inline bool NELogging::isLogScope( NELogging::eLogPriority prio )
{
    return (prio == NELogging::eLogPriority::PrioScope);
}

inline bool NELogging::isDisablingLog( NELogging::eLogPriority prio )
{
    return (prio == NELogging::eLogPriority::PrioNotset) || (prio == NELogging::eLogPriority::PrioInvalid);
}

inline const String& NELogging::logPrioToString(NELogging::eLogPriority prio)
{
    switch (prio)
    {
    case NELogging::eLogPriority::PrioNotset:
        return NELogging::PRIO_NOTSET_STR;
    case NELogging::eLogPriority::PrioScope:
        return NELogging::PRIO_SCOPE_STR;
    case NELogging::eLogPriority::PrioFatal:
        return NELogging::PRIO_FATAL_STR;
    case NELogging::eLogPriority::PrioError:
        return NELogging::PRIO_ERROR_STR;
    case NELogging::eLogPriority::PrioWarning:
        return NELogging::PRIO_WARNING_STR;
    case NELogging::eLogPriority::PrioInfo:
        return NELogging::PRIO_INFO_STR;
    case NELogging::eLogPriority::PrioDebug:
        return NELogging::PRIO_DEBUG_STR;

    case NELogging::eLogPriority::PrioInvalid:        // fall through
    case NELogging::eLogPriority::PrioLogs:           // fall through
    case NELogging::eLogPriority::PrioValidLogs:      // fall through
    case NELogging::eLogPriority::PrioIgnore:         // fall through
    case NELogging::eLogPriority::PrioIgnoreLayout:   // fall through
    case NELogging::eLogPriority::PrioAny:            // fall through
    case NELogging::eLogPriority::PrioValid:          // fall through
    default:
        return NELogging::PRIO_NO_PRIO;
    }
}

inline NELogging::eLogPriority NELogging::stringToLogPrio(const String& prio)
{
    if (NELogging::PRIO_DEBUG_STR == prio)
        return NELogging::eLogPriority::PrioDebug;
    else if (NELogging::PRIO_INFO_STR == prio)
        return NELogging::eLogPriority::PrioInfo;
    else if (NELogging::PRIO_WARNING_STR == prio)
        return NELogging::eLogPriority::PrioWarning;
    else if (NELogging::PRIO_ERROR_STR == prio)
        return NELogging::eLogPriority::PrioError;
    else if (NELogging::PRIO_FATAL_STR == prio)
        return NELogging::eLogPriority::PrioFatal;
    else
        return NELogging::eLogPriority::PrioIgnoreLayout;
}

#endif  // AREG_LOGGING_NELOGGING_HPP
