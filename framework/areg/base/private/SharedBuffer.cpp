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

namespace areg
{

    inline SharedBuffer& SharedBuffer::self()
    {
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Constructors / destructor
    //////////////////////////////////////////////////////////////////////////
    SharedBuffer::SharedBuffer( uint32_t blockSize /*= areg::BLOCK_SIZE*/ )
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
    }

    SharedBuffer::SharedBuffer( uint32_t reserveSize, uint32_t blockSize)
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        reserve(reserveSize, false);
    }

    SharedBuffer::SharedBuffer( const uint8_t* buffer, uint32_t size, uint32_t blockSize /*= areg::BLOCK_SIZE*/ )
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        reserve(size, false);
        writeData(buffer, size);
    }

    SharedBuffer::SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize)
        : areg::BufferStreamBase  (static_cast<areg::Cursor&>(self()), static_cast<areg::Cursor&>(self()))
        , areg::Cursor  ( )

        , mBlockSize        (areg::alignSize(blockSize, areg::BLOCK_SIZE))
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        reserveSize = std::max(reserveSize, size);
        reserve(reserveSize, false);
        writeData(buffer, size);
    }

    SharedBuffer::SharedBuffer(const char * textString, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        uint32_t size   = (static_cast<uint32_t>(areg::getStringLength<char>(textString)) + 1u) * sizeof(char);
        size = reserve(size, false);
        writeData( reinterpret_cast<const uint8_t *>(textString != nullptr ? textString : areg::EmptyStringA.data( )), size);
    }

    SharedBuffer::SharedBuffer(const wchar_t * textString, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        ( areg::alignSize(blockSize, areg::BLOCK_SIZE) )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        uint32_t size   = (static_cast<uint32_t>(areg::getStringLength<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
        size = reserve(size, false);
        writeData( reinterpret_cast<const uint8_t *>(textString != nullptr ? textString : areg::EmptyStringW.data( )), size);
    }

    SharedBuffer::SharedBuffer( const SharedBuffer & src )
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self()), static_cast<areg::Cursor &>(self()) )
        , areg::Cursor  ( )

        , mBlockSize        (src.mBlockSize)
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        mByteBuffer = src.mByteBuffer;
        mBufferPosition.setPosition(0, areg::Cursor::SeekOrigin::Begin);
    }

    SharedBuffer::SharedBuffer( SharedBuffer && src ) noexcept
        : areg::BufferStreamBase  ( static_cast<areg::Cursor &>(self( )), static_cast<areg::Cursor &>(self( )) )
        , areg::Cursor  ( )

        , mBlockSize        ( src.mBlockSize )
        , mBufferPosition   ( static_cast<areg::ByteBuffer&>(self()) )
    {
        mByteBuffer = src.mByteBuffer;
        mBufferPosition.setPosition(0, areg::Cursor::SeekOrigin::Begin);
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
                mBufferPosition.setPosition(0, areg::Cursor::SeekOrigin::Begin);
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
                mBufferPosition.setPosition( 0, areg::Cursor::SeekOrigin::Begin );
                src.invalidate();
            }
            else
            {
                invalidate( );
            }
        }

        return (*this);
    }

    uint32_t SharedBuffer::setPosition(int32_t offset, areg::Cursor::SeekOrigin startAt) const
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
        areg::BufferStreamBase::invalidate();
    }

    const uint8_t* SharedBuffer::getBufferAtCurrentPosition() const
    {
        const uint8_t* result = nullptr;
        if (isValid())
        {
            uint32_t curPos = getPosition();
            uint32_t written= getSizeUsed();
            ASSERT(curPos != areg::Cursor::INVALID_CURSOR_POSITION);
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
        uint32_t reserved{ getSizeUsed() };
        SharedBuffer result;
        if ((result.reserve(reserved, false) >= reserved) && (reserved != 0))
        {
            uint8_t * dst = result.getBuffer();
            const uint8_t * src = getBuffer();
            areg::memCopy(dst, reserved, src, reserved);
            result.setSizeUsed(reserved);
        }

        return result;
    }

    uint32_t SharedBuffer::getPosition() const
    {
        return mBufferPosition.getPosition();
    }

    bool SharedBuffer::canShare() const
    {
        return true;
    }

    uint32_t SharedBuffer::getDataOffset() const
    {
        return sizeof(areg::BufferHeader);
    }

    uint32_t SharedBuffer::getHeaderSize() const
    {
        return sizeof(areg::RawBuffer);
    }

    uint32_t SharedBuffer::getAlignedSize() const
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

} // namespace areg