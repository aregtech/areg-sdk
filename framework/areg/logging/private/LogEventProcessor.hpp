#ifndef AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
#define AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogEventProcessor.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform. Helper class to process events received by log manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/logging/NELogging.hpp"
#include "areg/logging/private/LoggingEvent.hpp"

#if AREG_LOGS
/************************************************************************
 * Dependencies
 ************************************************************************/
class LogManager;
class LogConfiguration;
class LoggerBase;
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// LogEventProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is a helper class to process the events received
 *          by LogManager.
 **/
class LogEventProcessor
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the logging event processor and set the instance of log manager.
     **/
    LogEventProcessor( LogManager & logManager );

    ~LogEventProcessor( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Dispatches and processes the log event.
     * \param   cmdLog  The log command to process.
     * \param   stream  The serialized data of the event.
     **/
    void processLogEvent( LoggingEventData::eLoggingAction cmdLog, const SharedBuffer & stream );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Logging commands
/************************************************************************/

    /**
     * \brief   Start the logging.
     **/
    void _loggingStartLogs( void );

    /**
     * \brief   Stop logging
     **/
    void _loggingStopLogs( void );

    /**
     * \brief   Enables or disables logs.
     **/
    void _loggingSetEnableLogs( bool logsEnable );

    /**
     * \brief   Saves log configuration in the file.
     **/
    void _loggingSaveScopes( void );

    /**
     * \brief   Logs the message.
     **/
    void _loggingLogMessage( const SharedBuffer & data );

    /**
     * \brief   Changes the priority of the scopes. The streaming object contains the list of scopes
     *          with priority to change. Each scope entry can be either a single scope
     *          or scope group. In case of scope group, the scope ID is ignored (should be 0).
     * \param   stream      The buffer with scope name, scope ID and scope priority.
     * \param   scopeCount  The number of scopes in the binary buffer.
     **/
    void _changeScopePriority( const SharedBuffer & stream, unsigned int scopeCount );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    //!< The instance of log manager.
    LogManager &      mLogManager;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LogEventProcessor( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LogEventProcessor );
};

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
