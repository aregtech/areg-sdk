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
 *              socket. POSIX part: a write completes when it returns.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/private/SendBacklog.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <cerrno>
#include <poll.h>
#include <sys/socket.h>
#include <sys/uio.h>

namespace areg::ext {

struct SendBacklogOs
{
};

void SendBacklogOsDelete::operator () (SendBacklogOs * os) const noexcept
{
    delete os;
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

bool backlog_os_cancel(SendBacklogOsPtr & os, SOCKETHANDLE /*hSocket*/) noexcept
{
    os.reset();
    return true;
}

void backlog_os_wait(SOCKETHANDLE hSocket, uint32_t timeoutMs) noexcept
{
    struct pollfd fd { };
    fd.fd       = static_cast<int>(hSocket);
    fd.events   = POLLOUT;
    static_cast<void>(::poll(&fd, 1, static_cast<int>(timeoutMs)));
}

} // namespace areg::ext

#endif  // defined(_POSIX) || defined(POSIX)
