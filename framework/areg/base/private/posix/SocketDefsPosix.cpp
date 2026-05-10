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

// Per-thread read-ahead cache. Populated by _os_recv_data whenever a recv()
// returns more bytes than the caller requested (i.e. the TCP layer delivered
// more than one protocol chunk in a single kernel wake-up). The excess bytes
// are kept here so that _os_recv_data can satisfy the next call without a
// blocking syscall, reducing recv() invocations from 2�N to ~1 per burst.
//
// Invariant: avail > 0 only while this thread is inside _os_recv_data or
// immediately after it returns, before the caller has had a chance to invoke
// recv_data_available() and drain the cache. Any thread that is about to
// block on a multiplexer (epoll/select) MUST drain the cache for each socket
// first via recv_data_available() / _os_recv_data() to avoid starving the
// multiplexer (the kernel buffer is empty once data is in the cache).
bool _os_init_socket()
{
    return true;
}

void _os_configure_connected_socket(SOCKETHANDLE hSocket) noexcept
{
    ASSERT(areg::is_valid_socket(hSocket));

#if defined(__linux__)
    // Emit aggressive keepalive probes so dead peers are detected quickly.
    // TCP_USER_TIMEOUT is intentionally not set: it aborts connections whenever
    // the send buffer stays full for the timeout duration.
    constexpr int32_t keepAlive{ 1 };
    constexpr int32_t keepIdle{ 5 };
    constexpr int32_t keepInterval{ 1 };
    constexpr int32_t keepCount{ 3 };
    ::setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&keepAlive), sizeof(keepAlive));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPIDLE, reinterpret_cast<const char *>(&keepIdle), sizeof(keepIdle));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPINTVL, reinterpret_cast<const char *>(&keepInterval), sizeof(keepInterval));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPCNT, reinterpret_cast<const char *>(&keepCount), sizeof(keepCount));

    // TCP_QUICKACK: suppress the 200 ms delayed-ACK for the initial message burst.
    // The kernel resets this flag after each receive, but setting it at connection time
    // ensures low-rate periods and connection startup are ACKed promptly rather than
    // stalling the sender for up to 200 ms. At high message rates (>1K msg/s) the
    // delayed-ACK timer never fires regardless, so there is no cost in that regime.
    constexpr int32_t quickAck{ 1 };
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_QUICKACK, reinterpret_cast<const char *>(&quickAck), sizeof(quickAck));
#elif defined(__APPLE__)
    // Darwin exposes per-socket idle time only. Interval and probe count stay at
    // the system defaults. SO_NOSIGPIPE suppresses SIGPIPE on broken peers.
    constexpr int32_t keepAlive{ 1 };
    constexpr int32_t keepIdle{ 5 };
    constexpr int32_t noSigPipe{ 1 };
    ::setsockopt(hSocket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const char *>(&keepAlive), sizeof(keepAlive));
    ::setsockopt(hSocket, IPPROTO_TCP, TCP_KEEPALIVE, reinterpret_cast<const char *>(&keepIdle), sizeof(keepIdle));
    ::setsockopt(hSocket, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<const char *>(&noSigPipe), sizeof(noSigPipe));
#endif  // __linux__ / __APPLE__
}

void _os_release_socket()
{
}

void _os_close_socket(SOCKETHANDLE hSocket)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ::shutdown(hSocket, SHUT_RDWR);
    ::close(hSocket);
}

DEBUG_DEF_LOG_SCOPE(areg_base_posix_SocketDefsPosix, _os_send_data);
int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    // The socket has SO_SNDTIMEO set (see ServerConnectionBase::accept_connection),
    // so blocking send() returns EAGAIN/EWOULDBLOCK after the kernel-level timeout.
    // No application-level deadline or clock_gettime is needed.
    //
    // MSG_NOSIGNAL suppresses SIGPIPE when the peer has closed the connection;
    // send() returns -1 with errno == EPIPE instead.  On macOS, SO_NOSIGPIPE
    // socket option achieves the same effect (set in socket_set_no_delay()).
#if defined(MSG_NOSIGNAL)
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

