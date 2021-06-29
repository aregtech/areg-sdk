#ifndef AREG_BASE_PRIVATE_NEDEBUG_HPP
#define AREG_BASE_PRIVATE_NEDEBUG_HPP
/************************************************************************
 * \file        areg/base/private/NEDebug.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Debugging utilities
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief   Debugging utilities.
 *          Contains some functions and constants available only in 
 *          Debug version. Normally, used to make outputs in
 *          Debug Output Window
 **/

/************************************************************************/
// Make visible only in Debug version
/************************************************************************/


class StringList;
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
    typedef enum E_DegubPrio
    {
          PRIO_NONE = OUTPUT_DEBUG_LEVEL_NONE   //!< No priority, output message shoud be ignored.
        , PRIO_DBG  = OUTPUT_DEBUG_LEVEL_DEBUG  //!< Priority Debug, outputs "DBG:" string in front of message
        , PRIO_INFO = OUTPUT_DEBUG_LEVEL_INFO   //!< Priority Information, outputs "INF:" string in front of message
        , PRIO_WARN = OUTPUT_DEBUG_LEVEL_WARN   //!< Priority Warning, outputs "WRN:" string in front of message
        , PRIO_ERR  = OUTPUT_DEBUG_LEVEL_ERROR  //!< Priority Error, outputs "ERR:" string in front of message

    } eDegubPrio;

    /**
     * \brief   NEDebug::MAX_DEBUG_BUFFER_SIZE
     *          The maximum buffer size for message output.
     **/
    const unsigned int  MAX_DEBUG_BUFFER_SIZE   = 1024;
    /**
     * \brief   NEDebug::PREFIX_PRIO_DEBUG
     *          Prefix string, used to output messages with NEDebug::PRIO_DBG priority
     **/
    const char* const   PREFIX_PRIO_DEBUG       = "DBG: ";
    /**
     * \brief   NEDebug::PREFIX_PRIO_INFO
     *          Prefix string, used to output messages with NEDebug::PRIO_INFO priority
     **/
    const char* const   PREFIX_PRIO_INFO        = "INF: ";
    /**
     * \brief   NEDebug::PREFIX_PRIO_WARNING
     *          Prefix string, used to output messages with NEDebug::PRIO_WARN priority
     **/
    const char* const   PREFIX_PRIO_WARNING     = "WRN: ";
    /**
     * \brief   NEDebug::PREFIX_PRIO_ERROR
     *          Prefix string, used to output messages with NEDebug::PRIO_ERR priority
     **/
    const char* const   PREFIX_PRIO_ERROR       = "ERR: ";

    /**
     * \brief   NEDebug::getPrioPrefix()
     *          Returns prefix string of specified priority
     * \param   priority    The priority of message
     * \return  Returns one of defined prefixes, depending of priority.
     *          By default, the priority is  NEDebug::PRIO_DBG
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

#ifdef  _WINDOWS
    /**
     * \brief   Extracts call stack information from passed exception pointer and dumps in
     *          specified vector object. On output, the vector object will contain
     *          call stack message where the exception was raised.
     *          The function extracts call stack only in debug version.
     * \param   ep              The pointer to exception structure, which contains pointer to CONTEXT
     *                          object to extract call stack.
     * \param   out_callStack   On output this will contain the message of call stack.
     **/

    void AREG_API dumpExceptionCallStack( struct _EXCEPTION_POINTERS *ep, StringList & out_callStack);
#endif  // _WINDOWS

    /**
     * \brief   Operating System specific call to output message.
     *          In Windows this may output on VS Output Window.
     *          In other OS this may output on console.
     *          If message is empty, nothing is output.
     *          Method is valid only for DEBUG build.
     * \param   msg     The message to output.
     * \param   isError And additional flag, indicating whether it an error or not.
     **/
    void AREG_API outputMessageOS( const char * msg );
}

//////////////////////////////////////////////////////////////////////////
// NEDebug namespace inline function implementation.
//////////////////////////////////////////////////////////////////////////
inline const char* NEDebug::getPrioPrefix( NEDebug::eDegubPrio priority )
{
    switch ( priority )
    {
    case NEDebug::PRIO_ERR:
        return NEDebug::PREFIX_PRIO_ERROR;

    case NEDebug::PRIO_WARN:
        return NEDebug::PREFIX_PRIO_WARNING;

    case NEDebug::PRIO_INFO:
        return NEDebug::PREFIX_PRIO_INFO;

    case NEDebug::PRIO_DBG:
    default:
        return NEDebug::PREFIX_PRIO_DEBUG;
    }
}

#endif  // AREG_BASE_PRIVATE_NEDEBUG_HPP
