/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_win32_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the Windows sockets, multiplexer, synchronization
 *              objects, files, timers and service manager calls when a system call fails.
 *              Each test runs in a child process.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if defined(_WIN32)

#include "fault-inject/FaultSocketPair.hpp"
#include "units/EventTestSupport.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/FatalError.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

#include <WinSock2.h>
#include <Windows.h>

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <deque>
#include <initializer_list>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace
{
    //!< The message of a fault point that did not fail any call.
    constexpr const char * NOT_REPLACED{ "the fault library did not replace the system call" };

    //!< The limit of a wait for an outcome that must come.
    constexpr uint32_t  WAIT_MS     { 30'000u };

    using fault_test::LoopPair;
    using fault_test::LoopListener;
    using fault_test::ModeScope;
    using fault_test::read_exact;
    using fault_test::round_trip;

    //!< Returns true when the socket has data to read or an end of stream, before the wait limit.
    bool wait_readable( SOCKETHANDLE sock )
    {
        WSAPOLLFD entry{ };
        entry.fd     = static_cast<SOCKET>(sock);
        entry.events = POLLRDNORM;
        return ( ::WSAPoll( &entry, 1u, static_cast<INT>(WAIT_MS) ) == 1 );
    }

    //!< The reason the fatal handler of a test expects.
    const char *        gFatalReason{ nullptr };

    //!< Ends the child with exit code 0 if the reason is the expected one, otherwise as failed.
    void _exit_on_expected_fatal( const char * reason ) noexcept
    {
        ::areg_fault_disarm_all( );
        if ( (reason != nullptr) && (gFatalReason != nullptr) && (std::strstr( reason, gFatalReason ) != nullptr) )
        {
            ::areg_child::pass( );
        }

        std::fprintf( stderr, "child check failed: unexpected fatal error '%s'\n", reason != nullptr ? reason : "" );
        std::fflush( stderr );
        std::_Exit( ::areg_child::CHILD_CHECK_FAILED );
    }

    //!< Installs the handler that ends the child when the fatal error names the reason.
    void expect_fatal( const char * reason )
    {
        gFatalReason = reason;
        static_cast<void>( areg::set_fatal_handler( &_exit_on_expected_fatal ) );
    }

    //!< Counts the expirations of each timer.
    class TimerListener final : public areg::TimerConsumer
    {
    public:
        //!< Waits until the timer expired at least the given number of times.
        bool wait_count( areg::Timer & timer, uint32_t expected )
        {
            std::unique_lock<std::mutex> guard( mMutex );
            return mChanged.wait_for( guard, std::chrono::milliseconds( WAIT_MS ), [&]( ) { return mCounts[&timer] >= expected; } );
        }

        //!< Returns the expirations of the timer.
        uint32_t count( areg::Timer & timer )
        {
            std::lock_guard<std::mutex> guard( mMutex );
            return mCounts[&timer];
        }

    private:
        void process_timer( areg::Timer & timer ) override
        {
            {
                std::lock_guard<std::mutex> guard( mMutex );
                ++ mCounts[&timer];
            }

            mChanged.notify_all( );
        }

        std::mutex                          mMutex;
        std::condition_variable             mChanged;
        std::map<areg::Timer *, uint32_t>   mCounts;
    };
}

//////////////////////////////////////////////////////////////////////////
// The socket library
//////////////////////////////////////////////////////////////////////////

namespace
{
    void socket_library_start_fails_and_the_last_release_stops_it( )
    {
        // Releases every reference of the process, also those taken while areg loads.
        uint32_t held{ 0u };
        for ( ; held < 64u; ++ held )
        {
            const SOCKETHANDLE probe{ areg::socket_create( ) };
            if ( areg::is_valid_socket( probe ) == false )
                break;

            areg::socket_close( probe );
            areg::socket_release( );
        }

        AREG_CHILD_CHECK( (held != 0u) && (held < 64u) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketInit, WSASYSNOTREADY ) );
        AREG_CHILD_CHECK( areg::socket_initialize( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketInit ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::socket_create( ) ) == false );

        for ( uint32_t i = 0u; i < held; ++ i )
        {
            AREG_CHILD_CHECK( areg::socket_initialize( ) );
        }

        const SOCKETHANDLE sock{ areg::socket_create( ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( sock ) );
        areg::socket_close( sock );
    }
}

TEST( FaultInjectWin32Test, socket_library_start_fails_and_the_last_release_stops_it )
{
    AREG_EXPECT_IN_CHILD( socket_library_start_fails_and_the_last_release_stops_it( ) );
}

//////////////////////////////////////////////////////////////////////////
// Send
//////////////////////////////////////////////////////////////////////////

namespace
{
    void send_data_repeats_an_interrupted_send_and_fails_when_a_blocking_socket_would_block( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAEINTR ) );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAEWOULDBLOCK ) );
        const uint8_t data[4]{ 1u, 2u, 3u, 4u };
        AREG_CHILD_CHECK( areg::send_data( pair.client, data, sizeof( data ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.client, pair.server ) );
    }
}

