#ifndef AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
#define AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogMessage.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Logging Message class. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class InStream;
    class OutStream;
    class LogScope;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// LogMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Encapsulates a logging message with type, priority, scope information, and text.
 *          Serializable for IPC and remote logging. Supports multiple construction patterns for
 *          different message types (enter/exit scope, debug/info/error messages).
 **/
class LogMessage  : public areg::LogEntry
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ScopeMessage;    // need to access message log structure

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes a log message with the specified type.
     *
     * \param   msgType     The log message type to set.
     **/
    inline explicit LogMessage( areg::LogMessageType msgType );

    /**
     * \brief   Initializes a log message with type, scope info, priority, and text.
     *
     * \param   msgType         The log message type.
     * \param   scopeId         The scope ID.
     * \param   sessionId       The session ID to differentiate messages in the same scope.
     * \param   scopeStamp      The scope timestamp for duration calculation; ignored if zero.
     * \param   msgPrio         The message priority.
     * \param   message         The message text.
     **/
    inline LogMessage(areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const String & message );

    /**
     * \brief   Initializes a log message from a raw string and length.
     *
     * \param   msgType         The log message type.
     * \param   scopeId         The scope ID.
     * \param   sessionId       The session ID to differentiate messages in the same scope.
     * \param   scopeStamp      The scope timestamp for duration calculation; ignored if zero.
     * \param   msgPrio         The message priority.
     * \param   message         The message text as a C string.
     * \param   msgLen          The length of the message string.
     **/
    inline LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen );

    /**
     * \brief   Initializes a scope enter/exit message from a LogScope.
     *
     * \param   msgType         The log message type (enter or exit).
     * \param   sessionId       The session ID to differentiate messages in the same scope.
     * \param   scopeStamp      The scope timestamp for duration calculation; ignored if zero.
     * \param   logScope        The LogScope containing name and ID.
     **/
    LogMessage( areg::LogMessageType msgType, uint32_t sessionid, TIME64 scopeStamp, const LogScope & logScope );

    /**
     * \brief   Copy constructor.
     *
     * \param   source      The source log message to copy.
     **/
    inline LogMessage( const LogMessage & source ) = default;

    /**
     * \brief   Initializes a log message by deserializing from a stream.
     **/
    inline LogMessage( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~LogMessage() = default;

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the underlying log entry structure.
     **/
    inline const areg::LogEntry & log_data() const;

    /**
     * \brief   Returns the log message type.
     **/
    inline areg::LogMessageType message_type() const;

    /**
     * \brief   Returns the ID of the thread that created the message.
     **/
    inline id_type thread_id() const;

    /**
     * \brief   Returns the scope ID.
     **/
    inline uint32_t scope_id() const;

    /**
     * \brief   Returns the message creation timestamp.
     **/
    inline const DateTime timestamp() const;

    /**
     * \brief   Returns the message reception timestamp.
     **/
    inline const DateTime time_received() const;

    /**
     * \brief   Returns the scope duration in microseconds since creation.
     **/
    inline uint32_t duration() const;

    /**
     * \brief   Returns the module ID (typically the process ID or network identifier).
     **/
    inline const ITEM_ID & module_id() const;

    /**
     * \brief   Sets the module ID for differentiating scopes across processes or machines.
     **/
    inline void set_module_id( const ITEM_ID & moduleId );

    /**
     * \brief   Returns the log cookie status controlling remote logging behavior.
     **/
    inline const ITEM_ID & cookie() const;

    /**
     * \brief   Sets the log cookie status.
     **/
    inline void set_cookie(const ITEM_ID& newCookie);

    /**
     * \brief   Returns the message priority level.
     **/
    inline areg::LogPriority message_prio() const;

    /**
     * \brief   Sets the message priority level.
     **/
    inline void set_message_prio( areg::LogPriority msgPrio );

    /**
     * \brief   Returns the message text, if any.
     **/
    inline const char * message() const;

    /**
     * \brief   Sets the message text.
     **/
    void set_message( const char * message, int32_t msgLen );
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is deleted.
     **/
    LogMessage() = delete;
    AREG_NOMOVE( LogMessage );
};

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// LogMessage class inline methods
//////////////////////////////////////////////////////////////////////////
inline LogMessage::LogMessage( areg::LogMessageType msgType )
    : areg::LogEntry(msgType)
{
}

inline LogMessage::LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const String & message )
    : areg::LogEntry( msgType, scopeId, sessionId, scopeStamp, msgPrio, message.as_string(), static_cast<uint32_t>(message.length()) )
{
}

inline LogMessage::LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen )
    : areg::LogEntry( msgType, scopeId, sessionId, scopeStamp, msgPrio, message, msgLen )
{
}

inline LogMessage::LogMessage( const InStream & stream )
    : areg::LogEntry( )
{
    stream >> static_cast<areg::LogEntry &>(*this);
}

inline const areg::LogEntry & LogMessage::log_data() const
{
    return static_cast<const areg::LogEntry &>(*this);
}

inline areg::LogMessageType LogMessage::message_type() const
{
    return this->logMsgType;
}

inline id_type LogMessage::thread_id() const
{
    return static_cast<id_type>(this->logThreadId);
}

inline uint32_t LogMessage::scope_id() const
{
    return this->logScopeId;
}

inline const DateTime LogMessage::timestamp() const
{
    return static_cast<DateTime>(this->logTimestamp);
}

inline const DateTime LogMessage::time_received() const
{
    return static_cast<DateTime>(this->logReceived);
}

inline uint32_t LogMessage::duration() const
{
    return static_cast<uint32_t>(this->logDuration);
}

inline const ITEM_ID & LogMessage::module_id() const
{
    return this->logModuleId;
}

inline void LogMessage::set_module_id(const ITEM_ID & moduleId)
{
    this->logModuleId = moduleId;
}

inline areg::LogPriority LogMessage::message_prio() const
{
    return this->logMessagePrio;
}

inline void LogMessage::set_message_prio(const areg::LogPriority msgPrio)
{
    this->logMessagePrio = msgPrio;
}

inline const char * LogMessage::message() const
{
    return this->logMessage;
}

inline const ITEM_ID & LogMessage::cookie() const
{
    return this->logCookie;
}

inline void LogMessage::set_cookie(const ITEM_ID & newCookie)
{
    this->logCookie = newCookie;
}

#else   // AREG_LOGS

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const String& /*message*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(const InStream& stream)
    : areg::LogEntry()
{
    stream >> static_cast<areg::LogEntry&>(*this);
}

#endif  // AREG_LOGS

} // namespace areg
#endif  // AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
