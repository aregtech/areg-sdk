#ifndef AREG_TRACE_PRIVATE_CELOGMESSAGE_HPP
#define AREG_TRACE_PRIVATE_CELOGMESSAGE_HPP
/************************************************************************
 * \file        areg/trace/private/CELogMessage.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Logging Message class. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/base/CEString.hpp"
#include "areg/base/CEDateTime.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;
class CETraceScope;

//////////////////////////////////////////////////////////////////////////
// CELogMessage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Message Log class is an extension of Message Log structure.
 *          It is initializing structure of logging message and set values.
 *          The structure can be serialized in the streaming object.
 *          Different types of constructor allow to initialize message log
 *          structure with certain log types and priorities.
 **/
class CELogMessage  : public NETrace::sLogMessage
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class CETraceMessage;    // need to access message log structure

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes message log structure and sets the logging type value.
     * \param   logType     The log type value to set in message log structure
     **/
    inline CELogMessage( NETrace::eLogType logType );

    /**
     * \brief   Initializes message log structure, sets the logging type value,
     *          scope ID, message priority and message text, if the string is not empty.
     * \param   logType     The log type value to set in message log structure
     * \param   scopeId     The ID of trace scope, which is messaging
     * \param   msgPrio     The priority of message to log
     * \param   message     The text message to log
     **/
    inline CELogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message = NULL );

    /**
     * \brief   Initializes message log structure for scope enter or exit event.
     *          The event depends on logging type value
     * \param   logType     The log type value to set in message log structure.
     *                      It is either to enter or exit scope.
     * \param   traceScope  The trace scope object, which contains ID and trace scope name is set as text.
     **/
    CELogMessage( NETrace::eLogType logType, const CETraceScope & traceScope );

    /**
     * \brief   Copies logging message data from given source.
     * \param   source  The source of data to copy.
     **/
    inline CELogMessage( const CELogMessage & source );

    /**
     * \brief   Initialization constructor.
     *          Initializes log message data from stream.
     **/
    inline CELogMessage( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    inline ~CELogMessage(void);

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Serialization operator. Initializes log message data from given stream
     * \param   stream  The streaming object, which contains log message data
     * \param   input   The log message object. On output, it will contain log message data,
     *                  which read from given stream.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CELogMessage & input );

    /**
     * \brief   Serialization operator. Serialize log message data to given stream
     * \param   stream  The streaming object, where the log message data will be serialized.
     * \param   output  The log message object to serialize to given stream.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CELogMessage & output );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns instance of Log message structure
     **/
    inline const NETrace::sLogMessage & GetLogMessage( void ) const;

    /**
     * \brief   Returns log type value
     **/
    inline NETrace::eLogType GetLogType( void ) const;

    /**
     * \brief   Return the ID of thread where the message was initialized
     **/
    inline ITEM_ID GetThreadId( void ) const;

    /**
     * \brief   Returns the ID of trace scope, which was logging message
     **/
    inline unsigned int GetScopeId( void ) const;

    /**
     * \brief   Returns the timestamp of created message log
     **/
    inline const CEDateTime GetTimestamp( void ) const;

    /**
     * \brief   Returns message log module ID. By default, it is process ID.
     *          The module ID is an unique identifier set by system.
     **/
    inline ITEM_ID GetModuleId( void ) const;

    /**
     * \brief   Set the module ID. The module ID can be any value
     *          which is unique within certain system.
     *          For example, within one host PC, this can be unique
     *          process ID. Within Network, this can be identifier
     *          value, unique in Network.
     *          The Module ID is used to differentiate same scopes
     *          in different processes / machines.
     **/
    inline void SetModuleId( ITEM_ID moduleId );

    /**
     * \brief   Returns the priority ob message log
     **/
    inline NETrace::eLogPriority GetMessagePrio( void ) const;

    /**
     * \brief   Sets the priority of message log
     **/
    inline void SetMessagePrio( NETrace::eLogPriority msgPrio );

    /**
     * \brief   Returns message text, if there is any.
     **/
    inline const char * GetMessage( void ) const;

    /**
     * \brief   Sets message text in log
     **/
    void SetMessage( const char * message );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CELogMessage( void );
    const CELogMessage & operator = ( const CELogMessage & );
};

//////////////////////////////////////////////////////////////////////////
// CELogMessage class inline methods
//////////////////////////////////////////////////////////////////////////
inline CELogMessage::CELogMessage( NETrace::eLogType logType )
    : NETrace::sLogMessage( logType )
{   ;   }

inline CELogMessage::CELogMessage( NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message /*= NULL*/ )
    : NETrace::sLogMessage( logType, scopeId, msgPrio, message )
{   ;   }

inline CELogMessage::CELogMessage( const CELogMessage & source )
    : NETrace::sLogMessage( static_cast<const NETrace::sLogMessage &>(source) )
{   ;   }

inline CELogMessage::CELogMessage( const IEInStream & stream )
    : NETrace::sLogMessage( )
{   stream >> static_cast<NETrace::sLogMessage &>(*this);  }

inline CELogMessage::~CELogMessage(void)
{   ;   }

inline const NETrace::sLogMessage & CELogMessage::GetLogMessage(void) const
{   return (*this); }

inline NETrace::eLogType CELogMessage::GetLogType(void) const
{   return lmHeader.logType;            }

inline ITEM_ID CELogMessage::GetThreadId(void) const
{   return lmTrace.traceThreadId;       }

inline unsigned int CELogMessage::GetScopeId(void) const
{   return lmTrace.traceScopeId;        }

inline const CEDateTime CELogMessage::GetTimestamp(void) const
{   return static_cast<CEDateTime>(lmTrace.traceTimestamp); }

inline ITEM_ID CELogMessage::GetModuleId(void) const
{   return lmHeader.logModuleId;        }

inline void CELogMessage::SetModuleId(ITEM_ID moduleId)
{   lmHeader.logModuleId = moduleId;    }

inline NETrace::eLogPriority CELogMessage::GetMessagePrio(void) const
{   return lmTrace.traceMessagePrio;    }

inline void CELogMessage::SetMessagePrio(const NETrace::eLogPriority msgPrio)
{   lmTrace.traceMessagePrio = msgPrio; }

inline const char * CELogMessage::GetMessage(void) const
{   return lmTrace.traceMessage;    }

inline const IEInStream & operator >> ( const IEInStream & stream, CELogMessage & input )
{   stream >> static_cast<NETrace::sLogMessage &>(input); return stream;   }

inline IEOutStream & operator << ( IEOutStream & stream, const CELogMessage & output )
{   stream << static_cast<const NETrace::sLogMessage &>(output); return stream;    }

#endif  // AREG_TRACE_PRIVATE_CELOGMESSAGE_HPP
