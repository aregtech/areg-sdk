/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_os_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the POSIX socket, multiplexer and lock layer when a
 *              system call fails or is interrupted. Each test arms a fault point in a child
 *              process and checks what the caller sees and that the next call works.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if defined(__linux__)

#include "areg/base/FatalError.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/private/SocketLiveness.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

namespace
{
    //!< The message of a fault point that did not fail any call.
    constexpr const char * NOT_REPLACED{ "the fault library did not replace the system call" };

    //!< Bytes larger than the send and receive buffers of a loopback pair together.
    constexpr uint32_t  LARGE_BYTES { 8u * 1024u * 1024u };

    //!< A connected TCP loopback pair of plain descriptors.
    struct LoopPair
    {
        SOCKETHANDLE    client  { areg::InvalidSocketHandle };
        SOCKETHANDLE    server  { areg::InvalidSocketHandle };

        //!< Opens the pair. Returns false when the system refuses.
        bool open( )
        {
            const int listener{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
            if ( listener < 0 )
                return false;

            sockaddr_in addr{ };
            addr.sin_family      = AF_INET;
            addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            addr.sin_port        = 0;
            socklen_t len{ sizeof( addr ) };
            bool result{ false };
            if ( ( ::bind( listener, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0 )
              && ( ::listen( listener, 4 ) == 0 )
              && ( ::getsockname( listener, reinterpret_cast<sockaddr *>(&addr), &len ) == 0 ) )
            {
                const int sock{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
                client = static_cast<SOCKETHANDLE>(sock);
                if ( ( sock >= 0 ) && ( ::connect( sock, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0 ) )
                {
                    const int accepted{ ::accept( listener, nullptr, nullptr ) };
                    server = static_cast<SOCKETHANDLE>(accepted);
                    result = ( accepted >= 0 );
                }
            }

            ::close( listener );
            return result;
        }

        //!< Closes both ends.
        void close( )
        {
            areg::socket_close( client );
            areg::socket_close( server );
            client = areg::InvalidSocketHandle;
            server = areg::InvalidSocketHandle;
        }

        ~LoopPair( )
        {
            close( );
        }
    };

    //!< A listening loopback socket and its address.
    struct LoopListener
    {
        SOCKETHANDLE        handle  { areg::InvalidSocketHandle };
        areg::SocketAddress address { };

        //!< Opens the listener. Returns false when the system refuses.
        bool open( )
        {
            const int sock{ ::socket( AF_INET, SOCK_STREAM, 0 ) };
            handle = static_cast<SOCKETHANDLE>(sock);
            if ( sock < 0 )
                return false;

            sockaddr_in addr{ };
            addr.sin_family      = AF_INET;
            addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            addr.sin_port        = 0;
            socklen_t len{ sizeof( addr ) };
            if ( ( ::bind( sock, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) != 0 )
              || ( ::listen( sock, 8 ) != 0 )
              || ( ::getsockname( sock, reinterpret_cast<sockaddr *>(&addr), &len ) != 0 ) )
            {
                return false;
            }

            address.from_sockaddr( addr );
            return true;
        }

        ~LoopListener( )
        {
            areg::socket_close( handle );
        }
    };

    //!< Sets or clears O_NONBLOCK on the socket.
    bool set_nonblocking( SOCKETHANDLE sock, bool nonBlocking )
    {
        const int flags{ ::fcntl( static_cast<int>(sock), F_GETFL, 0 ) };
        if ( flags == -1 )
            return false;

        const int wanted{ nonBlocking ? ( flags | O_NONBLOCK ) : ( flags & ~O_NONBLOCK ) };
        return ( ::fcntl( static_cast<int>(sock), F_SETFL, wanted ) != -1 );
    }

    //!< Receives exactly 'length' bytes from the plain descriptor. Returns false on an error or end of stream.
    bool read_exact( SOCKETHANDLE sock, uint8_t * buffer, uint32_t length )
    {
        uint32_t total{ 0u };
        while ( total < length )
        {
            const ssize_t got{ ::recv( static_cast<int>(sock), buffer + total, length - total, 0 ) };
            if ( got <= 0 )
                return false;

            total += static_cast<uint32_t>(got);
        }

        return true;
    }

    //!< Sends 4 bytes and checks the peer receives them.
    bool round_trip( SOCKETHANDLE from, SOCKETHANDLE to )
    {
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        uint8_t received[4]{ };
        return ( areg::send_data( from, data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) )
            && read_exact( to, received, sizeof( received ) )
            && ( std::memcmp( data, received, sizeof( data ) ) == 0 );
    }

    //!< Sets the receive mode of the calling thread and restores the previous one.
    class ModeScope
    {
    public:
        explicit ModeScope( areg::ReceiveMode mode )
            : mSaved( areg::receive_mode( ) )
        {
            areg::set_receive_mode( mode );
        }

        ~ModeScope( )
        {
            areg::set_receive_mode( mSaved );
        }

    private:
        areg::ReceiveMode mSaved;
    };
}

//////////////////////////////////////////////////////////////////////////
// Send
//////////////////////////////////////////////////////////////////////////

namespace
{
    void send_data_repeats_an_interrupted_send_and_delivers_the_data( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EINTR ) );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, send_data_repeats_an_interrupted_send_and_delivers_the_data )
{
    AREG_EXPECT_IN_CHILD( send_data_repeats_an_interrupted_send_and_delivers_the_data( ) );
}

namespace
{
    void send_data_fails_when_a_blocking_socket_would_block( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        AREG_CHILD_CHECK( areg::send_data( pair.client, data, sizeof( data ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
    }
}

TEST( FaultInjectOsTest, send_data_fails_when_a_blocking_socket_would_block )
{
    AREG_EXPECT_IN_CHILD( send_data_fails_when_a_blocking_socket_would_block( ) );
}

namespace
{
    void send_data_fails_when_the_socket_flags_cannot_be_read_after_would_block( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, EBADF ) );
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        AREG_CHILD_CHECK( areg::send_data( pair.client, data, sizeof( data ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
    }
}

TEST( FaultInjectOsTest, send_data_fails_when_the_socket_flags_cannot_be_read_after_would_block )
{
    AREG_EXPECT_IN_CHILD( send_data_fails_when_the_socket_flags_cannot_be_read_after_would_block( ) );
}

namespace
{
    void send_data_on_a_non_blocking_socket_waits_through_an_interrupted_poll( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, EINTR ) );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, send_data_on_a_non_blocking_socket_waits_through_an_interrupted_poll )
{
    AREG_EXPECT_IN_CHILD( send_data_on_a_non_blocking_socket_waits_through_an_interrupted_poll( ) );
}

namespace
{
    void send_data_on_a_non_blocking_socket_fails_when_the_wait_fails( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, ENOMEM ) );
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        AREG_CHILD_CHECK( areg::send_data( pair.client, data, sizeof( data ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
    }
}

TEST( FaultInjectOsTest, send_data_on_a_non_blocking_socket_fails_when_the_wait_fails )
{
    AREG_EXPECT_IN_CHILD( send_data_on_a_non_blocking_socket_fails_when_the_wait_fails( ) );
}

namespace
{
    void send_data_on_a_non_blocking_socket_waits_without_a_timeout_when_it_cannot_be_read( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        AREG_CHILD_CHECK( areg::set_send_timeout( pair.client, 1500u ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, EBADF ) );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, send_data_on_a_non_blocking_socket_waits_without_a_timeout_when_it_cannot_be_read )
{
    AREG_EXPECT_IN_CHILD( send_data_on_a_non_blocking_socket_waits_without_a_timeout_when_it_cannot_be_read( ) );
}

namespace
{
    void send_data_on_a_non_blocking_socket_waits_within_a_sub_second_and_a_mixed_timeout( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        areg_fault::FaultScope faults;
        for ( const uint32_t timeoutMs : { 500u, 1500u } )
        {
            AREG_CHILD_CHECK( areg::set_send_timeout( pair.client, timeoutMs ) );
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
            AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        }
    }
}

TEST( FaultInjectOsTest, send_data_on_a_non_blocking_socket_waits_within_a_sub_second_and_a_mixed_timeout )
{
    AREG_EXPECT_IN_CHILD( send_data_on_a_non_blocking_socket_waits_within_a_sub_second_and_a_mixed_timeout( ) );
}

namespace
{
    void try_send_repeats_an_interrupted_send_and_reports_zero_when_it_would_block( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint8_t first[3]{ 1u, 2u, 3u };
        const uint8_t second[2]{ 4u, 5u };
        const areg::IoBuffer buffers[2]{ { first, sizeof( first ) }, { second, sizeof( second ) } };

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 0u ) == 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EINTR ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 0u ) == 5 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        uint8_t received[5]{ };
        AREG_CHILD_CHECK( read_exact( pair.server, received, sizeof( received ) ) );
        AREG_CHILD_CHECK( ( received[0] == 1u ) && ( received[4] == 5u ) );
    }
}

TEST( FaultInjectOsTest, try_send_repeats_an_interrupted_send_and_reports_zero_when_it_would_block )
{
    AREG_EXPECT_IN_CHILD( try_send_repeats_an_interrupted_send_and_reports_zero_when_it_would_block( ) );
}

namespace
{
    void try_send_reports_an_error_when_the_rest_of_a_partly_sent_buffer_fails( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const std::vector<uint8_t> large( LARGE_BYTES, 0x5Au );
        const uint8_t small[16]{ };
        const areg::IoBuffer buffers[2]{ { large.data( ), large.size( ) }, { small, sizeof( small ) } };

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET, 1u ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 0u ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, try_send_reports_an_error_when_the_rest_of_a_partly_sent_buffer_fails )
{
    AREG_EXPECT_IN_CHILD( try_send_reports_an_error_when_the_rest_of_a_partly_sent_buffer_fails( ) );
}

namespace
{
    void try_send_reports_an_error_when_a_later_buffer_of_a_partial_send_fails( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const std::vector<uint8_t> large( LARGE_BYTES, 0x5Au );
        const uint8_t small[16]{ };
        const areg::IoBuffer buffers[2]{ { large.data( ), large.size( ) }, { small, sizeof( small ) } };

        std::atomic<bool> stop{ false };
        const SOCKETHANDLE server{ pair.server };
        std::thread reader( [server, &stop]( )
            {
                std::vector<uint8_t> sink( 64u * 1024u );
                while ( ( stop.load( ) == false ) && ( ::recv( static_cast<int>(server), sink.data( ), sink.size( ), 0 ) > 0 ) )
                {
                }
            } );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET, 2u ) );
        const int32_t result{ areg::try_send_data_v( pair.client, buffers, 2u, 0u ) };
        const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::SocketSend ) };
        stop.store( true );
        areg::socket_interrupt( pair.server );
        reader.join( );
        AREG_CHILD_CHECK( result < 0 );
        AREG_CHILD_CHECK( failed == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, try_send_reports_an_error_when_a_later_buffer_of_a_partial_send_fails )
{
    AREG_EXPECT_IN_CHILD( try_send_reports_an_error_when_a_later_buffer_of_a_partial_send_fails( ) );
}

namespace
{
    void scatter_send_of_large_blocks_repeats_an_interrupted_send_and_fails_on_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const std::vector<uint8_t> first( areg::MIN_BIG_BLOCK, 0x11u );
        const std::vector<uint8_t> second( areg::MIN_BIG_BLOCK, 0x22u );
        const areg::IoBuffer buffers[2]{ { first.data( ), first.size( ) }, { second.data( ), second.size( ) } };
        const uint32_t total{ 2u * areg::MIN_BIG_BLOCK };

        std::vector<uint8_t> received( total );
        std::atomic<bool> got{ false };
        const SOCKETHANDLE server{ pair.server };
        std::thread reader( [server, &received, &got, total]( )
            {
                got.store( read_exact( server, received.data( ), total ) );
            } );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EINTR ) );
        const int32_t sent{ areg::send_data_v( pair.client, buffers, 2u, total ) };
        const uint32_t interrupted{ areg_fault::failed( areg_fault::FaultPoint::SocketSend ) };
        reader.join( );
        AREG_CHILD_CHECK( sent == static_cast<int32_t>(total) );
        AREG_CHILD_CHECK( interrupted == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( got.load( ) && ( received[0] == 0x11u ) && ( received[total - 1u] == 0x22u ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, buffers, 2u, total ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, scatter_send_of_large_blocks_repeats_an_interrupted_send_and_fails_on_an_error )
{
    AREG_EXPECT_IN_CHILD( scatter_send_of_large_blocks_repeats_an_interrupted_send_and_fails_on_an_error( ) );
}

namespace
{
    void scatter_send_of_small_blocks_fails_at_every_flush_of_the_staging_buffer( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint32_t space{ ( areg::thread_tx_cache( ).cache( ) != nullptr ) ? areg::thread_tx_cache( ).space : 0u };
        AREG_CHILD_CHECK( space > 4u * 1024u );

        const std::vector<uint8_t> big( space + 1u, 0x33u );
        const std::vector<uint8_t> most( ( space / 4u ) * 3u, 0x44u );
        const uint8_t tiny[1]{ 0x55u };
        const areg::IoBuffer flushLast[2]{ { tiny, 1u }, { tiny, 1u } };
        const areg::IoBuffer bigFirst[12]{ { big.data( ), big.size( ) }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }
                                         , { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u } };
        const areg::IoBuffer bigAfter[12]{ { tiny, 1u }, { big.data( ), big.size( ) }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }
                                         , { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u } };
        const areg::IoBuffer overflow[12]{ { most.data( ), most.size( ) }, { most.data( ), most.size( ) }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }
                                         , { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u }, { tiny, 1u } };
        const uint32_t bigTotal{ static_cast<uint32_t>(big.size( )) + 11u };
        const uint32_t overflowTotal{ 2u * static_cast<uint32_t>(most.size( )) + 10u };
        AREG_CHILD_CHECK( ( bigTotal / 12u ) < areg::MIN_BIG_BLOCK );
        AREG_CHILD_CHECK( ( overflowTotal / 12u ) < areg::MIN_BIG_BLOCK );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, flushLast, 2u, 2u ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, bigFirst, 12u, bigTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, bigAfter, 12u, bigTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, overflow, 12u, overflowTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::send_data_v( pair.client, flushLast, 2u, 2u ) == 2 );
        uint8_t received[2]{ };
        AREG_CHILD_CHECK( read_exact( pair.server, received, sizeof( received ) ) && ( received[1] == 0x55u ) );
    }
}

TEST( FaultInjectOsTest, scatter_send_of_small_blocks_fails_at_every_flush_of_the_staging_buffer )
{
    AREG_EXPECT_IN_CHILD( scatter_send_of_small_blocks_fails_at_every_flush_of_the_staging_buffer( ) );
}

//////////////////////////////////////////////////////////////////////////
// Receive
//////////////////////////////////////////////////////////////////////////

namespace
{
    void receive_repeats_an_interrupted_receive_in_every_receive_mode( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        for ( const areg::ReceiveMode mode : { areg::ReceiveMode::NoCache, areg::ReceiveMode::MonoCache } )
        {
            ModeScope scope( mode );
            const uint8_t data[4]{ 9u, 8u, 7u, 6u };
            AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EINTR ) );
            uint8_t received[4]{ };
            AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );
        }
    }
}

