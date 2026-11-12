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
#include "areg/base/SyncPrimitives.hpp"
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
#if AREG_LOGGING
    #include "areglogger/client/LogObserverApi.h"
    #include "areglogger/client/LogObserverBase.hpp"
    #include "areglogger/client/private/LoggerClient.hpp"
#endif  // AREG_LOGGING

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

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

        //!< Accepts the next client and returns true if it closes the connection without a handshake.
        bool handshake_refused( )
        {
            close_peer( );
            peer = ::accept( handle, nullptr, nullptr );
            if ( peer < 0 )
                return false;

            const timeval timeout{ IO_WAIT_S, 0 };
            areg::MessageEnvelope handshake;
            errno = 0;
            return ( ::setsockopt( peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) == 0 )
                && ( receive( handshake ) == false )
                && ( errno != EAGAIN );
        }

        //!< Accepts the next client, answers its handshake with the notice and returns true if the client then closes the connection.
        bool refuse( const areg::MessageEnvelope & notice )
        {
            close_peer( );
            peer = ::accept( handle, nullptr, nullptr );
            if ( peer < 0 )
                return false;

            const timeval timeout{ IO_WAIT_S, 0 };
            areg::MessageEnvelope handshake;
            if ( ( ::setsockopt( peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) != 0 )
              || ( receive( handshake ) == false )
              || ( handshake.message_id( ) != areg::message_hello_server( ).messageId )
              || ( send( notice ) == false ) )
            {
                return false;
            }

            return closed_by_client( );
        }

        //!< Reads until the client closes the connection. Returns false on timeout.
        bool closed_by_client( )
        {
            areg::MessageEnvelope message;
            errno = 0;
            while ( receive( message ) )
            {
            }

            return ( errno != EAGAIN ) && ( errno != EWOULDBLOCK );
        }

        //!< Reads the next message without an areg allocation; returns its header.
        bool receive_raw( areg::EventHeader & header, std::vector<uint8_t> & body )
        {
            if ( read_all( areg::wire_header( header ), areg::WIRE_HEADER_SIZE ) == false )
                return false;

            body.resize( header.bufHeader.biUsed );
            return body.empty( ) || read_all( body.data( ), static_cast<uint32_t>( body.size( ) ) );
        }

        //!< Accepts clients until one sends its handshake, and answers it with the notice built in advance; no areg allocation.
        bool answer_raw( const areg::MessageEnvelope & notice, uint32_t attempts )
        {
            const timeval timeout{ IO_WAIT_S, 0 };
            std::vector<uint8_t> body;
            for ( uint32_t i = 0u; i < attempts; ++ i )
            {
                close_peer( );
                peer = ::accept( handle, nullptr, nullptr );
                if ( ( peer < 0 ) || ( ::setsockopt( peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) != 0 ) )
                    return false;

                areg::EventHeader header{ };
                if ( receive_raw( header, body ) && ( header.messageId == areg::message_hello_server( ).messageId ) )
                    return send( notice );
            }

            return false;
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

    //!< Starts the application and requests the connection of its router client to the test router.
    bool request_connection( TestRouter & router )
    {
        areg::Application::setup( false, true, false, true, false );
        areg::ConnectionConfiguration config( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip );
        config.set_connection_enable( true );
        config.set_connection_data( areg::String( "127.0.0.1" ), router.port );
        return areg::Application::connect_router( );
    }

    //!< Starts the application and connects its router client to the test router.
    bool start_application( TestRouter & router, ITEM_ID cookie )
    {
        return request_connection( router ) && router.answer( cookie ) && wait_connected( );
    }

    //!< A connection notice of the router to the client of the cookie with the state, which may be out of the enumeration.
    areg::MessageEnvelope connection_notice( ITEM_ID cookie, uint16_t state )
    {
        areg::EventHeader hdr{ areg::notify_client_connection( ) };
        hdr.source = static_cast<uint32_t>( areg::COOKIE_ROUTER );
        hdr.target = static_cast<uint32_t>( cookie );
        areg::MessageEnvelope notice;
        if ( notice.init_envelope( hdr, sizeof( ITEM_ID ) + sizeof( areg::ServiceConnectionState ) ) != nullptr )
        {
            notice << cookie;
            notice << static_cast<areg::ServiceConnectionState>( state );
        }

        return notice;
    }

    //!< A connection state that the enumeration does not name.
    constexpr uint16_t UNKNOWN_STATE{ 0x7777u };

    //!< The router answers the handshake with the notice state: the client closes the connection, is not connected, and connects again.
    void handshake_refused_then_connected( uint16_t state, ITEM_ID cookie )
    {
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( request_connection( router ) );
        AREG_CHILD_CHECK( router.refuse( connection_notice( cookie, state ) ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );
        AREG_CHILD_CHECK( router.answer( cookie + 1u ) && wait_connected( ) );
        areg::Application::release( );
    }
}

/**
 * \brief   The router rejects the handshake: the client closes the connection, the router state is
 *          not connected, and the client connects again and is confirmed with the new cookie.
 **/
TEST( FaultInjectTest, router_client_connects_again_after_the_router_rejects_its_handshake )
{
    AREG_EXPECT_IN_CHILD( handshake_refused_then_connected( static_cast<uint16_t>( areg::ServiceConnectionState::Rejected ), areg::COOKIE_REMOTE_SERVICE + 151u ); );
}

/**
 * \brief   The same when the router answers the handshake with its shutdown.
 **/
TEST( FaultInjectTest, router_client_connects_again_after_the_router_answers_its_handshake_with_shutdown )
{
    AREG_EXPECT_IN_CHILD( handshake_refused_then_connected( static_cast<uint16_t>( areg::ServiceConnectionState::Shutdown ), areg::COOKIE_REMOTE_SERVICE + 161u ); );
}

/**
 * \brief   The same when the router answers the handshake with a state the client does not know.
 **/
TEST( FaultInjectTest, router_client_connects_again_after_the_router_answers_its_handshake_with_an_unknown_state )
{
    AREG_EXPECT_IN_CHILD( handshake_refused_then_connected( UNKNOWN_STATE, areg::COOKIE_REMOTE_SERVICE + 171u ); );
}

namespace
{
    //!< Waits until the system refused one send of the process. Returns false on timeout.
    bool wait_send_refused( )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 0u )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
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
        AREG_CHILD_CHECK( wait_send_refused( ) );
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
            AREG_CHILD_CHECK( wait_send_refused( ) );
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

/**
 * \brief   Reproduces the defect: the system refuses to send the handshake of a new connection.
 *          The client closes that connection and connects again.
 **/
TEST( FaultInjectTest, client_connects_again_when_the_system_refuses_its_handshake )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 121u };
        TestRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );
        RemoteLogConfig config( collector.port );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        areg::Application::setup( true, true, false, true, false, nullptr, &config );
        AREG_CHILD_CHECK( collector.handshake_refused( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        AREG_CHILD_CHECK( collector.answer( COOKIE, areg::COOKIE_LOGGER ) && wait_log_cookie( COOKIE ) );
        areg::Application::release( );
    );
}

