#ifndef AREG_BASE_PRIVATE_POSIX_SOCKETSENDPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SOCKETSENDPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/SocketSendPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the send calls of a socket on POSIX.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include <cstddef>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

namespace areg::os
{
    /**
     * \brief   Sends bytes as ::send(). On Cygwin one call takes at most one send chunk of the
     *          socket, and the caller sends the rest.
     * \param   hSocket     The connected socket.
     * \param   data        The bytes to send.
     * \param   size        The number of bytes to send.
     * \param   flags       The flags of ::send().
     * \return  The bytes taken, or -1 with errno set.
     **/
    inline ssize_t _os_socket_send(int hSocket, const void * data, std::size_t size, int flags) noexcept;

    /**
     * \brief   Sends a gather list as ::sendmsg(). On Cygwin one call takes at most one send chunk
     *          of the socket, and the caller sends the rest.
     * \param   hSocket     The connected socket.
     * \param   message     The message with the gather list.
     * \param   flags       The flags of ::sendmsg().
     * \return  The bytes taken, or -1 with errno set.
     **/
    inline ssize_t _os_socket_send_v(int hSocket, const struct msghdr & message, int flags) noexcept;

#if defined(__CYGWIN__)

    //!< The largest send chunk of a socket whose send size was not set.
    constexpr int CYGWIN_SEND_CHUNK { 212'992 };

    //!< Returns the largest number of bytes one send of the socket may take.
    inline std::size_t _os_socket_send_chunk(int hSocket) noexcept
    {
        int size{ 0 };
        socklen_t length{ sizeof(size) };
        const bool known{ (::getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, &size, &length) == 0) && (size > 0) };
        return static_cast<std::size_t>((known && (size < CYGWIN_SEND_CHUNK)) ? size : CYGWIN_SEND_CHUNK);
    }

#endif  // defined(__CYGWIN__)
}

inline ssize_t areg::os::_os_socket_send(int hSocket, const void * data, std::size_t size, int flags) noexcept
{
#if defined(__CYGWIN__)
    const std::size_t chunk{ _os_socket_send_chunk(hSocket) };
    return ::send(hSocket, data, (size < chunk) ? size : chunk, flags);
#else   // defined(__CYGWIN__)
    return ::send(hSocket, data, size, flags);
#endif  // defined(__CYGWIN__)
}

inline ssize_t areg::os::_os_socket_send_v(int hSocket, const struct msghdr & message, int flags) noexcept
{
#if defined(__CYGWIN__)
    const std::size_t chunk{ _os_socket_send_chunk(hSocket) };
    const int count{ static_cast<int>(message.msg_iovlen) };
    std::size_t total{ 0u };
    int fits{ 0 };
    while ((fits < count) && (message.msg_iov[fits].iov_len <= (chunk - total)))
    {
        total += message.msg_iov[fits].iov_len;
        ++fits;
    }

    if (fits == 0)
    {
        return ::send(hSocket, message.msg_iov[0].iov_base, chunk, flags);
    }

    struct msghdr part { message };
    part.msg_iovlen = fits;
    return ::sendmsg(hSocket, &part, flags);
#else   // defined(__CYGWIN__)
    return ::sendmsg(hSocket, &message, flags);
#endif  // defined(__CYGWIN__)
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SOCKETSENDPOSIX_HPP
