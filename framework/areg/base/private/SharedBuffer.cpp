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
 * \brief       Areg Platform, Shared Buffer with integrated streaming.
 *
 ************************************************************************/
#include "areg/base/SharedBuffer.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/WideString.hpp"

#include <algorithm>
#include <atomic>
#include <string.h>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////

SharedBuffer::SharedBuffer(uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, uint32_t blockSize)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer(const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    reserveSize = std::max(reserveSize, size);
    ByteBuffer::reserve(reserveSize, false);
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(const char* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<char>(textString)) + 1u) * sizeof(char);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringA.data()), size);
}

SharedBuffer::SharedBuffer(const wchar_t* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringW.data()), size);
}

SharedBuffer::SharedBuffer(const SharedBuffer& src)
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    mByteBuffer = src.mByteBuffer;   // share the block; read cursor starts at 0
    mReadPos    = 0u;
}

SharedBuffer::SharedBuffer(SharedBuffer&& src) noexcept
    : ByteBuffer    ( )
    , IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mReadPos      ( Cursor::INVALID_CURSOR_POSITION )
{
    mByteBuffer     = std::move(src.mByteBuffer);
    mReadPos        = 0u;
    src.mReadPos    = Cursor::INVALID_CURSOR_POSITION;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

SharedBuffer& SharedBuffer::operator = (const SharedBuffer& src)
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer = src.mByteBuffer;
            mReadPos    = 0u;
        }
        else
        {
            invalidate();
        }
    }

    return (*this);
}