#if AREG_LOGGING

namespace
{
    //!< What the log observer reported to the application.
    struct ObserverSink
    {
        std::atomic_uint32_t    failed      { 0u };     //!< Calls of the messaging failed notification.
        std::atomic_uint32_t    stopped     { 0u };     //!< Calls of the logging started notification with false.
        std::atomic_uint32_t    connected   { 0u };     //!< Calls of the service connected notification with true.
        areg::SyncEvent         evFailed    { true, true };     //!< Signaled by the messaging failed notification.
        areg::SyncEvent         evStopped   { true, true };     //!< Signaled when the logging is reported as stopped.
        areg::SyncEvent         evConnected { true, true };     //!< Signaled when the connection is reported as established.
    };

    ObserverSink    gSink;

    void sink_failed( )
    {
        gSink.failed.fetch_add( 1u );
        gSink.evFailed.set_signaled( );
    }

    void sink_started( bool isStarted )
    {
        if ( isStarted == false )
        {
            gSink.stopped.fetch_add( 1u );
            gSink.evStopped.set_signaled( );
        }
    }

    void sink_connected( bool isConnected, const char * /*address*/, uint16_t /*port*/ )
    {
        if ( isConnected )
        {
            gSink.connected.fetch_add( 1u );
            gSink.evConnected.set_signaled( );
        }
    }

    //!< Takes the notifications of the observer as an object.
    class SinkObserver final : public areg::logger::LogObserverBase
    {
    public:
        SinkObserver( ) = default;
        ~SinkObserver( ) = default;

    protected:
        void on_log_observer_configured( bool, const std::string &, uint16_t ) override { }
        void on_log_db_configured( bool, const std::string &, const std::string &, const std::string & ) override { }
        void on_log_service_connected( bool isConnected, const std::string & address, uint16_t port ) override { sink_connected( isConnected, address.c_str( ), port ); }
        void on_log_observer_started( bool isStarted ) override { sink_started( isStarted ); }
        void on_log_db_created( const std::string & ) override { }
        void on_log_messaging_failed( ) override { sink_failed( ); }
        void on_log_instances_connect( const std::vector< areg::ConnectedInstance > & ) override { }
        void on_log_instances_disconnect( const std::vector< areg::ConnectedInstance > & ) override { }
        void on_log_service_disconnected( ) override { }
        void on_log_register_scopes( ITEM_ID, const ScopeInfo *, int32_t ) override { }
        void on_log_update_scopes( ITEM_ID, const ScopeInfo *, int32_t ) override { }
        void on_log_message( const areg::MessageEnvelope & ) override { }
        void on_log_source_state( ITEM_ID, areg::LogSourceState, ITEM_ID ) override { }
        void on_log_configuration_restored( ITEM_ID ) override { }
    };

    //!< Waits for the event up to the router timeout.
    bool wait_event( areg::SyncEvent & event )
    {
        return event.lock( static_cast<uint32_t>( IO_WAIT_S ) * 1000u );
    }

    //!< The message the router receives next is a query of the instances sent from the cookie.
    bool receive_instances_query( TestRouter & router, ITEM_ID cookie )
    {
        areg::MessageEnvelope message;
        return router.receive( message )
            && ( message.message_id( ) == areg::message_query_instances( ).messageId )
            && ( message.source( ) == cookie );
    }