TEST( FaultInjectOsTest, receive_repeats_an_interrupted_receive_in_every_receive_mode )
{
    AREG_EXPECT_IN_CHILD( receive_repeats_an_interrupted_receive_in_every_receive_mode( ) );
}

namespace
{
    void cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::MonoCache );
        areg::ThreadCache & cache{ areg::thread_rx_cache( pair.client ) };
        AREG_CHILD_CHECK( cache.cache( ) != nullptr );
        const uint32_t length{ cache.space };
        const std::vector<uint8_t> data( length, 0x66u );
        std::vector<uint8_t> received( length );

        std::thread writer( [&pair, &data]( )
            {
                static_cast<void>(areg::send_data( pair.server, data.data( ), static_cast<uint32_t>(data.size( )) ));
            } );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EINTR ) );
        const int32_t got{ areg::receive_data( pair.client, received.data( ), length ) };
        const uint32_t interrupted{ areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) };
        writer.join( );
        AREG_CHILD_CHECK( got == static_cast<int32_t>(length) );
        AREG_CHILD_CHECK( interrupted == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( received[length - 1u] == 0x66u );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received.data( ), length ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error )
{
    AREG_EXPECT_IN_CHILD( cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error( ) );
}

namespace
{
    void receive_fails_when_a_blocking_socket_times_out_without_a_live_handle( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::NoCache );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EAGAIN ) );
        uint8_t received[4]{ };
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.server, pair.client ) );
    }
}

