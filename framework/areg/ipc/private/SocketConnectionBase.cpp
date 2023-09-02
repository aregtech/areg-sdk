/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/SocketConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Connection base class declaration.
 ************************************************************************/

#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/NEMemory.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_ipc_SocketConnectionBase_sendMessage);
DEF_TRACE_SCOPE(areg_ipc_SocketConnectionBase_receiveMessage);

int SocketConnectionBase::sendMessage(const RemoteMessage & in_message, const Socket & clientSocket) const
{
    TRACE_SCOPE(areg_ipc_SocketConnectionBase_sendMessage);

    int result = -1;
    if ( in_message.isValid() && clientSocket.isValid() )
    {
        const NEMemory::sRemoteMessageHeader & buffer = reinterpret_cast<const NEMemory::sRemoteMessageHeader &>( *in_message.getByteBuffer() );

        TRACE_DBG("Sending message with ID [ %u ] from source [ %llu ] to target [ %llu ] via socket [ %llu ], data length [ %u ], checksum [ %u ]"
                        , in_message.getMessageId()
                        , in_message.getSource()
                        , in_message.getTarget()
                        , clientSocket.getHandle()
                        , buffer.rbhBufHeader.biUsed
                        , buffer.rbhChecksum);

        result = clientSocket.sendData( reinterpret_cast<const unsigned char *>(&buffer), sizeof(NEMemory::sRemoteMessageHeader) );
        if ((result == sizeof(NEMemory::sRemoteMessageHeader)) && (buffer.rbhBufHeader.biUsed != 0))
        {
            ASSERT(buffer.rbhBufHeader.biLength >= buffer.rbhBufHeader.biUsed);
            // send the aligned length.
            result += clientSocket.sendData(in_message.getBuffer(), static_cast<int>(buffer.rbhBufHeader.biLength));
        }
    }
    else
    {
        TRACE_ERR("Either socket is [ %s ] or the remote buffer is [ %s ] to send"
                    , clientSocket.isValid() ? "VALID" : "INVALID"
                    , in_message.isValid() ? "VALID" : "INVALID");
    }

    return result;
}

int SocketConnectionBase::receiveMessage(RemoteMessage & out_message, const Socket & clientSocket) const
{
    TRACE_SCOPE(areg_ipc_SocketConnectionBase_receiveMessage);

    int result = -1;
    if ( clientSocket.isValid() && clientSocket.isAlive() )
    {
        NEMemory::sRemoteMessageHeader msgHeader{};

        out_message.invalidate();
        TRACE_DBG("Wait to receive message header");
        result = clientSocket.receiveData(reinterpret_cast<unsigned char *>(&msgHeader), sizeof(NEMemory::sRemoteMessageHeader));
        if ( result == sizeof(NEMemory::sRemoteMessageHeader) )
        {
            TRACE_DBG("Receiving message with ID [ %u ] from target [ %llu ] for source [ %llu ], data length [ %u ], checksum [ %u ]"
                                , msgHeader.rbhMessageId
                                , msgHeader.rbhTarget
                                , msgHeader.rbhSource
                                , msgHeader.rbhBufHeader.biLength
                                , msgHeader.rbhChecksum);

            result = sizeof(NEMemory::sRemoteMessageHeader);
            unsigned char * buffer = out_message.initMessage( msgHeader );
            if ( (buffer != nullptr) && (msgHeader.rbhBufHeader.biUsed > 0))
            {
                ASSERT(msgHeader.rbhBufHeader.biLength >= msgHeader.rbhBufHeader.biUsed);

                // receive aligned length of data.
                result += clientSocket.receiveData(buffer, static_cast<int>(msgHeader.rbhBufHeader.biLength));
            }

            out_message.moveToBegin();
            if ( out_message.isChecksumValid() == false )
            {
                TRACE_WARN("Received [ %u ] bytes of message data, but checksum is invalid, ignoring message", result);
                result = 0;
                out_message.invalidate();
            }
        }
        else
        {
            TRACE_WARN("Failed to receive message data. Probably the connection is closed, received [ %d ] bytes.", result);
            result = (result > 0) && (result != sizeof(NEMemory::sRemoteMessageHeader)) ? 0 : result;
        }
    }
    else
    {
        TRACE_WARN("Ignore processing message, the client socket is [ %s : %s ]"
                    , clientSocket.isValid() ? "VALID" : "INVALID"
                    , clientSocket.isAlive() ? "ALIVE" : "DEAD");
    }

    return result;
}