TEST( FaultInjectWin32Test, send_data_repeats_an_interrupted_send_and_fails_when_a_blocking_socket_would_block )
{
    AREG_EXPECT_IN_CHILD( send_data_repeats_an_interrupted_send_and_fails_when_a_blocking_socket_would_block( ) );
}

namespace
{
    void try_send_reports_zero_when_it_would_block_and_an_error_when_the_send_or_the_wait_fails( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        const uint8_t first[3]{ 1u, 2u, 3u };
        const uint8_t second[2]{ 4u, 5u };
        const areg::IoBuffer buffers[2]{ { first, sizeof( first ) }, { second, sizeof( second ) } };

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAEWOULDBLOCK ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 5u ) == 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 5u ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 5u ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::try_send_data_v( pair.client, buffers, 2u, 5u ) == 5 );
        uint8_t received[5]{ };
        AREG_CHILD_CHECK( read_exact( pair.server, received, sizeof( received ) ) );
        AREG_CHILD_CHECK( ( received[0] == 1u ) && ( received[4] == 5u ) );
    }
}

TEST( FaultInjectWin32Test, try_send_reports_zero_when_it_would_block_and_an_error_when_the_send_or_the_wait_fails )
{
    AREG_EXPECT_IN_CHILD( try_send_reports_zero_when_it_would_block_and_an_error_when_the_send_or_the_wait_fails( ) );
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
        bool got{ false };
        const SOCKETHANDLE server{ pair.server };
        std::thread reader( [server, &received, &got]( )
            {
                got = read_exact( server, received.data( ), total );
            } );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAEINTR ) );
        const int32_t sent{ areg::send_data_v( pair.client, buffers, 2u, total ) };
        const uint32_t interrupted{ areg_fault::failed( areg_fault::FaultPoint::SocketSend ) };
        reader.join( );
        AREG_CHILD_CHECK( sent == static_cast<int32_t>(total) );
        AREG_CHILD_CHECK( interrupted == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( got && ( received[0] == 0x11u ) && ( received[total - 1u] == 0x22u ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, buffers, 2u, total ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectWin32Test, scatter_send_of_large_blocks_repeats_an_interrupted_send_and_fails_on_an_error )
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
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, flushLast, 2u, 2u ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, bigFirst, 12u, bigTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, bigAfter, 12u, bigTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::send_data_v( pair.client, overflow, 12u, overflowTotal ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::send_data_v( pair.client, flushLast, 2u, 2u ) == 2 );
        uint8_t received[2]{ };
        AREG_CHILD_CHECK( read_exact( pair.server, received, sizeof( received ) ) && ( received[1] == 0x55u ) );
    }
}

TEST( FaultInjectWin32Test, scatter_send_of_small_blocks_fails_at_every_flush_of_the_staging_buffer )
{
    AREG_EXPECT_IN_CHILD( scatter_send_of_small_blocks_fails_at_every_flush_of_the_staging_buffer( ) );
}

//////////////////////////////////////////////////////////////////////////
// Receive
//////////////////////////////////////////////////////////////////////////

namespace
{
    void receive_repeats_an_interrupted_receive_in_every_receive_mode_and_fails_when_it_would_block( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        for ( const areg::ReceiveMode mode : { areg::ReceiveMode::NoCache, areg::ReceiveMode::MonoCache } )
        {
            ModeScope scope( mode );
            const uint8_t data[4]{ 9u, 8u, 7u, 6u };
            AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEINTR ) );
            uint8_t received[4]{ };
            AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) == 4 );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );
        }

        ModeScope scope( areg::ReceiveMode::NoCache );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEWOULDBLOCK ) );
        uint8_t received[4]{ };
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( round_trip( pair.server, pair.client ) );
    }
}

TEST( FaultInjectWin32Test, receive_repeats_an_interrupted_receive_in_every_receive_mode_and_fails_when_it_would_block )
{
    AREG_EXPECT_IN_CHILD( receive_repeats_an_interrupted_receive_in_every_receive_mode_and_fails_when_it_would_block( ) );
}

