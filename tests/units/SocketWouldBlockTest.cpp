/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketWouldBlockTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the socket helpers when the socket would block.
 *              A try-send on a full socket returns at once, an accepted socket never blocks
 *              its writer, and the blocking helpers wait on a non-blocking socket instead of
 *              failing, within the send timeout of the socket.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"

#if defined(_POSIX)

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <future>
#include <thread>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< Send timeout of the sockets. A send that blocks returns only after it.
    constexpr uint32_t  SEND_TIMEOUT_MS { 30'000u };

    //!< Milliseconds a call that must not block may take: a blocking one takes the send timeout.
    constexpr int64_t   PROMPT_MS       { static_cast<int64_t>(SEND_TIMEOUT_MS) / 2 };

    //!< Milliseconds a call is given to return before the test closes its socket to free it.
    constexpr int64_t   DEADLINE_MS     { 60'000 };

    //!< Bytes of the large transfers.
    constexpr uint32_t  LARGE_BYTES     { 8u * 1024u * 1024u };

    //!< A connected TCP loopback pair. 'server' is accepted by areg::server_accept().
    struct Pair
    {
        int client { -1 };
        int server { -1 };
    };

    //!< Opens a connected loopback pair. Returns false when the system refuses.
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

    //!< Closes both ends of the pair.
    void close_pair( Pair & pair )
    {
        if ( pair.client >= 0 )
            areg::socket_close( static_cast<SOCKETHANDLE>(pair.client) );
        if ( pair.server >= 0 )
            areg::socket_close( static_cast<SOCKETHANDLE>(pair.server) );

        pair.client = -1;
        pair.server = -1;
    }

    //!< Sets or clears O_NONBLOCK on the socket. Returns false on failure.
    bool set_nonblocking( int sock, bool nonBlocking )
    {
        const int flags{ ::fcntl( sock, F_GETFL, 0 ) };
        if ( flags == -1 )
            return false;

        const int wanted{ nonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK ) };
        return ( ::fcntl( sock, F_SETFL, wanted ) != -1 );
    }

    //!< Returns true if O_NONBLOCK is set on the socket.
    bool is_nonblocking( int sock )
    {
        const int flags{ ::fcntl( sock, F_GETFL, 0 ) };
        return ( ( flags != -1 ) && ( ( flags & O_NONBLOCK ) != 0 ) );
    }

    //!< Sends 'chunk' sized pieces until the socket refuses one. Returns false on an error other than EAGAIN.
    bool send_until_full( int sock, size_t chunk, uint64_t & total )
    {
        std::vector<uint8_t> data( chunk, 0x5Au );
        for ( ;; )
        {
            const ssize_t written{ ::send( sock, data.data( ), data.size( ), 0 ) };
            if ( written > 0 )
            {
                total += static_cast<uint64_t>(written);
                continue;
            }

            if ( ( written < 0 ) && ( errno == EINTR ) )
                continue;

            return ( ( written < 0 ) && ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) ) );
        }
    }

    //!< Writes into the socket until its send buffer is full. Restores the blocking mode it found.
    //!< Queued data keeps moving to the peer after the first EAGAIN, so the fill is repeated with
    //!< ever smaller pieces until the socket stays not writable for SETTLE_MS.
    bool fill( int sock )
    {
        constexpr int       SETTLE_MS   { 50 };
        constexpr uint32_t  MAX_ROUNDS  { 50u };

        const bool wasNonBlocking{ is_nonblocking( sock ) };
        if ( ( wasNonBlocking == false ) && ( set_nonblocking( sock, true ) == false ) )
            return false;

        uint64_t total{ 0u };
        bool full{ false };
        for ( uint32_t round = 0u; ( full == false ) && ( round < MAX_ROUNDS ); ++ round )
        {
            bool sent{ true };
            for ( const size_t chunk : { size_t{ 64u * 1024u }, size_t{ 4096u }, size_t{ 256u }, size_t{ 1u } } )
            {
                sent = sent && send_until_full( sock, chunk, total );
            }

            if ( sent == false )
                break;

            pollfd ready{ };
            ready.fd     = sock;
            ready.events = POLLOUT;
            full = ( ::poll( &ready, 1, SETTLE_MS ) == 0 );
        }

        if ( wasNonBlocking == false )
            static_cast<void>(set_nonblocking( sock, false ));

        return ( full && ( total != 0u ) );
    }

    //!< Sets 64 KB send and receive buffers on both ends, so that a large transfer fills them.
    void shrink_buffers( const Pair & pair )
    {
        constexpr int SIZE{ 64 * 1024 };
        for ( const int sock : { pair.client, pair.server } )
        {
            static_cast<void>(::setsockopt( sock, SOL_SOCKET, SO_SNDBUF, &SIZE, sizeof( SIZE ) ));
            static_cast<void>(::setsockopt( sock, SOL_SOCKET, SO_RCVBUF, &SIZE, sizeof( SIZE ) ));
        }
    }

    //!< Milliseconds since the given moment.
    int64_t elapsed_ms( Clock::time_point since )
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>( Clock::now( ) - since ).count( );
    }

    //!< Reads 'total' bytes from the socket in small pieces with pauses, and checks the pattern.
    void slow_reader( int sock, uint32_t total, std::atomic<uint32_t> & received, std::atomic<bool> & intact )
    {
        std::vector<uint8_t> chunk( 64u * 1024u );
        uint32_t offset{ 0u };
        while ( offset < total )
        {
            const ssize_t got{ ::recv( sock, chunk.data( ), chunk.size( ), 0 ) };
            if ( got <= 0 )
            {
                if ( ( got < 0 ) && ( errno == EINTR ) )
                    continue;

                break;
            }

            for ( ssize_t i = 0; i < got; ++ i )
            {
                if ( chunk[static_cast<size_t>(i)] != static_cast<uint8_t>( ( offset + static_cast<uint32_t>(i) ) & 0xFFu ) )
                    intact = false;
            }

            offset += static_cast<uint32_t>(got);
            received = offset;
            std::this_thread::sleep_for( std::chrono::microseconds( 200 ) );
        }
    }

    //!< Fills the buffer with the byte pattern slow_reader() checks.
    std::vector<uint8_t> make_pattern( uint32_t size )
    {
        std::vector<uint8_t> data( size );
        for ( uint32_t i = 0u; i < size; ++ i )
            data[i] = static_cast<uint8_t>( i & 0xFFu );

        return data;
    }

    //!< Sends 'data' from another thread in two halves, after a delay and with a gap between them.
    std::thread late_writer( int sock, const std::vector<uint8_t> & data )
    {
        return std::thread( [sock, &data]( )
            {
                const size_t half{ data.size( ) / 2u };
                std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
                static_cast<void>(::send( sock, data.data( ), half, 0 ));
                std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
                static_cast<void>(::send( sock, data.data( ) + half, data.size( ) - half, 0 ));
            } );
    }
}