TEST( FaultInjectOsTest, receive_fails_when_a_blocking_socket_times_out_without_a_live_handle )
{
    AREG_EXPECT_IN_CHILD( receive_fails_when_a_blocking_socket_times_out_without_a_live_handle( ) );
}

namespace
{
    void receive_with_a_live_handle_waits_on_a_non_blocking_socket_and_fails_on_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::NoCache );
        std::atomic<SOCKETHANDLE> live{ pair.client };
        areg::set_receive_live_handle( &live );
        const uint8_t data[4]{ 1u, 3u, 5u, 7u };
        uint8_t received[4]{ };

        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EAGAIN ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );

        AREG_CHILD_CHECK( set_nonblocking( pair.client, false ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EAGAIN ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, EBADF ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );
        areg::set_receive_live_handle( nullptr );

        AREG_CHILD_CHECK( round_trip( pair.server, pair.client ) );
    }
}

TEST( FaultInjectOsTest, receive_with_a_live_handle_waits_on_a_non_blocking_socket_and_fails_on_an_error )
{
    AREG_EXPECT_IN_CHILD( receive_with_a_live_handle_waits_on_a_non_blocking_socket_and_fails_on_an_error( ) );
}

namespace
{
    void nowait_and_window_receives_repeat_an_interrupted_receive_and_fail_on_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::NoCache );
        const uint8_t data[4]{ 2u, 4u, 6u, 8u };
        uint8_t received[4]{ };
        areg_fault::FaultScope faults;

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EINTR ) );
        AREG_CHILD_CHECK( areg::receive_data_window( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data_window( pair.client, received, sizeof( received ) ) < 0 );

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EINTR ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) == 0 );
    }
}

