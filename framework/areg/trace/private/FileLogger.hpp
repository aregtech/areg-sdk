#ifndef AREG_TRACE_PRIVATE_FILELOGGER_HPP
#define AREG_TRACE_PRIVATE_FILELOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/FileLogger.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/LoggerBase.hpp"

#include "areg/base/File.hpp"

//////////////////////////////////////////////////////////////////////////
// DebugOutputLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger to output messages in to the file.
 *          At the moment the output logger supports only ASCII messages
 *          and any Unicode character might output wrong.
 **/
class FileLogger    : public    LoggerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor.
     *          Instantiates logger and sets tracer configuration object,
     *          which contains methods to get property values after
     *          configuring tracer.
     * \param   tracerConfig    The instance tracer configuration object,
     *                          which contains configuration values,
     *                          required by logger during initialization (open)
     *                          and when outputs message.
     **/
    FileLogger( LogConfiguration & tracerConfig );

    /**
     * \brief   Destructor
     **/
    virtual ~FileLogger( void )= default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase interface overrides
/************************************************************************/

    /**
     * \brief   Called to initialize / open logger. If method returns true, 
     *          the trace manager starts to forward messages for logging.
     *          If method returns false, the tracer manager assumes that
     *          logger is not initialized and will not send messages for logging.
     *          Before any message is logger, the logger should be opened.
     * \return  Returns true if logger succeeded initialization (open).
     **/
    virtual bool openLogger( void ) override;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void closeLogger( void ) override;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual bool logMessage( const NETrace::sLogMessage & logMessage ) override;

    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    virtual void flushLogs( void ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The log file object
     **/
    File              mLogFile;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    FileLogger( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( FileLogger );
};

#endif  // AREG_TRACE_PRIVATE_FILELOGGER_HPP
