/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/IEByteBuffer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Byte Buffer interface
 *
 ************************************************************************/
#include "areg/base/IEByteBuffer.hpp"

#include <utility>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
// IEByteBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

IEByteBuffer::IEByteBuffer( void )
    : mByteBuffer( nullptr, ByteBufferDeleter() )
{
}

IEByteBuffer::IEByteBuffer( NEMemory::sByteBuffer & byteBuffer )
    : mByteBuffer( &byteBuffer, ByteBufferDeleter( ) )
{
}

IEByteBuffer::IEByteBuffer( IEByteBuffer && src ) noexcept
    : mByteBuffer   ( std::move(src.mByteBuffer) )
{
    src.mByteBuffer.reset();
    src.invalidate();
}

void IEByteBuffer::invalidate( void )
{
    mByteBuffer.reset();
}

unsigned int IEByteBuffer::reserve(unsigned int size, bool copy)
{
    if (size != 0 )
    {
        // check that it is not shared
        if (mByteBuffer.use_count() <= 1)
        {
            size = size > IEByteBuffer::MAX_BUF_LENGTH ? IEByteBuffer::MAX_BUF_LENGTH : size;
            unsigned int sizeLength{ isValid() ? mByteBuffer->bufHeader.biLength : 0 };

            // If not enough space
            if (size > sizeLength)
            {
                unsigned int sizeAlign{ getAlignedSize() };
                unsigned int sizeBuffer{ getHeaderSize() + size };

                sizeBuffer = MACRO_ALIGN_SIZE(sizeBuffer, sizeAlign);
                unsigned char* buffer = DEBUG_NEW unsigned char[sizeBuffer];
                int copied = static_cast<int>(initBuffer(buffer, sizeBuffer, copy));
                if (static_cast<unsigned int>(copied) != IECursorPosition::INVALID_CURSOR_POSITION)
                {
                    NEMemory::sByteBuffer * temp = reinterpret_cast<NEMemory::sByteBuffer *>(buffer);
                    mByteBuffer = std::shared_ptr<NEMemory::sByteBuffer>(temp, ByteBufferDeleter());
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

unsigned int IEByteBuffer::initBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if ( newBuffer != nullptr )
    {
        result                      = 0;
        unsigned int dataOffset     = getDataOffset();
        unsigned int dataLength     = bufLength - dataOffset;

        NEMemory::sByteBuffer* buffer= new(newBuffer)NEMemory::sByteBuffer;
        buffer->bufHeader.biBufSize = bufLength;
        buffer->bufHeader.biLength  = dataLength;
        buffer->bufHeader.biOffset  = dataOffset;
        buffer->bufHeader.biBufType = NEMemory::eBufferType::BufferInternal;

        if (makeCopy && (mByteBuffer.get() != nullptr))
        {
            unsigned char* data         = newBuffer + dataOffset;
            const unsigned char* srcBuf = NEMemory::getBufferDataRead(mByteBuffer.get());
            unsigned int srcCount       = mByteBuffer->bufHeader.biUsed;
            srcCount                    = MACRO_MIN(srcCount, dataLength);
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

unsigned int IEByteBuffer::getAlignedSize(void) const
{
    return NEMemory::BLOCK_SIZE;
}
