/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SocketMultiplexerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. SocketMultiplexer POSIX implementation.
 *              Linux path  : epoll + eventfd for O(1) readiness and wakeup.
 *              macOS path  : poll() + anonymous pipe for wakeup.
 ************************************************************************/
#include "areg/base/SocketMultiplexer.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/SocketDefs.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

#if defined(__linux__)
    #include <sys/epoll.h>
    #include <sys/eventfd.h>
#endif  // __linux__

//////////////////////////////////////////////////////////////////////////
// areg::SocketMultiplexer — POSIX implementation
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
// Linux: epoll + eventfd
//
// WAKEUP DESIGN:
//   mWakeupReadFd == mWakeupWriteFd — both reference the same eventfd.
//   reset() writes 1 to the eventfd  →  epoll_wait() wakes immediately.
//   wait()  reads the eventfd counter to drain it, returns FailedSocketHandle.
//
// WHY mSockets[] IS TRACKED ALONGSIDE THE KERNEL EPOLL SET:
//   reset() must not close mEpollFd while another thread is inside
//   epoll_wait(mEpollFd, ...).  Closing a watched epoll fd from a
//   different thread is undefined behaviour — the old fd integer may be
//   reused by a new socket before epoll_wait() returns, causing a spurious
//   wake on the wrong descriptor.  Instead, reset() removes every real
//   socket via epoll_ctl(DEL) and then signals the eventfd.
// -----------------------------------------------------------------------

#if defined(__linux__)

areg::SocketMultiplexer::SocketMultiplexer(int32_t maxConnections) noexcept
    : mSockets      { }
    , mMaxCount     { (maxConnections < MIN_CONNECTIONS) ? MIN_CONNECTIONS
                    : (maxConnections > MAX_CONNECTIONS) ? MAX_CONNECTIONS
                    : maxConnections }
    , mIsReset      { false }
    , mEpollFd      { static_cast<SOCKETHANDLE>(::epoll_create1(EPOLL_CLOEXEC)) }
    , mWakeupReadFd { areg::InvalidSocketHandle }
    , mWakeupWriteFd{ areg::InvalidSocketHandle }
{
    mSockets.reserve(16);

    if (mEpollFd == areg::InvalidSocketHandle)
        return;

    const int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd == -1)
        return;

    // On Linux both ends of the wakeup are the same eventfd handle.
    mWakeupReadFd  = static_cast<SOCKETHANDLE>(efd);
    mWakeupWriteFd = static_cast<SOCKETHANDLE>(efd);

    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = efd;
    if (::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_ADD, efd, &ev) != 0)
    {
        ::close(efd);
        mWakeupReadFd  = areg::InvalidSocketHandle;
        mWakeupWriteFd = areg::InvalidSocketHandle;
    }
}

areg::SocketMultiplexer::~SocketMultiplexer() noexcept
{
    // Close the eventfd once (both members reference the same fd).
    if (mWakeupReadFd != areg::InvalidSocketHandle)
    {
        ::close(static_cast<int>(mWakeupReadFd));
        mWakeupReadFd  = areg::InvalidSocketHandle;
        mWakeupWriteFd = areg::InvalidSocketHandle;
    }

    if (mEpollFd != areg::InvalidSocketHandle)
    {
        ::close(static_cast<int>(mEpollFd));
        mEpollFd = areg::InvalidSocketHandle;
    }

    mSockets.clear();
}

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket) noexcept
{
    if (    (mEpollFd == areg::InvalidSocketHandle)
         || !areg::is_valid_socket(hSocket)
         || (hSocket == mWakeupReadFd)
         || (static_cast<int32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    // Reject duplicates.
    for (SOCKETHANDLE s : mSockets)
    {
        if (s == hSocket)
            return false;
    }

    struct epoll_event ev;
    ev.events   = EPOLLIN;
    ev.data.fd  = static_cast<int>(hSocket);
    if (::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_ADD, static_cast<int>(hSocket), &ev) != 0)
        return false;

    // Clear the reset flag so that subsequent wait() calls block normally.
    mIsReset.store(false, std::memory_order_release);

    mSockets.push_back(hSocket);
    return true;
}

bool areg::SocketMultiplexer::unregister_socket(SOCKETHANDLE hSocket) noexcept
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if (*it == hSocket)
        {
            // Remove from the kernel epoll set (epoll_ctl(DEL) ignores the
            // event pointer on Linux 2.6.9+).
            struct epoll_event ev{};
            ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(hSocket), &ev);

            // Swap-erase to keep the vector compact.
            *it = mSockets.back();
            mSockets.pop_back();
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    // Remove every real socket from the kernel set without closing mEpollFd
    // (see design note above).
    for (SOCKETHANDLE s : mSockets)
    {
        struct epoll_event ev{};
        ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(s), &ev);
    }

    mSockets.clear();

    // Mark reset before signaling so that any thread that re-enters wait()
    // after the wakeup eventfd is drained returns FailedSocketHandle immediately.
    mIsReset.store(true, std::memory_order_release);

    // Signal the wakeup eventfd so that any thread currently blocked in
    // epoll_wait() returns immediately.  A non-blocking write is used;
    // failure (counter overflow) is silently ignored — the fd is already
    // readable, so epoll_wait() will wake up regardless.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint64_t one{ 1u };
        (void)::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(one));
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    if (mIsReset.load(std::memory_order_acquire))
        return areg::FailedSocketHandle;

    if (mEpollFd == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    struct epoll_event ev{};
    const int nReady = ::epoll_wait(static_cast<int>(mEpollFd), &ev, 1, timeoutMs);

    if (nReady < 0)
        return (errno == EINTR) ? areg::InvalidSocketHandle : areg::FailedSocketHandle;

    if (nReady == 0)
        return areg::InvalidSocketHandle;   // timeout

    // Wakeup eventfd fired — reset() was called from the shutdown thread.
    // Drain the counter so the fd becomes non-readable again.
    if (ev.data.fd == static_cast<int>(mWakeupReadFd))
    {
        uint64_t val{ 0u };
        (void)::read(static_cast<int>(mWakeupReadFd), &val, sizeof(val));
        return areg::FailedSocketHandle;
    }

    return static_cast<SOCKETHANDLE>(ev.data.fd);
}