    //!< The system refuses a request of the log observer: it reports the failure and the lost connection, then connects again.
    void observer_reports_refused_send( bool asObject )
    {
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 131u };
        TestRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );

        ObserverEvents events{ };
        events.evtMessagingFailed   = &sink_failed;
        events.evtLoggingStarted    = &sink_started;
        events.evtServiceConnected  = &sink_connected;
        SinkObserver object;
        AREG_CHILD_CHECK( asObject ? object.initialize( ) : log_observer_initialize( &events, nullptr ) );
        AREG_CHILD_CHECK( log_observer_connect_logger( nullptr, "127.0.0.1", collector.port ) );
        AREG_CHILD_CHECK( collector.answer( COOKIE, areg::COOKIE_LOGGER ) && wait_event( gSink.evConnected ) );
        AREG_CHILD_CHECK( receive_instances_query( collector, COOKIE ) );
        AREG_CHILD_CHECK( gSink.failed.load( ) == 0u );

        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
            AREG_CHILD_CHECK( log_observer_request_instances( ) );
            AREG_CHILD_CHECK( wait_event( gSink.evFailed ) );
            AREG_CHILD_CHECK( wait_event( gSink.evStopped ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        }

        AREG_CHILD_CHECK( gSink.failed.load( ) >= 1u );
        AREG_CHILD_CHECK( gSink.stopped.load( ) >= 1u );

        AREG_CHILD_CHECK( collector.answer( COOKIE + 1u, areg::COOKIE_LOGGER ) && wait_event( gSink.evConnected ) );
        AREG_CHILD_CHECK( gSink.connected.load( ) == 2u );
        AREG_CHILD_CHECK( receive_instances_query( collector, COOKIE + 1u ) );
        AREG_CHILD_CHECK( gSink.failed.load( ) <= 2u );

        log_observer_disconnect_logger( );
        asObject ? object.release( ) : log_observer_release( );
    }
}

/**
 * \brief   The system refuses to send a request of the log observer on its live connection, and the
 *          observer reports the failure through the callback (the send, and the receive that the closed connection ends) and the loss of the connection. It
 *          connects again with a new cookie, and its next request reaches the collector.
 **/
TEST( FaultInjectTest, log_observer_reports_the_failure_and_connects_again_when_the_system_refuses_a_request_with_callbacks )
{
    AREG_EXPECT_IN_CHILD( observer_reports_refused_send( false ); );
}

/**
 * \brief   The same, when the application takes the notifications as an object.
 **/
TEST( FaultInjectTest, log_observer_reports_the_failure_and_connects_again_when_the_system_refuses_a_request_with_object )
{
    AREG_EXPECT_IN_CHILD( observer_reports_refused_send( true ); );
}

/**
 * \brief   The dispatcher thread of the logger client cannot be created: the connection is refused.
 *          After an attempt refused by the configuration the next request opens the
 *          connection, and a request on a live connection reports success.
 **/
TEST( FaultInjectTest, logger_client_connects_on_a_later_request_after_a_thread_or_the_configuration_refused )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 141u };
        TestRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );
        ObserverEvents events{ };
        events.evtServiceConnected = &sink_connected;
        AREG_CHILD_CHECK( log_observer_initialize( &events, nullptr ) );
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance( ) };
        const areg::String address( "127.0.0.1" );

        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN ) );
            AREG_CHILD_CHECK( client.start_logger_client( areg::String( ), areg::InvalidPort ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u );
        }

        areg::ConnectionConfiguration config( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip );
        config.set_connection_enable( false );
        AREG_CHILD_CHECK( client.start_logger_client( areg::String( ), areg::InvalidPort ) == false );

        config.set_connection_enable( true );
        AREG_CHILD_CHECK( client.start_logger_client( address, collector.port ) );
        AREG_CHILD_CHECK( collector.answer( COOKIE, areg::COOKIE_LOGGER ) && wait_event( gSink.evConnected ) );
        AREG_CHILD_CHECK( gSink.connected.load( ) == 1u );
        AREG_CHILD_CHECK( client.start_logger_client( address, collector.port ) );
        AREG_CHILD_CHECK( gSink.connected.load( ) == 1u );

        client.stop_logger_client( );
        client.close_logging_database( );
        log_observer_release( );
    );
}

#endif  // AREG_LOGGING

#if defined(AREG_FAULT_HELLO)

#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"

namespace
{
    constexpr char const    _helloModel[]   { "FiHelloModel" };
    constexpr char const    _helloRole[]    { "FiRemoteHello" };

    //!< The consumer of a remote hello service; it sends a request when the service connects.
    class FiHelloClient final : public    areg::Component
                              , protected HelloServiceConsumerBase
    {
    public:
        static std::atomic_uint32_t     sConnects;
        static std::atomic_uint32_t     sDisconnects;
        static std::atomic_uint32_t     sFailures;
        static std::atomic<areg::ResultType> sFirstReason;
        static std::atomic_bool         sOwnThread;

        FiHelloClient( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        {
        }

    protected:
        bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override
        {
            const bool result{ HelloServiceConsumerBase::service_connected( status, proxy ) };
            if ( areg::is_service_connected( status ) )
            {
                sConnects.fetch_add( 1u );
                request_hello_service( );
            }
            else
            {
                sDisconnects.fetch_add( 1u );
            }

            return result;
        }

        void request_hello_service_failed( areg::ResultType reason ) override
        {
            if ( sFailures.fetch_add( 1u ) == 0u )
            {
                sFirstReason.store( reason );
                sOwnThread.store( areg::Thread::current_thread( ) == static_cast<areg::Thread *>(&master_thread( )) );
            }
        }
    };

    std::atomic_uint32_t        FiHelloClient::sConnects    { 0u };
    std::atomic_uint32_t        FiHelloClient::sDisconnects { 0u };
    std::atomic_uint32_t        FiHelloClient::sFailures    { 0u };
    std::atomic<areg::ResultType> FiHelloClient::sFirstReason{ areg::ResultType::Undefined };
    std::atomic_bool            FiHelloClient::sOwnThread   { false };

    BEGIN_MODEL( _helloModel )
        BEGIN_REGISTER_THREAD( "FiHelloThread" )
            BEGIN_REGISTER_COMPONENT( "FiHelloClient", FiHelloClient )
                REGISTER_DEPENDENCY( _helloRole )
            END_REGISTER_COMPONENT( "FiHelloClient" )
        END_REGISTER_THREAD( "FiHelloThread" )
    END_MODEL( _helloModel )

    constexpr char const    _witnessModel[] { "FiWitnessModel" };

    //!< A second consumer of the remote hello service, in a thread of its own.
    namespace witness
    {
        BEGIN_MODEL( _witnessModel )
            BEGIN_REGISTER_THREAD( "FiWitnessThread" )
                BEGIN_REGISTER_COMPONENT( "FiWitnessClient", FiHelloClient )
                    REGISTER_DEPENDENCY( _helloRole )
                END_REGISTER_COMPONENT( "FiWitnessClient" )
            END_REGISTER_THREAD( "FiWitnessThread" )
        END_MODEL( _witnessModel )
    }

    //!< The address of a remote provider of the hello service in the process of the cookie.
    areg::StubAddress remote_provider( ITEM_ID cookie )
    {
        const areg::ServiceAddress service( areg::String( HelloService::ServiceName ), HelloService::InterfaceVersion, areg::ServiceType::Public, areg::String( _helloRole ) );
        areg::StubAddress stub( service, areg::item_name_number( areg::String( "FiRemoteHelloThread" ) ), 0x613F0001u );
        stub.set_cookie( cookie );
        return stub;
    }

    //!< Starts the application with the hello consumer and reads its registration at the test router.
    bool start_consumer( TestRouter & router, ITEM_ID cookie )
    {
        areg::MessageEnvelope consumer;
        return start_application( router, cookie )
            && areg::Application::load_model( _helloModel, true )
            && router.receive( consumer )
            && areg::is_register_message( consumer );
    }

    //!< Waits until the counter reaches the value. Returns false on timeout.
    bool wait_count( const std::atomic_uint32_t & counter, uint32_t value )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( counter.load( ) < value )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
    }
}

