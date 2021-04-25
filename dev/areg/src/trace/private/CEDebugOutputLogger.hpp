#ifndef AREG_TRACE_PRIVATE_CEDEBUGOUTPUTLOGGER_HPP
#define AREG_TRACE_PRIVATE_CEDEBUGOUTPUTLOGGER_HPP
/************************************************************************
 * \file        areg/src/trace/private/CEDebugOutputLogger.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/trace/private/CELoggerBase.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEDebugOutputLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger in output window, normally used in debugging.
 *          The messages are not saved in the file.
 *          At the moment the output logger supports only ASCII messages
 *          and any Unicode character might output wrong.
 **/
class CEDebugOutputLogger   : public    CELoggerBase
                            , private   IEOutStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates logger and sets tracer configuration object,
     *          which contains methods to get property values after
     *          configuring tracer.
     * \param   tracerConfig    The instance tracer configuration object,
     *                          which contains configuration values,
     *                          required by logger during initialization (open)
     *                          and when outputs message.
     **/
    CEDebugOutputLogger( IETraceConfiguration & tracerConfig );

    /**
     * \brief   Destructor
     **/
    virtual ~CEDebugOutputLogger( void );

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// CELoggerBase overrides
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
    virtual unsigned int Write( const unsigned char * buffer, unsigned int size );

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const IEByteBuffer & buffer );

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEString & asciiString );

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEWideString & wideString );

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function is device has caching mechanism
     **/
    virtual void Flush( void );

protected:
    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int GetWritableSize( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating whether the debug output logger is opened or not.
     **/
    bool                mIsOpened;
    /**
     * \brief   The generated ASCII message to output.
     **/
    CEString            mOutputMessageA;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CEDebugOutputLogger( void );
    CEDebugOutputLogger( const CEDebugOutputLogger & );
    const CEDebugOutputLogger & operator = ( const CEDebugOutputLogger & );
};

#endif  // AREG_TRACE_PRIVATE_CEDEBUGOUTPUTLOGGER_HPP
