#ifndef AREG_TRACE_PRIVATE_NELOGCONFIG_HPP
#define AREG_TRACE_PRIVATE_NELOGCONFIG_HPP
/************************************************************************
 * \file        areg/trace/private/NELogConfig.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    typedef enum E_LogConfig
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

        , ConfigLast                //!< The size of configuration data

    } eLogConfig;

    /**
     * \brief   Converts and returns the string of NELogConfig::eLogConfig value
     **/
    inline const char * getString( NELogConfig::eLogConfig logConfig );

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
    const char * convToString( NELogConfig::eLogConfig logConfig );

    /**
     * \brief   NELogConfig::eLogStatus
     *          Defines logging status
     **/
    typedef enum E_LogStatus
    {
          StatusUndefined   //!< Logging status is undefined
        , StatusDisabled    //!< Logging status is disabled, no messages are output
        , StatusEnabled     //!< Logging status is enabled, the enabled scope can output message.
    } eLogStatus;

    /**
     * \brief   NELogConfig::eConfigKey
     *          The configuration key visibility levels
     **/
    typedef enum E_ConfigKey
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
    typedef enum E_Layouts
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
    const char              SYNTAX_COMMENT                      = '#';

    /**
     * \brief   Symbol of equal operation in configuration file
     **/
    const char              SYNTAX_EQUAL                        = '=';

    /**
     * \brief   End of line symbol in configuration file
     **/
    const char              SYNTAX_LINEEND                      = '\n';

    /**
     * \brief   Symbol of object separator in configuration file
     **/
    const char              SYNTAX_OBJECT_SEPARATOR             = '.';

    /**
     * \brief   Symbol of data separator in configuration file
     **/
    const char              SYNTAX_DATA_SEPARATOR               = ':';

    /**
     * \brief   White space delimiter
     **/
    const char              SYNTAX_WHITESPACE_DELIMITER         = ' ';

    /**
     * \brief   Tab delimiter
     **/
    const char              SYNTAX_TAB_DELIMITER                = '\t';

    /**
     * \brief   End of command delimiter, optional. Might be skipped
     **/
    const char              SYNTAX_END_COMMAND_DELIMITER        = ';';

    /**
     * \brief   List of valid delimiters
     **/
    const char * const      SYNTAX_DELIMITERS                   = " \t";

    /**
     * \brief   Boolean syntax 'true'
     **/
    const char * const      SYNTAX_TRUE                         = String::BOOLEAN_TRUE;

    /**
     * \brief   Boolean syntax 'false'
     **/
    const char * const      SYNTAX_FALSE                        = String::BOOLEAN_FALSE;

    /**
     * \brief   The logical OR operation used setting scope priorities only
     **/
    const char * const      SYNTAX_LOGICAL_OR                   = "|";

    /**
     * \brief   Syntax of current directory (used in file path) in UNIX system
     **/
    const char * const      SYNTAX_CURRENT_DIR_UNIX             = "./";

    /**
     * \brief   Syntax of current directory (used in file path) in DOS system
     **/
    const char * const      SYNTAX_CURRENT_DIR_DOS              = ".\\";