TEST( FaultInjectOsTest, nowait_and_window_receives_repeat_an_interrupted_receive_and_fail_on_an_error )
{
    AREG_EXPECT_IN_CHILD( nowait_and_window_receives_repeat_an_interrupted_receive_and_fail_on_an_error( ) );
}

//////////////////////////////////////////////////////////////////////////
// Connect and liveness
//////////////////////////////////////////////////////////////////////////

namespace
{
    void client_connect_fails_when_the_socket_flags_cannot_be_read_or_set( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        for ( const uint32_t skip : { 0u, 1u } )
        {
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, EBADF, skip ) );
            AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );
        }

        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::socket_close( client );
    }
}

TEST( FaultInjectOsTest, client_connect_fails_when_the_socket_flags_cannot_be_read_or_set )
{
    AREG_EXPECT_IN_CHILD( client_connect_fails_when_the_socket_flags_cannot_be_read_or_set( ) );
}

namespace
{
    void client_connect_waits_through_an_interrupted_poll_and_fails_when_the_result_cannot_be_read( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, EINTR ) );
        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        areg::socket_close( client );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, ENOBUFS ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectOsTest, client_connect_waits_through_an_interrupted_poll_and_fails_when_the_result_cannot_be_read )
{
    AREG_EXPECT_IN_CHILD( client_connect_waits_through_an_interrupted_poll_and_fails_when_the_result_cannot_be_read( ) );
}

