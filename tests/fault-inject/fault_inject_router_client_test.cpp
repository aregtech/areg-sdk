/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_router_client_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the router client of an application when the system refuses
 *              to send on its connection: a test router accepts the client in a child process.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if defined(__linux__)

#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/persist/ConfigListener.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <thread>

namespace
{
    //!< The longest a test router waits for the client.
    constexpr int IO_WAIT_S { 30 };

    //!< A loopback router socket that accepts one client at a time.
    struct TestRouter
    {
        int         handle  { -1 };
        int         peer    { -1 };
        uint16_t    port    { 0u };

        ~TestRouter( )
        {
            close_peer( );
            if ( handle >= 0 )
                ::close( handle );
        }

        bool open( )
        {
            handle = ::socket( AF_INET, SOCK_STREAM, 0 );
            sockaddr_in addr{ };
            addr.sin_family      = AF_INET;
            addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
            socklen_t len{ sizeof( addr ) };
            if ( ( handle < 0 )
              || ( ::bind( handle, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) != 0 )
              || ( ::listen( handle, 4 ) != 0 )
              || ( ::getsockname( handle, reinterpret_cast<sockaddr *>(&addr), &len ) != 0 ) )
            {
                return false;
            }

            port = ntohs( addr.sin_port );
            return true;
        }

        void close_peer( )
        {
            if ( peer >= 0 )
                ::close( peer );
            peer = -1;
        }

        //!< Accepts the next client, reads its handshake and confirms it with the cookie, sent from the source.
        bool answer( ITEM_ID cookie, ITEM_ID source = areg::COOKIE_ROUTER )
        {
            close_peer( );
            peer = ::accept( handle, nullptr, nullptr );
            if ( peer < 0 )
                return false;

            const timeval timeout{ IO_WAIT_S, 0 };
            areg::MessageEnvelope handshake;
            return ( ::setsockopt( peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) == 0 )
                && receive( handshake )
                && ( handshake.message_id( ) == areg::message_hello_server( ).messageId )
                && send( areg::create_connect_notify( source, cookie ) );
        }

        bool read_all( uint8_t * buffer, uint32_t size )
        {
            uint32_t done{ 0u };
            while ( done < size )
            {
                const ssize_t got{ ::recv( peer, buffer + done, size - done, 0 ) };
                if ( got <= 0 )
                    return false;
                done += static_cast<uint32_t>( got );
            }

            return true;
        }

        bool receive( areg::MessageEnvelope & message )
        {
            areg::EventHeader header{ };
            if ( read_all( areg::wire_header( header ), areg::WIRE_HEADER_SIZE ) == false )
                return false;

            uint8_t * body{ areg::SocketConnectionBase::prepare_message( message, header ) };
            const uint32_t size{ header.bufHeader.biUsed };
            if ( ( body == nullptr ) || ( ( size != 0u ) && ( read_all( body, size ) == false ) ) )
                return false;

            message.set_size_used( size );
            message.move_to_begin( );
            return true;
        }

        bool send( const areg::MessageEnvelope & message )
        {
            const uint32_t size{ areg::wire_size( *message.header( ) ) };
            return ::send( peer, areg::wire_data( *message.header( ) ), size, MSG_NOSIGNAL ) == static_cast<ssize_t>( size );
        }
    };

    //!< Posts a remote request for the stub of the cookie to the dispatcher thread of the router client.
    bool post_remote_request( ITEM_ID stubCookie, uint32_t requestId )
    {
        areg::Thread * thread{ areg::Thread::find_by_address( areg::ThreadAddress( areg::String( "router_CLIENT_DISPATCH_MESSAGE_THREAD" ) ) ) };
        areg::DispatcherThread * dispatcher{ AREG_RUNTIME_CAST( thread, areg::DispatcherThread ) };
        if ( dispatcher == nullptr )
            return false;

        areg::ProxyAddress proxy( 0x613C2001u, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613C3001u, 0x613C4001u, 0x613C1001u );
        areg::StubAddress stub( 0x613C2001u, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613C3002u, 0x613C4002u, 0x613C1002u );
        stub.set_cookie( stubCookie );
        const uint8_t data[]{ 1u, 2u, 3u, 4u };
        areg::RemoteRequestEvent event( areg::SharedBuffer( data, static_cast<uint32_t>( sizeof( data ) ) ), proxy, stub, requestId );
        event.set_target_dispatcher( dispatcher );
        return event.deliver_event( );
    }

    //!< Waits until the router client reports the connection confirmed. Returns false on timeout.
    bool wait_connected( )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg::Application::router_state( ) != areg::RouterState::Connected )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
    }

    //!< Starts the application and connects its router client to the test router.
    bool start_application( TestRouter & router, ITEM_ID cookie )
    {
        areg::Application::setup( false, true, false, true, false );
        areg::ConnectionConfiguration config( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip );
        config.set_connection_enable( true );
        config.set_connection_data( areg::String( "127.0.0.1" ), router.port );
        return areg::Application::connect_router( ) && router.answer( cookie ) && wait_connected( );
    }
}