//////////////////////////////////////////////////////////////////////////
// Syntax of commands in configuration file
//////////////////////////////////////////////////////////////////////////

    const char * const      SYNTAX_CMD_EMPTY                    = "";               //!< The empty Syntax.
    extern const int        SYNTAX_CMD_EMPTY_LENGTH                 ;               //!< The length of empty syntax.

    const char * const      SYNTAX_CMD_LOG                      = "log";            //!< The logging general syntax. All logging related commands should start with this command.
    extern const int        SYNTAX_CMD_LOG_LENGTH                   ;               //!< The length of log syntax.

    const char * const      SYNTAX_CMD_LOG_VERSION              = "log.version";    //!< The syntax of logging version command.
    extern const int        SYNTAX_CMD_LOG_VERSION_LENGTH           ;               //!< The length of logging version syntax.

    const char * const      SYNTAX_CMD_LOG_FILE                 = "log.file";       //!< The syntax of logging file name command.
    extern const int        SYNTAX_CMD_LOG_FILE_LENGTH              ;               //!< The length of logging file name syntax.

    const char * const      SYNTAX_CMD_LOG_REMOTE               = "log.remote";     //!< The syntax of remote logging service command.
    extern const int        SYNTAX_CMD_LOG_REMOTE_LENGTH            ;               //!< The length of remote logging service syntax.

    const char * const      SYNTAX_CMD_LOG_REMOTE_HOST          = "log.remote.host";//!< The syntax of remote logging service host name command.
    extern const int        SYNTAX_CMD_LOG_REMOTE_HOST_LENGTH       ;               //!< The length of remote logging service host name syntax.

    const char * const      SYNTAX_CMD_LOG_REMOTE_PORT          = "log.remote.port";//!< The syntax of remote logging service port number command.
    extern const int        SYNTAX_CMD_LOG_REMOTE_PORT_LENGTH       ;               //!< The length of remote logging service port number syntax.

    const char * const      SYNTAX_CMD_LOG_DB                   = "log.db";         //!< The syntax of database service command.
    extern const int        SYNTAX_CMD_LOG_DB_LENGTH                ;               //!< The length of database service syntax.

    const char * const      SYNTAX_CMD_LOG_DB_DRIVER            = "log.db.driver";  //!< The syntax of database service driver name command.
    extern const int        SYNTAX_CMD_LOG_DB_DRIVER_LENGTH         ;               //!< The length of database service driver name syntax.

    const char * const      SYNTAX_CMD_LOG_DB_HOST              = "log.db.host";    //!< The syntax of database service driver host command.
    extern const int        SYNTAX_CMD_LOG_DB_HOST_LENGTH           ;               //!< The length of database service driver host syntax.

    const char * const      SYNTAX_CMD_LOG_DB_USER              = "log.db.user";    //!< The syntax of database service user name command.
    extern const int        SYNTAX_CMD_LOG_DB_USER_LENGTH           ;               //!< The length of database service user name syntax.

    const char * const      SYNTAX_CMD_LOG_DB_PASSWORD          = "log.db.pwd";     //!< The syntax of database service user password command.
    extern const int        SYNTAX_CMD_LOG_DB_PASSWORD_LENGTH       ;               //!< The length of database service user password syntax.

    const char * const      SYNTAX_CMD_LOG_DB_NAME              = "log.db.name";    //!< The syntax of database name command.
    extern const int        SYNTAX_CMD_LOG_DB_NAME_LENGTH           ;               //!< The length of database name syntax.

    const char * const      SYNTAX_CMD_LOG_DEBUG                = "log.debug";      //!< The syntax of debug output command.
    extern const int        SYNTAX_CMD_LOG_DEBUG_LENGTH             ;               //!< The length of debug output syntax.

    const char * const      SYNTAX_CMD_LOG_APPEND               = "log.append";     //!< The syntax of target (file or database) output command -- append in existing or create new.
    extern const int        SYNTAX_CMD_LOG_APPEND_LENGTH            ;               //!< The length of target output syntax.

    const char * const      SYNTAX_CMD_LOG_STACK                = "log.stack";      //!< The syntax of logging stack size command.
    extern const int        SYNTAX_CMD_LOG_STACK_LENGTH             ;               //!< The length of logging stack size syntax.

    const char * const      SYNTAX_CMD_LOG_ENABLE               = "log.enable";     //!< The syntax of logging enabling command.
    extern const int        SYNTAX_CMD_LOG_ENABLE_LENGTH            ;               //!< The length of logging enabling syntax.

    const char * const      SYNTAX_CMD_LOG_LAYOUT_ENTER         = "log.layout.enter";   //!< The syntax of logging enter scope command.
    extern const int        SYNTAX_CMD_LOG_LAYOUT_ENTER_LENGTH      ;                   //!< The length of logging enter scope syntax.

    const char * const      SYNTAX_CMD_LOG_LAYOUT_MESSAGE       = "log.layout.message"; //!< The syntax of logging message command.
    extern const int        SYNTAX_CMD_LOG_LAYOUT_MESSAGE_LENGTH    ;                   //!< The length of logging message syntax.

    const char * const      SYNTAX_CMD_LOG_LAYOUT_EXIT          = "log.layout.exit";    //!< The syntax of logging exit scope command.
    extern const int        SYNTAX_CMD_LOG_LAYOUT_EXIT_LENGTH       ;                   //!< The length of logging exit scope syntax

    const char * const      SYNTAX_CMD_LOG_SCOPE                = "scope";              //!< The syntax of logging general scope command.
    extern const int        SYNTAX_CMD_LOG_SCOPE_LENGTH             ;                   //!< The length of logging general scope syntax.

    /**
     * \brief   The scope name path separator
     **/
    const char              SYNTAX_SCOPE_SEPARATOR              = '_';

    /**
     * \brief   Group of scopes in specified sub-path
     **/
    const char              SYNTAX_SCOPE_GROUP                  = '*';

    /**
     * \brief   The reserved name of invalid scope
     **/
    const char * const      INVALID_SCOPE_NAME                  = "INVALID_SCOPE_NAME";

    /**
     * \brief   Use timestamp in name (file name, database name)
     **/
    extern const char * const      LOG_FILENAME_MASK_TIMESTAMP  /*= File::FILE_MASK_TIMESTAMP*/;  // %time%

    /**
     * \brief   Log in special folder for application data
     **/
    extern const char * const      LOG_FILE_NAME_MASK_APPDATA   /*= File::FILE_MASK_APP_DATA*/;   // %appdata%

    /**
     * \brief   The timestamp format to output when generate name
     **/
    extern const char * const      LOG_FORMAT_TIMESTAMP         /*= File::TIMESTAMP_FORMAT*/;     // "%04d_%02d_%02d_%02d_%02d_%02d_%03d" ==> yyyy_mm_dd_hh_mm_ss_ms

    /**
     * \brief   Format of timestamp to display logs
     **/
    const char* const       TIME_FORMAT_ISO8601                 = "ISO8601";
    const char* const       TIME_FORMAT_ISO8601_OUTPUT          = "%Y-%m-%d %H:%M:%S,%l";

    /**
     * \brief   Absolute time format of timestamp
     **/
    const char* const       TIME_FORMAT_ABSOLUTE                = "ABSOLUTE";
    const char* const       TIME_FORMAT_ABSOLUTE_OUTPUT         = "%H:%M:%S,%l";

    /**
     * \brief   Format only data of timestamp
     **/
    const char * const      TIME_FORMAT_DATE                    = "DATE";
    const char * const      TIME_FORMAT_DATE_OUTPUT             = "%d %b %Y %H:%M:%S,%l";

    /**
     * \brief   Enter scope marker to display / log
     **/
    const char * const      MARK_SCOPE_ENTER                    = "%s: Enter -->";

    /**
     * \brief   Exit scope marker to display / log
     **/
    const char * const      MARK_SCOPE_EXIT                     = "%s: Exit  <--";

    /**
     * \brief   Log messages to display / log
     **/
    const char * const      MARK_LOG_MESSAGE                    = ">>> ";

    /**
     * \brief   Predefined name of logger to log into file
     **/
    const char * const      LOGGER_NAME_FILE                    = "FileLogger";

    /**
     * \brief   Predefined name of logger to log to remote host
     **/
    const char * const      LOGGER_NAME_REMOTE                  = "RemoteLogger";
    /**
     * \brief   Predefined name of logger to log to database
     **/
    const char * const      LOGGER_NAME_DATABASE                = "DatabaseLogger";
    /**
     * \brief   Predefined name of logger to log to debug output window
     **/
    const char * const      LOGGER_NAME_DEBUG                   = "DebugLogger";

    /**
     * \brief   Default value of integer, if not set in configuration file
     **/
    const int               DEFAULT_INTEGER_VALUE               = 0;

    /**
     * \brief   Default value of boolean, if not set in configuration file
     **/
    const bool              DEFAULT_BOOLEAN_VALUE               = false;

    /**
     * \brief   The default logging priority
     **/
    const unsigned int      DEFAULT_LOG_PRIORITY                = static_cast<unsigned int>(NETrace::PrioNotset);

    /**
     * \brief   Default value of string, if not set in configuration file
     **/
    const char * const      DEFAULT_STRING_VALUE                = String::EmptyString;

    /**
     * \brief   Default delimiter
     **/
    const char              DEFAULT_SYNTAX_DELIMITER            = NELogConfig::SYNTAX_WHITESPACE_DELIMITER;
    /**
     * \brief   Default time format
     **/
    const char * const      DEFAULT_TIME_FORMAT                 = TIME_FORMAT_ISO8601;
    /**
     * \brief   Default time format output
     **/
    const char * const      DEFAULT_TIME_FORMAT_OUTPUT          = TIME_FORMAT_ISO8601_OUTPUT;

    /**
     * \brief   Default log initialization file name. Relative path to current process
     **/
    const char * const      DEFAULT_CONFIG_FILE_NAME            = NEApplication::DEFAULT_TRACING_CONFIG_FILE;

    /**
     * \brief   Default flag to indicate logging enable / disable status. By default it is disabled.
     **/
    const bool              DEFAULT_LOG_ENABLED                 = false;

    /**
     * \brief   Default host name to connect logging service
     **/
    const char * const      DEFAULT_REMOTE_HOST                 = "localhost";

    /**
     * \brief   Default port number to connect to logging service
     **/
    const unsigned short    DEFAULT_REMOTE_PORT                 = 8082;

    /**
     * \brief   Default log file name. Relative path to current process
     **/
    const char * const      DEFAULT_LOG_FILE_NAME               = "./logs/trace_%time%.log";

    /**
     * \brief   Symbol, indicating scope group, which start by name before '*' symbol
     **/
    const char * const      MODULE_SCOPE                        = "*";

    /**
     * \brief   NELogConfig::sConfigSytax
     *          The structure of log configuration syntax.
     **/
    typedef struct S_ConfigSytax
    {
        const char *    sytaxName;      //!< The syntax / or syntax name
        const int       syntaxLength;   //!< The length of syntax
    } sConfigSytax;

    /**
     * \brief   The list of valid syntax lists.
     **/
    extern const NELogConfig::sConfigSytax     ValidSytaxList[NELogConfig::ConfigLast];

}

