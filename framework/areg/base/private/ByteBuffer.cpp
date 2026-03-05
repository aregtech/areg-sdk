/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ByteBuffer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Byte Buffer interface
 *
 ************************************************************************/
#include "areg/base/ByteBuffer.hpp"
#include "areg/base/MathDefs.hpp"

#include <algorithm>
#include <utility>
#include <string.h>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ByteBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

ByteBuffer::ByteBuffer()
    : mByteBuffer( nullptr, ByteBufferDeleter() )
{
}

ByteBuffer::ByteBuffer( areg::RawBuffer & byteBuffer )
    : mByteBuffer( &byteBuffer, ByteBufferDeleter( ) )
{
}

ByteBuffer::ByteBuffer( ByteBuffer && src ) noexcept
    : mByteBuffer   ( std::move(src.mByteBuffer) )
{
    src.mByteBuffer.reset();
    src.invalidate();
}

void ByteBuffer::invalidate()
{
    mByteBuffer.reset();
}

uint32_t ByteBuffer::reserve(uint32_t size, bool copy)
{
    if (size == 0)
    {
        invalidate();
        return 0u;
    }

    if (mByteBuffer.use_count() > 1)
        return is_valid() ? (mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed) : 0u;

    size = std::min(size, ByteBuffer::MAX_BUF_LENGTH);

    const uint32_t curLength = is_valid() ? mByteBuffer->bufHeader.biLength : 0u;
    if (size <= curLength)
        return curLength - mByteBuffer->bufHeader.biUsed;

    // Doubling growth strategy: amortizes repeated reserve calls (O(log n) allocations)
    const uint32_t grownSize  = std::min(std::max(size, curLength * 2u), ByteBuffer::MAX_BUF_LENGTH);
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

uint32_t ByteBuffer::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const
{
    if (newBuffer == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t dataOffset = data_offset();
    const uint32_t dataLength = bufLength - dataOffset;

    areg::RawBuffer* buffer     = new(newBuffer) areg::RawBuffer;
    buffer->bufHeader.biBufSize = bufLength;
    buffer->bufHeader.biLength  = dataLength;
    buffer->bufHeader.biOffset  = dataOffset;
    buffer->bufHeader.biBufType = areg::BufferType::Internal;
    buffer->bufHeader.biUsed    = 0u;

    if (!makeCopy || (mByteBuffer == nullptr))
        return 0u;

    const uint32_t srcCount = std::min(mByteBuffer->bufHeader.biUsed, dataLength);
    ::memcpy(newBuffer + dataOffset, areg::buffer_data_read(mByteBuffer.get()), srcCount);
    buffer->bufHeader.biUsed = srcCount;

    return srcCount;
}

uint32_t ByteBuffer::aligned_size() const noexcept
{
    return areg::BLOCK_SIZE;
}

} // namespace areg
