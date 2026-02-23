#ifndef AREG_LOGGING_PRIVATE_LOGOPTIONS_HPP
#define AREG_LOGGING_PRIVATE_LOGOPTIONS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogOptions.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The namespace with helper constants
 *              to configure and set options.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <string_view>

 //////////////////////////////////////////////////////////////////////////
 // NELogging namespace declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief  NELogOptions namespace contains helper constants and syntax of configuration file.
  *         In configuration file there are such information like
  *         log enable / disable, scope (group of scopes) defines, 
  *         log target (file, remote host), comments, etc.
  **/
namespace areg
{
    /**
     * \brief   areg::LayoutToken
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
    enum class LayoutToken  : char
    {
          Undefined     = 0     //!< Undefined specifier, no layout is created
        , AnyText       = 1     //!< Create any text layout to output message without formating

        , CookieId      = 'a'   //!< Create layout to output the cookie id of the log message source module
        , TickCount     = 'c'   //!< Create layout to output tick-count value since process start
        , DayTime       = 'd'   //!< Create layout to output day and time data
        , ExecutableId  = 'e'   //!< Create layout to output module ID
        , Message       = 'm'   //!< Create layout to output logging message
        , EndOfLine     = 'n'   //!< Create layout to output end-of-line character
        , Priority      = 'p'   //!< Create layout to output message priority
        , ScopeId       = 's'   //!< Create layout to output message scope ID
        , ThreadId      = 't'   //!< Create layout to output thread ID
        , ExecutableName= 'x'   //!< Create layout to output module name
        , ThreadName    = 'y'   //!< Create layout to output thread name
        , ScopeName     = 'z'   //!< Create layout to output scope name
    };

  
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
     * \brief   Default value of integer, if not set in configuration file
     **/
    constexpr int32_t               DEFAULT_INTEGER_VALUE               { 0 };

    /**
     * \brief   Default value of boolean, if not set in configuration file
     **/
    constexpr bool              DEFAULT_BOOLEAN_VALUE               { false };

    /**
     * \brief   The default logging priority
     **/
    constexpr uint32_t      DEFAULT_LOG_PRIORITY                { static_cast<uint32_t>(areg::LogPriority::PrioNotset) };

    /**
     * \brief   The default scopes to activate or deactivate
     **/
    constexpr std::pair<std::string_view, uint32_t>  DEFAULT_LOG_ENABLED_SCOPES[]
    {
          { {"*"}       , static_cast<uint32_t>(areg::LogPriority::PrioNotset) }
    };
}

//////////////////////////////////////////////////////////////////////////
// NELogging namespace inline method implementation
//////////////////////////////////////////////////////////////////////////

#endif  // AREG_LOGGING_PRIVATE_LOGOPTIONS_HPP
