#ifndef AREG_TRACE_TRACEMESSAGE_HPP
#define AREG_TRACE_TRACEMESSAGE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/TraceMessage.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logging / tracing object to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include <stdarg.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;
class LogMessage;

//////////////////////////////////////////////////////////////////////////////
// TraceMessage class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging message (or trace message) is an object, which contains
 *          data to log. The Trace Scope determines the message log priorities
 *          and if the priority is enabled the system create message to log.
 * 
 *          The information of enabled logging priority is set in Trace Scope. 
 *          The log message and the priority are part of TraceMessage object.
 *          Before the message is generated to output, the system should start
 *          logging service to specify the message output target (file,
 *          console or remote service). Normally, the logging is tarted
 *          at application startup. If logging did not start, the system
 *          assumes that Trace Scopes are disabled and not activate and 
 *          ignores to log message.
 **/
class AREG_API TraceMessage
{
//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   It receives instance of initialized Trace Scope object.
     *          The logging message priorities are checked based on
     *          priority of Trace Scope object.
     *          If Trace Scope has enabled Scope priority, it will
     *          create enter trace scope message and forward to output on
     *          logging targets.
     * \param   traceScope  Initialized instance of Trace Scope object,
     *                      this is containing logging priority value.
     **/
    TraceMessage( const TraceScope & traceScope );

    /**
     * \brief   Destructor.
     *          If Trace Scope has enabled Scope priority, it will
     *          create exit trace scope message and forward to output on
     *          logging targets.
     **/
    ~TraceMessage( void );

//////////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Static method. Makes an message output on logging targets
     *          without specifying Trace Scope.
     * \param   logPrio     The message priority to show on logging targets
     * \param   format      The formatted message to forward.
     **/
    static void logTrace( NETrace::eLogPriority logPrio, const char * format, ... );

    /**
     * \brief   Makes Debug priority message output on logging targets.
     *          The Debug priority of Trace Scope object should be set.
     *          Otherwise, the logger will ignore this method call and 
     *          no message is forwarded.
     * \param   format      The formatted message to forward.
     **/
    void logDebug( const char * format, ... ) const;

    /**
     * \brief   Makes Info priority message output on logging targets.
     *          The Info priority of Trace Scope object should be set.
     *          Otherwise, the logger will ignore this method call and 
     *          no message is forwarded.
     * \param   format      The formatted message to forward.
     **/
    void logInfo( const char * format, ... ) const;

    /**
     * \brief   Makes Warning priority message output on logging targets.
     *          The Warning priority of Trace Scope object should be set.
     *          Otherwise, the logger will ignore this method call and 
     *          no message is forwarded.
     * \param   format      The formatted message to forward.
     **/
    void logWarning( const char * format, ... ) const;

    /**
     * \brief   Makes Error priority message output on logging targets.
     *          The Error priority of Trace Scope object should be set.
     *          Otherwise, the logger will ignore this method call and 
     *          no message is forwarded.
     * \param   format      The formatted message to forward.
     **/
    void logError( const char * format, ... ) const;

    /**
     * \brief   Makes Fatal Error priority message output on logging targets.
     *          The Fatal Error priority of Trace Scope object should be set.
     *          Otherwise, the logger will ignore this method call and 
     *          no message is forwarded.
     * \param   format      The formatted message to forward.
     **/
    void logFatal( const char * format, ... ) const;

    /**
     * \brief   Formats and logs message of given priority without checking 
     *          whether the priority is enabled or not.
     * \param   logPrio     The logging priority of the message.
     * \param   format      The format of message, followed by arguments.
     **/
    void logMessage( NETrace::eLogPriority logPrio, const char * format, ...);

    /**
     * \brief   Returns true if for message tracer object logging the message scope is enabled.
     **/
    inline bool isScopeEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object logging message of debug priority is enabled.
     **/
    inline bool isDbgEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object logging message of information priority is enabled.
     **/
    inline bool isInfoEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object logging message of warning priority is enabled.
     **/
    inline bool isWarnEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object logging message of error priority is enabled.
     **/
    inline bool isErrEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object logging message of fatal priority is enabled.
     **/
    inline bool isFatalEnabled( void ) const;

    /**
     * \brief   Returns true if for message tracer object the logging is enabled.
     **/
    inline bool isLogEnabled( void ) const;

    /**
     * \brief   Checks whether the given priority is enabled for message tracer object.
     * \param   msgPrio     The priority of message to check.
     * \return  Returns true if given priority is enabled. Otherwise, returns false.
     **/
    inline bool isPrioEnabled( NETrace::eLogPriority msgPrio ) const;

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Creates logging message object and forward to logging targets
     * \param   scopeId     The ID of trace scope to make messaging
     * \param   msgPrio     The priority of message to output
     * \param   format      The formated text to output
     * \param   args        The list of arguments to set in formated text.
     **/
    static void _sendLog( unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * format, va_list args );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    const String &      mScopeName; //!< The name of logging scope
    const unsigned int  mScopeId;   //!< The ID of logging scope
    const unsigned int& mScopePrio; //!< The logging priority enabled in scope.

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    TraceMessage( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TraceMessage );
};

//////////////////////////////////////////////////////////////////////////////
// TraceMessage class inline methods
//////////////////////////////////////////////////////////////////////////////

inline bool TraceMessage::isScopeEnabled(void) const
{
    return (mScopePrio &  static_cast<unsigned int>(NETrace::PrioScope));
}

inline bool TraceMessage::isDbgEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NETrace::PrioDebug));
}

inline bool TraceMessage::isInfoEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NETrace::PrioInfo));
}

inline bool TraceMessage::isWarnEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NETrace::PrioWarning));
}

inline bool TraceMessage::isErrEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NETrace::PrioError));
}

inline bool TraceMessage::isFatalEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NETrace::PrioFatal));
}

inline bool TraceMessage::isLogEnabled(void) const
{
    return (mScopePrio != static_cast<unsigned int>(NETrace::PrioNotset));
}

inline bool TraceMessage::isPrioEnabled(NETrace::eLogPriority msgPrio) const
{
    return (msgPrio == NETrace::PrioScope ? mScopePrio &  static_cast<unsigned int>(NETrace::PrioScope) : mScopePrio >= static_cast<unsigned int>(msgPrio)) ;
}

#endif  // AREG_TRACE_TRACEMESSAGE_HPP
