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
#include "areg/base/GEGlobal.h"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class InStream; }
namespace areg { class OutStream; }
class LogScope;

//////////////////////////////////////////////////////////////////////////
// LogMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Message Log class is an extension of Message Log structure.
 *          It is initializing structure of logging message and set values.
 *          The structure can be serialized in the streaming object.
 *          Different types of constructor allow to initialize message log
 *          structure with certain log types and priorities.
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
     * \brief   Initializes message log structure and sets the logging type value.
     * \param   msgType     The type of message to initialize
     **/
    inline explicit LogMessage( areg::LogMessageType msgType );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in the message structure.
     * \param   scopeId     The ID of messaging log scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   scopeStamp  The timestamp of the scope message, which is used to log message. This parameter is used to set duration.
     *                      The duration is ignored and set to 0 if the scopeStamp is 0.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     **/
    inline LogMessage(areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const areg::String & message );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in message structure
     * \param   scopeId     The ID of messaging log scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   scopeStamp  The timestamp of the scope message, which is used to log message. This parameter is used to set duration.
     *                      The duration is ignored and set to 0 if the scopeStamp is 0.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     * \param   msgLen      The length of the message to log.
     **/
    inline LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen );

    /**
     * \brief   Initializes message log structure for scope enter or exit event.
     *          The event depends on logging type value
     * \param   msgType     The log message type to set in message structure.
     *                      It is either to enter or exit scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   scopeStamp  The timestamp of the scope message, which is used to log message. This parameter is used to set duration.
     *                      The duration is ignored and set to 0 if the scopeStamp is 0.
     * \param   logScope    The log scope object with scope name and ID to set.
     **/
    LogMessage( areg::LogMessageType msgType, uint32_t sessionid, TIME64 scopeStamp, const LogScope & logScope );

    /**
     * \brief   Copies logging message data from given source.
     * \param   source  The source of data to copy.
     **/
    inline LogMessage( const LogMessage & source ) = default;

    /**
     * \brief   Initialization constructor.
     *          Initializes log message data from stream.
     **/
    inline LogMessage( const areg::InStream & stream );

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
     * \brief   Returns instance of Log message structure
     **/
    inline const areg::LogEntry & getLogData() const;

    /**
     * \brief   Returns log type value
     **/
    inline areg::LogMessageType getMessageType() const;

    /**
     * \brief   Return the ID of thread where the message was initialized.
     **/
    inline id_type getThreadId() const;

    /**
     * \brief   Returns the ID of log scope.
     **/
    inline uint32_t getScopeId() const;

    /**
     * \brief   Returns the time-stamp of created message log.
     **/
    inline const DateTime getTimestamp() const;

    /**
     * \brief   Returns the time-stamp when the message log was received.
     **/
    inline const DateTime getTimeReceived() const;

    /**
     * \brief   Returns the duration in microseconds since scope message object was instantiated.
     */
    inline uint32_t getDuration() const;

    /**
     * \brief   Returns message log module ID. By default, it is process ID.
     *          The module ID is an unique identifier set by system.
     **/
    inline const ITEM_ID & getModuleId() const;

    /**
     * \brief   Set the module ID. The module ID can be any value
     *          which is unique within certain system.
     *          For example, within one host PC, this can be unique
     *          process ID. Within Network, this can be identifier
     *          value, unique in Network.
     *          The Module ID is used to differentiate same scopes
     *          in different processes / machines.
     **/
    inline void setModuleId( const ITEM_ID & moduleId );

    /**
     * \brief   Returns the cookie value set by Log Collector service.
     *          If equal areg::COOKIE_LOCAL, the log messages should not be sent to remote host.
     *          If equal areg::COOKIE_ANY, the log message should be queued to wait to complete remote logging service setup.
     *          If equal or more than areg::COOKIE_REMOTE_SERVICE, the log message can be forwarded to remote host.
     **/
    inline const ITEM_ID & getCookie() const;

    /**
     * \brief   Sets the cookie value set by Log Collector service.
     **/
    inline void setCookie(const ITEM_ID& newCookie);

    /**
     * \brief   Returns the priority ob message log
     **/
    inline areg::LogPriority getMessagePrio() const;

    /**
     * \brief   Sets the priority of message log
     **/
    inline void setMessagePrio( areg::LogPriority msgPrio );

    /**
     * \brief   Returns message text, if there is any.
     **/
    inline const char * getMessage() const;

    /**
     * \brief   Sets message text in log
     **/
    void setMessage( const char * message, int32_t msgLen );
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
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

inline LogMessage::LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const areg::String & message )
    : areg::LogEntry( msgType, scopeId, sessionId, scopeStamp, msgPrio, message.getString(), static_cast<uint32_t>(message.getLength()) )
{
}

inline LogMessage::LogMessage( areg::LogMessageType msgType, uint32_t scopeId, uint32_t sessionId, TIME64 scopeStamp, areg::LogPriority msgPrio, const char * message, uint32_t msgLen )
    : areg::LogEntry( msgType, scopeId, sessionId, scopeStamp, msgPrio, message, msgLen )
{
}

inline LogMessage::LogMessage( const areg::InStream & stream )
    : areg::LogEntry( )
{
    stream >> static_cast<areg::LogEntry &>(*this);
}

inline const areg::LogEntry & LogMessage::getLogData() const
{
    return static_cast<const areg::LogEntry &>(*this);
}

inline areg::LogMessageType LogMessage::getMessageType() const
{
    return this->logMsgType;
}

inline id_type LogMessage::getThreadId() const
{
    return static_cast<id_type>(this->logThreadId);
}

inline uint32_t LogMessage::getScopeId() const
{
    return this->logScopeId;
}

inline const DateTime LogMessage::getTimestamp() const
{
    return static_cast<DateTime>(this->logTimestamp);
}

inline const DateTime LogMessage::getTimeReceived() const
{
    return static_cast<DateTime>(this->logReceived);
}

inline uint32_t LogMessage::getDuration() const
{
    return static_cast<uint32_t>(this->logDuration);
}

inline const ITEM_ID & LogMessage::getModuleId() const
{
    return this->logModuleId;
}

inline void LogMessage::setModuleId(const ITEM_ID & moduleId)
{
    this->logModuleId = moduleId;
}

inline areg::LogPriority LogMessage::getMessagePrio() const
{
    return this->logMessagePrio;
}

inline void LogMessage::setMessagePrio(const areg::LogPriority msgPrio)
{
    this->logMessagePrio = msgPrio;
}

inline const char * LogMessage::getMessage() const
{
    return this->logMessage;
}

inline const ITEM_ID & LogMessage::getCookie() const
{
    return this->logCookie;
}

inline void LogMessage::setCookie(const ITEM_ID & newCookie)
{
    this->logCookie = newCookie;
}

#else   // AREG_LOGS

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const areg::String& /*message*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(areg::LogMessageType /*msgType*/, uint32_t /*scopeId*/, uint32_t /*sessionId*/, TIME64 /*scopeStamp*/, areg::LogPriority /*msgPrio*/, const char* /*message*/, uint32_t /*msgLen*/)
    : areg::LogEntry()
{
}

inline LogMessage::LogMessage(const areg::InStream& stream)
    : areg::LogEntry()
{
    stream >> static_cast<areg::LogEntry&>(*this);
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
