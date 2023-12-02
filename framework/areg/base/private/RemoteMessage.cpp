/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RemoteMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between different processes running either on same or 
 *              different hosts.
 *
 ************************************************************************/
#include "areg/base/RemoteMessage.hpp"

#include "areg/base/NEMemory.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/trace/GETrace.h"

#include <string.h>
#include <cstddef>

inline unsigned int RemoteMessage::_checksumCalculate( const NEMemory::sRemoteMessage & remoteMessage )
{
    const unsigned int offset   = MACRO_OFFSETOF( NEMemory::sRemoteMessageHeader, rbhSource );
    const unsigned char * data  = reinterpret_cast<const unsigned char *>(&remoteMessage.rbHeader.rbhSource);
    const unsigned int remain   = remoteMessage.rbHeader.rbhBufHeader.biOffset - offset;
    const unsigned int used     = remoteMessage.rbHeader.rbhBufHeader.biUsed;
    const unsigned int length   = used + remain;

    return NEMath::crc32Calculate( data, static_cast<int>(length) );
}

RemoteMessage::RemoteMessage(unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
}

RemoteMessage::RemoteMessage(unsigned int reserveSize, unsigned int blockSize)
    : SharedBuffer  ( blockSize )
{
    reserve(reserveSize, false);
}

RemoteMessage::RemoteMessage(const unsigned char * buffer, unsigned int size, unsigned int blockSize /*= NEMemory::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
    reserve(size, false);
    writeData(buffer, size);
}

unsigned int RemoteMessage::initBuffer(unsigned char *newBuffer, unsigned int bufLength, bool makeCopy) const
{
    unsigned int result{ IECursorPosition::INVALID_CURSOR_POSITION };

    if (newBuffer != nullptr)
    {
        result = 0;
        unsigned int dataOffset { getDataOffset() };
        unsigned int dataLength { bufLength - dataOffset };

        NEMemory::memZero(newBuffer, sizeof(NEMemory::sRemoteMessage));
        NEMemory::sRemoteMessageHeader & header = NEMemory::constructElems<NEMemory::sRemoteMessage>(newBuffer, 1)->rbHeader;

        header.rbhBufHeader.biBufSize   = bufLength;
        header.rbhBufHeader.biLength    = dataLength;
        header.rbhBufHeader.biOffset    = dataOffset;
        header.rbhBufHeader.biBufType   = NEMemory::eBufferType::BufferRemote;
        header.rbhBufHeader.biUsed      = 0;

        if (isValid())
        {
            const NEMemory::sRemoteMessageHeader & hdrSrc = { _getHeader() };
            header.rbhTarget    = hdrSrc.rbhTarget;
            header.rbhSource    = hdrSrc.rbhSource;
            header.rbhMessageId = hdrSrc.rbhMessageId;
            header.rbhResult    = hdrSrc.rbhResult;
            header.rbhSequenceNr= hdrSrc.rbhSequenceNr;
        }

        if ( makeCopy )
        {
            unsigned char * dstBuf{ NEMemory::getBufferDataWrite(reinterpret_cast<NEMemory::sByteBuffer *>(&header.rbhBufHeader)) };
            const unsigned char* srcBuf { NEMemory::getBufferDataRead(mByteBuffer.get()) };
            unsigned int srcCount { getSizeUsed() };
            srcCount = MACRO_MIN(srcCount, dataLength);
            result   = srcCount;

            header.rbhBufHeader.biUsed  = srcCount;
            NEMemory::memCopy( dstBuf, dataLength, srcBuf, srcCount );
        }
    }
    else
    {
        ; // do nothing
    }

    return result;
}

bool RemoteMessage::isChecksumValid(void) const
{
    return isValid() ? getChecksum() == RemoteMessage::_checksumCalculate( _getRemoteMessage() ) : false;
}

void RemoteMessage::bufferCompletionFix(void) const
{
    if ( isValid() )
    {
        const NEMemory::sRemoteMessage & msg = _getRemoteMessage();
        const NEMemory::sRemoteMessageHeader & header = msg.rbHeader;

        unsigned int checksum   = RemoteMessage::_checksumCalculate( msg );
        unsigned int dataUsed   = header.rbhBufHeader.biUsed;
        unsigned int dataLen    = header.rbhBufHeader.biUsed;
        unsigned int bufSize    = header.rbhBufHeader.biOffset + dataUsed;

        dataLen = MACRO_MAX(dataLen, sizeof(NEMemory::BufferData));
        dataLen = MACRO_ALIGN_SIZE(dataLen, sizeof(int));

        bufSize = MACRO_MAX(bufSize, sizeof(NEMemory::sRemoteMessage));
        bufSize = MACRO_ALIGN_SIZE(bufSize, sizeof(int));

        ASSERT(dataLen <= header.rbhBufHeader.biLength);

        const_cast<NEMemory::sRemoteMessageHeader &>(header).rbhBufHeader.biBufSize   = bufSize;
        const_cast<NEMemory::sRemoteMessageHeader &>(header).rbhBufHeader.biLength    = dataLen;
        const_cast<NEMemory::sRemoteMessageHeader &>(header).rbhChecksum              = checksum;
    }
}

unsigned char * RemoteMessage::initMessage(const NEMemory::sRemoteMessageHeader & rmHeader, unsigned int reserve /*= 0*/ )
{
    invalidate();

    reserve = MACRO_MAX(reserve, 1);
    unsigned int sizeUsed   = MACRO_MAX(rmHeader.rbhBufHeader.biUsed, reserve);
    unsigned int hdrSize    = getHeaderSize();
    unsigned int msgSize    = hdrSize + sizeUsed;
    unsigned int sizeBuffer = MACRO_ALIGN_SIZE(msgSize, mBlockSize);
    unsigned int sizeData   = sizeBuffer - hdrSize;
    unsigned char * result  = DEBUG_NEW unsigned char[sizeBuffer];
    if ( result != nullptr )
    {
        NEMemory::memZero(result, sizeof(NEMemory::sRemoteMessage));
        NEMemory::sRemoteMessage * msg      = NEMemory::constructElems<NEMemory::sRemoteMessage>(result, 1);
        NEMemory::sRemoteMessageHeader & dst= msg->rbHeader;
        dst.rbhBufHeader.biBufSize  = sizeBuffer;
        dst.rbhBufHeader.biLength   = sizeData;
        dst.rbhBufHeader.biOffset   = getDataOffset();
        dst.rbhBufHeader.biBufType  = NEMemory::eBufferType::BufferRemote;
        dst.rbhBufHeader.biUsed     = rmHeader.rbhBufHeader.biUsed;
        dst.rbhTarget               = rmHeader.rbhTarget;
        dst.rbhChecksum             = rmHeader.rbhChecksum;
        dst.rbhSource               = rmHeader.rbhSource;
        dst.rbhMessageId            = rmHeader.rbhMessageId;
        dst.rbhResult               = rmHeader.rbhResult;
        dst.rbhSequenceNr           = rmHeader.rbhSequenceNr;
        msg->rbData[0]              = static_cast<NEMemory::BufferData>(0);

        mByteBuffer = std::shared_ptr<NEMemory::sByteBuffer>(reinterpret_cast<NEMemory::sByteBuffer *>(msg), ByteBufferDeleter());
    }

    return getBuffer();
}

RemoteMessage RemoteMessage::clone(const ITEM_ID & source /*= 0*/, const ITEM_ID & target /*= 0*/) const
{
    RemoteMessage result;
    unsigned int reserved{ getSizeUsed() };
    unsigned char * dst{ result.initMessage(getRemoteMessage()->rbHeader, reserved) };

    if (dst != nullptr)
    {
        if (source != NEService::COOKIE_UNKNOWN)
        {
            result.setSource(source);
        }

        if (target != NEService::COOKIE_UNKNOWN)
        {
            result.setTarget(target);
        }

        if (reserved != 0u)
        {
            const unsigned char * src{ getBuffer() };
            NEMemory::memCopy(dst, reserved, src, reserved);
            result.setSizeUsed(reserved);
        }
    }

    return result;
}

unsigned int RemoteMessage::getDataOffset(void) const
{
    return offsetof(NEMemory::sRemoteMessage, rbData);
}

unsigned int RemoteMessage::getHeaderSize(void) const
{
    return sizeof(NEMemory::sRemoteMessage);
}
