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
unsigned int BufferStreamBase::read( unsigned char* buffer, unsigned int size ) const
{
    return readData(buffer, size);
}

/**
 * \brief   Read data from input stream object, copies into give Byte Buffer object and returns the size of copied data.
 **/
unsigned int BufferStreamBase::read( ByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buffer))
    {
        buffer.invalidate();
        unsigned int length = 0;

        if (read(reinterpret_cast<unsigned char *>(&length), sizeof(unsigned int)) == sizeof(unsigned int))
        {
            length = buffer.reserve(length, false);
            if (length != 0)
            {
                unsigned char* data = buffer.getBuffer();
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
unsigned int BufferStreamBase::read( String & ascii ) const
{
    unsigned int result = 0;
    ascii.clear();

    const unsigned int curPos = mReadPosition.getPosition();
    const unsigned char* data = getBufferToRead();
    if ( data != nullptr )
    {
        ascii.assign(reinterpret_cast<const char*>(data));
        result = ascii.getSpace();
        mReadPosition.setPosition(static_cast<int>(curPos + result), Cursor::eCursorPosition::PositionBegin);
    }

    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given Wide String and returns the size of copied data.
 **/
unsigned int BufferStreamBase::read( WideString & wide ) const
{
    unsigned int result = 0;
    wide.clear();

    const unsigned int curPos = mReadPosition.getPosition();
    const short * data = reinterpret_cast<const short *>( getBufferToRead() );
    if ( data != nullptr )
    {
        wide.assign(reinterpret_cast<const wchar_t *>(data));
        result = wide.getSpace();
        mReadPosition.setPosition(static_cast<int>(curPos + result), Cursor::eCursorPosition::PositionBegin);
    }

    return result;
}

/**
 * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable size.
 **/
unsigned int BufferStreamBase::getSizeReadable() const
{
    unsigned int lenUsed = getSizeUsed();
    unsigned int lenRead = mReadPosition.getPosition();
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

/**
 * \brief   Write data to output stream object from given buffer and returns the size of written data.
 **/
unsigned int BufferStreamBase::write( const unsigned char* buffer, unsigned int size )
{
    return writeData(buffer, size);
}

/**
 * \brief   Writes Binary data from Byte Buffer object to output stream object and returns the size of written data.
 **/
unsigned int BufferStreamBase::write( const ByteBuffer & buffer )
{
    unsigned int result = 0;
    if (static_cast<const ByteBuffer *>(this) != static_cast<const ByteBuffer *>(&buffer))
    {
        const unsigned char* data = buffer.getBuffer();
        const unsigned int length = buffer.getSizeUsed();

        if (write(reinterpret_cast<const unsigned char *>(&length), sizeof(unsigned int)) == sizeof(unsigned int))
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
unsigned int BufferStreamBase::write( const String & ascii )
{
    return write( reinterpret_cast<const unsigned char *>(ascii.getString()), ascii.getSpace() );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
unsigned int BufferStreamBase::write( const WideString & wide )
{
    return write(reinterpret_cast<const unsigned char*>(wide.getString()), wide.getSpace());
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
unsigned int BufferStreamBase::getSizeWritable() const
{
    unsigned int result{ 0u };
    if (isValid())
    {
        unsigned int lenWritten{ mWritePosition.getPosition() };
        unsigned int lenAvailable{ getSizeAvailable() };
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
    mReadPosition.setPosition(0, Cursor::eCursorPosition::PositionBegin);
}

/**
 * \brief   Returns true if binary data of 2 buffers are equal.
 **/
bool BufferStreamBase::isEqual( const BufferStreamBase &other ) const
{
    bool result = static_cast<const BufferStreamBase *>(this) == &other;
    if ( (result == false) && (isValid() && other.isValid()))
    {
        unsigned int used = getSizeUsed();
        result = (used == other.getSizeUsed()) && areg::memEqual(getBuffer(), other.getBuffer(), static_cast<uint32_t>(used));
    }

    return result;
}

/**
 * \brief   Inserts buffer of data at the given position.
 **/
unsigned int BufferStreamBase::insertAt( const unsigned char* buffer, unsigned int size, unsigned int atPos )
{
    unsigned int result     = 0;
    if ((size != 0) && (buffer != nullptr))
    {
        unsigned int writePos   = mWritePosition.getPosition();
        if ((isValid() == false) || (atPos >= writePos))
        {
            result = write(buffer, size);
        }
        else
        {
            unsigned int remain = reserve(writePos + size, true);
            if (remain >= size)
            {
                ASSERT(isValid());
                unsigned char *dst      = getBuffer() + atPos;
                unsigned int moveSize   = writePos - atPos;

                areg::memMove( dst + size, dst, moveSize );
                areg::memCopy( dst, size, buffer, size );

                result = size;

                unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
                unsigned int newPos     = writePos + result;
                setSizeUsed( std::max(usedSize, newPos) );
                mWritePosition.setPosition(static_cast<int>(newPos), Cursor::eCursorPosition::PositionBegin);
            }
        }
    }

    return result;
}

/**
 * \brief   Writes data to buffer
 **/
unsigned int BufferStreamBase::writeData(const unsigned char* buffer, unsigned int size)
{
    ASSERT( (buffer != nullptr) || (size == 0) );
    unsigned int result     = 0;
    unsigned int writePos   = isValid() ? mWritePosition.getPosition() : 0;
    unsigned int remain     = reserve(writePos + size, true);

    if ((remain != 0) && (size != 0))
    {
        result = areg::memCopy( getBuffer( ) + writePos, static_cast<uint32_t>(remain), buffer, static_cast<uint32_t>(size) );
        unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
        unsigned int newPos     = writePos + result;
        setSizeUsed( std::max(usedSize, newPos) );
        mWritePosition.setPosition(static_cast<int>(newPos), Cursor::eCursorPosition::PositionBegin);
    }

    return result;
}

/**
 * \brief   Reads data from buffer
 **/
unsigned int BufferStreamBase::readData(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (isValid() && size != 0)
    {
        ASSERT(buffer != nullptr);
        unsigned int remain = getSizeReadable();
        result = std::min(remain, size);

        if (result != 0)
        {
            const unsigned char* src = getBufferToRead();
            areg::memCopy(buffer, static_cast<uint32_t>(size), src, static_cast<uint32_t>(result));
            mReadPosition.setPosition(static_cast<int>(result), Cursor::eCursorPosition::PositionCurrent);
        }
    }

    return result;
}

const unsigned char * BufferStreamBase::getBufferToRead() const
{
    const unsigned char * result = getBuffer();
    if ( result != nullptr )
    {
        unsigned int posRead = mReadPosition.getPosition();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : nullptr;
    }
    return result;
}

unsigned char * BufferStreamBase::getBufferToWrite()
{
    unsigned char * result = getBuffer();
    if ( result != nullptr )
    {
        unsigned int posWrite = mWritePosition.getPosition();
        result = posWrite <= mByteBuffer->bufHeader.biUsed ? result + posWrite : nullptr;
    }
    return result;
}

unsigned int BufferStreamBase::reserve(unsigned int size, bool copy)
{
    unsigned int result = getSizeWritable();
    if ((size == 0u) || (result < size))
    {
        unsigned int curPos = mWritePosition.getPosition();
        result = ByteBuffer::reserve(size, copy);
        if (curPos != Cursor::INVALID_CURSOR_POSITION)
        {
            mWritePosition.setPosition(static_cast<int>(curPos), Cursor::eCursorPosition::PositionBegin);
        }
    }

    return result;
}