SharedBuffer& SharedBuffer::operator = (SharedBuffer&& src) noexcept
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer     = std::move(src.mByteBuffer);
            mReadPos        = 0u;
            src.mReadPos    = Cursor::INVALID_CURSOR_POSITION;
        }
        else
        {
            invalidate();
        }
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Cursor overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::position() const noexcept
{
    if (is_valid() == false)
        return Cursor::INVALID_CURSOR_POSITION;

    return (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
}

uint32_t SharedBuffer::set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept
{
    if (is_valid() == false)
        return Cursor::INVALID_CURSOR_POSITION;

    const int32_t used   = static_cast<int32_t>(mByteBuffer->bufHeader.biUsed);
    int32_t       curPos = static_cast<int32_t>(mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);

    switch (startAt)
    {
    case Cursor::SeekOrigin::Begin:
        curPos = 0;
        offset = offset < 0 ? 0 : std::min(offset, used);
        break;

    case Cursor::SeekOrigin::Current:
        offset = offset < 0
               ? std::max(offset, -curPos)
               : std::min(offset, used - curPos);
        break;

    case Cursor::SeekOrigin::End:
        curPos = used;
        offset = offset < 0 ? std::max(offset, -used) : 0;
        break;

    default:
        ASSERT(false);
    }

    mReadPos = static_cast<uint32_t>(curPos + offset);
    return mReadPos;
}

//////////////////////////////////////////////////////////////////////////
// ByteBuffer overrides
//////////////////////////////////////////////////////////////////////////

bool SharedBuffer::is_shared() const noexcept
{
    return is_valid() && (mByteBuffer.use_count() > 1);
}

bool SharedBuffer::can_share() const noexcept
{
    return true;
}

void SharedBuffer::invalidate()
{
    mReadPos = Cursor::INVALID_CURSOR_POSITION;
    ByteBuffer::invalidate();
}

uint32_t SharedBuffer::reserve(uint32_t size, bool copy)
{
    // Write position is always biUsed; no cursor state to save/restore.
    return ByteBuffer::reserve(size, copy);
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::write(const uint8_t* buf, uint32_t size)
{
    return write_data(buf, size);
}

uint32_t SharedBuffer::write(const ByteBuffer& buf)
{
    if (static_cast<const ByteBuffer*>(this) == static_cast<const ByteBuffer*>(&buf))
        return size_used();   // same object: nothing to do, imitate full write

    const uint8_t* data   = buf.buffer();
    const uint32_t length = buf.size_used();
    return (write_data(reinterpret_cast<const uint8_t*>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
         ? write_data(data, length)
         : 0u;
}

uint32_t SharedBuffer::write(const String& ascii)
{
    return write_data(reinterpret_cast<const uint8_t*>(ascii.as_string()), ascii.space());
}

uint32_t SharedBuffer::write(const WideString& wide)
{
    return write_data(reinterpret_cast<const uint8_t*>(wide.as_string()), wide.space());
}

void SharedBuffer::flush()
{
    // No-op: in-memory buffer, nothing to flush.
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::read(uint8_t* buf, uint32_t size) const
{
    return read_data(buf, size);
}

uint32_t SharedBuffer::read(ByteBuffer& buf) const
{
    if (static_cast<const ByteBuffer*>(this) == static_cast<const ByteBuffer*>(&buf))
        return size_used();

    uint32_t length = 0u;
    if (read_data(reinterpret_cast<uint8_t*>(&length), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;

    buf.invalidate();
    const uint32_t avail = buf.reserve(length, false);
    if (avail == 0u)
        return 0u;

    uint8_t* dst    = buf.buffer();
    uint32_t result = read_data(dst, dst != nullptr ? length : 0u);
    buf.set_size_used(result);   // public setter — avoids protected byte_buffer() access

    return result;
}

uint32_t SharedBuffer::read(String& ascii) const
{
    ascii.clear();

    const uint8_t* data = buffer_to_read();
    if (data == nullptr)
        return 0u;

    ascii.assign(reinterpret_cast<const char*>(data));
    const uint32_t result = ascii.space();   // bytes consumed: length + NUL terminator
    // Advance read cursor by the bytes consumed (clamped by set_position logic).
    const uint32_t cur = (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
    mReadPos = cur + result;
    return result;
}

uint32_t SharedBuffer::read(WideString& wide) const
{
    wide.clear();

    const uint8_t* data = buffer_to_read();
    if (data == nullptr)
        return 0u;

    wide.assign(reinterpret_cast<const wchar_t*>(data));
    const uint32_t result = wide.space();
    const uint32_t cur = (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
    mReadPos = cur + result;
    return result;
}

void SharedBuffer::reset() const
{
    set_position(0, Cursor::SeekOrigin::Begin);
}

//////////////////////////////////////////////////////////////////////////
// Additional streaming operations
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::insert_at(const uint8_t* buf, uint32_t size, uint32_t atPos)
{
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    // If buffer is empty or insertion point is at/past the end, just append.
    const uint32_t writeEnd = is_valid() ? mByteBuffer->bufHeader.biUsed : 0u;
    if ((is_valid() == false) || (atPos >= writeEnd))
        return write_data(buf, size);

    // Ensure enough room for the shift + insert.
    const uint32_t needed = writeEnd + size;
    if (ByteBuffer::reserve(needed, true) < size)
        return 0u;

    ASSERT(is_valid());
    uint8_t*       dst      = buffer() + atPos;
    const uint32_t moveSize = writeEnd - atPos;

    areg::mem_move(dst + size, dst, moveSize);
    areg::mem_copy(dst, size, buf, size);

    mByteBuffer->bufHeader.biUsed = writeEnd + size;
    return size;
}

bool SharedBuffer::is_equal(const SharedBuffer& other) const
{
    if (static_cast<const SharedBuffer*>(this) == &other)
        return true;

    if (is_valid() == false || other.is_valid() == false)
        return false;

    const uint32_t used = size_used();
    return (used == other.size_used())
        && areg::mem_equal(buffer(), other.buffer(), used);
}

const uint8_t* SharedBuffer::current_ptr() const
{
    if (is_valid() == false)
        return nullptr;

    const uint32_t cur     = position();
    const uint32_t written = size_used();
    ASSERT(cur != Cursor::INVALID_CURSOR_POSITION);
    return (cur < written) ? buffer() + cur : nullptr;
}

SharedBuffer SharedBuffer::clone() const
{
    const uint32_t used = size_used();
    SharedBuffer   result(mBlockSize);
    if ((used != 0u) && (result.ByteBuffer::reserve(used, false) >= used))
    {
        areg::mem_copy(result.buffer(), used, buffer(), used);
        result.mByteBuffer->bufHeader.biUsed = used;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::size_readable() const
{
    if (is_valid() == false)
        return 0u;

    const uint32_t cur  = (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
    const uint32_t used = mByteBuffer->bufHeader.biUsed;
    ASSERT(cur <= used);
    return used - cur;
}

uint32_t SharedBuffer::size_writable() const
{
    if (is_valid() == false)
        return 0u;

    const uint32_t written = mByteBuffer->bufHeader.biUsed;
    const uint32_t avail   = mByteBuffer->bufHeader.biLength;
    ASSERT(written <= avail);
    return avail - written;
}

//////////////////////////////////////////////////////////////////////////
// ByteBuffer protected overrides
//////////////////////////////////////////////////////////////////////////

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
    static std::atomic_uint32_t cached{ 0u };
    uint32_t result = cached.load(std::memory_order_relaxed);
    if (result == 0u)
    {
        result = Application::config_manager().buffer_block_size();
        result = (result == 0u ? areg::BLOCK_SIZE : result);
        cached.store(result, std::memory_order_relaxed);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Hot-path private helpers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Core write helper.  Always appends at the current write position
 *          which is implicitly biUsed.
 *
 *          Fast path (buffer valid, capacity sufficient): one capacity check
 *          plus a memcpy — no virtual calls, no heap allocation.
 *          Slow path: delegates to ByteBuffer::reserve() for doubling growth,
 *          then performs the memcpy.
 **/
uint32_t SharedBuffer::write_data(const uint8_t* buf, uint32_t size)
{
    ASSERT((buf != nullptr) || (size == 0u));
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    const uint32_t writePos = is_valid() ? mByteBuffer->bufHeader.biUsed : 0u;
    const uint32_t needed   = writePos + size;

    // Fast path: buffer already has room.
    if (is_valid() && (needed <= mByteBuffer->bufHeader.biLength))
    {
        ::memcpy(areg::buffer_data_write(mByteBuffer.get()) + writePos, buf, size);
        mByteBuffer->bufHeader.biUsed = needed;
        return size;
    }

    // Slow path: grow the buffer (doubling strategy inside ByteBuffer::reserve).
    // After a successful reserve with copy=true, biUsed == original writePos.
    const uint32_t avail = ByteBuffer::reserve(needed, true);
    if (avail == 0u)
        return 0u;

    const uint32_t result = std::min(avail, size);
    if (result > 0u)
    {
        ::memcpy(areg::buffer_data_write(mByteBuffer.get()) + writePos, buf, result);
        mByteBuffer->bufHeader.biUsed = writePos + result;
    }

    return result;
}

/**
 * \brief   Core read helper.  Copies up to \a size bytes from mReadPos and
 *          advances the cursor.  All checks are done directly against the
 *          header fields — no virtual dispatch.
 **/
uint32_t SharedBuffer::read_data(uint8_t* buf, uint32_t size) const
{
    if ((is_valid() == false) || (size == 0u) || (buf == nullptr))
        return 0u;

    const uint32_t readPos = (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
    const uint32_t used    = mByteBuffer->bufHeader.biUsed;
    if (readPos >= used)
        return 0u;

    const uint32_t avail  = used - readPos;
    const uint32_t result = std::min(avail, size);
    ::memcpy(buf, areg::buffer_data_read(mByteBuffer.get()) + readPos, result);
    mReadPos = readPos + result;
    return result;
}

/**
 * \brief   Returns a pointer to the current read position in the data payload,
 *          or nullptr when the cursor is at or past the end of written data.
 **/
const uint8_t* SharedBuffer::buffer_to_read() const noexcept
{
    if (is_valid() == false)
        return nullptr;

    const uint32_t readPos = (mReadPos == Cursor::INVALID_CURSOR_POSITION ? 0u : mReadPos);
    const uint32_t used    = mByteBuffer->bufHeader.biUsed;
    return (readPos <= used) ? areg::buffer_data_read(mByteBuffer.get()) + readPos : nullptr;
}

} // namespace areg
