#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/NETrace.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, NETrace namespace contains constants, structures and types.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;

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
    /**
     * \brief   NETrace::eLogPriority
     *          Log priority definition set when logging message
     **/
    typedef enum E_LogPriority : unsigned int
    {
          PrioNotset        = 0x00  //!< No priority is set,        bit set:    0000 0000
        , PrioScope         = 0x01  //!< Output scopes priority,    bit set:    0000 0001
        , PrioFatal         = 0x02  //!< Fatal error log priority,  bit set:    0000 0010
        , PrioError         = 0x04  //!< Error log priority,        bit set:    0000 0100
        , PrioWarning       = 0x08  //!< Warning log priority,      bit set:    0000 1000
        , PrioInfo          = 0x10  //!< Information log priority,  bit set:    0001 0000
        , PrioDebug         = 0x20  //!< Debug log priority,        bit set:    0010 0000
        , PrioIgnore        = 0x40  //!< Ignore logging priority,   bit set:    0100 0000
        , PrioIgnoreLayout  = 0xC0  //!< Ignore layout priority,    bit set:    1100 0000
        , PrioAny           = 0xFF  //!< Log without priority,      bit set:    1111 1111
    } eLogPriority;

    /**
     * \brief   NETrace::GetString
     *          Converts NETrace::eLogPriority values to readable string values
     **/
    inline const char * getString( NETrace::eLogPriority prio );

    /**
     * \brief   NETrace::ToString
     *          Returns string value of NETrace::eLogPriority.
     *          There are following valid string priority values:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   prio    The priority to get string value.
     * \return  Returns string priority value
     * \see     NETrace::FromString
     **/
    AREG_API const char * convToString( NETrace::eLogPriority prio );

    /**
     * \brief   NETrace::FromString
     *          From given string value returns log priority value.
     *          The string values should be followings:
     *          NOTSET, SCOPE, FATAL, ERROR, WARNING, INFO, DEBUG.
     * \param   strPrio The priority string value to convert.
     *                  The given string is not case sensitive.
     * \return  Returns appropriate logging priority value.
     **/
    AREG_API NETrace::eLogPriority convFromString( const char * strPrio );

    /**
     * \brief   NETrace::HAS_MESSAGE_PRIORITY
     *          Flag, indicating whether there is any priority set to output message.
     *          The log has priority if one of first 5 bits are set.
     **/
    constexpr unsigned int  HAS_MESSAGE_PRIORITY    = 0x1F; // 63, bit set 0001 1111

    /**
     * \brief   NETrace::TRACE_SCOPE_ID_NONE
     *          Constant, defines no scope ID. It is used to output message without scope
     **/
    constexpr unsigned int  TRACE_SCOPE_ID_NONE     = 0;

    /**
     * \brief   NETrace::eLogType
     *          The logging type IDs. Should be set when sending log message
     **/
    typedef enum E_LogType : unsigned int
    {
          LogUndefined  = 0x00  //!< Undefined log message
        , LogCommand    = 0x96  //!< Log message contains command
        , LogScopeEnter = 0x97  //!< Enter to scope log message
        , LogScopeExit  = 0x98  //!< Exit from scope log message
        , LogMessage    = 0x99  //!< Text log message
    } eLogType;

    /**
     * \brief   The string value of no priority
     **/
    constexpr std::string_view  PRIO_NOTSET_STR    { "NOTSET" };
    /**
     * \brief   The string value of scope priority
     **/
    constexpr std::string_view  PRIO_SCOPE_STR      { "SCOPE" };
    /**
     * \brief   The string value of fatal error priority
     **/
    constexpr std::string_view  PRIO_FATAL_STR      { "FATAL" };
    /**
     * \brief   The string value of error priority
     **/
    constexpr std::string_view  PRIO_ERROR_STR      { "ERROR" };
    /**
     * \brief   The string value of warning priority
     **/
    constexpr std::string_view  PRIO_WARNING_STR    { "WARNING" };
    /**
     * \brief   The string value of information priority
     **/
    constexpr std::string_view  PRIO_INFO_STR       { "INFO" };
    /**
     * \brief   The string value of debug priority
     **/
    constexpr std::string_view  PRIO_DEBUG_STR      { "DEBUG" };
    /**
     * \brief   No priority string
     **/
    constexpr std::string_view  PRIO_NO_PRIO        { "" };

    /**
     * \brief   NETrace::LOG_MESSAGE_BUFFER_SIZE
     *          The maximum size of text in log message
     **/
    constexpr unsigned int      LOG_MESSAGE_BUFFER_SIZE     = 512;

    /**
     * \brief   NETrace::sLogHeader
     *          Structure of logging message header
     **/
    typedef struct AREG_API S_LogHeader
    {
        DECLARE_NOMOVE( S_LogHeader );

        /**
         * \brief   Initializes structure data and sets logging type.
         * \param   logType The type of log message.
         **/
        S_LogHeader( NETrace::eLogType logType = NETrace::LogUndefined );
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        S_LogHeader( const NETrace::S_LogHeader & src );
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        S_LogHeader & operator = ( const NETrace::S_LogHeader & src );

        unsigned int        logLength;      //!< The length in bytes of complete log object
        NETrace::eLogType   logType;        //!< The type of log message
        ITEM_ID             logModuleId;    //!< The ID of log module
    } sLogHeader;

    /**
     * \brief   NETrace::sLogData
     *          The structure of output logging message.
     **/
    typedef struct S_LogData
    {
        DECLARE_NOMOVE( S_LogData );

        /**
         * \brief   Initializes empty log tracing object with no data to output.
         **/
        S_LogData( void );
        /**
         * \brief   Initializes log tracing object, sets scope ID, priority and optional message.
         * \param   scopeId     The ID scope, which is calculated from name.
         * \param   msgPrio     The priority of output message.
         * \param   message     The message to output. Can be nullptr or empty.
         **/
        S_LogData( unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen );
        /**
         * \brief   Copies log tracing data from given source.
         * \param   src     The source of data to copy.
         **/
        S_LogData( const S_LogData & src );
        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        S_LogData  & operator = ( const S_LogData & src );

        ITEM_ID                 traceThreadId;      //!< The ID of thread, which is logging
        unsigned int            traceScopeId;       //!< The ID of trace scope, which is logging
        TIME64                  traceTimestamp;     //!< The time-stamp of trace message
        NETrace::eLogPriority   traceMessagePrio;   //!< The message priority to output
        unsigned int            traceMessageLen;    //!< The actual length of the message
        char                    traceMessage[LOG_MESSAGE_BUFFER_SIZE];  //!< The message text to output, with maximum NETrace::LOG_MESSAGE_BUFFER_SIZE characters
    } sLogData;

    /**
     * \brief   NETrace::sLogMessage
     *          The structure of logging message object to trace in output target.
     **/
    typedef struct S_LogMessage
    {
        DECLARE_NOMOVE( S_LogMessage );

        /**
         * \brief   Initializes logging message object of undefined type
         **/
        S_LogMessage( void );
        /**
         * \brief   Initializes logging message of specified type.
         * \param   logType     The type of logging message to set.
         **/
        S_LogMessage( NETrace::eLogType logType );
        /**
         * \brief   Initializes logging message and sets specified data.
         * \param   logType     The type of logging message to set.
         * \param   scopeId     The ID of message scope.
         * \param   mstPrio     The priority of logging message.
         * \param   message     The message text to output on target. Can be empty.
         **/
        S_LogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen);
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        S_LogMessage( const NETrace::S_LogMessage & src );
        /**
         * \brief   Copies data from given source.
         * \param   src     The source to copy data.
         **/
        S_LogMessage & operator = ( const S_LogMessage & src );

        NETrace::sLogHeader lmHeader;   //!< The logging message header.
        NETrace::sLogData   lmTrace;    //!< The logging message data with text to output on target.
    } sLogMessage;

    /**
     * \brief   Configures logging data from given logging file.
     * \param   fileConfig  The relative or absolute path of logging file.
     *                      If empty or nullptr, the system default path will be taken.
     * \return  Returns true if succeeded to open file and configure logging.
     **/
    AREG_API bool configureLoging( const char * fileConfig );

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
    AREG_API bool configAndStart( const char * fileConfig = nullptr );

    /**
     * \brief   Stops logging. No message will be logged anymore
     *          until it is not started again.
     **/
    AREG_API void stopLogging( void );

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
     * \brief   Returns the logging config file name.
     **/
    AREG_API const String& getConfigFile( void );
}

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace streamable types
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_STREAMABLE(NETrace::eLogPriority)
IMPLEMENT_STREAMABLE(NETrace::eLogType)
IMPLEMENT_STREAMABLE(NETrace::sLogMessage)

