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
 * \brief       Areg Platform, SocketMultiplexer generic POSIX implementation.
 *              Uses poll() + anonymous pipe for non-Linux, non-Apple POSIX platforms
 *              (Cygwin, FreeBSD, etc.).
 *              Linux uses epoll + eventfd  — see linux/SocketMultiplexerLinux.cpp.
 *              macOS uses kqueue + pipe    — see macos/SocketMultiplexerMacOS.cpp.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketDefs.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Generic POSIX: constructor, destructor, and wait()
// using poll() + anonymous pipe (Cygwin, FreeBSD, etc.)
// Excludes Linux (epoll) and macOS (kqueue) — see their respective files.
//////////////////////////////////////////////////////////////////////////

#if !defined(__linux__) && !defined(__APPLE__)

namespace {
// Drain all buffered bytes from the wakeup pipe so it does not re-fire.
inline void drain_pipe(int fd) noexcept
{
    char buf[64];
    while (::read(fd, buf, sizeof(buf)) > 0) {}
}
} // namespace

// -----------------------------------------------------------------------
// WAKEUP DESIGN:
//   mWakeupReadFd  = pipe read end  — polled alongside real sockets.
//   mWakeupWriteFd = pipe write end — one byte written by reset().
//   wait() checks the wakeup entry last, drains the pipe on fire,
//   and returns FailedSocketHandle.
// -----------------------------------------------------------------------

areg::SocketMultiplexer::SocketMultiplexer(uint32_t maxConnections /*= areg::DEFAULT_CONNECTIONS*/) noexcept
    : mSockets      { }
    , mMaxCount     { (maxConnections < areg::MIN_CONNECTIONS) ? areg::MIN_CONNECTIONS : (maxConnections > areg::MAX_CONNECTIONS) ? areg::MAX_CONNECTIONS : maxConnections }
    , mIsReset      { false }
#if defined(__linux__)
    , mEpollFd      { areg::InvalidSocketHandle }
#elif defined(__APPLE__)
    , mKqueueFd     { areg::InvalidSocketHandle }
#endif
    , mWakeupReadFd { areg::InvalidSocketHandle }
    , mWakeupWriteFd{ areg::InvalidSocketHandle }
    , mBatchCount   { 0 }
    , mBatchIdx     { 0 }
    , mBatchFds     { }
    , mBatchEvents  { }
{
    mSockets.reserve(areg::DEFAULT_CONNECTIONS);

    int pipeFds[2]{ -1, -1 };
    if (::pipe(pipeFds) != 0)
        return;

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

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket, bool search) noexcept
{
    if (    !areg::is_valid_socket(hSocket)
         || (hSocket == mWakeupWriteFd)
         || (hSocket == mWakeupReadFd)
         || (static_cast<uint32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    if (search && is_registered(hSocket))
        return false;

    // Transition out of reset state.  Drain any pending wakeup write.
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
            *it = mSockets.back();
            mSockets.pop_back();

            // Discard the batch cache: it may contain hSocket or a socket that
            // was ready alongside hSocket.  The next wait() will fetch a fresh batch.
            mBatchCount = mBatchIdx = 0;
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    mSockets.clear();
    mBatchCount = 0;
    mBatchIdx   = 0;
    mIsReset.store(true, std::memory_order_release);

    // Wake up any thread blocked in poll() by writing one byte to the pipe.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint32_t one{ 1u };
        static_cast<void>(::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint32_t)));
    }
}

