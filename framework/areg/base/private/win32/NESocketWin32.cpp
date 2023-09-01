/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/NESocketWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/NESocket.hpp"

#ifdef  _WINDOWS

#include "areg/base/SynchObjects.hpp"
#include "areg/base/GEMacros.h"
#include "areg/base/NEMemory.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

namespace
{
	/**
	 * \brief   Global socket initialize / release counter.
	 *          Initialize socket in process if counter is changing from 0 to 1.
	 *          Release socket in frees resources in process when counter reaches 0.
	 **/
	std::atomic_uint _instanceCount( 0u );

} // namespace

//////////////////////////////////////////////////////////////////////////
// NESocket namespace functions implementation
//////////////////////////////////////////////////////////////////////////

namespace NESocket
{
    bool _osInitSocket(void)
    {
        bool result = true;
        if (_instanceCount.fetch_add(1) == 0)
        {
            WSADATA wsaData;
            ::memset(&wsaData, 0, sizeof(WSADATA));
            if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                result = false;
                _instanceCount.fetch_sub(1);
            }
        }

        return result;
    }

    void _osReleaseSocket(void)
    {
        if (_instanceCount.fetch_sub(1) == 1)
        {
            ::WSACleanup();
        }
    }

    void _osCloseSocket(SOCKETHANDLE hSocket)
    {
        shutdown(hSocket, SD_BOTH);
        closesocket(hSocket);
    }


    int _osSendData(SOCKETHANDLE hSocket, const unsigned char* dataBuffer, int dataLength, int blockMaxSize)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        ASSERT((dataBuffer != nullptr) && (dataLength > 0));
        ASSERT(blockMaxSize > 0);

        int result{ dataLength };

        while (dataLength > 0)
        {
            int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
            int written = send(hSocket, reinterpret_cast<const char*>(dataBuffer), remain, 0);
            if (written > 0)
            {
                dataLength -= written;
                dataBuffer += written;
            }
            else
            {
                int errCode = ::WSAGetLastError();
                if (errCode == static_cast<int>(WSAEMSGSIZE))
                {
                    // try again with other package size
                    blockMaxSize = NESocket::getMaxSendSize(hSocket);
                }
                else
                {
                    // in all other cases
                    dataLength = 0; // break loop
                    result = -1;     // notify failure
                }
            }
        }

        return result;
    }

    int _osRecvData(SOCKETHANDLE hSocket, unsigned char* dataBuffer, int dataLength, int blockMaxSize)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        ASSERT((dataBuffer != nullptr) && (dataLength > 0));
        ASSERT(blockMaxSize > 0);

        int result{ 0 };

        while (dataLength > 0)
        {
            int remain = dataLength > blockMaxSize ? blockMaxSize : dataLength;
            int read = recv(hSocket, reinterpret_cast<char*>(dataBuffer) + result, remain, 0);
            if (read > 0)
            {
                dataLength -= read;
                result += read;
            }
            else if (read == 0)
            {
                dataLength = 0; // break loop. the other side disconnected
                result = 0;     // no data could read. specified socket is closed
            }
            else
            {
                int errCode = ::WSAGetLastError();
                if (errCode == static_cast<int>(WSAEMSGSIZE))
                {
                    // try again with other package size
                    blockMaxSize = NESocket::getMaxReceiveSize(hSocket);
                }
                else
                {
                    // in all other cases
                    dataLength = 0; // break loop
                    result = -1;    // notify failure
                }
            }
        }

        return result;
    }

    bool _osControl(SOCKETHANDLE hSocket, int cmd, unsigned long& arg)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        return (RETURNED_OK == ::ioctlsocket(hSocket, cmd, &arg));
    }

    bool _osGetOption(SOCKETHANDLE hSocket, int level, int name, unsigned long& value)
    {
        ASSERT(hSocket != NESocket::InvalidSocketHandle);
        int len{ sizeof(unsigned long) };
        return (RETURNED_OK == ::getsockopt(static_cast<SOCKET>(hSocket), level, name, reinterpret_cast<char *>(&value), &len));
    }

} // namespace NESocket

#endif  // _WINDOWS
