/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketOsCoverageTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the socket helpers at the edges of their input:
 *              address and credential comparison, sockets that are not connected or not
 *              listening, empty buffers, the read-ahead cache after it was served, and the
 *              configured send batch and keepalive limits.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/base/private/SocketLiveness.hpp"

#include <fstream>
#include <string>
#include <vector>

#if defined(_POSIX)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>
    #include <unistd.h>
#elif defined(WINDOWS)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif  // _POSIX / WINDOWS

namespace
{
    //!< The loopback address of every test.
    constexpr std::string_view  LOOPBACK    { "127.0.0.1" };

    //!< Initializes the sockets of the test and releases them at the end.
    class SocketOsLoopTest : public testing::Test
    {
    protected:
        void SetUp( ) override
        {
            ASSERT_TRUE( areg::socket_initialize( ) );
        }

        void TearDown( ) override
        {
            areg::socket_release( );
        }
    };

    //!< Binds a socket to a free loopback port and returns it with its address; listens if asked.
    SOCKETHANDLE bind_loopback( areg::SocketAddress & address, bool listen )
    {
        const SOCKETHANDLE handle{ areg::socket_create( ) };
        if ( areg::is_valid_socket( handle ) == false )
            return areg::InvalidSocketHandle;

        sockaddr_in local{ };
        local.sin_family        = AF_INET;
        local.sin_addr.s_addr   = htonl( INADDR_LOOPBACK );
        local.sin_port          = 0;
#if defined(WINDOWS)
        int length{ static_cast<int>(sizeof( local )) };
#else   // defined(WINDOWS)
        socklen_t length{ sizeof( local ) };
#endif  // defined(WINDOWS)
        if ( ( ::bind( handle, reinterpret_cast<sockaddr *>(&local), sizeof( local ) ) != 0 )
          || ( ::getsockname( handle, reinterpret_cast<sockaddr *>(&local), &length ) != 0 )
          || ( listen && ( areg::server_listen( handle, 4 ) == false ) ) )
        {
            areg::socket_close( handle );
            return areg::InvalidSocketHandle;
        }

        address.from_sockaddr( local );
        return handle;
    }

    //!< Writes the lines to a configuration file named after the test and returns its path.
    areg::String write_config( const std::string & lines )
    {
        const std::string path{ std::string( "./" ) + ::testing::UnitTest::GetInstance( )->current_test_info( )->name( ) + ".init" };
        std::ofstream out( path, std::ios::trunc );
        out << lines;
        return areg::String( path.c_str( ) );
    }
}

/**
 * \brief   Two addresses are equal only if both the IP and the port are equal.
 **/
TEST( SocketOsCoverageTest, address_comparison_checks_both_the_ip_and_the_port )
{
    const areg::SocketAddress address( areg::String( LOOPBACK ), 4000u );
    const areg::SocketAddress same( areg::String( LOOPBACK ), 4000u );
    const areg::SocketAddress otherPort( areg::String( LOOPBACK ), 4001u );
    const areg::SocketAddress otherIp( areg::String( "127.0.0.2" ), 4000u );

    EXPECT_TRUE( address == same );
    EXPECT_FALSE( address != same );
    EXPECT_FALSE( address == otherPort );
    EXPECT_TRUE( address != otherPort );
    EXPECT_FALSE( address == otherIp );
    EXPECT_TRUE( address != otherIp );

    EXPECT_TRUE( address.is_equal( areg::String( LOOPBACK ), 4000u ) );
    EXPECT_FALSE( address.is_equal( areg::String( "127.0.0.2" ), 4000u ) );
    EXPECT_FALSE( address.is_equal( areg::String( LOOPBACK ), 4001u ) );

    const areg::SocketAddress named( areg::String( "localhost" ), 4000u );
    EXPECT_TRUE( named.is_equal( areg::String( "localhost" ), 4000u ) );
    EXPECT_FALSE( named.is_equal( areg::String( "localhost" ), 4001u ) );
}

/**
 * \brief   Two credentials are equal only if both the user and the password are equal.
 **/
TEST( SocketOsCoverageTest, user_data_comparison_checks_both_the_user_and_the_password )
{
    const areg::UserData data( areg::String( "user" ), areg::String( "secret" ) );
    const areg::UserData otherPassword( areg::String( "user" ), areg::String( "other" ) );
    const areg::UserData otherUser( areg::String( "other" ), areg::String( "secret" ) );

    EXPECT_FALSE( data == otherPassword );
    EXPECT_TRUE( data != otherPassword );
    EXPECT_FALSE( data == otherUser );
    EXPECT_TRUE( data != otherUser );
    EXPECT_TRUE( data == areg::UserData( data ) );
    EXPECT_FALSE( data != areg::UserData( data ) );
}

