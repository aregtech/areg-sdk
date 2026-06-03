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
#include "areg/base/EventEnvelope.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include "areg/logging/areg_log.h"

namespace areg {

SocketConnectionBase::SocketConnectionBase() noexcept
{
}

int32_t SocketConnectionBase::receive_message(EventEnvelope & message, const Socket & socket) const
{
    areg::EventHeader evtHeader{};
    int32_t result = socket.receive(reinterpret_cast<uint8_t *>(&evtHeader), sizeof(areg::EventHeader));
    if (result != static_cast<int32_t>(sizeof(areg::EventHeader)))
        return 0;

    if (evtHeader.bufHeader.biUsed > areg::MAX_MESSAGE_DATA_SIZE)
        return 0;

    uint8_t * buffer = message.init_envelope(evtHeader, evtHeader.bufHeader.biUsed);
    if ((evtHeader.bufHeader.biUsed != 0u) && (buffer != nullptr))
    {
        const int32_t rest = socket.receive(buffer, static_cast<int32_t>(evtHeader.bufHeader.biUsed));
        message.set_size_used(evtHeader.bufHeader.biUsed);
        result = rest > 0 ? (result + rest) : 0;
    }

    message.move_to_begin();
    return (message.is_checksum_valid() ? result : 0);
}

} // namespace areg