namespace
{
    void buffer_sizes_are_the_default_when_the_socket_option_cannot_be_read( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, EBADF, 0u, 2u ) );
        AREG_CHILD_CHECK( areg::max_send_size( pair.client ) == areg::PACKET_DEFAULT_SIZE );
        AREG_CHILD_CHECK( areg::max_receive_size( pair.client ) == areg::PACKET_DEFAULT_SIZE );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 2u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::max_send_size( pair.client ) != 0u );
    }
}

TEST( FaultInjectOsTest, buffer_sizes_are_the_default_when_the_socket_option_cannot_be_read )
{
    AREG_EXPECT_IN_CHILD( buffer_sizes_are_the_default_when_the_socket_option_cannot_be_read( ) );
}

namespace
{
    void socket_alive_repeats_interrupted_calls_and_is_false_when_the_poll_fails( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, EINTR ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, ENOMEM ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );

        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EINTR ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::pending_read( pair.client ) == 1u );
    }
}

TEST( FaultInjectOsTest, socket_alive_repeats_interrupted_calls_and_is_false_when_the_poll_fails )
{
    AREG_EXPECT_IN_CHILD( socket_alive_repeats_interrupted_calls_and_is_false_when_the_poll_fails( ) );
}

//////////////////////////////////////////////////////////////////////////
// Read-ahead caches without memory
//////////////////////////////////////////////////////////////////////////

