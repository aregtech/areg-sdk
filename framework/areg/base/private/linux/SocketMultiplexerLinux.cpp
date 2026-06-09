/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/linux/SocketMultiplexerLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux SocketMultiplexer implementation.
 *              Uses epoll + eventfd for O(1) readiness and wakeup.
 ************************************************************************/
#ifdef __linux__

#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketDefs.hpp"

#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <errno.h>

namespace {
// A single read() resets the accumulated counter to zero (no EFD_SEMAPHORE).
inline void drain_eventfd(int fd) noexcept
{
    uint64_t dummy{ 0u };
    [[maybe_unused]] ssize_t ignored = ::read(fd, &dummy, sizeof(uint64_t));
}
} // namespace

// -----------------------------------------------------------------------
// WAKEUP DESIGN:
//   mWakeupReadFd == mWakeupWriteFd -- both reference the same eventfd.
//   reset() writes 1 to the eventfd  ->  epoll_wait() wakes immediately.
//   wait()  reads the eventfd counter to drain it, returns FailedSocketHandle.
//
// WHY mSockets[] IS TRACKED ALONGSIDE THE KERNEL EPOLL SET:
//   reset() must not close mEpollFd while another thread is inside epoll_wait().
//   Closing a watched epoll fd from a different thread is undefined behavior.
//   Instead, reset() removes every real socket via epoll_ctl(DEL) and then signals the eventfd.
// -----------------------------------------------------------------------

areg::SocketMultiplexer::SocketMultiplexer(uint32_t maxConnections /*= areg::DEFAULT_CONNECTIONS*/) noexcept
    : mSockets      { }
    , mMaxCount     { (maxConnections < MIN_CONNECTIONS) ? MIN_CONNECTIONS : (maxConnections > MAX_CONNECTIONS) ? MAX_CONNECTIONS : maxConnections }
    , mIsReset      { false }
    , mEpollFd      { static_cast<SOCKETHANDLE>(::epoll_create1(EPOLL_CLOEXEC)) }
    , mWakeupReadFd { areg::InvalidSocketHandle }
    , mWakeupWriteFd{ areg::InvalidSocketHandle }
    , mBatchCount   { 0 }
    , mBatchIdx     { 0 }
    , mBatchFds     { }
    , mBatchEvents  { }
{
    mSockets.reserve(DEFAULT_CONNECTIONS);

    if (mEpollFd == areg::InvalidSocketHandle)
        return;

    const int efd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd == -1)
        return;

    // Both ends of the wakeup reference the same eventfd handle.
    mWakeupReadFd  = static_cast<SOCKETHANDLE>(efd);
    mWakeupWriteFd = static_cast<SOCKETHANDLE>(efd);

    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = efd;
    if (::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_ADD, efd, &ev) != areg::RETURNED_OK)
    {
        ::close(efd);
        mWakeupReadFd  = areg::InvalidSocketHandle;
        mWakeupWriteFd = areg::InvalidSocketHandle;
    }
}

