#ifndef AREG_BASE_PRIVATE_SOCKETLIVENESS_HPP
#define AREG_BASE_PRIVATE_SOCKETLIVENESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/SocketLiveness.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, tells a peer whose TCP stack stopped answering from a peer that
 *              is reachable but does not read, for a connection that has data to send.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(__linux__)
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
#endif  // __linux__

namespace areg
{
    //!< The longest gap between two zero window probes of a sender, in milliseconds.
    constexpr uint32_t  ZERO_WINDOW_PROBE_MAX_MS    { 120'000u };

    //!< A refusal limit that never closes a reachable peer, in milliseconds.
    constexpr uint32_t  SEND_REFUSAL_UNLIMITED      { 0x7FFF'FFFFu };

    /**
     * \brief   Returns the milliseconds a reachable peer may take no data before its connection
     *          is closed, from the configured net::*::tcpip::timeout. Zero means the build
     *          default: unlimited in a debug build, the keepalive time in a release build.
     *
     * \param   configuredMs    The configured value, 0 if not set.
     * \param   keepaliveSec    The keepalive time in seconds.
     **/
    [[nodiscard]]
    constexpr uint32_t send_refusal_ms(uint32_t configuredMs, uint32_t keepaliveSec) noexcept;

    /**
     * \brief   Applies the refusal limit to a connected socket. On Linux the system closes a
     *          connection whose data is not acknowledged for that long; with an unlimited
     *          limit it is left to the keepalive and retransmission settings.
     **/
    inline void set_send_refusal(SOCKETHANDLE hSocket, uint32_t refusalMs) noexcept;

    /**
     * \brief   What the TCP stack of one connection reports about the data it holds.
     **/
    struct SocketProgress
    {
        bool        known       { false };  //!< True if the operating system reported the values below.
        uint32_t    unacked     { 0u };     //!< Segments sent and not acknowledged.
        uint32_t    queued      { 0u };     //!< Bytes in the send queue, sent or not.
        uint32_t    lastAckMs   { 0u };     //!< Milliseconds since the last acknowledgement arrived.
    };

    /**
     * \brief   Decides whether the peer of a connection is unreachable. Sent data with no
     *          acknowledgement for \a keepaliveMs means the peer's stack stopped answering.
     *          Data that waits for a closed receive window is lost only when even the window
     *          probes stay unanswered, which may take ZERO_WINDOW_PROBE_MAX_MS longer.
     *          A peer that answers but does not read is never unreachable.
     *
     * \param   progress        What the TCP stack reports, see socket_progress().
     * \param   userBacklog     True if data waits in user space to be written to the socket.
     * \param   outstandingMs   Milliseconds the connection has had data to send without a break.
     * \param   keepaliveMs     Milliseconds a silent peer is given, the keepalive time.
     * \return  True if the peer is unreachable and the connection must be closed.
     **/
    [[nodiscard]]
    constexpr bool is_peer_unreachable( const SocketProgress & progress
                                      , bool userBacklog
                                      , uint32_t outstandingMs
                                      , uint32_t keepaliveMs ) noexcept;

    /**
     * \brief   Returns what the TCP stack reports about the data of \a hSocket.
     *          Only Linux reports it; elsewhere SocketProgress::known is false and the
     *          operating system bounds unacknowledged data itself.
     **/
    [[nodiscard]]
    inline SocketProgress socket_progress(SOCKETHANDLE hSocket) noexcept;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Inline implementations
//////////////////////////////////////////////////////////////////////////

constexpr bool areg::is_peer_unreachable( const areg::SocketProgress & progress
                                        , bool userBacklog
                                        , uint32_t outstandingMs
                                        , uint32_t keepaliveMs ) noexcept
{
    const uint32_t silentMs{ progress.lastAckMs < outstandingMs ? progress.lastAckMs : outstandingMs };
    if ( progress.known == false )
    {
        return false;
    }
    else if ( progress.unacked != 0u )
    {
        return (silentMs >= keepaliveMs);
    }
    else if ( userBacklog || (progress.queued != 0u) )
    {
        return (silentMs >= keepaliveMs + areg::ZERO_WINDOW_PROBE_MAX_MS);
    }
    else
    {
        return false;
    }
}

constexpr uint32_t areg::send_refusal_ms(uint32_t configuredMs, [[maybe_unused]] uint32_t keepaliveSec) noexcept
{
    if ( configuredMs != 0u )
    {
        return (configuredMs < areg::SEND_REFUSAL_UNLIMITED ? configuredMs : areg::SEND_REFUSAL_UNLIMITED);
    }

#if defined(DEBUG)
    return areg::SEND_REFUSAL_UNLIMITED;
#else   // defined(DEBUG)
    return keepaliveSec * 1'000u;
#endif  // defined(DEBUG)
}

inline void areg::set_send_refusal([[maybe_unused]] SOCKETHANDLE hSocket, [[maybe_unused]] uint32_t refusalMs) noexcept
{
#if defined(__linux__) && defined(TCP_USER_TIMEOUT)
    const unsigned int userTimeout{ refusalMs < areg::SEND_REFUSAL_UNLIMITED ? refusalMs : 0u };
    ::setsockopt(static_cast<int>(hSocket), IPPROTO_TCP, TCP_USER_TIMEOUT, &userTimeout, static_cast<socklen_t>(sizeof(userTimeout)));
#endif  // defined(__linux__) && defined(TCP_USER_TIMEOUT)
}

inline areg::SocketProgress areg::socket_progress([[maybe_unused]] SOCKETHANDLE hSocket) noexcept
{
    areg::SocketProgress result{ };

#if defined(__linux__) && defined(TCP_INFO)
    struct tcp_info info { };
    socklen_t length{ static_cast<socklen_t>(sizeof(info)) };
    int queued{ 0 };
    if ( (::getsockopt(static_cast<int>(hSocket), IPPROTO_TCP, TCP_INFO, &info, &length) == 0) &&
         (::ioctl(static_cast<int>(hSocket), TIOCOUTQ, &queued) == 0) )
    {
        result.known     = true;
        result.unacked   = info.tcpi_unacked;
        result.queued    = queued > 0 ? static_cast<uint32_t>(queued) : 0u;
        result.lastAckMs = info.tcpi_last_ack_recv;
    }
#endif  // defined(__linux__) && defined(TCP_INFO)

    return result;
}

#endif  // AREG_BASE_PRIVATE_SOCKETLIVENESS_HPP
