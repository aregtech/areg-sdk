#ifndef AREG_TRACE_PRIVATE_LOGMESSAGE_HPP
#define AREG_TRACE_PRIVATE_LOGMESSAGE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogMessage.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logging Message class. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;
class TraceScope;

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
class LogMessage  : public NETrace::sLogMessage
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class TraceMessage;    // need to access message log structure

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes message log structure and sets the logging type value.
     * \param   msgType     The type of message to initialize
     **/
    inline explicit LogMessage( NETrace::eLogMessageType msgType );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in message structure
     * \param   scopeId     The ID of trace scope, which is messaging
     * \param   msgPrio     The priority of message to log
     * \param   message     The text message to log
     **/
    inline LogMessage(NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const String & message );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in message structure
     * \param   scopeId     The ID of trace scope, which is messaging.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     * \param   msgLen      The length of the message to log.
     **/
    inline LogMessage( NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen );

    /**
     * \brief   Initializes message log structure for scope enter or exit event.
     *          The event depends on logging type value
     * \param   msgType     The log message type to set in message structure.
     *                      It is either to enter or exit scope.
     * \param   traceScope  The trace scope object, which contains ID and trace scope name is set as text.
     **/
    LogMessage( NETrace::eLogMessageType msgType, const TraceScope & traceScope );

    /**
     * \brief   Copies logging message data from given source.
     * \param   source  The source of data to copy.
     **/
    inline LogMessage( const LogMessage & source ) = default;

    /**
     * \brief   Initialization constructor.
     *          Initializes log message data from stream.
     **/
    inline LogMessage( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~LogMessage(void) = default;

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns instance of Log message structure
     **/
    inline const NETrace::sLogMessage & getLogData( void ) const;

    /**
     * \brief   Returns log type value
     **/
    inline NETrace::eLogMessageType getMessageType( void ) const;

    /**
     * \brief   Return the ID of thread where the message was initialized
     **/
    inline id_type getThreadId( void ) const;

    /**
     * \brief   Returns the ID of trace scope, which was logging message
     **/
    inline unsigned int getScopeId( void ) const;

    /**
     * \brief   Returns the time-stamp of created message log
     **/
    inline const DateTime getTimestamp( void ) const;

    /**
     * \brief   Returns message log module ID. By default, it is process ID.
     *          The module ID is an unique identifier set by system.
     **/
    inline const ITEM_ID & getModuleId( void ) const;

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
     * \brief   Returns the cookie value set by logger service.
     *          If equal NEService::COOKIE_LOCAL, the log messages should not be sent to remote host.
     *          If equal NEService::COOKIE_ANY, the log message should be queued to wait to complete remote logging service setup.
     *          If equal or more than NEService::COOKIE_REMOTE_SERVICE, the log message can be forwarded to remote host.
     **/
    inline const ITEM_ID & getCookie(void) const;

    /**
     * \brief   Sets the cookie value set by logger service.
     **/
    inline void setCookie(const ITEM_ID& newCookie);

    /**
     * \brief   Returns the priority ob message log
     **/
    inline NETrace::eLogPriority getMessagePrio( void ) const;

    /**
     * \brief   Sets the priority of message log
     **/
    inline void setMessagePrio( NETrace::eLogPriority msgPrio );

    /**
     * \brief   Returns message text, if there is any.
     **/
    inline const char * getMessage( void ) const;

    /**
     * \brief   Sets message text in log
     **/
    void setMessage( const char * message, int msgLen );
#endif  // AREG_LOGS
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    LogMessage( void ) = delete;
    DECLARE_NOMOVE( LogMessage );
};

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// LogMessage class inline methods
//////////////////////////////////////////////////////////////////////////
inline LogMessage::LogMessage( NETrace::eLogMessageType msgType )
    : NETrace::sLogMessage(msgType)
{
}

inline LogMessage::LogMessage( NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const String & message )
    : NETrace::sLogMessage( msgType, scopeId, msgPrio, message.getString(), static_cast<uint32_t>(message.getLength()) )
{
}

inline LogMessage::LogMessage( NETrace::eLogMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : NETrace::sLogMessage( msgType, scopeId, msgPrio, message, msgLen )
{
}

inline LogMessage::LogMessage( const IEInStream & stream )
    : NETrace::sLogMessage( )
{
    stream >> static_cast<NETrace::sLogMessage &>(*this);
}

inline const NETrace::sLogMessage & LogMessage::getLogData(void) const
{
    return static_cast<const NETrace::sLogMessage &>(*this);
}

inline NETrace::eLogMessageType LogMessage::getMessageType(void) const
{
    return this->logMsgType;
}

inline id_type LogMessage::getThreadId(void) const
{
    return static_cast<id_type>(this->logThreadId);
}

inline unsigned int LogMessage::getScopeId(void) const
{
    return this->logScopeId;
}

inline const DateTime LogMessage::getTimestamp(void) const
{
    return static_cast<DateTime>(this->logTimestamp);
}

inline const ITEM_ID & LogMessage::getModuleId(void) const
{
    return this->logModuleId;
}

inline void LogMessage::setModuleId(const ITEM_ID & moduleId)
{
    this->logModuleId = moduleId;
}

inline NETrace::eLogPriority LogMessage::getMessagePrio(void) const
{
    return this->logMessagePrio;
}

inline void LogMessage::setMessagePrio(const NETrace::eLogPriority msgPrio)
{
    this->logMessagePrio = msgPrio;
}

inline const char * LogMessage::getMessage(void) const
{
    return this->logMessage;
}

inline const ITEM_ID & LogMessage::getCookie(void) const
{
    return this->logCookie;
}

inline void LogMessage::setCookie(const ITEM_ID & newCookie)
{
    this->logCookie = newCookie;
}

#else   // AREG_LOGS

inline LogMessage::LogMessage(NETrace::eLogMessageType /*msgType*/)
    : NETrace::sLogMessage()
{
}

inline LogMessage::LogMessage(NETrace::eLogMessageType /*msgType*/, unsigned int /*scopeId*/, NETrace::eLogPriority /*msgPrio*/, const String& /*message*/)
    : NETrace::sLogMessage()
{
}

inline LogMessage::LogMessage(NETrace::eLogMessageType /*msgType*/, unsigned int /*scopeId*/, NETrace::eLogPriority /*msgPrio*/, const char* /*message*/, unsigned int /*msgLen*/)
    : NETrace::sLogMessage()
{
}

inline LogMessage::LogMessage(const IEInStream& stream)
    : NETrace::sLogMessage()
{
    stream >> static_cast<NETrace::sLogMessage&>(*this);
}

#endif  // AREG_LOGS
#endif  // AREG_TRACE_PRIVATE_LOGMESSAGE_HPP
