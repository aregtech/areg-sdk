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

void _os_configure_connected_socket(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(areg::is_valid_socket(hSocket));

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
    ::shutdown(hSocket, SD_BOTH);
    ::closesocket(hSocket);
}

int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
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

// Coalesce all buffers into one contiguous block and send in a single syscall.
static int32_t _send_coalesced(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize)
{
    areg::ThreadCache& tc = areg::thread_local_cache();
    uint8_t* const cache = tc.cache();
    ASSERT((cache != nullptr) && (totalSize <= tc.space));

    uint32_t offset{ 0u };
    for (uint32_t i{ 0u }; i < count; ++i)
    {
        ::memcpy(cache + offset, buffers[i].data, buffers[i].size);
        offset += static_cast<uint32_t>(buffers[i].size);
    }

    return _os_send_data(hSocket, cache, static_cast<int32_t>(totalSize));
}

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((buffers != nullptr) && (count > 0u));

    // Fast path: single buffer — no staging overhead.
    if (count == 1u)
    {
        return _os_send_data(hSocket, buffers[0u].data, static_cast<int32_t>(buffers[0u].size));
    }

    // On Windows, WSASend(WSABUF[n]) on a blocking non-overlapped socket iterates
    // each buffer entry sequentially inside the kernel — it does NOT perform true
    // scatter/gather like POSIX writev(). Per-buffer kernel transitions are
    // equivalent in cost to N individual ::send() calls, eliminating any batching gain.
    //
    // Fix: coalesce all buffers into the thread-local staging buffer and flush
    // in one or more ::send() calls, each of exactly tc.space bytes (or the final
    // remainder). The number of send() syscalls is ceil(totalSize / tc.space),
    // not N — e.g. 128 × 3 KB = 384 KB with 512 KB staging → 1 send(),
    // 128 × 5 KB = 640 KB with 512 KB staging → 2 send() calls.

    if (totalSize == 0u)
    {
        for (uint32_t i = 0u; i < count; ++i)
            totalSize += static_cast<uint32_t>(buffers[i].size);

        ASSERT(totalSize != 0u);
    }

    // tc.cache() initialises tc.space on first call; must be called before comparing.
    areg::ThreadCache& tc = areg::thread_local_cache();
    uint8_t* const cache = tc.cache();

    // Entire batch fits in one chunk — single send() syscall, no partial-flush loop.
    if (totalSize <= tc.space)
        return _send_coalesced(hSocket, buffers, count, totalSize);

    // Batch exceeds staging buffer: coalesce into tc.space-byte chunks and flush each.
    // This is structurally correct for any batch size — memory usage is bounded by tc.space.
    if ((cache == nullptr) || (tc.space == 0u))
    {
        // No staging buffer (zero-cache config): send each buffer individually.
        int32_t total{ 0 };
        for (uint32_t i{ 0u }; i < count; ++i)
        {
            const int32_t sent = _os_send_data(hSocket, buffers[i].data, static_cast<int32_t>(buffers[i].size));
            if (sent < 0)
                return (total > 0) ? total : -1;

            total += sent;
        }

        return total;
    }

    int32_t totalSent{ 0 };
    uint32_t chunkFill{ 0u };   // bytes currently staged in the cache

    for (uint32_t i{ 0u }; i < count; ++i)
    {
        const uint8_t* src = buffers[i].data;
        uint32_t remaining = static_cast<uint32_t>(buffers[i].size);

        while (remaining > 0u)
        {
            const uint32_t room = tc.space - chunkFill;
            const uint32_t copy = (remaining < room) ? remaining : room;
            ::memcpy(cache + chunkFill, src, copy);
            src       += copy;
            chunkFill += copy;
            remaining -= copy;

            if (chunkFill == tc.space)
            {
                const int32_t sent = _os_send_data(hSocket, cache, static_cast<int32_t>(tc.space));
                if (sent < 0)
                    return (totalSent > 0) ? totalSent : -1;

                totalSent += sent;
                chunkFill  = 0u;
            }
        }
    }

    // Flush any partial final chunk.
    if (chunkFill > 0u)
    {
        const int32_t sent = _os_send_data(hSocket, cache, static_cast<int32_t>(chunkFill));
        if (sent < 0)
            return (totalSent > 0) ? totalSent : -1;

        totalSent += sent;
    }

    return totalSent;
}

