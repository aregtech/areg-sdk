/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SocketDefsWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Socket Win32 specific wrappers methods
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"

#ifdef  _WIN32

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/areg_macros.h"
#include "areg/base/MemoryDefs.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <atomic>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

namespace {
    /**
     * \brief   Global socket initialize / release counter.
     *          Initialize socket in process if counter is changing from 0 to 1.
     *          Release socket in frees resources in process when counter reaches 0.
     **/
    std::atomic_uint _instanceCount( 0u );

} // namespace

//////////////////////////////////////////////////////////////////////////
// OS specific socket namespace functions implementation
//////////////////////////////////////////////////////////////////////////
namespace areg::os {

bool _osInitSocket()
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

void _osReleaseSocket()
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

int32_t _osSendData(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    int32_t total{ 0 };

    while (total < dataLength)
    {
        int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), dataLength - total, 0);
        if (written > 0)
        {
            total += written;
        }
        else
        {
            return -1;  // connection error or peer closed
        }
    }

    return total;
}

int32_t _osRecvData(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    int32_t total{ 0 };

    while (total < dataLength)
    {
        int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), dataLength - total, 0);
        if (received > 0)
        {
            total += received;
        }
        else if (received == 0)
        {
            break;  // peer closed the connection gracefully
        }
        else
        {
            return -1;  // connection error
        }
    }

    return total;
}

bool _osConnectSocket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT(addr != nullptr);

    // Switch to non-blocking mode so connect() returns WSAEWOULDBLOCK immediately
    // instead of blocking for the full OS TCP connection timeout.
    u_long mode{ 1u };
    if (::ioctlsocket(hSocket, FIONBIO, &mode) != RETURNED_OK)
        return false;

    const int result = ::connect(hSocket, static_cast<const sockaddr*>(addr), static_cast<int>(addrLen));
    if (result == RETURNED_OK)
    {
        // Connected immediately
        mode = 0u;
        ::ioctlsocket(hSocket, FIONBIO, &mode);
        return true;
    }

    if (::WSAGetLastError() != WSAEWOULDBLOCK)
    {
        // Hard failure (e.g. WSAECONNREFUSED on loopback) — no need to wait
        return false;
    }

    // Wait up to timeoutMs for the connection to complete
    fd_set writeSet;
    fd_set exceptSet;
    FD_ZERO(&writeSet);
    FD_ZERO(&exceptSet);
    FD_SET(hSocket, &writeSet);
    FD_SET(hSocket, &exceptSet);

    struct timeval tv;
    tv.tv_sec  = static_cast<decltype(tv.tv_sec)>(timeoutMs / 1000u);
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>((timeoutMs % 1000u) * 1000u);
    const int selectResult = ::select(0, nullptr, &writeSet, &exceptSet, &tv);  // First arg ignored on Windows

    if (selectResult <= 0 || FD_ISSET(hSocket, &exceptSet))
        return false;   // Timeout or exceptional condition

    // Confirm the connection completed without error
    int connError{ 0 };
    int errLen{ static_cast<int>(sizeof(connError)) };
    if (::getsockopt(hSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&connError), &errLen) != RETURNED_OK || connError != 0)
        return false;

    // Restore blocking mode for normal I/O
    mode = 0u;
    ::ioctlsocket(hSocket, FIONBIO, &mode);
    return true;
}

bool _osControl(SOCKETHANDLE hSocket, int32_t cmd, unsigned long& arg)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    return (RETURNED_OK == ::ioctlsocket(hSocket, cmd, &arg));
}

bool _osGetOption(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    int32_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<SOCKET>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // _WIN32