/**
 * \brief   A try-send on a blocking socket whose send buffer is full returns 0 before the send
 *          timeout could expire.
 **/
TEST( SocketWouldBlockTest, TrySendOnFullSocketReturnsAtOnce )
{
    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.client), SEND_TIMEOUT_MS ) );
    ASSERT_FALSE( is_nonblocking( pair.client ) );
    shrink_buffers( pair );

    // The receive buffer of the peer can still grow after a fill (macOS), so a try-send may find
    // room. Sending all of it at once is then correct; the unread data stays and the fill repeats.
    constexpr uint32_t MAX_ATTEMPTS{ 10u };
    uint8_t message[64]{ };
    const areg::IoBuffer buffer{ message, sizeof( message ) };
    int32_t sent{ -1 };
    for ( uint32_t attempt = 0u; ( sent != 0 ) && ( attempt < MAX_ATTEMPTS ); ++ attempt )
    {
        ASSERT_TRUE( fill( pair.client ) );

        const Clock::time_point start{ Clock::now( ) };
        sent = areg::try_send_data_v( static_cast<SOCKETHANDLE>(pair.client), &buffer, 1u, sizeof( message ) );
        const int64_t took{ elapsed_ms( start ) };

        EXPECT_LT( took, PROMPT_MS ) << "the try-send blocked for " << took << " ms";
        ASSERT_TRUE( ( sent == 0 ) || ( sent == static_cast<int32_t>(sizeof( message )) ) ) << "sent " << sent;
    }

    EXPECT_EQ( sent, 0 );

    close_pair( pair );
}

