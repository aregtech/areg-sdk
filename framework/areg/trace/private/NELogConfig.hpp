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
 * \file        areg/trace/private/NELogConfig.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, The namespace with helper constants
 *              of trace log configuration syntax and outputs
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NESocket.hpp"
#include "areg/base/NECommon.hpp"

#include <string_view>

 //////////////////////////////////////////////////////////////////////////
 // NELogConfig namespace declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief  NELogConfig namespace contains helper constants and syntax of configuration file.
  *         In configuration file there are such information like
  *         log enable / disable, scope (group of scopes) defines, 
  *         log target (file, remote host), comments, etc.
  **/
namespace NELogConfig
{
    /**
     * \brief   NELogConfig::eLogConfig
     *          The list of supported logging properties in configuration file
     **/
    typedef enum class E_LogConfig : int
    {
          ConfigUnknown             //!< Configuration property is unknown, should not be used
        , ConfigLogVersion          //!< Configuration property is version information
        , ConfigLogFile             //!< Configuration property is logging file name
        , ConfigLogRemoteHost       //!< Configuration property is logging service host name or IP-address to connect
        , ConfigLogRemotePort       //!< Configuration property is logging service port number to connect
        , ConfigLogDatabaseDriver   //!< Configuration property is database driver name to initialize
        , ConfigLogDatabaseHost     //!< Configuration property is database host to connect
        , ConfigLogDatabaseUser     //!< Configuration property is database user name to login
        , ConfigLogDatabasePwd      //!< Configuration property is database user password to login
        , ConfigLogDatabaseName     //!< Configuration property is database name to connect
        , ConfigLogDebug            //!< Configuration property is information to print logging message in output debug console
        , ConfigLogAppend           //!< Configuration property is instruction to append message in existing file or create new logging file.
        , ConfigLogStack            //!< Configuration property is information of maximum stack size to hold log data before streaming is initialized
        , ConfigLogEnable           //!< Configuration property is information whether logging is enabled or not
        , ConfigLogLayoutEnter      //!< Configuration property is information for layout to output enter scope message
        , ConfigLogLayoutMessage    //!< Configuration property is information for layout to output message
        , ConfigLogLayoutExit       //!< Configuration property is information for layout to output exit scope message
        , ConfigScope               //!< Configuration property is information of scopes to activate or deactivate

    } eLogConfig;

    /**
     * \brief   Converts and returns the string of NELogConfig::eLogConfig value
     **/
    inline const char * getString( NELogConfig::eLogConfig logConfig );

    /**
     * \brief   The number of supported log configuration property entries, used when read configuration file.
     **/
    constexpr int       LOG_PROPERTY_COUNT      { static_cast<int>(eLogConfig::ConfigScope) + 1 };

    /**
     * \brief   Returns the log configuration type parsed from syntax of passed command string.
     * \param   cmdSyntax   The string with command to parse.
     * \Return  Returns valid log configuration type if succeeded to parse the command string.
     *          Otherwise, returns ConfigUnknown value.
     * \see     ToString, NELogConfig::eLogConfig
     **/
    NELogConfig::eLogConfig convFromString( const char * cmdSyntax );
    
    /**
     * \brief   Converts the log configuration type to command string syntax.
     * \param   logConfig   The log config type to convert.
     * \return  Returns valid command string of specified config type or empty string
     *          if type is undefined
     * \see     FromString, NELogConfig::eLogConfig
     **/
    const std::string_view & convToString( NELogConfig::eLogConfig logConfig );

    /**
     * \brief   NELogConfig::eLogStatus
     *          Defines logging status
     **/
    typedef enum class E_LogStatus
    {
          StatusUndefined   //!< Logging status is undefined
        , StatusDisabled    //!< Logging status is disabled, no messages are output
        , StatusEnabled     //!< Logging status is enabled, the enabled scope can output message.
    } eLogStatus;

    /**
     * \brief   NELogConfig::eConfigKey
     *          The configuration key visibility levels
     **/
    typedef enum class E_ConfigKey
    {
          KeyUndefined  //!< Configuration key visibility is undefined.
        , KeyGlobal     //!< Configuration key visibility level is global and valid for all modules.
        , KeyLocal      //!< Configuration key visibility level is local and valid only for specified module.

    } eConfigKey;