namespace
{
    void cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error_or_a_close( )
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
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEINTR ) );
        const int32_t got{ areg::receive_data( pair.client, received.data( ), length ) };
        const uint32_t interrupted{ areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) };
        writer.join( );
        AREG_CHILD_CHECK( got == static_cast<int32_t>(length) );
        AREG_CHILD_CHECK( interrupted == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( received[length - 1u] == 0x66u );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received.data( ), length ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );

        // A block cut by the close of the peer is an error, a close before the block is the end of the stream.
        AREG_CHILD_CHECK( areg::send_data( pair.server, data.data( ), 16u ) == 16 );
        areg::socket_close( pair.server );
        pair.server = areg::InvalidSocketHandle;
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received.data( ), length ) < 0 );
        AREG_CHILD_CHECK( areg::receive_data( pair.client, received.data( ), length ) == 0 );
    }
}

TEST( FaultInjectWin32Test, cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error_or_a_close )
{
    AREG_EXPECT_IN_CHILD( cached_receive_of_a_large_block_repeats_an_interrupted_receive_and_fails_on_an_error_or_a_close( ) );
}

namespace
{
    void window_and_nowait_receives_report_failed_calls( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        ModeScope scope( areg::ReceiveMode::NoCache );
        const uint8_t data[4]{ 2u, 4u, 6u, 8u };
        uint8_t received[4]{ };
        areg_fault::FaultScope faults;

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEINTR ) );
        AREG_CHILD_CHECK( areg::receive_data_window( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data_window( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );

        // Nothing to read: a failed wait is an error, a quiet socket gives nothing.
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) == 0 );

        AREG_CHILD_CHECK( areg::send_data( pair.server, data, sizeof( data ) ) == 4 );
        AREG_CHILD_CHECK( wait_readable( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, WSAENOTSOCK ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEWOULDBLOCK ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) == 0 );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::receive_data_nowait( pair.client, received, sizeof( received ) ) == 4 );
        AREG_CHILD_CHECK( std::memcmp( data, received, sizeof( data ) ) == 0 );
    }
}

TEST( FaultInjectWin32Test, window_and_nowait_receives_report_failed_calls )
{
    AREG_EXPECT_IN_CHILD( window_and_nowait_receives_report_failed_calls( ) );
}

//////////////////////////////////////////////////////////////////////////
// Connect, accept, options and liveness
//////////////////////////////////////////////////////////////////////////

namespace
{
    void client_connect_fails_when_the_mode_cannot_be_set_the_connect_is_refused_or_its_wait_fails( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Fcntl, WSAENOTSOCK ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Fcntl ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketConnect, WSAECONNREFUSED ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketConnect ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 1u && NOT_REPLACED );

        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::socket_close( client );
    }
}

TEST( FaultInjectWin32Test, client_connect_fails_when_the_mode_cannot_be_set_the_connect_is_refused_or_its_wait_fails )
{
    AREG_EXPECT_IN_CHILD( client_connect_fails_when_the_mode_cannot_be_set_the_connect_is_refused_or_its_wait_fails( ) );
}

namespace
{
    void connect_by_address_fails_when_no_socket_can_be_created( )
    {
        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, WSAEMFILE ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::client_connect( listener.address ) ) == false );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, WSAEMFILE ) );
        areg::SocketAddress any;
        AREG_CHILD_CHECK( any.resolve_address( areg::String( "127.0.0.1" ), 1u, true ) );
        AREG_CHILD_CHECK( areg::is_valid_socket( areg::server_connect( any ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketCreate ) == 1u && NOT_REPLACED );

        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::socket_close( client );
    }
}

TEST( FaultInjectWin32Test, connect_by_address_fails_when_no_socket_can_be_created )
{
    AREG_EXPECT_IN_CHILD( connect_by_address_fails_when_no_socket_can_be_created( ) );
}

namespace
{
    void accept_fails_on_a_socket_whose_listen_state_cannot_be_read_and_when_the_system_refuses( )
    {
        const SOCKETHANDLE idle{ areg::socket_create( ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( idle ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, WSAENOTSOCK ) );
        AREG_CHILD_CHECK( areg::server_accept( idle, nullptr, 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 1u && NOT_REPLACED );
        areg::socket_close( idle );

        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        const SOCKETHANDLE client{ areg::client_connect( listener.address ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( client ) );
        areg::SocketMultiplexer mux;
        AREG_CHILD_CHECK( mux.register_socket( listener.handle, true ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketAccept, WSAEMFILE ) );
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
}

TEST( FaultInjectWin32Test, accept_fails_on_a_socket_whose_listen_state_cannot_be_read_and_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD( accept_fails_on_a_socket_whose_listen_state_cannot_be_read_and_when_the_system_refuses( ) );
}

namespace
{
    void buffer_sizes_fall_back_when_the_socket_option_cannot_be_read_or_set( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketGetOption, WSAENOTSOCK, 0u, 2u ) );
        AREG_CHILD_CHECK( areg::max_send_size( pair.client ) == areg::PACKET_DEFAULT_SIZE );
        AREG_CHILD_CHECK( areg::max_receive_size( pair.client ) == areg::PACKET_DEFAULT_SIZE );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketGetOption ) == 2u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::max_send_size( pair.client ) != 0u );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSetOption, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::set_send_size( pair.client, 64u * 1024u ) == areg::PACKET_MIN_SIZE );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSetOption, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::set_recv_size( pair.client, 64u * 1024u ) == areg::PACKET_MIN_SIZE );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSetOption ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::set_send_size( pair.client, 64u * 1024u ) == 64u * 1024u );
        AREG_CHILD_CHECK( areg::set_recv_size( pair.client, 64u * 1024u ) == 64u * 1024u );
    }
}

