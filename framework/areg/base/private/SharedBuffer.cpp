/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SharedBuffer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform Switches
 *
 ************************************************************************/
#include "areg/base/SharedBuffer.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/NEString.hpp"
#include "areg/persist/ConfigManager.hpp"

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
    , IECursorPosition  ( )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
}

SharedBuffer::SharedBuffer( unsigned int reserveSize, unsigned int blockSize)
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , IECursorPosition  ( )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer( const unsigned char* buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , IECursorPosition  ( )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    reserve(size, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(unsigned int reserveSize, const unsigned char* buffer, unsigned int size, unsigned int blockSize)
    : BufferStreamBase  (static_cast<IECursorPosition&>(self()), static_cast<IECursorPosition&>(self()))
    , IECursorPosition  ( )

    , mBlockSize        (MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE))
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    reserveSize = MACRO_MAX(reserveSize, size);
    reserve(reserveSize, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(const char * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , IECursorPosition  ( )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    unsigned int size   = (static_cast<uint32_t>(NEString::getStringLength<char>(textString)) + 1u) * sizeof(char);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : NEString::EmptyStringA.data( )), size);
}

SharedBuffer::SharedBuffer(const wchar_t * textString, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , IECursorPosition  ( )

    , mBlockSize        ( MACRO_ALIGN_SIZE(blockSize, NEMemory::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    unsigned int size   = (static_cast<uint32_t>(NEString::getStringLength<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : NEString::EmptyStringW.data( )), size);
}

SharedBuffer::SharedBuffer( const SharedBuffer & src )
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , IECursorPosition  ( )

    , mBlockSize        (src.mBlockSize)
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
}

SharedBuffer::SharedBuffer( SharedBuffer && src ) noexcept
    : BufferStreamBase  ( static_cast<IECursorPosition &>(self( )), static_cast<IECursorPosition &>(self( )) )
    , IECursorPosition  ( )

    , mBlockSize        ( src.mBlockSize )
    , mBufferPosition   ( static_cast<IEByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
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
            mBufferPosition.setPosition(0, IECursorPosition::eCursorPosition::PositionBegin);
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
            mBufferPosition.setPosition( 0, IECursorPosition::eCursorPosition::PositionBegin );
            src.invalidate();
        }
        else
        {
            invalidate( );
        }
    }

    return (*this);
}

unsigned int SharedBuffer::setPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    return mBufferPosition.setPosition(offset, startAt);
}

bool SharedBuffer::isShared( void ) const
{
    return (isValid() && (mByteBuffer.use_count() > 1) );
}

void SharedBuffer::invalidate( void )
{
    mBufferPosition.invalidate( );
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

unsigned int SharedBuffer::getPosition(void) const
{
    return mBufferPosition.getPosition();
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

uint32_t SharedBuffer::getDefaultBlockSize(void)
{
    static std::atomic_uint32_t    _result{ 0 };
    uint32_t result = _result.load();
    if (result == 0)
    {
        result = Application::getConfigManager().getDefaultBufferBlockSize();
        _result.store(result);
        result = result == 0 ? NEMemory::BLOCK_SIZE : result;
    }

    return result;
}