    /**
     * \brief   NELogConfig::eLayouts
     *          The syntax of specifiers to parse and create layouts to output messages.
     * 
     * \example Formatting message out for scope:
     *          log.layout.enter    = %d: [ %c.%t  %x.%z: Enter --> ]%n
     * 
     *          In this example, the 'enter scope' makes output in following sequence and format:
     *          <timestamp / day-time>: [ <tick-count>.<thread-ID> <module-name>.<scope-name>: Enter ---> ] \n
     *          for example like this:
     *          2019-07-04 14:34:47,969: [ 1587.17524  distrbutedapp.distrbutedapp_ConnectionServicing_CreateComponent: Enter --> ]
     **/
    typedef enum class E_Layouts : char
    {
          LayoutUndefined       = 0     //!< Undefined specifier, no layout is created
        , LayoutAnyText         = 1     //!< Create any text layout to output message without formating

        , LayoutTickCount       = 'c'   //!< Create layout to output tick-count value since process start
        , LayoutDayTime         = 'd'   //!< Create layout to output day and time data
        , LayoutExecutableId    = 'e'   //!< Create layout to output module ID
        , LayoutMessage         = 'm'   //!< Create layout to output logging message
        , LayoutEndOfLine       = 'n'   //!< Create layout to output end-of-line character
        , LayoutPriority        = 'p'   //!< Create layout to output message priority
        , LaytoutScopeId        = 's'   //!< Create layout to output message scope ID
        , LayoutThreadId        = 't'   //!< Create layout to output thread ID
        , LayoutExecutableName  = 'x'   //!< Create layout to output module name
        , LayoutThreadName      = 'y'   //!< Create layout to output thread name
        , LaytoutScopeName      = 'z'   //!< Create layout to output scope name

    } eLayouts;

  
//////////////////////////////////////////////////////////////////////////
// Syntax to parse configuration file
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Symbol of comment in configuration file
     **/
    constexpr char              SYNTAX_COMMENT                      { '#' };

    /**
     * \brief   Symbol of equal operation in configuration file
     **/
    constexpr char              SYNTAX_EQUAL                        { '=' };

    /**
     * \brief   End of line symbol in configuration file
     **/
    constexpr char              SYNTAX_LINEEND                      { '\n' };

    /**
     * \brief   Symbol of object separator in configuration file
     **/
    constexpr char              SYNTAX_OBJECT_SEPARATOR             { '.' };

    /**
     * \brief   Symbol of data separator in configuration file
     **/
    constexpr char              SYNTAX_DATA_SEPARATOR               { ':' };

    /**
     * \brief   White space delimiter
     **/
    constexpr char              SYNTAX_WHITESPACE_DELIMITER         { ' ' };

    /**
     * \brief   Tab delimiter
     **/
    constexpr char              SYNTAX_TAB_DELIMITER                { '\t' };

    /**
     * \brief   End of command delimiter, optional. Might be skipped
     **/
    constexpr char              SYNTAX_END_COMMAND_DELIMITER        { ';' };

    /**
     * \brief   List of valid delimiters
     **/
    constexpr std::string_view  SYNTAX_DELIMITERS                   { " \t" };

    /**
     * \brief   Boolean syntax 'true'
     **/
    constexpr std::string_view  SYNTAX_TRUE                         { NECommon::BOOLEAN_TRUE };

    /**
     * \brief   Boolean syntax 'false'
     **/
    constexpr std::string_view  SYNTAX_FALSE                        { NECommon::BOOLEAN_FALSE };

