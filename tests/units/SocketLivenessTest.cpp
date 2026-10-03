/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketLivenessTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the decision that tells an unreachable peer from
 *              a peer that is reachable but does not read.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/private/SocketLiveness.hpp"

namespace
{
    constexpr uint32_t KEEPALIVE_MS{ 15'000u };

    areg::SocketProgress progress(uint32_t unacked, uint32_t queued, uint32_t lastAckMs)
    {
        areg::SocketProgress result{ };
        result.known     = true;
        result.unacked   = unacked;
        result.queued    = queued;
        result.lastAckMs = lastAckMs;
        return result;
    }
}

/**
 * \brief   Sent data unacknowledged for the keepalive time: the peer's stack stopped answering.
 **/
TEST(SocketLivenessTest, DeadPeerWithDataInFlight)
{
    EXPECT_FALSE(areg::is_peer_unreachable(progress(1u, 64u, KEEPALIVE_MS - 1u), false, 60'000u, KEEPALIVE_MS));
    EXPECT_TRUE (areg::is_peer_unreachable(progress(1u, 64u, KEEPALIVE_MS), false, 60'000u, KEEPALIVE_MS));
    EXPECT_TRUE (areg::is_peer_unreachable(progress(3u, 64u, 32'004u), true, 32'004u, KEEPALIVE_MS));
}

/**
 * \brief   A peer that answers window probes but does not read is never unreachable.
 **/
TEST(SocketLivenessTest, StoppedReaderIsReachable)
{
    EXPECT_FALSE(areg::is_peer_unreachable(progress(0u, 4'000'000u, 10'040u), true, 600'000u, KEEPALIVE_MS));
    EXPECT_FALSE(areg::is_peer_unreachable(progress(0u, 4'000'000u, 536u), false, 600'000u, KEEPALIVE_MS));
}

/**
 * \brief   A peer that dies while its window is closed answers no probe; it is lost once the
 *          longest probe gap has also passed.
 **/
TEST(SocketLivenessTest, DeadWhileWindowClosed)
{
    const uint32_t limit{ KEEPALIVE_MS + areg::ZERO_WINDOW_PROBE_MAX_MS };
    EXPECT_FALSE(areg::is_peer_unreachable(progress(0u, 1'000u, limit - 1u), false, limit, KEEPALIVE_MS));
    EXPECT_TRUE (areg::is_peer_unreachable(progress(0u, 1'000u, limit), false, limit, KEEPALIVE_MS));
    EXPECT_TRUE (areg::is_peer_unreachable(progress(0u, 0u, limit), true, limit, KEEPALIVE_MS));
}

/**
 * \brief   A connection that only just got data to send after a long idle time is not lost,
 *          although its last acknowledgement is old.
 **/
TEST(SocketLivenessTest, FreshDataAfterIdle)
{
    EXPECT_FALSE(areg::is_peer_unreachable(progress(1u, 64u, 600'000u), false, 5u, KEEPALIVE_MS));
    EXPECT_FALSE(areg::is_peer_unreachable(progress(1u, 64u, 600'000u), false, KEEPALIVE_MS - 1u, KEEPALIVE_MS));
    EXPECT_TRUE (areg::is_peer_unreachable(progress(1u, 64u, 600'000u), false, KEEPALIVE_MS, KEEPALIVE_MS));
}

/**
 * \brief   Nothing to send, or nothing known: never unreachable. Keepalive covers an idle
 *          connection, and the other systems bound unacknowledged data themselves.
 **/
TEST(SocketLivenessTest, IdleOrUnknown)
{
    EXPECT_FALSE(areg::is_peer_unreachable(progress(0u, 0u, 600'000u), false, 600'000u, KEEPALIVE_MS));

    areg::SocketProgress unknown{ };
    unknown.unacked   = 1u;
    unknown.lastAckMs = 600'000u;
    EXPECT_FALSE(areg::is_peer_unreachable(unknown, true, 600'000u, KEEPALIVE_MS));
}

/**
 * \brief   A new socket reports no outstanding data on Linux and nothing at all elsewhere.
 **/
TEST(SocketLivenessTest, SocketProgressOfAnIdleSocket)
{
    ASSERT_TRUE(areg::socket_initialize());
    const SOCKETHANDLE hSocket{ areg::socket_create() };
    ASSERT_TRUE(areg::is_valid_socket(hSocket));
    const areg::SocketProgress result{ areg::socket_progress(hSocket) };
    areg::socket_close(hSocket);

#if defined(__linux__)
    EXPECT_TRUE(result.known);
    EXPECT_EQ(result.unacked, 0u);
    EXPECT_EQ(result.queued, 0u);
#else   // __linux__
    EXPECT_FALSE(result.known);
#endif  // __linux__
}
