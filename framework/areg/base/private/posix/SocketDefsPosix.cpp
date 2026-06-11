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
#include "areg/ipc/private/ConnectionDefs.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <ctype.h>      // IEEE Std 1003.1-2001
#include <fcntl.h>

namespace areg::os {

// Per-thread receive cache used by Cached receive mode.
// Exact receive mode never speculatively reads beyond the caller request.
// On socket switch the cache is always invalidated.
bool _os_init_socket()
{
    return true;
}

void _os_configure_connected_socket(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(areg::is_valid_socket(hSocket));

#if defined(__linux__)
    // Emit aggressive keepalive probes so dead peers are detected quickly.
    // TCP_USER_TIMEOUT is intentionally not set. It aborts connections whenever
    // the send buffer stays full for the timeout duration.
    constexpr int32_t keepAlive     { 1 };
    constexpr int32_t keepIdle      { 5 };
    constexpr int32_t keepInterval  { 1 };
    constexpr int32_t keepCount     { 3 };

    ::setsockopt(hSocket, SOL_SOCKET , SO_KEEPALIVE, reinterpret_cast<const char *>(&keepAlive), sizeof(keepAlive));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPIDLE, reinterpret_cast<const char *>(&keepIdle), sizeof(keepIdle));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPINTVL, reinterpret_cast<const char *>(&keepInterval), sizeof(keepInterval));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPCNT, reinterpret_cast<const char *>(&keepCount), sizeof(keepCount));

    // TCP_QUICKACK: Suppress the 200 ms delayed-ACK for the initial message burst.
    //               The kernel resets this flag after each receive, but setting it 
    //               at connection time ensures low-rate periods and connection startup
    //               are ACKed promptly rather than stalling the sender for up to 200 ms.
    constexpr int32_t quickAck  { 1 };
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_QUICKACK, reinterpret_cast<const char *>(&quickAck), sizeof(quickAck));
#elif defined(__APPLE__)
    // Darwin exposes per-socket idle time only. Interval and probe count stay at the system defaults.
    // SO_NOSIGPIPE suppresses SIGPIPE on broken peers.
    constexpr int32_t keepAlive { 1 };
    constexpr int32_t keepIdle  { 5 };
    constexpr int32_t noSigPipe { 1 };
    ::setsockopt(hSocket, SOL_SOCKET , SO_KEEPALIVE, reinterpret_cast<const char *>(&keepAlive), sizeof(keepAlive));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPALIVE, reinterpret_cast<const char *>(&keepIdle), sizeof(keepIdle));
    ::setsockopt(hSocket, SOL_SOCKET , SO_NOSIGPIPE, reinterpret_cast<const char *>(&noSigPipe), sizeof(noSigPipe));
#else  // generic POSIX fallback (BSDs, QNX, etc.)
    constexpr int32_t keepAlive{ 1 };
    ::setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char*>(&keepAlive), sizeof(keepAlive));
    #if defined(TCP_KEEPIDLE)
        constexpr int32_t keepIdle{ 5 };
        ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPIDLE, reinterpret_cast<const char*>(&keepIdle), sizeof(keepIdle));
    #endif
    #if defined(TCP_KEEPINTVL)
        constexpr int32_t keepInterval{ 1 };
        ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPINTVL, reinterpret_cast<const char*>(&keepInterval), sizeof(keepInterval));
    #endif
    #if defined(TCP_KEEPCNT)
        constexpr int32_t keepCount{ 3 };
        ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPCNT, reinterpret_cast<const char*>(&keepCount), sizeof(keepCount));
    #endif
    #if defined(SO_NOSIGPIPE)
        constexpr int32_t noSigPipe{ 1 };
        ::setsockopt(hSocket, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<const char*>(&noSigPipe), sizeof(noSigPipe));
    #endif
#endif  // __linux__ / __APPLE__ / generic POSIX
}

void _os_release_socket()
{
}

void _os_close_socket(SOCKETHANDLE hSocket)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ::shutdown(hSocket, SHUT_RDWR);
    ::close(hSocket);

    areg::thread_rx_cache_release(hSocket);
}

