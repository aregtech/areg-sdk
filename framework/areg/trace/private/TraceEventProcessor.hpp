#ifndef AREG_TRACE_PRIVATE_TRACEEVENTPROCESSOR_HPP
#define AREG_TRACE_PRIVATE_TRACEEVENTPROCESSOR_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceEventProcessor.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace manager. This is a helper class to process
 *              events received by trace manager.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/TraceEvent.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class TraceManager;
class LogConfiguration;
class LoggerBase;
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// TraceEventProcessor class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is a helper class to process the events received
 *          by TraceManager.
 **/
class TraceEventProcessor
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the logging event processor.
     *          Sets the instance of trace manager.
     **/
    TraceEventProcessor( TraceManager & traceManager );

    ~TraceEventProcessor( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Dispatches and processes the log event.
     * \param   cmdLog  The log command to process.
     * \param   stream  The serialized data of the event.
     **/
    void processLogEvent( TraceEventData::eTraceAction cmdLog, const SharedBuffer & stream );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Logging commands
/************************************************************************/

    /**
     * \brief   Configures the logging
     **/
    void _traceConfigure( const SharedBuffer & data );

    /**
     * \brief   Start the logging.
     **/
    void _traceStartLogs( void );

    /**
     * \brief   Stop logging
     **/
    void _traceStopLogs( void );

    /**
     * \brief   Enables or disables logs.
     **/
    void _traceSetEnableLogs( bool logsEnable );

    /**
     * \brief   Saves log configuration in the file.
     **/
    void _traceSaveScopes( void );

    /**
     * \brief   Logs the message.
     **/
    void _traceLogMessage( const SharedBuffer & data );

    /**
     * \brief   Sends connect request message to the remote logging service.
     **/
    void _traceNetConnectService( void );

    /**
     * \brief   Sends disconnect request message to the remote logging service.
     **/
    void _traceNetDisconnectService( void );

    /**
     * \brief   Sends connection lost event to reconnect again.
     **/
    void _traceNetConnectionLost( void );

    /**
     * \brief   Sends message to register logs at remote logging service.
     **/
    void _traceNetRegisterScopes( void );

    /**
     * \brief   Sends empty list of scopes and scope 'set' action to reset the list of scopes on the service side.
     **/
    bool _traceNetRegisterScopesStart( void );

    /**
     * \brief   Sends empty list of scopes and scope 'no action' action to notify the end of scope list.
     **/
    bool _traceNetRegisterScopesEnd( void );

    /**
     * \brief   Received data from remote logging service, which should be processed.
     * \param   stream  The data, which first is converted into type NETrace::sLogCommand
     *                  to figure out the information of 'NETrace::eLogCommands'.
     **/
    void _traceNetReceivedData( const SharedBuffer & stream );

    /**
     * \brief   Called when receive the response from remote logging service.
     * \param   cmdData     The command data received from remote logging service.
     * \param   stream      The streaming object where the data is serialized.
     *                      Depending on the command, the stream object may contain
     *                      additional information or no information at all.
     **/
    void _traceNetResponseData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & stream );

    /**
     * \brief   Called when receive the notification from remote logging service.
     * \param   cmdData     The command data received from remote logging service.
     * \param   stream      The streaming object where the data is serialized.
     *                      Depending on the command, the stream object may contain
     *                      additional information or no information at all.
     **/
    void _traceNetNotifyData( const NETrace::sLogCommandData & cmdData, const SharedBuffer & stream );

    /**
     * \brief   Changes the priority of the scopes. The streaming object contains the list of scopes
     *          with ID and priority to change. Each scope entry can be either a single scope
     *          or scope group. In case of scope group, the scope ID is ignored (should be 0).
     * \param   stream      The buffer with scope name, scope ID and scope priority.
     * \param   scopeCount  The number of scopes in the binary buffer.
     **/
    void _changeScopePriority( const SharedBuffer & stream, unsigned int scopeCount );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    //!< The instance of trace manager.
    TraceManager &      mTraceManager;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TraceEventProcessor( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TraceEventProcessor );
};

#endif  // AREG_TRACE_PRIVATE_TRACEEVENTPROCESSOR_HPP
