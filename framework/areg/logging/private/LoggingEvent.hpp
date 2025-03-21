#ifndef AREG_LOGGING_PRIVATE_LOGGINGEVENT_HPP
#define AREG_LOGGING_PRIVATE_LOGGINGEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LoggingEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The logging thread, which is receiving logging events and performs log operations. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/SharedBuffer.hpp"

#if AREG_LOGS

/************************************************************************
 * Dependencies
 ************************************************************************/
class LogMessage;
namespace NELogging
{
    struct sLogMessage;
}

//////////////////////////////////////////////////////////////////////////
// LoggingEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The tracing (or logging) event is triggered each time when application
 *          needs a logging action to be performed. The event is forwarded to a logging thread
 *          for further processing, like configuring logging, output message, etc.
 *          The logging event contains data with the instruction to perform certain actions.
 *          This logging event data is automatically generated by system, when a certain API
 *          is called.
 **/
class LoggingEventData
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   LoggingEventData::eLoggingAction
     *          The list of supported actions for logging.
     **/
    typedef enum class E_LoggingAction
    {
          LoggingUndefined      //!< Action is undefined, do nothing
        , LoggingStartLogs      //!< Action to notify to start logging
        , LoggingStopLogs       //!< Action to notify to stop logging
        , LoggingSetEnableLogs  //!< Action to notify to enable logging
        , LoggingSetDisableLogs //!< Action to notify to stop logging
        , LoggingSaveScopes     //!< Action to notify to save scope list
        , LoggingLogMessage     //!< Action to output logging message
        , LoggingUpdateScopes   //!< Action to update scope priorities
        , LoggingQueryScopes    //!< Action to send the list of scopes.
    } eLoggingAction;

    /**
     * \brief   Converts and returns the string of LoggingEventData::eLoggingAction value
     **/
    static inline const char * getString( LoggingEventData::eLoggingAction action );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates the logging even data with undefined action
     **/
    LoggingEventData( void );

    /**
     * \brief   Creates the logging even data with specified action
     * \param   action  The action ID to set in event data
     **/
    explicit LoggingEventData( LoggingEventData::eLoggingAction action );
    
    /**
     * \brief   Creates the logging even data with specified action and data
     * \param   action      The action ID to set in event data
     * \param   dataBuffer  The buffer of data set.
     **/
    LoggingEventData( LoggingEventData::eLoggingAction action, const SharedBuffer & dataBuffer );

    /**
     * \brief   Creates the logging even data with specified action and logging message data
     * \param   action  The action ID to set in event data
     * \param   logData The buffer of logging message data set.
     **/
    LoggingEventData( LoggingEventData::eLoggingAction action, const NELogging::sLogMessage & logData );

    /**
     * \brief   Copies logging event data from given source.
     * \param   src     The source to copy data.
     **/
    LoggingEventData( const LoggingEventData & src );

    /**
     * \brief   Copies logging event data from given source.
     * \param   src     The source to copy data.
     **/
    LoggingEventData( LoggingEventData && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~LoggingEventData( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies data from given source
     * \param   src     The source of data to copy.
     **/
    LoggingEventData & operator = ( const LoggingEventData & src );

    /**
     * \brief   Moves data from given source
     * \param   src     The source of data to move.
     **/
    LoggingEventData & operator = ( LoggingEventData && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the value of action set in event data.
     **/
    inline LoggingEventData::eLoggingAction getLoggingAction( void ) const;

    /**
     * \brief   Returns the streaming buffer for writing.
     **/
    inline SharedBuffer & getWritableStream( void );

    /**
     * \brief   Returns the streaming buffer for reading.
     **/
    inline const SharedBuffer & getReadableStream( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    LoggingEventData::eLoggingAction  mAction;
    SharedBuffer                  mDataBuffer;
};

//////////////////////////////////////////////////////////////////////////
// Tracing Event and Consumer declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Declare LoggingEvent class, derived from TEEvent object,
 *          and IELoggingEventConsumer class, derived from TEEventConsumer object
 **/
DECLARE_EVENT(LoggingEventData, LoggingEvent, IELoggingEventConsumer)

//////////////////////////////////////////////////////////////////////////
// LoggingEventData class inline functions
//////////////////////////////////////////////////////////////////////////

inline LoggingEventData::eLoggingAction LoggingEventData::getLoggingAction( void ) const
{
    return mAction;
}

inline SharedBuffer & LoggingEventData::getWritableStream( void )
{
    return mDataBuffer;
}

inline const SharedBuffer & LoggingEventData::getReadableStream( void ) const
{
    return mDataBuffer;
}

inline const char * LoggingEventData::getString( LoggingEventData::eLoggingAction action )
{
    switch ( action )
    {
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingUndefined);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingStartLogs);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingStopLogs);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingSetEnableLogs);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingSetDisableLogs);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingSaveScopes);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingLogMessage);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingUpdateScopes);
    CASE_MAKE_STRING(LoggingEventData::eLoggingAction::LoggingQueryScopes);
    CASE_DEFAULT("ERR: Undefined LoggingEventData::eLoggingAction value!");
    }
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGGINGEVENT_HPP