/**
 * \brief   An IPv4 text needs exactly three dots and no dot at its end.
 **/
TEST( SocketOsCoverageTest, ip_address_needs_four_numbers )
{
    EXPECT_FALSE( areg::is_ip_address( areg::String( "11.22.33" ) ) );
    EXPECT_FALSE( areg::is_ip_address( areg::String( "1.2.33." ) ) );
    EXPECT_TRUE( areg::is_ip_address( areg::String( "1.22.33.4" ) ) );
}

/**
 * \brief   A receive into an empty buffer reads nothing and leaves the data for the next receive.
 **/
TEST_F( SocketOsLoopTest, receive_into_an_empty_buffer_reads_nothing )
{
    const SOCKETHANDLE handle{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( handle ) );
    uint8_t byte{ 0u };
    EXPECT_EQ( areg::receive_data_window( handle, nullptr, 1u ), 0 );
    EXPECT_EQ( areg::receive_data_window( handle, &byte, 0u ), 0 );
    EXPECT_EQ( areg::receive_data_nowait( handle, nullptr, 1u ), 0 );
    EXPECT_EQ( areg::receive_data_nowait( handle, &byte, 0u ), 0 );
    areg::socket_close( handle );
}

/**
 * \brief   A socket that is not connected cannot stop sending, and a listening socket has
 *          no bytes to read.
 **/
TEST_F( SocketOsLoopTest, unconnected_socket_cannot_stop_sending_and_a_listener_has_nothing_to_read )
{
    const SOCKETHANDLE handle{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( handle ) );
#if !defined(__CYGWIN__)
    // The Cygwin shutdown() accepts a socket that is not connected.
    EXPECT_FALSE( areg::disable_send( handle ) );
#endif  // !defined(__CYGWIN__)
    areg::socket_close( handle );

    areg::SocketAddress address;
    const SOCKETHANDLE listener{ bind_loopback( address, true ) };
    ASSERT_TRUE( areg::is_valid_socket( listener ) );
    EXPECT_EQ( areg::pending_read( listener ), 0u );
    areg::socket_close( listener );
}

/**
 * \brief   A server created by name fills in the address it is bound to; one that cannot be
 *          resolved is not created and leaves the address empty.
 **/
TEST_F( SocketOsLoopTest, server_by_name_fills_in_its_address )
{
    areg::SocketAddress probe;
    const SOCKETHANDLE taken{ bind_loopback( probe, false ) };
    ASSERT_TRUE( areg::is_valid_socket( taken ) );
    const uint16_t port{ probe.host_port( ) };
    areg::socket_close( taken );

    areg::SocketAddress bound;
    const SOCKETHANDLE server{ areg::server_connect( areg::String( ), port, &bound ) };
    ASSERT_TRUE( areg::is_valid_socket( server ) );
    EXPECT_TRUE( bound.is_valid( ) );
    EXPECT_EQ( bound.host_port( ), port );
    EXPECT_TRUE( areg::server_listen( server, 4 ) );
    areg::socket_close( server );

    areg::SocketAddress failed( areg::String( LOOPBACK ), port );
    EXPECT_FALSE( areg::is_valid_socket( areg::server_connect( areg::String( "::1" ), port, &failed ) ) );
    EXPECT_FALSE( failed.is_valid( ) );
}

/**
 * \brief   A client that is refused or cannot resolve its host leaves the address empty; one
 *          that connects without asking for the address still connects.
 **/
TEST_F( SocketOsLoopTest, client_by_name_fills_in_the_address_only_when_it_connects )
{
    areg::SocketAddress address;
    const SOCKETHANDLE listener{ bind_loopback( address, true ) };
    ASSERT_TRUE( areg::is_valid_socket( listener ) );
    const uint16_t port{ address.host_port( ) };

    const SOCKETHANDLE client{ areg::client_connect( areg::String( LOOPBACK ), port, nullptr ) };
    EXPECT_TRUE( areg::is_valid_socket( client ) );
    areg::socket_close( client );
    areg::socket_close( listener );

    areg::SocketAddress peer( areg::String( LOOPBACK ), port );
    EXPECT_FALSE( areg::is_valid_socket( areg::client_connect( areg::String( LOOPBACK ), port, &peer ) ) );
    EXPECT_FALSE( peer.is_valid( ) );
    EXPECT_FALSE( areg::is_valid_socket( areg::client_connect( areg::String( "::1" ), port, &peer ) ) );
    EXPECT_FALSE( peer.is_valid( ) );
}