/**
 * \brief   Reproduces the defect: a registration notice of the router with a provider or
 *          consumer address that is not valid reached an assertion of the service manager, and a
 *          Debug process aborted on its receive thread. Now the notices are ignored, the
 *          connection stays, and a valid notice after them connects the consumer.
 **/
TEST( FaultInjectTest, router_notices_with_invalid_addresses_are_ignored_and_a_valid_one_connects_the_consumer )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 311u };
        constexpr ITEM_ID PROVIDER{ areg::COOKIE_REMOTE_SERVICE + 312u };
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_consumer( router, COOKIE ) );

        areg::StubAddress invalid( areg::String( HelloService::ServiceName ), HelloService::InterfaceVersion, areg::ServiceType::Public
                                 , areg::String( _helloRole ), areg::String( "FiRemoteHelloThread" ) );
        invalid.set_cookie( PROVIDER );
        AREG_CHILD_CHECK( invalid.is_valid( ) == false );
        AREG_CHILD_CHECK( router.send( areg::service_registered_event( invalid, areg::COOKIE_ROUTER, COOKIE ) ) );
        AREG_CHILD_CHECK( router.send( areg::service_unregistered_event( invalid, areg::DisconnectReason::ProviderDisconnected, areg::COOKIE_ROUTER, COOKIE ) ) );
        // A consumer notice whose consumer number is damaged on the wire.
        areg::ProxyAddress consumer( 0x613Au, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613Bu, 0x613Cu, 0x613Du );
        consumer.set_cookie( PROVIDER );
        areg::MessageEnvelope registered{ areg::client_registered_event( consumer, areg::COOKIE_ROUTER, COOKIE ) };
        areg::MessageEnvelope unregistered{ areg::client_unregistered_event( consumer, areg::DisconnectReason::ConsumerDisconnected, areg::COOKIE_ROUTER, COOKIE ) };
        AREG_CHILD_CHECK( ( registered.header( ) != nullptr ) && ( unregistered.header( ) != nullptr ) );
        registered.header( )->consumer.number = areg::CHECKSUM_INVALID;
        unregistered.header( )->consumer.number = areg::CHECKSUM_INVALID;
        AREG_CHILD_CHECK( areg::ProxyAddress( *registered.header( ) ).is_valid( ) == false );
        AREG_CHILD_CHECK( router.send( registered ) && router.send( unregistered ) );
        AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( PROVIDER ), areg::COOKIE_ROUTER, COOKIE ) ) );

        AREG_CHILD_CHECK( wait_count( FiHelloClient::sConnects, 1u ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) == areg::RouterState::Connected );
        areg::MessageEnvelope request;
        AREG_CHILD_CHECK( router.receive( request ) );
        AREG_CHILD_CHECK( request.message_id( ) == static_cast<uint32_t>( HelloService::MessageIDs::MsgId_request_hello_service ) );
        AREG_CHILD_CHECK( request.target( ) == PROVIDER );
        AREG_CHILD_CHECK( FiHelloClient::sFailures.load( ) == 0u );

        areg::Application::unload_model( _helloModel );
        areg::Application::release( );
    );
}

namespace
{
    //!< Reads the messages of the client until a registration of a consumer, without an areg allocation.
    bool receive_consumer_registration( TestRouter & router, ITEM_ID cookie )
    {
        areg::EventHeader header{ };
        std::vector<uint8_t> body;
        while ( router.receive_raw( header, body ) )
        {
            if ( ( header.messageId == static_cast<uint32_t>( areg::FuncIdRange::SystemServiceRequestRegister ) )
              && ( static_cast<areg::RegistrationAction>( header.callType ) == areg::RegistrationAction::RegisterConsumer ) )
            {
                return header.source == static_cast<uint32_t>( cookie );
            }
        }

        return false;
    }

