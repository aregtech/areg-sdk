/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/BufferBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Shared Buffer with integrated streaming.
 *
 ************************************************************************/
#include "areg/base/BufferBase.hpp"
#include "areg/base/SharedBuffer.hpp"

#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/private/EnvelopeCache.hpp"


#include <algorithm>
#include <new>

namespace areg {

namespace {
    // Total capacity a write grows the block to: used + needed, capped at MAX_BUF_LENGTH.
    constexpr uint32_t grow_size(uint32_t used, uint32_t needed) noexcept
    {
        return (needed >= areg::MAX_BUF_LENGTH - std::min(used, areg::MAX_BUF_LENGTH)) ? areg::MAX_BUF_LENGTH : used + needed;
    }
}

//////////////////////////////////////////////////////////////////////////
// BufferBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////

BufferBase::BufferBase(uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
}

uint32_t BufferBase::position() const noexcept
{
    return is_valid() ? mPosition : areg::Cursor::INVALID_CURSOR_POSITION;
}

uint32_t BufferBase::write(const uint8_t* buf, uint32_t size)
{
    return write_bytes(buf, size);
}

void BufferBase::flush() noexcept
{
    // No-op: in-memory buffer, nothing to flush.
}

bool BufferBase::ensure_size(uint32_t addSize)
{
    if (!can_write() || (addSize > areg::MAX_BUF_LENGTH - mPosition))
        return false;

    const uint32_t needed{ mPosition + addSize };
    if ((addSize == 0u) || ((mByteBuffer.get() != nullptr) && (needed <= mByteBuffer->bufHeader.biLength)))
        return true;

    // Grows like a write does, the data and the cursor are kept.
    const uint32_t position{ mPosition };
    _grow_block(grow_size(size_used(), needed));
    mPosition = position;
    return ((mByteBuffer.get() != nullptr) && (needed <= mByteBuffer->bufHeader.biLength));
}

uint32_t BufferBase::read(uint8_t* buf, uint32_t size) const noexcept
{
    return read_data(buf, size);
}

void BufferBase::reset() const noexcept
{
    mPosition = 0u;
}

uint32_t BufferBase::size_readable() const noexcept
{
    if (!is_valid())
        return 0u;
    const uint32_t limit = mByteBuffer->bufHeader.biUsed;
    return (mPosition < limit ? limit - mPosition : 0u);
}

uint32_t BufferBase::size_writable() const noexcept
{
    if (!is_valid())
        return 0u;
    return mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed;
}

BufferBase::BufferBase(uint32_t reserveSize, uint32_t blockSize)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    reserve(reserveSize, false);
}

BufferBase::BufferBase(const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    write_data(buffer, size);
}

BufferBase::BufferBase(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    reserveSize = std::max(reserveSize, size);
    reserve(reserveSize, false);
    write_data(buffer, size);
}

BufferBase::BufferBase(const char* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    write_text(textString, static_cast<uint32_t>(areg::string_length<char>(textString)));
}

BufferBase::BufferBase(const wchar_t* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    write_text(textString, static_cast<uint32_t>(areg::string_length<wchar_t>(textString)));
}

BufferBase::BufferBase(const BufferBase& src) noexcept
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    mByteBuffer = src.mByteBuffer;   // share the block; read cursor at start
}

BufferBase::BufferBase(BufferBase&& src) noexcept
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( src.mBlockSize )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    mByteBuffer     = std::move(src.mByteBuffer);
    src.mPosition   = 0u;
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////

BufferBase& BufferBase::operator = (const BufferBase& src) noexcept
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer = src.mByteBuffer;
            mPosition   = 0u;
        }
        else
        {
            invalidate();
        }
    }

    return (*this);
}

