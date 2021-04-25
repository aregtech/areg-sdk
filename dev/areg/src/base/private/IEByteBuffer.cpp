/************************************************************************
 * \file        areg/src/base/private/IEByteBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Byte Buffer interface
 *
 ************************************************************************/
#include "areg/src/base/IEByteBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// IEByteBuffer pure virtual class implementation
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Maximum length of byte buffer. It is defined as 1 Mb.
 **/
const unsigned int IEByteBuffer::MAX_BUF_LENGTH = static_cast<unsigned int>(67108864); /*0x04000000*/

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IEByteBuffer::IEByteBuffer( NEMemory::sByteBuffer & byteBuffer )
    : mByteBuffer   ( &byteBuffer )
{
    ; // do nothing
}

IEByteBuffer::~IEByteBuffer( void )
{
    ; // do nothing
}

unsigned int IEByteBuffer::ResizeBuffer(unsigned int size, bool copy)
{
    if (size != 0 )
    {
        // If not enough space
        unsigned int sizeLength = GetLength();
        if (IsShared() == false)
        {
            unsigned int sizeUsed   = GetUsedSize();
            unsigned int sizeAlign  = SizeAlignment();
            size = size > IEByteBuffer::MAX_BUF_LENGTH ? IEByteBuffer::MAX_BUF_LENGTH : size;
            if ( (size > sizeLength) || (size < sizeUsed) )
            {
                unsigned int sizeBuffer = SizeBufferStruct() + size;
                sizeBuffer = MACRO_ALIGN_SIZE(sizeBuffer, sizeAlign);
                unsigned char* buffer = DEBUG_NEW unsigned char[sizeBuffer];
                int copied = static_cast<int>(InitBuffer(buffer, sizeBuffer, copy));
                if (static_cast<unsigned int>(copied) != IECursorPosition::INVALID_CURSOR_POSITION)
                {
                    RemoveReference();
                    mByteBuffer = reinterpret_cast<NEMemory::sByteBuffer *>(buffer);
                }
                else
                {
                    if (buffer != NULL)
                        delete [] buffer;
                }
            }
        }
        else
        {
            OUTPUT_WARN("Ignoring resizing shared buffer.");
        }
    }
    else
    {
        RemoveReference();
    }

    return (IsValid() ? mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed : 0);
}

unsigned int IEByteBuffer::InitBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (newBuffer != NULL)
    {
        result                      = 0;
        unsigned int dataOffset     = SizeDataOffset();
        unsigned int dataLength     = bufLength - dataOffset;

        NEMemory::sByteBuffer* buffer= NEMemory::ConstructElems<NEMemory::sByteBuffer>(newBuffer, 1);    
        buffer->bufHeader.biBufSize = bufLength;
        buffer->bufHeader.biLength  = dataLength;
        buffer->bufHeader.biOffset  = dataOffset;
        buffer->bufHeader.biBufType = NEMemory::BufferInternal;
        buffer->bufHeader.biRefCount= 1;

        if ( makeCopy )
        {
            unsigned char* data         = NEMemory::GetBufferDataWrite(buffer);
            const unsigned char* srcBuf = NEMemory::GetBufferDataRead(mByteBuffer);
            unsigned int srcCount       = GetUsedSize();
            srcCount                    = MACRO_MIN(srcCount, dataLength);
            result                      = srcCount;

            buffer->bufHeader.biUsed    = srcCount;
            NEMemory::MemCopy(data, dataLength, srcBuf, srcCount);
        }
        else
        {
            buffer->bufHeader.biUsed    = 0;
        }
    }
    else
    {
        ; // do nothing
    }

    return result;
}

unsigned int IEByteBuffer::SizeAlignment(void) const
{
    return NEMemory::BLOCK_SIZE;
}
