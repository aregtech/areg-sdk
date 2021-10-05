#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogMessage.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
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
     * \param   logType     The log type value to set in message log structure
     **/
    inline explicit LogMessage( NETrace::eLogType logType );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   logType     The log type value to set in message log structure
     * \param   scopeId     The ID of trace scope, which is messaging
     * \param   msgPrio     The priority of message to log
     * \param   message     The text message to log
     **/
    inline LogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const String & message );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   logType     The log type value to set in message log structure.
     * \param   scopeId     The ID of trace scope, which is messaging.
     * \param   msgPrio     The priority of message to log.
     * \param   message     The text message to log.
     * \param   msgLen      The length of the message to log.
     **/
    inline LogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen );

    /**
     * \brief   Initializes message log structure for scope enter or exit event.
     *          The event depends on logging type value
     * \param   logType     The log type value to set in message log structure.
     *                      It is either to enter or exit scope.
     * \param   traceScope  The trace scope object, which contains ID and trace scope name is set as text.
     **/
    LogMessage( NETrace::eLogType logType, const TraceScope & traceScope );

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

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copies data from given source. 
     * \param   src     The source of data to copy.
     **/
    LogMessage & operator = ( const LogMessage & src ) = default;

    /**
     * \brief   Serialization operator. Initializes log message data from given stream
     * \param   stream  The streaming object, which contains log message data
     * \param   input   The log message object. On output, it will contain log message data,
     *                  which read from given stream.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, LogMessage & input );

    /**
     * \brief   Serialization operator. Serialize log message data to given stream
     * \param   stream  The streaming object, where the log message data will be serialized.
     * \param   output  The log message object to serialize to given stream.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const LogMessage & output );

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
    inline NETrace::eLogType getLogType( void ) const;

    /**
     * \brief   Return the ID of thread where the message was initialized
     **/
    inline id_type getThreadId( void ) const;

    /**
     * \brief   Returns the ID of trace scope, which was logging message
     **/
    inline unsigned int getScopeId( void ) const;

    /**
     * \brief   Returns the timestamp of created message log
     **/
    inline const DateTime getTimestamp( void ) const;

    /**
     * \brief   Returns message log module ID. By default, it is process ID.
     *          The module ID is an unique identifier set by system.
     **/
    inline ITEM_ID getModuleId( void ) const;

    /**
     * \brief   Set the module ID. The module ID can be any value
     *          which is unique within certain system.
     *          For example, within one host PC, this can be unique
     *          process ID. Within Network, this can be identifier
     *          value, unique in Network.
     *          The Module ID is used to differentiate same scopes
     *          in different processes / machines.
     **/
    inline void setModuleId( ITEM_ID moduleId );

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

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    LogMessage( void ) = delete;
    DECLARE_NOMOVE( LogMessage );
};

//////////////////////////////////////////////////////////////////////////
// LogMessage class inline methods
//////////////////////////////////////////////////////////////////////////
inline LogMessage::LogMessage( NETrace::eLogType logType )
    : NETrace::sLogMessage( logType )
{
}

inline LogMessage::LogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const String & message )
    : NETrace::sLogMessage( logType, scopeId, msgPrio, message.getString(), message.getLength() )
{
}

inline LogMessage::LogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message, unsigned int msgLen )
    : NETrace::sLogMessage( logType, scopeId, msgPrio, message, msgLen )
{
}

inline LogMessage::LogMessage( const IEInStream & stream )
    : NETrace::sLogMessage( )
{
    stream >> static_cast<NETrace::sLogMessage &>(*this);
}

inline const NETrace::sLogMessage & LogMessage::getLogData(void) const
{
    return (*this);
}

inline NETrace::eLogType LogMessage::getLogType(void) const
{
    return lmHeader.logType;
}

inline id_type LogMessage::getThreadId(void) const
{
    return lmTrace.traceThreadId;
}

inline unsigned int LogMessage::getScopeId(void) const
{
    return lmTrace.traceScopeId;
}

inline const DateTime LogMessage::getTimestamp(void) const
{
    return static_cast<DateTime>(lmTrace.traceTimestamp);
}

inline ITEM_ID LogMessage::getModuleId(void) const
{
    return lmHeader.logModuleId;
}

inline void LogMessage::setModuleId(ITEM_ID moduleId)
{
    lmHeader.logModuleId = moduleId;
}

inline NETrace::eLogPriority LogMessage::getMessagePrio(void) const
{
    return lmTrace.traceMessagePrio;
}

inline void LogMessage::setMessagePrio(const NETrace::eLogPriority msgPrio)
{
    lmTrace.traceMessagePrio = msgPrio;
}

inline const char * LogMessage::getMessage(void) const
{
    return lmTrace.traceMessage;
}

inline const IEInStream & operator >> ( const IEInStream & stream, LogMessage & input )
{
    stream >> static_cast<NETrace::sLogMessage &>(input);
    return stream;
}

inline IEOutStream & operator << ( IEOutStream & stream, const LogMessage & output )
{
    stream << static_cast<const NETrace::sLogMessage &>(output);
    return stream;
}
