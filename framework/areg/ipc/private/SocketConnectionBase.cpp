/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/SocketConnectionBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Connection base class declaration.
 ************************************************************************/

#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"

#include <cstring>

namespace areg {

SocketConnectionBase::SocketConnectionBase() noexcept
{
}

uint8_t * SocketConnectionBase::prepare_message(MessageEnvelope & message, areg::EventHeader & header)
{
    if ((header.bufHeader.biUsed > areg::MAX_BUF_LENGTH) || (areg::is_valid(static_cast<areg::EventType>(header.eventType)) == false))
        return nullptr;

    // The local-only fields never take the values of the peer.
    header.bufHeader.biBufType = areg::BufferType::Remote;
    header.internal1 = 0u;
    header.internal2 = 0u;
    header.custom    = 0u;

    return message.init_envelope(header, header.bufHeader.biUsed);
}

int32_t SocketConnectionBase::receive_cached_message(MessageEnvelope & message, areg::ThreadCache & rxCache)
{
    if (rxCache.unread < areg::WIRE_HEADER_SIZE)
        return 0;

    const uint8_t * const data{ rxCache.buffer.get() + rxCache.head };
    areg::EventHeader header{ };
    std::memcpy(areg::wire_header(header), data, areg::WIRE_HEADER_SIZE);
    const uint32_t used{ header.bufHeader.biUsed };
    const uint32_t total{ areg::wire_size(header) };
    if (rxCache.unread < total)
        return 0;

    uint8_t * const body{ SocketConnectionBase::prepare_message(message, header) };
    if (body == nullptr)
        return 0;

    if (used != 0u)
    {
        std::memcpy(body, data + areg::WIRE_HEADER_SIZE, used);
        message.set_size_used(used);
    }

    message.move_to_begin();
    rxCache.unread -= total;
    rxCache.head    = (rxCache.unread != 0u) ? (rxCache.head + total) : 0u;
    return static_cast<int32_t>(total);
}

int32_t SocketConnectionBase::receive_message(MessageEnvelope & message, const Socket & socket) const
{
    areg::EventHeader evtHeader{};
    if (socket.receive(areg::wire_header(evtHeader), areg::WIRE_HEADER_SIZE) != static_cast<int32_t>(areg::WIRE_HEADER_SIZE))
        return 0;

    uint8_t * buffer = SocketConnectionBase::prepare_message(message, evtHeader);
    int32_t result{ buffer != nullptr ? static_cast<int32_t>(areg::WIRE_HEADER_SIZE) : 0 };
    if ((evtHeader.bufHeader.biUsed != 0u) && (result != 0))
    {
        const int32_t rest = socket.receive(buffer, static_cast<int32_t>(evtHeader.bufHeader.biUsed));
        message.set_size_used(evtHeader.bufHeader.biUsed);
        result = rest > 0 ? (static_cast<int32_t>(areg::WIRE_HEADER_SIZE) + rest) : 0;
    }

    message.move_to_begin();
    return result;
}

} // namespace areg
