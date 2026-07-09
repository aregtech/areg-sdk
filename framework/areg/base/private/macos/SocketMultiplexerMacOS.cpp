/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/macos/SocketMultiplexerMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, SocketMultiplexer macOS implementation.
 *              Uses kqueue (O(1)) + anonymous pipe for wakeup.
 *              Linux uses epoll + eventfd -- see linux/SocketMultiplexerLinux.cpp.
 *              Other POSIX uses poll + pipe  -- see posix/SocketMultiplexerPosix.cpp.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketDefs.hpp"

#include <unistd.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

namespace {
// Drain all buffered bytes from the wakeup pipe so it does not re-fire.
inline void drain_pipe(int fd) noexcept
{
    char buf[64];
    while (::read(fd, buf, sizeof(buf)) > 0) {}
}
} // namespace

//////////////////////////////////////////////////////////////////////////
// macOS: constructor, destructor, and wait() using kqueue + anonymous pipe
//////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
// WAKEUP DESIGN:
//   mKqueueFd      = kqueue instance fd -- monitors sockets + pipe read end.
//   mWakeupReadFd  = pipe read end  -- added to kqueue as EVFILT_READ.
//   mWakeupWriteFd = pipe write end -- one byte written by reset().
//   wait() checks result.ident against mWakeupReadFd first, drains the
//   pipe on match, and returns FailedSocketHandle.
// -----------------------------------------------------------------------

areg::SocketMultiplexer::SocketMultiplexer(uint32_t maxConnections /*= areg::DEFAULT_CONNECTIONS*/) noexcept
    : mSockets      { }
    , mMaxCount     { (maxConnections < MIN_CONNECTIONS) ? MIN_CONNECTIONS : (maxConnections > MAX_CONNECTIONS) ? MAX_CONNECTIONS : maxConnections }
    , mIsReset      { false }
    , mKqueueFd     { areg::InvalidSocketHandle }
    , mWakeupReadFd { areg::InvalidSocketHandle }
    , mWakeupWriteFd{ areg::InvalidSocketHandle }
    , mBatchCount   { 0 }
    , mBatchIdx     { 0 }
    , mBatchFds     { }
    , mBatchEvents  { }
{
    mSockets.reserve(DEFAULT_CONNECTIONS);

    const int kq = ::kqueue();
    if (kq == -1)
        return;

    int pipeFds[2]{ -1, -1 };
    if (::pipe(pipeFds) != 0)
    {
        ::close(kq);
        return;
    }

    for (int fd : pipeFds)
    {
        int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags != -1)
            ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        int fl = ::fcntl(fd, F_GETFD, 0);
        if (fl != -1)
            ::fcntl(fd, F_SETFD, fl | FD_CLOEXEC);
    }

    // Register the pipe read end in kqueue so wait() wakes when reset() writes.
    struct kevent kev;
    EV_SET(&kev, static_cast<uintptr_t>(pipeFds[0]), EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, nullptr);
    if (::kevent(kq, &kev, 1, nullptr, 0, nullptr) != 0)
    {
        ::close(pipeFds[0]);
        ::close(pipeFds[1]);
        ::close(kq);
        return;
    }

    mKqueueFd      = static_cast<SOCKETHANDLE>(kq);
    mWakeupReadFd  = static_cast<SOCKETHANDLE>(pipeFds[0]);
    mWakeupWriteFd = static_cast<SOCKETHANDLE>(pipeFds[1]);
}