TEST( FaultInjectWin32Test, buffer_sizes_fall_back_when_the_socket_option_cannot_be_read_or_set )
{
    AREG_EXPECT_IN_CHILD( buffer_sizes_fall_back_when_the_socket_option_cannot_be_read_or_set( ) );
}

namespace
{
    void socket_alive_is_false_when_the_poll_fails_and_with_unread_data_depends_on_the_peek( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );

        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        AREG_CHILD_CHECK( wait_readable( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAEWOULDBLOCK ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, WSAECONNRESET ) );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::is_socket_alive( pair.client ) );
        AREG_CHILD_CHECK( areg::pending_read( pair.client ) == 1u );
    }
}

TEST( FaultInjectWin32Test, socket_alive_is_false_when_the_poll_fails_and_with_unread_data_depends_on_the_peek )
{
    AREG_EXPECT_IN_CHILD( socket_alive_is_false_when_the_poll_fails_and_with_unread_data_depends_on_the_peek( ) );
}

namespace
{
    void host_name_is_empty_while_the_system_cannot_report_it( )
    {
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::HostName, WSAEFAULT ) );
        AREG_CHILD_CHECK( areg::hostname( ).is_empty( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::HostName ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::hostname( ).is_empty( ) == false );
        areg::socket_release( );
    }
}

TEST( FaultInjectWin32Test, host_name_is_empty_while_the_system_cannot_report_it )
{
    AREG_EXPECT_IN_CHILD( host_name_is_empty_while_the_system_cannot_report_it( ) );
}

//////////////////////////////////////////////////////////////////////////
// Multiplexer
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< The calls of the wake-up pair of the multiplexer, each one made to fail in turn.
    struct WakeupStep
    {
        areg_fault::FaultPoint  point;
        uint32_t                skip;
    };

    void multiplexer_without_a_wakeup_still_waits_and_completes_a_reset( const WakeupStep & step )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( step.point, WSAEMFILE, step.skip ) );
        areg::SocketMultiplexer mux;
        AREG_CHILD_CHECK( areg_fault::failed( step.point ) == 1u && NOT_REPLACED );
        ::areg_fault_disarm_all( );

        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
        mux.wakeup( );
        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::InvalidSocketHandle );

        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );
        AREG_CHILD_CHECK( mux.wait( static_cast<int32_t>(WAIT_MS) ) == pair.client );

        mux.reset( );
        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( mux.is_registered( pair.client ) == false );
        AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
        AREG_CHILD_CHECK( mux.is_registered( pair.client ) );
        uint8_t received[1]{ };
        AREG_CHILD_CHECK( read_exact( pair.client, received, 1u ) );
    }
}

TEST( FaultInjectWin32Test, multiplexer_without_a_wakeup_still_waits_and_completes_a_reset )
{
    const WakeupStep steps[]
    {
          { areg_fault::FaultPoint::SocketCreate , 0u }
        , { areg_fault::FaultPoint::SocketBind   , 0u }
        , { areg_fault::FaultPoint::SocketListen , 0u }
        , { areg_fault::FaultPoint::SocketName   , 0u }
        , { areg_fault::FaultPoint::SocketCreate , 1u }
        , { areg_fault::FaultPoint::SocketConnect, 0u }
        , { areg_fault::FaultPoint::SocketAccept , 0u }
    };

    for ( const WakeupStep & step : steps )
    {
        AREG_EXPECT_IN_CHILD( multiplexer_without_a_wakeup_still_waits_and_completes_a_reset( step ) );
    }
}

