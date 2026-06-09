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

#include "areg/base/MathDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/WideString.hpp"

#include <algorithm>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////

SharedBuffer::SharedBuffer(uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : BufferBase    ( blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, uint32_t blockSize)
    : BufferBase    ( reserveSize, blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : BufferBase    ( buffer, size, blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : BufferBase    ( reserveSize, buffer, size, blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(const char* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : BufferBase    ( textString, blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(const wchar_t* textString, uint32_t blockSize /* = areg::BLOCK_SIZE */)
    : BufferBase    ( textString, blockSize )
    , mViewStart    ( 0u )
    , mViewEnd      ( 0u )
{
}

SharedBuffer::SharedBuffer(const SharedBuffer& src) noexcept
    : BufferBase    ( src )         // shares the block + copies block size; cursor reset to 0
    , mViewStart    ( src.mViewStart )
    , mViewEnd      ( src.mViewEnd )
{
    mPosition = src.mViewStart;     // preserve the visible slice start
}

SharedBuffer::SharedBuffer(SharedBuffer&& src) noexcept
    : BufferBase    ( std::move(src) )
    , mViewStart    ( src.mViewStart )
    , mViewEnd      ( src.mViewEnd )
{
    mPosition      = mViewStart;
    src.mViewStart = 0u;
    src.mViewEnd   = 0u;
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
            mPosition   = src.mViewStart;
            mViewStart  = src.mViewStart;
            mViewEnd    = src.mViewEnd;
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
            mPosition       = src.mViewStart;
            mViewStart      = src.mViewStart;
            mViewEnd        = src.mViewEnd;
            src.mPosition   = 0u;
            src.mViewStart  = 0u;
            src.mViewEnd    = 0u;
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
    if (!is_valid())
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t viewStart  = mViewStart;
    const uint32_t viewEnd    = (mViewEnd != 0u ? mViewEnd : mByteBuffer->bufHeader.biUsed);
    const int32_t  rangeSize  = static_cast<int32_t>(viewEnd - viewStart);
    int32_t        curPos     = static_cast<int32_t>(mPosition - viewStart);

    switch (startAt)
    {
    case Cursor::SeekOrigin::Begin:
        curPos = 0;
        offset = offset < 0 ? 0 : std::min(offset, rangeSize);
        break;

    case Cursor::SeekOrigin::Current:
        offset = offset < 0 ? std::max(offset, -curPos) : std::min(offset, rangeSize - curPos);
        break;

    case Cursor::SeekOrigin::End:
        curPos = rangeSize;
        offset = offset < 0 ? std::max(offset, -rangeSize) : 0;
        break;

    default:
        ASSERT(false);
    }

    mPosition = viewStart + static_cast<uint32_t>(curPos + offset);
    return (mPosition - mViewStart);
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

    const areg::RawBuffer* raw = mByteBuffer.get();
    const bool hasView         = (mViewEnd != 0u);
    const bool preserveData    = copy || hasView;
    const uint32_t used        = preserveData ? size_used() : 0u;
    uint32_t total             = (preserveData && (raw != nullptr)) ? used + size : size;
    total = std::min(total, areg::MAX_BUF_LENGTH);

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

uint32_t SharedBuffer::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
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
    areg::mem_copy(newBuffer + dataOffset, dataLength, areg::buffer_data_read(mByteBuffer.get()) + mViewStart, srcCount);
    hdrDst.biUsed = srcCount;

    return srcCount;
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::write(const SharedBuffer& buf)
{
    if (static_cast<const SharedBuffer*>(this) == static_cast<const SharedBuffer*>(&buf))
        return size_used();
    else if (!buf.is_valid())
        return 0u;

    const uint32_t length = buf.size_used();  // view-aware: mViewEnd-mViewStart or biUsed
    const uint8_t* data   = areg::buffer_data_read(buf.mByteBuffer.get()) + buf.mViewStart;
    reserve(mPosition + static_cast<uint32_t>(sizeof(uint32_t)) + length, true);
    return (write_data(reinterpret_cast<const uint8_t*>(&length), sizeof(uint32_t)) == sizeof(uint32_t)) ? write_data(data, length) : 0u;
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::read(SharedBuffer& buf) const noexcept
{
    if (static_cast<const SharedBuffer*>(this) == static_cast<const SharedBuffer*>(&buf))
        return size_used();

    uint32_t length = 0u;
    if ((read_data(reinterpret_cast<uint8_t*>(&length), sizeof(uint32_t)) != sizeof(uint32_t)) || (length == 0u))
        return 0u;

    const areg::RawBuffer* const raw = mByteBuffer.get();
    const uint32_t srcEnd = (mViewEnd > 0u ? mViewEnd : (raw != nullptr ? raw->bufHeader.biUsed : 0u));
    if (mPosition + length > srcEnd)
    {
        // Not enough data, undo the 4-byte length read
        mPosition-= sizeof(uint32_t);
        return 0u;
    }

    // Zero-copy: share underlying allocation, expose [mPosition, mPosition+length).
    buf.mByteBuffer = mByteBuffer;
    buf.mViewStart  = mPosition;
    buf.mViewEnd    = mPosition + length;
    buf.mPosition   = mPosition;

    mPosition += length;
    return length;
}

uint32_t SharedBuffer::read(String& ascii) const
{
    ascii.clear();

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return 0u;

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : raw->bufHeader.biUsed);
    if (mPosition >= limit)
        return 0u;

    // Bounded scan: find NUL within the view/buffer window to prevent overrun.
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

uint32_t SharedBuffer::read(WideString& wide) const
{
    wide.clear();

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return 0u;

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : raw->bufHeader.biUsed);
    if (mPosition >= limit)
        return 0u;

    const uint32_t     avail      = limit - mPosition;
    const uint32_t     max_chars  = avail / static_cast<uint32_t>(sizeof(wchar_t));
    if (max_chars == 0u)
        return 0u;

    // Bounded scan: find NUL within the view/buffer window to prevent overrun.
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

bool SharedBuffer::is_equal(const SharedBuffer& other) const noexcept
{
    if (static_cast<const SharedBuffer*>(this) == &other)
        return true;
    else if (!is_valid() || !other.is_valid())
        return false;

    const uint32_t used = size_used();  // view-aware
    if (used != other.size_used())
        return false;

    const uint8_t* data1 = areg::buffer_data_read(mByteBuffer.get())       + mViewStart;
    const uint8_t* data2 = areg::buffer_data_read(other.mByteBuffer.get()) + other.mViewStart;
    return areg::mem_equal(data1, data2, used);
}

SharedBuffer SharedBuffer::clone() const
{
    const uint32_t used = size_used();  // view-aware: mViewEnd-mViewStart or biUsed
    SharedBuffer   result(mBlockSize);
    if ((used != 0u) && (result.reserve(used, false) >= used))
    {
        const uint8_t* src = areg::buffer_data_read(mByteBuffer.get()) + mViewStart;
        areg::mem_copy(result.buffer(), used, src, used);
        result.mByteBuffer->bufHeader.biUsed = used;
    }

    return result;
}

SharedBuffer SharedBuffer::detach() const
{
    return clone();
}

//////////////////////////////////////////////////////////////////////////
// Hot-path helpers (view-aware overrides)
//////////////////////////////////////////////////////////////////////////

uint32_t SharedBuffer::read_data(uint8_t* buf, uint32_t size) const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if ((raw == nullptr) || (size == 0u) || (buf == nullptr))
        return 0u;

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : raw->bufHeader.biUsed);
    if (mPosition >= limit)
        return 0u;

    const uint32_t avail  = limit - mPosition;
    const uint32_t result = std::min(avail, size);
    areg::mem_copy(buf, size, areg::buffer_data_read(raw) + mPosition, result);
    mPosition += result;
    return result;
}

uint32_t SharedBuffer::read_string_bin(String& str) const noexcept
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

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : raw->bufHeader.biUsed);
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

uint32_t SharedBuffer::read_string_bin(WideString& str) const noexcept
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

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : raw->bufHeader.biUsed);
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
