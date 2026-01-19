#ifndef AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
#define AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/DebugOutputLogger.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/private/LoggerBase.hpp"
#include "areg/base/IEIOStream.hpp"

#include "areg/base/String.hpp"

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// DebugOutputLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger in to log message in output window, normally used in debugging.
 *          At the moment the output logger supports only ASCII messages
 *          and any Unicode character might output wrong.
 **/
class DebugOutputLogger : public    LoggerBase
                        , private   IEOutStream
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
    explicit DebugOutputLogger( LogConfiguration & logConfig);

    /**
     * \brief   Destructor
     **/
    virtual ~DebugOutputLogger( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase overrides
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

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char * buffer, unsigned int size ) override;

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer ) override;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   ascii     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & ascii ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wide  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wide ) override;

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function is device has caching mechanism
     **/
    virtual void flush( void ) override;

protected:
    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating whether the debug output logger is opened or not.
     **/
    bool    mIsOpened;
    /**
     * \brief   The generated ASCII message to output.
     **/
    String  mOutputMessageA;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DebugOutputLogger( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( DebugOutputLogger );
};

#endif // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