//////////////////////////////////////////////////////////////////////////////
// NETrace namespace inline functions
//////////////////////////////////////////////////////////////////////////////

inline const char * NETrace::getString( NETrace::eLogPriority prio )
{
    switch ( prio )
    {
    case NETrace::eLogPriority::PrioNotset:
        return "NETrace::PrioNotset";
    case NETrace::eLogPriority::PrioFatal:
        return "NETrace::PrioFatal";
    case NETrace::eLogPriority::PrioError:
        return "NETrace::PrioError";
    case NETrace::eLogPriority::PrioWarning:
        return "NETrace::PrioWarning";
    case NETrace::eLogPriority::PrioInfo:
        return "NETrace::PrioInfo";
    case NETrace::eLogPriority::PrioDebug:
        return "NETrace::PrioDebug";
    case NETrace::eLogPriority::PrioScope:
        return "NETrace::PrioScope";
    case NETrace::eLogPriority::PrioIgnore:
        return "NETrace::PrioIgnore";
    case NETrace::eLogPriority::PrioIgnoreLayout:
        return "NETrace::PrioIgnoreLayout";
    case NETrace::eLogPriority::PrioAny:
        return "NETrace::PrioAny";
    default:
        ASSERT(false);
        return "ERR: Unexpected NETrace::eLogPrior value";
    }
}
