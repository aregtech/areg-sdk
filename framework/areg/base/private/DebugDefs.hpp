#ifndef AREG_BASE_PRIVATE_DEBUGDEFS_HPP
#define AREG_BASE_PRIVATE_DEBUGDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DebugDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debugging utilities
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include <list>
#include <string>
#include <string_view>

/**
 * \brief   Debugging utilities.
 *          Contains some functions and constants available only in 
 *          Debug version. Normally, used to make outputs in
 *          Debug Output Window
 **/

/************************************************************************/
// Make visible only in Debug version
/************************************************************************/

struct _EXCEPTION_POINTERS;

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace declaration
//////////////////////////////////////////////////////////////////////////
namespace NEDebug
{
    /**
     * \brief   NEDebug::DebugPriority
     *          Defines message priority in debug output window.
     *          There is no filter of messages in priority, but
     *          by using these constants, user makes appropriate 
     *          message output prefix, showing importance. 
     **/
    enum class DebugPriority : int32_t
    {
          PrioNone  = OUTPUT_DEBUG_LEVEL_NONE   //!< No priority, output message should be ignored.
        , PrioErr   = OUTPUT_DEBUG_LEVEL_ERROR  //!< Priority Error, outputs "ERR:" string in front of message
        , PrioWarn  = OUTPUT_DEBUG_LEVEL_WARN   //!< Priority Warning, outputs "WRN:" string in front of message
        , PrioInfo  = OUTPUT_DEBUG_LEVEL_INFO   //!< Priority Information, outputs "INF:" string in front of message
        , PrioDbg   = OUTPUT_DEBUG_LEVEL_DEBUG  //!< Priority Debug, outputs "DBG:" string in front of message

    };

    /**
     * \brief   NEDebug::MAX_DEBUG_BUFFER_SIZE
     *          The maximum buffer size for message output.
     **/
    constexpr uint32_t  MAX_DEBUG_BUFFER_SIZE       = 1024;

    constexpr std::string_view PREFIX_DBG_PRIORITIES [] =
    {
          ""        //!< No priority.
        , "ERR: "   //!< Priority Error message.
        , "WRN: "   //!< Priority Warning message.
        , "INF: "   //!< Priority Information message.
        , "DBG: "   //!< Priority Debug message.
    };

    /**
     * \brief   Returns the string prefix corresponding to the specified priority level.
     *
     * \param   priority    The priority of message.
     * \return  Returns the priority prefix string.
     **/
    inline constexpr std::string_view prio_prefix( NEDebug::DebugPriority priority );

    /**
     * \brief   Outputs a formatted message to the debug output window with the specified priority
     *          level.
     *
     * \param   priority    The priority of message.
     * \param   msg         The message format string.
     **/
    void AREG_API output_console(NEDebug::DebugPriority priority, const char* msg, ...);

    /**
     * \brief   Outputs a formatted message to the debug output window without adding priority or
     *          newline characters.
     *
     * \param   msg     The message format string.
     **/
    void AREG_API output_console(const char* msg, ...);

#ifdef  _WIN32
    /**
     * \brief   Extracts and dumps exception call stack information into the output list. Available
     *          in debug builds only.
     *
     * \param   ep              Pointer to the exception structure containing the CONTEXT object.
     * \param[out] callStack       List that receives the call stack messages.
     **/

    void AREG_API dump_call_stack( struct _EXCEPTION_POINTERS *ep, std::list<std::string> & callStack);
#endif  // _WIN32

    /**
     * \brief   Outputs a message via the operating system (VS Output Window on Windows, console on
     *          other platforms). Valid only for DEBUG builds.
     *
     * \param   msg     The message to output.
     **/
    void AREG_API output_message_os( const char * msg );
}

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace inline function implementation.
//////////////////////////////////////////////////////////////////////////
inline constexpr std::string_view NEDebug::prio_prefix( NEDebug::DebugPriority priority )
{
    return NEDebug::PREFIX_DBG_PRIORITIES[ static_cast<int32_t>(priority) ];
}

#endif  // AREG_BASE_PRIVATE_DEBUGDEFS_HPP