areg::SocketMultiplexer::~SocketMultiplexer() noexcept
{
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

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket, bool search) noexcept
{
    if (    !areg::is_valid_socket(hSocket)
         || (mEpollFd == areg::InvalidSocketHandle)
         || (hSocket == mWakeupReadFd)
         || (static_cast<uint32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    if (search && is_registered(hSocket))
        return false;

    struct epoll_event ev;
    // EPOLLIN    -- data available or peer sent FIN (recv returns 0)
    // EPOLLRDHUP -- peer shut down.
    // EPOLLERR / EPOLLHUP - reported automatically by the kernel regardless of the mask.
    ev.events   = EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
    ev.data.fd  = static_cast<int>(hSocket);
    if (::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_ADD, static_cast<int>(hSocket), &ev) != 0)
        return false;

    // Transition out of reset state. Drain here to start new cycle clean.
    if (mIsReset.exchange(false, std::memory_order_acq_rel) && (mWakeupReadFd != areg::InvalidSocketHandle))
    {
        drain_eventfd(static_cast<int>(mWakeupReadFd));
    }

    mSockets.push_back(hSocket);
    return true;
}

bool areg::SocketMultiplexer::unregister_socket(SOCKETHANDLE hSocket) noexcept
{
    for (auto it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if (*it == hSocket)
        {
            struct epoll_event ev{};
            ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(hSocket), &ev);

            *it = mSockets.back();
            mSockets.pop_back();

            // Discard the batch cache.
            mBatchCount = mBatchIdx = 0u;
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    for (SOCKETHANDLE s : mSockets)
    {
        struct epoll_event ev{};
        ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(s), &ev);
    }

    mSockets.clear();
    mBatchCount = mBatchIdx = 0u;
    mIsReset.store(true, std::memory_order_release);

    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        // Signal the wakeup eventfd (8-byte write required by the API).
        const uint64_t one{ 1u };
        [[maybe_unused]] ssize_t written = ::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint64_t));
    }
}

void areg::SocketMultiplexer::wakeup() noexcept
{
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        // Soft interrupt: signal the wakeup eventfd WITHOUT setting mIsReset.
        const uint64_t one{ 1u };
        [[maybe_unused]] ssize_t written = ::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint64_t));
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    if (mIsReset.load(std::memory_order_acquire))
    {
        mBatchCount = mBatchIdx = 0u;
        if (mWakeupReadFd != areg::InvalidSocketHandle)
        {
            drain_eventfd(static_cast<int>(mWakeupReadFd));
        }

        return areg::FailedSocketHandle;
    }

    if (mEpollFd == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    // Serve cached results from the previous epoll_wait batch before issuing another syscall.
    if (mBatchIdx < mBatchCount)
    {
        const SOCKETHANDLE fd   = mBatchFds[mBatchIdx];
        const uint32_t evFlags  = mBatchEvents[mBatchIdx];
        ++mBatchIdx;

        if (fd == mWakeupReadFd)
        {
            drain_eventfd(static_cast<int>(mWakeupReadFd));
            mBatchCount = mBatchIdx = 0u;
            // Hard reset -> FailedSocketHandle; soft wakeup() -> InvalidSocketHandle.
            return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
        }

        // If the socket has error/hangup with NO readable data, remove it from
        // the epoll interest list to prevent busy re-firing.
        if ((evFlags & EPOLLIN) == 0 && (evFlags & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) != 0)
        {
            struct epoll_event ev{};
            ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(fd), &ev);
        }

        return fd;
    }

    struct epoll_event events[BATCH_SIZE];
    const int n = ::epoll_wait(static_cast<int>(mEpollFd), events, BATCH_SIZE, timeoutMs);

    if (n < 0)
        return (errno == EINTR) ? areg::InvalidSocketHandle : areg::FailedSocketHandle;
    else if (n == 0)
        return areg::InvalidSocketHandle;   // timeout

    mBatchCount = mBatchIdx = 0u;
    for (int i = 1; i < n; ++i)
    {
        mBatchFds[mBatchCount]    = static_cast<SOCKETHANDLE>(events[i].data.fd);
        mBatchEvents[mBatchCount] = events[i].events;
        ++mBatchCount; 
    }

    const SOCKETHANDLE first    = static_cast<SOCKETHANDLE>(events[0].data.fd);
    const uint32_t     firstEv  = events[0].events;
    if (first == mWakeupReadFd)
    {
        drain_eventfd(static_cast<int>(mWakeupReadFd));
        mBatchCount = mBatchIdx = 0u;
        // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
        return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
    }

    // Same error-only handling for the first event returned directly.
    if ((firstEv & EPOLLIN) == 0 && (firstEv & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) != 0)
    {
        struct epoll_event ev{};
        ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(first), &ev);
    }

    return first;
}

#endif  // __linux__
