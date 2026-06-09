/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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

#include "areg/appbase/Application.hpp"

#include <algorithm>
#include <atomic>

namespace areg {

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
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<char>(textString)) + 1u) * sizeof(char);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringA.data()), size);
}

BufferBase::BufferBase(const wchar_t* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : IOStream      ( )
    , Cursor        ( )
    , mBlockSize    ( areg::align_size(blockSize, areg::BLOCK_SIZE) )
    , mByteBuffer   ( )
    , mPosition     ( 0u )
{
    const uint32_t size = (static_cast<uint32_t>(areg::string_length<wchar_t>(textString)) + 1u) * sizeof(wchar_t);
    write_data(reinterpret_cast<const uint8_t*>(textString != nullptr ? textString : areg::EmptyStringW.data()), size);
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

    switch (startAt)
    {
    case Cursor::SeekOrigin::Begin:
        curPos = 0;
        offset = offset < 0 ? 0 : std::min(offset, rangeSize);
        break;

    case Cursor::SeekOrigin::Current:
        offset = offset < 0
               ? std::max(offset, -curPos)
               : std::min(offset, rangeSize - curPos);
        break;

    case Cursor::SeekOrigin::End:
        curPos = rangeSize;
        offset = offset < 0 ? std::max(offset, -rangeSize) : 0;
        break;

    default:
        ASSERT(false);
    }

    mPosition = static_cast<uint32_t>(curPos + offset);
    return mPosition;
}

//////////////////////////////////////////////////////////////////////////
// BufferBase methods
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::reserve(uint32_t size, bool copy)
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
    uint32_t total             = (preserveData && (raw != nullptr)) ? used + size : size;
    total = std::min(total, BufferBase::MAX_BUF_LENGTH);

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
    const uint32_t sizeBuffer = header_size() + total;
    uint8_t* buffer = DEBUG_NEW uint8_t[sizeBuffer];
    const uint32_t position = init_buffer(buffer, sizeBuffer, preserveData);
    if (position != Cursor::INVALID_CURSOR_POSITION)
    {
        invalidate();
        mByteBuffer = areg::RawBufferPtr(reinterpret_cast<areg::RawBuffer*>(buffer));
        mPosition   = position;
        return (mByteBuffer->bufHeader.biLength - mPosition);
    }
    else
    {
        invalidate();
        return 0u;
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
    else if (!buf.is_valid())
        return 0u;

    const uint32_t length = buf.size_used();  // source may be a view; size_used()/buffer() are view-aware
    const uint8_t* data   = buf.buffer();
    reserve(mPosition + static_cast<uint32_t>(sizeof(uint32_t)) + length, true);
    return (write_data(reinterpret_cast<const uint8_t*>(&length), sizeof(uint32_t)) == sizeof(uint32_t)) ? write_data(data, length) : 0u;
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t BufferBase::read(SharedBuffer& buf) const noexcept
{
    if (static_cast<const BufferBase*>(this) == static_cast<const BufferBase*>(&buf))
        return size_used();

    uint32_t length = 0u;
    if ((read_data(reinterpret_cast<uint8_t*>(&length), sizeof(uint32_t)) != sizeof(uint32_t)) || (length == 0u))
        return 0u;

    const areg::RawBuffer* const raw = mByteBuffer.get();
    const uint32_t srcEnd = (raw != nullptr ? raw->bufHeader.biUsed : 0u);
    if (mPosition + length > srcEnd)
    {
        // Not enough data, undo the 4-byte length read
        mPosition-= sizeof(uint32_t);
        return 0u;
    }

    if (buf.reserve(length, false) < length)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    areg::mem_copy(buf.buffer(), length, areg::buffer_data_read(raw) + mPosition, length);
    buf.set_size_used(length);
    buf.move_to_begin();

    mPosition += length;
    return length;
}

uint32_t BufferBase::read(String& ascii) const
{
    ascii.clear();

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return 0u;

    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition >= limit)
        return 0u;

    const char* const src   = reinterpret_cast<const char*>(areg::buffer_data_read(raw) + mPosition);
    const uint32_t    avail = limit - mPosition;
    const char* const nul   = static_cast<const char*>(::memchr(src, '\0', avail));
    const uint32_t    len   = (nul != nullptr) ? static_cast<uint32_t>(nul - src) : avail;

    if (len > 0u)
        ascii.assign(src, static_cast<areg::CharCount>(len));

    const uint32_t advance = (nul != nullptr) ? len + 1u : len;
    mPosition += advance;
    return advance;
}

uint32_t BufferBase::read(WideString& wide) const
{
    wide.clear();

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return 0u;

    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition >= limit)
        return 0u;

    const uint32_t     avail      = limit - mPosition;
    const uint32_t     max_chars  = avail / static_cast<uint32_t>(sizeof(wchar_t));
    if (max_chars == 0u)
        return 0u;

    const wchar_t* const src = reinterpret_cast<const wchar_t*>(areg::buffer_data_read(raw) + mPosition);
    const wchar_t* const nul = static_cast<const wchar_t*>(std::wmemchr(src, L'\0', max_chars));
    const uint32_t       len = (nul != nullptr) ? static_cast<uint32_t>(nul - src) : max_chars;

    if (len > 0u)
        wide.assign(src, static_cast<areg::CharCount>(len));

    const uint32_t advance = (nul != nullptr) ? (len + 1u) * static_cast<uint32_t>(sizeof(wchar_t)) : len * static_cast<uint32_t>(sizeof(wchar_t));
    mPosition += advance;
    return advance;
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
    if (!is_valid() || (atPos >= writeEnd))
    {
        const uint32_t needed = writeEnd + size;
        reserve(needed, true);
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
        if (reserve(needed, true) < size)
            return 0u;

        ASSERT(is_valid());
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

uint32_t BufferBase::default_block_size() noexcept
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

uint32_t BufferBase::write_data(const uint8_t* buf, uint32_t size) noexcept
{
    if ((size == 0u) || (buf == nullptr))
        return 0u;

    if (!can_write())
        return 0u;

    areg::RawBuffer* const raw  = mByteBuffer.get();
    const uint32_t writePos     = mPosition;
    const uint32_t needed       = writePos + size;

    if ((raw != nullptr) && (needed <= raw->bufHeader.biLength))
    {
        areg::mem_copy(areg::buffer_data_write(raw) + writePos, raw->bufHeader.biLength - writePos, buf, size);
        if (needed > raw->bufHeader.biUsed)
            raw->bufHeader.biUsed = needed;

        mPosition= needed;
        return size;
    }
    else
    {
        // Slow path: grow the buffer (doubling strategy inside reserve).
        reserve(needed, true);
        areg::RawBuffer* const grownRaw = mByteBuffer.get();
        if ((grownRaw == nullptr) || (needed > grownRaw->bufHeader.biLength))
            return 0u;

        areg::mem_copy(areg::buffer_data_write(grownRaw) + writePos, grownRaw->bufHeader.biLength - writePos, buf, size);
        if (needed > grownRaw->bufHeader.biUsed)
            grownRaw->bufHeader.biUsed = needed;

        mPosition = needed;
        return size;
    }
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
    areg::mem_copy(buf, size, areg::buffer_data_read(raw) + mPosition, result);
    mPosition += result;
    return result;
}

uint32_t BufferBase::read_string_bin(String& str) const noexcept
{
    str.clear();

    uint32_t byte_count = 0u;
    if (read_data(reinterpret_cast<uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;

    if (byte_count == 0u)
        return sizeof(uint32_t);

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition + byte_count > limit)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    const char* src = reinterpret_cast<const char*>(areg::buffer_data_read(raw) + mPosition);
    str.assign(src, static_cast<areg::CharCount>(byte_count));
    mPosition += byte_count;
    return sizeof(uint32_t) + byte_count;
}

uint32_t BufferBase::read_string_bin(WideString& str) const noexcept
{
    str.clear();

    uint32_t byte_count = 0u;
    if (read_data(reinterpret_cast<uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;

    if (byte_count == 0u)
        return sizeof(uint32_t);

    const uint32_t char_count = byte_count / static_cast<uint32_t>(sizeof(wchar_t));
    if (char_count == 0u)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition + byte_count > limit)
    {
        mPosition -= sizeof(uint32_t);
        return 0u;
    }

    const wchar_t* src = reinterpret_cast<const wchar_t*>(areg::buffer_data_read(raw) + mPosition);
    str.assign(src, static_cast<areg::CharCount>(char_count));
    mPosition += byte_count;
    return sizeof(uint32_t) + byte_count;
}

} // namespace areg
