/************************************************************************
 * \file        areg/src/base/private/CEBufferStreamBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Raw Buffer class
 *
 ************************************************************************/
#include "areg/src/base/CEBufferStreamBase.hpp"

#include "areg/src/base/IECursorPosition.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// CEBufferStreamBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

CEBufferStreamBase::CEBufferStreamBase( NEMemory::sByteBuffer & byteBuffer, IECursorPosition & readPosition, IECursorPosition & writePosition )
    : IEByteBuffer     ( byteBuffer )
    , IEIOStream       ( )

    , mReadPosition     ( readPosition )
    , mWritePosition    ( writePosition )
{
    ; // do nothing
}

CEBufferStreamBase::~CEBufferStreamBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Read data from input stream object, copies into given buffer and returns the size of copied data.
 **/
unsigned int CEBufferStreamBase::Read( unsigned char* buffer, unsigned int size ) const
{
    return ReadData(buffer, size);
}

/**
 * \brief   Read data from input stream object, copies into give Byte Buffer object and returns the size of copied data.
 **/
unsigned int CEBufferStreamBase::Read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (static_cast<const IEByteBuffer *>(this) != static_cast<const IEByteBuffer *>(&buffer))
    {
        buffer.InvalidateBuffer();
        unsigned int length = 0;

        if (Read(reinterpret_cast<unsigned char *>(&length), sizeof(unsigned int)) == sizeof(unsigned int))
        {
            length = buffer.ResizeBuffer(length, false);
            if (length != 0)
            {
                unsigned char* data = buffer.GetBuffer();
                result = Read(data, data != NULL ? length : 0);
                buffer.GetByteBuffer().bufHeader.biUsed = result > 0 ? result : 0;
            }
        }
    }
    else
    {
        result = GetUsedSize();
    }

    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given ASCII String and returns the size of copied data.
 **/
unsigned int CEBufferStreamBase::Read( CEString & asciiString ) const
{
    unsigned int result = 0;
    asciiString.Clear();
    const unsigned char* data = GetBufferAtReadPosition();
    if ( data != NULL )
    {
        asciiString = reinterpret_cast<const char *>(data);
        result      = asciiString.GetOccupiedBytes();
        mReadPosition.SetPosition(result, IECursorPosition::POSITION_CURRENT);
    }
    return result;
}

/**
 * \brief   Reads string data from Input Stream object, copies into given Wide String and returns the size of copied data.
 **/
unsigned int CEBufferStreamBase::Read( CEWideString & wideString ) const
{
    unsigned int result = 0;
    wideString.Clear();
    const unsigned char* data = GetBufferAtReadPosition();
    if ( data != NULL )
    {
        wideString  = reinterpret_cast<const wchar_t *>(data);
        result      = (wideString.GetLength() + 1) * sizeof(wchar_t);
        mReadPosition.SetPosition(result, IECursorPosition::POSITION_CURRENT);
    }
    return result;
}

/**
 * \brief   Returns size in bytes of available data that can be read, i.e. remaining readable size.
 **/
unsigned int CEBufferStreamBase::GetReadableSize( void ) const
{
    unsigned int lenUsed = GetUsedSize();
    unsigned int lenRead = mReadPosition.GetPosition();
    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

/**
 * \brief   Write data to output stream object from given buffer and returns the size of written data.
 **/
unsigned int CEBufferStreamBase::Write( const unsigned char* buffer, unsigned int size )
{
    return WriteData(buffer, size);
}

/**
 * \brief   Writes Binary data from Byte Buffer object to output stream object and returns the size of written data.
 **/
unsigned int CEBufferStreamBase::Write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;
    if (static_cast<const IEByteBuffer *>(this) != static_cast<const IEByteBuffer *>(&buffer))
    {
        const unsigned char* data = buffer.GetBuffer();
        const unsigned int length = buffer.GetUsedSize();

        if (Write(reinterpret_cast<const unsigned char *>(&length), sizeof(unsigned int)) == sizeof(unsigned int))
            result = Write(data, length);
    }
    else
    {
        result = GetUsedSize(); // if same object, just imitate that the complete data was written.
    }
    return result;
}

/**
 * \brief   Writes string data from given ASCII String object to output stream object.
 **/
unsigned int CEBufferStreamBase::Write( const CEString & asciiString )
{
    const char * buffer = asciiString.GetBuffer();
    ASSERT(buffer != NULL_STRING);
    return Write( reinterpret_cast<const unsigned char *>(buffer), asciiString.GetOccupiedBytes() );
}

/**
 * \brief   Writes string data from given wide-char String object to output stream object.
 **/
unsigned int CEBufferStreamBase::Write( const CEWideString & wideString )
{
    const wchar_t * buffer = wideString.GetBuffer();
    ASSERT(buffer != static_cast<const wchar_t *>(NULL));
    return Write( reinterpret_cast<const unsigned char *>(buffer), wideString.GetOccupiedBytes() );
}

