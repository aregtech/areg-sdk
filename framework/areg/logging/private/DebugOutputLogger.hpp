#ifndef AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
#define AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/DebugOutputLogger.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/logging/private/LoggerBase.hpp"
#include "areg/base/IOStream.hpp"

#include "areg/base/String.hpp"

#if AREG_LOGGING

namespace areg {
//////////////////////////////////////////////////////////////////////////
// DebugOutputLogger class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message logger to log messages in the output window, normally used in debugging. At the
 *          moment the output logger supports only ASCII messages and any Unicode character might
 *          output wrong.
 **/
class DebugOutputLogger final   : public    LoggerBase
                                , private   OutStream
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Initializes the logger and sets the provided log configuration object, which
     *          supplies methods for accessing property values.
     *
     * \param   logConfig       An instance of the log configuration object containing settings for
     *                          initialization and message output.
     **/
    explicit DebugOutputLogger( LogConfiguration & logConfig);

    ~DebugOutputLogger() override = default;

//////////////////////////////////////////////////////////////////////////
// Override operations and attribute
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// LoggerBase overrides
/************************************************************************/

    /**
     * \brief   Initializes and opens the logger. If this method returns true, the log manager will
     *          start forwarding messages for logging. If it returns false, the log manager assumes
     *          the logger is not initialized and will not send messages for logging. The logger
     *          must be opened before any messages can be logged.
     *
     * \return  Returns true if the logger was successfully initialized and opened.
     **/
    bool open_logger() final;

    /**
     * \brief   Closes the logger and stops logging.
     **/
    void close_logger() final;

    /**
     * \brief   Called when message should be logged. Every logger should implement method to
     *          process logger specific logging.
     *
     * \param   logMessage     The logging message to process.
     **/
    void log_message( const areg::LogEntry & logMessage) final;

    /**
     * \brief   Returns true if logger is initialized (opened).
     **/
    [[nodiscard]]
    bool is_logger_opened() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// OutStream interface overrides
/************************************************************************/

    /**
     * \brief   Writes data from given buffer to output stream and returns the size of written data.
     *
     * \param   buffer      The pointer to buffer to read data and copy to output stream.
     * \param   size        The size in bytes of data buffer.
     * \return  Returns the size in bytes of written data.
     **/
    uint32_t write( const uint8_t * buffer, uint32_t size ) final;

    /**
     * \brief   Writes binary data from SharedBuffer object to output stream and returns the size of
     *          written data.
     *
     * \param   buffer      The instance of SharedBuffer object containing data to stream to output
     *                      stream.
     * \return  Returns the size in bytes of written data.
     **/
    uint32_t write( const SharedBuffer& buffer ) final;

    /**
     * \brief   Writes ASCII string data to output stream.
     *
     * \param   ascii       The string containing data to stream to output stream.
     * \return  Returns the size in bytes of written string data.
     **/
    uint32_t write( const String & ascii ) final;

    /**
     * \brief   Writes wide-character string data to output stream.
     *
     * \param   wide    The wide string containing data to stream to output stream.
     * \return  Returns the size in bytes of written string data.
     * \note    Move overload. Takes ownership of the string.
     **/
    uint32_t write( const WideString & wide ) final;

    /**
     * \brief   Flushes cached data to output stream. Implement if device has caching mechanism.
     **/
    void flush() noexcept final;

protected:
    /**
     * \brief   Returns size in bytes of available space that can be written, i.e. remaining
     *          writable size. No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was already written
     *          to stream, the available writable size is 'n - x'.
     **/
    uint32_t size_writable() const noexcept final;

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
    DebugOutputLogger() = delete;
    AREG_NOCOPY_NOMOVE( DebugOutputLogger );
};

} // namespace areg

#endif // AREG_LOGGING

#endif  // AREG_LOGGING_PRIVATE_DEBUGOUTPUTLOGGER_HPP
