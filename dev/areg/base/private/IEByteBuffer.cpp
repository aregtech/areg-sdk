/************************************************************************
 * \file        areg/base/private/IEByteBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Byte Buffer interface
 *
 ************************************************************************/
#include "areg/base/IEByteBuffer.hpp"

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

unsigned int IEByteBuffer::resize(unsigned int size, bool copy)
{
    if (size != 0 )
    {
        // If not enough space
        unsigned int sizeLength = getSizeAvailable();
        if (isShared() == false)
        {
            unsigned int sizeUsed   = this->getSizeUsed();
            unsigned int sizeAlign  = this->getAlignedSize();
            size = size > IEByteBuffer::MAX_BUF_LENGTH ? IEByteBuffer::MAX_BUF_LENGTH : size;
            if ( (size > sizeLength) || (size < sizeUsed) )
            {
                unsigned int sizeBuffer = getHeaderSize() + size;
                sizeBuffer = MACRO_ALIGN_SIZE(sizeBuffer, sizeAlign);
                unsigned char* buffer = DEBUG_NEW unsigned char[sizeBuffer];
                int copied = static_cast<int>(initBuffer(buffer, sizeBuffer, copy));
                if (static_cast<unsigned int>(copied) != IECursorPosition::INVALID_CURSOR_POSITION)
                {
                    removeReference();
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
        removeReference();
    }

    return (isValid() ? mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed : 0);
}

unsigned int IEByteBuffer::initBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (newBuffer != NULL)
    {
        result                      = 0;
        unsigned int dataOffset     = this->getDataOffset();
        unsigned int dataLength     = bufLength - dataOffset;

        NEMemory::sByteBuffer* buffer= NEMemory::constructElems<NEMemory::sByteBuffer>(newBuffer, 1);    
        buffer->bufHeader.biBufSize = bufLength;
        buffer->bufHeader.biLength  = dataLength;
        buffer->bufHeader.biOffset  = dataOffset;
        buffer->bufHeader.biBufType = NEMemory::BufferInternal;
        buffer->bufHeader.biRefCount= 1;

        if ( makeCopy )
        {
            unsigned char* data         = NEMemory::getBufferDataWrite(buffer);
            const unsigned char* srcBuf = NEMemory::getBufferDataRead(mByteBuffer);
            unsigned int srcCount       = getSizeUsed();
            srcCount                    = MACRO_MIN(srcCount, dataLength);
            result                      = srcCount;

            buffer->bufHeader.biUsed    = srcCount;
            NEMemory::memCopy(data, dataLength, srcBuf, srcCount);
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

unsigned int IEByteBuffer::getAlignedSize(void) const
{
    return NEMemory::BLOCK_SIZE;
}
