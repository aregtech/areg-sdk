/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/NESocketPosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/SynchObjects.hpp"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"
#include "areg/trace/GETrace.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>      // IEEE Std 1003.1-2001
#include <atomic>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(areg_base_NESocketPosix__osCloseSocket);
DEF_TRACE_SCOPE(areg_base_NESocketPosix__osSendData);
DEF_TRACE_SCOPE(areg_base_NESocketPosix__osReceiveData);

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

namespace NESocket
{

    bool _osInitSocket(void)
    {
        return true;
    }

    void _osReleaseSocket(void)
    {
    }

    void _osCloseSocket(SOCKETHANDLE hSocket)
    {
        TRACE_SCOPE(areg_base_NESocketPosix__osCloseSocket);
        TRACE_WARN("Closing socket [ %p ]", hSocket);

        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        ::shutdown(hSocket, SHUT_RDWR);
        ::close(hSocket);
    }

    int _osSendData(SOCKETHANDLE hSocket, const unsigned char* dataBuffer, int dataLength, int blockMaxSize /*= -1*/)
    {
        TRACE_SCOPE(areg_base_NESocketPosix__osSendData);

        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        ASSERT((dataBuffer != nullptr) && (dataLength > 0));
        ASSERT(blockMaxSize > 0);

        int result{ dataLength };
        bool checkSize = false;

        TRACE_DBG("Going to send data, length = [ %d ], blockMaxSize [ %d ], socket is VALID", dataLength, blockMaxSize);
        while (dataLength > 0)
        {
            int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
            TRACE_DBG("Sending [ %d ] bytes of data", remain);
            int written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer), remain, 0);
            if (written > 0)
            {
                dataLength -= written;
                dataBuffer += written;
                TRACE_DBG("Has sent [ %d ] bytes of data, there is still [ %d ] bytes left", written, dataLength);
            }
            else
            {
                if ((checkSize == false) && (errno == EMSGSIZE))
                {
                    // try again with other package size
                    checkSize = true;
                    blockMaxSize = NESocket::getMaxSendSize(hSocket);
                    TRACE_WARN("No data has sent, trying to change the maximum block size to [ %d ] bytes and try again, there are still [ %d ] bytes to sent", blockMaxSize, dataLength);
                }
                else
                {
                    TRACE_ERR("FAILED to sent [ %d ] bytes of data, error code is [ %p ], terminating sending", dataLength, static_cast<id_type>(errno));
                    // in all other cases
                    dataLength = 0;    // break loop
                    result = -1;   // notify failure
                }
            }
        }

        return result;
    }

    int _osRecvData(SOCKETHANDLE hSocket, unsigned char* dataBuffer, int dataLength, int blockMaxSize)
    {
        TRACE_SCOPE(areg_base_NESocketPosix__osReceiveData);

        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        ASSERT((dataBuffer != nullptr) && (dataLength > 0));
        ASSERT(blockMaxSize > 0);

        int result{ 0 };

        while (dataLength > 0)
        {
            int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
            TRACE_DBG("Receiving [ %d ] bytes of data", remain);
            int read = ::recv(hSocket, dataBuffer + result, remain, 0);
            if (read > 0)
            {
                dataLength -= read;
                result += read;
                TRACE_DBG("Has read [ %d ] bytes of data, there is still [ %d ] bytes of data to wait, totally received [ %d ] bytes", read, dataLength, result);
            }
            else if (read == 0)
            {
                TRACE_INFO("No data to read. There are still [ %d ] bytes to wait, breaking loop", dataLength);
                dataLength = 0;    // break loop. the other side disconnected
                result = 0;    // no data could read. specified socket is closed
            }
            else
            {
                TRACE_ERR("FAILED to receive [ %d ] bytes of data, error code is [ %p ], terminating receiving", dataLength, static_cast<id_type>(errno));
                // in all other cases
                dataLength = 0;    // break loop
                result = -1;   // notify failure
            }
        }

        return result;
    }

    bool _osControl(SOCKETHANDLE hSocket, int cmd, unsigned long& arg)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        return (RETURNED_OK == ::ioctl(hSocket, cmd, &arg));
    }

    bool _osGetOption(SOCKETHANDLE hSocket, int level, int name, unsigned long& value)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        int len{ sizeof(unsigned long) };
        return (RETURNED_OK == ::getsockopt(static_cast<int>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
    }

} // namespace NESocket

#endif  // defined(_POSIX) || defined(POSIX)
