/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SocketMultiplexerWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. SocketMultiplexer Windows implementation.
 *              Uses WSAPoll for readiness monitoring and a loopback TCP
 *              socket pair as the wakeup mechanism.
 ************************************************************************/

#ifdef _WIN32

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketDefs.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

//////////////////////////////////////////////////////////////////////////
// areg::SocketMultiplexer -- Windows (WSAPoll + loopback socket pair)
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * WAKEUP DESIGN -- loopback TCP socket pair:
 *
 *  mWakeupReadFd  = accepted side  -- added to the WSAPoll descriptor list.
 *   mWakeupWriteFd = connected side -- send() writes one byte in reset().
 *
 *   Construction creates a temporary listener on 127.0.0.1:0, connects
 *   to it, accepts one connection, then closes the listener.  The result
 *   is a connected socket pair where mWakeupWriteFd can signal
 *   mWakeupReadFd purely via loopback TCP.  The pair is created eagerly
 *   in the constructor so that wait() blocks correctly even before any
 *   real sockets are registered.
 *
 *   This mirrors the macOS pipe approach: the same members (mWakeupReadFd,
 *   mWakeupWriteFd) and the same wait() / reset() logic apply on both
 *   platforms.  Only the OS primitive that implements the pair differs.
 *
 *   Winsock must be initialized (WSAStartup called) before constructing
 *   a SocketMultiplexer.  In areg, socket_initialize() satisfies this
 *   requirement before any ServerConnectionBase is created.
 ************************************************************************/

namespace {

/**
 * \brief   Creates a connected loopback TCP socket pair.
 *          On success, sets \a readEnd and \a writeEnd to the two sockets
 *          and returns true.  On any failure, both are set to
 *          areg::InvalidSocketHandle and false is returned.
 **/
static bool _create_wakeup_pair(SOCKETHANDLE& readEnd, SOCKETHANDLE& writeEnd) noexcept
{
    readEnd  = areg::InvalidSocketHandle;
    writeEnd = areg::InvalidSocketHandle;

    // Step 1: temporary listener on 127.0.0.1:0
    const SOCKET listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET)
        return false;

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK);
    addr.sin_port        = 0;   // OS picks a free port

    if (::bind(listener, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0 ||
        ::listen(listener, 1) != 0)
    {
        ::closesocket(listener);
        return false;
    }

    // Step 2: learn the bound port
    int addrLen = static_cast<int>(sizeof(addr));
    if (::getsockname(listener, reinterpret_cast<sockaddr*>(&addr), &addrLen) != 0)
    {
        ::closesocket(listener);
        return false;
    }

    // Step 3: connect side (will become mWakeupWriteFd)
    const SOCKET connectSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSock == INVALID_SOCKET)
    {
        ::closesocket(listener);
        return false;
    }

    if (::connect(connectSock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        ::closesocket(connectSock);
        ::closesocket(listener);
        return false;
    }

    // Step 4: accept side (will become mWakeupReadFd)
    const SOCKET acceptSock = ::accept(listener, nullptr, nullptr);
    ::closesocket(listener);   // listener is no longer needed

    if (acceptSock == INVALID_SOCKET)
    {
        ::closesocket(connectSock);
        return false;
    }

    u_long mode{ 1 };
    ::ioctlsocket(acceptSock,  static_cast<long>(FIONBIO), &mode);
    ::ioctlsocket(connectSock, static_cast<long>(FIONBIO), &mode);

    readEnd  = static_cast<SOCKETHANDLE>(acceptSock);
    writeEnd = static_cast<SOCKETHANDLE>(connectSock);
    return true;
}

// Drain all buffered bytes from the wakeup socket so it does not re-fire.
inline void drain_wakeup(SOCKET fd) noexcept
{
    char buf[64];
    while (::recv(fd, buf, static_cast<int>(sizeof(buf)), 0) > 0) {}
}

} // namespace