namespace
{
    void multiplexer_waits_report_a_failed_poll_as_failed( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SocketMultiplexer mux;
        AREG_CHILD_CHECK( mux.register_socket( pair.client, true ) );
        const uint8_t data[1]{ 1u };
        AREG_CHILD_CHECK( areg::send_data( pair.server, data, 1u ) == 1 );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( mux.wait( 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( mux.wait( static_cast<int32_t>(WAIT_MS) ) == pair.client );

        LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, WSAENOBUFS ) );
        AREG_CHILD_CHECK( areg::SocketMultiplexer{ }.wait( listener.handle, nullptr, 0, 0 ) == areg::FailedSocketHandle );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::SocketMultiplexer{ }.wait( listener.handle, nullptr, 0, 0 ) == areg::InvalidSocketHandle );
    }
}

TEST( FaultInjectWin32Test, multiplexer_waits_report_a_failed_poll_as_failed )
{
    AREG_EXPECT_IN_CHILD( multiplexer_waits_report_a_failed_poll_as_failed( ) );
}

//////////////////////////////////////////////////////////////////////////
// Synchronization objects
//////////////////////////////////////////////////////////////////////////

namespace
{
    void mutex_is_fatal_while_the_system_cannot_create_it( )
    {
        expect_fatal( "a Mutex cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::MutexInit, ERROR_NOT_ENOUGH_MEMORY ) );
        areg::Mutex mutex( false );
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::MutexInit ) == 0u) && "the mutex was created" );
    }
}

TEST( FaultInjectWin32Test, mutex_is_fatal_while_the_system_cannot_create_it )
{
    AREG_EXPECT_IN_CHILD( mutex_is_fatal_while_the_system_cannot_create_it( ) );
}

namespace
{
    void event_is_fatal_while_the_system_cannot_create_it( )
    {
        expect_fatal( "a SyncEvent cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EventCreate, ERROR_NOT_ENOUGH_MEMORY ) );
        areg::SyncEvent event( true, true );
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::EventCreate ) == 0u) && "the event was created" );
    }
}

TEST( FaultInjectWin32Test, event_is_fatal_while_the_system_cannot_create_it )
{
    AREG_EXPECT_IN_CHILD( event_is_fatal_while_the_system_cannot_create_it( ) );
}

namespace
{
    void semaphore_is_fatal_while_the_system_cannot_create_it( )
    {
        expect_fatal( "a Semaphore cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SemaphoreCreate, ERROR_NOT_ENOUGH_MEMORY ) );
        areg::Semaphore semaphore( 2, 0 );
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::SemaphoreCreate ) == 0u) && "the semaphore was created" );
    }
}

TEST( FaultInjectWin32Test, semaphore_is_fatal_while_the_system_cannot_create_it )
{
    AREG_EXPECT_IN_CHILD( semaphore_is_fatal_while_the_system_cannot_create_it( ) );
}

namespace
{
    void steady_timer_falls_back_to_a_plain_timer_and_is_fatal_without_any( )
    {
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::WaitableTimerCreate, ERROR_INVALID_PARAMETER ) );
            areg::SyncTimer timer( 1u, false, true, true );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::WaitableTimerCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( timer.set_timer( ) );
            AREG_CHILD_CHECK( timer.lock( WAIT_MS ) );
        }

        expect_fatal( "a SyncTimer cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::WaitableTimerCreate, ERROR_NOT_ENOUGH_MEMORY, 0u, 2u ) );
        areg::SyncTimer timer( 1u, false, true, true );
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::WaitableTimerCreate ) == 0u) && "the timer was created" );
    }
}

TEST( FaultInjectWin32Test, steady_timer_falls_back_to_a_plain_timer_and_is_fatal_without_any )
{
    AREG_EXPECT_IN_CHILD( steady_timer_falls_back_to_a_plain_timer_and_is_fatal_without_any( ) );
}

namespace
{
    void plain_timer_is_fatal_while_the_system_cannot_create_it( )
    {
        expect_fatal( "a SyncTimer cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::WaitableTimerCreate, ERROR_NOT_ENOUGH_MEMORY ) );
        areg::SyncTimer timer( 1u, false, true, false );
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::WaitableTimerCreate ) == 0u) && "the timer was created" );
    }
}

TEST( FaultInjectWin32Test, plain_timer_is_fatal_while_the_system_cannot_create_it )
{
    AREG_EXPECT_IN_CHILD( plain_timer_is_fatal_while_the_system_cannot_create_it( ) );
}

namespace
{
    void wait_falls_back_to_a_plain_timer_and_is_fatal_without_any( )
    {
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::WaitableTimerCreate, ERROR_INVALID_PARAMETER ) );
            areg::Wait wait;
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::WaitableTimerCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( wait.wait_for( areg::Wait::MIN_WAIT ) == areg::Wait::WaitResolution::Millisecond );
        }

        expect_fatal( "a Wait cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::WaitableTimerCreate, ERROR_NOT_ENOUGH_MEMORY, 0u, 2u ) );
        areg::Wait wait;
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::WaitableTimerCreate ) == 0u) && "the wait was created" );
    }
}

