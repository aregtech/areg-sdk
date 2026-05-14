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

#include <atomic>

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
    ASSERT(areg::is_valid_socket(hSocket));
    ::shutdown(hSocket, SD_BOTH);
    ::closesocket(hSocket);

    if (areg::receive_mode() == ReceiveMode::MultiCache)
    {
        areg::thread_rx_cache_release(hSocket);
    }
}

int32_t _os_send_data_window(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    constexpr int32_t WINDOW{ static_cast<int32_t>(areg::DEFAULT_THREAD_CACHE_KB * areg::ONE_KILOBYTE) };
    int32_t total{ 0 };
    do
    {
        const int32_t remaining = dataLength - total;
        const int32_t chunk = remaining < WINDOW ? remaining : WINDOW;
        const int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), chunk, 0);
        if (written > 0)
            total += written;
        else if (::WSAGetLastError() != WSAEINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    int32_t total{ 0 };
    do
    {
        const int32_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), (dataLength - total), 0);
        if (written > 0)
            total += written;
        else if (::WSAGetLastError() != WSAEINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize)
{
    // Single buffer, bypass setup entirely.
    if (count == 1u)
    {
        return _os_send_data(hSocket, buffers->data, static_cast<int32_t>(buffers->size));
    }

    ASSERT(count <= areg::THREAD_BATCH_LIMIT);
    int32_t result = 0;
    areg::ThreadCache& tc = areg::thread_tx_cache();
    uint8_t* const staging = tc.cache();
    if (((totalSize / count) >= (tc.space / 2)) || (staging == nullptr))
    {
        for (uint32_t i = 0; i < count; ++i)
        {
            const int32_t written = _os_send_data_window(hSocket, buffers[i].data, static_cast<int32_t>(buffers[i].size));
            if (written < 0)
                return -1;

            result += written;
        }
    }
    else
    {
        uint32_t copied = static_cast<int32_t>(buffers->size);
        ::memcpy(staging, buffers->data, copied);
        ++buffers;
        for (uint32_t i = 1; i < count; ++i, ++buffers)
        {
            if ((copied + buffers->size) > tc.space)
            {
                const int32_t written = _os_send_data(hSocket, staging, copied);
                if (written < 0)
                    return -1;
                result += written;
                copied = 0;
            }

            ::memcpy(staging + copied, buffers->data, buffers->size);
            copied += static_cast<int32_t>(buffers->size);
        }

        if (copied != 0)
        {
            const int32_t written = _os_send_data(hSocket, staging, copied);
            if (written < 0)
                return -1;

            result += written;
        }
    }

    return result;
}

int32_t _os_recv_data_window(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    constexpr int32_t WINDOW{ static_cast<int32_t>(areg::DEFAULT_THREAD_CACHE_KB * areg::ONE_KILOBYTE) };
    int32_t total{ 0 };
    do
    {
        const int32_t remaining = dataLength - total;
        const int32_t chunk = remaining < WINDOW ? remaining : WINDOW;
        const int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), chunk, MSG_WAITALL);
        if (received > 0)
            total += received;
        else if (received == 0 || ::WSAGetLastError() != WSAEINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

// Blocking exact read -- MSG_WAITALL, no speculative buffering, no cache access.
static inline int32_t _recv_exact(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    int32_t total = 0;
    do
    {
        const int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), dataLength - total, MSG_WAITALL);
        if (received > 0)
            total += received;
        else if (received == 0 || ::WSAGetLastError() != WSAEINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

// Greedy cached read -- 3-phase strategy.
static inline int32_t _recv_cached(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    areg::ThreadCache& tc = areg::thread_rx_cache(hSocket);
    uint8_t* const cache = tc.cache();
    if (cache == nullptr)
    {
        return _recv_exact(hSocket, dataBuffer, dataLength);
    }

    uint32_t needed = static_cast<uint32_t>(dataLength);
    uint32_t total  = 0u;

    // Phase 1: serve from cache, zero kernel cost.
    if (tc.unread > 0u)
    {
        const uint32_t take = (tc.unread < needed) ? tc.unread : needed;
        ::memcpy(dataBuffer, cache + tc.head, take);
        tc.head   += take;
        tc.unread -= take;
        total     += take;
        if (total == needed)
            return static_cast<int32_t>(total);

        needed -= take;
    }

    // tc.unread == 0: cache is empty (drained by Phase 1 or was already empty).
    // Phase 2: large request, bypass cache, read directly into the caller's buffer.
    if (needed >= static_cast<uint32_t>(tc.space))
    {
        while (needed > 0u)
        {
            const int32_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), static_cast<int32_t>(needed), MSG_WAITALL);
            if (received > 0)
            {
                total  += static_cast<uint32_t>(received);
                needed -= static_cast<uint32_t>(received);
            }
            else if (received == 0 || ::WSAGetLastError() != WSAEINTR)
            {
                return -1;
            }
        }

        return static_cast<int32_t>(total);
    }

    // Phase 3: small request, fill cache greedily, serve needed bytes, keep surplus.
    // Reset to the start of the buffer (tc.head may be non-zero after a Phase 1 drain).
    tc.head   = 0u;
    tc.unread = 0u;
    while (tc.unread < needed)
    {
        const int32_t filled = ::recv(hSocket, reinterpret_cast<char*>(cache + tc.unread), static_cast<int>(tc.space - tc.unread), 0);
        if (filled > 0)
            tc.unread += static_cast<uint32_t>(filled);
        else if (filled == 0 || ::WSAGetLastError() != WSAEINTR)
        {
            tc.head   = 0u;
            tc.unread = 0u;
            return -1;
        }
    }

    // Cache holds >= needed bytes; copy needed and retain the rest.
    ::memcpy(dataBuffer + total, cache, needed);
    tc.head    = needed;
    tc.unread -= needed;
    total     += needed;
    return static_cast<int32_t>(total);
}

int32_t _os_recv_data(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    return (areg::receive_mode() == areg::ReceiveMode::NoCache)
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
        // Hard failure, no need to wait
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
