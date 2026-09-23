/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketMultiplexerTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for SocketMultiplexer.
 *              Covers the readiness that wait() has already taken out of the
 *              kernel: it is held in the batch cache and must reach the caller,
 *              with and without a wakeup() arriving in the same batch.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketMultiplexer.hpp"

#if defined(_POSIX)

#include <sys/socket.h>
#include <unistd.h>

#include <set>

namespace
{
    using areg::SocketMultiplexer;

    //!< How many sockets are made readable at the same moment.
    constexpr uint32_t  PAIR_COUNT      { 8u };

    //!< Upper bound on the wait() calls a collection loop makes.
    constexpr uint32_t  MAX_WAIT_CALLS  { 64u };

    //!< How many silent wait() calls in a row end a collection loop.
    constexpr uint32_t  SILENT_LIMIT    { 2u };

    //!< Milliseconds the first wait() of a test may block.
    constexpr int32_t   WAIT_MS         { 500 };

    //!< A connected socket pair. 'sock' is registered, 'peer' makes it readable.
    struct Pair
    {
        int sock { -1 };
        int peer { -1 };
    };

    //!< Opens a connected socket pair. Returns false when the system refuses.
    bool open_pair( Pair & pair )
    {
        int fds[2]{ -1, -1 };
        if ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, fds ) != 0 )
            return false;

        pair.sock = fds[0];
        pair.peer = fds[1];
        return true;
    }

    void close_pair( Pair & pair )
    {
        if ( pair.peer != -1 )
            ::close( pair.peer );

        if ( pair.sock != -1 )
            ::close( pair.sock );

        pair.peer = pair.sock = -1;
    }

    //!< Sends one byte, which makes the registered end readable.
    bool feed( const Pair & pair )
    {
        const char one{ 'x' };
        return ::write( pair.peer, &one, sizeof( one ) ) == static_cast<ssize_t>(sizeof( one ));
    }

    //!< Reads every buffered byte out of the registered end.
    void drain( const Pair & pair )
    {
        char buf[64];
        while ( ::recv( pair.sock, buf, sizeof( buf ), MSG_DONTWAIT ) > 0 )
        {
        }
    }

    inline bool is_handle( SOCKETHANDLE handle )
    {
        return (handle != areg::InvalidSocketHandle) && (handle != areg::FailedSocketHandle);
    }

    //!< Calls wait() without blocking until it stays silent, recording every handle.
    void collect( const SocketMultiplexer & mux, std::set<SOCKETHANDLE> & seen )
    {
        uint32_t silent{ 0u };
        for ( uint32_t i = 0u; (i < MAX_WAIT_CALLS) && (silent < SILENT_LIMIT); ++ i )
        {
            const SOCKETHANDLE ready{ mux.wait( 0 ) };
            if ( is_handle( ready ) )
            {
                silent = 0u;
                seen.insert( ready );
            }
            else
            {
                ++ silent;
            }
        }
    }
}

/**
 * \brief   The control. No wakeup is involved, so nothing may interfere with the
 *          batch, and every socket of the burst must be reported.
 **/
TEST( SocketMultiplexerTest, batch_survives_kernel_drain )
{
    SocketMultiplexer mux;
    Pair pairs[PAIR_COUNT];

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
    {
        ASSERT_TRUE( open_pair( pairs[i] ) );
        ASSERT_TRUE( mux.register_socket( static_cast<SOCKETHANDLE>(pairs[i].sock), true ) );
        ASSERT_TRUE( feed( pairs[i] ) );
    }

    // One wait() takes the whole ready set out of the kernel and caches it.
    const SOCKETHANDLE first{ mux.wait( WAIT_MS ) };
    EXPECT_NE( first, areg::FailedSocketHandle );

    // Empty every receive buffer. What wait() reports from here on can only come
    // from the batch it has already taken.
    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        drain( pairs[i] );

    std::set<SOCKETHANDLE> seen;
    if ( is_handle( first ) )
        seen.insert( first );

    collect( mux, seen );

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
    {
        EXPECT_EQ( seen.count( static_cast<SOCKETHANDLE>(pairs[i].sock) ), 1u )
            << "socket " << i << " was never reported";
    }

    EXPECT_EQ( static_cast<uint32_t>(seen.size()), PAIR_COUNT );

    mux.reset();
    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        close_pair( pairs[i] );
}

/**
 * \brief   The same burst with a wakeup() in the same batch. wait() takes the
 *          readiness of every socket out of the kernel before it sees the wakeup,
 *          so serving the wakeup may not drop what was taken with it.
 *
 *          Where the wakeup lands in the batch is the system's choice. Ahead of the
 *          sockets it costs the whole batch, behind them it costs the remainder, and
 *          the sockets are reported either way only while the batch is kept.
 **/
TEST( SocketMultiplexerTest, wakeup_does_not_discard_ready_sockets )
{
    SocketMultiplexer mux;
    Pair pairs[PAIR_COUNT];

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
    {
        ASSERT_TRUE( open_pair( pairs[i] ) );
        ASSERT_TRUE( mux.register_socket( static_cast<SOCKETHANDLE>(pairs[i].sock), true ) );
    }

    // A new connection in the pool receive thread raises the wakeup exactly like this,
    // next to sockets that are already readable.
    mux.wakeup();

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        ASSERT_TRUE( feed( pairs[i] ) );

    const SOCKETHANDLE first{ mux.wait( WAIT_MS ) };
    EXPECT_NE( first, areg::FailedSocketHandle );

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        drain( pairs[i] );

    std::set<SOCKETHANDLE> seen;
    if ( is_handle( first ) )
        seen.insert( first );

    collect( mux, seen );

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
    {
        EXPECT_EQ( seen.count( static_cast<SOCKETHANDLE>(pairs[i].sock) ), 1u )
            << "socket " << i << " was dropped with the wakeup";
    }

    EXPECT_EQ( static_cast<uint32_t>(seen.size()), PAIR_COUNT );

    mux.reset();
    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        close_pair( pairs[i] );
}

/**
 * \brief   A disconnect in the middle of a batch. Unregistering one socket takes that
 *          socket out of the batch and leaves the readiness of the others in place.
 **/
TEST( SocketMultiplexerTest, unregister_keeps_the_other_sockets )
{
    SocketMultiplexer mux;
    Pair pairs[PAIR_COUNT];

    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
    {
        ASSERT_TRUE( open_pair( pairs[i] ) );
        ASSERT_TRUE( mux.register_socket( static_cast<SOCKETHANDLE>(pairs[i].sock), true ) );
        ASSERT_TRUE( feed( pairs[i] ) );
    }

    const SOCKETHANDLE first{ mux.wait( WAIT_MS ) };
    EXPECT_NE( first, areg::FailedSocketHandle );

    // The first pair leaves, exactly as a closing connection does.
    EXPECT_TRUE( mux.unregister_socket( static_cast<SOCKETHANDLE>(pairs[0].sock) ) );

    for ( uint32_t i = 1u; i < PAIR_COUNT; ++ i )
        drain( pairs[i] );

    std::set<SOCKETHANDLE> seen;
    if ( is_handle( first ) )
        seen.insert( first );

    collect( mux, seen );

    for ( uint32_t i = 1u; i < PAIR_COUNT; ++ i )
    {
        EXPECT_EQ( seen.count( static_cast<SOCKETHANDLE>(pairs[i].sock) ), 1u )
            << "socket " << i << " was dropped with the unregistered one";
    }

    mux.reset();
    for ( uint32_t i = 0u; i < PAIR_COUNT; ++ i )
        close_pair( pairs[i] );
}

#endif  // defined(_POSIX)
