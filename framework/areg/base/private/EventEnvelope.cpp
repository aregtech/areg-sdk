/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/EventEnvelope.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Event transport buffer. Extends SharedBuffer with EventHeader
 *              at the start of the allocation and a variable payload stream.
 *
 ************************************************************************/
#include "areg/base/EventEnvelope.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <string.h>
#include <cstddef>

namespace areg {

// EventEnvelopeDeleter is used for buffers allocated by init_envelope() and the
// EventHeader-based constructor (IPC receive path only).
// IPC buffers always arrive with internal1/internal2/custom zeroed, so there is
// never a DATA_CLASS cleanup hook to call here. DATA_CLASS destruction for local
// custom events is owned exclusively by Event::~Event() and destroy_event(), which
// run on the Event owner before the shared_ptr refcount ever reaches zero.
// This deleter just frees the raw bytes.
namespace
{

struct EventEnvelopeDeleter
{
    void operator()(areg::RawBuffer* p) const noexcept
    {
        delete[] reinterpret_cast<uint8_t*>(p);
    }
};

} // anonymous namespace

const SequenceNumber EventEnvelope::_INVALID_SEQUENCE{ areg::SEQUENCE_NUMBER_ANY };
const areg::Endpoint EventEnvelope::_INVALID_ENDPOINT{ };
const areg::RawService EventEnvelope::_INVALID_SERVICE{ };

uint32_t EventEnvelope::_checksum_calculate(const areg::RawEnvelope& env) noexcept
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

EventEnvelope::EventEnvelope(uint16_t eventType, uint8_t prio, uint32_t reserveSize /*= 0*/, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer(blockSize)
{
    if (eventType != 0)
    {
        reserve(static_cast<uint32_t>(sizeof(areg::EventHeader)) + reserveSize, false);
        set_event_type(eventType);
        set_priority(prio);
    }
}

EventEnvelope::EventEnvelope(uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer(blockSize)
{
}

EventEnvelope::EventEnvelope(bool init, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer(blockSize)
{
    if (init)
    {
        reserve(blockSize == 0u ? 1u : blockSize, false);
    }
}

EventEnvelope::EventEnvelope(uint32_t reserveSize, uint32_t blockSize)
    : SharedBuffer(blockSize)
{
    reserve(reserveSize, false);
}

EventEnvelope::EventEnvelope(const uint8_t* buffer, uint32_t size, uint32_t blockSize /*= areg::BLOCK_SIZE*/)
    : SharedBuffer(blockSize)
{
    reserve(size, false);
    write_data(buffer, size);
}

EventEnvelope::EventEnvelope(const areg::EventHeader& evtHeader, uint32_t reserve, uint32_t blockSize)
    : SharedBuffer(blockSize)
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
        mByteBuffer = std::shared_ptr<areg::RawBuffer>(reinterpret_cast<areg::RawBuffer*>(env), EventEnvelopeDeleter());
    }
}

uint32_t EventEnvelope::init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept
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

void EventEnvelope::buffer_completion_fix() const
{
    const areg::EventHeader* hdr{ header() };
    if (hdr == nullptr)
        return;

    if (hdr->checksum != areg::CHECKSUM_INVALID)
        return;

    const_cast<areg::EventHeader*>(hdr)->checksum = EventEnvelope::_checksum_calculate(reinterpret_cast<const areg::RawEnvelope&>(*hdr));
}

uint8_t* EventEnvelope::init_envelope(const areg::EventHeader& evtHeader, uint32_t reserve /*= 0*/)
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

            env->envHeader.bufHeader.biLength = biLength;
            env->envHeader.bufHeader.biOffset = biOffset;
            env->envHeader.bufHeader.biUsed   = std::min(evtHeader.bufHeader.biUsed, biLength);

            mPosition  = 0u;
            mViewStart = 0u;
            mViewEnd   = 0u;
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
    mByteBuffer = std::shared_ptr<areg::RawBuffer>(
        reinterpret_cast<areg::RawBuffer*>(env), EventEnvelopeDeleter());

    return env->envData;
}

EventEnvelope EventEnvelope::clone(const areg::Endpoint* consumer /*= nullptr*/,
                                   const areg::Endpoint* provider /*= nullptr*/) const
{
    EventEnvelope result;
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

void EventEnvelope::destroy_event() noexcept
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