TEST( FaultInjectWin32Test, wait_falls_back_to_a_plain_timer_and_is_fatal_without_any )
{
    AREG_EXPECT_IN_CHILD( wait_falls_back_to_a_plain_timer_and_is_fatal_without_any( ) );
}

//////////////////////////////////////////////////////////////////////////
// Files
//////////////////////////////////////////////////////////////////////////

namespace
{
    void file_cursor_size_and_reserve_report_a_failed_seek( )
    {
        constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_win32_seek.bin", mode ) );
        const uint8_t data[8]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
        AREG_CHILD_CHECK( file.write( data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileSeek, ERROR_INVALID_HANDLE ) );
        AREG_CHILD_CHECK( file.position( ) == areg::Cursor::INVALID_CURSOR_POSITION );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileSeek, ERROR_INVALID_HANDLE ) );
        AREG_CHILD_CHECK( file.set_position( 2, areg::Cursor::SeekOrigin::Begin ) == areg::Cursor::INVALID_CURSOR_POSITION );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileSeek, ERROR_INVALID_HANDLE, 1u ) );
        AREG_CHILD_CHECK( file.reserve( 64u ) == areg::Cursor::INVALID_CURSOR_POSITION );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileSeek, ERROR_INVALID_HANDLE ) );
        AREG_CHILD_CHECK( file.truncate( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileSeek ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( file.length( ) == sizeof( data ) );

        AREG_CHILD_CHECK( file.set_position( 2, areg::Cursor::SeekOrigin::Begin ) == 2u );
        AREG_CHILD_CHECK( file.truncate( ) );
        AREG_CHILD_CHECK( file.length( ) == 0u );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_win32_seek.bin" ));
    }
}

TEST( FaultInjectWin32Test, file_cursor_size_and_reserve_report_a_failed_seek )
{
    AREG_EXPECT_IN_CHILD( file_cursor_size_and_reserve_report_a_failed_seek( ) );
}

namespace
{
    void temporary_name_retries_a_taken_name_and_is_not_made_when_the_folder_refuses( )
    {
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileOpen, ERROR_FILE_EXISTS ) );
        const areg::String taken{ areg::File::temp_name( areg::String( "fault_inject_" ), false, true ) };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileOpen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::File::has_file( taken ) );
        static_cast<void>(areg::File::delete_file( taken ));

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileOpen, ERROR_ACCESS_DENIED ) );
        const areg::String refused{ areg::File::temp_name( areg::String( "fault_inject_" ), false, true ) };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileOpen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( refused.is_empty( ) == false );
        AREG_CHILD_CHECK( areg::File::has_file( refused ) == false );
    }
}

TEST( FaultInjectWin32Test, temporary_name_retries_a_taken_name_and_is_not_made_when_the_folder_refuses )
{
    AREG_EXPECT_IN_CHILD( temporary_name_retries_a_taken_name_and_is_not_made_when_the_folder_refuses( ) );
}

//////////////////////////////////////////////////////////////////////////
// Timers
//////////////////////////////////////////////////////////////////////////