    /**
     * \brief   The logical OR operation used setting scope priorities only
     **/
    constexpr std::string_view  SYNTAX_LOGICAL_OR                   { "|" };

//////////////////////////////////////////////////////////////////////////
// Syntax of commands in configuration file
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   The empty Syntax.
     **/
    constexpr std::string_view  SYNTAX_CMD_EMPTY                    { "" };
    /**
     * \brief   The logging general syntax. All logging related commands should start with this command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG                      { "log" };
    /**
     * \brief   The syntax of logging version command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_VERSION              { "log.version" };
    /**
     * \brief   The syntax of logging file name command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_FILE                 { "log.file" };
    /**
     * \breif   The syntax of remote logging service command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_REMOTE               { "log.remote" };
    /**
     * \breif   The syntax of remote logging service host name command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_REMOTE_HOST          { "log.remote.host" };
    /**
     * \breif   The syntax of remote logging service port number command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_REMOTE_PORT          { "log.remote.port" };
    /**
     * \breif   The syntax of database service command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB                   { "log.db" };
    /**
     * \breif   The syntax of database service driver name command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB_DRIVER            { "log.db.driver" };
    /**
     * \breif   The syntax of database service driver host command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB_HOST              { "log.db.host" };
    /**
     * \breif   The syntax of database service user name command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB_USER              { "log.db.user" };
    /**
     * \breif   The syntax of database service user password command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB_PASSWORD          { "log.db.pwd" };
    /**
     * \breif   The syntax of database name command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DB_NAME              { "log.db.name" };
    /**
     * \breif   The syntax of debug output command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_DEBUG                { "log.debug" };
    /**
     * \breif   The syntax of target (file or database) output command -- append in existing or create new.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_APPEND               { "log.append" };
    /**
     * \breif   The syntax of logging stack size command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_STACK                { "log.stack" };
    /**
     * \breif   The syntax of logging enabling command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_ENABLE               { "log.enable" };
    /**
     * \breif   The syntax of logging enter scope command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_LAYOUT_ENTER         { "log.layout.enter" };
    /**
     * \breif   The syntax of logging message command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_LAYOUT_MESSAGE       { "log.layout.message" };
    /**
     * \breif   The syntax of logging exit scope command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_LAYOUT_EXIT          { "log.layout.exit" };
    /**
     * \breif   The syntax of logging general scope command.
     **/
    constexpr std::string_view  SYNTAX_CMD_LOG_SCOPE                { "scope" };

    /**
     * \brief   The scope name path separator
     **/
    constexpr char              SYNTAX_SCOPE_SEPARATOR              { '_' };

    /**
     * \brief   Group of scopes in specified sub-path
     **/
    constexpr char              SYNTAX_SCOPE_GROUP                  { '*' };

    /**
     * \brief   The reserved name of invalid scope
     **/
    constexpr std::string_view  INVALID_SCOPE_NAME                  { "INVALID_SCOPE_NAME" };

    /**
     * \brief   Format of timestamp to display logs
     **/
    constexpr std::string_view  TIME_FORMAT_ISO8601                 { "ISO8601" };
    constexpr std::string_view  TIME_FORMAT_ISO8601_OUTPUT          { "%Y-%m-%d %H:%M:%S,%l" };

    /**
     * \brief   Absolute time format of timestamp
     **/
    constexpr std::string_view  TIME_FORMAT_ABSOLUTE                { "ABSOLUTE" };
    constexpr std::string_view  TIME_FORMAT_ABSOLUTE_OUTPUT         { "%H:%M:%S,%l" };

    /**
     * \brief   Format only data of timestamp
     **/
    constexpr std::string_view  TIME_FORMAT_DATE                    { "DATE" };
    constexpr std::string_view  TIME_FORMAT_DATE_OUTPUT             { "%d %b %Y %H:%M:%S,%l" };

    /**
     * \brief   Enter scope marker to display / log
     **/
    constexpr std::string_view  MARK_SCOPE_ENTER                    { "%s: Enter -->" };

    /**
     * \brief   Exit scope marker to display / log
     **/
    constexpr std::string_view  MARK_SCOPE_EXIT                     { "%s: Exit  <--" };

    /**
     * \brief   Log messages to display / log
     **/
    constexpr std::string_view  MARK_LOG_MESSAGE                    { ">>> " };

    /**
     * \brief   Predefined name of logger to log into file
     **/
    constexpr std::string_view  LOGGER_NAME_FILE                    { "FileLogger" };

    /**
     * \brief   Predefined name of logger to log to remote host
     **/
    constexpr std::string_view  LOGGER_NAME_REMOTE                  { "RemoteLogger" };
    /**
     * \brief   Predefined name of logger to log to database
     **/
    constexpr std::string_view  LOGGER_NAME_DATABASE                { "DatabaseLogger" };
    /**
     * \brief   Predefined name of logger to log to debug output window
     **/
    constexpr std::string_view  LOGGER_NAME_DEBUG                   { "DebugLogger" };

    /**
     * \brief   Default value of integer, if not set in configuration file
     **/
    constexpr int               DEFAULT_INTEGER_VALUE               { 0 };

