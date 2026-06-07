/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/MessageEnvelope.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Event transport buffer. Extends SharedBuffer with EventHeader
 *              at the start of the allocation and a variable payload stream.
 *
 ************************************************************************/
#include "areg/base/MessageEnvelope.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <string.h>
#include <cstddef>

namespace areg {

// Ownership of the heap block is handled by areg::RawBufferPtr (intrusive refcount in
// BufferHeader::biRefCount): one allocation per envelope, freed with delete[] when the last
// owner drops its reference. DATA_CLASS destruction for local custom events is owned exclusively
// by Event::~Event() and destroy_event(), which run on the Event owner while it is the sole owner
// (before the refcount reaches zero). IPC buffers always arrive with internal1/internal2/custom
// zeroed, so there is never a DATA_CLASS cleanup hook on the receive path.

const SequenceNumber MessageEnvelope::_INVALID_SEQUENCE{ areg::SEQUENCE_NUMBER_ANY };
const areg::Endpoint MessageEnvelope::_INVALID_ENDPOINT{ };
const areg::RawService MessageEnvelope::_INVALID_SERVICE{ };

uint32_t MessageEnvelope::_checksum_calculate(const areg::RawEnvelope& env) noexcept
{
    const areg::EventHeader& hdr{ env.envHeader };
    const uint32_t buffer[] =
    {
          hdr.bufHeader.biOffset
        , static_cast<uint32_t>(hdr.bufHeader.biBufType)
        , hdr.bufHeader.biUsed
        , hdr.rawService.role
        , hdr.consumer.thread
        , hdr.rawService.service
        , hdr.provider.thread
        , hdr.channel
        , areg::hi_dword(static_cast<uint64_t>(hdr.sequenceNr))
        , areg::lo_dword(static_cast<uint64_t>(hdr.sequenceNr))
        , hdr.messageId
        , hdr.result
        , areg::make32(hdr.eventType, areg::make16(hdr.callType, hdr.priority))
        , hdr.eventId
    };

    return areg::crc32_calculate(reinterpret_cast<const uint8_t*>(buffer), static_cast<int32_t>(sizeof(buffer)));
}

MessageEnvelope::MessageEnvelope(const areg::EventHeader& evtHeader, uint32_t reserve, uint32_t blockSize)
    : BufferBase(blockSize)
{
    uint32_t sizeUsed{ std::max(evtHeader.bufHeader.biUsed, reserve != 0u ? reserve : 1u) };
    sizeUsed = areg::align_size(sizeUsed, block_size());

    const uint32_t sizeBuffer{ sizeUsed + static_cast<uint32_t>(sizeof(areg::EventHeader)) };
    uint8_t* result{ DEBUG_NEW uint8_t[sizeBuffer] };
    if (result != nullptr)
    {
        areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(result) };
        areg::EventHeader& hdr{ env->envHeader };
        areg::mem_copy(&hdr, sizeof(areg::EventHeader), &evtHeader, sizeof(areg::EventHeader));
        hdr.bufHeader.biLength = sizeUsed;
        hdr.bufHeader.biOffset = sizeof(areg::EventHeader);
        hdr.bufHeader.biUsed   = 0u;
        mByteBuffer = areg::RawBufferPtr(reinterpret_cast<areg::RawBuffer*>(env));   // adopt: sets biRefCount = 1
    }
}