/**
 * \brief   The send batch is the default when the configured value is above the limit.
 **/
TEST( SocketOsCoverageTest, send_batch_limit_is_the_default_above_its_range )
{
    AREG_EXPECT_IN_CHILD(
        const areg::String path{ write_config( "net::*::*::drain = 100000\n" ) };
        AREG_CHILD_CHECK( areg::Application::config_manager( ).read_config( path ) );
        static_cast<void>(areg::File::delete_file( path ));
        AREG_CHILD_CHECK( areg::Application::config_manager( ).network_drain_limit( ) == 100000u );
        AREG_CHILD_CHECK( areg::send_batch_limit( ) == areg::DEFAULT_DRAIN_LIMIT );
    );
}

/**
 * \brief   The send batch is the default when the configured value is 0.
 **/
TEST( SocketOsCoverageTest, send_batch_limit_is_the_default_when_it_is_zero )
{
    AREG_EXPECT_IN_CHILD(
        const areg::String path{ write_config( "net::*::*::drain = 0\n" ) };
        AREG_CHILD_CHECK( areg::Application::config_manager( ).read_config( path ) );
        static_cast<void>(areg::File::delete_file( path ));
        AREG_CHILD_CHECK( areg::Application::config_manager( ).network_drain_limit( ) == 0u );
        AREG_CHILD_CHECK( areg::send_batch_limit( ) == areg::DEFAULT_DRAIN_LIMIT );
    );
}

#if defined(_POSIX)

/**
 * \brief   A keepalive shorter than its probes gives the shortest idle time and interval.
 **/