// -----------------------------------------------------------------------
// macOS + other POSIX: poll() + anonymous pipe
//
// WAKEUP DESIGN:
//   mWakeupReadFd  = pipe read end  — polled alongside real sockets.
//   mWakeupWriteFd = pipe write end — one byte written by reset().
//   wait() checks the wakeup pollfd last entry, drains the pipe on fire,
//   and returns FailedSocketHandle.
// -----------------------------------------------------------------------

#else   // !__linux__

areg::SocketMultiplexer::SocketMultiplexer(int32_t maxConnections) noexcept
    : mSockets       { }
    , mMaxCount      { (maxConnections < MIN_CONNECTIONS) ? MIN_CONNECTIONS
                     : (maxConnections > MAX_CONNECTIONS) ? MAX_CONNECTIONS
                     : maxConnections }
    , mIsReset       { false }
    , mWakeupReadFd  { areg::InvalidSocketHandle }
    , mWakeupWriteFd { areg::InvalidSocketHandle }
{
    mSockets.reserve(16);

    int pipeFds[2]{ -1, -1 };
    if (::pipe(pipeFds) != 0)
        return;

    // Set both ends non-blocking and close-on-exec.
    for (int fd : pipeFds)
    {
        int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags != -1)
            ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        int fl = ::fcntl(fd, F_GETFD, 0);
        if (fl != -1)
            ::fcntl(fd, F_SETFD, fl | FD_CLOEXEC);
    }

    mWakeupReadFd  = static_cast<SOCKETHANDLE>(pipeFds[0]);
    mWakeupWriteFd = static_cast<SOCKETHANDLE>(pipeFds[1]);
}