void areg::SocketMultiplexer::wakeup() noexcept
{
    // Soft interrupt: write one byte to the pipe WITHOUT setting mIsReset.
    // wait() will drain the pipe and return InvalidSocketHandle, letting
    // the receive thread re-check pending socket registrations and re-enter wait().
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const uint32_t one{ 1u };
        static_cast<void>(::write(static_cast<int>(mWakeupWriteFd), &one, sizeof(uint32_t)));
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    if (mIsReset.load(std::memory_order_acquire))
    {
        mBatchCount = mBatchIdx = 0;
        if (mWakeupReadFd != areg::InvalidSocketHandle)
        {
            drain_pipe(static_cast<int>(mWakeupReadFd));
        }

        return areg::FailedSocketHandle;
    }

    // Serve cached results from the previous poll() batch before issuing another syscall.
    if (mBatchIdx < mBatchCount)
    {
        const SOCKETHANDLE fd = mBatchFds[mBatchIdx++];
        if (fd == mWakeupReadFd)
        {
            drain_pipe(static_cast<int>(mWakeupReadFd));
            mBatchCount = mBatchIdx = 0;
            // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
            return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
        }

        return fd;
    }

    const uint32_t wakeupSlots = (mWakeupReadFd != areg::InvalidSocketHandle) ? 1 : 0;
    const uint32_t total       = static_cast<uint32_t>(mSockets.size()) + wakeupSlots;

    if (total == 0u)
        return areg::FailedSocketHandle;

    struct pollfd  stackFds[areg::DEFAULT_CONNECTIONS];
    std::vector<struct pollfd> heapFds;
    struct pollfd* const fds = (total <= areg::DEFAULT_CONNECTIONS) ? stackFds : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    const uint32 socketCount = static_cast<uint32_t>(mSockets.size());
    for (uint32_t i = 0; i < socketCount; ++i)
    {
        fds[i].fd      = static_cast<int>(mSockets[static_cast<std::size_t>(i)]);
        fds[i].events  = POLLIN;
        fds[i].revents = 0;
    }

    if (wakeupSlots > 0)
    {
        fds[socketCount].fd      = static_cast<int>(mWakeupReadFd);
        fds[socketCount].events  = POLLIN;
        fds[socketCount].revents = 0;
    }

    const int nReady = ::poll(fds, static_cast<nfds_t>(total), timeoutMs);
    if (nReady < 0)
        return (errno == EINTR) ? areg::InvalidSocketHandle : areg::FailedSocketHandle;
    else if (nReady == 0)
        return areg::InvalidSocketHandle;   // timeout

    if ((wakeupSlots > 0) && (fds[socketCount].revents & POLLIN))
    {
        drain_pipe(static_cast<int>(mWakeupReadFd));
        mBatchCount = mBatchIdx = 0;
        // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
        return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
    }

    // Collect ALL ready sockets into the batch cache; return the first immediately.
    mBatchCount = 0u;
    SOCKETHANDLE first{ areg::InvalidSocketHandle };
    for (uint32_t i = 0; i < socketCount; ++i)
    {
        const short rev = fds[i].revents;
        if (rev & (POLLIN | POLLERR | POLLHUP))
        {
            if (first == areg::InvalidSocketHandle)
            {
                first   = mSockets[static_cast<std::size_t>(i)];
            }
            else if (mBatchCount < areg::BATCH_SIZE)
            {
                mBatchFds[mBatchCount]    = mSockets[static_cast<std::size_t>(i)];
                mBatchEvents[mBatchCount] = static_cast<uint32_t>(rev);
                ++mBatchCount;
            }
        }
    }

    mBatchIdx = 0u;
    return first;
}

#endif  // !defined(__linux__) && !defined(__APPLE__)

//////////////////////////////////////////////////////////////////////////
// Common POSIX: legacy stateless wait() — available on all POSIX platforms
//////////////////////////////////////////////////////////////////////////

SOCKETHANDLE areg::SocketMultiplexer::wait( SOCKETHANDLE            serverSocket
                                           , const SOCKETHANDLE *   clientSockets
                                           , int32_t                count
                                           , int32_t                timeoutMs ) const
{
    if (serverSocket == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    const uint32_t total = static_cast<uint32_t>(count) + 1u;

    struct pollfd        stackFds[areg::DEFAULT_CONNECTIONS];
    std::vector<struct pollfd> heapFds;
    struct pollfd* const fds = (total <= areg::DEFAULT_CONNECTIONS) ? stackFds : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    fds[0].fd      = static_cast<int>(serverSocket);
    fds[0].events  = POLLIN;
    fds[0].revents = 0;

    for (uint32_t i = 0; i < count; ++i)
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

    for (uint32_t i = 0; i < count; ++i)
    {
        const short rev = fds[static_cast<std::size_t>(i + 1)].revents;
        if (rev & (POLLIN | POLLERR | POLLHUP))
            return clientSockets[i];
    }

    return areg::InvalidSocketHandle;
}

#endif  // defined(_POSIX) || defined(POSIX)
