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
uint32_t BufferStreamBase::read( uint8_t* buffer, uint32_t size ) const
{
    return readData(buffer, size);
}

/**
 * \brief   Read data from input stream object, copies into give Byte Buffer object and returns the size of copied data.
 **/
uint32_t BufferStreamBase::read( ByteBuffer & buffer ) const
{
    uint32_t result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buffer))
    {
        buffer.invalidate();
        uint32_t length = 0;

        if (read(reinterpret_cast<uint8_t *>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
        {
            length = buffer.reserve(length, false);
            if (length != 0)
            {
                uint8_t* data = buffer.getBuffer();
                result = read(data, data != nullptr ? length : 0);
                buffer.getByteBuffer()->bufHeader.biUsed = result > 0 ? result : 0;
            }
        }
    }
    else
    {
        result = getSizeUsed();
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

    const uint32_t curPos = mReadPosition.getPosition();
    const uint8_t* data = getBufferToRead();
    if ( data != nullptr )
    {
        ascii.assign(reinterpret_cast<const char*>(data));
        result = ascii.getSpace();
        mReadPosition.setPosition(static_cast<int32_t>(curPos + result), Cursor::SeekOrigin::Begin);
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

    const uint32_t curPos = mReadPosition.getPosition();
    const int16_t * data = reinterpret_cast<const int16_t *>( getBufferToRead() );
    if ( data != nullptr )
    {
        wide.assign(reinterpret_cast<const wchar_t *>(data));
        result = wide.getSpace();
        mReadPosition.setPosition(static_cast<int32_t>(curPos + result), Cursor::SeekOrigin::Begin);
    }

    return result;
}

/**
 * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable size.
 **/
uint32_t BufferStreamBase::getSizeReadable() const
{
    uint32_t lenUsed = getSizeUsed();
    uint32_t lenRead = mReadPosition.getPosition();
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

/**
 * \brief   Write data to output stream object from given buffer and returns the size of written data.
 **/
uint32_t BufferStreamBase::write( const uint8_t* buffer, uint32_t size )
{
    return writeData(buffer, size);
}

/**
 * \brief   Writes Binary data from Byte Buffer object to output stream object and returns the size of written data.
 **/
uint32_t BufferStreamBase::write( const ByteBuffer & buffer )
{
    uint32_t result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buffer))
    {
        const uint8_t* data = buffer.getBuffer();
        const uint32_t length = buffer.getSizeUsed();

        if (write(reinterpret_cast<const uint8_t *>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
        {
            result = write( data, length );
        }
    }
    else
    {
        result = getSizeUsed(); // if same object, just imitate that the complete data was written.
    }
    return result;
}

/**
 * \brief   Writes string data from given ASCII String object to output stream object.
 **/
uint32_t BufferStreamBase::write( const String & ascii )
{
    return write( reinterpret_cast<const uint8_t *>(ascii.getString()), ascii.getSpace() );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
uint32_t BufferStreamBase::write( const WideString & wide )
{
    return write(reinterpret_cast<const uint8_t*>(wide.getString()), wide.getSpace());
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
uint32_t BufferStreamBase::getSizeWritable() const
{
    uint32_t result{ 0u };
    if (isValid())
    {
        uint32_t lenWritten{ mWritePosition.getPosition() };
        uint32_t lenAvailable{ getSizeAvailable() };
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

void BufferStreamBase::resetCursor() const
{
    mReadPosition.setPosition(0, Cursor::SeekOrigin::Begin);
}

/**
 * \brief   Returns true if binary data of 2 buffers are equal.
 **/
bool BufferStreamBase::isEqual( const BufferStreamBase &other ) const
{
    bool result = static_cast<const BufferStreamBase *>(this) == &other;
    if ( (result == false) && (isValid() && other.isValid()))
    {
        uint32_t used = getSizeUsed();
        result = (used == other.getSizeUsed()) && NEMemory::memEqual(getBuffer(), other.getBuffer(), static_cast<uint32_t>(used));
    }

    return result;
}

/**
 * \brief   Inserts buffer of data at the given position.
 **/
uint32_t BufferStreamBase::insertAt( const uint8_t* buffer, uint32_t size, uint32_t atPos )
{
    uint32_t result     = 0;
    if ((size != 0) && (buffer != nullptr))
    {
        uint32_t writePos   = mWritePosition.getPosition();
        if ((isValid() == false) || (atPos >= writePos))
        {
            result = write(buffer, size);
        }
        else
        {
            uint32_t remain = reserve(writePos + size, true);
            if (remain >= size)
            {
                ASSERT(isValid());
                uint8_t *dst      = getBuffer() + atPos;
                uint32_t moveSize   = writePos - atPos;

                NEMemory::memMove( dst + size, dst, moveSize );
                NEMemory::memCopy( dst, size, buffer, size );

                result = size;

                uint32_t usedSize   = mByteBuffer->bufHeader.biUsed;
                uint32_t newPos     = writePos + result;
                setSizeUsed( std::max(usedSize, newPos) );
                mWritePosition.setPosition(static_cast<int32_t>(newPos), Cursor::SeekOrigin::Begin);
            }
        }
    }

    return result;
}

/**
 * \brief   Writes data to buffer
 **/
uint32_t BufferStreamBase::writeData(const uint8_t* buffer, uint32_t size)
{
    ASSERT( (buffer != nullptr) || (size == 0) );
    uint32_t result     = 0;
    uint32_t writePos   = isValid() ? mWritePosition.getPosition() : 0;
    uint32_t remain     = reserve(writePos + size, true);

    if ((remain != 0) && (size != 0))
    {
        result = NEMemory::memCopy( getBuffer( ) + writePos, static_cast<uint32_t>(remain), buffer, static_cast<uint32_t>(size) );
        uint32_t usedSize   = mByteBuffer->bufHeader.biUsed;
        uint32_t newPos     = writePos + result;
        setSizeUsed( std::max(usedSize, newPos) );
        mWritePosition.setPosition(static_cast<int32_t>(newPos), Cursor::SeekOrigin::Begin);
    }

    return result;
}

/**
 * \brief   Reads data from buffer
 **/
uint32_t BufferStreamBase::readData(uint8_t* buffer, uint32_t size) const
{
    uint32_t result = 0;
    if (isValid() && size != 0)
    {
        ASSERT(buffer != nullptr);
        uint32_t remain = getSizeReadable();
        result = std::min(remain, size);

        if (result != 0)
        {
            const uint8_t* src = getBufferToRead();
            NEMemory::memCopy(buffer, static_cast<uint32_t>(size), src, static_cast<uint32_t>(result));
            mReadPosition.setPosition(static_cast<int32_t>(result), Cursor::SeekOrigin::Current);
        }
    }

    return result;
}

const uint8_t * BufferStreamBase::getBufferToRead() const
{
    const uint8_t * result = getBuffer();
    if ( result != nullptr )
    {
        uint32_t posRead = mReadPosition.getPosition();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : nullptr;
    }
    return result;
}

uint8_t * BufferStreamBase::getBufferToWrite()
{
    uint8_t * result = getBuffer();
    if ( result != nullptr )
    {
        uint32_t posWrite = mWritePosition.getPosition();
        result = posWrite <= mByteBuffer->bufHeader.biUsed ? result + posWrite : nullptr;
    }
    return result;
}

uint32_t BufferStreamBase::reserve(uint32_t size, bool copy)
{
    uint32_t result = getSizeWritable();
    if ((size == 0u) || (result < size))
    {
        uint32_t curPos = mWritePosition.getPosition();
        result = ByteBuffer::reserve(size, copy);
        if (curPos != Cursor::INVALID_CURSOR_POSITION)
        {
            mWritePosition.setPosition(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
        }
    }

    return result;
}
