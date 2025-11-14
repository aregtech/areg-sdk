#ifndef AREG_LOGGING_SCOPEMESSAGE_HPP
#define AREG_LOGGING_SCOPEMESSAGE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/ScopeMessage.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logging / tracing object to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/NELogging.hpp"
#include <stdarg.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
class LogScope;
class LogMessage;

//////////////////////////////////////////////////////////////////////////////
// ScopeMessage class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   Represents a message of log scope object containing log message priority.
 *          If the priority is enabled, the system creates the log message.
 * 
 *          The ScopeMessage object encapsulates both the log message and its priority.
 *          Logging targets (e.g., file, console, or remote services) are specified
 *          by the logging service, which should be started at application initialization.
 *          If logging is not started, Log Scopes are assumed to be disabled, 
 *          and messages will not be logged.
 **/
class AREG_API ScopeMessage
{
//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object with an instance of an initialized Log Scope.
     *          Logging priorities are determined based on the Log Scope's priority.
     *          If Scope Priority is enabled, creates an "enter scope" tracing message 
     *          and sends it to configured logging targets.
     * \param   logScope    Instance of Log Scope object containing log priority values.
     **/
    ScopeMessage( const LogScope & logScope );

    /**
     * \brief   If Scope Priority is enabled for the Log Scope, creates an 
     *          "exit scope" tracing message and sends it to the configured logging targets.
     **/
    ~ScopeMessage( void );

#if AREG_LOGS
//////////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Outputs a message to the logging targets without associating it with a specific Log Scope.
     * \param   logPrio The priority level of the message for logging targets.
     * \param   format  The formatted message string to forward.
     **/
    static void log( NELogging::eLogPriority logPrio, const char * format, ... );

    /**
     * \brief   Outputs a Debug priority message to logging targets.
     *          The Log Scope must have Debug priority enabled; otherwise,
     *          the message will not be logged.
     * \param   format  The formatted message string to log.
     **/
    void logDebug( const char * format, ... ) const;

    /**
     * \brief   Outputs an Information priority message to logging targets.
     *          The Log Scope must have Information or a lower priority enabled;
     *          otherwise, the message will not be logged.
     * \param   format  The formatted message string to log.
     **/
    void logInfo( const char * format, ... ) const;

    /**
     * \brief   Outputs a Warning priority message to logging targets.
     *          The Log Scope must have Warning or a lower priority enabled;
     *          otherwise, the message will not be logged.
     * \param   format  The formatted message string to log.
     **/
    void logWarning( const char * format, ... ) const;

    /**
     * \brief   Outputs an Error priority message to logging targets.
     *          The Log Scope must have Error or a lower priority enabled;
     *          otherwise, the message will not be logged.
     * \param   format  The formatted message string to log.
     **/
    void logError( const char * format, ... ) const;

    /**
     * \brief   Outputs a Fatal Error priority message to logging targets.
     *          The Log Scope must have any log priority enabled; otherwise,
     *          the message will not be logged.
     * \param   format  The formatted message string to log.
     **/
    void logFatal( const char * format, ... ) const;

    /**
     * \brief   Formats and logs a message with the specified priority, bypassing 
     *          the priority check.
     * \param   logPrio The priority of the message.
     * \param   format  The formatted message string to log.
     **/
    void logMessage( NELogging::eLogPriority logPrio, const char * format, ...);

    /**
     * \brief   Checks if Scope Priority logging is enabled for the Log Scope.
     **/
    inline bool isScopeEnabled( void ) const;

    /**
     * \brief   Checks if Debug priority logging is enabled for the Log Scope.
     **/
    inline bool isDbgEnabled( void ) const;

    /**
     * \brief   Checks if Information or a lower priority logging is enabled for the Log Scope.
     **/
    inline bool isInfoEnabled( void ) const;

    /**
     * \brief   Checks if Warning or a lower priority logging is enabled for the Log Scope.
     **/
    inline bool isWarnEnabled( void ) const;

    /**
     * \brief   Checks if Error or a lower priority logging is enabled for the Log Scope.
     **/
    inline bool isErrEnabled( void ) const;

    /**
     * \brief   Checks if Fatal Error priority logging is enabled.
     **/
    inline bool isFatalEnabled( void ) const;

    /**
     * \brief   Checks if the Log Scope has any priority enabled.
     **/
    inline bool isLogEnabled( void ) const;

    /**
     * \brief   Checks if the specified log message priority is enabled.
     * \param   msgPrio     The priority level of the message to check.
     * \return  Returns true if the specified priority is enabled; otherwise, false.
     **/
    inline bool isPrioEnabled( NELogging::eLogPriority msgPrio ) const;

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Creates a logging message object and sends it to the logging targets.
     * \param   scopeId     The ID of the Log Scope.
     * \param   sessionId   The ID of the session, used to differentiate messages of the same scope.
     * \param   scopeStamp  The timestamp of the scope message, which is used to log message. This parameter is used to set duration.
     *                      The duration is ignored and set to 0 if the scopeStamp is 0.
     * \param   msgPrio     The priority of the message to log.
     * \param   format      The formatted text to output.
     * \param   args        The list of arguments to apply to the formatted text.
     **/
    static void _sendLog( unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const char * format, va_list args );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    const String &      mScopeName; //!< Name of the logging scope.
    const unsigned int  mScopeId;   //!< ID of the logging scope.
    const unsigned int  mSessionId; //!< Priority of the logging scope.
    const TIME64        mTimestamp; //!< The timestamp when the scope message object was instantiated.
    const unsigned int& mScopePrio; //!< Enabled logging priority for the scope.

#endif  // AREG_LOGS

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    ScopeMessage( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ScopeMessage );
};

//////////////////////////////////////////////////////////////////////////////
// ScopeMessage class inline methods
//////////////////////////////////////////////////////////////////////////////

#if AREG_LOGS

inline bool ScopeMessage::isScopeEnabled(void) const
{
    return (mScopePrio &  static_cast<unsigned int>(NELogging::PrioScope));
}

inline bool ScopeMessage::isDbgEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NELogging::PrioDebug));
}

inline bool ScopeMessage::isInfoEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NELogging::PrioInfo));
}

inline bool ScopeMessage::isWarnEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NELogging::PrioWarning));
}

inline bool ScopeMessage::isErrEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NELogging::PrioError));
}

inline bool ScopeMessage::isFatalEnabled(void) const
{
    return (mScopePrio >= static_cast<unsigned int>(NELogging::PrioFatal));
}

inline bool ScopeMessage::isLogEnabled(void) const
{
    return (mScopePrio != static_cast<unsigned int>(NELogging::PrioNotset));
}

inline bool ScopeMessage::isPrioEnabled(NELogging::eLogPriority msgPrio) const
{
    return (msgPrio == NELogging::PrioScope ? mScopePrio &  static_cast<unsigned int>(NELogging::PrioScope) : mScopePrio >= static_cast<unsigned int>(msgPrio)) ;
}

#endif  // AREG_LOGS

#endif  // AREG_LOGGING_SCOPEMESSAGE_HPP