//////////////////////////////////////////////////////////////////////////
// NELogConfig namespace inline method implementation
//////////////////////////////////////////////////////////////////////////
inline const char * NELogConfig::getString( NELogConfig::eLogConfig logConfig )
{
    switch ( logConfig )
    {
    case NELogConfig::ConfigUnknown:
        return "NELogConfig::ConfigUnknown";
    case NELogConfig::ConfigLogVersion:
        return "NELogConfig::ConfigLogVersion";
    case NELogConfig::ConfigLogFile:
        return "NELogConfig::ConfigLogFile";
    case NELogConfig::ConfigLogRemoteHost:
        return "NELogConfig::ConfigLogRemoteHost";
    case NELogConfig::ConfigLogRemotePort:
        return "NELogConfig::ConfigLogRemotePort";
    case NELogConfig::ConfigLogDatabaseDriver:
        return "NELogConfig::ConfigLogDatabaseDriver";
    case NELogConfig::ConfigLogDatabaseHost:
        return "NELogConfig::ConfigLogDatabaseHost";
    case NELogConfig::ConfigLogDatabaseUser:
        return "NELogConfig::ConfigLogDatabaseUser";
    case NELogConfig::ConfigLogDatabasePwd:
        return "NELogConfig::ConfigLogDatabasePwd";
    case NELogConfig::ConfigLogDatabaseName:
        return "NELogConfig::ConfigLogDatabaseName";
    case NELogConfig::ConfigLogDebug:
        return "NELogConfig::ConfigLogDebug";
    case NELogConfig::ConfigLogAppend:
        return "NELogConfig::ConfigLogNew";
    case NELogConfig::ConfigLogStack:
        return "NELogConfig::ConfigLogStack";
    case NELogConfig::ConfigLogEnable:
        return "NELogConfig::ConfigLogEnable";
    case NELogConfig::ConfigLogLayoutEnter:
        return "NELogConfig::ConfigLogLayoutEnter";
    case NELogConfig::ConfigLogLayoutMessage:
        return "NELogConfig::ConfigLogLayoutMessage";
    case NELogConfig::ConfigLogLayoutExit:
        return "NELogConfig::ConfigLogLayoutExit";
    case NELogConfig::ConfigScope:
        return "NELogConfig::ConfigScope";
    default:
        ASSERT(false);
    return "ERR: Invalid NELogConfig::eLogConfig value!";
    }
}

#endif  // AREG_TRACE_PRIVATE_NELOGCONFIG_HPP
