/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SharedBuffer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform Switches
 *
 ************************************************************************/
#include "areg/base/SharedBuffer.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <algorithm>
#include <atomic>

inline SharedBuffer& SharedBuffer::self()
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
SharedBuffer::SharedBuffer( unsigned int blockSize /*= areg::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
}

SharedBuffer::SharedBuffer( unsigned int reserveSize, unsigned int blockSize)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer( const unsigned char* buffer, unsigned int size, unsigned int blockSize /*= areg::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserve(size, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(unsigned int reserveSize, const unsigned char* buffer, unsigned int size, unsigned int blockSize)
    : BufferStreamBase  (static_cast<Cursor&>(self()), static_cast<Cursor&>(self()))
    , Cursor  ( )

    , mBlockSize        (areg::alignSize(blockSize, areg::BLOCK_SIZE))
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserveSize = std::max(reserveSize, size);
    reserve(reserveSize, false);
    writeData(buffer, size);
}

SharedBuffer::SharedBuffer(const char * textString, unsigned int blockSize /*= areg::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    unsigned int size   = (static_cast<uint32_t>(areg::getStringLength<char>(textString)) + 1u) * sizeof(char);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : areg::EmptyStringA.data( )), size);
}

SharedBuffer::SharedBuffer(const wchar_t * textString, unsigned int blockSize /*= areg::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    unsigned int size   = (static_cast<uint32_t>(areg::getStringLength<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    size = reserve(size, false);
    writeData( reinterpret_cast<const unsigned char *>(textString != nullptr ? textString : areg::EmptyStringW.data( )), size);
}

SharedBuffer::SharedBuffer( const SharedBuffer & src )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        (src.mBlockSize)
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.setPosition(0, Cursor::eCursorPosition::PositionBegin);
}

SharedBuffer::SharedBuffer( SharedBuffer && src ) noexcept
    : BufferStreamBase  ( static_cast<Cursor &>(self( )), static_cast<Cursor &>(self( )) )
    , Cursor  ( )

    , mBlockSize        ( src.mBlockSize )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.setPosition(0, Cursor::eCursorPosition::PositionBegin);
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
            mBufferPosition.setPosition(0, Cursor::eCursorPosition::PositionBegin);
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
            mBufferPosition.setPosition( 0, Cursor::eCursorPosition::PositionBegin );
            src.invalidate();
        }
        else
        {
            invalidate( );
        }
    }

    return (*this);
}

unsigned int SharedBuffer::setPosition(int offset, Cursor::eCursorPosition startAt) const
{
    return mBufferPosition.setPosition(offset, startAt);
}

bool SharedBuffer::isShared() const
{
    return (isValid() && (mByteBuffer.use_count() > 1) );
}

void SharedBuffer::invalidate()
{
    mBufferPosition.invalidate( );
    BufferStreamBase::invalidate();
}

const unsigned char* SharedBuffer::getBufferAtCurrentPosition() const
{
    const unsigned char* result = nullptr;
    if (isValid())
    {
        unsigned int curPos = getPosition();
        unsigned int written= getSizeUsed();
        ASSERT(curPos != Cursor::INVALID_CURSOR_POSITION);
        if (curPos != written)
        {
            ASSERT(curPos < written);
            result = getBuffer() + curPos;
        }
    }

    return result;
}

SharedBuffer SharedBuffer::clone() const
{
    unsigned int reserved{ getSizeUsed() };
    SharedBuffer result;
    if ((result.reserve(reserved, false) >= reserved) && (reserved != 0))
    {
        unsigned char * dst = result.getBuffer();
        const unsigned char * src = getBuffer();
        areg::memCopy(dst, reserved, src, reserved);
        result.setSizeUsed(reserved);
    }

    return result;
}

unsigned int SharedBuffer::getPosition() const
{
    return mBufferPosition.getPosition();
}

bool SharedBuffer::canShare() const
{
    return true;
}

unsigned int SharedBuffer::getDataOffset() const
{
    return sizeof(areg::BufferHeader);
}

unsigned int SharedBuffer::getHeaderSize() const
{
    return sizeof(areg::sByteBuffer);
}

unsigned int SharedBuffer::getAlignedSize() const
{
    return mBlockSize;
}

uint32_t SharedBuffer::getDefaultBlockSize()
{
    static std::atomic_uint32_t    _result{ 0 };
    uint32_t result = _result.load();
    if (result == 0)
    {
        result = Application::getConfigManager().getDefaultBufferBlockSize();
        _result.store(result);
        result = result == 0 ? areg::BLOCK_SIZE : result;
    }

    return result;
}