    //!< Receives messages of the client until one is a hello request to the provider of the cookie. Returns false on a timeout.
    bool receive_request_to( TestRouter & router, ITEM_ID provider )
    {
        for ( uint32_t i = 0u; i < 64u; ++ i )
        {
            areg::MessageEnvelope message;
            if ( router.receive( message ) == false )
                return false;
            if ( ( message.message_id( ) == static_cast<uint32_t>( HelloService::MessageIDs::MsgId_request_hello_service ) ) && ( message.target( ) == provider ) )
                return true;
        }

        return false;
    }

    //!< Waits until an allocation failed or the counter passed the value. Returns false on timeout.
    bool wait_failed_or_count( const std::atomic_uint32_t & counter, uint32_t value )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( ( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 0u ) && ( counter.load( ) <= value ) )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
    }
}

/**
 * \brief   Reproduces the defect: the unregister of a remote provider whose event could not be
 *          built was lost, and a consumer created later was connected to the provider that was gone.
 *          Expected: after any one failed allocation, a new consumer is connected only to the
 *          provider registered next.
 **/
TEST( FaultInjectTest, bug_918_a_failed_allocation_of_a_remote_provider_unregister_leaves_no_stale_entry_in_the_registry )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 321u };
        constexpr ITEM_ID PROVIDER{ areg::COOKIE_REMOTE_SERVICE + 1000u };
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );

        areg_fault::FaultScope faults;
        uint32_t failures{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < 200u ) && ( finished == false ); ++ skip )
        {
            const ITEM_ID provider{ PROVIDER + 2u * skip };
            AREG_CHILD_CHECK( areg::Application::load_model( _helloModel, true ) );
            AREG_CHILD_CHECK( receive_consumer_registration( router, COOKIE ) );
            AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( provider ), areg::COOKIE_ROUTER, COOKIE ) ) );
            AREG_CHILD_CHECK( receive_request_to( router, provider ) );

            areg::MessageEnvelope unregistered{ areg::service_unregistered_event( remote_provider( provider ), areg::DisconnectReason::ProviderDisconnected, areg::COOKIE_ROUTER, COOKIE ) };
            const uint32_t disconnects{ FiHelloClient::sDisconnects.load( ) };
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            AREG_CHILD_CHECK( router.send( unregistered ) );
            const bool reached{ wait_failed_or_count( FiHelloClient::sDisconnects, disconnects ) };
            const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
            ::areg_fault_disarm_all( );
            failures += failed;
            finished = ( failed == 0u );
            AREG_CHILD_CHECK( reached );

            // The first consumer keeps the entry of the provider; a new consumer must not be connected to it.
            const ITEM_ID next{ provider + 1u };
            AREG_CHILD_CHECK( areg::Application::load_model( _witnessModel, true ) );
            AREG_CHILD_CHECK( receive_consumer_registration( router, COOKIE ) );
            AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( next ), areg::COOKIE_ROUTER, COOKIE ) ) );
            AREG_CHILD_CHECK( receive_request_to( router, next ) );
            AREG_CHILD_CHECK( areg::Application::unload_model( _witnessModel ) );
            AREG_CHILD_CHECK( areg::Application::unload_model( _helloModel ) );
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( failures != 0u );
        AREG_CHILD_CHECK( areg::Application::router_state( ) == areg::RouterState::Connected );
        areg::Application::release( );
    );
}

/**
 * \brief   The system refuses to send the request of a consumer to its remote provider: the
 *          consumer gets the request failure MessageUndelivered on its own thread, and the router
 *          client connects again.
 **/
TEST( FaultInjectTest, request_to_a_remote_provider_whose_send_fails_reaches_the_consumer_as_undelivered )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 301u };
        constexpr ITEM_ID PROVIDER{ areg::COOKIE_REMOTE_SERVICE + 302u };
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_consumer( router, COOKIE ) );

        // The first send is the notice of the test router, the next one is the request of the client.
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET, 1u ) );
        AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( PROVIDER ), areg::COOKIE_ROUTER, COOKIE ) ) );

        AREG_CHILD_CHECK( wait_count( FiHelloClient::sFailures, 1u ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        AREG_CHILD_CHECK( FiHelloClient::sConnects.load( ) == 1u );
        AREG_CHILD_CHECK( FiHelloClient::sFirstReason.load( ) == areg::ResultType::MessageUndelivered );
        AREG_CHILD_CHECK( FiHelloClient::sOwnThread.load( ) );
        AREG_CHILD_CHECK( router.answer( COOKIE + 1u ) && wait_connected( ) );

        areg::Application::unload_model( _helloModel );
        areg::Application::release( );
    );
}

namespace
{
    //!< The router ends the live connection with the notice state while a request of the consumer waits for its response.
    void pending_request_fails_when_the_router_ends_the_connection( uint16_t state, ITEM_ID cookie )
    {
        const ITEM_ID PROVIDER{ cookie + 1u };
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_consumer( router, cookie ) );
        AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( PROVIDER ), areg::COOKIE_ROUTER, cookie ) ) );
        areg::MessageEnvelope request;
        AREG_CHILD_CHECK( router.receive( request ) );
        AREG_CHILD_CHECK( request.message_id( ) == static_cast<uint32_t>( HelloService::MessageIDs::MsgId_request_hello_service ) );

        AREG_CHILD_CHECK( router.send( connection_notice( cookie, state ) ) );
        AREG_CHILD_CHECK( router.closed_by_client( ) );
        AREG_CHILD_CHECK( wait_count( FiHelloClient::sFailures, 1u ) );
        AREG_CHILD_CHECK( FiHelloClient::sFirstReason.load( ) == areg::ResultType::ServiceUnavailable );
        AREG_CHILD_CHECK( FiHelloClient::sOwnThread.load( ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );

        // The client connects again and registers its consumer on the new connection.
        AREG_CHILD_CHECK( router.answer( cookie + 2u ) && wait_connected( ) );
        areg::MessageEnvelope consumer;
        AREG_CHILD_CHECK( router.receive( consumer ) && areg::is_register_message( consumer ) );
        AREG_CHILD_CHECK( consumer.source( ) == cookie + 2u );
        AREG_CHILD_CHECK( FiHelloClient::sFailures.load( ) == 1u );

        areg::Application::unload_model( _helloModel );
        areg::Application::release( );
    }
}