/**
 * \brief   A send with MSG_DONTWAIT on a full accepted socket returns before the send timeout could expire.
 **/
TEST( SocketWouldBlockTest, AcceptedSocketNeverBlocksItsWriter )
{
    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.server), SEND_TIMEOUT_MS ) );
    ASSERT_TRUE( fill( pair.server ) );

    std::vector<uint8_t> message( 256u * 1024u, 0xA5u );
    const Clock::time_point start{ Clock::now( ) };
    ssize_t written{ 0 };
    do
    {
        written = ::send( pair.server, message.data( ), message.size( ), MSG_DONTWAIT );
    } while ( ( written < 0 ) && ( errno == EINTR ) );

    const int64_t took{ elapsed_ms( start ) };
    EXPECT_LT( took, PROMPT_MS ) << "the send blocked for " << took << " ms";
    if ( written < 0 )
    {
        EXPECT_TRUE( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) );
    }

    close_pair( pair );
}

/**
 * \brief   A child process inherits neither a created nor an accepted socket.
 **/
TEST( SocketWouldBlockTest, SocketsAreClosedOnExec )
{
    const SOCKETHANDLE created{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( created ) );
    EXPECT_NE( ::fcntl( static_cast<int>(created), F_GETFD ) & FD_CLOEXEC, 0 );
    areg::socket_close( created );

    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    EXPECT_NE( ::fcntl( pair.server, F_GETFD ) & FD_CLOEXEC, 0 );
    close_pair( pair );
}

/**
 * \brief   The blocking sends deliver everything on a non-blocking socket to a slow reader.
 **/
TEST( SocketWouldBlockTest, BlockingSendsCompleteOnNonBlockingSocket )
{
    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.server), SEND_TIMEOUT_MS ) );
    ASSERT_TRUE( set_nonblocking( pair.server, true ) );
    shrink_buffers( pair );

    const std::vector<uint8_t> data{ make_pattern( LARGE_BYTES ) };
    const SOCKETHANDLE hSocket{ static_cast<SOCKETHANDLE>(pair.server) };

    // One contiguous send, then the same bytes as 4 KB pieces (staging copy) and as 128 KB pieces (writev).
    for ( uint32_t round = 0u; round < 3u; ++ round )
    {
        std::atomic<uint32_t> received{ 0u };
        std::atomic<bool> intact{ true };
        std::thread reader( slow_reader, pair.client, LARGE_BYTES, std::ref( received ), std::ref( intact ) );

        int32_t sent{ -1 };
        if ( round == 0u )
        {
            sent = areg::send_data( hSocket, data.data( ), LARGE_BYTES );
        }
        else
        {
            const uint32_t pieces{ round == 1u ? 2048u : 64u };
            const uint32_t piece{ LARGE_BYTES / pieces };
            std::vector<areg::IoBuffer> buffers;
            for ( uint32_t i = 0u; i < pieces; ++ i )
                buffers.push_back( { data.data( ) + i * piece, piece } );

            sent = 0;
            for ( uint32_t first = 0u; ( first < pieces ) && ( sent >= 0 ); first += areg::DEFAULT_DRAIN_LIMIT )
            {
                const uint32_t count{ ( pieces - first ) < areg::DEFAULT_DRAIN_LIMIT ? ( pieces - first ) : areg::DEFAULT_DRAIN_LIMIT };
                const int32_t part{ areg::send_data_v( hSocket, buffers.data( ) + first, count, count * piece ) };
                sent = ( part < 0 ) ? -1 : sent + part;
            }
        }

        // A failed send ends the stream, so that the reader does not wait for the missing bytes.
        if ( sent != static_cast<int32_t>(LARGE_BYTES) )
            static_cast<void>(::shutdown( pair.server, SHUT_WR ));

        reader.join( );
        EXPECT_EQ( sent, static_cast<int32_t>(LARGE_BYTES) ) << "round " << round;
        EXPECT_EQ( received.load( ), LARGE_BYTES ) << "round " << round;
        EXPECT_TRUE( intact.load( ) ) << "round " << round;
        if ( sent != static_cast<int32_t>(LARGE_BYTES) )
            break;
    }

    close_pair( pair );
}

