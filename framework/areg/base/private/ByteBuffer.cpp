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

namespace areg
{
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
        if (size != 0 )
        {
            // check that it is not shared
            if (mByteBuffer.use_count() <= 1)
            {
                size = size > ByteBuffer::MAX_BUF_LENGTH ? ByteBuffer::MAX_BUF_LENGTH : size;
                uint32_t sizeLength{ isValid() ? mByteBuffer->bufHeader.biLength : 0 };

                // If not enough space
                if (size > sizeLength)
                {
                    uint32_t sizeAlign{ getAlignedSize() };
                    uint32_t sizeBuffer{ getHeaderSize() + size };

                    sizeBuffer = areg::alignSize(sizeBuffer, sizeAlign);
                    uint8_t* buffer = DEBUG_NEW uint8_t[sizeBuffer];
                    int32_t copied = static_cast<int32_t>(initBuffer(buffer, sizeBuffer, copy));
                    if (static_cast<uint32_t>(copied) != areg::Cursor::INVALID_CURSOR_POSITION)
                    {
                        areg::RawBuffer * temp = reinterpret_cast<areg::RawBuffer *>(buffer);
                        mByteBuffer = std::shared_ptr<areg::RawBuffer>(temp, ByteBufferDeleter());
                    }
                    else
                    {
                        delete[] buffer;
                    }
                }
            }
        }
        else
        {
            invalidate();
        }

        return (isValid() ? mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed : 0);
    }

    uint32_t ByteBuffer::initBuffer(uint8_t * newBuffer, uint32_t bufLength, bool makeCopy) const
    {
        uint32_t result = areg::Cursor::INVALID_CURSOR_POSITION;

        if ( newBuffer != nullptr )
        {
            result                      = 0;
            uint32_t dataOffset     = getDataOffset();
            uint32_t dataLength     = bufLength - dataOffset;

            areg::RawBuffer* buffer= new(newBuffer)areg::RawBuffer;
            buffer->bufHeader.biBufSize = bufLength;
            buffer->bufHeader.biLength  = dataLength;
            buffer->bufHeader.biOffset  = dataOffset;
            buffer->bufHeader.biBufType = areg::BufferType::Internal;

            if (makeCopy && (mByteBuffer.get() != nullptr))
            {
                uint8_t* data         = newBuffer + dataOffset;
                const uint8_t* srcBuf = areg::getBufferDataRead(mByteBuffer.get());
                uint32_t srcCount       = mByteBuffer->bufHeader.biUsed;
                srcCount                    = std::min(srcCount, dataLength);
                result                      = srcCount;

                buffer->bufHeader.biUsed    = srcCount;
                ::memcpy(data, srcBuf, srcCount);
            }
            else
            {
                buffer->bufHeader.biUsed    = 0;
            }
        }

        return result;
    }

    uint32_t ByteBuffer::getAlignedSize() const
    {
        return areg::BLOCK_SIZE;
    }

} // namespace areg