areg::SocketMultiplexer::SocketMultiplexer(uint32_t maxConnections) noexcept
    : mSockets      { }
    , mMaxCount     { (maxConnections < areg::MIN_CONNECTIONS) ? areg::MIN_CONNECTIONS : (maxConnections > areg::MAX_CONNECTIONS) ? areg::MAX_CONNECTIONS : maxConnections }
    , mIsReset      { false }
    , mWakeupReadFd { areg::InvalidSocketHandle }
    , mWakeupWriteFd{ areg::InvalidSocketHandle }
    , mBatchCount   { 0 }
    , mBatchIdx     { 0 }
    , mBatchFds     { }
    , mBatchEvents  { }
{
    mSockets.reserve(areg::DEFAULT_CONNECTIONS);
    // Create the wakeup pair before any real sockets are registered.
    _create_wakeup_pair(mWakeupReadFd, mWakeupWriteFd);
}

areg::SocketMultiplexer::~SocketMultiplexer() noexcept
{
    // Read and write ends are different sockets on Windows; close both.
    if (mWakeupReadFd != areg::InvalidSocketHandle)
    {
        ::closesocket(static_cast<SOCKET>(mWakeupReadFd));
        mWakeupReadFd = areg::InvalidSocketHandle;
    }

    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        ::closesocket(static_cast<SOCKET>(mWakeupWriteFd));
        mWakeupWriteFd = areg::InvalidSocketHandle;
    }

    mSockets.clear();
}

bool areg::SocketMultiplexer::register_socket(SOCKETHANDLE hSocket, bool search) noexcept
{
    if (    (hSocket == areg::InvalidSocketHandle)
         || (hSocket == mWakeupReadFd)
         || (hSocket == mWakeupWriteFd)
         || (static_cast<int32_t>(mSockets.size()) >= mMaxCount) )
    {
        return false;
    }

    // Reject duplicates.
    if (search && is_registered(hSocket))
    {
        return false;
    }

    if (mIsReset.exchange(false, std::memory_order_acq_rel) && (mWakeupReadFd != areg::InvalidSocketHandle))
    {
        drain_wakeup(static_cast<SOCKET>(mWakeupReadFd));
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
            mBatchCount = mBatchIdx = 0u;
            return true;
        }
    }

    return false;
}

void areg::SocketMultiplexer::reset() noexcept
{
    mSockets.clear();
    mBatchCount = mBatchIdx = 0u;
    mIsReset.store(true, std::memory_order_release);
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const char one{ 1 };
        (void)::send(static_cast<SOCKET>(mWakeupWriteFd), &one, 1, 0);
    }
}

void areg::SocketMultiplexer::wakeup() noexcept
{
    // Soft interrupt.
    if (mWakeupWriteFd != areg::InvalidSocketHandle)
    {
        const char one{ 1 };
        (void)::send(static_cast<SOCKET>(mWakeupWriteFd), &one, 1, 0);
    }
}

