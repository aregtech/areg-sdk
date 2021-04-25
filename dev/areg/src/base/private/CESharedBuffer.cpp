/************************************************************************
 * \file        areg/src/base/private/CESharedBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Switches
 *
 ************************************************************************/
#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/ESynchObjects.hpp"
#include "areg/src/base/NEString.hpp"

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
CESharedBuffer::CESharedBuffer( unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    ; // do nothing
}

CESharedBuffer::CESharedBuffer( unsigned int reserveSize, unsigned int blockSize)
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    ResizeBuffer(reserveSize, false);
}

CESharedBuffer::CESharedBuffer( const unsigned char* buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    ResizeBuffer(size, false);
    WriteData(buffer, size);
}

CESharedBuffer::CESharedBuffer(const char * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<char>(textString) + 1) * sizeof(char);
    size = ResizeBuffer(size, false);
    WriteData( reinterpret_cast<const unsigned char *>(textString != NULL ? textString : NEString::EmptyStringA), size);
}

CESharedBuffer::CESharedBuffer(const wchar_t * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<wchar_t>(textString) + 1) * sizeof(wchar_t);
    size = ResizeBuffer(size, false);
    WriteData( reinterpret_cast<const unsigned char *>(textString != NULL ? textString : NEString::EmptyStringW), size);
}

CESharedBuffer::CESharedBuffer( NEMemory::sByteBuffer & buffer )
    : CEBufferStreamBase( buffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        (NEMemory::BLOCK_SIZE)
{
    IncreaseLock();
}

CESharedBuffer::CESharedBuffer( const CESharedBuffer & src )
    : CEBufferStreamBase( *src.mByteBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        (src.mBlockSize)
{
    IncreaseLock();
}

CESharedBuffer::~CESharedBuffer( void )
{
    RemoveReference();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void CESharedBuffer::IncreaseLock( void )
{
    if (IsValid())
    {
        CEInterlockedValue interlock(&mByteBuffer->bufHeader.biRefCount);
        interlock.Increment();
    }
}

void CESharedBuffer::DecreaseLock( void )
{
    if (IsValid())
    {
        CEInterlockedValue interlock(&mByteBuffer->bufHeader.biRefCount);
        if (interlock.TestDecrement() == false)
        {
#ifdef DEBUG

            if (mByteBuffer->bufHeader.biUsed > 1024 * 5)
            {
                OUTPUT_DBG("Deleting big buffer of length [ %d ], used size [ %d ], address [ %p ]"
                            , mByteBuffer->bufHeader.biLength
                            , mByteBuffer->bufHeader.biUsed
                            , mByteBuffer);
            }

#endif // DEBUG

            delete [] reinterpret_cast<unsigned char *>(mByteBuffer);

        }
    }

    CEBufferPosition::InvalidatePosition();
    mByteBuffer = &NEMemory::InvalidBuffer;
}


const CESharedBuffer& CESharedBuffer::operator = ( const CESharedBuffer &src )
{
    if (this != &src)
    {
        RemoveReference();

        mByteBuffer = src.mByteBuffer;

        MoveToBeginOfData();
        AddReference();
    }
    return (*this);
}

void CESharedBuffer::AddReference( void )
{
    IncreaseLock( );
}

void CESharedBuffer::RemoveReference( void )
{
    DecreaseLock( );
}

bool CESharedBuffer::IsShared( void ) const
{
    return (IsValid() ? mByteBuffer->bufHeader.biRefCount > 1 : false);
}

void CESharedBuffer::InvalidateBuffer( void )
{
    RemoveReference();
}

const unsigned char* CESharedBuffer::GetBufferAtCurrentPosition( void ) const
{
    const unsigned char* result = NULL;
    if (IsValid())
    {
        unsigned int curPos = GetCursorPosition();
        unsigned int written= GetUsedSize();
        ASSERT(curPos != IECursorPosition::INVALID_CURSOR_POSITION);
        if (curPos != written)
        {
            ASSERT(curPos < written);
            result = GetBuffer() + curPos;
        }
        else
        {
            OUTPUT_WARN("The current cursor position is at the end. Buffer at current position is NULL.");
        }
    }

    return result;
}

bool CESharedBuffer::IsSharable( void ) const
{
    return true;
}

bool CESharedBuffer::IsEqual( const CEBufferStreamBase &other ) const
{
    bool result = static_cast<const NEMemory::sByteBuffer *>(&(this->GetByteBuffer())) == static_cast<const NEMemory::sByteBuffer *>(&(other.GetByteBuffer()));
    if ( (result == false) && this->IsValid() && other.IsValid())
    {
        result = GetUsedSize() == other.GetUsedSize() ? NEMemory::IsEqualBuffer<unsigned char>(GetBuffer(), other.GetBuffer(), GetUsedSize()) : false;
    }

    return result;
}

int CESharedBuffer::SizeDataOffset(void) const
{
    return sizeof(NEMemory::sBuferHeader);
}

int CESharedBuffer::SizeBufferStruct(void) const
{
    return sizeof(NEMemory::sByteBuffer);
}

unsigned int CESharedBuffer::SizeAlignment(void) const
{
    return mBlockSize;
}
