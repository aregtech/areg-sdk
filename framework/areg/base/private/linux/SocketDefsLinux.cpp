/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/linux/SocketDefsLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Linux-specific socket helpers: MSG_ZEROCOPY send and
 *              ERRQUEUE drain for confirming kernel DMA completion.
 ************************************************************************/
#ifdef __linux__

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SocketDefs.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <linux/errqueue.h>
#include <poll.h>

namespace areg::os {

// Forward-declare POSIX fallback used when MSG_ZEROCOPY is unavailable or ENOBUFS.
int32_t _os_send_data(SOCKETHANDLE hSocket, const uint8_t* dataBuffer, int32_t dataLength);

namespace {
    thread_local uint32_t _zerocopyCount { 0u };
} // namespace

bool _os_enable_zerocopy(SOCKETHANDLE fd) noexcept
{
#if defined(SO_ZEROCOPY)
    const int one{ 1 };
    return (RETURNED_OK == ::setsockopt(static_cast<int>(fd), SOL_SOCKET, SO_ZEROCOPY, &one, sizeof(one)));
#else
    return false;
#endif  // defined(SO_ZEROCOPY)
}

uint32_t _os_take_zerocopy_count() noexcept
{
    const uint32_t count{ _zerocopyCount };
    _zerocopyCount = 0u;
    return count;
}

int32_t _os_send_zerocopy(SOCKETHANDLE fd, const uint8_t* buf, int32_t len) noexcept
{
#if defined(MSG_ZEROCOPY) && defined(MSG_NOSIGNAL)
    constexpr int flags = MSG_ZEROCOPY | MSG_NOSIGNAL;
#elif defined(MSG_ZEROCOPY)
    constexpr int flags = MSG_ZEROCOPY;
#else
    // Old Kernel, fall back to regular send.
    return _os_send_data(fd, buf, len);
#endif
#if defined(MSG_ZEROCOPY)
    int32_t total{ 0 };
    while (total < len)
    {
        const ssize_t written = ::send(static_cast<int>(fd),
                                       reinterpret_cast<const char*>(buf + total),
                                       static_cast<size_t>(len - total),
                                       flags);
        if (written > 0)
        {
            // Each successful MSG_ZEROCOPY send gets a unique sequence ID.
            ++_zerocopyCount;
            total += static_cast<int32_t>(written);
        }
        else if (errno == EINTR)
        {
            continue;   // interrupted by signal — retry, same sequence ID
        }
        else if (errno == ENOBUFS)
        {
            // Kernel zerocopy buffers exhausted — fall back to regular copy.
            const int32_t rest = _os_send_data(fd, buf + total, len - total);
            if (rest < 0)
                return -1;
            total += rest;
            break;
        }
        else
        {
            return -1;  // hard error (EPIPE, ECONNRESET, SO_SNDTIMEO expired …)
        }
    }

    return total;
#endif  // defined(MSG_ZEROCOPY)
}

void _os_drain_zerocopy_nb(SOCKETHANDLE fd, uint32_t& max_confirmed) noexcept
{
#if defined(SO_EE_ORIGIN_ZEROCOPY)
    while (true)
    {
        struct pollfd pfd { static_cast<int>(fd), POLLERR, 0 };
        const int ready = ::poll(&pfd, 1, 0);   // timeout = 0 --> non-blocking
        if (ready <= 0)
            return;

        if (pfd.revents & (POLLHUP | POLLNVAL))
            return;

        if (!(pfd.revents & POLLERR))
            return;

        char ctrl[128];
        char data[1];
        struct iovec iov_data { data, sizeof(data) };
        struct msghdr msg {};
        msg.msg_iov        = &iov_data;
        msg.msg_iovlen     = 1;
        msg.msg_control    = ctrl;
        msg.msg_controllen = sizeof(ctrl);

        const ssize_t ret = ::recvmsg(static_cast<int>(fd), &msg, MSG_ERRQUEUE);
        if (ret < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                return;

            return;   // socket error
        }

        for (struct cmsghdr* cm = CMSG_FIRSTHDR(&msg); cm != nullptr; cm = CMSG_NXTHDR(&msg, cm))
        {
            if (cm->cmsg_level != SOL_IP || cm->cmsg_type != IP_RECVERR)
                continue;

            const auto* ee = reinterpret_cast<const struct sock_extended_err*>(CMSG_DATA(cm));
            if (ee->ee_origin != SO_EE_ORIGIN_ZEROCOPY)
                continue;

            // ee_data is the inclusive end of the confirmed range.
            // Use wrap-safe comparison to update max_confirmed.
            if (static_cast<int32_t>(ee->ee_data - max_confirmed) > 0)
                max_confirmed = ee->ee_data;
        }
    }
#endif  // defined(SO_EE_ORIGIN_ZEROCOPY)
}

void _os_drain_zerocopy(SOCKETHANDLE fd, uint32_t hi_id) noexcept
{
#if defined(SO_EE_ORIGIN_ZEROCOPY)
    constexpr int POLL_TIMEOUT_MS{ 5000 };
    while (true)
    {
        struct pollfd pfd { static_cast<int>(fd), POLLERR, 0 };
        const int ready = ::poll(&pfd, 1, POLL_TIMEOUT_MS);
        if (ready <= 0)
            return;   // timeout or error — release buffers conservatively

        if (pfd.revents & (POLLHUP | POLLNVAL))
            return;   // socket closed

        if (!(pfd.revents & POLLERR))
            continue; // spurious wake

        char ctrl[128];
        char data[1];
        struct iovec iov_data { data, sizeof(data) };
        struct msghdr msg {};
        msg.msg_iov        = &iov_data;
        msg.msg_iovlen     = 1;
        msg.msg_control    = ctrl;
        msg.msg_controllen = sizeof(ctrl);

        const ssize_t ret = ::recvmsg(static_cast<int>(fd), &msg, MSG_ERRQUEUE);
        if (ret < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                continue;

            return;   // socket error or closed
        }

        for (struct cmsghdr* cm = CMSG_FIRSTHDR(&msg); cm != nullptr; cm = CMSG_NXTHDR(&msg, cm))
        {
            if (cm->cmsg_level != SOL_IP || cm->cmsg_type != IP_RECVERR)
                continue;

            const auto* ee = reinterpret_cast<const struct sock_extended_err*>(CMSG_DATA(cm));
            if (ee->ee_origin != SO_EE_ORIGIN_ZEROCOPY)
                continue;

            // ee_data is the inclusive end of the confirmed ID range.
            // Wrap-safe comparison: (int32_t)(completed - requested) >= 0.
            if (static_cast<int32_t>(ee->ee_data - hi_id) >= 0)
                return;   // all sends up to hi_id confirmed
        }
    }
#endif  // defined(SO_EE_ORIGIN_ZEROCOPY)
}

} // namespace areg::os

#endif  // __linux__