    /**
     * \brief   Default value of boolean, if not set in configuration file
     **/
    constexpr bool              DEFAULT_BOOLEAN_VALUE               { false };

    /**
     * \brief   The default logging priority
     **/
    constexpr unsigned int      DEFAULT_LOG_PRIORITY                { static_cast<unsigned int>(NETrace::PrioNotset) };

    /**
     * \brief   Default value of string, if not set in configuration file
     **/
    constexpr std::string_view  DEFAULT_STRING_VALUE                { String::EmptyString };

    /**
     * \brief   Default delimiter
     **/
    constexpr char              DEFAULT_SYNTAX_DELIMITER            { NELogConfig::SYNTAX_WHITESPACE_DELIMITER };
    /**
     * \brief   Default time format
     **/
    constexpr std::string_view  DEFAULT_TIME_FORMAT                 { TIME_FORMAT_ISO8601 };
    /**
     * \brief   Default time format output
     **/
    constexpr std::string_view  DEFAULT_TIME_FORMAT_OUTPUT          { TIME_FORMAT_ISO8601_OUTPUT };

    /**
     * \brief   Default flag to indicate logging enable / disable status. By default it is disabled.
     **/
    constexpr bool              DEFAULT_LOG_ENABLED                 { false };

    /**
     * \brief   Default host name to connect logging service
     **/
    constexpr std::string_view  DEFAULT_REMOTE_HOST                 { NESocket::LocalHost };

    /**
     * \brief   Default port number to connect to logging service
     **/
    constexpr unsigned short    DEFAULT_REMOTE_PORT                 { 8082 };

    /**
     * \brief   Default log file name. Relative path to current process
     **/
    constexpr std::string_view  DEFAULT_LOG_FILE_NAME               { "./logs/trace_%time%.log" };

    /**
     * \brief   Symbol, indicating scope group, which start by name before '*' symbol
     **/
    constexpr std::string_view  MODULE_SCOPE                        { "*" };

    /**
     * \brief   The list of valid syntax lists.
     **/
    constexpr const std::string_view    ValidSytaxList[ ] = 
            {
                  NEString::EmptyStringA                        //!< eLogConfig::ConfigUnknown
                , NELogConfig::SYNTAX_CMD_LOG_VERSION           //!< eLogConfig::ConfigLogVersion
                , NELogConfig::SYNTAX_CMD_LOG_FILE              //!< eLogConfig::ConfigLogFile
                , NELogConfig::SYNTAX_CMD_LOG_REMOTE_HOST       //!< eLogConfig::ConfigLogRemoteHost
                , NELogConfig::SYNTAX_CMD_LOG_REMOTE_PORT       //!< eLogConfig::ConfigLogRemotePort
                , NELogConfig::SYNTAX_CMD_LOG_DB_DRIVER         //!< eLogConfig::ConfigLogDatabaseDriver
                , NELogConfig::SYNTAX_CMD_LOG_DB_HOST           //!< eLogConfig::ConfigLogDatabaseHost
                , NELogConfig::SYNTAX_CMD_LOG_DB_USER           //!< eLogConfig::ConfigLogDatabaseUser
                , NELogConfig::SYNTAX_CMD_LOG_DB_PASSWORD       //!< eLogConfig::ConfigLogDatabasePwd
                , NELogConfig::SYNTAX_CMD_LOG_DB_NAME           //!< eLogConfig::ConfigLogDatabaseName
                , NELogConfig::SYNTAX_CMD_LOG_DEBUG             //!< eLogConfig::ConfigLogDebug
                , NELogConfig::SYNTAX_CMD_LOG_APPEND            //!< eLogConfig::ConfigLogAppend
                , NELogConfig::SYNTAX_CMD_LOG_STACK             //!< eLogConfig::ConfigLogStack
                , NELogConfig::SYNTAX_CMD_LOG_ENABLE            //!< eLogConfig::ConfigLogEnable
                , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_ENTER      //!< eLogConfig::ConfigLogLayoutEnter
                , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_MESSAGE    //!< eLogConfig::ConfigLogLayoutMessage
                , NELogConfig::SYNTAX_CMD_LOG_LAYOUT_EXIT       //!< eLogConfig::ConfigLogLayoutExit
                , NELogConfig::SYNTAX_CMD_LOG_SCOPE             //!< eLogConfig::ConfigScope
            };

