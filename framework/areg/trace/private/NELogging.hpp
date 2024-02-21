#ifndef AREG_TRACE_PRIVATE_NELOGGING_HPP
#define AREG_TRACE_PRIVATE_NELOGGING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NELogging.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The namespace with helper constants
 *              of trace log configuration syntax and outputs
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/trace/NETrace.hpp"

#include <string_view>

 //////////////////////////////////////////////////////////////////////////
 // NELogging namespace declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief  NELogging namespace contains helper constants and syntax of configuration file.
  *         In configuration file there are such information like
  *         log enable / disable, scope (group of scopes) defines, 
  *         log target (file, remote host), comments, etc.
  **/
namespace NELogging
{
    /**
     * \brief   NELogging::eLogStatus
     *          Defines logging status
     **/
    typedef enum class E_LogStatus
    {
          StatusUndefined   //!< Logging status is undefined
        , StatusDisabled    //!< Logging status is disabled, no messages are output
        , StatusEnabled     //!< Logging status is enabled, the enabled scope can output message.
    } eLogStatus;

    /**
     * \brief   NELogging::eConfigKey
     *          The configuration key visibility levels
     **/
    typedef enum class E_ConfigKey
    {
          KeyUndefined  //!< Configuration key visibility is undefined.
        , KeyGlobal     //!< Configuration key visibility level is global and valid for all modules.
        , KeyLocal      //!< Configuration key visibility level is local and valid only for specified module.

    } eConfigKey;

    /**
     * \brief   NELogging::eLayouts
     *          The syntax of specifiers to parse and create layouts to output messages.
     * 
     * \example Formatting message out for scope:
     *          log.layout.enter    = %d: [ %c.%t  %x.%z: Enter --> ]%n
     * 
     *          In this example, the 'enter scope' makes output in following sequence and format:
     *          <time-stamp / day-time>: [ <tick-count>.<thread-ID> <module-name>.<scope-name>: Enter ---> ] \n
     *          for example like this:
     *          2019-07-04 14:34:47,969: [ 1587.17524  distrbutedapp.distrbutedapp_ConnectionServicing_CreateComponent: Enter --> ]
     **/
    typedef enum class E_Layouts : char
    {
          LayoutUndefined       = 0     //!< Undefined specifier, no layout is created
        , LayoutAnyText         = 1     //!< Create any text layout to output message without formating

        , LayoutCookieId        = 'a'   //!< Create layout to output the cookie id of the log message source module
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
     * \brief   The special character meaning formating
     **/
    constexpr char              SYNTAX_SPECIAL_FORMAT               { '%' };

//////////////////////////////////////////////////////////////////////////
// Syntax of commands in configuration file
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   The empty Syntax.
     **/
    constexpr std::string_view  SYNTAX_CMD_EMPTY                    { "" };

    /**
     * \brief   The scope name path separator
     **/
    constexpr char              SYNTAX_SCOPE_SEPARATOR              { '_' };

    /**
     * \brief   Group of scopes in specified sub-path
     **/
    constexpr char              SYNTAX_SCOPE_GROUP                  { '*' };

    /**
     * \brief   Symbol, indicating scope group or all scopes.
     *          For example, the scope "scope.*" means all scope.
     *          And the scope "scope.areg_*" means all scopes of that start with "areg_"
     **/
    constexpr std::string_view  LOG_SCOPES_GRPOUP                   { "*" };

    /**
     * \brief   The reserved name of invalid scope
     **/
    constexpr std::string_view  INVALID_SCOPE_NAME                  { "INVALID_SCOPE_NAME" };

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
     * \brief   The default scopes to activate or deactivate
     **/
    constexpr std::pair<std::string_view, unsigned int>  DEFAULT_LOG_ENABLED_SCOPES[]
    {
          { {"*"}       , static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset) }
    };
}

//////////////////////////////////////////////////////////////////////////
// NELogging namespace inline method implementation
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_TRACE_PRIVATE_NELOGGING_HPP