SOCKETHANDLE areg::SocketMultiplexer::wait(int32_t timeoutMs) const noexcept
{
    if (mIsReset.load(std::memory_order_acquire))
    {
        mBatchCount = mBatchIdx = 0u;
        if (mWakeupReadFd != areg::InvalidSocketHandle)
        {
            drain_wakeup(static_cast<SOCKET>(mWakeupReadFd));
        }

        return areg::FailedSocketHandle;
    }

    // Serve cached results from the previous WSAPoll batch before issuing another syscall.
    if (mBatchIdx < mBatchCount)
    {
        const SOCKETHANDLE fd = mBatchFds[mBatchIdx++];
        if (fd == mWakeupReadFd)
        {
            drain_wakeup(static_cast<SOCKET>(mWakeupReadFd));
            mBatchCount = mBatchIdx = 0u;
            // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
            return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
        }

        return fd;
    }

    // Always include the wakeup socket if valid to interrupt WSAPoll.
    const int32_t wakeupSlots = (mWakeupReadFd != areg::InvalidSocketHandle) ? 1 : 0;
    const auto    total       = static_cast<INT>(mSockets.size()) + wakeupSlots;

    if (total == 0)
        return areg::FailedSocketHandle;

    WSAPOLLFD     stackFds[areg::DEFAULT_CONNECTIONS];
    std::vector<WSAPOLLFD> heapFds;
    WSAPOLLFD* const fds = (total <= areg::DEFAULT_CONNECTIONS) ? stackFds : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    const auto socketCount = static_cast<INT>(mSockets.size());
    for (INT i = 0; i < socketCount; ++i)
    {
        fds[i].fd      = mSockets[static_cast<std::size_t>(i)];
        fds[i].events  = POLLRDNORM;
        fds[i].revents = 0;
    }

    // Append the wakeup socket read end as the last entry.
    if (wakeupSlots > 0)
    {
        fds[socketCount].fd      = mWakeupReadFd;
        fds[socketCount].events  = POLLRDNORM;
        fds[socketCount].revents = 0;
    }

    const int nReady = ::WSAPoll(fds, static_cast<ULONG>(total), timeoutMs);
    if (nReady <= 0)
    {
        return nReady == 0 ? areg::InvalidSocketHandle : areg::FailedSocketHandle;
    }

    // Drain received bytes so the socket does not fire again on the next call.
    if ((wakeupSlots > 0) && (fds[socketCount].revents & (POLLRDNORM | POLLHUP | POLLERR)))
    {
        drain_wakeup(static_cast<SOCKET>(mWakeupReadFd));
        mBatchCount = mBatchIdx = 0u;
        // Hard reset --> FailedSocketHandle; soft wakeup() --> InvalidSocketHandle.
        return mIsReset.load(std::memory_order_acquire) ? areg::FailedSocketHandle : areg::InvalidSocketHandle;
    }

    // Collect ALL ready sockets from this WSAPoll result into the batch cache.
    mBatchCount = mBatchIdx = 0u;
    SOCKETHANDLE first{ areg::InvalidSocketHandle };
    for (INT i = 0; i < socketCount; ++i)
    {
        const SHORT rev = fds[i].revents;
        if (rev & (POLLRDNORM | POLLERR | POLLHUP))
        {
            if (first == areg::InvalidSocketHandle)
            {
                first = mSockets[static_cast<std::size_t>(i)];
            }
            else if (mBatchCount < areg::BATCH_SIZE)
            {
                mBatchFds[mBatchCount]    = mSockets[static_cast<std::size_t>(i)];
                mBatchEvents[mBatchCount] = static_cast<uint32_t>(rev);
                ++mBatchCount;
            }
        }
    }

    return first;
}

//////////////////////////////////////////////////////////////////////////
// Legacy stateless wait() -- Windows
//////////////////////////////////////////////////////////////////////////

SOCKETHANDLE areg::SocketMultiplexer::wait( SOCKETHANDLE            serverSocket
                                           , const SOCKETHANDLE *   clientSockets
                                           , int32_t                count
                                           , int32_t                timeoutMs ) const
{
    if (serverSocket == areg::InvalidSocketHandle)
        return areg::FailedSocketHandle;

    const INT total = static_cast<INT>(count + 1);

    WSAPOLLFD     stackFds[areg::DEFAULT_CONNECTIONS];
    std::vector<WSAPOLLFD> heapFds;
    WSAPOLLFD* const fds = (total <= areg::DEFAULT_CONNECTIONS) ? stackFds : (heapFds.resize(static_cast<std::size_t>(total)), heapFds.data());

    fds[0].fd      = serverSocket;
    fds[0].events  = POLLRDNORM;
    fds[0].revents = 0;

    for (int32_t i = 0; i < count; ++i)
    {
        fds[static_cast<std::size_t>(i + 1)].fd      = clientSockets[i];
        fds[static_cast<std::size_t>(i + 1)].events  = POLLRDNORM;
        fds[static_cast<std::size_t>(i + 1)].revents = 0;
    }

    const int selected = ::WSAPoll(fds, static_cast<ULONG>(total), timeoutMs);
    if (selected <= 0)
        return selected == 0 ? areg::InvalidSocketHandle : areg::FailedSocketHandle;

    if (fds[0].revents & POLLRDNORM)
        return serverSocket;    // new connection pending

    for (int32_t i = 0; i < count; ++i)
    {
        const SHORT rev = fds[static_cast<std::size_t>(i + 1)].revents;
        if (rev & (POLLRDNORM | POLLERR | POLLHUP))
            return clientSockets[i];
    }

    return areg::InvalidSocketHandle;
}

#endif  // _WIN32