uint32_t MessageEnvelope::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
{
    if (newBuffer == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t dataLength{ bufLength - static_cast<uint32_t>(sizeof(areg::EventHeader)) };

    areg::RawEnvelope* envDst    { areg::construct_elems<areg::RawEnvelope>(newBuffer, 1) };
    areg::EventHeader& hdrDst    { envDst->envHeader };
    hdrDst.bufHeader.biLength    = dataLength;
    hdrDst.bufHeader.biOffset    = sizeof(areg::EventHeader);
    hdrDst.bufHeader.biBufType   = areg::BufferType::Remote;
    hdrDst.bufHeader.biUsed      = 0u;

    if (!makeCopy || (mByteBuffer.get() == nullptr))
        return 0u;

    const areg::RawEnvelope* envSrc{ reinterpret_cast<const areg::RawEnvelope*>(mByteBuffer.get()) };
    const areg::EventHeader& hdrSrc{ envSrc->envHeader };
    const uint32_t srcCount        { std::min(hdrSrc.bufHeader.biUsed, dataLength) };

    // Copy all header fields past bufHeader in one shot: target, source, endpoints, routing,
    // and local-only fields (internal1/2, custom). bufHeader was already initialized above.
    static_assert(sizeof(areg::EventHeader) == 128u, "EventHeader size changed; update init_buffer");
    constexpr uint32_t BUF_HDR_SIZE{ static_cast<uint32_t>(sizeof(areg::BufferHeader)) };
    std::memcpy(reinterpret_cast<uint8_t*>(&hdrDst) + BUF_HDR_SIZE,
                reinterpret_cast<const uint8_t*>(&hdrSrc) + BUF_HDR_SIZE,
                sizeof(areg::EventHeader) - BUF_HDR_SIZE);
    hdrDst.bufHeader.biUsed = srcCount;

    std::memcpy(envDst->envData, envSrc->envData, srcCount);
    return hdrDst.bufHeader.biUsed;
}

void MessageEnvelope::buffer_completion_fix() const
{
    const areg::EventHeader* hdr{ header() };
    if (hdr == nullptr)
        return;

    if (hdr->checksum != areg::CHECKSUM_INVALID)
        return;

    const_cast<areg::EventHeader*>(hdr)->checksum = MessageEnvelope::_checksum_calculate(reinterpret_cast<const areg::RawEnvelope&>(*hdr));
}

uint8_t* MessageEnvelope::init_envelope(const areg::EventHeader& evtHeader, uint32_t reserve /*= 0*/)
{
    uint32_t sizeUsed{ std::max(evtHeader.bufHeader.biUsed, reserve != 0u ? reserve : 1u) };
    sizeUsed = areg::align_size(sizeUsed, block_size());

    areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(mByteBuffer.get()) };
    if ((mByteBuffer.use_count() == 1) && (env != nullptr))
    {
        if (env->envHeader.bufHeader.biLength >= sizeUsed)
        {
            const uint32_t biLength{ env->envHeader.bufHeader.biLength };
            const uint32_t biOffset{ env->envHeader.bufHeader.biOffset };

            areg::mem_copy(&env->envHeader, sizeof(areg::EventHeader), &evtHeader, sizeof(areg::EventHeader));

            env->envHeader.bufHeader.biLength   = biLength;
            env->envHeader.bufHeader.biOffset   = biOffset;
            env->envHeader.bufHeader.biRefCount = 1u;   // restore: the memcpy above clobbered the live refcount (sole owner)
            env->envHeader.bufHeader.biUsed     = std::min(evtHeader.bufHeader.biUsed, biLength);

            mPosition  = 0u;
            return env->envData;
        }
    }

    // Slow path: allocate a new block.
    invalidate();

    const uint32_t sizeBuffer{ sizeUsed + static_cast<uint32_t>(sizeof(areg::EventHeader)) };
    uint8_t* result{ DEBUG_NEW uint8_t[sizeBuffer] };
    if (result == nullptr)
        return nullptr;

    env = reinterpret_cast<areg::RawEnvelope*>(result);
    areg::EventHeader& hdr{ env->envHeader };
    areg::mem_copy(&hdr, sizeof(areg::EventHeader), &evtHeader, sizeof(areg::EventHeader));
    hdr.bufHeader.biLength = sizeUsed;
    hdr.bufHeader.biOffset = sizeof(areg::EventHeader);
    hdr.bufHeader.biUsed   = 0u;
    mByteBuffer = areg::RawBufferPtr(reinterpret_cast<areg::RawBuffer*>(env));   // adopt: sets biRefCount = 1

    return env->envData;
}

MessageEnvelope MessageEnvelope::clone(const areg::Endpoint* consumer /*= nullptr*/,
                                   const areg::Endpoint* provider /*= nullptr*/) const
{
    MessageEnvelope result;
    const areg::RawEnvelope* env{ raw_envelope() };
    if (env == nullptr)
        return result;

    const uint32_t reserved{ size_used() };
    uint8_t* dst{ result.init_envelope(env->envHeader, reserved) };
    if (dst == nullptr)
        return result;

    if (consumer != nullptr)
    {
        result.set_consumer(*consumer);
    }

    if (provider != nullptr)
    {
        result.set_provider(*provider);
    }

    if (reserved != 0u)
    {
        areg::mem_copy(dst, reserved, buffer(), reserved);
        result.set_size_used(reserved);
    }

    return result;
}

void MessageEnvelope::destroy_event() noexcept
{
    if (is_unique())
    {
        areg::RawEnvelope* env{ reinterpret_cast<areg::RawEnvelope*>(mByteBuffer.get()) };
        if ((env != nullptr) && (env->envHeader.custom != 0u))
        {
            using CleanupFn = void(*)(void*) noexcept;
            CleanupFn fn{ nullptr };
            std::memcpy(&fn, &env->envHeader.custom, sizeof(fn));
            env->envHeader.custom = 0u;   // clear before call — prevent double-call
            fn(env->envData);
        }
    }

    invalidate();
}

} // namespace areg