int32_t _os_send_data_window(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

#if defined(MSG_NOSIGNAL)
    constexpr int sendFlags = MSG_NOSIGNAL;
#else
    constexpr int sendFlags = 0;
#endif

    constexpr int32_t WINDOW{ static_cast<int32_t>(areg::DEFAULT_THREAD_CACHE) };

    int32_t total{ 0 };
    do
    {
        const int32_t remaining = dataLength - total;
        const int32_t chunk = remaining < WINDOW ? remaining : WINDOW;
        const ssize_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), static_cast<size_t>(chunk), sendFlags);
        if (written > 0)
            total += static_cast<int32_t>(written);
        else if (errno != EINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

#if defined(MSG_NOSIGNAL)
    constexpr int sendFlags = MSG_NOSIGNAL;
#else
    constexpr int sendFlags = 0;
#endif

    int32_t total = 0;
    do
    {
        const ssize_t written = ::send(hSocket, reinterpret_cast<const char*>(dataBuffer + total), static_cast<size_t>(dataLength - total), sendFlags);
        if (written > 0)
            total += static_cast<int32_t>(written);
        else if (errno != EINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t totalSize)
{
    // Single buffer -- bypass setup entirely
    if (count == 1u)
    {
        return _os_send_data(hSocket, buffers->data, static_cast<int32_t>(buffers->size));
    }

    ASSERT(count <= areg::DEFAULT_DRAIN_LIMIT);
    int32_t result = 0;
    areg::ThreadCache& tc = areg::thread_tx_cache();
    uint8_t* const staging = tc.cache();
    if (((totalSize / count) >= areg::MIN_BIG_BLOCK) || (staging == nullptr))
    {
        struct iovec iov[areg::DEFAULT_DRAIN_LIMIT];
        for (uint32_t i = 0u; i < count; ++i)
        {
            iov[i].iov_base = const_cast<uint8_t*>(buffers[i].data);
            iov[i].iov_len  = buffers[i].size;
        }

        uint32_t idx = 0u;  // first region not yet fully sent
        while (idx < count)
        {
            const ssize_t written = ::writev(static_cast<int>(hSocket), &iov[idx], static_cast<int>(count - idx));
            if (written < 0)
            {
                if (errno == EINTR)
                    continue;
                return -1;
            }

            result += static_cast<int32_t>(written);

            size_t advance = static_cast<size_t>(written);
            while ((idx < count) && (advance >= iov[idx].iov_len))
            {
                advance -= iov[idx].iov_len;
                ++idx;
            }
            if ((idx < count) && (advance > 0u))
            {
                iov[idx].iov_base = static_cast<uint8_t*>(iov[idx].iov_base) + advance;
                iov[idx].iov_len -= advance;
            }
        }
    }
    else
    {
        // AAvetyan:
        //      I've noticed that in case of small buffers `writev()` performs a little slower.
        //      Copying in one buffer is faster than sending a list.
        //      the condition above `(tc.space / 3)` must be tuned.
        uint32_t copied = 0u;
        for (uint32_t i = 0u; i < count; ++i, ++buffers)
        {
            const uint32_t bsize = static_cast<uint32_t>(buffers->size);
            if (bsize > tc.space)
            {
                if (copied != 0u)
                {
                    const int32_t written = _os_send_data(hSocket, staging, copied);
                    if (written < 0)
                        return -1;
                    result += written;
                    copied = 0u;
                }

                const int32_t written = _os_send_data(hSocket, buffers->data, static_cast<int32_t>(bsize));
                if (written < 0)
                    return -1;
                result += written;
                continue;
            }

            if ((copied + bsize) > tc.space)
            {
                const int32_t written = _os_send_data(hSocket, staging, copied);
                if (written < 0)
                    return -1;
                result += written;
                copied = 0u;
            }

            ::memcpy(staging + copied, buffers->data, bsize);
            copied += bsize;
        }

        if (copied != 0u)
        {
            const int32_t written = _os_send_data(hSocket, staging, copied);
            if (written < 0)
                return -1;

            result += written;
        }
    }

    return result;
}

// Blocking exact read
// MSG_WAITALL, no speculative buffering, no cache access.
static int32_t _recv_exact(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
#ifdef MSG_WAITALL
    constexpr int recvExact = MSG_WAITALL;
#else
    constexpr int recvExact = 0;
#endif

    int32_t total = 0;
    do
    {
        const ssize_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), static_cast<size_t>(dataLength - total), recvExact);
        if (received > 0)
            total += static_cast<int32_t>(received);
        else if (received == 0 || errno != EINTR)
            return -1;
    } while (total < dataLength);
    return total;
}