/**
 * \brief   The blocking receives wait for late data on a non-blocking socket, in every receive mode.
 **/
TEST( SocketWouldBlockTest, BlockingReceivesWaitOnNonBlockingSocket )
{
    const areg::ReceiveMode savedMode{ areg::receive_mode( ) };
    const std::vector<uint8_t> small{ make_pattern( 4096u ) };
    const std::vector<uint8_t> large{ make_pattern( 2u * areg::DEFAULT_THREAD_CACHE ) };

    struct Case
    {
        areg::ReceiveMode           mode;
        bool                        window;
        const std::vector<uint8_t> *data;
    };

    const Case cases[]
    {
          { areg::ReceiveMode::NoCache  , false, &small }
        , { areg::ReceiveMode::MonoCache, false, &small }
        , { areg::ReceiveMode::MonoCache, false, &large }
        , { areg::ReceiveMode::NoCache  , true , &large }
    };

    for ( const Case & entry : cases )
    {
        Pair pair;
        ASSERT_TRUE( open_pair( pair ) );
        ASSERT_TRUE( set_nonblocking( pair.server, true ) );
        areg::set_receive_mode( entry.mode );

        const std::vector<uint8_t> & data{ *entry.data };
        std::vector<uint8_t> got( data.size( ), 0u );
        std::thread writer{ late_writer( pair.client, data ) };

        const SOCKETHANDLE hSocket{ static_cast<SOCKETHANDLE>(pair.server) };
        const int32_t received{ entry.window
                              ? areg::receive_data_window( hSocket, got.data( ), static_cast<uint32_t>(got.size( )) )
                              : areg::receive_data( hSocket, got.data( ), static_cast<uint32_t>(got.size( )) ) };
        writer.join( );

        EXPECT_EQ( received, static_cast<int32_t>(data.size( )) ) << "mode " << static_cast<int>(entry.mode) << ", window " << entry.window << ", bytes " << data.size( );
        EXPECT_TRUE( got == data ) << "mode " << static_cast<int>(entry.mode) << ", window " << entry.window << ", bytes " << data.size( );

        close_pair( pair );
    }

    areg::set_receive_mode( savedMode );
}

/**
 * \brief   A blocking send on a non-blocking socket still fails after the send timeout
 *          when the peer never reads.
 **/
TEST( SocketWouldBlockTest, NonBlockingSendStillTimesOut )
{
    constexpr uint32_t TIMEOUT_MS{ 200u };

    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.server), TIMEOUT_MS ) );
    ASSERT_TRUE( set_nonblocking( pair.server, true ) );

    const std::vector<uint8_t> data( 64u * 1024u * 1024u, 0x3Cu );
    const int server{ pair.server };
    const Clock::time_point start{ Clock::now( ) };
    std::future<int32_t> send{ std::async( std::launch::async, [server, &data]( )
        {
            return areg::send_data( static_cast<SOCKETHANDLE>(server), data.data( ), static_cast<uint32_t>(data.size( )) );
        } ) };

    const bool returned{ send.wait_for( std::chrono::milliseconds( DEADLINE_MS ) ) == std::future_status::ready };
    const int64_t took{ elapsed_ms( start ) };
    if ( returned == false )
        static_cast<void>(::shutdown( pair.server, SHUT_RDWR ));

    const int32_t sent{ send.get( ) };
    EXPECT_TRUE( returned );
    EXPECT_LT( sent, 0 );
    EXPECT_GE( took, static_cast<int64_t>(TIMEOUT_MS) / 2 );

    close_pair( pair );
}

namespace
{
    struct ScopedPair : Pair
    {
        ~ScopedPair()
        {
            close_pair(*this);
        }
    };

    class ScopedReceiveMode
    {
    public:
        explicit ScopedReceiveMode(areg::ReceiveMode mode)
            : mSaved(areg::receive_mode())
        {
            areg::set_receive_mode(mode);
        }

        ~ScopedReceiveMode()
        {
            areg::set_receive_mode(mSaved);
        }

    private:
        areg::ReceiveMode mSaved;
    };
}