/**
 * \brief   The router rejects the live connection while a request waits for its response: the
 *          request fails to the consumer with ServiceUnavailable on its own thread, and the client
 *          connects again and registers the consumer.
 **/
TEST( FaultInjectTest, pending_request_fails_and_the_client_connects_again_when_the_router_rejects_the_connection )
{
    AREG_EXPECT_IN_CHILD( pending_request_fails_when_the_router_ends_the_connection( static_cast<uint16_t>( areg::ServiceConnectionState::Rejected ), areg::COOKIE_REMOTE_SERVICE + 321u ); );
}

/**
 * \brief   The same when the router reports its shutdown on the live connection.
 **/
TEST( FaultInjectTest, pending_request_fails_and_the_client_connects_again_when_the_router_shuts_down )
{
    AREG_EXPECT_IN_CHILD( pending_request_fails_when_the_router_ends_the_connection( static_cast<uint16_t>( areg::ServiceConnectionState::Shutdown ), areg::COOKIE_REMOTE_SERVICE + 331u ); );
}

/**
 * \brief   The same when the router sends a connection state the client does not know.
 **/
TEST( FaultInjectTest, pending_request_fails_and_the_client_connects_again_on_an_unknown_connection_state )
{
    AREG_EXPECT_IN_CHILD( pending_request_fails_when_the_router_ends_the_connection( UNKNOWN_STATE, areg::COOKIE_REMOTE_SERVICE + 341u ); );
}

#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"

namespace
{
    constexpr char const    _localModel[]   { "FiLocalModel" };
    constexpr char const    _localRole[]    { "FiLocalHello" };

    //!< A local provider of the hello service; it answers every request.
    class FiHelloService final : public    areg::Component
                               , protected HelloServiceProviderBase
    {
    public:
        static std::atomic_uint32_t     sRequests;

        FiHelloService( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
        {
        }

    protected:
        void request_hello_service( ) override
        {
            sRequests.fetch_add( 1u );
            response_hello_service( );
        }
    };

    //!< A consumer of the local provider; it sends a request when connected and on each call of ask().
    class FiLocalClient final : public    areg::Component
                              , protected HelloServiceConsumerBase
    {
    public:
        static FiLocalClient *          sInstance;
        static std::atomic_uint32_t     sConnects;
        static std::atomic_uint32_t     sDisconnects;
        static std::atomic_uint32_t     sResponses;
        static std::atomic_uint32_t     sFailures;

        FiLocalClient( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        {
            sInstance = this;
        }

        ~FiLocalClient( ) override
        {
            sInstance = nullptr;
        }

        //!< Sends a request; called on the thread of the component.
        void ask( )
        {
            request_hello_service( );
        }

    protected:
        bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override
        {
            const bool result{ HelloServiceConsumerBase::service_connected( status, proxy ) };
            if ( areg::is_service_connected( status ) )
            {
                sConnects.fetch_add( 1u );
                request_hello_service( );
            }
            else
            {
                sDisconnects.fetch_add( 1u );
            }

            return result;
        }

        void response_hello_service( ) override
        {
            sResponses.fetch_add( 1u );
        }

        void request_hello_service_failed( areg::ResultType /*reason*/ ) override
        {
            sFailures.fetch_add( 1u );
        }
    };

    //!< A consumer of the remote role; its connection asks the local consumer of the same thread for one more request.
    class FiBarrierClient final : public    areg::Component
                                , protected HelloServiceConsumerBase
    {
    public:
        static std::atomic_uint32_t     sConnects;

        FiBarrierClient( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        {
        }

    protected:
        bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override
        {
            const bool result{ HelloServiceConsumerBase::service_connected( status, proxy ) };
            if ( areg::is_service_connected( status ) )
            {
                if ( FiLocalClient::sInstance != nullptr )
                    FiLocalClient::sInstance->ask( );
                sConnects.fetch_add( 1u );
            }

            return result;
        }
    };

    std::atomic_uint32_t    FiHelloService::sRequests       { 0u };
    FiLocalClient *         FiLocalClient::sInstance        { nullptr };
    std::atomic_uint32_t    FiLocalClient::sConnects        { 0u };
    std::atomic_uint32_t    FiLocalClient::sDisconnects     { 0u };
    std::atomic_uint32_t    FiLocalClient::sResponses       { 0u };
    std::atomic_uint32_t    FiLocalClient::sFailures        { 0u };
    std::atomic_uint32_t    FiBarrierClient::sConnects      { 0u };

    //!< Reads the messages of the client until the registration of its provider; returns its address.
    bool receive_provider_registration( TestRouter & router, areg::StubAddress & provider )
    {
        areg::MessageEnvelope message;
        while ( router.receive( message ) )
        {
            if ( areg::is_register_message( message ) && ( static_cast<areg::RegistrationAction>( message.call_type( ) ) == areg::RegistrationAction::RegisterProvider ) )
            {
                provider = areg::StubAddress( *message.header( ) );
                return true;
            }
        }

        return false;
    }
}

/**
 * \brief   The router rejects a public provider of this process, with the cookie of this process:
 *          the provider keeps serving its local consumer, which stays connected and gets the
 *          response of a request sent after the rejection, and the router connection stays.
 **/
TEST( FaultInjectTest, provider_rejected_by_the_router_keeps_serving_the_consumers_of_its_process )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 351u };
        constexpr ITEM_ID PROVIDER{ areg::COOKIE_REMOTE_SERVICE + 352u };
        BEGIN_MODEL_LOCAL( _localModel )
            BEGIN_REGISTER_THREAD( "FiLocalThread" )
                BEGIN_REGISTER_COMPONENT( _localRole, FiHelloService )
                    REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
                END_REGISTER_COMPONENT( _localRole )
                BEGIN_REGISTER_COMPONENT( "FiLocalClient", FiLocalClient )
                    REGISTER_DEPENDENCY( _localRole )
                END_REGISTER_COMPONENT( "FiLocalClient" )
                BEGIN_REGISTER_COMPONENT( "FiBarrierClient", FiBarrierClient )
                    REGISTER_DEPENDENCY( _helloRole )
                END_REGISTER_COMPONENT( "FiBarrierClient" )
            END_REGISTER_THREAD( "FiLocalThread" )
        END_MODEL_LOCAL( _localModel )
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );
        AREG_CHILD_CHECK( areg::Application::load_model( _localModel, true ) );