namespace
{
    void timer_whose_system_timer_cannot_be_created_stays_inactive_and_starts_after( )
    {
        areg::Application::setup( false, false, false, true, false );
        EventTestDispatcher dispatcher{ "FaultInjectWin32Timers" };
        TimerListener consumer;
        AREG_CHILD_CHECK( ( areg::Application::system_state( ) == areg::AppState::Ready ) && dispatcher.start_ready( ) );
        areg::Timer refused( consumer, "FaultInjectWin32Refused" );
        areg::Timer marker( consumer, "FaultInjectWin32Marker" );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::TimerFdCreate, ERROR_NOT_ENOUGH_MEMORY ) );
            AREG_CHILD_CHECK( refused.start_timer( 1u, dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( marker.start_timer( 1u, dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( consumer.wait_count( marker, 1u ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::TimerFdCreate ) == 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( refused.is_active( ) == false );
        AREG_CHILD_CHECK( consumer.count( refused ) == 0u );

        AREG_CHILD_CHECK( refused.start_timer( 1u, dispatcher, areg::Timer::ONE_TIME ) );
        AREG_CHILD_CHECK( consumer.wait_count( refused, 1u ) );
        refused.stop_timer( );
        marker.stop_timer( );
        dispatcher.shutdown( areg::WAIT_INFINITE );
        areg::Application::release( );
    }
}

TEST( FaultInjectWin32Test, timer_whose_system_timer_cannot_be_created_stays_inactive_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_whose_system_timer_cannot_be_created_stays_inactive_and_starts_after( ) );
}

//////////////////////////////////////////////////////////////////////////
// The send backlog
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Builds a message with a payload of the given size.
    areg::MessageEnvelope make_message( uint32_t bytes )
    {
        areg::MessageEnvelope message;
        if ( message.init_envelope( areg::notify_client_connection( ), bytes ) != nullptr )
        {
            message.header( )->bufHeader.biUsed = bytes;
        }

        return message;
    }

    //!< Returns true if the peer receives exactly the bytes of the message.
    bool peer_receives( SOCKETHANDLE peer, const areg::MessageEnvelope & message )
    {
        const uint32_t size{ static_cast<uint32_t>(areg::wire_size( *message.header( ) )) };
        std::vector<uint8_t> received( size );
        return read_exact( peer, received.data( ), size )
            && (std::memcmp( received.data( ), areg::wire_data( *message.header( ) ), size ) == 0);
    }
}

namespace
{
    void backlog_without_its_completion_port_writes_and_wakes_its_send_thread( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SendQueueGate gate{ };
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollCreate, ERROR_NOT_ENOUGH_MEMORY ) );
        areg::ext::SendBacklog backlog{ gate };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollCreate ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( backlog.attach( pair.server ) == false );
        const areg::MessageEnvelope message{ make_message( 256u ) };
        AREG_CHILD_CHECK( message.header( ) != nullptr );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 7u, pair.server, message ) == 1 );
        }

        AREG_CHILD_CHECK( peer_receives( pair.client, message ) );
        AREG_CHILD_CHECK( backlog.is_empty( ) );

        backlog.reset_wake( );
        backlog.wake( );
        backlog.wait( areg::WAIT_INFINITE );
    }
}

TEST( FaultInjectWin32Test, backlog_without_its_completion_port_writes_and_wakes_its_send_thread )
{
    AREG_EXPECT_IN_CHILD( backlog_without_its_completion_port_writes_and_wakes_its_send_thread( ) );
}

namespace
{
    void backlog_writes_a_socket_it_cannot_attach_and_a_failed_port_wait_returns( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SendQueueGate gate{ };
        areg::ext::SendBacklog backlog{ gate };
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollCreate, ERROR_INVALID_PARAMETER ) );
        AREG_CHILD_CHECK( backlog.attach( pair.server ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollCreate ) == 1u && NOT_REPLACED );

        const areg::MessageEnvelope message{ make_message( 512u ) };
        AREG_CHILD_CHECK( message.header( ) != nullptr );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 9u, pair.server, message ) == 1 );
        }

        AREG_CHILD_CHECK( peer_receives( pair.client, message ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, ERROR_INVALID_HANDLE ) );
        backlog.reset_wake( );
        backlog.wait( areg::WAIT_INFINITE );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollWait ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectWin32Test, backlog_writes_a_socket_it_cannot_attach_and_a_failed_port_wait_returns )
{
    AREG_EXPECT_IN_CHILD( backlog_writes_a_socket_it_cannot_attach_and_a_failed_port_wait_returns( ) );
}

namespace
{
    void inline_write_reports_a_failed_socket_and_cancels_the_writes_it_started( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        static_cast<void>( areg::set_send_size( pair.server, areg::PACKET_MIN_SIZE ) );
        static_cast<void>( areg::set_recv_size( pair.client, areg::PACKET_MIN_SIZE ) );
        areg::SendQueueGate gate{ };
        areg::ext::SendBacklog backlog{ gate };
        AREG_CHILD_CHECK( backlog.attach( pair.server ) );

        const areg::MessageEnvelope small{ make_message( 256u ) };
        AREG_CHILD_CHECK( small.header( ) != nullptr );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET ) );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 3u, pair.server, small ) == -1 );
        }

        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( backlog.is_empty( ) );

        // The peer reads nothing: a large message fills the connection and its rest is kept.
        const areg::MessageEnvelope large{ make_message( 4u * areg::ext::SendBacklog::WRITE_CHUNK ) };
        AREG_CHILD_CHECK( large.header( ) != nullptr );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 4u, pair.server, large ) == 2 );
        }

        AREG_CHILD_CHECK( backlog.is_empty( ) == false );

        // The first write of the next message waits, the second fails: the started one is cancelled.
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, WSAECONNRESET, 1u ) );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 5u, pair.server, large ) == -1 );
        }

        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        std::deque<ITEM_ID> cut;
        backlog.release_all( cut );
        AREG_CHILD_CHECK( backlog.is_empty( ) );
    }
}

TEST( FaultInjectWin32Test, inline_write_reports_a_failed_socket_and_cancels_the_writes_it_started )
{
    AREG_EXPECT_IN_CHILD( inline_write_reports_a_failed_socket_and_cancels_the_writes_it_started( ) );
}

