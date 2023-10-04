/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SharedBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform Switches
 *
 ************************************************************************/
#include "areg/base/SharedBuffer.hpp"

#include "areg/base/SynchObjects.hpp"
#include "areg/base/NEString.hpp"
#include <atomic>

inline SharedBuffer& SharedBuffer::self( void )
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
SharedBuffer::SharedBuffer( unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition    ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
}

SharedBuffer::SharedBuffer( unsigned int reserveSize, unsigned int blockSize)
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition    ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer( const unsigned char* buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition    ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    reserve(size, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(const char * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<char>(textString) + 1) * sizeof(char);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : NEString::EmptyStringA.data( )), size);
}

SharedBuffer::SharedBuffer(const wchar_t * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
{
    unsigned int size   = (NEString::getStringLength<wchar_t>(textString) + 1) * sizeof(wchar_t);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : NEString::EmptyStringW.data( )), size);
}

SharedBuffer::SharedBuffer( const SharedBuffer & src )
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition    ( static_cast<IEByteBuffer &>(self()) )

    , mBlockSize        (src.mBlockSize)
{
    mByteBuffer = src.mByteBuffer;
    BufferPosition::setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
}

SharedBuffer::SharedBuffer( SharedBuffer && src ) noexcept
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self( )), static_cast<IECursorPosition &>(self( )) )
    , BufferPosition    ( static_cast<IEByteBuffer &>(self( )) )

    , mBlockSize        ( src.mBlockSize )
{
    mByteBuffer = src.mByteBuffer;
    BufferPosition::setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
    src.invalidate();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

SharedBuffer & SharedBuffer::operator = ( const SharedBuffer &src )
{
    if (this != &src)
    {
        if (src.isValid())
        {
            mByteBuffer = src.mByteBuffer;
            BufferPosition::setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
        }
        else
        {
            invalidate( );
        }
    }

    return (*this);
}

SharedBuffer & SharedBuffer::operator = ( SharedBuffer && src ) noexcept
{
    if ( this != &src )
    {
        if ( src.isValid( ) )
        {
            mByteBuffer = src.mByteBuffer;
            BufferPosition::setPosition( 0, IECursorPosition::eCursorPosition::PositionBegin );
            src.invalidate();
        }
        else
        {
            invalidate( );
        }
    }

    return (*this);
}

bool SharedBuffer::isShared( void ) const
{
    return (isValid() && (mByteBuffer.use_count() > 1) );
}

void SharedBuffer::invalidate( void )
{
    BufferPosition::invalidate( );
    BufferStreamBase::invalidate();
}

const unsigned char* SharedBuffer::getBufferAtCurrentPosition( void ) const
{
    const unsigned char* result = nullptr;
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
            OUTPUT_WARN("The current cursor position is at the end. Buffer at current position is nullptr.");
        }
    }

    return result;
}

SharedBuffer SharedBuffer::clone(void) const
{
    unsigned int reserved{ getSizeUsed() };
    SharedBuffer result;
    if ((result.reserve(reserved, false) >= reserved) && (reserved != 0))
    {
        unsigned char * dst = result.getBuffer();
        const unsigned char * src = getBuffer();
        NEMemory::memCopy(dst, reserved, src, reserved);
        result.setSizeUsed(reserved);
    }

    return result;
}

bool SharedBuffer::canShare( void ) const
{
    return true;
}

unsigned int SharedBuffer::getDataOffset(void) const
{
    return sizeof(NEMemory::sBuferHeader);
}

unsigned int SharedBuffer::getHeaderSize(void) const
{
    return sizeof(NEMemory::sByteBuffer);
}

unsigned int SharedBuffer::getAlignedSize(void) const
{
    return mBlockSize;
}
