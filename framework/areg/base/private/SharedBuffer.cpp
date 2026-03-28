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

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////

SharedBuffer::SharedBuffer(uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, uint32_t blockSize)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
    reserve(reserveSize, false);
}

SharedBuffer::SharedBuffer(const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
    reserveSize = std::max(reserveSize, size);
    reserve(reserveSize, false);
    write_data(buffer, size);
}

SharedBuffer::SharedBuffer(const char* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<char>(textString)) + 1u) * sizeof(char);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringA.data()), size);
}

SharedBuffer::SharedBuffer(const wchar_t* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( 0u )
    , mViewSize     ( 0u )
    , mReadPos      ( 0u )
{
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringW.data()), size);
}

SharedBuffer::SharedBuffer(const SharedBuffer& src) noexcept
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( src.mBaseOffset )
    , mViewSize     ( src.mViewSize )
    , mReadPos      ( 0u )
{
    mByteBuffer = src.mByteBuffer;   // share the block; read cursor starts at 0
}

SharedBuffer::SharedBuffer(SharedBuffer&& src) noexcept
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mByteBuffer   ( nullptr, ByteBufferDeleter() )
    , mBaseOffset   ( src.mBaseOffset )
    , mViewSize     ( src.mViewSize )
    , mReadPos      ( 0u )
{
    mByteBuffer     = std::move(src.mByteBuffer);
    src.mBaseOffset = 0u;
    src.mViewSize   = 0u;
    src.mReadPos    = 0u;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

SharedBuffer& SharedBuffer::operator = (const SharedBuffer& src) noexcept
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer = src.mByteBuffer;
            mBaseOffset = src.mBaseOffset;
            mViewSize   = src.mViewSize;
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
            mBaseOffset     = src.mBaseOffset;
            mViewSize       = src.mViewSize;
            mReadPos        = 0u;
            src.mBaseOffset = 0u;
            src.mViewSize   = 0u;
            src.mReadPos    = 0u;
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

uint32_t SharedBuffer::set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept
{
    if (is_valid() == false)
        return Cursor::INVALID_CURSOR_POSITION;

    const int32_t used   = static_cast<int32_t>((mViewSize > 0u) ? mViewSize : mByteBuffer->bufHeader.biUsed);
    int32_t       curPos = static_cast<int32_t>(mReadPos);

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
// SharedBuffer methods
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::reserve(uint32_t size, bool copy)
{
    if (size == 0)
    {
        invalidate();
        return 0u;
    }

    // View buffers are read-only windows into a shared block — cannot be resized.
    if (mViewSize > 0u)
        return 0u;

    if (mByteBuffer.use_count() > 1)
        return is_valid() ? (mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed) : 0u;

    size = std::min(size, SharedBuffer::MAX_BUF_LENGTH);

    const uint32_t curLength = is_valid() ? mByteBuffer->bufHeader.biLength : 0u;
    if (size <= curLength)
        return curLength - mByteBuffer->bufHeader.biUsed;

    // Doubling growth strategy: amortizes repeated reserve calls (O(log n) allocations)
    const uint32_t grownSize = std::min(std::max(size, curLength * 2u), SharedBuffer::MAX_BUF_LENGTH);
    const uint32_t sizeBuffer = areg::align_size(header_size() + grownSize, aligned_size());

    // Single allocation: shared_ptr aliasing constructor shares control block
    std::shared_ptr<uint8_t[]> spRaw(DEBUG_NEW uint8_t[sizeBuffer]);
    if (init_buffer(spRaw.get(), sizeBuffer, copy) != Cursor::INVALID_CURSOR_POSITION)
    {
        // aliasing: no 2nd heap alloc
        mByteBuffer = std::shared_ptr<areg::RawBuffer>(spRaw, reinterpret_cast<areg::RawBuffer*>(spRaw.get()));
    }

    return is_valid() ? (mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed) : 0u;
}

uint32_t SharedBuffer::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
{
    if (newBuffer == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t dataOffset = data_offset();
    const uint32_t dataLength = bufLength - dataOffset;

    areg::RawBuffer* buffer = new(newBuffer) areg::RawBuffer;
    buffer->bufHeader.biLength = dataLength;
    buffer->bufHeader.biOffset = dataOffset;
    buffer->bufHeader.biBufType = areg::BufferType::Internal;
    buffer->bufHeader.biUsed = 0u;

    if (!makeCopy || (mByteBuffer == nullptr))
        return 0u;

    const uint32_t srcCount = std::min(mByteBuffer->bufHeader.biUsed, dataLength);
    ::memcpy(newBuffer + dataOffset, areg::buffer_data_read(mByteBuffer.get()), srcCount);
    buffer->bufHeader.biUsed = srcCount;

    return srcCount;
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::write(const SharedBuffer& buf)
{
    if (static_cast<const SharedBuffer*>(this) == static_cast<const SharedBuffer*>(&buf))
        return size_used();

    const uint32_t length = buf.size_used();
    // For view buffers buf.buffer() returns the raw data-payload start, not the view start.
    // Account for mBaseOffset to get the correct source pointer.
    const uint8_t* data = buf.is_valid()
                        ? areg::buffer_data_read(buf.mByteBuffer.get()) + buf.mBaseOffset
                        : nullptr;
    return (write_data(reinterpret_cast<const uint8_t*>(&length), sizeof(uint32_t)) == sizeof(uint32_t))
             ? write_data(data, length)
             : 0u;
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::read(SharedBuffer& buf) const noexcept
{
    if (static_cast<const SharedBuffer*>(this) == static_cast<const SharedBuffer*>(&buf))
        return size_used();

    uint32_t length = 0u;
    if (read_data(reinterpret_cast<uint8_t*>(&length), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;

    if (length == 0u)
    {
        buf.invalidate();
        return 0u;
    }

    // Verify the claimed payload fits in the remaining source data.
    const areg::RawBuffer* const raw = mByteBuffer.get();
    const uint32_t srcEnd = (mViewSize > 0u) ? (mBaseOffset + mViewSize)
                                             : (raw != nullptr ? raw->bufHeader.biUsed : 0u);
    if (mBaseOffset + mReadPos + length > srcEnd)
    {
        // Not enough data; undo the 4-byte length read so caller can retry or detect EOF.
        mReadPos -= sizeof(uint32_t);
        return 0u;
    }

    // Zero-copy: share mByteBuffer and expose only [mBaseOffset+mReadPos, length) as a view.
    // No allocation, no memcpy — O(1) regardless of payload size.
    buf.mByteBuffer  = mByteBuffer;
    buf.mBaseOffset  = mBaseOffset + mReadPos;
    buf.mViewSize    = length;
    buf.mReadPos     = 0u;

    // Advance source cursor past the payload.
    mReadPos += length;

    return length;
}

uint32_t SharedBuffer::read(String& ascii) const
{
    ascii.clear();

    const uint8_t* data = buffer_to_read();
    if (data == nullptr)
        return 0u;

    ascii.assign(reinterpret_cast<const char*>(data));
    const uint32_t result = ascii.space();
    mReadPos += result;
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
    mReadPos += result;
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Additional streaming operations
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::insert_at(const uint8_t* buf, uint32_t size, uint32_t atPos)
{
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    const uint32_t writeEnd = is_valid() ? mByteBuffer->bufHeader.biUsed : 0u;
    if ((is_valid() == false) || (atPos >= writeEnd))
        return write_data(buf, size);

    const uint32_t needed = writeEnd + size;
    if (reserve(needed, true) < size)
        return 0u;

    ASSERT(is_valid());
    uint8_t*       dst      = buffer() + atPos;
    const uint32_t moveSize = writeEnd - atPos;

    areg::mem_move(dst + size, dst, moveSize);
    areg::mem_copy(dst, size, buf, size);

    mByteBuffer->bufHeader.biUsed = writeEnd + size;
    return size;
}

bool SharedBuffer::is_equal(const SharedBuffer& other) const noexcept
{
    if (static_cast<const SharedBuffer*>(this) == &other)
        return true;
    else if (!is_valid() || !other.is_valid())
        return false;

    const uint32_t used = size_used();  // view-aware
    if (used != other.size_used())
        return false;

    // For view buffers compare from the view start (mBaseOffset), not the raw buffer start.
    const uint8_t* data1 = areg::buffer_data_read(mByteBuffer.get()) + mBaseOffset;
    const uint8_t* data2 = areg::buffer_data_read(other.mByteBuffer.get()) + other.mBaseOffset;
    return areg::mem_equal(data1, data2, used);
}

const uint8_t* SharedBuffer::current_ptr() const noexcept
{
    if (!is_valid())
        return nullptr;

    const uint32_t limit = (mViewSize > 0u) ? mViewSize : mByteBuffer->bufHeader.biUsed;
    ASSERT(mReadPos != Cursor::INVALID_CURSOR_POSITION);
    return (mReadPos < limit)
         ? areg::buffer_data_read(mByteBuffer.get()) + mBaseOffset + mReadPos
         : nullptr;
}

SharedBuffer SharedBuffer::clone() const
{
    const uint32_t used = size_used();  // view-aware (returns mViewSize if set)
    SharedBuffer   result(mBlockSize);
    if ((used != 0u) && (result.reserve(used, false) >= used))
    {
        // For view buffers, copy only the view window (mBaseOffset..mBaseOffset+used).
        const uint8_t* src = areg::buffer_data_read(mByteBuffer.get()) + mBaseOffset;
        areg::mem_copy(result.buffer(), used, src, used);
        result.mByteBuffer->bufHeader.biUsed = used;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SharedBuffer protected overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::data_offset() const noexcept
{
    return sizeof(areg::BufferHeader);
}

uint32_t SharedBuffer::header_size() const noexcept
{
    return sizeof(areg::RawBuffer);
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
// Hot-path helpers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Core write: appends at biUsed (the write position).
 *
 *          Fast path (buffer valid, capacity sufficient): one null-check +
 *          one capacity check + memcpy — no virtual calls, no heap allocation.
 *          Slow path: delegates to reserve() for doubling growth.
 *
 *          The raw pointer is cached in a local to avoid calling .get() twice.
 *          After a successful reserve(copy=true), biUsed equals the original
 *          writePos, so we can safely re-use writePos across the reallocation.
 **/
uint32_t SharedBuffer::write_data(const uint8_t* buf, uint32_t size) noexcept
{
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    // View buffers are read-only windows — writes are not allowed.
    if (mViewSize > 0u)
        return 0u;

    areg::RawBuffer* const raw = mByteBuffer.get();
    const uint32_t writePos = (raw != nullptr) ? raw->bufHeader.biUsed : 0u;
    const uint32_t needed   = writePos + size;

    // Fast path: buffer already has room.
    if ((raw != nullptr) && (needed <= raw->bufHeader.biLength))
    {
        ::memcpy(areg::buffer_data_write(raw) + writePos, buf, size);
        raw->bufHeader.biUsed = needed;
        return size;
    }

    // Slow path: grow the buffer (doubling strategy inside reserve).
    // After a successful reserve with copy=true, biUsed == original writePos.
    const uint32_t avail = reserve(needed, true);
    if (avail == 0u)
        return 0u;

    // avail > 0 and size > 0, so result > 0 — no need to guard the memcpy.
    const uint32_t result = std::min(avail, size);
    ::memcpy(areg::buffer_data_write(mByteBuffer.get()) + writePos, buf, result);
    mByteBuffer->bufHeader.biUsed = writePos + result;
    return result;
}

/**
 * \brief   Core read: copies up to \a size bytes from mReadPos and advances
 *          the cursor. All checks are done directly against the header fields.
 *
 *          The raw pointer is cached to avoid calling .get() twice.
 *          mReadPos is always in [0, biUsed] — no INVALID_CURSOR_POSITION guard
 *          needed here; is_valid() / invalidate() maintain this invariant.
 **/
uint32_t SharedBuffer::read_data(uint8_t* buf, uint32_t size) const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (size == 0u) || (buf == nullptr))
        return 0u;

    // For view buffers the readable window is [0, mViewSize); for normal buffers [0, biUsed).
    const uint32_t used = (mViewSize > 0u) ? mViewSize : raw->bufHeader.biUsed;
    if (mReadPos >= used)
        return 0u;

    const uint32_t avail  = used - mReadPos;
    const uint32_t result = std::min(avail, size);
    // mBaseOffset is 0 for normal buffers, non-zero for views — add it unconditionally.
    ::memcpy(buf, areg::buffer_data_read(raw) + mBaseOffset + mReadPos, result);
    mReadPos += result;
    return result;
}

/**
 * \brief   Returns a pointer to the data at mReadPos, or nullptr when the
 *          cursor is at or past the end of written data.
 **/
const uint8_t* SharedBuffer::buffer_to_read() const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return nullptr;

    const uint32_t limit = (mViewSize > 0u) ? mViewSize : raw->bufHeader.biUsed;
    return (mReadPos <= limit)
         ? areg::buffer_data_read(raw) + mBaseOffset + mReadPos
         : nullptr;
}

} // namespace areg