int32_t _os_send_data_v(SOCKETHANDLE hSocket, const areg::IoBuffer* buffers, uint32_t count, uint32_t /*totalSize*/)
{
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((buffers != nullptr) && (count > 0u));

    // Build iovec array on the stack. Batch sizes are bounded by THREAD_BATCH_LIMIT
    // (128 entries), so 128 * sizeof(iovec) = 128 * 16 = 2 KB on the stack.
    constexpr uint32_t MAX_IOV{ areg::THREAD_BATCH_LIMIT };
    const uint32_t iovCount{ (count < MAX_IOV) ? count : MAX_IOV };

    // Verify layout compatibility at compile time, then copy into a mutable local array.
    // The copy is required because the partial-write retry path must slide iov_base and
    // shrink iov_len in-place — impossible through a const pointer to the caller's buffer.
    static_assert(sizeof(areg::IoBuffer) == sizeof(struct iovec),         "IoBuffer/iovec size mismatch");
    static_assert(offsetof(areg::IoBuffer, data) == offsetof(struct iovec, iov_base), "IoBuffer/iovec data offset mismatch");
    static_assert(offsetof(areg::IoBuffer, size) == offsetof(struct iovec, iov_len),  "IoBuffer/iovec size offset mismatch");
    static_assert(sizeof(areg::IoBuffer::size) == sizeof(size_t),         "IoBuffer::size / iov_len width mismatch");
    struct iovec iov[MAX_IOV];
    ::memcpy(iov, buffers, iovCount * sizeof(struct iovec));

    // iovBase/iovRemaining track the current position within the iovec array when
    // writev() performs a partial write (rare on blocking sockets with SO_SNDTIMEO,
    // but handled correctly to preserve message framing integrity).
    int32_t total{ 0 };
    uint32_t iovBase{ 0u };
    uint32_t iovRemaining{ iovCount };

    while (iovRemaining > 0u)
    {
        const ssize_t written = ::writev(static_cast<int>(hSocket), iov + iovBase, static_cast<int>(iovRemaining));

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
    ASSERT(areg::is_valid_socket(hSocket));
    ASSERT((dataBuffer != nullptr) && (dataLength > 0));

    const uint32_t needed = static_cast<uint32_t>(dataLength);
    uint32_t total = 0u;
    areg::ThreadCache& tc = thread_local_cache();
    uint8_t* cache = tc.cache();

    // Socket-switch guard: if the thread is now serving a different socket,
    // discard any cached bytes from the previous socket before proceeding.
    if (tc.socket != hSocket)
    {
        tc.socket = hSocket;
        tc.head   = 0u;
        tc.unread = 0u;
    }

    // Phase 1: serve from cache -- zero kernel cost.
    if (tc.unread != 0u)
    {
        const uint32_t take = (tc.unread >= needed) ? needed : tc.unread;
        ::memcpy(dataBuffer, cache + tc.head, take);
        tc.head   += take;
        tc.unread -= take;
        total     += take;

        if (total == needed)
            return static_cast<int32_t>(total);

        // Partial cache hit -- advance the output pointer for the remaining phases.
        dataBuffer += take;
    }

    // Phase 2: large request (> tc.space bytes) -- bypass cache to avoid staging a
    // multi-MB body through the small cache buffer. MSG_WAITALL delivers exact bytes.
    if ((needed - total) > tc.space)
    {
        while (total < needed)
        {
            const int32_t receive = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer), static_cast<int>(needed - total), MSG_WAITALL);
            if (receive > 0)
            {
                total      += static_cast<uint32_t>(receive);
                dataBuffer += static_cast<uint32_t>(receive);
            }
            else if (receive == 0)
            {
                break;  // peer closed gracefully
            }
            else if (errno == EINTR)
            {
                continue;  // signal interrupted -- retry
            }
            else
            {
                return (total > 0u) ? static_cast<int32_t>(total) : -1;
            }
        }

        return (total > 0u) ? static_cast<int32_t>(total) : -1;
    }

    // Phase 3: greedy blocking cache refill.
    // One blocking recv() captures all bytes the kernel has buffered for this socket
    // (up to tc.space bytes). Under burst load this includes both the remaining needed
    // bytes AND surplus from the next message, turning two-recv-per-message into
    // approximately one recv per burst. Using flags=0 (blocking) instead of
    // MSG_DONTWAIT eliminates the two-syscall worst case (EAGAIN + Phase 4 fallback).
    tc.head   = 0u;
    tc.unread = 0u;
    {
        ssize_t filled{ 0 };
        do
        {
            filled = ::recv(hSocket, reinterpret_cast<char*>(cache), static_cast<size_t>(tc.space), 0);
        } while ((filled < 0) && (errno == EINTR));

        if (filled <= 0)
            return (total > 0u) ? static_cast<int32_t>(total) : -1;

        tc.unread = static_cast<uint32_t>(filled);
    }

    // Serve the remaining needed bytes from the cache (Phase 3 filled it).
    {
        const uint32_t remaining = needed - total;
        const uint32_t take = (tc.unread >= remaining) ? remaining : tc.unread;
        ::memcpy(dataBuffer, cache + tc.head, take);
        tc.head   += take;
        tc.unread -= take;
        total     += take;
        dataBuffer += take;

        if (total == needed)
            return static_cast<int32_t>(total);
    }

    // Phase 4: Phase 3 returned fewer bytes than needed (partial kernel fill, rare).
    // Block with MSG_WAITALL for the remaining bytes. Cumulative total ensures
    // the caller always sees the correct byte count even when Phases 1+3 contributed.
#ifdef MSG_WAITALL
    constexpr int recvExact = MSG_WAITALL;
#else
    constexpr int recvExact = 0;
#endif
    while (total < needed)
    {
        const int32_t receive = ::recv(hSocket, reinterpret_cast<char*>(dataBuffer), static_cast<int>(needed - total), recvExact);
        if (receive > 0)
        {
            total      += static_cast<uint32_t>(receive);
            dataBuffer += static_cast<uint32_t>(receive);
        }
        else if (receive == 0)
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

    if (total != static_cast<uint32_t>(dataLength))
        return total;   // short read (peer closed mid-message)

    // Post-fill: grab any surplus bytes the kernel already has buffered for this socket.
    // Store them in tc so the next _os_recv_data call is served from cache (zero syscall).
    // MSG_DONTWAIT ensures this never blocks; EINTR is not retried because the caller's
    // data is already complete and any retry belongs in the next call.
    {
        const ssize_t extra = ::recv(hSocket, reinterpret_cast<char*>(cache), tc.space, MSG_DONTWAIT);
        if (extra > 0)
        {
            tc.socket = hSocket;
            tc.head   = 0u;
            tc.unread = static_cast<uint32_t>(extra);
        }
        // extra == 0: peer closed (will be caught on the next call)
        // extra < 0 with EAGAIN/EWOULDBLOCK: nothing extra in kernel -- expected
        // extra < 0 with other errors: suppress here; next call returns -1 naturally
    }

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