areg::SocketMultiplexer::~SocketMultiplexer() noexcept
{
    // Read and write ends are different fds on macOS; close both.
    if (mWakeupReadFd != areg::InvalidSocketHandle)
    {
        ::close(static_cast<int>(mWakeupReadFd));
        mWakeupReadFd = areg::InvalidSocketHandle;
    }

    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        ::close(static_cast<int>(mWakeupWriteFd));
        mWakeupWriteFd = areg::InvalidSocketHandle;
    }

    mSockets.clear();
}

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket) noexcept
{
    if (    !areg::is_valid_socket(hSocket)
         || (hSocket == mWakeupReadFd)
         || (hSocket == mWakeupWriteFd)
         || (static_cast<int32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    // Reject duplicates.
    for (SOCKETHANDLE s : mSockets)
    {
        if (s == hSocket)
            return false;
    }

    // Clear the reset flag so that subsequent wait() calls block normally.
    mIsReset.store(false, std::memory_order_release);

    mSockets.push_back(hSocket);
    return true;
}

bool areg::SocketMultiplexer::unregister_socket(SOCKETHANDLE hSocket) noexcept
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if (*it == hSocket)
        {
            *it = mSockets.back();
            mSockets.pop_back();
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    mSockets.clear();

    // Mark reset before signaling so that any thread that re-enters wait()
    // after the wakeup pipe is drained returns FailedSocketHandle immediately.
    mIsReset.store(true, std::memory_order_release);

    // Wake up any thread blocked in poll() by writing one byte to the pipe.
    // Non-blocking write; EAGAIN means the pipe already has data and poll()
    // will wake up regardless.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const char one{ 1 };
        (void)::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(one));
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    // If reset() was called (and the wakeup was already drained by a previous
    // wait() call), return immediately without entering poll().
    if (mIsReset.load(std::memory_order_acquire))
        return areg::FailedSocketHandle;

    // Always include the wakeup pipe read end (if valid) so reset() can
    // interrupt poll() from the shutdown thread regardless of socket count.
    const int32_t wakeupSlots = (mWakeupReadFd != areg::InvalidSocketHandle) ? 1 : 0;
    const auto    total       = static_cast<int32_t>(mSockets.size()) + wakeupSlots;

    if (total == 0)
        return areg::FailedSocketHandle;

    // Stack-allocated fast path for the common case (small socket count).
    constexpr int32_t    STACK_MAX{ 33 };   // 32 real sockets + 1 wakeup
    struct pollfd        stackFds[STACK_MAX];
    std::vector<struct pollfd> heapFds;
    struct pollfd* const fds = (total <= STACK_MAX)
                             ? stackFds
                             : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    const auto socketCount = static_cast<int32_t>(mSockets.size());
    for (int32_t i = 0; i < socketCount; ++i)
    {
        fds[i].fd      = static_cast<int>(mSockets[static_cast<std::size_t>(i)]);
        fds[i].events  = POLLIN;
        fds[i].revents = 0;
    }

    // Append the wakeup pipe read end as the last entry.
    if (wakeupSlots > 0)
    {
        fds[socketCount].fd      = static_cast<int>(mWakeupReadFd);
        fds[socketCount].events  = POLLIN;
        fds[socketCount].revents = 0;
    }

    const int nReady = ::poll(fds, static_cast<nfds_t>(total), timeoutMs);
    if (nReady < 0)
        return (errno == EINTR) ? areg::InvalidSocketHandle : areg::FailedSocketHandle;

    if (nReady == 0)
        return areg::InvalidSocketHandle;   // timeout

    // Check wakeup pipe first — reset() was called from the shutdown thread.
    // Drain the pipe so it does not fire again on the next poll() call.
    if ((wakeupSlots > 0) && (fds[socketCount].revents & POLLIN))
    {
        char buf[64];
        while (::read(static_cast<int>(mWakeupReadFd), buf, sizeof(buf)) > 0) {}
        return areg::FailedSocketHandle;
    }

    for (int32_t i = 0; i < socketCount; ++i)
    {
        const short rev = fds[i].revents;
        if (rev & (POLLIN | POLLERR | POLLHUP))
            return mSockets[static_cast<std::size_t>(i)];
    }

    return areg::InvalidSocketHandle;
}

#endif  // __linux__

//////////////////////////////////////////////////////////////////////////
// Legacy stateless wait() — all POSIX platforms
//////////////////////////////////////////////////////////////////////////

SOCKETHANDLE areg::SocketMultiplexer::wait( SOCKETHANDLE            serverSocket
                                           , const SOCKETHANDLE *   clientSockets
                                           , int32_t                count
                                           , int32_t                timeoutMs ) const
{
    if (serverSocket == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    const int32_t total = count + 1;

    constexpr int32_t    STACK_MAX{ 32 };
    struct pollfd        stackFds[STACK_MAX];
    std::vector<struct pollfd> heapFds;
    struct pollfd* const fds = (total <= STACK_MAX)
                             ? stackFds
                             : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    fds[0].fd      = static_cast<int>(serverSocket);
    fds[0].events  = POLLIN;
    fds[0].revents = 0;

    for (int32_t i = 0; i < count; ++i)
    {
        fds[static_cast<std::size_t>(i + 1)].fd      = static_cast<int>(clientSockets[i]);
        fds[static_cast<std::size_t>(i + 1)].events  = POLLIN;
        fds[static_cast<std::size_t>(i + 1)].revents = 0;
    }

    const int selected = ::poll(fds, static_cast<nfds_t>(total), timeoutMs);
    if (selected <= 0)
        return selected == 0 ? areg::InvalidSocketHandle : areg::FailedSocketHandle;

    if (fds[0].revents & POLLIN)
        return serverSocket;    // new connection pending

    for (int32_t i = 0; i < count; ++i)
    {
        const short rev = fds[static_cast<std::size_t>(i + 1)].revents;
        if (rev & (POLLIN | POLLERR | POLLHUP))
            return clientSockets[i];
    }

    return areg::InvalidSocketHandle;
}

#endif  // defined(_POSIX) || defined(POSIX)
