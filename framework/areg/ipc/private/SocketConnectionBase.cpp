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

int32_t SocketConnectionBase::send_message(const RemoteMessage & in_message, const Socket & clientSocket) const
{
    int32_t result{ -1 };
    if ( in_message.is_valid() && clientSocket.is_valid() )
    {
        in_message.buffer_completion_fix();
        const areg::MessageHeader & buffer = reinterpret_cast<const areg::MessageHeader &>( *in_message.byte_buffer() );
        result = clientSocket.send_data( reinterpret_cast<const uint8_t *>(&buffer), sizeof(areg::MessageHeader) );
        if ((result == sizeof(areg::MessageHeader)) && (buffer.rbhBufHeader.biUsed != 0))
        {
            ASSERT(buffer.rbhBufHeader.biLength >= buffer.rbhBufHeader.biUsed);
            // send the aligned length.
            result += clientSocket.send_data(in_message.buffer(), static_cast<int32_t>(buffer.rbhBufHeader.biLength));
        }
    }

    return result;
}

int32_t SocketConnectionBase::receive_message(RemoteMessage & out_message, const Socket & clientSocket) const
{
    int32_t result{ -1 };
    if ( clientSocket.is_valid() && clientSocket.is_alive() )
    {
        areg::MessageHeader msgHeader{};

        out_message.invalidate();
        result = clientSocket.receive_data(reinterpret_cast<uint8_t *>(&msgHeader), sizeof(areg::MessageHeader));
        if ( result == sizeof(areg::MessageHeader) )
        {
            result = sizeof(areg::MessageHeader);
            uint8_t * buffer = out_message.init_message( msgHeader );
            if ( (buffer != nullptr) && (msgHeader.rbhBufHeader.biUsed > 0))
            {
                ASSERT(msgHeader.rbhBufHeader.biLength >= msgHeader.rbhBufHeader.biUsed);

                // receive aligned length of data.
                result += clientSocket.receive_data(buffer, static_cast<int32_t>(msgHeader.rbhBufHeader.biLength));
            }

            out_message.move_to_begin();
            if ( out_message.is_checksum_valid() == false )
            {
                result = 0;
                out_message.invalidate();
            }
        }
        else
        {
            result = (result > 0) && (result != sizeof(areg::MessageHeader)) ? 0 : result;
        }
    }

    return result;
}

} // namespace areg
