#ifndef AREG_BASE_PRIVATE_NEDEBUG_HPP
#define AREG_BASE_PRIVATE_NEDEBUG_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEDebug.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Debugging utilities
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
     * \brief   NEDebug::eDegubPrio
     *          Defines message priority in debug output window.
     *          There is no filter of messages in priority, but
     *          by using these constants, user makes appropriate 
     *          message output prefix, showing importance. 
     **/
    typedef enum class E_DegubPrio : int
    {
          PrioNone  = OUTPUT_DEBUG_LEVEL_NONE   //!< No priority, output message should be ignored.
        , PrioErr   = OUTPUT_DEBUG_LEVEL_ERROR  //!< Priority Error, outputs "ERR:" string in front of message
        , PrioWarn  = OUTPUT_DEBUG_LEVEL_WARN   //!< Priority Warning, outputs "WRN:" string in front of message
        , PrioInfo  = OUTPUT_DEBUG_LEVEL_INFO   //!< Priority Information, outputs "INF:" string in front of message
        , PrioDbg   = OUTPUT_DEBUG_LEVEL_DEBUG  //!< Priority Debug, outputs "DBG:" string in front of message

    } eDegubPrio;

    /**
     * \brief   NEDebug::MAX_DEBUG_BUFFER_SIZE
     *          The maximum buffer size for message output.
     **/
    constexpr unsigned int  MAX_DEBUG_BUFFER_SIZE       = 1024;

    constexpr std::string_view PREFIX_DBG_PRIORITIES [] =
    {
          ""        //!< No priority.
        , "ERR: "   //!< Priority Error message.
        , "WRN: "   //!< Priority Warning message.
        , "INF: "   //!< Priority Information message.
        , "DBG: "   //!< Priority Debug message.
    };

    /**
     * \brief   NEDebug::getPrioPrefix()
     *          Returns prefix string of specified priority
     * \param   priority    The priority of message
     * \return  Returns one of defined prefixes, depending of priority.
     *          By default, the priority is  NEDebug::PrioDbg
     **/
    inline const char * getPrioPrefix( NEDebug::eDegubPrio priority );

    /**
     * \brief   NEDebug::outputConsole()
     *          Makes message output in Debug Output Window
     * \param   priority    The priority of message
     * \param   msg         The message to output. Before output, the message is formated.
     **/
    void AREG_API outputConsole(NEDebug::eDegubPrio priority, const char* msg, ...);

    /**
     * \brief   NEDebug::outputConsole()
     *          Makes message output in Debug Output Window without changing string.
     *          No priority level or new line special characters will be added.
     * \param   msg         The message to output. Before output, the message is formated.
     **/
    void AREG_API outputConsole(const char* msg, ...);

#ifdef  _WIN32
    /**
     * \brief   Extracts call stack information from passed exception pointer and dumps in
     *          specified vector object. On output, the vector object will contain
     *          call stack message where the exception was raised.
     *          The function extracts call stack only in debug version.
     * \param   ep              The pointer to exception structure, which contains pointer to CONTEXT
     *                          object to extract call stack.
     * \param   out_callStack   On output this will contain the message of call stack.
     **/

    void AREG_API dumpExceptionCallStack( struct _EXCEPTION_POINTERS *ep, std::list<std::string> & OUT out_callStack);
#endif  // _WIN32

    /**
     * \brief   Operating System specific call to output message.
     *          In Windows this may output on VS Output Window.
     *          In other OS this may output on console.
     *          If message is empty, nothing is output.
     *          Method is valid only for DEBUG build.
     * \param   msg     The message to output.
     **/
    void AREG_API outputMessageOS( const char * msg );
}

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace inline function implementation.
//////////////////////////////////////////////////////////////////////////
inline const char* NEDebug::getPrioPrefix( NEDebug::eDegubPrio priority )
{
    return NEDebug::PREFIX_DBG_PRIORITIES[ static_cast<int>(priority) ].data();
}

#endif  // AREG_BASE_PRIVATE_NEDEBUG_HPP
