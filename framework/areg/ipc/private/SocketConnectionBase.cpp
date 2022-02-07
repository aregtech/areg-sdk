/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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

        TRACE_DBG("Sending message with ID [ %p ] from source [ %p ] to target [ %p ] via client socket [ %u ] to address [ %s : %d ]. Buffer length [ %u ], used data [ %u ], data lengt [ %u ], checksum [ %u ]"
                        , static_cast<id_type>(in_message.getMessageId())
                        , static_cast<id_type>(in_message.getSource())
                        , static_cast<id_type>(in_message.getTarget())
                        , static_cast<unsigned int>(clientSocket.getHandle())
                        , clientSocket.getAddress().getHostAddress().c_str()
                        , clientSocket.getAddress().getHostPort()
                        , buffer.rbhBufHeader.biBufSize
                        , buffer.rbhBufHeader.biUsed
                        , buffer.rbhBufHeader.biLength
                        , buffer.rbhChecksum);

        TRACE_DBG("Sending message [ %p ] of [ %d ] bytes of header data, follow data is [ ] bytes."
                            , in_message.getMessageId()
                            , sizeof(NEMemory::sRemoteMessageHeader)
                            , buffer.rbhBufHeader.biUsed);
        result = clientSocket.sendData( reinterpret_cast<const unsigned char *>(&buffer), sizeof(NEMemory::sRemoteMessageHeader) );
        if ((result == sizeof(NEMemory::sRemoteMessageHeader)) && (buffer.rbhBufHeader.biUsed != 0))
        {
            ASSERT(buffer.rbhBufHeader.biLength >= buffer.rbhBufHeader.biUsed);
            // send the aligned length.
            result += clientSocket.sendData(in_message.getBuffer(), static_cast<int>(buffer.rbhBufHeader.biLength));
            TRACE_DBG("Sent [ %d ] bytes of aligned data, used size [ %d ]", buffer.rbhBufHeader.biLength, buffer.rbhBufHeader.biUsed);
        }

        TRACE_DBG("Sent [ %d ] bytes of data. The remote buffer size is [ %u ], checksum [ %u ]", result, buffer.rbhBufHeader.biBufSize, buffer.rbhChecksum);
    }
    else
    {
        TRACE_ERR("Either socket is invalid or the remote buffer to send");
    }

    return result;
}

int SocketConnectionBase::receiveMessage(RemoteMessage & out_message, const Socket & clientSocket) const
{
    TRACE_SCOPE(areg_ipc_SocketConnectionBase_receiveMessage);

    int result = -1;
    if ( clientSocket.isValid() && clientSocket.isAlive() )
    {
        NEMemory::sRemoteMessageHeader msgHeader;
        DBG_ZERO_MEM(&msgHeader, sizeof(NEMemory::sRemoteMessageHeader));

        out_message.invalidate();
        TRACE_DBG("Going to receive [ %d ] bytes of message header data.", sizeof(NEMemory::sRemoteMessageHeader));
        result = clientSocket.receiveData(reinterpret_cast<unsigned char *>(&msgHeader), sizeof(NEMemory::sRemoteMessageHeader));
        if ( result == sizeof(NEMemory::sRemoteMessageHeader) )
        {
            TRACE_DBG("Going to receive remote message data of ID [ %p ] from target [ %p ] for source [ %p ], buffer length [ %u ], used data length [ %u ], checksum [ %u ]"
                                , static_cast<id_type>(msgHeader.rbhMessageId)
                                , static_cast<id_type>(msgHeader.rbhTarget)
                                , static_cast<id_type>(msgHeader.rbhSource)
                                , msgHeader.rbhBufHeader.biBufSize
                                , msgHeader.rbhBufHeader.biUsed
                                , msgHeader.rbhChecksum);

            result = sizeof(NEMemory::sRemoteMessageHeader);
            unsigned char * buffer = out_message.initMessage( msgHeader );
            if ( (buffer != nullptr) && (msgHeader.rbhBufHeader.biUsed > 0))
            {
                ASSERT(msgHeader.rbhBufHeader.biLength >= msgHeader.rbhBufHeader.biUsed);

                // receive aligned length of data.
                result += clientSocket.receiveData(buffer, static_cast<int>(msgHeader.rbhBufHeader.biLength));
                TRACE_DBG("Received [ %d ] bytes of aligned data, used size [ %d ]", msgHeader.rbhBufHeader.biLength, msgHeader.rbhBufHeader.biUsed);
            }

            out_message.moveToBegin();
            if ( out_message.isChecksumValid() == false )
            {
                TRACE_DBG("Received [ %u ] bytes of remote message, but checksum is invalid, ignoring and invalidating message", result);
                result = 0;
                out_message.invalidate();
            }
            else
            {
                TRACE_DBG("Received [ %u ] bytes of remote message, checksum is valid, going to process", result);
            }
        }
        else
        {
            TRACE_WARN("Failed to receive remote message data. Probably the connection is closed, received [ %d ] bytes.", result);
            result = (result > 0) && (result != sizeof(NEMemory::sRemoteMessageHeader)) ? 0 : result;
        }
    }

    return result;
}
