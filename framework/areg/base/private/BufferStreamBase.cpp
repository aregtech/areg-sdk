/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/BufferStreamBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Raw Buffer class
 *
 ************************************************************************/
#include "areg/base/BufferStreamBase.hpp"

#include "areg/base/Cursor.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <algorithm>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// BufferStreamBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

BufferStreamBase::BufferStreamBase( Cursor & readPosition, Cursor & writePosition )
    : ByteBuffer  ( )
    , IOStream    ( )

    , mReadPosition ( readPosition )
    , mWritePosition( writePosition )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Read data from input stream object, copies into given buffer and returns the size of copied data.
 **/
uint32_t BufferStreamBase::read( uint8_t* buf, uint32_t size ) const
{
    return read_data(buf, size);
}

/**
 * \brief   Read data from input stream object, copies into give Byte Buffer object and returns the size of copied data.
 **/
uint32_t BufferStreamBase::read( ByteBuffer & buf) const
{
    uint32_t result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buf))
    {
        buf.invalidate();
        uint32_t length = 0;

        if (read(reinterpret_cast<uint8_t *>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
        {
            length = buf.reserve(length, false);
            if (length != 0)
            {
                uint8_t* data = buf.buffer();
                result = read(data, data != nullptr ? length : 0);
                buf.byte_buffer()->bufHeader.biUsed = result > 0 ? result : 0;
            }
        }
    }
    else
    {
        result = size_used();
    }

    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given ASCII String and returns the size of copied data.
 **/
uint32_t BufferStreamBase::read( String & ascii ) const
{
    uint32_t result = 0;
    ascii.clear();

    const uint32_t curPos = mReadPosition.position();
    const uint8_t* data = buffer_to_read();
    if ( data != nullptr )
    {
        ascii.assign(reinterpret_cast<const char*>(data));
        result = ascii.space();
        mReadPosition.set_position(static_cast<int32_t>(curPos + result), Cursor::SeekOrigin::Begin);
    }

    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given Wide String and returns the size of copied data.
 **/
uint32_t BufferStreamBase::read( WideString & wide ) const
{
    uint32_t result = 0;
    wide.clear();

    const uint32_t curPos = mReadPosition.position();
    const int16_t * data = reinterpret_cast<const int16_t *>( buffer_to_read() );
    if ( data != nullptr )
    {
        wide.assign(reinterpret_cast<const wchar_t *>(data));
        result = wide.space();
        mReadPosition.set_position(static_cast<int32_t>(curPos + result), Cursor::SeekOrigin::Begin);
    }

    return result;
}

/**
 * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable size.
 **/
uint32_t BufferStreamBase::size_readable() const
{
    uint32_t lenUsed = size_used();
    uint32_t lenRead = mReadPosition.position();
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

/**
 * \brief   Write data to output stream object from given buffer and returns the size of written data.
 **/
uint32_t BufferStreamBase::write( const uint8_t* buf, uint32_t size )
{
    return write_data(buf, size);
}

/**
 * \brief   Writes Binary data from Byte Buffer object to output stream object and returns the size of written data.
 **/
uint32_t BufferStreamBase::write( const ByteBuffer & buf)
{
    uint32_t result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buf))
    {
        const uint8_t* data = buf.buffer();
        const uint32_t length = buf.size_used();

        if (write(reinterpret_cast<const uint8_t *>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
        {
            result = write( data, length );
        }
    }
    else
    {
        result = size_used(); // if same object, just imitate that the complete data was written.
    }
    return result;
}

/**
 * \brief   Writes string data from given ASCII String object to output stream object.
 **/
uint32_t BufferStreamBase::write( const String & ascii )
{
    return write( reinterpret_cast<const uint8_t *>(ascii.as_string()), ascii.space() );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
uint32_t BufferStreamBase::write( const WideString & wide )
{
    return write(reinterpret_cast<const uint8_t*>(wide.as_string()), wide.space());
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
uint32_t BufferStreamBase::size_writable() const
{
    uint32_t result{ 0u };
    if (is_valid())
    {
        uint32_t lenWritten{ mWritePosition.position() };
        uint32_t lenAvailable{ size_available() };
        ASSERT(lenWritten <= lenAvailable);
        result = lenAvailable - lenWritten;
    }

    return result;
}

/**
 * \brief   Flushes cached data to output stream object.
 **/
void BufferStreamBase::flush()
{
}

void BufferStreamBase::reset() const
{
    mReadPosition.set_position(0, Cursor::SeekOrigin::Begin);
}

/**
 * \brief   Returns true if binary data of 2 buffers are equal.
 **/
bool BufferStreamBase::is_equal( const BufferStreamBase &other ) const
{
    bool result = static_cast<const BufferStreamBase *>(this) == &other;
    if ( (result == false) && (is_valid() && other.is_valid()))
    {
        uint32_t used = size_used();
        result = (used == other.size_used()) && NEMemory::mem_equal(buffer(), other.buffer(), static_cast<uint32_t>(used));
    }

    return result;
}

/**
 * \brief   Inserts buffer of data at the given position.
 **/
uint32_t BufferStreamBase::insert_at( const uint8_t* buf, uint32_t size, uint32_t atPos )
{
    uint32_t result     = 0;
    if ((size != 0) && (buf != nullptr))
    {
        uint32_t writePos   = mWritePosition.position();
        if ((is_valid() == false) || (atPos >= writePos))
        {
            result = write(buf, size);
        }
        else
        {
            uint32_t remain = reserve(writePos + size, true);
            if (remain >= size)
            {
                ASSERT(is_valid());
                uint8_t *dst      = buffer() + atPos;
                uint32_t moveSize   = writePos - atPos;

                NEMemory::mem_move( dst + size, dst, moveSize );
                NEMemory::mem_copy( dst, size, buf, size );

                result = size;

                uint32_t usedSize   = mByteBuffer->bufHeader.biUsed;
                uint32_t newPos     = writePos + result;
                set_size_used( std::max(usedSize, newPos) );
                mWritePosition.set_position(static_cast<int32_t>(newPos), Cursor::SeekOrigin::Begin);
            }
        }
    }

    return result;
}

/**
 * \brief   Writes data to buffer
 **/
uint32_t BufferStreamBase::write_data(const uint8_t* buf, uint32_t size)
{
    ASSERT( (buf != nullptr) || (size == 0) );
    uint32_t result     = 0;
    uint32_t writePos   = is_valid() ? mWritePosition.position() : 0;
    uint32_t remain     = reserve(writePos + size, true);

    if ((remain != 0) && (size != 0))
    {
        result = NEMemory::mem_copy( buffer( ) + writePos, static_cast<uint32_t>(remain), buf, static_cast<uint32_t>(size) );
        uint32_t usedSize   = mByteBuffer->bufHeader.biUsed;
        uint32_t newPos     = writePos + result;
        set_size_used( std::max(usedSize, newPos) );
        mWritePosition.set_position(static_cast<int32_t>(newPos), Cursor::SeekOrigin::Begin);
    }

    return result;
}

/**
 * \brief   Reads data from buffer
 **/
uint32_t BufferStreamBase::read_data(uint8_t* buf, uint32_t size) const
{
    uint32_t result = 0;
    if (is_valid() && size != 0)
    {
        ASSERT(buf != nullptr);
        uint32_t remain = size_readable();
        result = std::min(remain, size);

        if (result != 0)
        {
            const uint8_t* src = buffer_to_read();
            NEMemory::mem_copy(buf, static_cast<uint32_t>(size), src, static_cast<uint32_t>(result));
            mReadPosition.set_position(static_cast<int32_t>(result), Cursor::SeekOrigin::Current);
        }
    }

    return result;
}

const uint8_t * BufferStreamBase::buffer_to_read() const
{
    const uint8_t * result = buffer();
    if ( result != nullptr )
    {
        uint32_t posRead = mReadPosition.position();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : nullptr;
    }
    return result;
}

uint8_t * BufferStreamBase::buffer_to_write()
{
    uint8_t * result = buffer();
    if ( result != nullptr )
    {
        uint32_t posWrite = mWritePosition.position();
        result = posWrite <= mByteBuffer->bufHeader.biUsed ? result + posWrite : nullptr;
    }
    return result;
}

uint32_t BufferStreamBase::reserve(uint32_t size, bool copy)
{
    uint32_t result = size_writable();
    if ((size == 0u) || (result < size))
    {
        uint32_t curPos = mWritePosition.position();
        result = ByteBuffer::reserve(size, copy);
        if (curPos != Cursor::INVALID_CURSOR_POSITION)
        {
            mWritePosition.set_position(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
        }
    }

    return result;
}