// The sanitizer runtimes define the global allocation functions themselves.
#if !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
    #if defined(__has_feature)
        #if !__has_feature(address_sanitizer) && !__has_feature(thread_sanitizer) && !__has_feature(memory_sanitizer)
            #define AREG_FAULT_OS_ALLOC_TESTS  1
        #endif  // !__has_feature(...)
    #else   // defined(__has_feature)
        #define AREG_FAULT_OS_ALLOC_TESTS      1
    #endif  // defined(__has_feature)
#endif  // !sanitizer

#if defined(AREG_FAULT_OS_ALLOC_TESTS)

namespace
{
    void scatter_send_without_a_staging_buffer_sends_the_list_directly( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint8_t first[3]{ 1u, 2u, 3u };
        const uint8_t second[2]{ 4u, 5u };
        const areg::IoBuffer buffers[2]{ { first, sizeof( first ) }, { second, sizeof( second ) } };

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, buffers, 2u, 5u ) == 5 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );

        uint8_t received[5]{ };
        AREG_CHILD_CHECK( read_exact( pair.server, received, sizeof( received ) ) );
        AREG_CHILD_CHECK( ( received[0] == 1u ) && ( received[4] == 5u ) );
        AREG_CHILD_CHECK( areg::thread_tx_cache( ).cache( ) != nullptr );
    }
}

TEST( FaultInjectOsTest, scatter_send_without_a_staging_buffer_sends_the_list_directly )
{
    AREG_EXPECT_IN_CHILD( scatter_send_without_a_staging_buffer_sends_the_list_directly( ) );
}

namespace
{
    void cached_receive_without_a_cache_reads_exactly_and_fill_reports_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::MultiCache );
        const uint8_t data[4]{ 4u, 3u, 2u, 1u };
        uint8_t received[4]{ };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );
        AREG_CHILD_CHECK( areg::recv_data_available( pair.client ) == 0u );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( areg::recv_fill_nowait( pair.client ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
    }
}

TEST( FaultInjectOsTest, cached_receive_without_a_cache_reads_exactly_and_fill_reports_an_error )
{
    AREG_EXPECT_IN_CHILD( cached_receive_without_a_cache_reads_exactly_and_fill_reports_an_error( ) );
}

#endif  // defined(AREG_FAULT_OS_ALLOC_TESTS)

//////////////////////////////////////////////////////////////////////////
// Multiplexer
//////////////////////////////////////////////////////////////////////////

namespace
{
    void multiplexer_without_an_epoll_set_refuses_sockets_and_fails_every_wait( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollCreate, EMFILE ) );
        {
            areg::SocketMultiplexer broken;
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( broken.register_socket( pair.client, true ) == false );
            AREG_CHILD_CHECK( broken.wait( 0 ) == areg::FailedSocketHandle );
            broken.reset( );
            AREG_CHILD_CHECK( broken.wait( 0 ) == areg::FailedSocketHandle );
            AREG_CHILD_CHECK( broken.register_socket( pair.client, true ) == false );
        }

        areg::SocketMultiplexer working;
        AREG_CHILD_CHECK( working.register_socket( pair.client, true ) );
        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        AREG_CHILD_CHECK( working.wait( static_cast<int32_t>(areg::WAIT_INFINITE) ) == pair.client );
    }
}

TEST( FaultInjectOsTest, multiplexer_without_an_epoll_set_refuses_sockets_and_fails_every_wait )
{
    AREG_EXPECT_IN_CHILD( multiplexer_without_an_epoll_set_refuses_sockets_and_fails_every_wait( ) );
}

