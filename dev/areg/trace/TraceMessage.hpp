#ifndef AREG_TRACE_TRACEMESSAGE_HPP
#define AREG_TRACE_TRACEMESSAGE_HPP
/************************************************************************
 * \file        areg/trace/TraceMessage.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Logging / tracing object to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include <stdio.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceScope;
class LogMessage;

//////////////////////////////////////////////////////////////////////////////
// TraceMessage class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging message (or trace message) is an object, which contains data to log.
 *          By the given Trace Scope, it determines the message log priorities
 *          and if Trace Scope has enabled certain message priority, the logger
 *          will create message longing object for further output on logging targets.
 *          The information of enabled logging priority is set on trace scope and
 *          the message, as well as message priority, is part of logging message.
 *          Before the logger generates output message, the scope should have certain
 *          priority enabled. And before the message is output of target (file, console or remote service),
 *          the logging should be started and the output targets should be initialized.
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
    const char *        mScopeName; //!< The name of logging scope
    const unsigned int  mScopeId;   //!< The ID of logging scope
    const unsigned int& mScopePrio; //!< The logging priority enabled in scope.

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    TraceMessage( void );
    TraceMessage( const TraceMessage & /* src */ );
    const TraceMessage & operator = ( const TraceMessage & /* src */ );
};

#endif  // AREG_TRACE_TRACEMESSAGE_HPP
