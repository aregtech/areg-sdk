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

#ifdef  _WIN32

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/areg_macros.h"
#include "areg/base/MemoryDefs.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Mstcpip.h>
#include <WS2tcpip.h>

#include <algorithm>
#include <atomic>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Local static members
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OS specific socket namespace functions implementation
//////////////////////////////////////////////////////////////////////////
namespace areg::os {
    //!< Global socket initialize / release counter.
    std::atomic_uint _instanceCount(0u);


bool _os_init_socket()
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

void _os_configure_socket(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    areg::set_send_size(hSocket, areg::SOCKET_SEND_BUFFER_SIZE);
    areg::set_recv_size(hSocket, areg::SOCKET_RECV_BUFFER_SIZE);
}

void _os_configure_connected_socket(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(hSocket != areg::InvalidSocketHandle);

    constexpr int32_t keepAlive{ 1 };
    ::setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&keepAlive), sizeof(keepAlive));

    tcp_keepalive keepAliveValues{ };
    keepAliveValues.onoff             = 1u;
    keepAliveValues.keepalivetime     = 5'000u;
    keepAliveValues.keepaliveinterval = 1'000u;

    DWORD bytesReturned{ 0u };
    ::WSAIoctl(hSocket, SIO_KEEPALIVE_VALS, &keepAliveValues, static_cast<DWORD>(sizeof(keepAliveValues)), nullptr, 0u, &bytesReturned, nullptr, nullptr);

#ifdef SIO_TCP_SET_ACK_FREQUENCY
    DWORD ackFreq{ 2u };   // ACK every segment
    DWORD outBytes{ 0u };
    ::WSAIoctl(hSocket, SIO_TCP_SET_ACK_FREQUENCY, &ackFreq, static_cast<DWORD>(sizeof(ackFreq)), nullptr, 0u, &outBytes, nullptr, nullptr);
#endif  // SIO_TCP_SET_ACK_FREQUENCY
}

void _os_release_socket()
{
    if (_instanceCount.fetch_sub(1) == 1)
    {
        ::WSACleanup();
    }
}

void _os_close_socket(SOCKETHANDLE hSocket)
{
    shutdown(hSocket, SD_BOTH);
    closesocket(hSocket);
}

int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    // The socket has SO_SNDTIMEO set (see ServerConnectionBase::accept_connection),
    // so blocking send() returns WSAETIMEDOUT after the kernel-level timeout.
    // No application-level deadline, GetTickCount64, or WSAPoll is needed.
    int32_t total{ 0 };
    while (total < dataLength)
    {
        const int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), dataLength - total, 0);
        if (written > 0)
        {
            total += written;
        }
        else
        {
            return -1;  // SO_SNDTIMEO expired, connection error, or peer closed
        }
    }

    return total;
}

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((buffers != nullptr) && (count > 0u));

    // Fast path: single buffer.
    if (count == 1u)
    {
        return _os_send_data(hSocket, buffers[0u].data, static_cast<int32_t>(buffers[0u].size));
    }

    // On Windows, WSASend(WSABUF[n]) on a blocking non-overlapped socket iterates
    // each buffer entry sequentially inside the kernel -- it does NOT perform true
    // scatter/gather like Linux writev(). The per-buffer kernel overhead is therefore
    // equivalent to n individual ::send() calls with none of the syscall savings,
    // but the caller pays the full Phase-1+Phase-2 overhead of building a 128-entry
    // batch. The net result on Windows is a throughput regression.
    //
    // Fix: coalesce only tiny-message batches into one contiguous block in a thread-local
    // staging buffer and issue a single ::send() via _os_send_data(), which already
    // contains a correct partial-send retry loop.  For medium / large messages, the
    // extra full-batch memcpy becomes more expensive than the syscall savings, so
    // fall back to direct per-buffer sends.
    //
    // The thread-local buffer grows lazily to the maximum batch size this thread ever
    // uses and is then reused at zero allocation cost per call.
    //
    // Coalesce only when both the total batch and each entry stay tiny.
    constexpr uint32_t MAX_COALESCE_TOTAL  { 64u * 1024u };
    constexpr uint32_t MAX_COALESCE_ENTRY  { 1024u };

    uint32_t maxBufferSize{ 0u };
    uint32_t totalSize{ 0u };
    for (uint32_t i = 0u; i < count; ++i)
    {
        totalSize += buffers[i].size;
        maxBufferSize = buffers[i].size > maxBufferSize ? buffers[i].size : maxBufferSize;
    }

    if (totalSize == 0u)
    {
        return 0;
    }

    if ((totalSize <= MAX_COALESCE_TOTAL) && (maxBufferSize <= MAX_COALESCE_ENTRY))
    {
        static thread_local uint8_t _tlSendBuf[MAX_COALESCE_TOTAL];

        uint32_t offset{ 0u };
        for (uint32_t i{ 0u }; i < count; ++i)
        {
            ::memcpy(_tlSendBuf + offset, buffers[i].data, buffers[i].size);
            offset += buffers[i].size;
        }

        return _os_send_data(hSocket, _tlSendBuf, static_cast<int32_t>(totalSize));
    }

    // Fallback for non-tiny batches: send each buffer individually.
    // _os_send_data() handles partial sends via its internal retry loop.
    int32_t total{ 0 };
    for (uint32_t i{ 0u }; i < count; ++i)
    {
        const int32_t sent = _os_send_data(hSocket, buffers[i].data, static_cast<int32_t>(buffers[i].size));
        if (sent < 0)
        {
            return (total > 0) ? total : -1;
        }

        total += sent;
    }

    return total;
}

