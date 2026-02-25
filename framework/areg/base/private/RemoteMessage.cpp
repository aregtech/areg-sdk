/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/RemoteMessage.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Remote Shared Buffer class. This Buffer is used for 
 *              Read and Write of data during remote communication
 *              between different processes running either on same or 
 *              different hosts.
 *
 ************************************************************************/
#include "areg/base/RemoteMessage.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/logging/GELog.h"

#include <algorithm>
#include <string.h>
#include <cstddef>

namespace areg
{
    inline uint32_t RemoteMessage::_checksumCalculate( const areg::RawMessage & remoteMessage )
    {
        const uint32_t offset   = offsetof( areg::MessageHeader, rbhSource );
        const uint8_t * data  = reinterpret_cast<const uint8_t *>(&remoteMessage.rbHeader.rbhSource);
        const uint32_t remain   = remoteMessage.rbHeader.rbhBufHeader.biOffset - offset;
        const uint32_t used     = remoteMessage.rbHeader.rbhBufHeader.biUsed;
        const uint32_t length   = used + remain;

        return areg::crc32Calculate( data, static_cast<int32_t>(length) );
    }

    RemoteMessage::RemoteMessage(uint32_t blockSize /*= areg::BLOCK_SIZE*/)
        : areg::SharedBuffer  ( blockSize )
    {
    }

    RemoteMessage::RemoteMessage(uint32_t reserveSize, uint32_t blockSize)
        : areg::SharedBuffer  ( blockSize )
    {
        reserve(reserveSize, false);
    }

