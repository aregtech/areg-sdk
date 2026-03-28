/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SocketDefsPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Socket POSIX specific wrappers methods
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/areg_macros.h"
#include "areg/base/MemoryDefs.hpp"
#include "areg/logging/areg_log.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>      // IEEE Std 1003.1-2001
#include <fcntl.h>

namespace areg::os {

bool _osInitSocket()
{
    return true;
}

void _osReleaseSocket()
{
}

void _osCloseSocket(SOCKETHANDLE hSocket)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ::shutdown(hSocket, SHUT_RDWR);
    ::close(hSocket);
}

int32_t _osSendData(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    int32_t total{ 0 };

    while (total < dataLength)
    {
        int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), dataLength - total, 0);
        if (written > 0)
        {
            total += written;
        }
        else if (errno == EINTR)
        {
            continue;   // interrupted by signal, retry
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
        else if (errno == EINTR)
        {
            continue;   // interrupted by signal, retry
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

    // Save current socket flags and switch to non-blocking mode.
    // This allows the connect to return EINPROGRESS immediately if the
    // server is not reachable, instead of blocking for the full OS TCP timeout.
    const int savedFlags = ::fcntl(hSocket, F_GETFL, 0);
    if (savedFlags == -1)
        return false;

    if (::fcntl(hSocket, F_SETFL, savedFlags | O_NONBLOCK) == -1)
        return false;

    const int result = ::connect(hSocket, static_cast<const sockaddr*>(addr), static_cast<socklen_t>(addrLen));
    if (result == RETURNED_OK)
    {
        // Connected immediately (rare on non-loopback but possible)
        ::fcntl(hSocket, F_SETFL, savedFlags);
        return true;
    }

    if (errno != EINPROGRESS)
    {
        // Hard failure (e.g. ECONNREFUSED on loopback) — no need to wait
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
    const int selectResult = ::select(static_cast<int>(hSocket) + 1, nullptr, &writeSet, &exceptSet, &tv);

    if (selectResult <= 0 || FD_ISSET(hSocket, &exceptSet))
        return false;   // Timeout or exceptional condition

    // Confirm the connection completed without error
    int connError{ 0 };
    socklen_t errLen{ sizeof(connError) };
    if (::getsockopt(hSocket, SOL_SOCKET, SO_ERROR, &connError, &errLen) != RETURNED_OK || connError != 0)
        return false;

    // Restore blocking mode for normal I/O
    ::fcntl(hSocket, F_SETFL, savedFlags);
    return true;
}

bool _osControl(SOCKETHANDLE hSocket, int32_t cmd, unsigned long& arg)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    return (RETURNED_OK == ::ioctl(hSocket, cmd, &arg));
}

bool _osGetOption(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    socklen_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<int>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