/**
 * \brief   Returns size in bytes of available space that can be written, i.e. remaining writable size.
 **/
unsigned int CEBufferStreamBase::GetWritableSize( void ) const
{
    unsigned int lenWritten     = mWritePosition.GetPosition();
    unsigned int lenAvailable   = GetLength();
    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

/**
 * \brief   Flushes cached data to output stream object.
 **/
void CEBufferStreamBase::Flush( void )
{
    ; // do nothing
}

void CEBufferStreamBase::ResetCursor(void) const
{
    mReadPosition.SetPosition(0, IECursorPosition::POSITION_BEGIN);
}

/**
 * \brief   Returns true if binary data of 2 buffers are equal.
 **/
bool CEBufferStreamBase::IsEqual( const CEBufferStreamBase &other ) const
{
    bool result = static_cast<const CEBufferStreamBase *>(this) == &other;
    if ( result == false && this->IsValid() && other.IsValid())
    {
        result = GetUsedSize() == other.GetUsedSize() ? NEMemory::IsEqualBuffer<unsigned char>(GetBuffer(), other.GetBuffer(), GetUsedSize()) : false;
    }

    return result;
}

/**
 * \brief   Inserts buffer of data at the given position.
 **/
unsigned int CEBufferStreamBase::InsertData( const unsigned char* buffer, unsigned int size, unsigned int insertAt )
{
    unsigned int result     = 0;
    if (size > 0 && buffer != NULL)
    {
        unsigned int writePos   = mWritePosition.GetPosition();
        if (IsValid() == false || insertAt >= writePos)
        {
            result = Write(buffer, size);
        }
        else
        {
            unsigned int remain = ResizeBuffer(writePos + size, true);
            if (remain >= size)
            {
                ASSERT(IsValid());
                unsigned char *dst      = GetBuffer() + insertAt;
                unsigned int moveSize   = writePos - insertAt;

                NEMemory::MemMove(dst + size, dst, moveSize);
                result = NEMemory::MemCopy(dst, size, buffer, size);

                unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
                unsigned int newPos     = writePos + result;
                SetUsedSize( MACRO_MAX(usedSize, newPos) );
                mWritePosition.SetPosition(static_cast<int>(newPos), IECursorPosition::POSITION_BEGIN);
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
unsigned int CEBufferStreamBase::WriteData(const unsigned char* buffer, unsigned int size)
{
    ASSERT( (buffer != NULL) || (size == 0) );
    unsigned int result     = 0;
    unsigned int writePos   = IsValid() ? mWritePosition.GetPosition() : 0;
    unsigned int remain     = ResizeBuffer(writePos + size, writePos != 0);

    result = MACRO_MIN(remain, size);
    if (result != 0)
    {
        ASSERT(IsValid());

        NEMemory::MemCopy(GetBuffer() + writePos, remain, buffer, result);
        unsigned int usedSize   = mByteBuffer->bufHeader.biUsed;
        unsigned int newPos     = writePos + result;
        SetUsedSize( MACRO_MAX(usedSize, newPos) );
        mWritePosition.SetPosition(static_cast<int>(newPos), IECursorPosition::POSITION_BEGIN);
    }

    return result;
}

/**
 * \brief   Reads data from buffer
 **/
unsigned int CEBufferStreamBase::ReadData(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (IsValid() && size != 0)
    {
        ASSERT(buffer != NULL);
        unsigned int remain = GetReadableSize();
        result = MACRO_MIN(remain, size);

        if (result != 0)
        {
            const unsigned char* src = GetBufferAtReadPosition();
            NEMemory::MemCopy(buffer, size, src, result);
            mReadPosition.SetPosition(static_cast<int>(result), IECursorPosition::POSITION_CURRENT);
        }
    }
    return result;
}

const unsigned char * CEBufferStreamBase::GetBufferAtReadPosition(void) const
{
    const unsigned char * result = GetBuffer();
    if ( result != NULL )
    {
        unsigned int posRead = mReadPosition.GetPosition();
        result = posRead <= mByteBuffer->bufHeader.biUsed ? result + posRead : NULL;
    }
    return result;
}

unsigned char * CEBufferStreamBase::GetBufferAtWritePosition(void)
{
    unsigned char * result = GetBuffer();
    if ( result != NULL )
    {
        unsigned int posWrite = mWritePosition.GetPosition();
        result = posWrite <= mByteBuffer->bufHeader.biUsed ? result + posWrite : NULL;
    }
    return result;
}

unsigned int CEBufferStreamBase::ResizeBuffer(unsigned int size, bool copy)
{
    unsigned int result = IEByteBuffer::ResizeBuffer(size, copy);

    if (result != 0)
    {
        ASSERT(IsValid());
        mWritePosition.SetPosition( mByteBuffer->bufHeader.biUsed, IECursorPosition::POSITION_BEGIN );
    }

    return result;
}