int32_t _os_recv_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    // Per-thread read-ahead buffer. One greedy recv() (no MSG_WAITALL) fills it
    // with all bytes currently in the kernel socket buffer — under burst load this
    // captures many complete messages at once. Header and payload reads then drain
    // from the cache without kernel transitions. Buffer is invalidated on socket
    // switch; for single-connection threads (ClientReceiveThread) the hit rate is
    // near-100%, cutting recv() kernel calls from 2×N to ~N/msgs_per_fill.
    struct RecvAhead
    {
        SOCKETHANDLE sock{ areg::InvalidSocketHandle };
        uint32_t     head{ 0u };
        uint32_t     avail{ 0u };
        uint8_t      buf[131072u];
    };
    static thread_local RecvAhead _ra;

    if (_ra.sock != hSocket)
    {
        _ra.sock  = hSocket;
        _ra.head  = 0u;
        _ra.avail = 0u;
    }

    const uint32_t needed = static_cast<uint32_t>(dataLength);
    uint32_t       total  = 0u;

    // Phase 1: serve from cache — zero kernel cost.
    if (_ra.avail > 0u)
    {
        const uint32_t take = (_ra.avail < needed) ? _ra.avail : needed;
        ::memcpy(dataBuffer, _ra.buf + _ra.head, take);
        _ra.head  += take;
        _ra.avail -= take;
        total     += take;

        if (total == needed)
            return static_cast<int32_t>(total);
    }

    // Phase 2: large request (> 64 KB) — bypass cache to avoid staging a multi-MB
    // payload through the 64 KB buffer. MSG_WAITALL delivers the exact byte count.
    if ((needed - total) > static_cast<uint32_t>(sizeof(_ra.buf)))
    {
        while (total < needed)
        {
            const int32_t r = ::recv(hSocket
                                    , reinterpret_cast<char*>(dataBuffer + total)
                                    , static_cast<int>(needed - total)
                                    , MSG_WAITALL);
            if (r <= 0)
                return (total > 0u) ? static_cast<int32_t>(total) : -1;

            total += static_cast<uint32_t>(r);
        }

        return static_cast<int32_t>(total);
    }

    // Phase 3: greedy cache refill. recv() without MSG_WAITALL on a blocking socket
    // blocks until at least 1 byte arrives, then returns all bytes currently queued
    // in the kernel buffer (up to 64 KB). Multiple back-to-back messages in the OS
    // receive buffer are captured in a single kernel transition.
    _ra.head  = 0u;
    _ra.avail = 0u;
    {
        const int32_t filled = ::recv(hSocket
                                     , reinterpret_cast<char*>(_ra.buf)
                                     , static_cast<int>(sizeof(_ra.buf))
                                     , 0);
        if (filled <= 0)
            return (total > 0u) ? static_cast<int32_t>(total) : -1;

        _ra.avail = static_cast<uint32_t>(filled);
    }

    {
        const uint32_t take = (_ra.avail < (needed - total)) ? _ra.avail : (needed - total);
        ::memcpy(dataBuffer + total, _ra.buf + _ra.head, take);
        _ra.head  += take;
        _ra.avail -= take;
        total     += take;
    }

    // Phase 4: greedy recv returned fewer bytes than needed (rare: kernel buffer had
    // only a partial message). Complete with MSG_WAITALL.
    while (total < needed)
    {
        const int32_t r = ::recv(hSocket
                                , reinterpret_cast<char*>(dataBuffer + total)
                                , static_cast<int>(needed - total)
                                , MSG_WAITALL);
        if (r <= 0)
            return (total > 0u) ? static_cast<int32_t>(total) : -1;

        total += static_cast<uint32_t>(r);
    }

    return static_cast<int32_t>(total);
}

bool _os_connect_socket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    ASSERT(addr != nullptr);

    u_long mode{ 1u };
    if (::ioctlsocket(hSocket, static_cast<long>(FIONBIO), &mode) != RETURNED_OK)
        return false;

    const int result = ::connect(hSocket, static_cast<const sockaddr*>(addr), static_cast<int>(addrLen));
    if (result == RETURNED_OK)
    {
        // Connected immediately
        mode = 0u;
        ::ioctlsocket(hSocket, static_cast<long>(FIONBIO), &mode);
        return true;
    }

    if (::WSAGetLastError() != WSAEWOULDBLOCK)
    {
        // Hard failure (e.g. WSAECONNREFUSED on loopback) -- no need to wait
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
    if (::getsockopt(hSocket, static_cast<int>(SOL_SOCKET), static_cast<int>(SO_ERROR), reinterpret_cast<char*>(&connError), &errLen) != RETURNED_OK || connError != 0)
        return false;

    // Restore blocking mode for normal I/O
    mode = 0u;
    ::ioctlsocket(hSocket, static_cast<long>(FIONBIO), &mode);
    return true;
}

bool _os_control(SOCKETHANDLE hSocket, int32_t cmd, unsigned long& arg)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    return (RETURNED_OK == ::ioctlsocket(hSocket, cmd, &arg));
}

bool _os_get_option(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(hSocket != areg::InvalidSocketHandle);
    int32_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<SOCKET>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // _WIN32
