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
namespace NELogging
{
    /**
     * \brief   The supported logging version
     **/
    constexpr std::string_view LOG_VERSION{ "3.0.0" };

    /**
     * \brief   The list of the scopes. It is a pair, where the key is the ID of the scope
     *          and the value is the pointer to the scope.
     **/
    using ScopeList     = HashMap<uint32_t, LogScope*>;

    /**
     * \brief   Alias of the map position.
     **/
    using SCOPEPOS      = ScopeList::MAPPOS;

    /**
     * \brief   Structure to keep scope information, composed of scope name, scope ID, and scope
     *          priority values. Used to generate scope priority update messages.
     **/
    struct ScopeEntry
    {
        /**
         * \brief
         **/
        inline ScopeEntry();

        /**
         * \brief   Initializes the scope name, ID, and priority.
         *
         * \param   name    The name of the scope.
         * \param   id      The scope ID. Can be 0 if the ID is unknown or the name is a scope
         *                  group.
         * \param   prio    The scope message priority.
         **/
        inline ScopeEntry(const char* name, uint32_t id, uint32_t prio);

        uint32_t    scopeId;    //!< The scope ID, can be 0 (NELogging::LOG_SCOPE_ID_NONE). For scope group should be 0.
        uint32_t    scopePrio;  //!< The scope priority.
        String      scopeName;  //!< The name of the scope or scope group.
    };

    //!< The list of scope update structure.
    using ScopeNames    = ArrayList<ScopeEntry>;

    /**
     * \brief   NELogging::LogTarget
     *          The logging types in Areg framework
     **/
    enum class LogTarget    : uint32_t
    {
          Undefined  = 0 //!< Logging is undefined
        , Remote     = 1 //!< Logging is to remote log collector
        , File       = 2 //!< Logging is directly in file as plain text
        , Debug      = 4 //!< Logging is on debug output console
        , Database   = 8 //!< Logging is in database, not implemented yet.
    };

    /**
     * \brief   NELogging::LogPriority
     *          Log priority definition set when logging message
     **/
    enum class LogPriority : uint16_t
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
     * \brief   Converts a log priority value to its string representation.
     **/
    inline const char * as_string( NELogging::LogPriority prio );

    /**
     * \brief   Returns true if the specified log priority value is valid.
     **/
    inline bool is_valid_priority( NELogging::LogPriority prio );