// Cached read, 3-phase strategy:
//  - Phase 1:  serve from thread-local cache.
//  - Phase 2:  large request (needed >= cache capacity) -- reads directly into the
//              caller's buffer with MSG_WAITALL; the cache plays no role.
//  - Phase 3:  small request (needed < cache capacity) -- fills the cache greedily:
//              recv(flag=0) returns as soon as ANY bytes arrive in the kernel buffer
//              (not waiting for a full cache), so no idle-period block occurs.
static int32_t _recv_cached(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
#ifdef MSG_WAITALL
    constexpr int recvExact = MSG_WAITALL;
#else
    constexpr int recvExact = 0;
#endif

    areg::ThreadCache& tc = areg::thread_rx_cache(hSocket);
    uint8_t* const cache = tc.cache();
    if ((cache == nullptr) || (tc.space == 0u))
        return _recv_exact(hSocket, dataBuffer, dataLength);

    uint32_t needed = static_cast<uint32_t>(dataLength);
    uint32_t total  = 0u;

    // Phase 1: serve from cache, no kernel cost.
    if (tc.unread != 0u)
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

    // Phase 2: large request, bypass cache, read directly into the caller's buffer.
    if (dataLength >= static_cast<int32_t>(tc.space))
    {
        while (needed > 0u)
        {
            const ssize_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), static_cast<size_t>(needed), recvExact);
            if (received > 0)
            {
                total  += static_cast<uint32_t>(received);
                needed -= static_cast<uint32_t>(received);
            }
            else if (received == 0 || errno != EINTR)
            {
                return -1;
            }
        }

        return static_cast<int32_t>(total);
    }

    // Phase 3: small request -- fill cache, serve needed bytes, keep surplus.
    // Reset to the start of the buffer
    tc.head   = 0u;
    tc.unread = 0u;
    while (tc.unread < needed)
    {
        const ssize_t filled = ::recv(hSocket, reinterpret_cast<char*>(cache + tc.unread), static_cast<size_t>(tc.space - tc.unread), 0);
        if (filled > 0)
            tc.unread += static_cast<uint32_t>(filled);
        else if (filled == 0 || errno != EINTR)
        {
            tc.head   = 0u;
            tc.unread = 0u;
            return -1;
        }
    }

    // Cache holds >= needed bytes from cache[0]; copy needed and retain the rest.
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

int32_t _os_recv_data_window(SOCKETHANDLE hSocket, uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

#ifdef MSG_WAITALL
    constexpr int recvExact = MSG_WAITALL;
#else
    constexpr int recvExact = 0;
#endif

    constexpr int32_t WINDOW{ static_cast<int32_t>(areg::DEFAULT_THREAD_CACHE) };
    int32_t total{ 0 };
    do
    {
        const int32_t remaining = dataLength - total;
        const int32_t chunk = remaining < WINDOW ? remaining : WINDOW;
        const ssize_t received = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer + total), static_cast<size_t>(chunk), recvExact);
        if (received > 0)
            total += static_cast<int32_t>(received);
        else if (received == 0 || errno != EINTR)
            return -1;
    } while (total < dataLength);

    return total;
}

bool _os_connect_socket(SOCKETHANDLE hSocket, const void* addr, uint32_t addrLen, uint32_t timeoutMs)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT(addr != nullptr);

    // Save current socket flags and switch to non-blocking mode.
    const int savedFlags = ::fcntl(hSocket, F_GETFL, 0);
    if (savedFlags == -1)
        return false;

    if (::fcntl(hSocket, F_SETFL, savedFlags | O_NONBLOCK) == -1)
        return false;

    const int result = ::connect(hSocket, static_cast<const sockaddr*>(addr), static_cast<socklen_t>(addrLen));
    if (result == RETURNED_OK)
    {
        // Connected immediately
        ::fcntl(hSocket, F_SETFL, savedFlags);
        return true;
    }

    if (errno != EINPROGRESS)
    {
        return false;   // Hard failure
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

    if ((selectResult <= 0) || FD_ISSET(hSocket, &exceptSet))
        return false;   // Timeout or exceptional condition

    int connError{ 0 };
    socklen_t errLen{ sizeof(connError) };
    if ((::getsockopt(hSocket, SOL_SOCKET, SO_ERROR, &connError, &errLen) != RETURNED_OK) || (connError != 0))
        return false;

    // Restore blocking mode for normal I/O
    ::fcntl(hSocket, F_SETFL, savedFlags);
    return true;
}

bool _os_control(SOCKETHANDLE hSocket, int32_t cmd, unsigned long& arg)
{
    ASSERT(areg::is_valid_socket(hSocket));
    return (RETURNED_OK == ::ioctl(hSocket, cmd, &arg));
}

bool _os_get_option(SOCKETHANDLE hSocket, int32_t level, int32_t name, unsigned long& value)
{
    ASSERT(areg::is_valid_socket(hSocket));
    socklen_t len{ sizeof(unsigned long) };
    return (RETURNED_OK == ::getsockopt(static_cast<int>(hSocket), level, name, reinterpret_cast<char*>(&value), &len));
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
