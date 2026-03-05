#ifndef AREG_LOGGING_SCOPEMESSAGE_HPP
#define AREG_LOGGING_SCOPEMESSAGE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/ScopeMessage.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Logging / tracing object to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/LoggingDefs.hpp"
#include <stdarg.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class LogScope;
    class LogMessage;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////////
// ScopeMessage class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   Encapsulates a log scope with message logging capabilities. Tracks scope lifetime and
 *          manages message priority filtering. Sends scope enter/exit messages and log messages to
 *          configured targets.
 **/
class AREG_API ScopeMessage
{
//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes with a LogScope and sends an enter message if scope priority is enabled.
     *
     * \param   logScope    The LogScope object containing priority and name.
     **/
    ScopeMessage( const LogScope & logScope );

    ~ScopeMessage();

#if AREG_LOGGING
//////////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Logs a message without a specific scope.
     *
     * \param   logPrio     The priority level of the message.
     * \param   format      The format string.
     **/
    static void log( areg::LogPriority logPrio, const char * format, ... );

    /**
     * \brief   Logs a Debug priority message if the scope has Debug priority enabled.
     *
     * \param   format      The format string.
     **/
    void log_debug( const char * format, ... ) const;

    /**
     * \brief   Logs an Information priority message if the scope has Information or lower priority
     *          enabled.
     *
     * \param   format      The format string.
     **/
    void log_info( const char * format, ... ) const;

    /**
     * \brief   Logs a Warning priority message if the scope has Warning or lower priority enabled.
     *
     * \param   format      The format string.
     **/
    void log_warning( const char * format, ... ) const;

    /**
     * \brief   Logs an Error priority message if the scope has Error or lower priority enabled.
     *
     * \param   format      The format string.
     **/
    void log_error( const char * format, ... ) const;

    /**
     * \brief   Logs a Fatal Error priority message if the scope has any priority enabled.
     *
     * \param   format      The format string.
     **/
    void log_fatal( const char * format, ... ) const;

    /**
     * \brief   Logs a message with the specified priority, bypassing the scope's priority check.
     *
     * \param   logPrio     The priority level of the message.
     * \param   format      The format string.
     **/
    void log_message( areg::LogPriority logPrio, const char * format, ...);

    /**
     * \brief   Returns true if scope priority logging is enabled.
     **/
    [[nodiscard]]
    inline bool is_scope_enabled() const noexcept;

    /**
     * \brief   Returns true if Debug priority logging is enabled.
     **/
    [[nodiscard]]
    inline bool is_dbg_enabled() const noexcept;

    /**
     * \brief   Returns true if Information priority or lower is enabled.
     **/
    [[nodiscard]]
    inline bool is_info_enabled() const noexcept;

    /**
     * \brief   Returns true if Warning priority or lower is enabled.
     **/
    [[nodiscard]]
    inline bool is_warn_enabled() const noexcept;

    /**
     * \brief   Returns true if Error priority or lower is enabled.
     **/
    [[nodiscard]]
    inline bool is_err_enabled() const noexcept;

    /**
     * \brief   Returns true if Fatal Error priority is enabled.
     **/
    [[nodiscard]]
    inline bool is_fatal_enabled() const noexcept;

    /**
     * \brief   Returns true if any priority is enabled for this scope.
     **/
    [[nodiscard]]
    inline bool is_log_enabled() const noexcept;

    /**
     * \brief   Returns true if the specified priority is enabled.
     *
     * \param   msgPrio     The priority level to check.
     * \return  True if the priority is enabled; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_prio_enabled( areg::LogPriority msgPrio ) const;

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Creates a log message and sends it to configured logging targets.
     *
     * \param   scopeId         The scope ID.
     * \param   sessionId       The session ID.
     * \param   scopeStamp      The scope timestamp for duration calculation.
     * \param   msgPrio         The message priority.
     * \param   format          The format string.
     * \param   args            The format arguments.
     **/
    static void _send_log( uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * format, va_list args );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    const String &      mScopeName; //!< name of the logging scope.
    const uint32_t  mScopeId;   //!< ID of the logging scope.
    const uint32_t  mSessionId; //!< Priority of the logging scope.
    const TIME64        mTimestamp; //!< The timestamp when the scope message object was instantiated.
    const uint32_t& mScopePrio; //!< Enabled logging priority for the scope.

#endif  // AREG_LOGGING

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is deleted.
     **/
    ScopeMessage() = delete;
    AREG_NOCOPY_NOMOVE( ScopeMessage );
};

//////////////////////////////////////////////////////////////////////////////
// ScopeMessage class inline methods
//////////////////////////////////////////////////////////////////////////////

#if AREG_LOGGING

inline bool ScopeMessage::is_scope_enabled() const noexcept
{
    return (mScopePrio &  static_cast<uint32_t>(areg::LogPriority::PrioScope));
}

inline bool ScopeMessage::is_dbg_enabled() const noexcept
{
    return (mScopePrio >= static_cast<uint32_t>(areg::LogPriority::PrioDebug));
}

inline bool ScopeMessage::is_info_enabled() const noexcept
{
    return (mScopePrio >= static_cast<uint32_t>(areg::LogPriority::PrioInfo));
}

inline bool ScopeMessage::is_warn_enabled() const noexcept
{
    return (mScopePrio >= static_cast<uint32_t>(areg::LogPriority::PrioWarning));
}

inline bool ScopeMessage::is_err_enabled() const noexcept
{
    return (mScopePrio >= static_cast<uint32_t>(areg::LogPriority::PrioError));
}

inline bool ScopeMessage::is_fatal_enabled() const noexcept
{
    return (mScopePrio >= static_cast<uint32_t>(areg::LogPriority::PrioFatal));
}

inline bool ScopeMessage::is_log_enabled() const noexcept
{
    return (mScopePrio != static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

inline bool ScopeMessage::is_prio_enabled(areg::LogPriority msgPrio) const
{
    return (msgPrio == areg::LogPriority::PrioScope ? mScopePrio &  static_cast<uint32_t>(areg::LogPriority::PrioScope) : mScopePrio >= static_cast<uint32_t>(msgPrio)) ;
}

#endif  // AREG_LOGGING

} // namespace areg
#endif  // AREG_LOGGING_SCOPEMESSAGE_HPP