    /**
     * \brief   Returns true if the specified priority refers to logging.
     **/
    inline bool is_log_priority( NELogging::LogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables logging.
     **/
    inline bool is_enabling_log( NELogging::LogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables scope logging.
     **/
    inline bool is_log_scope( NELogging::LogPriority prio );

    /**
     * \brief   Returns true if the specified priority disables logging.
     **/
    inline bool is_disabling_log( NELogging::LogPriority prio );

    /**
     * \brief   Returns the cookie of the log collector.
     **/
    AREG_API const ITEM_ID & cookie();

    /**
     * \brief   NELogging::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr uint32_t  HAS_MESSAGE_PRIORITY    { 0x1F }; // 63, bit set 0001 1111

    /**
     * \brief   NELogging::LOG_SCOPE_ID_NONE
     *          Constant, defines no scope ID. It is used to output message without scope
     **/
    constexpr uint32_t  LOG_SCOPE_ID_NONE     { 0 };

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
     * \brief   Converts a log priority value to its string representation.
     *
     * \param   prio    The priority to convert.
     * \return  The string value of the priority.
     **/
    inline const String& priority_to_string(NELogging::LogPriority prio);

    /**
     * \brief   Converts a string to its corresponding log priority value.
     *
     * \param   prio    The priority string value to convert. Case-insensitive. Valid values:
     *                  NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \return  The log priority value corresponding to the string.
     **/
    inline NELogging::LogPriority string_to_priority(const String& prio);

    /**
     * \brief   Converts a bitwise set of priorities to a human-readable string.
     *
     * \param   priorities      The bitwise set of priorities to convert.
     * \return  The converted string representation with multiple priorities separated by '|'.
     **/
    AREG_API String make_prio_string(uint32_t priorities);

    /**
     * \brief   Converts a human-readable string with priorities separated by '|' to a bitwise set.
     *
     * \param   prio    The human-readable string with priorities separated by '|' to convert.
     * \return  The converted integer value with priorities set bitwise.
     **/
    AREG_API uint32_t make_priorities(const String& prio);

    /**
     * \brief   NELogging::LOG_MESSAGE_IZE
     *          The maximum size of text in log message
     **/
    constexpr uint32_t  LOG_MESSAGE_IZE     { 332 };
    /**
     * \brief   NELogging::LOG_NAMES_SIZE
     *          The maximum length of the names in logging objects
     **/
    constexpr uint32_t   LOG_NAMES_SIZE     { 48 };

    /**
     * \brief   NELogging::LogMessageType
     *          The logging message type.
     **/
    enum class LogMessageType  : uint8_t
    {
          Undefined     = 0 //!< The log origin is undefined.
        , ScopeEnter    = 1 //!< Indicates the begin of the logging scope.
        , MessageText   = 2 //!< Indicates the message to log.
        , ScopeExit     = 4 //!< Indicates the end of the logging scope.
    };

    /**
     * \brief   NELogging::LogDataType
     *          The data type in the message log
     **/
    enum class LogDataType : uint8_t
    {
          Local     = 0 //!< The message data is generated locally.
        , Remote    = 1 //!< The message data is prepared for remote logging.
    };

    /**
     * \brief   The structure of logging message object to output on target (log collector or
     *          observer).
     **/
    struct AREG_API LogEntry
    {
        /**
         * \brief   Initializes logging message of specified type.
         *
         * \param   msgType     The logging message type.
         **/
        LogEntry( NELogging::LogMessageType msgType = NELogging::LogMessageType::Undefined );
        /**
         * \brief   Initializes logging message and sets specified data.
         *
         * \param   msgType         The logging message type.
         * \param   scopeId         The ID of message scope.
         * \param   sessionId       The ID of session, which is used to differentiate messages of
         *                          the same scope.
         * \param   scopeStamp      The timestamp of the scope message, which is used to log
         *                          message. This parameter is used to set duration. The duration is
         *                          ignored and set to 0 if the scopeStamp is 0.
         * \param   msgPrio         The priority of logging message.
         * \param   message         The message text to output on target. Can be empty.
         * \param   msgLen          The length of the message string.
         **/
        LogEntry(NELogging::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, NELogging::LogPriority msgPrio, const char * message, uint32_t msgLen);
        /**
         * \brief   Copies data from given source.
         *
         * \param   src     The source to copy data from.
         **/
        LogEntry( const LogEntry & src );

        /**
         * \brief   Copies data from given source.
         *
         * \param   src     The source to copy data from.
         **/
        LogEntry & operator = (const LogEntry & src);

        NELogging::LogDataType      logDataType{ LogDataType::Local };          //!< The type of log message data.
        NELogging::LogMessageType   logMsgType{ LogMessageType::Undefined };    //!< The type of the logging message.
        NELogging::LogPriority      logMessagePrio{ LogPriority::PrioInvalid }; //!< The log message priority
        ITEM_ID                     logSource{ 0 };     //!< The ID of the source that generated logging message.
        ITEM_ID                     logTarget{ 0 };     //!< The ID of the target to send logging message, valid only in case of TCP/IP logging.
        ITEM_ID                     logCookie{ 0 };     //!< The cookie set by the networking service, i.e. the log collector. Valid only in case of TCP/IP logging.
        ITEM_ID                     logModuleId{ 0 };   //!< The ID of the process in the local machine.
        ITEM_ID                     logThreadId{ 0 };   //!< The ID the thread in the local process.
        TIME64                      logTimestamp{ 0 };  //!< The timestamp of generated log.
        TIME64                      logReceived{ 0 };   //!< The timestamp when the log message is updated.
        uint32_t                    logDuration{ 0 };   //!< The duration in microseconds after scope message is instantiated in the method call.
        uint32_t                    logScopeId{ 0 };    //!< The ID of log scope that generated log message
        uint32_t                    logSessionId{ 0 };  //!< The session ID of the logging message, valid only in case of remote logging.
        uint32_t                    logMessageLen{ 0 }; //!< The actual length of the log message
        char                        log_message[LOG_MESSAGE_IZE]{0}; //!< The message text to output, with maximum NELogging::LOG_MESSAGE_IZE characters.
        uint32_t                    logThreadLen{ 0 };              //!< The length of the thread name;
        char                        logThread[LOG_NAMES_SIZE]{ 0 }; //!< The name of the thread that generated the log. Valid only for remote logging
        uint32_t                    logModuleLen{ 0 };              //!< The length of the module name.
        char                        logModule[LOG_NAMES_SIZE]{ 0 }; //!< The name of the module that generated the log. Valid only for remote logging.
    };

    /**
     * \brief   Configures and starts logging, optionally loading configuration from a file.
     *
     * \param   fileConfig      Path to the logging configuration file. If nullptr, uses the default
     *                          configuration file if logging was previously configured, or default
     *                          settings if not.
     * \return  Returns true if succeeded to configure and start logging.
     **/
    AREG_API bool start_logging( const char * fileConfig = nullptr );

    /**
     * \brief   Forces logging to start with default configuration values.
     *
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool force_start_logging();

    /**
     * \brief   Loads logging configuration and starts logging, using default values if the
     *          configuration file is unavailable.
     *
     * \param   fileConfig      Path to the logging configuration file. If nullptr or empty,
     *                          attempts to load the default configuration file. If no configuration
     *                          file is found, uses default values.
     * \return  Returns true if succeeded to start logging.
     **/
    AREG_API bool init_logging( const char * fileConfig = nullptr );

    /**
     * \brief   Stops logging and exits the logging thread.
     *
     * \param   waitComplete    If true, blocks until logging completes and cleans resources.
     *                          Otherwise, triggers exit and returns immediately.
     **/
    AREG_API void stop_logging( bool waitComplete);

    /**
     * \brief   Blocks until logging completes and exits.
     **/
    AREG_API void wait_logging_end();

    /**
     * \brief   Activates a log scope and sets its logging priority from the configuration.
     *
     * \param   logScope    The log scope instance to activate. If no priority is configured, the
     *                      scope remains unable to log.
     * \note    Activation alone does not guarantee logging. Messages are logged only if a
     *          non-NOTSET priority is configured for the scope.
     **/
    AREG_API void activate_scope( LogScope & logScope );

    /**
     * \brief   Returns true if logging has been started.
     **/
    AREG_API bool is_started();

    /**
     * \brief   Returns true if logging has been configured.
     **/
    AREG_API bool is_configured();

    /**
     * \brief   Returns true if logging is enabled.
     **/
    AREG_API bool is_enabled();

    /**
     * \brief   Initializes logging by reading configuration from a file.
     *
     * \param   fileConfig      Path to the logging configuration file. If empty or nullptr, uses
     *                          the default configuration file path.
     * \return  Returns true if succeeded to open the file and configure logging.
     **/
    AREG_API bool initialize_logging( const char * fileConfig );

    /**
     * \brief   Saves the current logging configuration state to a file.
     *
     * \param   configFile      Path to the configuration file. If nullptr, uses the currently
     *                          active configuration file.
     * \return  Returns true if succeeded to save the configuration.
     **/
    AREG_API bool save_logging( const char * configFile = nullptr );

    /**
     * \brief   Generates an ID for the given scope name.
     *
     * \param   scopeName       The name of the scope.
     * \return  The ID of the scope.
     **/
    AREG_API uint32_t make_scope_id( const char * scopeName );

    /**
     * \brief   Generates an ID for the given scope name, with special handling for null, empty, or
     *          wildcard scope names.
     *
     * \param   scopeName       The name of the scope. Returns 0 if null, empty, or ends with '*'.
     * \return  The ID of the scope; 0 if the name is null, empty, or is a wildcard group.
     **/
    AREG_API uint32_t make_scope_id_ex(const char* scopeName);

    /**
     * \brief   Sets the logging priority for a scope.
     *
     * \param   scopeName       The name of the existing scope. Ignored if scope does not exist.
     * \param   newPrio         The new priority to set. Can be a bitwise combination of priorities.
     * \return  Returns the previous priority if the scope was found and priority changed.
     **/
    AREG_API uint32_t set_scope_priority( const char * scopeName, uint32_t newPrio );

    /**
     * \brief   Returns the priority of a scope.
     *
     * \param   scopeName       The name of the existing scope.
     * \return  The priority of the scope if found; otherwise, returns
     *          NELogging::LogPriority::PrioInvalid.
     **/
    AREG_API uint32_t scope_priority( const char * scopeName );

    /**
     * \brief   Creates a network message containing a log entry.
     *
     * \param   log_message     The logging message structure.
     * \param   dataType        The type of data to set in the message.
     * \param   srcCookie       The cookie of the source that generated the message.
     * \return  The message object ready for network communication.
     **/
    AREG_API RemoteMessage create_log_message(const NELogging::LogEntry& log_message, NELogging::LogDataType dataType, const ITEM_ID & srcCookie);

    /**
     * \brief   Logs a remote message contained in the buffer.
     *
     * \param   message     The buffer containing the information to log.
     **/
    AREG_API void log_message(const RemoteMessage& message);

    /**
     * \brief   Logs a custom message without including process and thread names.
     *
     * \param   log_message     The structure containing the message to log.
     **/
    AREG_API void log_local(const NELogging::LogEntry& log_message);

    /**
     * \brief   Logs a custom message with process and thread information.
     *
     * \param   log_message     The structure containing the message to log.
     **/
    AREG_API void log_any_message(const NELogging::LogEntry& log_message);

    /**
     * \brief   Creates a message to register scopes with their logging priorities.
     *
     * \param   source          The ID of the source that generated the message.
     * \param   target          The ID of the target to receive the message.
     * \param   scopeList       The list of scopes to register.
     * \return  The message ready to send from source to target.
     **/
    AREG_API RemoteMessage message_register_scopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeList & scopeList);

    /**
     * \brief   Creates a message to update the priorities of scopes or scope groups.
     *
     * \param   source          The ID of the source that generated the message.
     * \param   target          The ID of the target to receive the message.
     * \param   scopeNames      List of scope names, IDs, and priorities. Scope IDs can be omitted
     *                          (set to 0) for scope groups.
     * \return  The message ready to send from source to target.
     **/
    AREG_API RemoteMessage message_update_scopes(const ITEM_ID & source, const ITEM_ID & target, const NELogging::ScopeNames & scopeNames);

    /**
     * \brief   Creates a message to update the logging priority of a scope or scope group.
     *
     * \param   source          The ID of the source that generated the message.
     * \param   target          The ID of the target to receive the message.
     * \param   scopeName       The scope name (single scope or group ending with '*').
     * \param   scopeId         The ID of the scope. Can be 0 for scope groups.
     * \param   scopePrio       The new logging priority for the scope.
     * \return  The message ready to send from source to target.
     **/
    AREG_API RemoteMessage message_update_scope(const ITEM_ID& source, const ITEM_ID& target, const String & scopeName, uint32_t scopeId, uint32_t scopePrio);

    /**
     * \brief   Creates a message to query instances connected to the logging service.
     *
     * \param   source      The ID of the source (log observer or log collector) creating the query.
     * \param   target      The target ID or NEService::TARGET_ALL for all clients.
     * \return  The message ready to forward via the log collector service.
     **/
    AREG_API RemoteMessage message_query_instances(const ITEM_ID& source, const ITEM_ID& target);

    /**
     * \brief   Creates a message to query the list of scopes from connected clients.
     *
     * \param   source      The ID of the source (log observer or log collector). Ignored if created
     *                      by regular clients.
     * \param   target      The ID of the target or NEService::TARGET_ALL for all connected clients.
     * \return  The message ready to send from source to target.
     **/
    AREG_API RemoteMessage message_query_scopes(const ITEM_ID& source, const ITEM_ID& target);

    /**
     * \brief   Creates a message to notify that scope priorities have been changed.
     *
     * \param   source          The ID of the source creating the message.
     * \param   target          The ID of the target (typically NEService::COOKIE_LOGGER for
     *                          forwarding to all observers).
     * \param   scopeList       The list of all scopes, IDs, and their updated priorities.
     * \return  The message ready to send to the log collector.
     **/
    AREG_API RemoteMessage message_scopes_updated(const ITEM_ID& source, const ITEM_ID& target, const NELogging::ScopeList& scopeList);

    /**
     * \brief   Creates a message to request connected clients to save configuration.
     *
     * \param   source      The ID of the source (log observer or log collector).
     * \param   target      The ID of the target or NEService::TARGET_ALL to forward to all clients.
     * \return  The message ready to send to client(s) via the log collector service.
     **/
    AREG_API RemoteMessage message_save_configuration(const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   Creates a message to notify the log collector that configuration has been saved.
     *
     * \return  The message ready to send to the log collector.
     **/
    AREG_API RemoteMessage message_configuration_saved();

    /**
     * \brief   Sets the external logging database engine.
     **/
    AREG_API void set_db_engine(LogDatabaseEngine* dbEngine);
}

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace streamable types
//////////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_STREAMABLE(NELogging::LogPriority)
AREG_IMPLEMENT_STREAMABLE(NELogging::LogMessageType)

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
inline const InStream & operator >> (const InStream& stream, NELogging::LogEntry& input)
{
    stream.read(reinterpret_cast<uint8_t *>(&input), offsetof(NELogging::LogEntry, log_message));
    stream.read(reinterpret_cast<uint8_t *>(input.log_message), input.logMessageLen + 1);
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline OutStream& operator << (OutStream& stream, const NELogging::LogEntry& output)
{
    stream.write(reinterpret_cast<const uint8_t *>(&output), offsetof(NELogging::LogEntry, log_message));
    stream.write(reinterpret_cast<const uint8_t *>(output.log_message), output.logMessageLen + 1);
    return stream;
}

/**
 * \brief   De-serializes a scope update structure from the stream.
 * \param   stream  The source of data that contains scope update structure information.
 * \param   input   On output this contains structured scope update data.
 **/
inline const InStream& operator >> (const InStream& stream, NELogging::ScopeEntry & input)
{
    stream >> input.scopeId >> input.scopePrio >> input.scopeName;
    return stream;
}

/**
 * \brief   Serializes a scope update structure to the stream.
 * \param   stream  The streaming object to save scope update information.
 * \param   output  The source of scope update structure to serialize message.
 **/
inline OutStream& operator << (OutStream& stream, const NELogging::ScopeEntry & output)
{
    stream << output.scopeId << output.scopePrio << output.scopeName;
    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// NELogging namespace inline methods
//////////////////////////////////////////////////////////////////////////////

inline const char* NELogging::as_string(NELogging::LogPriority prio)
{
    switch ( prio )
    {
    case NELogging::LogPriority::PrioInvalid:
        return "NELogging::PrioInvalid";
    case NELogging::LogPriority::PrioNotset:
        return "NELogging::PrioNotset";
    case NELogging::LogPriority::PrioFatal:
        return "NELogging::PrioFatal";
    case NELogging::LogPriority::PrioScope:
        return "NELogging::PrioScope";
    case NELogging::LogPriority::PrioError:
        return "NELogging::PrioError";
    case NELogging::LogPriority::PrioWarning:
        return "NELogging::PrioWarning";
    case NELogging::LogPriority::PrioInfo:
        return "NELogging::PrioInfo";
    case NELogging::LogPriority::PrioDebug:
        return "NELogging::PrioDebug";
    case NELogging::LogPriority::PrioLogs:
        return "NELogging::PrioLogs";
    case NELogging::LogPriority::PrioScopeLogs:
        return "NELogging::PrioScopeLogs";
    case NELogging::LogPriority::PrioValidLogs:
        return "NELogging::PrioValidLogs";
    case NELogging::LogPriority::PrioIgnore:
        return "NELogging::PrioIgnore";
    case NELogging::LogPriority::PrioMarker:
        return "NELogging::PrioMarker";
    case NELogging::LogPriority::PrioMarkerError:
        return "NELogging::PrioMarkerError";
    case NELogging::LogPriority::PrioMarkerWarning:
        return "NELogging::PrioMarkerWarning";
    case NELogging::LogPriority::PrioMarkerInfo:
        return "NELogging::PrioMarkerInfo";
    case NELogging::LogPriority::PrioIgnoreLayout:
        return "NELogging::PrioIgnoreLayout";
    case NELogging::LogPriority::PrioAny:
        return "NELogging::PrioAny";
    case NELogging::LogPriority::PrioValid:
        return "NELogging::PrioValid";
    default:
        ASSERT(false);
        return "ERR: Unexpected NELogging::LogPriority value";
    }
}

inline NELogging::ScopeEntry::ScopeEntry()
    : scopeId   ( 0u )
    , scopePrio ( static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid) )
    , scopeName (String::EmptyString)
{
}

inline NELogging::ScopeEntry::ScopeEntry(const char* name, uint32_t id, uint32_t prio)
    : scopeId   (id)
    , scopePrio (prio)
    , scopeName (name)
{
}

inline bool NELogging::is_valid_priority( NELogging::LogPriority prio )
{
    return (static_cast<uint32_t>(prio) & static_cast<uint32_t>(NELogging::LogPriority::PrioValid)) != 0;
}

inline bool NELogging::is_log_priority( NELogging::LogPriority prio )
{
    return (static_cast<uint32_t>(prio) & static_cast<uint32_t>(NELogging::LogPriority::PrioValidLogs)) != 0;
}

inline bool NELogging::is_enabling_log( NELogging::LogPriority prio )
{
    return (static_cast<uint32_t>(prio) & static_cast<uint32_t>(NELogging::LogPriority::PrioLogs)) != 0;
}

inline bool NELogging::is_log_scope( NELogging::LogPriority prio )
{
    return (prio == NELogging::LogPriority::PrioScope);
}

inline bool NELogging::is_disabling_log( NELogging::LogPriority prio )
{
    return (prio == NELogging::LogPriority::PrioNotset) || (prio == NELogging::LogPriority::PrioInvalid);
}

inline const String& NELogging::priority_to_string(NELogging::LogPriority prio)
{
    switch (prio)
    {
    case NELogging::LogPriority::PrioNotset:
        return NELogging::PRIO_NOTSET_STR;
    case NELogging::LogPriority::PrioScope:
        return NELogging::PRIO_SCOPE_STR;
    case NELogging::LogPriority::PrioFatal:
        return NELogging::PRIO_FATAL_STR;
    case NELogging::LogPriority::PrioError:
        return NELogging::PRIO_ERROR_STR;
    case NELogging::LogPriority::PrioWarning:
        return NELogging::PRIO_WARNING_STR;
    case NELogging::LogPriority::PrioInfo:
        return NELogging::PRIO_INFO_STR;
    case NELogging::LogPriority::PrioDebug:
        return NELogging::PRIO_DEBUG_STR;

    case NELogging::LogPriority::PrioInvalid:        // fall through
    case NELogging::LogPriority::PrioLogs:           // fall through
    case NELogging::LogPriority::PrioValidLogs:      // fall through
    case NELogging::LogPriority::PrioIgnore:         // fall through
    case NELogging::LogPriority::PrioIgnoreLayout:   // fall through
    case NELogging::LogPriority::PrioAny:            // fall through
    case NELogging::LogPriority::PrioValid:          // fall through
    default:
        return NELogging::PRIO_NO_PRIO;
    }
}

inline NELogging::LogPriority NELogging::string_to_priority(const String& prio)
{
    if (NELogging::PRIO_DEBUG_STR == prio)
        return NELogging::LogPriority::PrioDebug;
    else if (NELogging::PRIO_INFO_STR == prio)
        return NELogging::LogPriority::PrioInfo;
    else if (NELogging::PRIO_WARNING_STR == prio)
        return NELogging::LogPriority::PrioWarning;
    else if (NELogging::PRIO_ERROR_STR == prio)
        return NELogging::LogPriority::PrioError;
    else if (NELogging::PRIO_FATAL_STR == prio)
        return NELogging::LogPriority::PrioFatal;
    else if (NELogging::PRIO_SCOPE_STR == prio)
        return NELogging::LogPriority::PrioScope;
    else if (NELogging::PRIO_NOTSET_STR == prio)
        return NELogging::LogPriority::PrioNotset;
    else
        return NELogging::LogPriority::PrioIgnoreLayout;
}

#endif  // AREG_LOGGING_LOGGINGDEFS_HPP