//////////////////////////////////////////////////////////////////////////
// The service control manager
//////////////////////////////////////////////////////////////////////////

namespace
{
    void local_service_does_not_start_when_the_service_manager_or_the_service_cannot_be_opened( )
    {
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ServiceManagerOpen, ERROR_ACCESS_DENIED ) );
        AREG_CHILD_CHECK( areg::Application::start_router_service( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ServiceManagerOpen ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ServiceOpen, ERROR_SERVICE_DOES_NOT_EXIST ) );
        AREG_CHILD_CHECK( areg::Application::start_logging_service( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ServiceOpen ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectWin32Test, local_service_does_not_start_when_the_service_manager_or_the_service_cannot_be_opened )
{
    AREG_EXPECT_IN_CHILD( local_service_does_not_start_when_the_service_manager_or_the_service_cannot_be_opened( ) );
}

namespace
{
    //!< Gives the process a console of its own as the standard input and output.
    void open_own_console( )
    {
        static_cast<void>(::FreeConsole( ));
        AREG_CHILD_CHECK( ::AllocConsole( ) != FALSE );
        const HANDLE input{ ::CreateFileW( L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr ) };
        const HANDLE output{ ::CreateFileW( L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr ) };
        AREG_CHILD_CHECK( (input != INVALID_HANDLE_VALUE) && (output != INVALID_HANDLE_VALUE) );
        AREG_CHILD_CHECK( (::SetStdHandle( STD_INPUT_HANDLE, input ) != FALSE) && (::SetStdHandle( STD_OUTPUT_HANDLE, output ) != FALSE) );
    }

    //!< Returns the record of a pressed key: a character, or a key code without a character.
    INPUT_RECORD key_press( WCHAR ch, WORD vk = 0u, WORD repeat = 1u )
    {
        INPUT_RECORD record{ };
        record.EventType                        = KEY_EVENT;
        record.Event.KeyEvent.bKeyDown          = TRUE;
        record.Event.KeyEvent.wRepeatCount      = repeat;
        record.Event.KeyEvent.wVirtualKeyCode   = vk;
        record.Event.KeyEvent.uChar.UnicodeChar = ch;
        return record;
    }

    //!< Puts the records into the input of the console.
    void put_keys( std::initializer_list<INPUT_RECORD> records )
    {
        DWORD written{ 0u };
        AREG_CHILD_CHECK( ::WriteConsoleInputW( ::GetStdHandle( STD_INPUT_HANDLE ), records.begin( ), static_cast<DWORD>(records.size( )), &written ) != FALSE );
        AREG_CHILD_CHECK( written == records.size( ) );
    }

    void console_without_terminal_input_edits_and_recalls_with_key_codes( )
    {
        open_own_console( );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ConsoleMode, ERROR_INVALID_PARAMETER, 1u, 1u ) );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        AREG_CHILD_CHECK( (areg_fault::failed( areg_fault::FaultPoint::ConsoleMode ) == 1u) && NOT_REPLACED );
        AREG_CHILD_CHECK( console.is_terminal( ) );
        AREG_CHILD_CHECK( console.enable_console_input( true ) );

        put_keys( { key_press( L'a', 0u, 3u ), key_press( 0, VK_LEFT ), key_press( 0, VK_LEFT ), key_press( 0, VK_DELETE ), key_press( 0, VK_HOME )
                  , key_press( L'x' ), key_press( 0, VK_END ), key_press( L'z' ), key_press( 0, VK_RIGHT ), key_press( 0, VK_F1 ), key_press( L'\r' ) } );
        AREG_CHILD_CHECK( console.wait_for_input( nullptr ) == "xaaz" );

        put_keys( { key_press( L'n' ), key_press( 0, VK_UP ), key_press( 0, VK_DOWN ), key_press( L'!' ), key_press( L'\r' ) } );
        AREG_CHILD_CHECK( console.wait_for_input( nullptr ) == "n!" );
        put_keys( { key_press( 0, VK_UP ), key_press( 0, VK_UP ), key_press( L'\r' ) } );
        AREG_CHILD_CHECK( console.wait_for_input( nullptr ) == "xaaz" );
    }
}

// A console that refuses the terminal input mode reports key codes: arrows, Home, End and Delete
// still edit the line and recall the entered lines, and a repeated key counts as several presses.
TEST( FaultInjectWin32Test, console_without_terminal_input_edits_and_recalls_with_key_codes )
{
    AREG_EXPECT_IN_CHILD( console_without_terminal_input_edits_and_recalls_with_key_codes( ) );
}

#endif  // defined(_WIN32)
