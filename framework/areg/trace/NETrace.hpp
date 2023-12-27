#ifndef AREG_TRACE_NETRACE_HPP
#define AREG_TRACE_NETRACE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/NETrace.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, NETrace namespace contains constants, structures and types.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/component/NEService.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;
class FileBase;

//////////////////////////////////////////////////////////////////////////
// NETrace namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This namespace is a collection of helper objects like
 *          constants, structures, methods and types used
 *          to provide logging functionality.
 **/
namespace NETrace
{
    //!< The list of the scopes. It is a pair, where the key is the ID of the scope
    //!< and the value is the pointer to the scope.
    using ScopeList     = TEHashMap<unsigned int, TraceScope*>;
    //!< Alias of the map position.
    using SCOPEPOS      = ScopeList::MAPPOS;

    /**
     * \brief   NETrace::sScopeInfo
     *          The structure to keep scope information. It is used to generate scope priority update messages.
     *          The structure contains scope name, scope ID and scope priority values.
     **/
    struct sScopeInfo
    {
        String       scopeName; //!< The name of the scope or scope group.
        unsigned int scopeId;   //!< The scope ID, can be 0 (NETrace::TRACE_SCOPE_ID_NONE). For scope group should be 0.
        unsigned int scopePrio; //!< The scope priority.
    };

    //!< The list of scope update structure.
    using ScopeNames    = TEArrayList<sScopeInfo>;

    /**
     * \brief   The supported logging version
     **/
    constexpr std::string_view LOG_VERSION                  { "2.0.0" };

    /**
     * \brief   NETrace::eLogingTypes
     *          The logging types in AREG framework
     **/
    enum eLogingTypes
    {
          LogTypeUndefined  = 0 //!< Logging is undefined
        , LogTypeRemote     = 1 //!< Logging is to remote log collector
        , LogTypeFile       = 2 //!< Logging is directly in file as plain text
        , LogTypeDebug      = 4 //!< Logging is on debug output console
        , LogTypeDatabase   = 8 //!< Logging is in database, not implemented yet.
    };

