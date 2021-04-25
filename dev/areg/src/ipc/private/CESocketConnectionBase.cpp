/************************************************************************
 * \file        areg/src/ipc/private/CESocketConnectionBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Connection base class declaration.
 ************************************************************************/

#include "areg/src/ipc/private/CESocketConnectionBase.hpp"
#include "areg/src/base/CESocket.hpp"
#include "areg/src/base/CERemoteMessage.hpp"
#include "areg/src/base/NEMemory.hpp"

#include "areg/src/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_ipc_private_CESocketConnectionBase_SendMessage);
DEF_TRACE_SCOPE(areg_ipc_private_CESocketConnectionBase_ReceiveMessage);

CESocketConnectionBase::CESocketConnectionBase( void )
{
    ; // do nothing
}

CESocketConnectionBase::~CESocketConnectionBase( void )
{

}

int CESocketConnectionBase::SendMessage(const CERemoteMessage & in_message, const CESocket & clientSocket) const
{
    TRACE_SCOPE(areg_ipc_private_CESocketConnectionBase_SendMessage);

    int result = -1;
    const NEMemory::sRemoteMessageHeader & buffer = reinterpret_cast<const NEMemory::sRemoteMessageHeader &>( in_message.GetByteBuffer() );

    TRACE_DBG("Sending message with ID [ %p ] from source [ %p ] to target [ %p ] via client socket [ %u ] to address [ %s : %d ]. Buffer length [ %u ], used data length [ %u ], checksum [ %u ]"
                    , in_message.GetMessageId()
                    , in_message.GetSource()
                    , in_message.GetTarget()
                    , clientSocket.GetHandle()
                    , clientSocket.GetAddress().GetHostAddress().GetBuffer()
                    , clientSocket.GetAddress().GetHostPort()
                    , buffer.rbhBufHeader.biBufSize
                    , buffer.rbhBufHeader.biUsed
                    , buffer.rbhChecksum);

    if ( clientSocket.IsValid() )
    {
        // result = clientSocket.SendData( reinterpret_cast<const unsigned char *>(&buffer), buffer.rbhBufHeader.biBufSize );
        result = clientSocket.SendData( reinterpret_cast<const unsigned char *>(&buffer), sizeof(NEMemory::sRemoteMessageHeader) );
        if (buffer.rbhBufHeader.biUsed != 0)
        {
            result += clientSocket.SendData(in_message.GetBuffer(), buffer.rbhBufHeader.biUsed);
        }

        TRACE_DBG("Sent [ %d ] bytes of data. The remote buffer size is [ %u ], checksum [ %u ]", result, buffer.rbhBufHeader.biBufSize, buffer.rbhChecksum);
    }

    return result;
}

int CESocketConnectionBase::ReceiveMessage(CERemoteMessage & out_message, const CESocket & clientSocket) const
{
    TRACE_SCOPE(areg_ipc_private_CESocketConnectionBase_ReceiveMessage);

    int result = -1;
    if ( clientSocket.IsValid() )
    {
        NEMemory::sRemoteMessageHeader msgHeader;
        DBG_ZERO_MEM(&msgHeader, sizeof(NEMemory::sRemoteMessageHeader));

        out_message.InvalidateBuffer();
        if ( clientSocket.ReceiveData(reinterpret_cast<unsigned char *>(&msgHeader), sizeof(NEMemory::sRemoteMessageHeader)) == sizeof(NEMemory::sRemoteMessageHeader) )
        {
            TRACE_DBG("Going to receive remote message data of ID [ %p ] from target [ %p ] for source [ %p ], buffer length [ %u ], used data length [ %u ], checksum [ %u ]"
                                , msgHeader.rbhMessageId
                                , msgHeader.rbhTarget
                                , msgHeader.rbhSource
                                , msgHeader.rbhBufHeader.biBufSize
                                , msgHeader.rbhBufHeader.biUsed
                                , msgHeader.rbhChecksum);

            result = sizeof(NEMemory::sRemoteMessageHeader);
            unsigned char * buffer = out_message.InitializeMessage( msgHeader );
            if ( buffer != NULL && msgHeader.rbhBufHeader.biUsed > 0)
            {
                result += clientSocket.ReceiveData(buffer, msgHeader.rbhBufHeader.biUsed);
            }

            out_message.MoveToBeginOfData();
            if ( out_message.IsChecksumValid() == false )
            {
                TRACE_DBG("Received [ %u ] bytes of remote message, but checksum is invalid, ignoring and invalidating message", result);
                result = 0;
                out_message.InvalidateBuffer();
            }
            else
            {
                TRACE_DBG("Received [ %u ] bytes of remote message, checksum is valid, going to process", result);
            }
        }
        else
        {
            TRACE_WARN("Failed to receive remote message data. Probably the connection is closed");
        }
    }
    return result;
}