namespace
{
    void multiplexer_without_a_wakeup_still_waits_and_completes_a_reset( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        for ( const areg_fault::FaultPoint point : { areg_fault::FaultPoint::EventCreate, areg_fault::FaultPoint::EpollCtl } )
        {
            AREG_CHILD_CHECK( areg_fault::arm( point, ENOMEM ) );
            areg::SocketMultiplexer mux;
            AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
            mux.wakeup( );
            AREG_CHILD_CHECK( mux.wait( 0 ) == areg::InvalidSocketHandle );

            const uint8_t data[1]{ 1u };
            AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
            AREG_CHILD_CHECK( mux.wait( static_cast<int32_t>(areg::WAIT_INFINITE) ) == pair.client );

            mux.reset( );
            AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
            AREG_CHILD_CHECK( mux.is_registered( pair.client ) == false );
            mux.reset( );
            AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
            AREG_CHILD_CHECK( mux.is_registered( pair.client ) );
            uint8_t received[1]{ };
            AREG_CHILD_CHECK( read_exact( pair.client, received, 1u ) );
        }
    }
}

TEST( FaultInjectOsTest, multiplexer_without_a_wakeup_still_waits_and_completes_a_reset )
{
    AREG_EXPECT_IN_CHILD( multiplexer_without_a_wakeup_still_waits_and_completes_a_reset( ) );
}

namespace
{
    void multiplexer_reset_drops_the_registrations_when_no_new_epoll_set_can_be_made( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        LoopPair idle;
        AREG_CHILD_CHECK( idle.open( ) );
        areg_fault::FaultScope faults;
        for ( const areg_fault::FaultPoint point : { areg_fault::FaultPoint::EpollCreate, areg_fault::FaultPoint::EpollCtl } )
        {
            areg::SocketMultiplexer mux;
            AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
            mux.reset( );
            AREG_CHILD_CHECK( areg_fault::arm( point, ENOMEM ) );
            AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
            AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( mux.is_registered( pair.client ) == false );
            AREG_CHILD_CHECK( mux.register_socket( idle.client, true ) );
            AREG_CHILD_CHECK( mux.wait( 0 ) == areg::InvalidSocketHandle );

            AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
            AREG_CHILD_CHECK( mux.wait( static_cast<int32_t>(areg::WAIT_INFINITE) ) == pair.client );
        }
    }
}

TEST( FaultInjectOsTest, multiplexer_reset_drops_the_registrations_when_no_new_epoll_set_can_be_made )
{
    AREG_EXPECT_IN_CHILD( multiplexer_reset_drops_the_registrations_when_no_new_epoll_set_can_be_made( ) );
}

namespace
{
    void multiplexer_wait_reports_an_interrupted_wait_as_no_socket_and_an_error_as_failed( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SocketMultiplexer mux;
        AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, EINTR ) );
        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::InvalidSocketHandle );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, EBADF ) );
        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollWait ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( mux.wait( static_cast<int32_t>(areg::WAIT_INFINITE) ) == pair.client );
    }
}

TEST( FaultInjectOsTest, multiplexer_wait_reports_an_interrupted_wait_as_no_socket_and_an_error_as_failed )
{
    AREG_EXPECT_IN_CHILD( multiplexer_wait_reports_an_interrupted_wait_as_no_socket_and_an_error_as_failed( ) );
}

namespace
{
    void stateless_wait_reports_a_failed_poll_as_failed( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, ENOMEM ) );
        AREG_CHILD_CHECK( areg::SocketMultiplexer{ }.wait( listener.handle, nullptr, 0, 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::SocketMultiplexer{ }.wait( listener.handle, nullptr, 0, 0 ) == areg::InvalidSocketHandle );
    }
}

TEST( FaultInjectOsTest, stateless_wait_reports_a_failed_poll_as_failed )
{
    AREG_EXPECT_IN_CHILD( stateless_wait_reports_a_failed_poll_as_failed( ) );
}

namespace
{
    void connect_by_address_fails_when_no_socket_can_be_created( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, EMFILE ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, EMFILE ) );
        areg::SocketAddress any;
        AREG_CHILD_CHECK( any.resolve_address( areg::String( "127.0.0.1" ), 1u, true ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::server_connect( any ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketCreate ) == 1u && NOT_REPLACED );

        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::socket_close( client );
    }

    void buffer_sizes_report_the_minimum_when_the_socket_option_cannot_be_set( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSetOption, ENOBUFS ) );
        AREG_CHILD_CHECK( areg::set_send_size( pair.client, 64u * 1024u ) == areg::PACKET_MIN_SIZE );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSetOption, ENOBUFS ) );
        AREG_CHILD_CHECK( areg::set_recv_size( pair.client, 64u * 1024u ) == areg::PACKET_MIN_SIZE );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSetOption ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::set_send_size( pair.client, 64u * 1024u ) == 64u * 1024u );
        AREG_CHILD_CHECK( areg::set_recv_size( pair.client, 64u * 1024u ) == 64u * 1024u );
    }
}

