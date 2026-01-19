/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/SocketConnectionBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Connection base class declaration.
 ************************************************************************/

#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/NEMemory.hpp"

#include "areg/logging/GELog.h"

int SocketConnectionBase::sendMessage(const RemoteMessage & in_message, const Socket & clientSocket) const
{
    int result{ -1 };
    if ( in_message.isValid() && clientSocket.isValid() )
    {
        in_message.bufferCompletionFix();
        const NEMemory::sRemoteMessageHeader & buffer = reinterpret_cast<const NEMemory::sRemoteMessageHeader &>( *in_message.getByteBuffer() );
        result = clientSocket.sendData( reinterpret_cast<const unsigned char *>(&buffer), sizeof(NEMemory::sRemoteMessageHeader) );
        if ((result == sizeof(NEMemory::sRemoteMessageHeader)) && (buffer.rbhBufHeader.biUsed != 0))
        {
            ASSERT(buffer.rbhBufHeader.biLength >= buffer.rbhBufHeader.biUsed);
            // send the aligned length.
            result += clientSocket.sendData(in_message.getBuffer(), static_cast<int>(buffer.rbhBufHeader.biLength));
        }
    }

    return result;
}

int SocketConnectionBase::receiveMessage(RemoteMessage & out_message, const Socket & clientSocket) const
{
    int result{ -1 };
    if ( clientSocket.isValid() && clientSocket.isAlive() )
    {
        NEMemory::sRemoteMessageHeader msgHeader{};

        out_message.invalidate();
        result = clientSocket.receiveData(reinterpret_cast<unsigned char *>(&msgHeader), sizeof(NEMemory::sRemoteMessageHeader));
        if ( result == sizeof(NEMemory::sRemoteMessageHeader) )
        {
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
                result = 0;
                out_message.invalidate();
            }
        }
        else
        {
            result = (result > 0) && (result != sizeof(NEMemory::sRemoteMessageHeader)) ? 0 : result;
        }
    }

    return result;
}
