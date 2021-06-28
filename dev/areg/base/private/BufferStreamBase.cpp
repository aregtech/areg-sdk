/************************************************************************
 * \file        areg/base/private/BufferStreamBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Raw Buffer class
 *
 ************************************************************************/
#include "areg/base/BufferStreamBase.hpp"

#include "areg/base/IECursorPosition.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// BufferStreamBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

BufferStreamBase::BufferStreamBase( NEMemory::sByteBuffer & byteBuffer, IECursorPosition & readPosition, IECursorPosition & writePosition )
    : IEByteBuffer     ( byteBuffer )
    , IEIOStream       ( )

    , mReadPosition     ( readPosition )
    , mWritePosition    ( writePosition )
{
    ; // do nothing
}

BufferStreamBase::~BufferStreamBase( void )
{
    ; // do nothing
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
            length = buffer.resize(length, false);
            if (length != 0)
            {
                unsigned char* data = buffer.getBuffer();
                result = read(data, data != NULL ? length : 0);
                buffer.getByteBuffer().bufHeader.biUsed = result > 0 ? result : 0;
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
    if ( data != NULL )
    {
        asciiString = reinterpret_cast<const char *>(data);
        result      = (asciiString.getLength() + 1) * sizeof(char);
        mReadPosition.setPosition(curPos + result, IECursorPosition::POSITION_BEGIN);
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
    const unsigned char* data = getBufferToRead();
    if ( data != NULL )
    {
        wideString  = reinterpret_cast<const wchar_t *>(data);
        result      = (wideString.getLength() + 1) * sizeof(wchar_t);
        mReadPosition.setPosition(curPos + result, IECursorPosition::POSITION_BEGIN);
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
            result = write(data, length);
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
    const char * buffer = asciiString.getString();
    buffer = buffer != NULL_STRING ? buffer : String::EmptyString;
    unsigned int len = asciiString.getLength() + 1;

    return write( reinterpret_cast<const unsigned char *>(buffer), len * sizeof(char) );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
unsigned int BufferStreamBase::write( const WideString & wideString )
{
    const wchar_t * buffer = wideString.getString();
    buffer = buffer != NULL_STRING_W ? buffer : WideString::EmptyString;
    unsigned int len = wideString.getLength() + 1;

    return write( reinterpret_cast<const unsigned char *>(buffer), len * sizeof(wchar_t) );
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
unsigned int BufferStreamBase::getSizeWritable( void ) const
{
    unsigned int lenWritten     = mWritePosition.getPosition();
    unsigned int lenAvailable   = getSizeAvailable();
    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

/**
 * \brief   Flushes cached data to output stream object.
 **/
void BufferStreamBase::flush( void )
{
    ; // do nothing
}

void BufferStreamBase::resetCursor(void) const
{
    mReadPosition.setPosition(0, IECursorPosition::POSITION_BEGIN);
}

/**
 * \brief   Returns true if binary data of 2 buffers are equal.
 **/
bool BufferStreamBase::isEqual( const BufferStreamBase &other ) const
{
    bool result = static_cast<const BufferStreamBase *>(this) == &other;
    if ( result == false && this->isValid() && other.isValid())
    {
        result = getSizeUsed() == other.getSizeUsed() ? NEMemory::isEqualBuffer<unsigned char>(getBuffer(), other.getBuffer(), getSizeUsed()) : false;
    }

    return result;
}

/**
 * \brief   Inserts buffer of data at the given position.
 **/
unsigned int BufferStreamBase::insertAt( const unsigned char* buffer, unsigned int size, unsigned int atPos )
{
    unsigned int result     = 0;
    if (size > 0 && buffer != NULL)
    {
        unsigned int writePos   = mWritePosition.getPosition();
        if ((isValid() == false) || (atPos >= writePos))
        {
            result = write(buffer, size);
        }
        else
        {
            unsigned int remain = resize(writePos + size, true);
            if (remain >= size)
            {
                ASSERT(isValid());
                unsigned char *dst      = getBuffer() + atPos;
                unsigned int moveSize   = writePos - atPos;

                NEMemory::memMove(dst + size, dst, moveSize);
                result = NEMemory::memCopy(dst, size, buffer, size);

                unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
                unsigned int newPos     = writePos + result;
                setSizeUsed( MACRO_MAX(usedSize, newPos) );
                mWritePosition.setPosition(static_cast<int>(newPos), IECursorPosition::POSITION_BEGIN);
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
    ASSERT( (buffer != NULL) || (size == 0) );
    unsigned int result     = 0;
    unsigned int writePos   = isValid() ? mWritePosition.getPosition() : 0;
    unsigned int remain     = resize(writePos + size, writePos != 0);

    result = MACRO_MIN(remain, size);
    if (result != 0)
    {
        ASSERT(isValid());

        NEMemory::memCopy(getBuffer() + writePos, remain, buffer, result);
        unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
        unsigned int newPos     = writePos + result;
        setSizeUsed( MACRO_MAX(usedSize, newPos) );
        mWritePosition.setPosition(static_cast<int>(newPos), IECursorPosition::POSITION_BEGIN);
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
        ASSERT(buffer != NULL);
        unsigned int remain = getSizeReadable();
        result = MACRO_MIN(remain, size);

        if (result != 0)
        {
            const unsigned char* src = getBufferToRead();
            NEMemory::memCopy(buffer, size, src, result);
            mReadPosition.setPosition(static_cast<int>(result), IECursorPosition::POSITION_CURRENT);
        }
    }
    return result;
}

const unsigned char * BufferStreamBase::getBufferToRead(void) const
{
    const unsigned char * result = getBuffer();
    if ( result != NULL )
    {
        unsigned int posRead = mReadPosition.getPosition();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : NULL;
    }
    return result;
}

unsigned char * BufferStreamBase::getBufferToWrite(void)
{
    unsigned char * result = getBuffer();
    if ( result != NULL )
    {
        unsigned int posWrite = mWritePosition.getPosition();
        result = posWrite <= mByteBuffer->bufHeader.biUsed ? result + posWrite : NULL;
    }
    return result;
}

unsigned int BufferStreamBase::resize(unsigned int size, bool copy)
{
    unsigned int result = IEByteBuffer::resize(size, copy);

    if (result != 0)
    {
        ASSERT(isValid());
        mWritePosition.setPosition( mByteBuffer->bufHeader.biUsed, IECursorPosition::POSITION_BEGIN );
    }

    return result;
}