areg::SocketMultiplexer::~SocketMultiplexer() noexcept
{
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

    if (mKqueueFd != areg::InvalidSocketHandle)
    {
        ::close(static_cast<int>(mKqueueFd));
        mKqueueFd = areg::InvalidSocketHandle;
    }

    mSockets.clear();
}

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket, bool search) noexcept
{
    if (    !areg::is_valid_socket(hSocket)
         || (mKqueueFd == areg::InvalidSocketHandle)
         || (hSocket == mWakeupWriteFd)
         || (hSocket == mWakeupReadFd)
         || (static_cast<uint32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    if (search && is_registered(hSocket))
        return false;

    // kevent() keeps firing as long as data remains in the socket receive buffer.
    // Read one message per kevent() wakeup. It guarantees that a second message in the same TCP burst is delivered.
    struct kevent kev;
    EV_SET(&kev, static_cast<uintptr_t>(static_cast<int>(hSocket)), EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (::kevent(static_cast<int>(mKqueueFd), &kev, 1, nullptr, 0, nullptr) != 0)
        return false;

    if (mIsReset.exchange(false, std::memory_order_acq_rel) && (mWakeupReadFd != areg::InvalidSocketHandle))
    {
        drain_pipe(static_cast<int>(mWakeupReadFd));
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
            if (mKqueueFd != areg::InvalidSocketHandle)
            {
                struct kevent kev;
                EV_SET(&kev, static_cast<uintptr_t>(static_cast<int>(hSocket)), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
                ::kevent(static_cast<int>(mKqueueFd), &kev, 1, nullptr, 0, nullptr);
            }

            *it = mSockets.back();
            mSockets.pop_back();
            mBatchCount = mBatchIdx = 0u;
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    if (mKqueueFd != areg::InvalidSocketHandle)
    {
        for (SOCKETHANDLE s : mSockets)
        {
            struct kevent kev;
            EV_SET(&kev, static_cast<uintptr_t>(static_cast<int>(s)), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
            ::kevent(static_cast<int>(mKqueueFd), &kev, 1, nullptr, 0, nullptr);
        }
    }

    mSockets.clear();
    mBatchCount = mBatchIdx = 0u;
    mIsReset.store(true, std::memory_order_release);

    // Wake up any thread blocked in kevent() by writing one byte to the pipe.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint32_t one{ 1u };
        static_cast<void>(::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint32_t)));
    }
}

void areg::SocketMultiplexer::wakeup() noexcept
{
    // Soft interrupt: write one byte to the pipe WITHOUT setting mIsReset.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint8_t one{ 1u };
        static_cast<void>(::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint8_t)));
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    if (mIsReset.load(std::memory_order_acquire))
    {
        mBatchCount = mBatchIdx = 0u;
        if (mWakeupReadFd != areg::InvalidSocketHandle)
        {
            drain_pipe(static_cast<int>(mWakeupReadFd));
        }

        return areg::FailedSocketHandle;
    }

    if (mKqueueFd == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    // Serve cached results before issuing another kevent() syscall.
    if (mBatchIdx < mBatchCount)
    {
        const SOCKETHANDLE fd   = mBatchFds[mBatchIdx];
        const uint32_t evFlags  = mBatchEvents[mBatchIdx];
        ++mBatchIdx;

        if (fd == mWakeupReadFd)
        {
            drain_pipe(static_cast<int>(mWakeupReadFd));
            mBatchCount = mBatchIdx = 0u;
            // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
            return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
        }

        // If kqueue reports EV_EOF (peer disconnected) with no data available,
        // remove from kqueue to prevent busy re-firing.
        if ((evFlags & EV_EOF) != 0 && (evFlags & EV_ERROR) == 0)
        {
            struct kevent kev;
            EV_SET(&kev, static_cast<uintptr_t>(static_cast<int>(fd)), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
            ::kevent(static_cast<int>(mKqueueFd), &kev, 1, nullptr, 0, nullptr);
        }

        return fd;
    }

    // Fetch a new batch, up to DEFAULT_DRAIN_LIMIT events in one kevent() syscall.
    struct kevent events[DEFAULT_DRAIN_LIMIT];
    struct timespec ts;
    const struct timespec * pTs = nullptr;

    if (timeoutMs >= 0)
    {
        ts.tv_sec  = static_cast<time_t>(timeoutMs / 1000);
        ts.tv_nsec = static_cast<long>((timeoutMs % 1000) * 1000000L);
        pTs = &ts;
    }

    const int n = ::kevent(static_cast<int>(mKqueueFd), nullptr, 0, events, DEFAULT_DRAIN_LIMIT, pTs);

    if (n < 0)
        return (errno == EINTR) ? areg::InvalidSocketHandle : areg::FailedSocketHandle;
    else if (n == 0)
        return areg::InvalidSocketHandle;   // timeout

    mBatchCount = mBatchIdx = 0u;
    for (int i = 1; i < n; ++i)
    {
        if ((events[i].flags & EV_ERROR) == 0)
        {
            mBatchFds[mBatchCount]    = static_cast<SOCKETHANDLE>(events[i].ident);
            mBatchEvents[mBatchCount] = events[i].flags;
            ++mBatchCount;
        }
    }

    if ((events[0].flags & EV_ERROR) != 0)
        return areg::FailedSocketHandle;

    const SOCKETHANDLE first    = static_cast<SOCKETHANDLE>(events[0].ident);
    const uint32_t     firstEv  = events[0].flags;
    if (first == mWakeupReadFd)
    {
        drain_pipe(static_cast<int>(mWakeupReadFd));
        mBatchCount = mBatchIdx = 0u;
        // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
        return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
    }

    // Same EV_EOF handling for the first event.
    if ((firstEv & EV_EOF) != 0 && (firstEv & EV_ERROR) == 0)
    {
        struct kevent kev;
        EV_SET(&kev, static_cast<uintptr_t>(static_cast<int>(first)), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        ::kevent(static_cast<int>(mKqueueFd), &kev, 1, nullptr, 0, nullptr);
    }

    return first;
}

#endif  // __APPLE__