    //!< The default logging version
    constexpr std::string_view   DEFAULT_LOG_VERSION            { "log.version = 1.0.0" };

    //!< The default logging enabled flag.
    constexpr std::string_view   DEFAULT_LOG_ENABLE             { "log.enable = true" };

    //!< The default file name of loggs.
    constexpr std::string_view   DEFAULT_LOG_FILE               { "log.file = ./logs/trace_%time%.log" };

    //!< The default flag, indicating whether logs are enabled.
    constexpr std::string_view   DEFAULT_LOG_APPEND             { "log.append = false" };

    //!< Logging default layout format of logging scope activation. /*%d: [ %c.%t  %x.%z: Enter --> ]%n*/
    constexpr std::string_view   DEFAULT_LOG_LAYOUT_ENTER       { "log.layout.enter = %d: [ %t  %x.%z: Enter --> ]%n" };

    //!< Logging default layout format for logging messages.        /*d: [ %c.%t  %p >>> ] %m%n*/
    constexpr std::string_view   DEFAULT_LOG_LAYOUT_MESSAGE     { "log.layout.message = %d: [ %t  %p >>> ] %m%n" };

    //!< Logging default layout format of logging scope deactivation./*%d: [ %c.%t  %x.%z: Exit <-- ]%n*/
    constexpr std::string_view   DEFAULT_LOG_LAYOUT_EXIT        { "log.layout.exit = %d: [ %t  %x.%z: Exit <-- ]%n" };

    //!< Default flag enabling output in debugging window or console.
    constexpr std::string_view   DEFAULT_LOG_LAYOUT_DEBUG       { "log.debug = false" };
}

//////////////////////////////////////////////////////////////////////////
// NELogConfig namespace inline method implementation
//////////////////////////////////////////////////////////////////////////
inline const char * NELogConfig::getString( NELogConfig::eLogConfig logConfig )
{
    switch ( logConfig )
    {
    case NELogConfig::eLogConfig::ConfigUnknown:
        return "NELogConfig::ConfigUnknown";
    case NELogConfig::eLogConfig::ConfigLogVersion:
        return "NELogConfig::ConfigLogVersion";
    case NELogConfig::eLogConfig::ConfigLogFile:
        return "NELogConfig::ConfigLogFile";
    case NELogConfig::eLogConfig::ConfigLogRemoteHost:
        return "NELogConfig::ConfigLogRemoteHost";
    case NELogConfig::eLogConfig::ConfigLogRemotePort:
        return "NELogConfig::ConfigLogRemotePort";
    case NELogConfig::eLogConfig::ConfigLogDatabaseDriver:
        return "NELogConfig::ConfigLogDatabaseDriver";
    case NELogConfig::eLogConfig::ConfigLogDatabaseHost:
        return "NELogConfig::ConfigLogDatabaseHost";
    case NELogConfig::eLogConfig::ConfigLogDatabaseUser:
        return "NELogConfig::ConfigLogDatabaseUser";
    case NELogConfig::eLogConfig::ConfigLogDatabasePwd:
        return "NELogConfig::ConfigLogDatabasePwd";
    case NELogConfig::eLogConfig::ConfigLogDatabaseName:
        return "NELogConfig::ConfigLogDatabaseName";
    case NELogConfig::eLogConfig::ConfigLogDebug:
        return "NELogConfig::ConfigLogDebug";
    case NELogConfig::eLogConfig::ConfigLogAppend:
        return "NELogConfig::ConfigLogNew";
    case NELogConfig::eLogConfig::ConfigLogStack:
        return "NELogConfig::ConfigLogStack";
    case NELogConfig::eLogConfig::ConfigLogEnable:
        return "NELogConfig::ConfigLogEnable";
    case NELogConfig::eLogConfig::ConfigLogLayoutEnter:
        return "NELogConfig::ConfigLogLayoutEnter";
    case NELogConfig::eLogConfig::ConfigLogLayoutMessage:
        return "NELogConfig::ConfigLogLayoutMessage";
    case NELogConfig::eLogConfig::ConfigLogLayoutExit:
        return "NELogConfig::ConfigLogLayoutExit";
    case NELogConfig::eLogConfig::ConfigScope:
        return "NELogConfig::ConfigScope";
    default:
        ASSERT(false);
    return "ERR: Invalid NELogConfig::eLogConfig value!";
    }
}