        areg::StubAddress local;
        AREG_CHILD_CHECK( receive_provider_registration( router, local ) );
        AREG_CHILD_CHECK( wait_count( FiLocalClient::sResponses, 1u ) );
        local.set_cookie( COOKIE );
        AREG_CHILD_CHECK( router.send( areg::service_unregistered_event( local, areg::DisconnectReason::ProviderRejected, areg::COOKIE_ROUTER, COOKIE ) ) );

        // The notice after the rejection takes the same path; its consumer asks the local one for a request.
        AREG_CHILD_CHECK( router.send( areg::service_registered_event( remote_provider( PROVIDER ), areg::COOKIE_ROUTER, COOKIE ) ) );
        AREG_CHILD_CHECK( wait_count( FiBarrierClient::sConnects, 1u ) );
        AREG_CHILD_CHECK( wait_count( FiLocalClient::sResponses, 2u ) );
        AREG_CHILD_CHECK( FiHelloService::sRequests.load( ) == 2u );
        AREG_CHILD_CHECK( FiLocalClient::sConnects.load( ) == 1u );
        AREG_CHILD_CHECK( FiLocalClient::sDisconnects.load( ) == 0u );
        AREG_CHILD_CHECK( FiLocalClient::sFailures.load( ) == 0u );
        AREG_CHILD_CHECK( areg::Application::router_state( ) == areg::RouterState::Connected );

        areg::Application::unload_model( _localModel );
        areg::Application::release( );
    );
}

// The sanitizer runtimes define the global allocation functions themselves.
#if !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__) && !( defined(__has_feature) && ( __has_feature(address_sanitizer) || __has_feature(thread_sanitizer) || __has_feature(memory_sanitizer) ) )

namespace
{
    //!< The most allocations of one connect and registration that are failed in turn.
    constexpr uint32_t      CONNECT_ATTEMPTS    { 2'000u };
    //!< The connections a test router accepts until one sends its handshake.
    constexpr uint32_t      ACCEPT_ATTEMPTS     { 16u };

    //!< Waits until the router client reports the state. Returns false on timeout.
    bool wait_router_state( areg::RouterState state )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg::Application::router_state( ) != state )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }

        return true;
    }

    //!< Answers the next client with the notice; while none connects, requests the connection again. Returns false on timeout.
    bool accept_requesting_again( TestRouter & router, const areg::MessageEnvelope & notice )
    {
        constexpr int POLL_MS{ 50 };
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( std::chrono::steady_clock::now( ) < deadline )
        {
            pollfd listener{ router.handle, POLLIN, 0 };
            if ( ::poll( &listener, 1, POLL_MS ) > 0 )
                return router.answer_raw( notice, ACCEPT_ATTEMPTS );

            // A start that failed after the request leaves the client disconnected; a started one is kept.
            static_cast<void>( areg::Application::connect_router( ) );
        }

        return false;
    }

    //!< Fails each allocation of the point in a connect of the router client and the registration of a consumer in turn.
    void connect_and_register_survive( areg_fault::FaultPoint point )
    {
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        const timeval timeout{ IO_WAIT_S, 0 };
        AREG_CHILD_CHECK( ::setsockopt( router.handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) == 0 );
        AREG_CHILD_CHECK( request_connection( router ) );
        areg::Application::disconnect_router( );
        AREG_CHILD_CHECK( wait_router_state( areg::RouterState::Disconnected ) );

        areg_fault::FaultScope faults;
        uint32_t refused{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < CONNECT_ATTEMPTS ) && ( finished == false ); ++ skip )
        {
            const ITEM_ID cookie{ areg::COOKIE_REMOTE_SERVICE + 1000u + skip };
            const areg::MessageEnvelope notice{ areg::create_connect_notify( areg::COOKIE_ROUTER, cookie ) };
            AREG_CHILD_CHECK( notice.is_valid( ) );

            AREG_CHILD_CHECK( areg_fault::arm( point, 0, skip, 1u ) );
            bool requested{ areg::Application::connect_router( ) };
            if ( requested == false )
            {
                // A start request that cannot be built changes nothing; the next one connects.
                ++ refused;
                AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );
                ::areg_fault_disarm_all( );
                requested = areg::Application::connect_router( );
            }

            AREG_CHILD_CHECK( requested );
            AREG_CHILD_CHECK( accept_requesting_again( router, notice ) && wait_connected( ) );
            const bool loaded{ areg::Application::load_model( _helloModel, true ) };
            if ( loaded )
            {
                AREG_CHILD_CHECK( receive_consumer_registration( router, cookie ) );
            }
            else
            {
                ++ refused;
                AREG_CHILD_CHECK( areg::Application::model_state( _helloModel ) == areg::ModelState::Unloaded );
            }

            const uint32_t failed{ areg_fault::failed( point ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );

            if ( areg::Application::model_state( _helloModel ) != areg::ModelState::Unloaded )
            {
                AREG_CHILD_CHECK( areg::Application::unload_model( _helloModel ) );
            }

            areg::Application::disconnect_router( );
            AREG_CHILD_CHECK( router.closed_by_client( ) );
            AREG_CHILD_CHECK( wait_router_state( areg::RouterState::Disconnected ) );
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( refused != 0u );
        areg::Application::release( );
    }
}