TEST( FaultInjectOsTest, connect_by_address_fails_when_no_socket_can_be_created )
{
    AREG_EXPECT_IN_CHILD( connect_by_address_fails_when_no_socket_can_be_created( ) );
}

TEST( FaultInjectOsTest, buffer_sizes_report_the_minimum_when_the_socket_option_cannot_be_set )
{
    AREG_EXPECT_IN_CHILD( buffer_sizes_report_the_minimum_when_the_socket_option_cannot_be_set( ) );
}

namespace
{
    void client_connect_fails_when_the_wait_for_the_connection_fails( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, ENOMEM ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );

        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::socket_close( client );
    }

    void socket_alive_with_unread_data_depends_on_the_peek( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EAGAIN ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
    }

    void cached_small_receive_waits_on_a_non_blocking_socket_and_fails_on_an_error( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::MonoCache );
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        uint8_t received[4]{ };
        areg_fault::FaultScope faults;

        AREG_CHILD_CHECK( set_nonblocking( pair.client, true ) );
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, EAGAIN ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );

        AREG_CHILD_CHECK( set_nonblocking( pair.client, false ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, ECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::recv_data_available( pair.client ) == 0u );

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
    }

    void server_accept_reports_no_socket_when_the_accept_fails( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::SocketMultiplexer mux;
        AREG_CHILD_CHECK( mux.register_socket( listener.handle, true ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketAccept, EMFILE ) );
        areg::SocketAddress peer;
        AREG_CHILD_CHECK( areg::server_accept( mux, listener.handle, &peer, static_cast<int32_t>(areg::WAIT_INFINITE) ) == areg::InvalidSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketAccept ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( peer.is_valid( ) == false );

        const SOCKETHANDLE accepted{ areg::server_accept( mux, listener.handle, &peer, static_cast<int32_t>(areg::WAIT_INFINITE) ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( accepted ) );
        AREG_CHILD_CHECK( peer.is_valid( ) );
        areg::socket_close( accepted );
        areg::socket_close( client );
    }

    void host_name_is_empty_while_the_system_cannot_report_it( )
    {
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::HostName, EFAULT ) );
        AREG_CHILD_CHECK( areg::hostname( ).is_empty( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::HostName ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::hostname( ).is_empty( ) == false );
    }
}

TEST( FaultInjectOsTest, client_connect_fails_when_the_wait_for_the_connection_fails )
{
    AREG_EXPECT_IN_CHILD( client_connect_fails_when_the_wait_for_the_connection_fails( ) );
}

TEST( FaultInjectOsTest, socket_alive_with_unread_data_depends_on_the_peek )
{
    AREG_EXPECT_IN_CHILD( socket_alive_with_unread_data_depends_on_the_peek( ) );
}

TEST( FaultInjectOsTest, cached_small_receive_waits_on_a_non_blocking_socket_and_fails_on_an_error )
{
    AREG_EXPECT_IN_CHILD( cached_small_receive_waits_on_a_non_blocking_socket_and_fails_on_an_error( ) );
}

TEST( FaultInjectOsTest, server_accept_reports_no_socket_when_the_accept_fails )
{
    AREG_EXPECT_IN_CHILD( server_accept_reports_no_socket_when_the_accept_fails( ) );
}

TEST( FaultInjectOsTest, host_name_is_empty_while_the_system_cannot_report_it )
{
    AREG_EXPECT_IN_CHILD( host_name_is_empty_while_the_system_cannot_report_it( ) );
}

namespace
{
    void file_lock_repeats_an_interrupted_lock( )
    {
        constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_os_lock.bin", mode ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, EINTR ) );
        AREG_CHILD_CHECK( file.lock_file( true ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );
        file.unlock_file( );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_os_lock.bin" ));
    }
}

TEST( FaultInjectOsTest, file_lock_repeats_an_interrupted_lock )
{
    AREG_EXPECT_IN_CHILD( file_lock_repeats_an_interrupted_lock( ) );
}

#endif  // defined(__linux__)