/**
 * \brief   The system refuses to send a request of the router client on its live connection:
 *          the client closes that connection and connects again, and its next request reaches
 *          the router on the new connection with the new cookie.
 **/
TEST( FaultInjectTest, router_client_reconnects_when_the_system_refuses_to_send_a_request )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 101u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 102u };
        constexpr uint32_t REQUEST{ areg::REQUEST_ID_FIRST + 3u };
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        AREG_CHILD_CHECK( post_remote_request( STUB, REQUEST ) );
        AREG_CHILD_CHECK( router.answer( COOKIE + 1u ) && wait_connected( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );

        AREG_CHILD_CHECK( post_remote_request( STUB, REQUEST + 1u ) );
        areg::MessageEnvelope request;
        AREG_CHILD_CHECK( router.receive( request ) );
        AREG_CHILD_CHECK( request.message_id( ) == REQUEST + 1u );
        AREG_CHILD_CHECK( request.source( ) == COOKIE + 1u );
        AREG_CHILD_CHECK( request.target( ) == STUB );
        areg::Application::release( );
    );
}

namespace
{
    //!< Points the remote logger of the process at the test peer; no router, no log file.
    class RemoteLogConfig final : public areg::ConfigListener
    {
    public:
        explicit RemoteLogConfig( uint16_t port )
            : mPort( port )
        { }

        void apply( areg::ConfigManager & config ) const
        {
            config.set_service_enable( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true );
            config.set_service_enable( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, true, true );
            config.set_service_address( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, areg::String( "127.0.0.1" ), true );
            config.set_service_port( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, mPort, true );
            config.set_log_enabled( areg::LogTarget::Remote, true, true );
            config.set_log_enabled( areg::LogTarget::File, false, true );
        }

    protected:
        void prepare_save_configuration( areg::ConfigManager & /*config*/ ) final
        { }

        void post_save_configuration( areg::ConfigManager & /*config*/ ) final
        { }

        void prepare_read_configuration( areg::ConfigManager & /*config*/ ) final
        { }

        void on_setup_configuration( const areg::ListProperties & /*listReadonly*/, const areg::ListProperties & /*listWritable*/, areg::ConfigManager & /*config*/ ) final
        { }

        void post_read_configuration( areg::ConfigManager & config ) final
        {
            apply( config );
        }

    private:
        const uint16_t mPort;   //!< The port of the test peer.
    };

    //!< Waits until the logger of the process has the cookie. Returns false on timeout.
    bool wait_log_cookie( ITEM_ID cookie )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg::cookie( ) != cookie )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
    }

    //!< Writes one log row of the text through the logger of the process.
    void log_row( const char * text )
    {
        const areg::LogEntry row( areg::LogMessageType::MessageText, areg::LOG_SCOPE_ID_NONE, 0u, 0u, areg::LogPriority::PrioDebug, text, static_cast<uint32_t>( std::strlen( text ) ) );
        areg::log_any_message( row );
    }
}

/**
 * \brief   The system refuses to send a log row of the remote logger on its live connection:
 *          the logger closes that connection and connects again, and its next row reaches the
 *          log collector on the new connection with the new cookie.
 **/
TEST( FaultInjectTest, remote_logger_reconnects_when_the_system_refuses_to_send_a_row )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 111u };
        TestRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );
        RemoteLogConfig config( collector.port );
        areg::Application::setup( true, true, false, true, false, nullptr, &config );
        AREG_CHILD_CHECK( collector.answer( COOKIE, areg::COOKIE_LOGGER ) && wait_log_cookie( COOKIE ) );

        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
            log_row( "refused row" );
            AREG_CHILD_CHECK( collector.answer( COOKIE + 1u, areg::COOKIE_LOGGER ) && wait_log_cookie( COOKIE + 1u ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        }

        log_row( "row after the reconnect" );
        bool received{ false };
        areg::MessageEnvelope message;
        while ( ( received == false ) && collector.receive( message ) )
        {
            received = ( message.message_id( ) == static_cast<uint32_t>( areg::FuncIdRange::ServiceLogMessage ) )
                    && ( message.source( ) == COOKIE + 1u );
        }

        AREG_CHILD_CHECK( received );
        areg::Application::release( );
    );
}

#else   // defined(__linux__)

TEST( FaultInjectTest, router_client_reconnects_when_the_system_refuses_to_send_a_request )
{
    GTEST_SKIP( ) << "the send fault is replaced on Linux only";
}

TEST( FaultInjectTest, remote_logger_reconnects_when_the_system_refuses_to_send_a_row )
{
    GTEST_SKIP( ) << "the send fault is replaced on Linux only";
}

#endif  // defined(__linux__)
