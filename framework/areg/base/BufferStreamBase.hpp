#ifndef AREG_BASE_BUFFERSTREAMBASE_HPP
#define AREG_BASE_BUFFERSTREAMBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/BufferStreamBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform base class of streaming buffer.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ByteBuffer.hpp"
#include "areg/base/IOStream.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Cursor;

//////////////////////////////////////////////////////////////////////////
// BufferStreamBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for streaming binary data into buffers. Provides abstract interface for data
 *          transfer between threads and processes, with behavior determined by derived classes.
 **/
class AREG_API BufferStreamBase : public    ByteBuffer
                                , public    IOStream
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor. Do not call directly to instantiate object
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the stream with read and write position cursors.
     *
     * \param   readPosition        The cursor object for read position.
     * \param   writePosition       The cursor object for write position.
     **/
    BufferStreamBase( Cursor & readPosition, Cursor & writePosition );
    /**
     * \brief	Destructor
     **/
    virtual ~BufferStreamBase() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// BufferStreamBase interface
/************************************************************************/
    /**
     * \brief   Inserts data at the specified position and returns the number of bytes inserted.
     *
     * \param   buf     The pointer to the data buffer to insert.
     * \param   size    The size in bytes of the data buffer.
     * \param   atPos   The position to insert data; if invalid, appends at the end.
     * \return  Returns the number of bytes inserted.
     **/
    virtual uint32_t insert_at(const uint8_t* buf, uint32_t size, uint32_t atPos);

    /**
     * \brief   Checks if the binary data in two buffers are identical.
     *
     * \param   other       The streaming buffer object to compare against.
     * \return  Returns true if the binary data in both buffers is equal.
     **/
    virtual bool is_equal(const BufferStreamBase &other) const;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief   Writes data from the given buffer to the output stream.
     *
     * \param   buf     The pointer to the data buffer to write.
     * \param   size    The size in bytes of the data buffer.
     * \return  Returns the number of bytes written.
     **/
    uint32_t write( const uint8_t* buf, uint32_t size ) override;

    /**
     * \brief   Writes data from a ByteBuffer object to the output stream.
     *
     * \param   buf     The ByteBuffer object containing data to write.
     * \return  Returns the number of bytes written.
     **/
    uint32_t write( const ByteBuffer & buf) override;

    /**
     * \brief   Writes ASCII string data to the output stream.
     *
     * \param   ascii       The String object containing ASCII data to write.
     * \return  Returns the number of bytes written.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes wide-character string data to the output stream.
     *
     * \param   wide    The WideString object containing data to write.
     * \return  Returns the number of bytes written.
     **/
    uint32_t write( const WideString & wide ) override;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from the input stream and copies it into the given buffer.
     *
     * \param[out]  buf     The pointer to the destination buffer.
     * \param[in]   size    The size in bytes of the available buffer.
     * \return  Returns the number of bytes read.
     **/
    uint32_t read( uint8_t* buf, uint32_t size ) const override;

    /**
     * \brief   Reads data from the input stream and copies it into a ByteBuffer object.
     *
     * \param[out]  buf     The ByteBuffer object to receive the data.
     * \return  Returns the number of bytes read.
     **/
    uint32_t read( ByteBuffer & buf) const override;

    /**
     * \brief   Reads ASCII string data from the input stream.
     *
     * \param[out] ascii       The String object to receive the ASCII data.
     * \return  Returns the number of bytes read.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads wide-character string data from the input stream.
     *
     * \param[out] wide    The WideString object to receive the data.
     * \return  Returns the number of bytes read.
     **/
    uint32_t read( WideString & wide ) const override;

/************************************************************************/
// ByteBuffer overrides
/************************************************************************/
    /**
     * \brief   Reserves buffer space and optionally copies existing data. Returns the available
     *          space for writing.
     *
     * \param   size    The size in bytes to reserve.
     * \param   copy    If true and buffer is valid, copies existing data to the new buffer.
     * \return  Returns the total remaining space available for writing.
     **/
    uint32_t reserve(uint32_t size, bool copy) override;

protected:
/************************************************************************/
// IOStream interface overrides
/************************************************************************/
    /**
     * \brief   Returns the number of bytes available to read from the current position.
     **/
    uint32_t size_readable() const override;

    /**
     * \brief   Returns the number of bytes available to write from the current position.
     **/
    uint32_t size_writable() const override;

    /**
     * \brief   Flushes any cached data to the output stream.
     **/
    void flush() override;

    /**
     * \brief   Resets the read position cursor to the beginning of the buffer.
     **/
    void reset() const override;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Writes data to the buffer and returns the number of bytes written.
     *
     * \param   buf     The pointer to the source data buffer.
     * \param   size    The size in bytes of the data buffer.
     * \return  Returns the number of bytes written.
     **/
    uint32_t write_data(const uint8_t* buf, uint32_t size);

    /**
     * \brief   Reads data from the buffer and returns the number of bytes copied.
     *
     * \param[out]  buf     The pointer to the destination buffer.
     * \param       size    The size in bytes of the available buffer.
     * \return  Returns the number of bytes read.
     **/
    uint32_t read_data(uint8_t* buf, uint32_t size) const;

    /**
     * \brief   Returns a pointer to the buffer at the current read position.
     *
     * \return  Returns a pointer to the readable buffer; nullptr if invalid or at end.
     **/
    const uint8_t * buffer_to_read() const;

    /**
     * \brief   Returns a pointer to the buffer at the current write position.
     *
     * \return  Returns a pointer to the writable buffer; nullptr if invalid or at end.
     **/
    uint8_t * buffer_to_write();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Read position object
     **/
    Cursor &  mReadPosition;
    /**
     * \brief   Write position object
     **/
    Cursor &  mWritePosition;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    BufferStreamBase() = delete;
    AREG_NOCOPY_NOMOVE( BufferStreamBase );
};

} // namespace areg
#endif  // AREG_BASE_BUFFERSTREAMBASE_HPP