TEST( SocketOsCoverageTest, short_keepalive_gives_one_second_idle_and_interval )
{
    AREG_EXPECT_IN_CHILD(
        const areg::String path{ write_config( "net::*::tcpip::keepalive = 3\n" ) };
        AREG_CHILD_CHECK( areg::Application::config_manager( ).read_config( path ) );
        static_cast<void>(areg::File::delete_file( path ));
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        const SOCKETHANDLE handle{ areg::socket_create( ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( handle ) );
        areg::socket_set_no_delay( handle );
        int interval{ 0 };
        socklen_t len{ sizeof( interval ) };
        AREG_CHILD_CHECK( ::getsockopt( handle, IPPROTO_TCP, TCP_KEEPINTVL, &interval, &len ) == 0 );
        AREG_CHILD_CHECK( interval == 1 );
        areg::socket_close( handle );
    );
}

/**
 * \brief   A handle that is not an IPv4 socket has no IPv4 peer address.
 **/
TEST_F( SocketOsLoopTest, peer_of_a_socket_that_is_not_ipv4_is_not_resolved )
{
    int fds[2]{ -1, -1 };
    ASSERT_EQ( ::socketpair( AF_UNIX, SOCK_STREAM, 0, fds ), 0 );
    areg::SocketAddress address( areg::String( LOOPBACK ), 4000u );
    EXPECT_FALSE( address.resolve_socket( static_cast<SOCKETHANDLE>(fds[0]) ) );
    EXPECT_FALSE( address.is_valid( ) );
    ::close( fds[0] );
    ::close( fds[1] );
}

/**
 * \brief   The stateless wait reports a server socket that has no connection to accept as no
 *          socket, also when the clients have nothing to read, and rejects a wrong client list.
 **/
TEST_F( SocketOsLoopTest, stateless_wait_on_a_socket_that_does_not_listen_reports_no_socket )
{
    const SOCKETHANDLE server{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( server ) );
    areg::SocketAddress address;
    const SOCKETHANDLE idle{ bind_loopback( address, true ) };
    ASSERT_TRUE( areg::is_valid_socket( idle ) );

    const areg::SocketMultiplexer mux;
    EXPECT_EQ( mux.wait( server, nullptr, 0, 0 ), areg::InvalidSocketHandle );
    EXPECT_EQ( mux.wait( server, &idle, 1, 0 ), areg::InvalidSocketHandle );
    EXPECT_EQ( mux.wait( server, &idle, -1, 0 ), areg::FailedSocketHandle );
    EXPECT_EQ( mux.wait( server, nullptr, 1, 0 ), areg::FailedSocketHandle );

    EXPECT_EQ( areg::server_accept( server, nullptr, 0 ), areg::FailedSocketHandle );
    areg::socket_close( idle );
    areg::socket_close( server );
}

/**
 * \brief   A multiplexer asked for more connections than the limit is still usable.
 **/
TEST_F( SocketOsLoopTest, multiplexer_above_the_connection_limit_is_usable )
{
    areg::SocketMultiplexer mux( areg::MAX_CONNECTIONS + 1u );
    const SOCKETHANDLE handle{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( handle ) );
    EXPECT_TRUE( mux.register_socket( handle, true ) );
    EXPECT_TRUE( mux.unregister_socket( handle ) );
    areg::socket_close( handle );
}

/**
 * \brief   After the read-ahead cache has served all its bytes, a fill starts at the cache
 *          begin and reports 0 while the peer sent nothing.
 **/
TEST_F( SocketOsLoopTest, fill_after_the_cache_was_served_starts_at_the_begin )
{
    areg::SocketAddress address;
    const SOCKETHANDLE listener{ bind_loopback( address, true ) };
    ASSERT_TRUE( areg::is_valid_socket( listener ) );
    const SOCKETHANDLE client{ areg::client_connect( address ) };
    ASSERT_TRUE( areg::is_valid_socket( client ) );
    const SOCKETHANDLE server{ areg::server_accept( listener, nullptr, 0 ) };
    ASSERT_TRUE( areg::is_valid_socket( server ) );

    const areg::ReceiveMode saved{ areg::receive_mode( ) };
    areg::set_receive_mode( areg::ReceiveMode::MonoCache );
    const uint8_t data[8]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
    ASSERT_EQ( areg::send_data( server, data, sizeof( data ) ), 8 );
    uint8_t received[4]{ };
    EXPECT_EQ( areg::receive_data( client, received, 4u ), 4 );
    EXPECT_EQ( areg::receive_data( client, received, 4u ), 4 );
    EXPECT_EQ( received[3], 8u );
    areg::ThreadCache & cache{ areg::thread_rx_cache( client ) };
    EXPECT_EQ( cache.unread, 0u );
    EXPECT_NE( cache.head, 0u );
    EXPECT_EQ( areg::recv_fill_nowait( client ), 0 );
    EXPECT_EQ( cache.head, 0u );

    ASSERT_EQ( areg::send_data( server, data, 2u ), 2 );
    EXPECT_EQ( areg::receive_data( client, received, 2u ), 2 );
    EXPECT_EQ( received[1], 2u );
    areg::set_receive_mode( saved );

    areg::socket_close( server );
    areg::socket_close( client );
    areg::socket_close( listener );
}

namespace
{
    //!< A connected loopback pair made by areg calls, closed when the object ends.
    struct ConnectedPair
    {
        SOCKETHANDLE    client  { areg::InvalidSocketHandle };
        SOCKETHANDLE    server  { areg::InvalidSocketHandle };

        //!< Connects the pair. Returns false when the system refuses.
        bool open( )
        {
            areg::SocketAddress address;
            const SOCKETHANDLE listener{ bind_loopback( address, true ) };
            client = areg::is_valid_socket( listener ) ? areg::client_connect( address ) : areg::InvalidSocketHandle;
            server = areg::is_valid_socket( client ) ? areg::server_accept( listener, nullptr, 0 ) : areg::InvalidSocketHandle;
            areg::socket_close( listener );
            return areg::is_valid_socket( server );
        }

        ~ConnectedPair( )
        {
            areg::socket_close( client );
            areg::socket_close( server );
        }
    };
}

/**
 * \brief   A peer that resets the connection is not alive.
 **/
TEST_F( SocketOsLoopTest, reset_peer_is_not_alive )
{
    ConnectedPair pair;
    ASSERT_TRUE( pair.open( ) );
    EXPECT_TRUE( areg::is_socket_alive( pair.client ) );
    const linger reset{ 1, 0 };
    ASSERT_EQ( ::setsockopt( pair.server, SOL_SOCKET, SO_LINGER, &reset, sizeof( reset ) ), 0 );
    ::close( pair.server );
    pair.server = areg::InvalidSocketHandle;
    EXPECT_FALSE( areg::is_socket_alive( pair.client ) );
}

/**
 * \brief   A cached receive of a block as large as the cache returns 0 if the peer closed before
 *          sending, and an error if it closed inside the block.
 **/
TEST_F( SocketOsLoopTest, large_cached_receive_tells_an_orderly_close_from_a_cut_block )
{
    const areg::ReceiveMode saved{ areg::receive_mode( ) };
    areg::set_receive_mode( areg::ReceiveMode::MonoCache );
    {
        ConnectedPair pair;
        ASSERT_TRUE( pair.open( ) );
        areg::ThreadCache & cache{ areg::thread_rx_cache( pair.client ) };
        ASSERT_NE( cache.cache( ), nullptr );
        std::vector<uint8_t> buffer( cache.space );
        ASSERT_TRUE( areg::disable_send( pair.server ) );
        EXPECT_EQ( areg::receive_data( pair.client, buffer.data( ), cache.space ), 0 );
    }
    {
        ConnectedPair pair;
        ASSERT_TRUE( pair.open( ) );
        areg::ThreadCache & cache{ areg::thread_rx_cache( pair.client ) };
        ASSERT_NE( cache.cache( ), nullptr );
        std::vector<uint8_t> buffer( cache.space );
        const uint8_t part[16]{ };
        ASSERT_EQ( areg::send_data( pair.server, part, sizeof( part ) ), 16 );
        ASSERT_TRUE( areg::disable_send( pair.server ) );
        EXPECT_LT( areg::receive_data( pair.client, buffer.data( ), cache.space ), 0 );
    }

    areg::set_receive_mode( saved );
}

/**
 * \brief   A socket unregistered while its readiness waits in the batch of the multiplexer is
 *          not reported, and the other sockets of the batch still are.
 **/
TEST_F( SocketOsLoopTest, unregistered_socket_leaves_the_pending_batch )
{
    ConnectedPair pairs[3];
    areg::SocketMultiplexer mux;
    const uint8_t data[1]{ 1u };
    for ( ConnectedPair & pair : pairs )
    {
        ASSERT_TRUE( pair.open( ) );
        ASSERT_EQ( areg::send_data( pair.server, data, 1u ), 1 );
        // Waits until the byte can be read.
        ASSERT_EQ( areg::SocketMultiplexer{ }.wait( pair.client, nullptr, 0, 10'000 ), pair.client );
        ASSERT_TRUE( mux.register_socket( pair.client, true ) );
    }

    const SOCKETHANDLE first{ mux.wait( 0 ) };
    ASSERT_TRUE( areg::is_valid_socket( first ) );
    SOCKETHANDLE removed{ areg::InvalidSocketHandle };
    SOCKETHANDLE kept{ areg::InvalidSocketHandle };
    for ( const ConnectedPair & pair : pairs )
    {
        if ( pair.client == first )
            continue;

        if ( removed == areg::InvalidSocketHandle )
            removed = pair.client;
        else
            kept = pair.client;
    }

    ASSERT_TRUE( mux.unregister_socket( removed ) );
    EXPECT_EQ( mux.wait( 0 ), kept );
    EXPECT_FALSE( mux.is_registered( removed ) );
}

/**
 * \brief   A receive recheck time for an invalid handle changes nothing.
 **/
TEST_F( SocketOsLoopTest, receive_recheck_ignores_an_invalid_handle )
{
    areg::set_receive_recheck( areg::InvalidSocketHandle, 100u );
    ConnectedPair pair;
    ASSERT_TRUE( pair.open( ) );
    areg::set_receive_recheck( pair.client, 1500u );
    timeval tv{ };
    socklen_t len{ sizeof( tv ) };
    ASSERT_EQ( ::getsockopt( pair.client, SOL_SOCKET, SO_RCVTIMEO, &tv, &len ), 0 );
    EXPECT_EQ( tv.tv_sec, 1 );
    EXPECT_EQ( tv.tv_usec, 500000 );
}

#endif  // defined(_POSIX)

#if defined(__linux__)

/**
 * \brief   A socket that is neither connected nor listening is reported once by the multiplexer
 *          and then left out, so it does not wake every wait.
 **/
TEST_F( SocketOsLoopTest, multiplexer_reports_a_hung_up_socket_once )
{
    const SOCKETHANDLE first{ areg::socket_create( ) };
    const SOCKETHANDLE second{ areg::socket_create( ) };
    ASSERT_TRUE( areg::is_valid_socket( first ) && areg::is_valid_socket( second ) );

    areg::SocketMultiplexer mux;
    ASSERT_TRUE( mux.register_socket( first, true ) );
    ASSERT_TRUE( mux.register_socket( second, true ) );
    const SOCKETHANDLE one{ mux.wait( 0 ) };
    const SOCKETHANDLE two{ mux.wait( 0 ) };
    EXPECT_TRUE( ( ( one == first ) && ( two == second ) ) || ( ( one == second ) && ( two == first ) ) );
    EXPECT_EQ( mux.wait( 0 ), areg::InvalidSocketHandle );

    areg::socket_close( first );
    areg::socket_close( second );
}

#endif  // defined(__linux__)
