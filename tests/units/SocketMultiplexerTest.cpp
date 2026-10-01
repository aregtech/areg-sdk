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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
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

/**
 * \brief   The read-ahead cache against the multiplexer. One cached receive moves the
 *          whole kernel buffer into user space, so a peer that goes silent without
 *          closing leaves the multiplexer with nothing to report while the cache is
 *          still full. A receive loop that waits for readiness never comes back for it.
 **/
TEST( SocketMultiplexerTest, cached_data_is_invisible_to_the_multiplexer )
{
    constexpr uint32_t  BURST_SIZE  { 4096u };
    constexpr uint32_t  CHUNK       { 64u };

    SocketMultiplexer mux;
    Pair pair;

    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( mux.register_socket( static_cast<SOCKETHANDLE>(pair.sock), true ) );

    // The peer bursts as much as it can without blocking, then stays silent with the
    // connection open. The peer is non-blocking, so the burst ends when its buffer is full.
    const int peerFlags{ ::fcntl( pair.peer, F_GETFL, 0 ) };
    ASSERT_NE( peerFlags, -1 );
    ASSERT_NE( ::fcntl( pair.peer, F_SETFL, peerFlags | O_NONBLOCK ), -1 );

    char block[CHUNK];
    ::memset( block, 'x', sizeof( block ) );
    uint32_t written{ 0u };
    for ( uint32_t i = 0u; i < BURST_SIZE; ++ i )
    {
        if ( ::send( pair.peer, block, sizeof( block ), 0 ) != static_cast<ssize_t>(sizeof( block )) )
            break;

        written += CHUNK;
    }

    ASSERT_GT( written, CHUNK );

    EXPECT_NE( mux.wait( WAIT_MS ), areg::FailedSocketHandle );

    // One small cached receive. Phase 3 of the cached read asks the kernel for the whole
    // cache capacity, so it takes far more than the caller asked for.
    areg::set_receive_mode( areg::ReceiveMode::MultiCache );
    char one[CHUNK];
    const int32_t got{ areg::receive_data( static_cast<SOCKETHANDLE>(pair.sock), reinterpret_cast<uint8_t *>(one), sizeof( one ) ) };
    EXPECT_EQ( got, static_cast<int32_t>(sizeof( one )) );

    // What the kernel still holds for this socket. The multiplexer can see this and
    // nothing else.
    int pending{ -1 };
    ASSERT_EQ( ::ioctl( pair.sock, FIONREAD, &pending ), 0 );

    const uint32_t consumed{ CHUNK };

    // The premise, stated so that it fails loudly if a platform behaves otherwise.
    ASSERT_GT( written, consumed ) << "the burst was too small to outlive one read";
    ASSERT_EQ( pending, 0 ) << "the cached read left " << pending << " bytes in the kernel";

    // The caller has taken one chunk, the kernel has nothing left, and the rest of the
    // burst is in user space. Nothing has been sent since, so the multiplexer is the only
    // way back to it -- and it cannot see a user-space buffer.
    std::set<SOCKETHANDLE> seen;
    collect( mux, seen );

    EXPECT_EQ( seen.count( static_cast<SOCKETHANDLE>(pair.sock) ), 0u )
        << "the multiplexer reported a socket whose unread data is only in the cache";

    mux.reset();
    close_pair( pair );
}

#endif  // defined(_POSIX)
