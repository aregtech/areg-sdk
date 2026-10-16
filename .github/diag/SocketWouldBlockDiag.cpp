/************************************************************************
 * TEMPORARY diagnostic for bug 150, copied into tests/units by .github/workflows/diag-814.yml.
 * Not part of the unit tests. Removed together with the workflow.
 *
 * Question: after fill() reports a full send buffer, why does a 64-byte try-send find room
 * on macOS? Each iteration prints the send-buffer state right after the fill, right before
 * the try-send, and 20 ms later, so the log shows whether the room comes from data leaving
 * the send buffer (used shrinks), from the buffer growing (sndbuf grows), or from neither.
 *
 * OriginalFill        - the fill of SocketWouldBlockTest (64 KB chunks until EAGAIN).
 * SettledFill         - candidate: chunks down to 1 byte, refilled until the socket stays
 *                       non-writable for SETTLE_MS.
 * SettledFillFixedBuf - candidate: as SettledFill, with SO_SNDBUF / SO_RCVBUF set first.
 * RetryFixedBuf       - the fixed test: as SettledFillFixedBuf, filled again while the try-send
 *                       finds room. Prints the attempts it needed.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"

#if defined(_POSIX)

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;

    constexpr uint32_t  SEND_TIMEOUT_MS { 2500u };
    constexpr int       SETTLE_MS       { 50 };
    constexpr uint32_t  MAX_ROUNDS      { 50u };

    struct Pair
    {
        int client { -1 };
        int server { -1 };
    };

    bool open_pair( Pair & pair )
    {
        const int listener{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
        if ( listener < 0 )
            return false;

        sockaddr_in addr{ };
        addr.sin_family         = AF_INET;
        addr.sin_addr.s_addr    = htonl( INADDR_LOOPBACK );
        addr.sin_port           = 0;
        socklen_t len{ sizeof( addr ) };
        bool result{ false };
        if ( ( ::bind( listener, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0 )
          && ( ::listen( listener, 1 ) == 0 )
          && ( ::getsockname( listener, reinterpret_cast<sockaddr *>(&addr), &len ) == 0 ) )
        {
            pair.client = ::socket( AF_INET, SOCK_STREAM, 0 );
            if ( ( pair.client >= 0 ) && ( ::connect( pair.client, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0 ) )
            {
                const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                const SOCKETHANDLE accepted{ areg::server_accept( static_cast<SOCKETHANDLE>(listener), &none, 0 ) };
                if ( areg::is_valid_socket( accepted ) )
                {
                    pair.server = static_cast<int>(accepted);
                    result      = true;
                }
            }
        }

        ::close( listener );
        return result;
    }

    void close_pair( Pair & pair )
    {
        if ( pair.client >= 0 )
            areg::socket_close( static_cast<SOCKETHANDLE>(pair.client) );
        if ( pair.server >= 0 )
            areg::socket_close( static_cast<SOCKETHANDLE>(pair.server) );

        pair.client = -1;
        pair.server = -1;
    }

    bool set_nonblocking( int sock, bool nonBlocking )
    {
        const int flags{ ::fcntl( sock, F_GETFL, 0 ) };
        if ( flags == -1 )
            return false;

        const int wanted{ nonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK ) };
        return ( ::fcntl( sock, F_SETFL, wanted ) != -1 );
    }

    int sock_opt( int sock, int name )
    {
        int value{ -1 };
        socklen_t len{ sizeof( value ) };
        return ( ::getsockopt( sock, SOL_SOCKET, name, &value, &len ) == 0 ) ? value : -1;
    }

    //!< Bytes in the send buffer not yet acknowledged by the peer.
    int send_queued( int sock )
    {
#if defined(SO_NWRITE)
        return sock_opt( sock, SO_NWRITE );
#elif defined(SIOCOUTQ)
        int value{ -1 };
        return ( ::ioctl( sock, SIOCOUTQ, &value ) == 0 ) ? value : -1;
#else
        return -1;
#endif
    }

    //!< Bytes waiting in the receive buffer.
    int recv_queued( int sock )
    {
        int value{ -1 };
        return ( ::ioctl( sock, FIONREAD, &value ) == 0 ) ? value : -1;
    }

    //!< 1 if the socket polls writable within 'ms', 0 if not, -1 on error.
    int writable( int sock, int ms )
    {
        pollfd ready{ };
        ready.fd     = sock;
        ready.events = POLLOUT;
        return ::poll( &ready, 1, ms );
    }

    struct Snapshot
    {
        int sndbuf      { -1 };
        int lowat       { -1 };
        int queued      { -1 };
        int peerRcvbuf  { -1 };
        int peerQueued  { -1 };
        int ready       { -1 };
    };

    Snapshot snap( const Pair & pair )
    {
        Snapshot result;
        result.sndbuf       = sock_opt( pair.client, SO_SNDBUF );
        result.lowat        = sock_opt( pair.client, SO_SNDLOWAT );
        result.queued       = send_queued( pair.client );
        result.peerRcvbuf   = sock_opt( pair.server, SO_RCVBUF );
        result.peerQueued   = recv_queued( pair.server );
        result.ready        = writable( pair.client, 0 );
        return result;
    }

    void print( const char * test, const char * tag, uint32_t iter, const Snapshot & s )
    {
        std::printf( "DIAG150 %s %s iter=%u sndbuf=%d lowat=%d queued=%d peer_rcvbuf=%d peer_queued=%d writable=%d\n"
                   , test, tag, iter, s.sndbuf, s.lowat, s.queued, s.peerRcvbuf, s.peerQueued, s.ready );
    }

    //!< Sends 'chunk' sized pieces until the socket refuses one. Returns the bytes sent, or -1 on an error.
    int64_t send_until_full( int sock, size_t chunk )
    {
        std::vector<uint8_t> data( chunk, 0x5Au );
        int64_t total{ 0 };
        for ( ;; )
        {
            const ssize_t written{ ::send( sock, data.data( ), data.size( ), 0 ) };
            if ( written > 0 )
            {
                total += written;
                continue;
            }

            if ( ( written < 0 ) && ( errno == EINTR ) )
                continue;

            return ( ( written < 0 ) && ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) ) ) ? total : -1;
        }
    }

    int64_t fill_original( int sock )
    {
        static_cast<void>(set_nonblocking( sock, true ));
        const int64_t total{ send_until_full( sock, 64u * 1024u ) };
        static_cast<void>(set_nonblocking( sock, false ));
        return total;
    }

    int64_t fill_settled( int sock, uint32_t & rounds )
    {
        static_cast<void>(set_nonblocking( sock, true ));
        int64_t total{ 0 };
        for ( rounds = 1u; rounds <= MAX_ROUNDS; ++ rounds )
        {
            for ( const size_t chunk : { size_t{ 64u * 1024u }, size_t{ 4096u }, size_t{ 256u }, size_t{ 1u } } )
            {
                const int64_t sent{ send_until_full( sock, chunk ) };
                if ( sent < 0 )
                {
                    total = -1;
                    break;
                }

                total += sent;
            }

            if ( ( total < 0 ) || ( writable( sock, SETTLE_MS ) == 0 ) )
                break;
        }

        static_cast<void>(set_nonblocking( sock, false ));
        return total;
    }

    void set_buffers( const Pair & pair )
    {
        constexpr int SIZE{ 64 * 1024 };
        static_cast<void>(::setsockopt( pair.client, SOL_SOCKET, SO_SNDBUF, &SIZE, sizeof( SIZE ) ));
        static_cast<void>(::setsockopt( pair.server, SOL_SOCKET, SO_RCVBUF, &SIZE, sizeof( SIZE ) ));
    }

    enum class Fill { Original, Settled, SettledFixedBuffers };

    void run_case( const char * test, Fill kind, uint32_t & iter, uint32_t & hits )
    {
        Pair pair;
        ASSERT_TRUE( open_pair( pair ) );
        ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.client), SEND_TIMEOUT_MS ) );
        if ( kind == Fill::SettledFixedBuffers )
            set_buffers( pair );

        const Snapshot initial{ snap( pair ) };
        uint32_t rounds{ 0u };
        const int64_t total{ kind == Fill::Original ? fill_original( pair.client ) : fill_settled( pair.client, rounds ) };
        const Snapshot filled{ snap( pair ) };
        ASSERT_GT( total, 0 );

        uint8_t message[64]{ };
        const areg::IoBuffer buffer{ message, sizeof( message ) };
        const Clock::time_point start{ Clock::now( ) };
        const int32_t sent{ areg::try_send_data_v( static_cast<SOCKETHANDLE>(pair.client), &buffer, 1u, sizeof( message ) ) };
        const int64_t took{ std::chrono::duration_cast<std::chrono::microseconds>( Clock::now( ) - start ).count( ) };

        std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
        const Snapshot later{ snap( pair ) };

        ++ iter;
        if ( sent != 0 )
            ++ hits;

        if ( ( sent != 0 ) || ( iter <= 3u ) )
        {
            std::printf( "DIAG150 %s %s iter=%u filled=%lld rounds=%u sent=%d took_us=%lld hits=%u\n"
                       , test, ( sent != 0 ) ? "HIT" : "sample", iter, static_cast<long long>(total), rounds, sent
                       , static_cast<long long>(took), hits );
            print( test, "initial", iter, initial );
            print( test, "filled ", iter, filled );
            print( test, "later  ", iter, later );
        }

        EXPECT_EQ( sent, 0 );
        close_pair( pair );
    }

    //!< The method of the fixed test: fixed buffers, settled fill, refill while the try-send finds room.
    void run_retry( uint32_t & iter, uint32_t & hits )
    {
        constexpr uint32_t MAX_ATTEMPTS{ 10u };

        Pair pair;
        ASSERT_TRUE( open_pair( pair ) );
        ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.client), SEND_TIMEOUT_MS ) );
        set_buffers( pair );

        uint8_t message[64]{ };
        const areg::IoBuffer buffer{ message, sizeof( message ) };
        int32_t sent{ -1 };
        int64_t maxTook{ 0 };
        uint32_t attempts{ 0u };
        Snapshot filled;
        while ( ( sent != 0 ) && ( attempts < MAX_ATTEMPTS ) )
        {
            ++ attempts;
            uint32_t rounds{ 0u };
            ASSERT_GT( fill_settled( pair.client, rounds ), 0 );
            filled = snap( pair );

            const Clock::time_point start{ Clock::now( ) };
            sent = areg::try_send_data_v( static_cast<SOCKETHANDLE>(pair.client), &buffer, 1u, sizeof( message ) );
            const int64_t took{ std::chrono::duration_cast<std::chrono::microseconds>( Clock::now( ) - start ).count( ) };
            maxTook = ( took > maxTook ) ? took : maxTook;
        }

        ++ iter;
        if ( sent != 0 )
            ++ hits;

        std::printf( "DIAG150 RetryFixedBuf done iter=%u attempts=%u sent=%d max_took_us=%lld\n"
                   , iter, attempts, sent, static_cast<long long>(maxTook) );
        if ( attempts > 1u )
            print( "RetryFixedBuf", "filled ", iter, filled );

        EXPECT_EQ( sent, 0 );
        close_pair( pair );
    }

    uint32_t gOriginalIter{ 0u }, gOriginalHits{ 0u };
    uint32_t gSettledIter{ 0u }, gSettledHits{ 0u };
    uint32_t gFixedIter{ 0u }, gFixedHits{ 0u };
    uint32_t gRetryIter{ 0u }, gRetryHits{ 0u };
}

TEST( SocketWouldBlockDiag, OriginalFill )
{
    run_case( "OriginalFill", Fill::Original, gOriginalIter, gOriginalHits );
}

TEST( SocketWouldBlockDiag, SettledFill )
{
    run_case( "SettledFill", Fill::Settled, gSettledIter, gSettledHits );
}

TEST( SocketWouldBlockDiag, SettledFillFixedBuf )
{
    run_case( "SettledFillFixedBuf", Fill::SettledFixedBuffers, gFixedIter, gFixedHits );
}

TEST( SocketWouldBlockDiag, RetryFixedBuf )
{
    run_retry( gRetryIter, gRetryHits );
}

#endif  // defined(_POSIX)
