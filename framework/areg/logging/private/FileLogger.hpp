#ifndef AREG_LOGGING_PRIVATE_FILELOGGER_HPP
#define AREG_LOGGING_PRIVATE_FILELOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/FileLogger.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/private/LoggerBase.hpp"

#include "areg/base/File.hpp"

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// FileLogger class declaration
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
     * \brief   Initializes the logger and sets the provided log configuration object,
     *          which supplies methods for accessing property values.
     * \param   logConfig   An instance of the log configuration object containing
     *                      settings for initialization and message output.
     **/
    explicit FileLogger( LogConfiguration & logConfig );

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
     * \brief   Initializes and opens the logger. If this method returns true,
     *          the log manager will start forwarding messages for logging.
     *          If it returns false, the log manager assumes the logger is not
     *          initialized and will not send messages for logging.
     *          The logger must be opened before any messages can be logged.
     * \return  Returns true if the logger was successfully initialized and opened.
     **/
    virtual bool openLogger(void) override;

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void closeLogger( void ) override;

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual void logMessage( const NELogging::sLogMessage & logMessage ) override;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool isLoggerOpened( void ) const override;

public:
    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    void flushLogs(void);

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

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_FILELOGGER_HPP
