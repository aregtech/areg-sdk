#ifndef AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
#define AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogMessage.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Logging Message class. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/NELogging.hpp"
#include "areg/base/String.hpp"
#include "areg/base/DateTime.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;
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
class LogMessage  : public NELogging::sLogMessage
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
    inline explicit LogMessage( NELogging::eLogMessageType msgType );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in the message structure.
     * \param   scopeId     The ID of messaging log scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     **/
    inline LogMessage(NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const String & message );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   msgType     The log message type to set in message structure
     * \param   scopeId     The ID of messaging log scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     * \param   msgLen      The length of the message to log.
     **/
    inline LogMessage( NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const char * message, unsigned int msgLen );

    /**
     * \brief   Initializes message log structure for scope enter or exit event.
     *          The event depends on logging type value
     * \param   msgType     The log message type to set in message structure.
     *                      It is either to enter or exit scope.
     * \param   sessionId   The ID of session, which is used to differentiate messages of the same scope.
     * \param   logScope    The log scope object with scope name and ID to set.
     **/
    LogMessage( NELogging::eLogMessageType msgType, unsigned int sessionid, TIME64 scopeStamp, const LogScope & logScope );

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
    inline const NELogging::sLogMessage & getLogData( void ) const;

    /**
     * \brief   Returns log type value
     **/
    inline NELogging::eLogMessageType getMessageType( void ) const;

    /**
     * \brief   Return the ID of thread where the message was initialized.
     **/
    inline id_type getThreadId( void ) const;

    /**
     * \brief   Returns the ID of log scope.
     **/
    inline unsigned int getScopeId( void ) const;

    /**
     * \brief   Returns the time-stamp of created message log.
     **/
    inline const DateTime getTimestamp( void ) const;

    /**
     * \brief   Returns the time-stamp when the message log was received.
     **/
    inline const DateTime getTimeReceived(void) const;

    /**
     * \brief   Returns the duration in microseconds since scope message object was instantiated.
     */
    inline const uint32_t getDuration(void) const;

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
     * \brief   Returns the cookie value set by Log Collector service.
     *          If equal NEService::COOKIE_LOCAL, the log messages should not be sent to remote host.
     *          If equal NEService::COOKIE_ANY, the log message should be queued to wait to complete remote logging service setup.
     *          If equal or more than NEService::COOKIE_REMOTE_SERVICE, the log message can be forwarded to remote host.
     **/
    inline const ITEM_ID & getCookie(void) const;

    /**
     * \brief   Sets the cookie value set by Log Collector service.
     **/
    inline void setCookie(const ITEM_ID& newCookie);

    /**
     * \brief   Returns the priority ob message log
     **/
    inline NELogging::eLogPriority getMessagePrio( void ) const;

    /**
     * \brief   Sets the priority of message log
     **/
    inline void setMessagePrio( NELogging::eLogPriority msgPrio );

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
inline LogMessage::LogMessage( NELogging::eLogMessageType msgType )
    : NELogging::sLogMessage(msgType)
{
}

inline LogMessage::LogMessage( NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const String & message )
    : NELogging::sLogMessage( msgType, scopeId, sessionId, scopeStamp, msgPrio, message.getString(), static_cast<uint32_t>(message.getLength()) )
{
}

inline LogMessage::LogMessage( NELogging::eLogMessageType msgType, unsigned int scopeId, unsigned int sessionId, TIME64 scopeStamp, NELogging::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : NELogging::sLogMessage( msgType, scopeId, sessionId, scopeStamp, msgPrio, message, msgLen )
{
}

inline LogMessage::LogMessage( const IEInStream & stream )
    : NELogging::sLogMessage( )
{
    stream >> static_cast<NELogging::sLogMessage &>(*this);
}

inline const NELogging::sLogMessage & LogMessage::getLogData(void) const
{
    return static_cast<const NELogging::sLogMessage &>(*this);
}

inline NELogging::eLogMessageType LogMessage::getMessageType(void) const
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

inline const DateTime LogMessage::getTimeReceived(void) const
{
    return static_cast<DateTime>(this->logReceived);
}

inline const uint32_t LogMessage::getDuration(void) const
{
    return static_cast<uint32_t>(this->logDuration);
}

inline const ITEM_ID & LogMessage::getModuleId(void) const
{
    return this->logModuleId;
}

inline void LogMessage::setModuleId(const ITEM_ID & moduleId)
{
    this->logModuleId = moduleId;
}

inline NELogging::eLogPriority LogMessage::getMessagePrio(void) const
{
    return this->logMessagePrio;
}

inline void LogMessage::setMessagePrio(const NELogging::eLogPriority msgPrio)
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

inline LogMessage::LogMessage(NELogging::eLogMessageType /*msgType*/)
    : NELogging::sLogMessage()
{
}

inline LogMessage::LogMessage(NELogging::eLogMessageType /*msgType*/, unsigned int /*scopeId*/, unsigned int /*sessionId*/, NELogging::eLogPriority /*msgPrio*/, const String& /*message*/)
    : NELogging::sLogMessage()
{
}

inline LogMessage::LogMessage(NELogging::eLogMessageType /*msgType*/, unsigned int /*scopeId*/, unsigned int /*sessionId*/, NELogging::eLogPriority /*msgPrio*/, const char* /*message*/, unsigned int /*msgLen*/)
    : NELogging::sLogMessage()
{
}

inline LogMessage::LogMessage(const IEInStream& stream)
    : NELogging::sLogMessage()
{
    stream >> static_cast<NELogging::sLogMessage&>(*this);
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGMESSAGE_HPP