namespace
{
    //!< Reads the messages of the client until it closes, without an areg allocation. Returns false on timeout.
    bool closed_by_client_raw( TestRouter & router )
    {
        areg::EventHeader header{ };
        std::vector<uint8_t> body;
        errno = 0;
        while ( router.receive_raw( header, body ) )
        {
        }

        return ( errno != EAGAIN ) && ( errno != EWOULDBLOCK );
    }

    //!< Answers each client that connects with the notice and requests the connection, until one is connected. Returns false on timeout.
    bool connect_answering( TestRouter & router, const areg::MessageEnvelope & notice )
    {
        constexpr int POLL_MS{ 50 };
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg::Application::router_state( ) != areg::RouterState::Connected )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            pollfd listener{ router.handle, POLLIN, 0 };
            if ( ::poll( &listener, 1, POLL_MS ) > 0 )
            {
                // A client that dropped its connection connects again by itself.
                static_cast<void>( router.answer_raw( notice, 1u ) );
            }
            else
            {
                static_cast<void>( areg::Application::connect_router( ) );
            }
        }

        return true;
    }

    //!< Fails each allocation of the point in a disconnect of the router client and the connect after it in turn.
    void disconnect_and_connect_survive( areg_fault::FaultPoint point )
    {
        TestRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        const timeval timeout{ IO_WAIT_S, 0 };
        AREG_CHILD_CHECK( ::setsockopt( router.handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) == 0 );
        AREG_CHILD_CHECK( request_connection( router ) );
        const areg::MessageEnvelope first{ areg::create_connect_notify( areg::COOKIE_ROUTER, areg::COOKIE_REMOTE_SERVICE + 999u ) };
        AREG_CHILD_CHECK( first.is_valid( ) );
        AREG_CHILD_CHECK( accept_requesting_again( router, first ) && wait_connected( ) );

        areg_fault::FaultScope faults;
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < CONNECT_ATTEMPTS ) && ( finished == false ); ++ skip )
        {
            const ITEM_ID cookie{ areg::COOKIE_REMOTE_SERVICE + 1000u + skip };
            const areg::MessageEnvelope notice{ areg::create_connect_notify( areg::COOKIE_ROUTER, cookie ) };
            AREG_CHILD_CHECK( notice.is_valid( ) );

            // The fault stays armed over the stop of the client, which ends after the router saw the close.
            AREG_CHILD_CHECK( areg_fault::arm( point, 0, skip, 1u ) );
            areg::Application::disconnect_router( );
            AREG_CHILD_CHECK( closed_by_client_raw( router ) );
            static_cast<void>( areg::Application::connect_router( ) );
            AREG_CHILD_CHECK( connect_answering( router, notice ) );

            finished = ( areg_fault::failed( point ) == 0u );
            ::areg_fault_disarm_all( );
        }

        AREG_CHILD_CHECK( finished );
        areg::Application::disconnect_router( );
        AREG_CHILD_CHECK( closed_by_client_raw( router ) );
        areg::Application::release( );
    }
}

/**
 * \brief   Reproduces the defect: a start request of the router client whose data was half written
 *          was reported as requested and then dropped. Each nothrow allocation of a connect of the
 *          router client and of the registration of a consumer fails in turn: a refused request
 *          changes nothing and the next one connects, a refused load leaves the model unloaded,
 *          and a loaded consumer is registered at the router.
 **/
TEST( FaultInjectTest, bug_919_every_failed_allocation_of_a_router_connect_and_registration_is_survived )
{
    AREG_EXPECT_IN_CHILD( connect_and_register_survive( areg_fault::FaultPoint::Allocation ); );
}

/**
 * \brief   Reproduces the defect: the stop report of the router client was lost when its event
 *          could not be allocated, and no later request connected the client. Each nothrow
 *          allocation of a disconnect and of the connect after it fails in turn: the router sees
 *          the close, and the client connects again.
 **/
TEST( FaultInjectTest, bug_919_every_failed_allocation_of_a_router_disconnect_is_followed_by_a_connection )
{
    AREG_EXPECT_IN_CHILD( disconnect_and_connect_survive( areg_fault::FaultPoint::Allocation ); );
}

TEST( FaultInjectTest, every_failed_aligned_allocation_of_a_router_connect_and_registration_is_survived )
{
    AREG_EXPECT_IN_CHILD( connect_and_register_survive( areg_fault::FaultPoint::AlignedAllocation ); );
}

#endif  // !sanitizer

#endif  // defined(AREG_FAULT_HELLO)

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