    /**
     * \brief   NETrace::eLogPriority
     *          Log priority definition set when logging message
     **/
    enum eLogPriority : unsigned int
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
    };

    /**
     * \brief   NETrace::getString
     *          Converts NETrace::eLogPriority values to readable string values
     **/
    inline const char * getString( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified log priority value is valid.
     **/
    inline bool isValidLogPriority( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority refers to the logging.
     **/
    inline bool isLogPriority( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority enables logging, i.e. logs messages..
     **/
    inline bool isEnablingLog( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority makes scope logging.
     **/
    inline bool isLogScope( NETrace::eLogPriority prio );

    /**
     * \brief   Returns true if the specified priority does not log messages.
     **/
    inline bool isDisablingLog( NETrace::eLogPriority prio );

    /**
     * \brief   Returns the cookie of the logger.
     **/
    AREG_API const ITEM_ID & getCookie(void);

    /**
     * \brief   NETrace::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr unsigned int  HAS_MESSAGE_PRIORITY    { 0x1F }; // 63, bit set 0001 1111

    /**
     * \brief   NETrace::TRACE_SCOPE_ID_NONE
     *          Constant, defines no scope ID. It is used to output message without scope
     **/
    constexpr unsigned int  TRACE_SCOPE_ID_NONE     { 0 };

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
    const String  PRIO_WARNING_STR    { "WARNING" };
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
     * \brief   Returns string value of NETrace::eLogPriority.
     *          There are following valid string priority values:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority to get string value.
     * \return  Returns string priority value
     **/
    inline const String& logPrioToString(NETrace::eLogPriority prio);

    /**
     * \brief   From given string value returns log priority value.
     *          The string values should be followings:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   strPrio The priority string value to convert.
     *                  The given string is not case sensitive.
     * \return  Returns appropriate logging priority value.
     **/
    inline const NETrace::eLogPriority stringToLogPrio(const String& prio);

    /**
     * \brief   Converts the bitwise set of priority into the human readable string.
     * \param   priorities      The bitwise set of priorities integer value to convert to string.
     * \return  Returns converted string that may contain logical OR ('|') if more than one priority is set.
     **/
    AREG_API String makePrioString(unsigned int priorities);

    /**
     * \brief   Converts the human readable string with priorities separate by logical OR ('|')
     *          into bitwise set of integer value with priorities.
     * \param   priorities      The human readable string with priorities separated by logical OR ('|')
     *                          to convert into integer.
     * \return  Returns converted integer value where the priorities are set bitwise.
     **/
    AREG_API unsigned int makePriorities(const String& prioString);

    /**
     * \brief   NETrace::LOG_MESSAGE_IZE
     *          The maximum size of text in log message
     **/
    constexpr uint32_t  LOG_MESSAGE_IZE     { 512 };
    /**
     * \brief   NETrace::LOG_NAMES_SIZE
     *          The maximum length of the names in logging objects
     **/
    constexpr uint32_t   LOG_NAMES_SIZE     { 128 };

    /**
     * \brief   NETrace::eLogMessageType
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
     * \brief   NETrace::eLogDataType
     *          The data type in the message log
     **/
    enum class eLogDataType : unsigned short
    {
          LogDataLocal          = 0 //!< The message data is generated locally.
        , LogDataRemote         = 1 //!< The message data is prepared for remote logging.
    };

    /**
     * \brief   NETrace::sLogMessage
     *          The structure of logging message object to trace in output target.
     **/
    struct AREG_API sLogMessage
    {
        /**
         * \brief   Initializes logging message of specified type.
         * \param   msgType     The logging message type.
         **/
        sLogMessage( NETrace::eLogMessageType msgType = NETrace::eLogMessageType::LogMessageUndefined );
        /**
         * \brief   Initializes logging message and sets specified data.
         * \param   msgType     The logging message type.
         * \param   scopeId     The ID of message scope.
         * \param   mstPrio     The priority of logging message.
         * \param   message     The message text to output on target. Can be empty.
         **/
        sLogMessage(NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen);
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

        NETrace::eLogMessageType    logMsgType;     //!< The type of the logging message.
        NETrace::eLogDataType       logDataType;    //!< The type of log message data.
        NETrace::eLogPriority       logMessagePrio; //!< The log message priority
        ITEM_ID                     logSource;      //!< The ID of the source that generated logging message.
        ITEM_ID                     logTarget;      //!< The ID of the target to send logging message, valid only in case of TCP/IP logging.
        ITEM_ID                     logCookie;      //!< The cookie set by the networking service, i.e. the log collector. Valid only in case of TCP/IP logging.
        ITEM_ID                     logModuleId;    //!< The ID of the process in the local machine.
        ITEM_ID                     logThreadId;    //!< The ID the thread in the local process.
        TIME64                      logTimestamp;   //!< The timestamp of generated log.
        unsigned int                logScopeId;     //!< The ID of trace scope that generated log message
        unsigned int                logMessageLen;  //!< The actual length of the log message
        char                        logMessage[LOG_MESSAGE_IZE];//!< The message text to output, with maximum NETrace::LOG_MESSAGE_IZE characters.
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
     * \param   configFile
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
     * \brief   Marks specified scope as active, so that message related with scope can be logged.
     *          By default, the activated scope logging priority is NETrace::PrioNotset and
     *          the messages cannot be logged yet. If the configuration contains logging
     *          information of specified log and the information permits logging, 
     *          i.e. the logging priority is not equal to NETrace::PrioNotset,
     *          the message texts can be logged in output target.
     * \param   tarceScope  The instance of scope to activate to set appropriate logging priority.
     *                      If no priority information found, the messages of scope cannot be logged yet.
     * \note    It is not enough to activate scope to output message on the target.
     *          If configuration file does not have priority information, messages are not logged.
     *          If configuration file contains information of scope and the priority is not NETrace::PrioNotset,
     *          the system can log messages of appropriate scope.
     **/
    AREG_API void activateScope( TraceScope & traceScope );

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
     *          If logging is enabled in configuration file, 
     *          the system ignores logging.
     **/
    AREG_API bool isEnabled( void );

    /**
     * \brief   Initializes the logging by reading and configuration instructions
     *          from given logging file.
     * \param   fileConfig  The relative or absolute path of logging file.
     *                      If empty or nullptr, the system default path will be taken.
     * \return  Returns true if succeeded to open file and configure logging.
     **/
    AREG_API bool initializeLogging( const char * fileConfig );

    /**
     * \brief   Saves the current configuration of the logging state in the file.
     * \param   configFile  The pointer to the configuration file to save.
     * \return  Returns true if succeeded to save the current state of the logging.
     **/
    AREG_API bool saveLogging( const char * configFile = nullptr );

    /**
     * \brief   Returns the ID of given scope name.
     *          If scope name is nullptr or empty, it returns zero.
     * \param   scopeName   The name of scope. If nullptr or empty,
     *                      the return value is zero.
     * \return  Returns the ID of given scope name.
     **/
    AREG_API unsigned int makeScopeId( const char * scopeName );

    /**
     * \brief   Call to change the scope log priority.
     * \param   scopeName   The name of the existing scope. Ignored if scope does not exit.
     * \param   newPrio     The new priority to set. Can be bitwise combination with scopes.
     * \return  Returns true if scope found and priority changed.
     **/
    AREG_API unsigned int setScopePriority( const char * scopeName, unsigned int newPrio );

    /**
     * \brief   Returns the scope priority if found. Otherwise, returns invalid priority.
     * \param   scopeName   The name of the existing scope.
     * \return  Is found the scope, returns the actual priority of the scope.
     *          Otherwise, returns invalid priority (NETrace::eLogPriority::PrioInvalid).
     **/
    AREG_API unsigned int getScopePriority( const char * scopeName );

    /**
     * \brief   NETrace::eScopeList
     *          Defines the state of registering logging scopes.
     **/
    typedef enum E_ScopeList        : unsigned char
    {
          ScopeListUndefined    //!< The state is undefined.
        , ScopeListStart        //!< Indicates the begin to register the logging scopes
        , ScopeListContinue     //!< Indicates the continuation to register the logging scopes.
        , ScopeListEnd          //!< Indicates the end to register the logging scopes.
    } eScopeList;

    /**
     * \brief   Creates a network communication message to make a log.
     * \param   logMessage  The message log structure.
     * \param   dataType    The type of created data to set in the structure.
     * \param   srcCookie   The source of cookie to set in the structure.
     * \return  Returns message object for network communication.
     **/
    AREG_API RemoteMessage createLogMessage(const NETrace::sLogMessage& logMessage, NETrace::eLogDataType dataType, const ITEM_ID & srcCookie);

    /**
     * \brief   Triggers an event to log the message, contained in the remote buffer.
     * \param   message     The shared buffer with the information to log.
     **/
    AREG_API void logMessage(const RemoteMessage& message);

    /**
     * \brief   The maximum scope entries in one message.
     **/
    constexpr uint32_t  SCOPE_ENTRIES_MAX_COUNT     { 1'000u };

    /**
     * \brief   Creates a message for logging service to start registering application logging scopes.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message
     * \param   scopeCount  The number of scopes to send to register.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageRegisterScopesStart( const ITEM_ID & source, const ITEM_ID & target, unsigned int scopeCount );

    /**
     * \brief   Creates a message for logging service to end registering application logging scopes.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageRegisterScopesEnd(const ITEM_ID & source, const ITEM_ID & target);

    /**
     * \brief   Creates a message for logging service to register scopes with message priority.
     * \param   source      The ID of the source that generated the message.
     * \param   target      The ID of the target to send the message
     * \param   scopeList   The list of scopes to register.
     * \param   startAt     The position in the list to extract and register scopes.
     *                          - If in input the value is invalid, it starts to register from beginning.
     *                          - If on output the value is valid, there are still unregistered scopes 
     *                            in the list and the value indicates the next valid scope.
     *                          - In on output the value is invalid, there are no more unregistered scopes
     *                            in the list.
     * \param   maxEntries  The maximum entries to push to scope registering message.
     *                      If the value is 0xFFFFFFFF, it registers all entries.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageRegisterScopes(const ITEM_ID & source, const ITEM_ID & target, const NETrace::ScopeList & scopeList, NETrace::SCOPEPOS & startAt, unsigned int maxEntries = 0xFFFFFFFF );

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
    AREG_API RemoteMessage messageUpdateScopes(const ITEM_ID & source, const ITEM_ID & target, const NETrace::ScopeNames & scopeNames);

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
     * \brief   Creates a message to query the list of scopes of connected client applications.
     *          After option it should follow either '*' for all connected clients or the list of connected cookies.
     *          The cookie ID is specified, the message is sent to the specified target. Otherwise, the message is sent
     *          to all connected clients. An empty command after option is considered as sending message to all clients.
     *          Only observers and the log collector service (logger) can generate and send the message.
     *          The message is ignored if the client creates and send the message.
     * \param   source      The ID of the source that generated the message.
     *                      The source should be either observer or NEService::COOKIE_LOGGER.
     * \param   target      The ID of the target to send the message.
     *                      If the ID is NEService::COOKIE_ANY, the message is sent to all connected clients.
     * \return  Returns generated message ready to send from indicated source to the target.
     **/
    AREG_API RemoteMessage messageQueryScopes(const ITEM_ID& source, const ITEM_ID& target);
}

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace streamable types
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NETrace::eLogPriority)
IMPLEMENT_STREAMABLE(NETrace::eScopeList)
IMPLEMENT_STREAMABLE(NETrace::eLogMessageType)

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace objects
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////////

/**
 * \brief   Deserializes log message from the stream.
 * \param   stream  The source of log message data.
 * \param   input   On output this contains structured logging message.
 **/
inline const IEInStream & operator >> (const IEInStream& stream, NETrace::sLogMessage& input)
{
    stream.read(reinterpret_cast<unsigned char *>(&input), offsetof(NETrace::sLogMessage, logMessage));
    stream.read(reinterpret_cast<unsigned char *>(input.logMessage), input.logMessageLen + 1);
    return stream;
}

/**
 * \brief   Serializes log message to the stream.
 * \param   stream  The streaming object to save log message.
 * \param   output  The source of log message to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sLogMessage& output)
{
    stream.write(reinterpret_cast<const unsigned char *>(&output), offsetof(NETrace::sLogMessage, logMessage));
    stream.write(reinterpret_cast<const unsigned char *>(output.logMessage), output.logMessageLen + 1);
    return stream;
}

/**
 * \brief   Deserializes a scope update structure from the stream.
 * \param   stream  The source of data that contains scope update structure information.
 * \param   input   On output this contains structured scope update data.
 **/
inline const IEInStream& operator >> (const IEInStream& stream, NETrace::sScopeInfo & input)
{
    stream >> input.scopeName >> input.scopeId >> input.scopePrio;
    return stream;
}

/**
 * \brief   Serializes a scope update structure to the stream.
 * \param   stream  The streaming object to save scope update information.
 * \param   output  The source of scope update structure to serialize message.
 **/
inline IEOutStream& operator << (IEOutStream& stream, const NETrace::sScopeInfo & output)
{
    stream << output.scopeName << output.scopeId << output.scopePrio;
    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace inline methods
//////////////////////////////////////////////////////////////////////////////

inline const char* NETrace::getString(NETrace::eLogPriority prio)
{
    switch ( prio )
    {
    case NETrace::eLogPriority::PrioInvalid:
        return "NETrace::PrioInvalid";
    case NETrace::eLogPriority::PrioNotset:
        return "NETrace::PrioNotset";
    case NETrace::eLogPriority::PrioFatal:
        return "NETrace::PrioFatal";
    case NETrace::eLogPriority::PrioScope:
        return "NETrace::PrioScope";
    case NETrace::eLogPriority::PrioError:
        return "NETrace::PrioError";
    case NETrace::eLogPriority::PrioWarning:
        return "NETrace::PrioWarning";
    case NETrace::eLogPriority::PrioInfo:
        return "NETrace::PrioInfo";
    case NETrace::eLogPriority::PrioDebug:
        return "NETrace::PrioDebug";
    case NETrace::eLogPriority::PrioLogs:
        return "NETrace::PrioLogs";
    case NETrace::eLogPriority::PrioValidLogs:
        return "NETrace::PrioValidLogs";
    case NETrace::eLogPriority::PrioIgnore:
        return "NETrace::PrioIgnore";
    case NETrace::eLogPriority::PrioIgnoreLayout:
        return "NETrace::PrioIgnoreLayout";
    case NETrace::eLogPriority::PrioAny:
        return "NETrace::PrioAny";
    case NETrace::eLogPriority::PrioValid:
        return "NETrace::PrioValid";
    default:
        ASSERT(false);
        return "ERR: Unexpected NETrace::eLogPrior value";
    }
}

inline bool NETrace::isValidLogPriority( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioValid)) != 0;
}

inline bool NETrace::isLogPriority( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioValidLogs)) != 0;
}

inline bool NETrace::isEnablingLog( NETrace::eLogPriority prio )
{
    return (static_cast<unsigned int>(prio) & static_cast<unsigned int>(NETrace::eLogPriority::PrioLogs)) != 0;
}

inline bool NETrace::isLogScope( NETrace::eLogPriority prio )
{
    return (prio == NETrace::eLogPriority::PrioScope);
}

inline bool NETrace::isDisablingLog( NETrace::eLogPriority prio )
{
    return (prio == NETrace::eLogPriority::PrioNotset) || (prio == NETrace::eLogPriority::PrioInvalid);
}

inline const String& NETrace::logPrioToString(NETrace::eLogPriority prio)
{
    switch (prio)
    {
    case NETrace::eLogPriority::PrioNotset:
        return NETrace::PRIO_NOTSET_STR;
    case NETrace::eLogPriority::PrioScope:
        return NETrace::PRIO_SCOPE_STR;
    case NETrace::eLogPriority::PrioFatal:
        return NETrace::PRIO_FATAL_STR;
    case NETrace::eLogPriority::PrioError:
        return NETrace::PRIO_ERROR_STR;
    case NETrace::eLogPriority::PrioWarning:
        return NETrace::PRIO_WARNING_STR;
    case NETrace::eLogPriority::PrioInfo:
        return NETrace::PRIO_INFO_STR;
    case NETrace::eLogPriority::PrioDebug:
        return NETrace::PRIO_DEBUG_STR;

    case NETrace::eLogPriority::PrioLogs:           // fall through
    case NETrace::eLogPriority::PrioValidLogs:      // fall through
    case NETrace::eLogPriority::PrioIgnore:         // fall through
    case NETrace::eLogPriority::PrioIgnoreLayout:   // fall through
    case NETrace::eLogPriority::PrioAny:            // fall through
    case NETrace::eLogPriority::PrioValid:          // fall through
    default:
        return NETrace::PRIO_NO_PRIO;
    }
}

inline const NETrace::eLogPriority NETrace::stringToLogPrio(const String& prio)
{
    if (NETrace::PRIO_DEBUG_STR == prio)
        return NETrace::eLogPriority::PrioDebug;
    else if (NETrace::PRIO_INFO_STR == prio)
        return NETrace::eLogPriority::PrioInfo;
    else if (NETrace::PRIO_WARNING_STR == prio)
        return NETrace::eLogPriority::PrioWarning;
    else if (NETrace::PRIO_ERROR_STR == prio)
        return NETrace::eLogPriority::PrioError;
    else if (NETrace::PRIO_FATAL_STR == prio)
        return NETrace::eLogPriority::PrioFatal;
    else
        return NETrace::eLogPriority::PrioIgnoreLayout;
}

#endif  // AREG_TRACE_NETRACE_HPP
