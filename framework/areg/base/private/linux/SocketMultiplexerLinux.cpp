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

// -----------------------------------------------------------------------
// WAKEUP DESIGN:
//   mWakeupReadFd == mWakeupWriteFd — both reference the same eventfd.
//   reset() writes 1 to the eventfd  ->  epoll_wait() wakes immediately.
//   wait()  reads the eventfd counter to drain it, returns FailedSocketHandle.
//
// WHY mSockets[] IS TRACKED ALONGSIDE THE KERNEL EPOLL SET:
//   reset() must not close mEpollFd while another thread is inside
//   epoll_wait(). Closing a watched epoll fd from a different thread is
//   undefined behaviour. Instead, reset() removes every real socket via
//   epoll_ctl(DEL) and then signals the eventfd.
// -----------------------------------------------------------------------

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

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket, bool search) noexcept
{
    if (    !areg::is_valid_socket(hSocket)
         || (mEpollFd == areg::InvalidSocketHandle)
         || (hSocket == mWakeupReadFd)
         || (static_cast<int32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    if (search && is_registered(hSocket))
        return false;

    struct epoll_event ev;
    ev.events   = EPOLLIN;
    ev.data.fd  = static_cast<int>(hSocket);
    if (::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_ADD, static_cast<int>(hSocket), &ev) != 0)
        return false;

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
            struct epoll_event ev{};
            ::epoll_ctl(static_cast<int>(mEpollFd), EPOLL_CTL_DEL, static_cast<int>(hSocket), &ev);

            *it = mSockets.back();
            mSockets.pop_back();
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
    mIsReset.store(true, std::memory_order_release);

    // Signal the wakeup eventfd (8-byte write required by the API).
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint64_t one{ 1u };
        (void)::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint64_t));
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

    // Wakeup eventfd fired — drain the counter so the fd becomes non-readable again.
    if (ev.data.fd == static_cast<int>(mWakeupReadFd))
    {
        uint64_t counter{ 0u };
        (void)::read(static_cast<int>(mWakeupReadFd), &counter, sizeof(uint64_t));
        return areg::FailedSocketHandle;
    }

    return static_cast<SOCKETHANDLE>(ev.data.fd);
}

#endif  // __linux__
