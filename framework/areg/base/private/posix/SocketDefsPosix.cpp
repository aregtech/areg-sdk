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

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/areg_macros.h"
#include "areg/base/MemoryDefs.hpp"
#include "areg/logging/areg_log.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>      // IEEE Std 1003.1-2001
#include <fcntl.h>

namespace areg::os {

bool _os_init_socket()
{
    return true;
}

void _os_release_socket()
{
}

void _os_close_socket(SOCKETHANDLE hSocket)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ::shutdown(hSocket, SHUT_RDWR);
    ::close(hSocket);
}

DEBUG_DEF_LOG_SCOPE(areg_base_posix_SocketDefsPosix, _os_send_data);
int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    // The socket has SO_SNDTIMEO set (see ServerConnectionBase::accept_connection),
    // so blocking send() returns EAGAIN/EWOULDBLOCK after the kernel-level timeout.
    // No application-level deadline or clock_gettime is needed.
    //
    // MSG_NOSIGNAL suppresses SIGPIPE when the peer has closed the connection;
    // send() returns -1 with errno == EPIPE instead.  On macOS, SO_NOSIGPIPE
    // socket option achieves the same effect (set in socket_set_no_delay()).
#ifdef MSG_NOSIGNAL
    constexpr int sendFlags = MSG_NOSIGNAL;
#else
    constexpr int sendFlags = 0;
#endif

    int32_t total{ 0 };
    while (total < dataLength)
    {
        const int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), dataLength - total, sendFlags);
        if (written > 0)
        {
            total += written;
        }
        else if (errno == EINTR)
        {
            continue;   // interrupted by signal, retry immediately
        }
        else
        {
            DEBUG_LOG_SCOPE(areg_base_posix_SocketDefsPosix, _os_send_data);
            DEBUG_LOG_WARN("Send error: socket [ %u ], errno [ %d ], sent [ %d / %d ] bytes", static_cast<uint32_t>(hSocket), errno, total, dataLength);

            return -1;  // EAGAIN (SO_SNDTIMEO expired), EPIPE, or connection error
        }
    }

    return total;
}

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((buffers != nullptr) && (count > 0u));

    // Build iovec array on the stack. Batch sizes are bounded by THREAD_DRAIN_LIMIT
    // (128 entries), so 128 * sizeof(iovec) = 128 * 16 = 2 KB on the stack.
    constexpr uint32_t MAX_IOV{ 128u };
    const uint32_t iovCount{ (count < MAX_IOV) ? count : MAX_IOV };

    struct iovec iov[MAX_IOV];
    for (uint32_t i{ 0u }; i < iovCount; ++i)
    {
        // iov_base is void* in POSIX -- cast away const; writev does not modify buffers.
        iov[i].iov_base = const_cast<uint8_t*>(buffers[i].data);
        iov[i].iov_len  = static_cast<size_t>(buffers[i].size);
    }

    int32_t total{ 0 };

    // iovBase/iovRemaining track the current position within the iovec array when
    // writev() performs a partial write (rare on blocking sockets with SO_SNDTIMEO,
    // but handled correctly to preserve message framing integrity).
    uint32_t iovBase{ 0u };
    uint32_t iovRemaining{ iovCount };

    while (iovRemaining > 0u)
    {
        const ssize_t written = ::writev(static_cast<int>(hSocket),
                                         iov + iovBase,
                                         static_cast<int>(iovRemaining));
        if (written > 0)
        {
            total += static_cast<int32_t>(written);

            // Advance past fully-consumed iovecs.
            size_t advance{ static_cast<size_t>(written) };
            while ((iovBase < iovCount) && (advance >= iov[iovBase].iov_len))
            {
                advance -= iov[iovBase].iov_len;
                ++iovBase;
                --iovRemaining;
            }

            if ((iovRemaining > 0u) && (advance > 0u))
            {
                // Partial consume of the current iovec: slide the pointer.
                iov[iovBase].iov_base  = static_cast<uint8_t*>(iov[iovBase].iov_base) + advance;
                iov[iovBase].iov_len  -= advance;
            }
        }
        else if (errno == EINTR)
        {
            continue;   // interrupted by signal; retry without advancing
        }
        else
        {
            return -1;  // EAGAIN (SO_SNDTIMEO expired), EPIPE, or connection error
        }
    }

    return total;
}

int32_t _os_recv_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    // MSG_WAITALL asks the kernel to accumulate the full requested chunk before
    // returning, eliminating repeated userspace loop iterations for large payloads
    // (e.g. ~3 MB messages need only one recv() call instead of many).
    // The socket has SO_RCVTIMEO set so the kernel may still return a short read
    // if the timeout fires before all bytes arrive; the outer while-loop retries
    // in that case.  EINTR is also handled by the retry path below.
#ifdef MSG_WAITALL
    constexpr int recvFlags = MSG_WAITALL;
#else
    constexpr int recvFlags = 0;
#endif

    int32_t total{ 0 };

    while (total < dataLength)
    {
        int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), dataLength - total, recvFlags);
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
            continue;
        }
        else
        {
            return -1;
        }
    }

    return total;
}

bool _os_connect_socket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs)
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
        // Hard failure (e.g. ECONNREFUSED on loopback) -- no need to wait
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

bool _os_control(SOCKETHANDLE hSocket, int32_t cmd, unsigned long& arg)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    return (RETURNED_OK == ::ioctl(hSocket, cmd, &arg));
}

bool _os_get_option(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    socklen_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<int>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
