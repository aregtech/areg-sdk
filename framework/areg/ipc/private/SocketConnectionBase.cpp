/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/SocketConnectionBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Connection base class declaration.
 ************************************************************************/

#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"

namespace areg {

SocketConnectionBase::SocketConnectionBase() noexcept
    : mZerocopyWanted   ( false )
    , mZerocopyEnabled  ( false )
{
}

int32_t SocketConnectionBase::send_message(const RemoteMessage & message, const Socket & socket) const
{
    int32_t result{ -1 };
    if ( message.is_valid() )
    {
        message.buffer_completion_fix();
        const areg::MessageHeader & header = reinterpret_cast<const areg::MessageHeader &>( *message.byte_buffer() );
        if (header.rbhBufHeader.biUsed != 0)
        {
            ASSERT(header.rbhBufHeader.biLength >= header.rbhBufHeader.biUsed);
            // Header and payload are contiguous in the RawMessage allocation.
            // Send both in one syscall to eliminate the extra round-trip latency.
            const int32_t totalLen = static_cast<int32_t>(sizeof(areg::MessageHeader) + header.rbhBufHeader.biUsed);
            result = socket.send(reinterpret_cast<const uint8_t *>(&header), totalLen);
        }
        else
        {
            result = socket.send(reinterpret_cast<const uint8_t *>(&header), sizeof(areg::MessageHeader));
        }
    }

    return result;
}

int32_t SocketConnectionBase::send_messages_batch(const RemoteMessage* const* messages, uint32_t count, const Socket& socket) const
{
    if ((messages == nullptr) || (count == 0u) || !socket.is_valid())
        return 0;

    // Build IoBuffer array on the stack; capped at the same bound as THREAD_BATCH_LIMIT.
    constexpr uint32_t MAX_BATCH{ areg::THREAD_BATCH_LIMIT };
    const uint32_t batchCount{ (count < MAX_BATCH) ? count : MAX_BATCH };

    areg::IoBuffer iovs[MAX_BATCH];
    uint32_t validCount{ 0u };

    for (uint32_t i{ 0u }; i < batchCount; ++i)
    {
        const RemoteMessage* msg{ messages[i] };
        if ((msg == nullptr) || !msg->is_valid())
            continue;

        msg->buffer_completion_fix();
        const areg::MessageHeader& hdr = reinterpret_cast<const areg::MessageHeader&>( *msg->byte_buffer() );
        if (hdr.rbhBufHeader.biUsed == 0u)
        {
            // Header-only message.
            iovs[validCount++] = { reinterpret_cast<const uint8_t*>(&hdr)
                                 , static_cast<uint32_t>(sizeof(areg::MessageHeader)) };
        }
        else
        {
            ASSERT(hdr.rbhBufHeader.biLength >= hdr.rbhBufHeader.biUsed);
            iovs[validCount++] = { reinterpret_cast<const uint8_t*>(&hdr)
                                 , static_cast<uint32_t>(sizeof(areg::MessageHeader)) + hdr.rbhBufHeader.biUsed };
        }
    }

    return (validCount != 0u ? areg::send_data_v(socket.handle(), iovs, validCount) : 0u);
}

int32_t SocketConnectionBase::send_message(const RemoteMessage & message, SOCKETHANDLE hSocket) const
{
    int32_t result{ -1 };
    if ( message.is_valid() && (hSocket != areg::InvalidSocketHandle) )
    {
        message.buffer_completion_fix();
        const areg::MessageHeader * header = reinterpret_cast<const areg::MessageHeader *>( message.byte_buffer() );
        if (header->rbhBufHeader.biUsed != 0)
        {
            ASSERT(header->rbhBufHeader.biLength >= header->rbhBufHeader.biUsed);
            const int32_t totalLen = static_cast<int32_t>(sizeof(areg::MessageHeader) + header->rbhBufHeader.biUsed);
            result = areg::send_data(hSocket, reinterpret_cast<const uint8_t *>(header), totalLen);
        }
        else
        {
            result = areg::send_data(hSocket, reinterpret_cast<const uint8_t *>(header), sizeof(areg::MessageHeader));
        }
    }

    return result;
}

int32_t SocketConnectionBase::send_messages_batch(const RemoteMessage* const* messages, uint32_t count, SOCKETHANDLE hSocket) const
{
    if ((messages == nullptr) || (count == 0u) || (hSocket == areg::InvalidSocketHandle))
        return 0;

    constexpr uint32_t MAX_BATCH{ areg::THREAD_BATCH_LIMIT };
    const uint32_t batchCount{ (count < MAX_BATCH) ? count : MAX_BATCH };

    areg::IoBuffer iovs[MAX_BATCH];
    uint32_t validCount{ 0u };

    for (uint32_t i{ 0u }; i < batchCount; ++i)
    {
        const RemoteMessage* msg{ messages[i] };
        if ((msg == nullptr) || !msg->is_valid())
            continue;

        msg->buffer_completion_fix();
        const areg::MessageHeader& hdr = reinterpret_cast<const areg::MessageHeader&>( *msg->byte_buffer() );
        if (hdr.rbhBufHeader.biUsed == 0u)
        {
            iovs[validCount++] = { reinterpret_cast<const uint8_t*>(&hdr)
                                 , static_cast<uint32_t>(sizeof(areg::MessageHeader)) };
        }
        else
        {
            ASSERT(hdr.rbhBufHeader.biLength >= hdr.rbhBufHeader.biUsed);
            iovs[validCount++] = { reinterpret_cast<const uint8_t*>(&hdr)
                                 , static_cast<uint32_t>(sizeof(areg::MessageHeader)) + hdr.rbhBufHeader.biUsed };
        }
    }

    return (validCount != 0u ? areg::send_data_v(hSocket, iovs, validCount) : 0u);
}

int32_t SocketConnectionBase::receive_message(RemoteMessage & message, const Socket & socket) const
{
    areg::MessageHeader msgHeader{};
    int32_t result = socket.receive(reinterpret_cast<uint8_t *>(&msgHeader), sizeof(areg::MessageHeader));
    if (result != sizeof(areg::MessageHeader))
        return 0;

    uint8_t * buffer = message.init_message( msgHeader);
    if ((msgHeader.rbhBufHeader.biUsed != 0) && (buffer != nullptr))
    {
        ASSERT(msgHeader.rbhBufHeader.biLength >= msgHeader.rbhBufHeader.biUsed);

        // receive aligned length of data.
        const int32_t rest = socket.receive(buffer, static_cast<int32_t>(msgHeader.rbhBufHeader.biUsed));
        message.set_size_used(msgHeader.rbhBufHeader.biUsed);
        result = rest > 0 ? (result + rest) : 0;
    }

    message.move_to_begin();
    return (message.is_checksum_valid() ? result : 0);
}

#if defined(__linux__)

int32_t SocketConnectionBase::send_message_zerocopy(const RemoteMessage & message, SOCKETHANDLE hSocket) const
{
    if (!message.is_valid() || (hSocket == areg::InvalidSocketHandle))
        return 0;

    message.buffer_completion_fix();
    const areg::MessageHeader & header = reinterpret_cast<const areg::MessageHeader &>(*message.byte_buffer());
    const int32_t total_len = header.rbhBufHeader.biUsed != 0u
        ? static_cast<int32_t>(sizeof(areg::MessageHeader) + header.rbhBufHeader.biUsed)
        : static_cast<int32_t>(sizeof(areg::MessageHeader));
    return areg::socket_send_zerocopy(hSocket, reinterpret_cast<const uint8_t *>(&header), total_len);
}

#endif  // defined(__linux__)

} // namespace areg
