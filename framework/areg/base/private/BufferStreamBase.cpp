/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/BufferStreamBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform Raw Buffer class
 *
 ************************************************************************/
#include "areg/base/BufferStreamBase.hpp"

#include "areg/base/IECursorPosition.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEUtilities.hpp"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
// BufferStreamBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

BufferStreamBase::BufferStreamBase( IECursorPosition & readPosition, IECursorPosition & writePosition )
    : IEByteBuffer  ( )
    , IEIOStream    ( )

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
unsigned int BufferStreamBase::read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (static_cast<const IEByteBuffer *>(this) != static_cast<const IEByteBuffer *>(&buffer))
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
unsigned int BufferStreamBase::read( String & asciiString ) const
{
    unsigned int result = 0;
    asciiString.clear();

    const unsigned int curPos = mReadPosition.getPosition();
    const unsigned char* data = getBufferToRead();
    if ( data != nullptr )
    {
        asciiString.assign(reinterpret_cast<const char*>(data));
        result = asciiString.getSpace();
        mReadPosition.setPosition(static_cast<int>(curPos + result), IECursorPosition::eCursorPosition::PositionBegin);
    }

    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given Wide String and returns the size of copied data.
 **/
unsigned int BufferStreamBase::read( WideString & wideString ) const
{
    unsigned int result = 0;
    wideString.clear();

    const unsigned int curPos = mReadPosition.getPosition();
    const short * data = reinterpret_cast<const short *>( getBufferToRead() );
    if ( data != nullptr )
    {
        wideString.assign(reinterpret_cast<const wchar_t *>(data));
        result = wideString.getSpace();
        mReadPosition.setPosition(static_cast<int>(curPos + result), IECursorPosition::eCursorPosition::PositionBegin);
    }

    return result;
}

/**
 * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable size.
 **/
unsigned int BufferStreamBase::getSizeReadable( void ) const
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
unsigned int BufferStreamBase::write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;
    if (static_cast<const IEByteBuffer *>(this) != static_cast<const IEByteBuffer *>(&buffer))
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
unsigned int BufferStreamBase::write( const String & asciiString )
{
    return write( reinterpret_cast<const unsigned char *>(asciiString.getString()), asciiString.getSpace() );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
unsigned int BufferStreamBase::write( const WideString & wideString )
{
    return write(reinterpret_cast<const unsigned char*>(wideString.getString()), wideString.getSpace());
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
unsigned int BufferStreamBase::getSizeWritable( void ) const
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
void BufferStreamBase::flush( void )
{
}

void BufferStreamBase::resetCursor(void) const
{
    mReadPosition.setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
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
        result = (used == other.getSizeUsed()) && NEMemory::memEqual(getBuffer(), other.getBuffer(), static_cast<int>(used));
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

                NEMemory::memMove( dst + size, dst, moveSize );
                NEMemory::memCopy( dst, size, buffer, size );

                result = size;

                unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
                unsigned int newPos     = writePos + result;
                setSizeUsed( MACRO_MAX(usedSize, newPos) );
                mWritePosition.setPosition(static_cast<int>(newPos), IECursorPosition::eCursorPosition::PositionBegin);
            }
            else
            {
                ; // do nothing, not enough space
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
        result = NEMemory::memCopy( getBuffer( ) + writePos, static_cast<int>(remain), buffer, static_cast<int>(size) );
        unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
        unsigned int newPos     = writePos + result;
        setSizeUsed( MACRO_MAX(usedSize, newPos) );
        mWritePosition.setPosition(static_cast<int>(newPos), IECursorPosition::eCursorPosition::PositionBegin);
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
        result = MACRO_MIN(remain, size);

        if (result != 0)
        {
            const unsigned char* src = getBufferToRead();
            NEMemory::memCopy(buffer, static_cast<int>(size), src, static_cast<int>(result));
            mReadPosition.setPosition(static_cast<int>(result), IECursorPosition::eCursorPosition::PositionCurrent);
        }
    }

    return result;
}

const unsigned char * BufferStreamBase::getBufferToRead(void) const
{
    const unsigned char * result = getBuffer();
    if ( result != nullptr )
    {
        unsigned int posRead = mReadPosition.getPosition();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : nullptr;
    }
    return result;
}

unsigned char * BufferStreamBase::getBufferToWrite(void)
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
        result = IEByteBuffer::reserve(size, copy);
        if (curPos != IECursorPosition::INVALID_CURSOR_POSITION)
        {
            mWritePosition.setPosition(static_cast<int>(curPos), IECursorPosition::eCursorPosition::PositionBegin);
        }
    }

    return result;
}
