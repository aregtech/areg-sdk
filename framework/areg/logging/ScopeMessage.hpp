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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/logging/ScopeMessage.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Logging / tracing object to log messages.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/DateTime.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/LogScope.hpp"
#include <stdarg.h>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
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
    void log_message( areg::LogPriority logPrio, const char * format, ... ) const;

    /**
     * \brief   Returns true if scope enter and exit messages are enabled for this object.
     *
     * \note    The value is captured when the object is created, so the enter and the exit
     *          message are always sent as a pair even if the scope priority changes in between.
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
    inline bool is_prio_enabled( areg::LogPriority msgPrio ) const noexcept;

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

    /**
     * \brief   Sends a scope enter or scope exit message to the configured logging targets.
     *
     * \param   msgType     Either LogMessageType::ScopeEnter or LogMessageType::ScopeExit.
     **/
    void _send_scope( areg::LogMessageType msgType ) const;

    /**
     * \brief   Returns true if the given priority lets the scope emit a message.
     *
     * \param   scopePrio   The scope priority to check.
     **/
    [[nodiscard]]
    static inline bool _can_log( uint32_t scopePrio ) noexcept;

    /**
     * \brief   Initializes the object from a priority that the caller read once.
     *
     * \param   logScope    The scope object.
     * \param   scopePrio   The priority of the scope.
     **/
    ScopeMessage( const LogScope & logScope, uint32_t scopePrio );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    const LogScope& mScope;         //!< The scope object to filter messages.
    const uint32_t  mSessionId;     //!< Session of the logging scope.
    const bool      mScopeEnabled;  //!< True if the scope enter and exit messages are enabled.
    const TIME64    mTimestamp;     //!< The timestamp when the scope message object was instantiated.

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
    return mScopeEnabled;
}

inline bool ScopeMessage::is_dbg_enabled() const noexcept
{
    return (mScope.priority() >= static_cast<uint32_t>(areg::LogPriority::PrioDebug));
}

inline bool ScopeMessage::is_info_enabled() const noexcept
{
    return (mScope.priority() >= static_cast<uint32_t>(areg::LogPriority::PrioInfo));
}

inline bool ScopeMessage::is_warn_enabled() const noexcept
{
    return (mScope.priority() >= static_cast<uint32_t>(areg::LogPriority::PrioWarning));
}

inline bool ScopeMessage::is_err_enabled() const noexcept
{
    return (mScope.priority() >= static_cast<uint32_t>(areg::LogPriority::PrioError));
}

inline bool ScopeMessage::is_fatal_enabled() const noexcept
{
    return (mScope.priority() >= static_cast<uint32_t>(areg::LogPriority::PrioFatal));
}

inline bool ScopeMessage::is_log_enabled() const noexcept
{
    return (mScope.priority() != static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

inline bool ScopeMessage::is_prio_enabled(areg::LogPriority msgPrio) const noexcept
{
    return (msgPrio == areg::LogPriority::PrioScope 
                ? (mScope.priority() &  static_cast<uint32_t>(areg::LogPriority::PrioScope)) != 0
                : mScope.priority() >= static_cast<uint32_t>(msgPrio)) ;
}

inline bool ScopeMessage::_can_log( uint32_t scopePrio ) noexcept
{
    return (scopePrio & static_cast<uint32_t>(areg::LogPriority::PrioScopeLogs)) != 0u;
}

inline ScopeMessage::ScopeMessage( const LogScope & logScope, uint32_t scopePrio )
    : mScope       ( logScope )
    , mSessionId   ( _can_log(scopePrio) ? logScope.next_session() : 0u )
    , mScopeEnabled( (scopePrio & static_cast<uint32_t>(areg::LogPriority::PrioScope)) != 0u )
    , mTimestamp   ( _can_log(scopePrio) ? static_cast<TIME64>(DateTime::timestamp()) : static_cast<TIME64>(0u) )
{
    if ( mScopeEnabled )
    {
        _send_scope( areg::LogMessageType::ScopeEnter );
    }
}

inline ScopeMessage::~ScopeMessage()
{
    if ( mScopeEnabled )
    {
        _send_scope( areg::LogMessageType::ScopeExit );
    }
}

inline ScopeMessage::ScopeMessage( const LogScope & logScope )
    : ScopeMessage( logScope, logScope.priority() )
{
}

#else   // AREG_LOGGING

inline ScopeMessage::ScopeMessage( const LogScope & /*logScope*/ )
{
}

inline ScopeMessage::~ScopeMessage()
{
}

#endif  // AREG_LOGGING

} // namespace areg
#endif  // AREG_LOGGING_SCOPEMESSAGE_HPP
