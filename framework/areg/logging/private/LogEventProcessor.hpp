#ifndef AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
#define AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogEventProcessor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Helper class to process events received by log manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/private/LoggingEvent.hpp"

#if AREG_LOGGING
/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class LogManager;
    class LogConfiguration;
    class LoggerBase;
    class InStream;
} // namespace areg

namespace areg {

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
     *
     * \param   logManager      The log manager instance.
     **/
    LogEventProcessor( LogManager & logManager );

    ~LogEventProcessor() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Dispatches and processes the log event.
     *
     * \param   cmdLog      The log command to process.
     * \param   stream      The serialized data of the event.
     **/
    void process_log_event( LoggingEventData::LogAction cmdLog, const SharedBuffer & stream );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Logging commands
/************************************************************************/

    /**
     * \brief   Starts logging.
     **/
    void _logging_start_logs();

    /**
     * \brief   Stops logging.
     **/
    void _logging_stop_logs();

    /**
     * \brief   Enables or disables logs.
     *
     * \param   logsEnable      True to enable logs, false to disable.
     **/
    void _set_logging_enabled( bool logsEnable );

    /**
     * \brief   Saves log configuration in the file.
     **/
    void _logging_save_scopes();

    /**
     * \brief   Processes a log message.
     *
     * \param   data    The message data.
     **/
    void _logging_log_message( const SharedBuffer & data );

    /**
     * \brief   Changes the priority of the scopes. The streaming object contains the list of scopes
     *          with priority to change. Each scope entry can be either a single scope or scope
     *          group. In case of scope group, the scope ID is ignored (should be 0).
     *
     * \param   stream          The buffer with scope name, scope ID and scope priority.
     * \param   scopeCount      The number of scopes in the binary buffer.
     **/
    void _change_scope_priority( const SharedBuffer & stream, uint32_t scopeCount );

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
    LogEventProcessor() = delete;
    AREG_NOCOPY_NOMOVE( LogEventProcessor );
};

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_LOGEVENTPROCESSOR_HPP