// Mixed scatter/gather buffers arrive in order without an explicit total-size hint.
TEST(SocketWouldBlockCoverageTest, mixed_scatter_gather_buffers_arrive_in_order_without_a_size_hint)
{
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    ASSERT_TRUE(areg::set_send_timeout(pair.server, 30'000u));
    ASSERT_NE(areg::thread_tx_cache().cache(), nullptr);
    const uint32_t large{ areg::thread_tx_cache().space + 1u };
    const std::vector<uint8_t> data{ make_pattern(large + 8u) };
    const areg::IoBuffer buffers[]
    {
          { data.data(), 3u }
        , { data.data() + 3u, large }
        , { data.data() + large + 3u, 5u }
    };
    std::vector<uint8_t> received(data.size());
    std::future<int32_t> reader{ std::async(std::launch::async, [&]()
        {
            return areg::receive_data_window(pair.client, received.data(), static_cast<uint32_t>(received.size()));
        }) };
    const int32_t sent{ areg::send_data_v(pair.server, buffers, 3u) };
    if (sent != static_cast<int32_t>(data.size()))
        areg::socket_interrupt(pair.client);

    const bool ready{ reader.wait_for(std::chrono::seconds(30)) == std::future_status::ready };
    if (!ready)
        areg::socket_interrupt(pair.client);

    const int32_t got{ reader.get() };
    EXPECT_TRUE(ready);
    EXPECT_EQ(sent, static_cast<int32_t>(data.size()));
    EXPECT_EQ(got, sent);
    EXPECT_EQ(received, data);
}

// Nonblocking scatter/gather sends deliver the complete large payload.
TEST(SocketWouldBlockCoverageTest, try_scatter_gather_completes_the_entire_large_payload)
{
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    ASSERT_TRUE(areg::set_send_timeout(pair.server, 30'000u));
    ASSERT_EQ(areg::set_send_size(pair.server, areg::PACKET_MIN_SIZE), areg::PACKET_MIN_SIZE);
    const std::vector<uint8_t> data{ make_pattern(LARGE_BYTES) };
    const areg::IoBuffer buffers[]
    {
          { data.data(), 4096u }
        , { data.data() + 4096u, LARGE_BYTES / 2u }
        , { data.data() + 4096u + LARGE_BYTES / 2u, LARGE_BYTES / 2u - 4096u }
    };
    std::vector<uint8_t> received(data.size());
    std::future<int32_t> reader{ std::async(std::launch::async, [&]()
        {
            return areg::receive_data_window(pair.client, received.data(), LARGE_BYTES);
        }) };
    const int32_t sent{ areg::try_send_data_v(pair.server, buffers, 3u) };
    if (sent != static_cast<int32_t>(data.size()))
        areg::socket_interrupt(pair.client);

    const bool ready{ reader.wait_for(std::chrono::seconds(30)) == std::future_status::ready };
    if (!ready)
        areg::socket_interrupt(pair.client);

    const int32_t got{ reader.get() };
    EXPECT_TRUE(ready);
    EXPECT_EQ(sent, static_cast<int32_t>(data.size()));
    EXPECT_EQ(got, sent);
    EXPECT_EQ(received, data);
}

// A nonblocking receive drains cached bytes before reporting the peer's close.
TEST(SocketWouldBlockCoverageTest, nowait_receive_preserves_cached_bytes_when_the_peer_closes)
{
    ScopedReceiveMode mode(areg::ReceiveMode::MultiCache);
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(pair.server, true));
    uint8_t received[16]{};
    EXPECT_EQ(areg::receive_data_nowait(pair.server, received, sizeof(received)), 0);
    EXPECT_EQ(areg::recv_fill_nowait(pair.server), 0);
    const uint8_t data[]{ 0u, 1u, 2u, 3u, 4u, 5u, 6u, 255u };
    ASSERT_EQ(areg::send_data(pair.client, data, sizeof(data)), static_cast<int32_t>(sizeof(data)));
    areg::ThreadCache& cache{ areg::thread_rx_cache(pair.server) };
    for (uint32_t attempt = 0u; (cache.unread < sizeof(data)) && (attempt < sizeof(data)); ++attempt)
    {
        ASSERT_EQ(mux.wait(30'000), pair.server);
        ASSERT_GT(areg::recv_fill_nowait(pair.server), 0);
    }

    ASSERT_EQ(cache.unread, sizeof(data));
    ASSERT_EQ(areg::receive_data_nowait(pair.server, received, 3u), 3);
    EXPECT_EQ(std::memcmp(received, data, 3u), 0);
    EXPECT_EQ(cache.unread, 5u);
    ASSERT_TRUE(areg::disable_send(pair.client));
    ASSERT_EQ(mux.wait(30'000), pair.server);
    ASSERT_EQ(areg::receive_data_nowait(pair.server, received + 3u, 8u), 5);
    EXPECT_EQ(std::memcmp(received, data, sizeof(data)), 0);
    EXPECT_EQ(cache.head, 0u);
    EXPECT_EQ(cache.unread, 0u);
    EXPECT_LT(areg::receive_data_nowait(pair.server, received, sizeof(received)), 0);
}

// Refilling the receive cache compacts unread bytes without exceeding capacity.
TEST(SocketWouldBlockCoverageTest, nowait_fill_compacts_unread_bytes_and_respects_the_cache_capacity)
{
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    areg::ThreadCache cache;
    cache.space = 8u;
    cache.buffer.reset(new (std::nothrow) uint8_t[cache.space]);
    ASSERT_NE(cache.buffer, nullptr);
    cache.head = 3u;
    cache.unread = 3u;
    cache.buffer[3] = 1u;
    cache.buffer[4] = 2u;
    cache.buffer[5] = 3u;
    const uint8_t tail[]{ 4u, 5u, 6u, 7u, 8u };
    ASSERT_EQ(areg::send_data(pair.client, tail, sizeof(tail)), static_cast<int32_t>(sizeof(tail)));
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(pair.server, true));
    for (uint32_t attempt = 0u; (cache.unread < cache.space) && (attempt < sizeof(tail)); ++attempt)
    {
        ASSERT_EQ(mux.wait(30'000), pair.server);
        ASSERT_GT(areg::recv_fill_nowait(pair.server, cache), 0);
    }

    ASSERT_EQ(cache.unread, cache.space);
    EXPECT_EQ(cache.head, 0u);
    const uint8_t expected[]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
    EXPECT_EQ(std::memcmp(cache.buffer.get(), expected, sizeof(expected)), 0);
    EXPECT_EQ(areg::recv_fill_nowait(pair.server, cache), 0);
    EXPECT_EQ(cache.unread, cache.space);
}

// A receive cache must never expose bytes belonging to another live connection.
TEST(SocketWouldBlockCoverageTest, mono_cache_does_not_expose_another_live_connections_bytes)
{
    ScopedReceiveMode mode(areg::ReceiveMode::MonoCache);
    ScopedPair first;
    ScopedPair second;
    ASSERT_TRUE(open_pair(first));
    ASSERT_TRUE(open_pair(second));
    areg::ThreadCache& cache{ areg::thread_rx_cache(first.server) };
    cache.head = 0u;
    cache.unread = 0u;
    const uint8_t data[]{ 0x11u, 0x22u, 0x33u, 0x44u };
    ASSERT_EQ(areg::send_data(first.client, data, sizeof(data)), static_cast<int32_t>(sizeof(data)));
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(first.server, true));
    for (uint32_t attempt = 0u; (cache.unread < sizeof(data)) && (attempt < sizeof(data)); ++attempt)
    {
        ASSERT_EQ(mux.wait(30'000), first.server);
        ASSERT_GT(areg::recv_fill_nowait(first.server), 0);
    }

    ASSERT_EQ(cache.unread, sizeof(data));
    uint8_t received[sizeof(data)]{};
    const int32_t got{ areg::receive_data_nowait(second.server, received, sizeof(received)) };
    cache.head = 0u;
    cache.unread = 0u;
    EXPECT_EQ(got, 0);
}

// Releasing the socket of the mono cache drops its unread bytes, so a reused handle starts empty.
TEST(SocketWouldBlockCoverageTest, mono_cache_release_drops_the_unread_bytes_of_the_socket)
{
    ScopedReceiveMode mode(areg::ReceiveMode::MonoCache);
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    areg::ThreadCache& cache{ areg::thread_rx_cache(pair.server) };
    cache.head = 0u;
    cache.unread = 0u;
    const uint8_t data[]{ 0x11u, 0x22u };
    ASSERT_EQ(areg::send_data(pair.client, data, sizeof(data)), static_cast<int32_t>(sizeof(data)));
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(pair.server, true));
    for (uint32_t attempt = 0u; (cache.unread < sizeof(data)) && (attempt < sizeof(data)); ++attempt)
    {
        ASSERT_EQ(mux.wait(30'000), pair.server);
        ASSERT_GT(areg::recv_fill_nowait(pair.server), 0);
    }

    ASSERT_EQ(cache.unread, sizeof(data));
    areg::thread_rx_cache_release(pair.server);
    EXPECT_EQ(cache.unread, 0u);
    EXPECT_EQ(areg::recv_data_available(pair.server), 0u);
    uint8_t received[sizeof(data)]{};
    EXPECT_EQ(areg::receive_data_nowait(pair.server, received, sizeof(received)), 0);
}

#if GTEST_HAS_DEATH_TEST
// Sending a large scatter/gather payload to a closed peer must report an error without SIGPIPE.
TEST(SocketWouldBlockCoverageTest, large_scatter_gather_send_reports_error_instead_of_sigpipe)
{
    ScopedPair pair;
    ASSERT_TRUE(open_pair(pair));
    ASSERT_TRUE(areg::disable_send(pair.server));
    const std::vector<uint8_t> data(2u * areg::MIN_BIG_BLOCK, 0x5Au);
    const areg::IoBuffer buffers[]
    {
          { data.data(), areg::MIN_BIG_BLOCK }
        , { data.data() + areg::MIN_BIG_BLOCK, areg::MIN_BIG_BLOCK }
    };
    ASSERT_EXIT(
        {
            std::signal(SIGPIPE, SIG_DFL);
            const int32_t sent{ areg::send_data_v(pair.server, buffers, 2u, static_cast<uint32_t>(data.size())) };
            ::_exit(sent < 0 ? 0 : 1);
        }
        , testing::ExitedWithCode(0)
        , "");
}
#endif

#elif defined(WINDOWS)

#include <WinSock2.h>

#ifdef _MSC_VER
    #pragma comment(lib, "ws2_32")
#endif  // _MSC_VER

namespace
{
    //!< Returns true if a child process would inherit the socket.
    bool is_inherited( SOCKETHANDLE hSocket )
    {
        DWORD flags{ 0 };
        return ( ::GetHandleInformation( reinterpret_cast<HANDLE>(hSocket), &flags ) != FALSE ) && ( ( flags & HANDLE_FLAG_INHERIT ) != 0 );
    }
}

/**
 * \brief   A child process inherits neither a created nor an accepted socket.
 **/
TEST( SocketWouldBlockTest, SocketsAreClosedOnExec )
{
    ASSERT_TRUE( areg::socket_initialize( ) );
    const SOCKETHANDLE created{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( created ) );
    EXPECT_FALSE( is_inherited( created ) );
    areg::socket_close( created );

    const SOCKET listener{ ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) };
    ASSERT_NE( listener, INVALID_SOCKET );
    sockaddr_in addr{ };
    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = ::htonl( INADDR_LOOPBACK );
    addr.sin_port           = 0;
    int len{ static_cast<int>(sizeof( addr )) };
    ASSERT_EQ( ::bind( listener, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ), 0 );
    ASSERT_EQ( ::listen( listener, 1 ), 0 );
    ASSERT_EQ( ::getsockname( listener, reinterpret_cast<sockaddr *>(&addr), &len ), 0 );

    const SOCKET client{ ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) };
    ASSERT_NE( client, INVALID_SOCKET );
    ASSERT_EQ( ::connect( client, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ), 0 );

    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
    const SOCKETHANDLE accepted{ areg::server_accept( static_cast<SOCKETHANDLE>(listener), &none, 0 ) };
    ASSERT_TRUE( areg::is_valid_socket( accepted ) );
    EXPECT_FALSE( is_inherited( accepted ) );

    areg::socket_close( accepted );
    ::closesocket( client );
    ::closesocket( listener );
    areg::socket_release( );
}

#endif  // defined(_POSIX) / defined(WINDOWS)
