#ifndef AREG_TRACE_PRIVATE_CEFILELOGGER_HPP
#define AREG_TRACE_PRIVATE_CEFILELOGGER_HPP
/************************************************************************
 * \file        areg/trace/private/CEFileLogger.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/CELoggerBase.hpp"

#include "areg/base/CEFile.hpp"

//////////////////////////////////////////////////////////////////////////
// CEDebugOutputLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger to output messages in to the file.
 *          At the moment the output logger supports only ASCII messages
 *          and any Unicode character might output wrong.
 **/
class CEFileLogger  : public    CELoggerBase
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
    CEFileLogger( IETraceConfiguration & tracerConfig );

    /**
     * \brief   Destructor
     **/
    virtual ~CEFileLogger( void );

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// CELoggerBase interface overrides
/************************************************************************/

    /**
     * \brief   Called to initialize / open logger. If method returns true, 
     *          the trace manager starts to forward messages for logging.
     *          If method returns false, the tracer manager assumes that
     *          logger is not initialized and will not send messages for logging.
     *          Before any message is logger, the logger should be opened.
     * \return  Returns true if logger succeeded initialization (open).
     **/
    virtual bool OpenLogger( void );

    /**
     * \brief   Called to close logger and stop logging.
     **/
    virtual void CloseLogger( void );

    /**
     * \brief   Called when message should be logged.
     *          Every logger should implement method to process logger specific logging.
     **/
    virtual bool LogMessage( const NETrace::sLogMessage & logMessage );

    /**
     * \brief   Call to flush logs, if they are queued. Some loggers might ignore this.
     **/
    virtual void FlushLogs( void );

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    virtual bool IsLoggerOpened( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The log file object
     **/
    CEFile              mLogFile;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEFileLogger( void );
    CEFileLogger( const CEFileLogger & );
    const CEFileLogger & operator = ( const CEFileLogger & );
};

#endif  // AREG_TRACE_PRIVATE_CEFILELOGGER_HPP