// Blocking exact read — MSG_WAITALL loop, no speculative buffering, no cache access.
static int32_t _recv_exact(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    int32_t total{ 0 };
    while (total < dataLength)
    {
        const int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), dataLength - total, MSG_WAITALL);
        if (received > 0)
        {
            total += received;
        }
        else if ((received == SOCKET_ERROR) && (::WSAGetLastError() == WSAEINTR))
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

// Greedy cached read — 3-phase speculative buffering.
//   Phase 1: serve from thread-local cache (zero syscall on cache hit).
//   Phase 2: one greedy recv() fills the cache; serve needed bytes, keep surplus.
//   Phase 3: MSG_WAITALL completion for the rare partial-fill or oversized-request case.
static int32_t _recv_cached(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    areg::ThreadCache& tc = areg::thread_local_cache();
    uint8_t* const cache = tc.cache();
    if ((cache == nullptr) || (tc.space == 0u))
        return _recv_exact(hSocket, dataBuffer, dataLength);

    if (tc.socket != hSocket)
    {
        tc.socket = hSocket;
        tc.head   = 0u;
        tc.unread = 0u;
    }

    const uint32_t needed = static_cast<uint32_t>(dataLength);
    uint32_t total = 0u;

    // Phase 1: serve from cache — zero kernel cost.
    if (tc.unread > 0u)
    {
        const uint32_t take = (tc.unread < needed) ? tc.unread : needed;
        ::memcpy(dataBuffer, cache + tc.head, take);
        tc.head   += take;
        tc.unread -= take;
        total     += take;
        if (total == needed)
            return static_cast<int32_t>(needed);
    }

    // Phase 2: greedy fill — one blocking recv captures all bytes in the kernel buffer.
    if ((needed - total) <= tc.space)
    {
        tc.head   = 0u;
        tc.unread = 0u;

        int32_t filled{ 0 };
        do
        {
            filled = ::recv(hSocket, reinterpret_cast<char*>(cache), static_cast<int>(tc.space), 0);
        } while ((filled == SOCKET_ERROR) && (::WSAGetLastError() == WSAEINTR));

        if (filled <= 0)
            return -1;

        tc.unread = static_cast<uint32_t>(filled);
        const uint32_t take = (tc.unread < (needed - total)) ? tc.unread : (needed - total);
        ::memcpy(dataBuffer + total, cache, take);
        tc.head   = take;
        tc.unread -= take;
        total     += take;
    }

    // Phase 3: complete any remainder — rare (partial greedy fill or oversized request).
    while (total < needed)
    {
        const int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), static_cast<int>(needed - total), MSG_WAITALL);
        if (received > 0)
        {
            total += static_cast<uint32_t>(received);
        }
        else if ((received == SOCKET_ERROR) && (::WSAGetLastError() == WSAEINTR))
        {
            continue;
        }
        else
        {
            tc.head   = 0u;
            tc.unread = 0u;
            return -1;
        }
    }

    return static_cast<int32_t>(needed);
}

int32_t _os_recv_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    return (areg::receive_mode() == areg::ReceiveMode::Exact)
         ? _recv_exact(hSocket, dataBuffer, dataLength)
         : _recv_cached(hSocket, dataBuffer, dataLength);
}

bool _os_connect_socket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs)
{
    ASSERT(areg::is_valid_socket(hSocket));
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
    ASSERT(areg::is_valid_socket(hSocket));
    return (RETURNED_OK == ::ioctlsocket(hSocket, cmd, &arg));
}

bool _os_get_option(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(areg::is_valid_socket(hSocket));
    int32_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<SOCKET>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // _WIN32
