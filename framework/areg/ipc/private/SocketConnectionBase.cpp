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

#include "areg/logging/areg_log.h"
namespace areg {

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

int32_t SocketConnectionBase::receive_message(RemoteMessage & message, const Socket & socket) const
{
    areg::MessageHeader msgHeader{};
    int32_t result = socket.receive(reinterpret_cast<uint8_t *>(&msgHeader), sizeof(areg::MessageHeader));
    if ( result == sizeof(areg::MessageHeader) )
    {
        uint8_t * buffer = message.init_message( msgHeader );
        if ((msgHeader.rbhBufHeader.biUsed > 0) && (buffer != nullptr))
        {
            ASSERT(msgHeader.rbhBufHeader.biLength >= msgHeader.rbhBufHeader.biUsed);

            // receive aligned length of data.
            const int32_t rest = socket.receive(buffer, static_cast<int32_t>(msgHeader.rbhBufHeader.biUsed));
            result += rest > 0 ? rest : 0;
        }

        message.move_to_begin();
        if ( !message.is_checksum_valid() )
        {
            result = 0;
            message.invalidate();
        }
    }
    else if (result > 0)
    {
        result = 0;
    }
    

    return result;
}

} // namespace areg
