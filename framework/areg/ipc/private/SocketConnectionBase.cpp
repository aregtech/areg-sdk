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
{
}

int32_t SocketConnectionBase::send_message(const RemoteMessage & message, SOCKETHANDLE hSocket) const
{
    int32_t result{ -1 };
    const areg::MessageHeader* header = message.header();
    if ((header != nullptr) && areg::is_valid_socket(hSocket))
    {
        message.buffer_completion_fix();
        ASSERT(header->rbhBufHeader.biLength >= header->rbhBufHeader.biUsed);
        const int32_t totalLen = static_cast<int32_t>(sizeof(areg::MessageHeader) + header->rbhBufHeader.biUsed);
        result = areg::send_data(hSocket, reinterpret_cast<const uint8_t *>(header), totalLen);
    }

    return result;
}

int32_t SocketConnectionBase::send_messages_batch(const RemoteMessage* const* messages, uint32_t count, SOCKETHANDLE hSocket) const
{
    if ((messages == nullptr) || (count == 0u) || !areg::is_valid_socket(hSocket))
        return 0;

    // Build IoBuffer array on the stack; capped at the same bound as THREAD_BATCH_LIMIT.
    const uint32_t batchCount{ (count < areg::THREAD_BATCH_LIMIT) ? count : areg::THREAD_BATCH_LIMIT };
    areg::IoBuffer ioBuffer[areg::THREAD_BATCH_LIMIT];
    uint32_t bufCount { 0u };
    uint32_t totalSize{ 0u };

    for (uint32_t i = 0u; i < batchCount; ++i)
    {
        const RemoteMessage* msg{ messages[i] };
        const areg::MessageHeader* hdr = msg != nullptr ? msg->header() : nullptr;
        if (hdr == nullptr)
            continue;

        msg->buffer_completion_fix();
        ASSERT(hdr->rbhBufHeader.biLength >= hdr->rbhBufHeader.biUsed);
        uint32_t bufSize = static_cast<uint32_t>(sizeof(areg::MessageHeader)) + hdr->rbhBufHeader.biUsed;
        ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t*>(hdr), bufSize };
        totalSize += bufSize;
    }

    int32_t result = send_messages_batch(ioBuffer, bufCount, hSocket, totalSize);
    if (count > areg::THREAD_BATCH_LIMIT)
    {
        result += send_messages_batch(messages + batchCount, count - batchCount, hSocket);
    }

    return result;
}

int32_t SocketConnectionBase::receive_message(RemoteMessage & message, const Socket & socket) const
{
    areg::MessageHeader msgHeader{};
    int32_t result = socket.receive(reinterpret_cast<uint8_t *>(&msgHeader), sizeof(areg::MessageHeader));
    if (result != sizeof(areg::MessageHeader))
        return 0;

    if (msgHeader.rbhBufHeader.biUsed > areg::MAX_MESSAGE_DATA_SIZE)
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

} // namespace areg
