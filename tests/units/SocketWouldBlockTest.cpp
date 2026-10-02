/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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

#if defined(_POSIX)

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< Milliseconds a call that must not block may take.
    constexpr int64_t   PROMPT_MS       { 500 };

    //!< Send timeout of the sockets, the value the framework uses.
    constexpr uint32_t  SEND_TIMEOUT_MS { 2500u };

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

    //!< Writes into the socket until its send buffer is full. Restores the blocking mode it found.
    bool fill( int sock )
    {
        const bool wasNonBlocking{ is_nonblocking( sock ) };
        if ( ( wasNonBlocking == false ) && ( set_nonblocking( sock, true ) == false ) )
            return false;

        std::vector<uint8_t> chunk( 64u * 1024u, 0x5Au );
        uint64_t total{ 0u };
        for ( ;; )
        {
            const ssize_t written{ ::send( sock, chunk.data( ), chunk.size( ), 0 ) };
            if ( written > 0 )
            {
                total += static_cast<uint64_t>(written);
                continue;
            }

            if ( ( written < 0 ) && ( errno == EINTR ) )
                continue;

            break;
        }

        const bool full{ ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ) };
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
 * \brief   A try-send on a blocking socket whose send buffer is full returns 0 at once,
 *          not after the send timeout.
 **/
TEST( SocketWouldBlockTest, TrySendOnFullSocketReturnsAtOnce )
{
    Pair pair;
    ASSERT_TRUE( open_pair( pair ) );
    ASSERT_TRUE( areg::set_send_timeout( static_cast<SOCKETHANDLE>(pair.client), SEND_TIMEOUT_MS ) );
    ASSERT_FALSE( is_nonblocking( pair.client ) );
    ASSERT_TRUE( fill( pair.client ) );

    uint8_t message[64]{ };
    const areg::IoBuffer buffer{ message, sizeof( message ) };
    const Clock::time_point start{ Clock::now( ) };
    const int32_t sent{ areg::try_send_data_v( static_cast<SOCKETHANDLE>(pair.client), &buffer, 1u, sizeof( message ) ) };
    const int64_t took{ elapsed_ms( start ) };

    EXPECT_EQ( sent, 0 );
    EXPECT_LT( took, PROMPT_MS ) << "the try-send blocked for " << took << " ms";

    close_pair( pair );
}

/**
 * \brief   A send with MSG_DONTWAIT on a full accepted socket returns at once.
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
    const Clock::time_point start{ Clock::now( ) };
    const int32_t sent{ areg::send_data( static_cast<SOCKETHANDLE>(pair.server), data.data( ), static_cast<uint32_t>(data.size( )) ) };
    const int64_t took{ elapsed_ms( start ) };

    EXPECT_LT( sent, 0 );
    EXPECT_GE( took, static_cast<int64_t>(TIMEOUT_MS) / 2 );
    EXPECT_LT( took, 10 * static_cast<int64_t>(TIMEOUT_MS) );

    close_pair( pair );
}

#endif  // defined(_POSIX)
