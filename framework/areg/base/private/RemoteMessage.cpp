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
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <string.h>
#include <cstddef>

namespace areg {

uint32_t RemoteMessage::_checksum_calculate(const areg::RawMessage& remoteMessage) noexcept
{
#if 0
    const uint32_t offset = offsetof(areg::MessageHeader, rbhSource);
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&remoteMessage.rbHeader.rbhSource);
    const uint32_t remain = remoteMessage.rbHeader.rbhBufHeader.biOffset - offset;
    const uint32_t used = remoteMessage.rbHeader.rbhBufHeader.biUsed;
    const uint32_t length = used + remain;

    return areg::crc32_calculate(data, static_cast<int32_t>(length));
#else
    const uint32_t buffer[ ] = 
    {
         remoteMessage.rbHeader.rbhBufHeader.biOffset
        , static_cast<uint32_t>(remoteMessage.rbHeader.rbhBufHeader.biBufType)
        , remoteMessage.rbHeader.rbhBufHeader.biUsed
        , areg::hi_dword(static_cast<uint64_t>(remoteMessage.rbHeader.rbhSource))
        , areg::lo_dword(static_cast<uint64_t>(remoteMessage.rbHeader.rbhSource))
        , remoteMessage.rbHeader.rbhMessageId
        , remoteMessage.rbHeader.rbhResult
        , static_cast<uint32_t>(remoteMessage.rbHeader.rbhSequenceNr)
    };

    return areg::crc32_calculate(reinterpret_cast<const uint8_t*>(buffer), static_cast<int32_t>(sizeof(buffer)));
#endif
}

RemoteMessage::RemoteMessage(uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
}

RemoteMessage::RemoteMessage(uint32_t reserveSize, uint32_t blockSize)
    : SharedBuffer  ( blockSize )
{
    reserve(reserveSize, false);
}

RemoteMessage::RemoteMessage(const uint8_t * buffer, uint32_t size, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
    reserve(size, false);
    write_data(buffer, size);
}

uint32_t RemoteMessage::init_buffer(uint8_t *newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
{
    uint32_t result{ Cursor::INVALID_CURSOR_POSITION };

    if (newBuffer != nullptr)
    {
        result = 0;
        uint32_t dataOffset { data_offset() };
        uint32_t dataLength { bufLength - dataOffset };

        areg::mem_zero(newBuffer, sizeof(areg::RawMessage));
        areg::MessageHeader & header = areg::construct_elems<areg::RawMessage>(newBuffer, 1)->rbHeader;

        header.rbhBufHeader.biLength    = dataLength;
        header.rbhBufHeader.biOffset    = dataOffset;
        header.rbhBufHeader.biBufType   = areg::BufferType::Remote;
        header.rbhBufHeader.biUsed      = 0;

        if (is_valid())
        {
            const areg::MessageHeader & hdrSrc = { _header() };
            header.rbhTarget    = hdrSrc.rbhTarget;
            header.rbhSource    = hdrSrc.rbhSource;
            header.rbhMessageId = hdrSrc.rbhMessageId;
            header.rbhResult    = hdrSrc.rbhResult;
            header.rbhSequenceNr= hdrSrc.rbhSequenceNr;
        }

        if ( makeCopy )
        {
            uint8_t * dstBuf{ areg::buffer_data_write(reinterpret_cast<areg::RawBuffer *>(&header.rbhBufHeader)) };
            const uint8_t* srcBuf { areg::buffer_data_read(mByteBuffer.get()) };
            uint32_t srcCount { size_used() };
            srcCount = std::min(srcCount, dataLength);
            result   = srcCount;

            header.rbhBufHeader.biUsed  = srcCount;
            areg::mem_copy( dstBuf, dataLength, srcBuf, srcCount );
        }
    }

    return result;
}

void RemoteMessage::buffer_completion_fix() const
{
    if (!is_valid())
        return;

    const areg::RawMessage & msg = _remote_message();
    const areg::MessageHeader & header = msg.rbHeader;

    // Broadcast messages are sent to multiple targets — skip redundant writes when already fixed.
    if (header.rbhChecksum == areg::CHECKSUM_IGNORE)
        return;

#if 0
    const_cast<areg::MessageHeader &>(header).rbhChecksum           = areg::CHECKSUM_IGNORE;
#else
    const_cast<areg::MessageHeader &>(header).rbhChecksum           = RemoteMessage::_checksum_calculate(msg);
#endif
}

uint8_t * RemoteMessage::init_message(const areg::MessageHeader & rmHeader, uint32_t reserve /*= 0*/ )
{
    invalidate();

    reserve = std::max(reserve, 1u);
    uint32_t sizeUsed   = std::max(rmHeader.rbhBufHeader.biUsed, reserve);
    uint32_t hdrSize    = header_size();
    uint32_t msgSize    = hdrSize + sizeUsed;
    uint32_t sizeBuffer = areg::align_size(msgSize, mBlockSize);
    uint32_t sizeData   = sizeBuffer - hdrSize;
    uint8_t * result    = DEBUG_NEW uint8_t[sizeBuffer];
    if ( result != nullptr )
    {
        areg::mem_zero(result, sizeof(areg::RawMessage));
        areg::RawMessage * msg      = areg::construct_elems<areg::RawMessage>(result, 1);
        areg::MessageHeader & dst   = msg->rbHeader;
        dst.rbhBufHeader.biLength   = sizeData;
        dst.rbhBufHeader.biOffset   = data_offset();
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

    return buffer();
}

RemoteMessage RemoteMessage::clone(const ITEM_ID & source /*= 0*/, const ITEM_ID & target /*= 0*/) const
{
    RemoteMessage result;
    uint32_t reserved{ size_used() };
    uint8_t * dst{ result.init_message(remote_message()->rbHeader, reserved) };

    if (dst != nullptr)
    {
        if (source != areg::COOKIE_UNKNOWN)
        {
            result.set_source(source);
        }

        if (target != areg::COOKIE_UNKNOWN)
        {
            result.set_target(target);
        }

        if (reserved != 0u)
        {
            const uint8_t * src{ buffer() };
            areg::mem_copy(dst, reserved, src, reserved);
            result.set_size_used(reserved);
        }
    }

    return result;
}

} // namespace areg
