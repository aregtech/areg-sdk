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
namespace areg {

inline SharedBuffer& SharedBuffer::self()
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
SharedBuffer::SharedBuffer( uint32_t blockSize /*= areg::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
}

SharedBuffer::SharedBuffer( uint32_t reserveSize, uint32_t blockSize)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer( const uint8_t* buffer, uint32_t size, uint32_t blockSize /*= areg::BLOCK_SIZE*/ )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserve(size, false);
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize)
    : BufferStreamBase  (static_cast<Cursor&>(self()), static_cast<Cursor&>(self()))
    , Cursor  ( )

    , mBlockSize        (areg::align_size(blockSize, areg::BLOCK_SIZE))
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    reserveSize = std::max(reserveSize, size);
    reserve(reserveSize, false);
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(const char * textString, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    uint32_t size   = (static_cast<uint32_t>(areg::string_length<char>(textString)) + 1u) * sizeof(char);
    size = reserve(size, false);
    write_data( reinterpret_cast<const uint8_t *>(textString != nullptr ? textString : areg::EmptyStringA.data( )), size);
}

SharedBuffer::SharedBuffer(const wchar_t * textString, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    uint32_t size   = (static_cast<uint32_t>(areg::string_length<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    size = reserve(size, false);
    write_data( reinterpret_cast<const uint8_t *>(textString != nullptr ? textString : areg::EmptyStringW.data( )), size);
}

SharedBuffer::SharedBuffer( const SharedBuffer & src )
    : BufferStreamBase  ( static_cast<Cursor &>(self()), static_cast<Cursor &>(self()) )
    , Cursor  ( )

    , mBlockSize        (src.mBlockSize)
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.set_position(0, Cursor::SeekOrigin::Begin);
}

SharedBuffer::SharedBuffer( SharedBuffer && src ) noexcept
    : BufferStreamBase  ( static_cast<Cursor &>(self( )), static_cast<Cursor &>(self( )) )
    , Cursor  ( )

    , mBlockSize        ( src.mBlockSize )
    , mBufferPosition   ( static_cast<ByteBuffer&>(self()) )
{
    mByteBuffer = src.mByteBuffer;
    mBufferPosition.set_position(0, Cursor::SeekOrigin::Begin);
    src.invalidate();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

SharedBuffer & SharedBuffer::operator = ( const SharedBuffer &src )
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer = src.mByteBuffer;
            mBufferPosition.set_position(0, Cursor::SeekOrigin::Begin);
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
        if ( src.is_valid( ) )
        {
            mByteBuffer = src.mByteBuffer;
            mBufferPosition.set_position( 0, Cursor::SeekOrigin::Begin );
            src.invalidate();
        }
        else
        {
            invalidate( );
        }
    }

    return (*this);
}

uint32_t SharedBuffer::set_position(int32_t offset, Cursor::SeekOrigin startAt) const
{
    return mBufferPosition.set_position(offset, startAt);
}

bool SharedBuffer::is_shared() const noexcept
{
    return (is_valid() && (mByteBuffer.use_count() > 1) );
}

void SharedBuffer::invalidate()
{
    mBufferPosition.invalidate( );
    BufferStreamBase::invalidate();
}

const uint8_t* SharedBuffer::current_ptr() const
{
    const uint8_t* result = nullptr;
    if (is_valid())
    {
        uint32_t curPos = position();
        uint32_t written= size_used();
        ASSERT(curPos != Cursor::INVALID_CURSOR_POSITION);
        if (curPos != written)
        {
            ASSERT(curPos < written);
            result = buffer() + curPos;
        }
    }

    return result;
}

SharedBuffer SharedBuffer::clone() const
{
    uint32_t reserved{ size_used() };
    SharedBuffer result;
    if ((result.reserve(reserved, false) >= reserved) && (reserved != 0))
    {
        uint8_t * dst = result.buffer();
        const uint8_t * src = buffer();
        areg::mem_copy(dst, reserved, src, reserved);
        result.set_size_used(reserved);
    }

    return result;
}

uint32_t SharedBuffer::position() const
{
    return mBufferPosition.position();
}

bool SharedBuffer::can_share() const noexcept
{
    return true;
}

uint32_t SharedBuffer::data_offset() const noexcept
{
    return sizeof(areg::BufferHeader);
}

uint32_t SharedBuffer::header_size() const noexcept
{
    return sizeof(areg::RawBuffer);
}

uint32_t SharedBuffer::aligned_size() const noexcept
{
    return mBlockSize;
}

uint32_t SharedBuffer::default_block_size() noexcept
{
    static std::atomic_uint32_t    _result{ 0 };
    uint32_t result = _result.load();
    if (result == 0)
    {
        result = Application::config_manager().buffer_block_size();
        _result.store(result);
        result = result == 0 ? areg::BLOCK_SIZE : result;
    }

    return result;
}

} // namespace areg
