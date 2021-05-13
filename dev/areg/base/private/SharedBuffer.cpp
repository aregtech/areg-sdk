/************************************************************************
 * \file        areg/base/private/SharedBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Switches
 *
 ************************************************************************/
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/NEString.hpp"

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
SharedBuffer::SharedBuffer( unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    ; // do nothing
}

SharedBuffer::SharedBuffer( unsigned int reserveSize, unsigned int blockSize)
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    resize(reserveSize, false);
}

SharedBuffer::SharedBuffer( const unsigned char* buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    resize(size, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(const char * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<char>(textString) + 1) * sizeof(char);
    size = resize(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != NULL ? textString : NEString::EmptyStringA), size);
}

SharedBuffer::SharedBuffer(const wchar_t * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<wchar_t>(textString) + 1) * sizeof(wchar_t);
    size = resize(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != NULL ? textString : NEString::EmptyStringW), size);
}

SharedBuffer::SharedBuffer( NEMemory::sByteBuffer & buffer )
    : BufferStreamBase( buffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        (NEMemory::BLOCK_SIZE)
{
    increaseLock();
}

SharedBuffer::SharedBuffer( const SharedBuffer & src )
    : BufferStreamBase( *src.mByteBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        (src.mBlockSize)
{
    increaseLock();
}

SharedBuffer::~SharedBuffer( void )
{
    removeReference();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void SharedBuffer::increaseLock( void )
{
    if (isValid())
    {
        InterlockedValue interlock(&mByteBuffer->bufHeader.biRefCount);
        interlock.increment();
    }
}

void SharedBuffer::decreaseLock( void )
{
    if (isValid())
    {
        InterlockedValue interlock(&mByteBuffer->bufHeader.biRefCount);
        if (interlock.testDecrement() == false)
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

    BufferPosition::invalidate();
    mByteBuffer = &NEMemory::InvalidBuffer;
}


const SharedBuffer& SharedBuffer::operator = ( const SharedBuffer &src )
{
    if (this != &src)
    {
        removeReference();

        mByteBuffer = src.mByteBuffer;

        moveToBegin();
        addReference();
    }
    return (*this);
}

void SharedBuffer::addReference( void )
{
    increaseLock( );
}

void SharedBuffer::removeReference( void )
{
    decreaseLock( );
}

bool SharedBuffer::isShared( void ) const
{
    return (isValid() ? mByteBuffer->bufHeader.biRefCount > 1 : false);
}

void SharedBuffer::invalidate( void )
{
    removeReference();
}

const unsigned char* SharedBuffer::getBufferAtCurrentPosition( void ) const
{
    const unsigned char* result = NULL;
    if (isValid())
    {
        unsigned int curPos = getPosition();
        unsigned int written= getSizeUsed();
        ASSERT(curPos != IECursorPosition::INVALID_CURSOR_POSITION);
        if (curPos != written)
        {
            ASSERT(curPos < written);
            result = getBuffer() + curPos;
        }
        else
        {
            OUTPUT_WARN("The current cursor position is at the end. Buffer at current position is NULL.");
        }
    }

    return result;
}

bool SharedBuffer::canShare( void ) const
{
    return true;
}

bool SharedBuffer::isEqual( const BufferStreamBase &other ) const
{
    bool result = static_cast<const NEMemory::sByteBuffer *>(&(this->getByteBuffer())) == static_cast<const NEMemory::sByteBuffer *>(&(other.getByteBuffer()));
    if ( (result == false) && this->isValid() && other.isValid())
    {
        result = getSizeUsed() == other.getSizeUsed() ? NEMemory::isEqualBuffer<unsigned char>(getBuffer(), other.getBuffer(), getSizeUsed()) : false;
    }

    return result;
}

int SharedBuffer::getDataOffset(void) const
{
    return sizeof(NEMemory::sBuferHeader);
}

int SharedBuffer::getHeaderSize(void) const
{
    return sizeof(NEMemory::sByteBuffer);
}

unsigned int SharedBuffer::getAlignedSize(void) const
{
    return mBlockSize;
}