    RemoteMessage::RemoteMessage(const uint8_t * buffer, uint32_t size, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
        : areg::SharedBuffer  ( blockSize )
    {
        reserve(size, false);
        writeData(buffer, size);
    }

    uint32_t RemoteMessage::initBuffer(uint8_t *newBuffer, uint32_t bufLength, bool makeCopy) const
    {
        uint32_t result{ Cursor::INVALID_CURSOR_POSITION };

        if (newBuffer != nullptr)
        {
            result = 0;
            uint32_t dataOffset { getDataOffset() };
            uint32_t dataLength { bufLength - dataOffset };

            areg::memZero(newBuffer, sizeof(areg::RawMessage));
            areg::MessageHeader & header = areg::constructElems<areg::RawMessage>(newBuffer, 1)->rbHeader;

            header.rbhBufHeader.biBufSize   = bufLength;
            header.rbhBufHeader.biLength    = dataLength;
            header.rbhBufHeader.biOffset    = dataOffset;
            header.rbhBufHeader.biBufType   = areg::BufferType::Remote;
            header.rbhBufHeader.biUsed      = 0;

            if (isValid())
            {
                const areg::MessageHeader & hdrSrc = { _getHeader() };
                header.rbhTarget    = hdrSrc.rbhTarget;
                header.rbhSource    = hdrSrc.rbhSource;
                header.rbhMessageId = hdrSrc.rbhMessageId;
                header.rbhResult    = hdrSrc.rbhResult;
                header.rbhSequenceNr= hdrSrc.rbhSequenceNr;
            }

            if ( makeCopy )
            {
                uint8_t * dstBuf{ areg::getBufferDataWrite(reinterpret_cast<areg::RawBuffer *>(&header.rbhBufHeader)) };
                const uint8_t* srcBuf { areg::getBufferDataRead(mByteBuffer.get()) };
                uint32_t srcCount { getSizeUsed() };
                srcCount = std::min(srcCount, dataLength);
                result   = srcCount;

                header.rbhBufHeader.biUsed  = srcCount;
                areg::memCopy( dstBuf, dataLength, srcBuf, srcCount );
            }
        }

        return result;
    }

    bool RemoteMessage::isChecksumValid() const
    {
        return isValid() ? getChecksum() == RemoteMessage::_checksumCalculate( _getRemoteMessage() ) : false;
    }

    void RemoteMessage::bufferCompletionFix() const
    {
        if ( isValid() )
        {
            const areg::RawMessage & msg = _getRemoteMessage();
            const areg::MessageHeader & header = msg.rbHeader;

            uint32_t checksum   = RemoteMessage::_checksumCalculate( msg );
            uint32_t dataUsed   = header.rbhBufHeader.biUsed;
            uint32_t dataLen    = header.rbhBufHeader.biUsed;
            uint32_t bufSize    = header.rbhBufHeader.biOffset + dataUsed;

            dataLen = std::max(dataLen, static_cast<uint32_t>(sizeof(areg::BufferData)));
            dataLen = areg::alignSize(dataLen, static_cast<uint32_t>(sizeof(int32_t)));

            bufSize = std::max(bufSize, static_cast<uint32_t>(sizeof(areg::RawMessage)));
            bufSize = areg::alignSize(bufSize, static_cast<uint32_t>(sizeof(int32_t)));

            ASSERT(dataLen <= header.rbhBufHeader.biLength);

            const_cast<areg::MessageHeader &>(header).rbhBufHeader.biBufSize   = bufSize;
            const_cast<areg::MessageHeader &>(header).rbhBufHeader.biLength    = dataLen;
            const_cast<areg::MessageHeader &>(header).rbhChecksum              = checksum;
        }
    }

    uint8_t * RemoteMessage::initMessage(const areg::MessageHeader & rmHeader, uint32_t reserve /*= 0*/ )
    {
        invalidate();

        reserve = std::max(reserve, 1u);
        uint32_t sizeUsed   = std::max(rmHeader.rbhBufHeader.biUsed, reserve);
        uint32_t hdrSize    = getHeaderSize();
        uint32_t msgSize    = hdrSize + sizeUsed;
        uint32_t sizeBuffer = areg::alignSize(msgSize, mBlockSize);
        uint32_t sizeData   = sizeBuffer - hdrSize;
        uint8_t * result  = DEBUG_NEW uint8_t[sizeBuffer];
        if ( result != nullptr )
        {
            areg::memZero(result, sizeof(areg::RawMessage));
            areg::RawMessage * msg      = areg::constructElems<areg::RawMessage>(result, 1);
            areg::MessageHeader & dst= msg->rbHeader;
            dst.rbhBufHeader.biBufSize  = sizeBuffer;
            dst.rbhBufHeader.biLength   = sizeData;
            dst.rbhBufHeader.biOffset   = getDataOffset();
            dst.rbhBufHeader.biBufType  = areg::BufferType::Remote;
            dst.rbhBufHeader.biUsed     = rmHeader.rbhBufHeader.biUsed;
            dst.rbhTarget               = rmHeader.rbhTarget;
            dst.rbhChecksum             = rmHeader.rbhChecksum;
            dst.rbhSource               = rmHeader.rbhSource;
            dst.rbhMessageId            = rmHeader.rbhMessageId;
            dst.rbhResult               = rmHeader.rbhResult;
            dst.rbhSequenceNr           = rmHeader.rbhSequenceNr;
            msg->rbData[0]              = static_cast<areg::BufferData>(0);

            mByteBuffer = std::shared_ptr<areg::RawBuffer>(reinterpret_cast<areg::RawBuffer *>(msg), ByteBufferDeleter());
        }

        return getBuffer();
    }

    RemoteMessage RemoteMessage::clone(const ITEM_ID & source /*= 0*/, const ITEM_ID & target /*= 0*/) const
    {
        RemoteMessage result;
        uint32_t reserved{ getSizeUsed() };
        uint8_t * dst{ result.initMessage(getRemoteMessage()->rbHeader, reserved) };

        if (dst != nullptr)
        {
            if (source != areg::COOKIE_UNKNOWN)
            {
                result.setSource(source);
            }

            if (target != areg::COOKIE_UNKNOWN)
            {
                result.setTarget(target);
            }

            if (reserved != 0u)
            {
                const uint8_t * src{ getBuffer() };
                areg::memCopy(dst, reserved, src, reserved);
                result.setSizeUsed(reserved);
            }
        }

        return result;
    }

    uint32_t RemoteMessage::getDataOffset() const
    {
        return offsetof(areg::RawMessage, rbData);
    }

    uint32_t RemoteMessage::getHeaderSize() const
    {
        return sizeof(areg::RawMessage);
    }

} // namespace areg