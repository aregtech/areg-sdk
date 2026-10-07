/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/posix/SendBacklogPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the writer of a service send thread that never waits for one
 *              socket. POSIX part: a write completes when it returns, and the send thread
 *              waits in poll() for its sockets and its wake-up descriptor.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/private/SendBacklog.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <new>

#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#if defined(__linux__)
    #include <sys/eventfd.h>
#endif  // defined(__linux__)

namespace areg::ext {

struct SendBacklogOs
{
};

void SendBacklogOsDelete::operator () (SendBacklogOs * os) const noexcept
{
    delete os;
}

struct SendBacklogWaitOs
{
    int                         readFd  { -1 }; //!< Readable while the wait is signalled.
    int                         writeFd { -1 }; //!< Written to signal; the same as readFd for an eventfd.
    std::vector<struct pollfd>  fds     { };    //!< The descriptors of one wait, reused.
};

void SendBacklogWaitOsDelete::operator () (SendBacklogWaitOs * wait) const noexcept
{
    if (wait == nullptr)
        return;

    if (wait->writeFd != wait->readFd)
    {
        static_cast<void>(::close(wait->writeFd));
    }

    static_cast<void>(::close(wait->readFd));
    delete wait;
}

int64_t backlog_os_start( SendBacklogOsPtr & /*os*/
                        , SOCKETHANDLE hSocket
                        , const areg::IoBuffer * buffers
                        , uint32_t count
                        , uint64_t /*maxInFlight*/
                        , uint64_t & started )
{
    ASSERT((buffers != nullptr) && (count != 0u) && (count <= areg::DEFAULT_DRAIN_LIMIT));

#if defined(MSG_NOSIGNAL)
    constexpr int sendFlags{ MSG_NOSIGNAL | MSG_DONTWAIT };
#else   // defined(MSG_NOSIGNAL)
    constexpr int sendFlags{ MSG_DONTWAIT };
#endif  // defined(MSG_NOSIGNAL)

    struct iovec iov[areg::DEFAULT_DRAIN_LIMIT];
    for (uint32_t i = 0u; i < count; ++i)
    {
        iov[i].iov_base = const_cast<uint8_t *>(buffers[i].data);
        iov[i].iov_len  = buffers[i].size;
    }

    struct msghdr msg { };
    msg.msg_iov     = iov;
    msg.msg_iovlen  = count;

    ssize_t written{ 0 };
    do
    {
        written = ::sendmsg(static_cast<int>(hSocket), &msg, sendFlags);
    } while ((written < 0) && (errno == EINTR));

    if (written < 0)
    {
        started = 0u;
        return (((errno == EAGAIN) || (errno == EWOULDBLOCK)) ? 0 : -1);
    }

    started = static_cast<uint64_t>(written);
    return static_cast<int64_t>(written);
}

int64_t backlog_os_collect(SendBacklogOs * /*os*/, SOCKETHANDLE /*hSocket*/)
{
    return 0;
}

uint64_t backlog_os_in_flight(const SendBacklogOs * /*os*/) noexcept
{
    return 0u;
}

void backlog_os_cancel(SendBacklogOs * /*os*/, SOCKETHANDLE /*hSocket*/) noexcept
{
}

bool backlog_os_done(const SendBacklogOs * /*os*/) noexcept
{
    return true;
}

SendBacklogWaitOs * backlog_os_wait_create() noexcept
{
    SendBacklogWaitOs * wait{ new (std::nothrow) SendBacklogWaitOs() };
    if (wait == nullptr)
        return nullptr;

#if defined(__linux__)
    wait->readFd  = ::eventfd(0u, EFD_NONBLOCK | EFD_CLOEXEC);
    wait->writeFd = wait->readFd;
    if (wait->readFd >= 0)
        return wait;
#else   // defined(__linux__)
    int fds[2]{ -1, -1 };
    if (::pipe(fds) == 0)
    {
        for (const int fd : fds)
        {
            static_cast<void>(::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK));
            static_cast<void>(::fcntl(fd, F_SETFD, FD_CLOEXEC));
        }

        wait->readFd  = fds[0];
        wait->writeFd = fds[1];
        return wait;
    }
#endif  // defined(__linux__)

    delete wait;
    return nullptr;
}

bool backlog_os_attach(SendBacklogWaitOs * /*wait*/, SOCKETHANDLE /*hSocket*/) noexcept
{
    return true;
}

void backlog_os_signal(SendBacklogWaitOs * wait) noexcept
{
#if defined(__linux__)
    const uint64_t one{ 1u };
#else   // defined(__linux__)
    const uint8_t one{ 1u };
#endif  // defined(__linux__)

    ssize_t result{ 0 };
    do
    {
        result = ::write(wait->writeFd, &one, sizeof(one));
    } while ((result < 0) && (errno == EINTR));
}

bool backlog_os_wait(SendBacklogWaitOs * wait, const SOCKETHANDLE * sockets, uint32_t count, uint32_t timeoutMs) noexcept
{
    std::vector<struct pollfd> & fds{ wait->fds };
    if (fds.size() < static_cast<std::size_t>(count) + 1u)
    {
        fds.resize(static_cast<std::size_t>(count) + 1u);
    }

    fds[0] = { wait->readFd, POLLIN, 0 };
    for (uint32_t i = 0u; i < count; ++i)
    {
        fds[i + 1u] = { static_cast<int>(sockets[i]), POLLOUT, 0 };
    }

    const int timeout{ timeoutMs < static_cast<uint32_t>(INT32_MAX) ? static_cast<int>(timeoutMs) : -1 };
    if ((::poll(fds.data(), static_cast<nfds_t>(count + 1u), timeout) <= 0) || (fds[0].revents == 0))
        return false;

    // One read empties the wake descriptor.
    uint64_t drained[8];
    const ssize_t drainedBytes{ ::read(wait->readFd, drained, sizeof(drained)) };
    static_cast<void>(drainedBytes);
    return true;
}

} // namespace areg::ext

#endif  // defined(_POSIX) || defined(POSIX)