BufferBase& BufferBase::operator = (BufferBase&& src) noexcept
{
    if (this != &src)
    {
        if (src.is_valid())
        {
            mByteBuffer     = std::move(src.mByteBuffer);
            mPosition       = 0u;
            src.mPosition   = 0u;
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

uint32_t BufferBase::set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept
{
    if (!is_valid())
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t viewEnd    = mByteBuffer->bufHeader.biUsed;
    const int32_t  rangeSize  = static_cast<int32_t>(viewEnd);
    int32_t        curPos     = static_cast<int32_t>(mPosition);

    if (startAt == Cursor::SeekOrigin::Begin)
    {
        curPos = 0;
        offset = offset < 0 ? 0 : std::min(offset, rangeSize);
    }
    else if (startAt == Cursor::SeekOrigin::Current)
    {
        offset = offset < 0 ? std::max(offset, -curPos) : std::min(offset, rangeSize - curPos);
    }
    else
    {
        curPos = rangeSize;
        offset = offset < 0 ? std::max(offset, -rangeSize) : 0;
    }

    mPosition = static_cast<uint32_t>(curPos + offset);
    return mPosition;
}

//////////////////////////////////////////////////////////////////////////
// BufferBase methods
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::reserve(uint32_t size, bool copy) noexcept
{
    if (size == 0)
    {
        invalidate();
        return 0u;
    }

    const areg::RawBuffer* raw = mByteBuffer.get();
    const bool hasView         = false;     // BufferBase never holds a view
    const bool preserveData    = copy || hasView;
    const uint32_t used        = preserveData ? size_used() : 0u;
    uint32_t total             = std::min(std::max(size, used), areg::MAX_BUF_LENGTH);

    if ((raw != nullptr) && !hasView && (total <= raw->bufHeader.biLength))
    {
        if (copy)
        {
            mPosition = mByteBuffer->bufHeader.biUsed;
        }
        else
        {
            mByteBuffer->bufHeader.biUsed = 0u;
            mPosition = 0u;
        }

        return (mByteBuffer->bufHeader.biLength - mPosition);
    }

    total = areg::align_size(total, block_size());
    const uint32_t headerSize = header_size();
    uint32_t sizeBuffer = headerSize + total;
    uint8_t* buffer{ nullptr };
    if (headerSize == static_cast<uint32_t>(sizeof(areg::RawEnvelope)))
    {
        uint32_t payload{ sizeBuffer - static_cast<uint32_t>(sizeof(areg::EventHeader)) };
        buffer = areg::envelope_block_alloc(payload);
        sizeBuffer = payload + static_cast<uint32_t>(sizeof(areg::EventHeader));
    }
    else
    {
        buffer = new (std::nothrow) uint8_t[sizeBuffer];
    }

    const uint32_t position = init_buffer(buffer, sizeBuffer, preserveData);
    if (position == Cursor::INVALID_CURSOR_POSITION)
    {
        // A message that cannot grow is lost as a whole.
        if (headerSize == static_cast<uint32_t>(sizeof(areg::RawEnvelope)))
        {
            invalidate();
        }

        return 0u;
    }

    invalidate();
    mByteBuffer = areg::RawBufferPtr(reinterpret_cast<areg::RawBuffer*>(buffer));
    mPosition   = position;
    return (mByteBuffer->bufHeader.biLength - mPosition);
}

void BufferBase::_grow_block(uint32_t size) noexcept
{
    // A message has its header in the block: without the block it takes no data.
    if ((mByteBuffer.get() != nullptr) || (header_size() != static_cast<uint32_t>(sizeof(areg::RawEnvelope))))
    {
        reserve(size, true);
    }
}

uint32_t BufferBase::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
{
    if (newBuffer == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t dataOffset{ data_offset() };
    const uint32_t dataLength{ bufLength - dataOffset };

    areg::RawBuffer* msgDst     = areg::construct_elems<areg::RawBuffer>(newBuffer, 1); // new(newBuffer) areg::RawBuffer;
    areg::BufferHeader& hdrDst  = msgDst->bufHeader;
    hdrDst.biLength  = dataLength;
    hdrDst.biOffset  = dataOffset;
    hdrDst.biBufType = areg::BufferType::Internal;
    hdrDst.biUsed    = 0u;

    if (!makeCopy || !mByteBuffer)
        return 0u;

    const uint32_t srcCount = std::min(size_used(), dataLength);
    areg::mem_copy(newBuffer + dataOffset, dataLength, areg::buffer_data_read(mByteBuffer.get()), srcCount);
    hdrDst.biUsed = srcCount;

    return srcCount;
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::write(const SharedBuffer& buf)
{
    if (static_cast<const BufferBase*>(this) == static_cast<const BufferBase*>(&buf))
        return size_used();

    // An invalid source is written as an empty one: the length prefix is always present.
    const uint32_t length = buf.size_used();  // source may be a view; size_used()/buffer() are view-aware
    const uint8_t* data   = buf.buffer();
    if (!BufferBase::ensure_size(static_cast<uint32_t>(sizeof(uint32_t)) + length))
        return 0u;

    write_bytes(reinterpret_cast<const uint8_t*>(&length), sizeof(uint32_t));
    return (length != 0u ? write_bytes(data, length) : 0u);
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::read(SharedBuffer& buf) const noexcept
{
    if (static_cast<const BufferBase*>(this) == static_cast<const BufferBase*>(&buf))
        return size_used();

    const uint32_t start{ mPosition };
    uint32_t length = 0u;
    if (read_data(reinterpret_cast<uint8_t*>(&length), sizeof(uint32_t)) != sizeof(uint32_t))
    {
        mPosition = start;
        return 0u;
    }

    if (length == 0u)
    {
        buf.invalidate();
        return 0u;
    }

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if ((length > raw->bufHeader.biUsed - mPosition) || (buf.reserve(length, false) < length))
    {
        mPosition = start;
        return 0u;
    }

    areg::mem_copy(buf.buffer(), length, areg::buffer_data_read(raw) + mPosition, length);
    buf.set_size_used(length);
    buf.move_to_begin();

    mPosition += length;
    return length;
}

//////////////////////////////////////////////////////////////////////////
// Additional streaming operations
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::insert_at(const uint8_t* buf, uint32_t size, uint32_t atPos)
{
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    if (!can_write())   // false only while a SharedBuffer is a read-only view
        return 0u;

    const uint32_t writeEnd = is_valid() ? mByteBuffer->bufHeader.biUsed : 0u;
    if (size > areg::MAX_BUF_LENGTH - std::min(writeEnd, areg::MAX_BUF_LENGTH))
        return 0u;

    if (!is_valid() || (atPos >= writeEnd))
    {
        const uint32_t needed = writeEnd + size;
        _grow_block(grow_size(writeEnd, needed));
        areg::RawBuffer* const raw = mByteBuffer.get();
        if ((raw == nullptr) || (needed > raw->bufHeader.biLength))
            return 0u;

        ::memcpy(areg::buffer_data_write(raw) + writeEnd, buf, size);

        raw->bufHeader.biUsed = needed;
        return size;
    }
    else
    {
        const uint32_t needed = writeEnd + size;
        if (reserve(grow_size(writeEnd, needed), true) < size)
            return 0u;

        uint8_t* dst = areg::buffer_data_write(mByteBuffer.get()) + atPos;
        const uint32_t moveSize = writeEnd - atPos;

        areg::mem_move(dst + size, dst, moveSize);
        areg::mem_copy(dst, size, buf, size);
        mByteBuffer->bufHeader.biUsed = writeEnd + size;
        return size;
    }
}

bool BufferBase::is_equal(const BufferBase& other) const noexcept
{
    if (static_cast<const BufferBase*>(this) == &other)
        return true;
    else if (!is_valid() || !other.is_valid())
        return false;

    const uint32_t used = size_used();  // view-aware
    if (used != other.size_used())
        return false;

    const uint8_t* data1 = areg::buffer_data_read(mByteBuffer.get());
    const uint8_t* data2 = areg::buffer_data_read(other.mByteBuffer.get());
    return areg::mem_equal(data1, data2, used);
}

BufferBase BufferBase::clone() const
{
    const uint32_t used = size_used();
    BufferBase   result(mBlockSize);
    if ((used != 0u) && (result.reserve(used, false) >= used))
    {
        const uint8_t* src = areg::buffer_data_read(mByteBuffer.get());
        areg::mem_copy(result.buffer(), used, src, used);
        result.mByteBuffer->bufHeader.biUsed = used;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// BufferBase protected overrides
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::data_offset() const noexcept
{
    return sizeof(areg::BufferHeader);
}

uint32_t BufferBase::header_size() const noexcept
{
    return sizeof(areg::RawBuffer);
}

bool BufferBase::can_write() const noexcept
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Hot-path helpers
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::write_data(const uint8_t* buf, uint32_t size) noexcept
{
    return can_write() ? write_bytes(buf, size) : 0u;
}

uint32_t BufferBase::write_bytes(const uint8_t* buf, uint32_t size) noexcept
{
    areg::RawBuffer* const raw  = mByteBuffer.get();
    const uint32_t writePos     = mPosition;
    const uint32_t needed       = writePos + size;

    if ((raw == nullptr) || (needed > raw->bufHeader.biLength) || (needed < writePos) || (size == 0u) || (buf == nullptr))
        return _write_grow(buf, size);

    areg::mem_copy_exact(areg::buffer_data_write(raw) + writePos, buf, size);
    if (needed > raw->bufHeader.biUsed)
        raw->bufHeader.biUsed = needed;

    mPosition = needed;
    return size;
}

uint32_t BufferBase::_write_grow(const uint8_t* buf, uint32_t size) noexcept
{
    const uint32_t writePos = mPosition;
    const uint32_t needed   = writePos + size;
    if ((size == 0u) || (buf == nullptr) || (needed < writePos) || (needed > areg::MAX_BUF_LENGTH))
        return 0u;

    _grow_block(grow_size(size_used(), needed));
    areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (needed > raw->bufHeader.biLength))
        return 0u;

    areg::mem_copy_exact(areg::buffer_data_write(raw) + writePos, buf, size);
    raw->bufHeader.biUsed = needed;
    mPosition = needed;
    return size;
}

uint32_t BufferBase::read_data(uint8_t* buf, uint32_t size) const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (size == 0u) || (buf == nullptr))
        return 0u;

    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition >= limit)
        return 0u;

    const uint32_t avail  = limit - mPosition;
    const uint32_t result = std::min(avail, size);
    areg::mem_copy_exact(buf, size, areg::buffer_data_read(raw) + mPosition, result);
    mPosition += result;
    return result;
}

const uint8_t* BufferBase::read_view(uint32_t size) const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (size == 0u))
        return nullptr;

    const uint32_t limit = raw->bufHeader.biUsed;
    if ((mPosition > limit) || (size > (limit - mPosition)))
        return nullptr;

    const uint8_t* const result = areg::buffer_data_read(raw) + mPosition;
    mPosition += size;
    return result;
}

uint8_t* BufferBase::write_view(uint32_t size) noexcept
{
    areg::RawBuffer* const raw  = mByteBuffer.get();
    const uint32_t writePos     = mPosition;
    const uint32_t needed       = writePos + size;

    if ((raw == nullptr) || (needed > raw->bufHeader.biLength) || (needed < writePos) || (size == 0u))
        return _view_grow(size);

    if (needed > raw->bufHeader.biUsed)
        raw->bufHeader.biUsed = needed;

    mPosition = needed;
    return areg::buffer_data_write(raw) + writePos;
}

uint8_t* BufferBase::_view_grow(uint32_t size) noexcept
{
    const uint32_t writePos = mPosition;
    const uint32_t needed   = writePos + size;
    if ((size == 0u) || (needed < writePos))
        return nullptr;

    _grow_block(grow_size(size_used(), needed));
    areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (needed > raw->bufHeader.biLength))
        return nullptr;

    raw->bufHeader.biUsed = needed;
    mPosition = needed;
    return areg::buffer_data_write(raw) + writePos;
}

} // namespace areg
