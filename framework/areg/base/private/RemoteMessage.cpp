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

const ITEM_ID        RemoteMessage::_INVALID_COOKIE     { static_cast<ITEM_ID>(areg::Cookie::Invalid) };
const SequenceNumber RemoteMessage::_INVALID_SEQUENCE   { areg::SEQUENCE_NUMBER_ANY };

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
        , areg::hi_dword(static_cast<uint64_t>(remoteMessage.rbHeader.rbhSequenceNr))
        , areg::lo_dword(static_cast<uint64_t>(remoteMessage.rbHeader.rbhSequenceNr))
    };

    return areg::crc32_calculate(reinterpret_cast<const uint8_t*>(buffer), static_cast<int32_t>(sizeof(buffer)));
#endif
}

RemoteMessage::RemoteMessage(uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer  ( blockSize )
{
}

RemoteMessage::RemoteMessage(bool init, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer(blockSize)
{
    if (init)
    {
        reserve(blockSize == 0 ? 1u : blockSize, false);
    }
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

RemoteMessage::RemoteMessage(const areg::MessageHeader& rmHeader, uint32_t reserve, uint32_t blockSize)
    : SharedBuffer  ( blockSize) 
{
    uint32_t sizeUsed{ std::max(rmHeader.rbhBufHeader.biUsed, reserve != 0 ? reserve : 1u) };
    sizeUsed = areg::align_size(sizeUsed, block_size());

    uint32_t sizeBuffer = sizeUsed + sizeof(areg::MessageHeader);
    uint8_t* result = DEBUG_NEW uint8_t[sizeBuffer];
    if (result != nullptr)
    {
        areg::RawMessage* msg = reinterpret_cast<areg::RawMessage *>(result);
        areg::MessageHeader& hdr{ msg->rbHeader };
        areg::mem_copy(&hdr, sizeof(areg::MessageHeader), &rmHeader, sizeof(areg::MessageHeader));
        hdr.rbhBufHeader.biLength = sizeUsed;
        hdr.rbhBufHeader.biOffset = sizeof(areg::MessageHeader);
        hdr.rbhBufHeader.biUsed = 0u;
        mByteBuffer = std::shared_ptr<areg::RawBuffer>(reinterpret_cast<areg::RawBuffer*>(msg), ByteBufferDeleter());
    }
}

uint32_t RemoteMessage::init_buffer(uint8_t *newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
{
    if (newBuffer == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t dataLength { bufLength - static_cast<uint32_t>(sizeof(areg::MessageHeader))};

    areg::RawMessage* msgDst        = areg::construct_elems<areg::RawMessage>(newBuffer, 1); // new(newBuffer) areg::RawMessage;
    areg::MessageHeader & hdrDst    = msgDst->rbHeader;
    hdrDst.rbhBufHeader.biLength    = dataLength;
    hdrDst.rbhBufHeader.biOffset    = sizeof(areg::MessageHeader);
    hdrDst.rbhBufHeader.biBufType   = areg::BufferType::Remote;
    hdrDst.rbhBufHeader.biUsed      = 0;

    if (!makeCopy || (mByteBuffer.get() == nullptr))
        return 0u;

    const areg::RawMessage* msgSrc     = reinterpret_cast<const areg::RawMessage *>(mByteBuffer.get());
    const areg::MessageHeader & hdrSrc = { msgSrc->rbHeader };
    uint8_t* dstBuf       = msgDst->rbData;
    const uint8_t* srcBuf = msgSrc->rbData;
    uint32_t srcCount     = std::min(hdrSrc.rbhBufHeader.biUsed, dataLength);

    hdrDst.rbhTarget    = hdrSrc.rbhTarget;
    hdrDst.rbhSource    = hdrSrc.rbhSource;
    hdrDst.rbhMessageId = hdrSrc.rbhMessageId;
    hdrDst.rbhResult    = hdrSrc.rbhResult;
    hdrDst.rbhSequenceNr= hdrSrc.rbhSequenceNr;
    hdrDst.rbhBufHeader.biUsed = srcCount;
    areg::mem_copy(dstBuf, dataLength, srcBuf, srcCount);

    return hdrDst.rbhBufHeader.biUsed;
}

void RemoteMessage::buffer_completion_fix() const
{
    const areg::MessageHeader* hdr = header();
    if (hdr == nullptr)
        return;

    // skip redundant writes when already fixed.
    if (hdr->rbhChecksum != areg::CHECKSUM_INVALID)
        return;

    const_cast<areg::MessageHeader *>(hdr)->rbhChecksum = RemoteMessage::_checksum_calculate(reinterpret_cast<const areg::RawMessage &>(*hdr));
}

uint8_t * RemoteMessage::init_message(const areg::MessageHeader & rmHeader, uint32_t reserve /*= 0*/ )
{
    uint32_t sizeUsed{ std::max(rmHeader.rbhBufHeader.biUsed, reserve != 0 ? reserve : 1u) };
    sizeUsed = areg::align_size(sizeUsed, block_size());

    areg::RawMessage* msg = reinterpret_cast<areg::RawMessage*>(mByteBuffer.get());
    if ((mByteBuffer.use_count() == 1) && (msg != nullptr))
    {
        // Fast path: reuse existing buffer if we are the sole owner and capacity is sufficient.
        if (msg->rbHeader.rbhBufHeader.biLength >= sizeUsed)
        {
            // Save our actual allocation metadata before overwriting the header — the sender's
            // biLength and biOffset reflect their allocation, not ours.  Restoring them prevents
            // write_data from believing it has more space than was actually allocated here.
            const uint32_t biLength = msg->rbHeader.rbhBufHeader.biLength;
            const uint32_t biOffset = msg->rbHeader.rbhBufHeader.biOffset;

            areg::mem_copy(&msg->rbHeader, sizeof(areg::MessageHeader), &rmHeader, sizeof(areg::MessageHeader));

            msg->rbHeader.rbhBufHeader.biLength = biLength;
            msg->rbHeader.rbhBufHeader.biOffset = biOffset;
            msg->rbHeader.rbhBufHeader.biUsed   = std::min(rmHeader.rbhBufHeader.biUsed, biLength);

            mPosition  = 0u;
            mViewStart = 0u;
            mViewEnd   = 0u;
            return msg->rbData;
        }
    }

    // Slow path: allocate a new buffer.
    invalidate();

    uint32_t sizeBuffer = sizeUsed + sizeof(areg::MessageHeader);
    uint8_t * result    = DEBUG_NEW uint8_t[sizeBuffer];
    if (result == nullptr)
        return nullptr;

    msg = reinterpret_cast<areg::RawMessage*>(result);
    areg::MessageHeader& hdr{ msg->rbHeader };
    areg::mem_copy(&hdr, sizeof(areg::MessageHeader), &rmHeader, sizeof(areg::MessageHeader));
    hdr.rbhBufHeader.biLength   = sizeUsed;
    hdr.rbhBufHeader.biOffset   = sizeof(areg::MessageHeader);
    hdr.rbhBufHeader.biUsed     = 0u;
    mByteBuffer = std::shared_ptr<areg::RawBuffer>(reinterpret_cast<areg::RawBuffer *>(msg), ByteBufferDeleter());

    return msg->rbData;
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
