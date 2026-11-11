/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_ipc_more_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the router client against a peer that sends unexpected or damaged
 *              messages, closes the connection in the middle of a message, never answers,
 *              or stops reading; a test router in the test process, the client in a child process.
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
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/logging/LoggingDefs.hpp"
#if AREG_LOGGING
    #include "areglogger/client/LogObserverApi.h"
#endif  // AREG_LOGGING

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{
    //!< The longest a test waits for the client; a hang guard only.
    constexpr int IO_WAIT_S { 30 };

    //!< A message id that is neither executable nor known to the router client.
    constexpr uint32_t UNKNOWN_ID { 0x00000007u };
    static_assert( areg::is_executable_id( UNKNOWN_ID ) == false, "the unknown id must not be executable" );

    //!< A loopback router socket that serves one client at a time.
    struct PeerRouter
    {
        int         handle  { -1 };
        int         peer    { -1 };
        uint16_t    port    { 0u };
        bool        eof     { false };  //!< The last read ended because the client closed the connection.

        ~PeerRouter( )
        {
            close_peer( );
            if ( handle >= 0 )
                ::close( handle );
        }

        //!< Opens the listening socket; a nonzero receive buffer limits what an unread connection holds.
        bool open( int receiveBuffer = 0 )
        {
            handle = ::socket( AF_INET, SOCK_STREAM, 0 );
            if ( ( receiveBuffer != 0 ) && ( ::setsockopt( handle, SOL_SOCKET, SO_RCVBUF, &receiveBuffer, sizeof( receiveBuffer ) ) != 0 ) )
                return false;

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

        bool accept_client( )
        {
            close_peer( );
            peer = ::accept( handle, nullptr, nullptr );
            const timeval timeout{ IO_WAIT_S, 0 };
            return ( peer >= 0 ) && ( ::setsockopt( peer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof( timeout ) ) == 0 );
        }

        //!< Accepts the next client, reads its handshake and confirms it with the cookie.
        bool answer( ITEM_ID cookie )
        {
            areg::MessageEnvelope handshake;
            return accept_client( )
                && receive( handshake )
                && ( handshake.message_id( ) == areg::message_hello_server( ).messageId )
                && send( areg::create_connect_notify( areg::COOKIE_ROUTER, cookie ) );
        }

        bool read_all( uint8_t * buffer, uint32_t size )
        {
            uint32_t done{ 0u };
            while ( done < size )
            {
                const ssize_t got{ ::recv( peer, buffer + done, size - done, 0 ) };
                eof = ( got == 0 );
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

        bool send_raw( const void * data, size_t size )
        {
            return ::send( peer, data, size, MSG_NOSIGNAL ) == static_cast<ssize_t>( size );
        }

        bool send( const areg::MessageEnvelope & message )
        {
            return send_raw( areg::wire_data( *message.header( ) ), areg::wire_size( *message.header( ) ) );
        }

        //!< Reads until the client closes the connection; false if it sends a wrong byte count or times out.
        bool drain_until_closed( )
        {
            uint8_t buffer[512];
            for ( ;; )
            {
                const ssize_t got{ ::recv( peer, buffer, sizeof( buffer ), 0 ) };
                if ( got == 0 )
                    return true;
                if ( got < 0 )
                    return false;
            }
        }
    };

    areg::DispatcherThread * client_dispatcher( )
    {
        areg::Thread * thread{ areg::Thread::find_by_address( areg::ThreadAddress( areg::String( "router_CLIENT_DISPATCH_MESSAGE_THREAD" ) ) ) };
        return AREG_RUNTIME_CAST( thread, areg::DispatcherThread );
    }

    //!< Posts a remote request with the payload for the stub of the cookie to the router client.
    bool post_request( ITEM_ID stubCookie, uint32_t requestId, uint32_t payloadSize = 4u )
    {
        areg::DispatcherThread * dispatcher{ client_dispatcher( ) };
        if ( dispatcher == nullptr )
            return false;

        areg::ProxyAddress proxy( 0x613C2001u, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613C3001u, 0x613C4001u, 0x613C1001u );
        areg::StubAddress stub( 0x613C2001u, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613C3002u, 0x613C4002u, 0x613C1002u );
        stub.set_cookie( stubCookie );
        const std::vector<uint8_t> data( payloadSize, 0x5Au );
        areg::RemoteRequestEvent event( areg::SharedBuffer( data.data( ), payloadSize ), proxy, stub, requestId );
        event.set_target_dispatcher( dispatcher );
        return event.deliver_event( );
    }

    //!< Waits until the router client reports the state. Returns false on timeout.
    bool wait_router_state( areg::RouterState state )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg::Application::router_state( ) != state )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::yield( );
        }

        return true;
    }

    //!< Waits until the system refused one thread of the process. Returns false on timeout.
    bool wait_thread_refused( )
    {
        const auto deadline{ std::chrono::steady_clock::now( ) + std::chrono::seconds( IO_WAIT_S ) };
        while ( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 0u )
        {
            if ( std::chrono::steady_clock::now( ) > deadline )
                return false;

            std::this_thread::yield( );
        }

        return true;
    }

    void configure_router( uint16_t port )
    {
        areg::ConnectionConfiguration config( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip );
        config.set_connection_enable( true );
        config.set_connection_data( areg::String( "127.0.0.1" ), port );
    }

    //!< Starts the application and connects its router client to the peer router with the cookie.
    bool start_application( PeerRouter & router, ITEM_ID cookie )
    {
        areg::Application::setup( false, true, false, true, false );
        configure_router( router.port );
        return areg::Application::connect_router( ) && router.answer( cookie ) && wait_router_state( areg::RouterState::Connected );
    }

    //!< Builds a message from the router to the client with the header fields of the test.
    areg::MessageEnvelope router_message( uint32_t messageId, areg::EventType type, uint32_t target, uint32_t payload = 0u )
    {
        areg::EventHeader hdr{ };
        hdr.messageId = messageId;
        hdr.eventType = static_cast<uint16_t>( type );
        hdr.source    = static_cast<uint32_t>( areg::COOKIE_ROUTER );
        hdr.target    = target;
        areg::MessageEnvelope message;
        message.init_envelope( hdr, payload );
        if ( payload != 0u )
            message.set_size_used( payload );
        return message;
    }

    //!< The message the client sends next is the request of the cookie source for the stub; the connection is the one of the cookie.
    bool request_arrives( PeerRouter & router, ITEM_ID cookie, ITEM_ID stub, uint32_t requestId )
    {
        areg::MessageEnvelope request;
        return post_request( stub, requestId )
            && router.receive( request )
            && ( request.message_id( ) == requestId )
            && ( request.source( ) == cookie )
            && ( request.target( ) == stub );
    }
}

/**
 * \brief   The router sends a registration notification of an unknown type, one for another connection,
 *          a request that no stub serves, a message of an unknown kind and one of an unknown event type. The client ignores each of
 *          them: it stays on the same connection with the same cookie, and its next request reaches the router.
 **/
TEST( FaultInjectIpcMoreTest, router_client_ignores_unexpected_messages_and_keeps_the_connection )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 201u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 202u };
        constexpr uint32_t REQUEST{ areg::REQUEST_ID_FIRST + 5u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );

        areg::StubAddress stub( 0x613C2001u, areg::Version( 1u, 0u, 0u ), areg::ServiceType::Public, 0x613C3002u, 0x613C4002u, 0x613C1002u );
        areg::MessageEnvelope unknownType{ areg::service_registered_event( stub, areg::COOKIE_ROUTER, COOKIE ) };
        AREG_CHILD_CHECK( unknownType.is_valid( ) );
        unknownType.header( )->callType = 0xEEu;
        AREG_CHILD_CHECK( router.send( unknownType ) );

        const areg::MessageEnvelope otherConnection{ areg::service_unregistered_event( stub, areg::DisconnectReason::ProviderRejected, areg::COOKIE_ROUTER, COOKIE + 77u ) };
        AREG_CHILD_CHECK( otherConnection.is_valid( ) );
        AREG_CHILD_CHECK( router.send( otherConnection ) );

        const areg::MessageEnvelope ownRejected{ areg::service_unregistered_event( stub, areg::DisconnectReason::ProviderRejected, areg::COOKIE_ROUTER, COOKIE ) };
        AREG_CHILD_CHECK( router.send( ownRejected ) );

        const areg::MessageEnvelope unroutable{ router_message( areg::REQUEST_ID_FIRST + 9u, areg::EventType::EventRemoteRequest, COOKIE, 4u ) };
        AREG_CHILD_CHECK( router.send( unroutable ) );
        const areg::MessageEnvelope unknownKind{ router_message( UNKNOWN_ID, areg::EventType::EventRemoteResponse, COOKIE ) };
        AREG_CHILD_CHECK( router.send( unknownKind ) );
        areg::MessageEnvelope unknownEvent{ router_message( areg::REQUEST_ID_FIRST + 9u, areg::EventType::EventRemoteRequest, COOKIE, 4u ) };
        unknownEvent.header( )->eventType = 0xFFFFu;
        AREG_CHILD_CHECK( router.send( unknownEvent ) );

        AREG_CHILD_CHECK( request_arrives( router, COOKIE, STUB, REQUEST ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) == areg::RouterState::Connected );
        areg::Application::release( );
    );
}

/**
 * \brief   The router closes the connection in the middle of a message. The client connects again,
 *          and its next request reaches the router on the new connection with the new cookie.
 **/
TEST( FaultInjectIpcMoreTest, router_client_connects_again_when_the_router_closes_in_the_middle_of_a_message )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 211u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 212u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );

        const areg::MessageEnvelope message{ router_message( areg::REQUEST_ID_FIRST + 9u, areg::EventType::EventRemoteRequest, COOKIE, 64u ) };
        const uint32_t half{ areg::wire_size( *message.header( ) ) / 2u };
        AREG_CHILD_CHECK( router.send_raw( areg::wire_data( *message.header( ) ), half ) );
        router.close_peer( );

        AREG_CHILD_CHECK( router.answer( COOKIE + 1u ) && wait_router_state( areg::RouterState::Connected ) );
        AREG_CHILD_CHECK( request_arrives( router, COOKIE + 1u, STUB, areg::REQUEST_ID_FIRST + 10u ) );
        areg::Application::release( );
    );
}

/**
 * \brief   The router sends a header that is not a valid message: the unknown event type, or a
 *          body larger than a message may be. The client drops that connection and connects again.
 **/
TEST( FaultInjectIpcMoreTest, router_client_connects_again_when_the_router_sends_an_invalid_header )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 221u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 222u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );

        areg::MessageEnvelope badType{ router_message( areg::REQUEST_ID_FIRST, areg::EventType::EventRemoteRequest, COOKIE ) };
        badType.header( )->eventType = static_cast<uint16_t>( areg::EventType::EventUnknown );
        AREG_CHILD_CHECK( router.send( badType ) );
        AREG_CHILD_CHECK( router.drain_until_closed( ) );
        AREG_CHILD_CHECK( router.answer( COOKIE + 1u ) && wait_router_state( areg::RouterState::Connected ) );

        areg::MessageEnvelope bigBody{ router_message( areg::REQUEST_ID_FIRST, areg::EventType::EventRemoteRequest, COOKIE + 1u ) };
        const areg::EventHeader & original{ *bigBody.header( ) };
        areg::EventHeader wire{ original };
        wire.bufHeader.biUsed = areg::MAX_BUF_LENGTH + 1u;
        AREG_CHILD_CHECK( router.send_raw( areg::wire_header( wire ), areg::WIRE_HEADER_SIZE ) );
        AREG_CHILD_CHECK( router.drain_until_closed( ) );
        AREG_CHILD_CHECK( router.answer( COOKIE + 2u ) && wait_router_state( areg::RouterState::Connected ) );
        AREG_CHILD_CHECK( request_arrives( router, COOKIE + 2u, STUB, areg::REQUEST_ID_FIRST + 10u ) );
        areg::Application::release( );
    );
}

/**
 * \brief   The router confirms the connection with a failed result. The client does not take the cookie,
 *          and connects again.
 **/
TEST( FaultInjectIpcMoreTest, router_client_connects_again_when_the_router_confirms_with_a_failure )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 231u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 232u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        areg::Application::setup( false, true, false, true, false );
        configure_router( router.port );
        AREG_CHILD_CHECK( areg::Application::connect_router( ) );

        areg::MessageEnvelope handshake;
        AREG_CHILD_CHECK( router.accept_client( ) && router.receive( handshake ) );
        areg::MessageEnvelope failed{ areg::create_connect_notify( areg::COOKIE_ROUTER, COOKIE ) };
        failed.header( )->result = areg::MESSAGE_SUCCESS + 1u;
        AREG_CHILD_CHECK( router.send( failed ) );
        AREG_CHILD_CHECK( router.drain_until_closed( ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );

        AREG_CHILD_CHECK( router.answer( COOKIE + 1u ) && wait_router_state( areg::RouterState::Connected ) );
        AREG_CHILD_CHECK( request_arrives( router, COOKIE + 1u, STUB, areg::REQUEST_ID_FIRST + 10u ) );
        areg::Application::release( );
    );
}

/**
 * \brief   The application is released while the router has read the handshake and never answers.
 *          The release returns, and the router sees the connection closed.
 **/
TEST( FaultInjectIpcMoreTest, release_ends_a_connection_that_the_router_never_confirmed )
{
    AREG_EXPECT_IN_CHILD(
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        areg::Application::setup( false, true, false, true, false );
        configure_router( router.port );
        AREG_CHILD_CHECK( areg::Application::connect_router( ) );

        areg::MessageEnvelope handshake;
        AREG_CHILD_CHECK( router.accept_client( ) && router.receive( handshake ) );
        AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );
        areg::Application::release( );
        AREG_CHILD_CHECK( router.drain_until_closed( ) );
    );
}

/**
 * \brief   The system refuses to create the socket of the first connection attempt. The client tries
 *          again by itself, and the router sees the client connect with the cookie it confirms.
 **/
TEST( FaultInjectIpcMoreTest, router_client_tries_again_when_the_system_refuses_to_create_its_socket )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 241u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 242u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        areg::Application::setup( false, true, false, true, false );
        configure_router( router.port );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, EMFILE ) );
        AREG_CHILD_CHECK( areg::Application::connect_router( ) );
        AREG_CHILD_CHECK( router.answer( COOKIE ) && wait_router_state( areg::RouterState::Connected ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketCreate ) == 1u );
        AREG_CHILD_CHECK( request_arrives( router, COOKIE, STUB, areg::REQUEST_ID_FIRST + 10u ) );
        areg::Application::release( );
    );
}

namespace
{
    constexpr uint32_t  BULK_COUNT      { 256u };           //!< The requests posted to a router that does not read.
    constexpr uint32_t  BULK_PAYLOAD    { 60u * 1024u };    //!< The size of one request: it fits the inline send.
    constexpr int       SMALL_RCVBUF    { 4096 };           //!< The receive buffer of the router connection.

    //!< Posts the requests to the stub, one after another; the first one has the id.
    bool post_bulk( ITEM_ID stub, uint32_t firstId )
    {
        for ( uint32_t i{ 0u }; i < BULK_COUNT; ++i )
        {
            if ( post_request( stub, firstId + i, BULK_PAYLOAD ) == false )
                return false;
        }

        return true;
    }
}

/**
 * \brief   The router does not read while the client posts more requests than the connection holds. The
 *          requests that the socket does not take go to the send queue. When the router reads, it receives every
 *          request once, whole, in the order of the posts, from the cookie of the client.
 **/
TEST( FaultInjectIpcMoreTest, router_client_delivers_in_order_the_requests_that_the_full_connection_did_not_take )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 251u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 252u };
        constexpr uint32_t FIRST{ areg::REQUEST_ID_FIRST + 100u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( SMALL_RCVBUF ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );
        AREG_CHILD_CHECK( post_bulk( STUB, FIRST ) );

        for ( uint32_t i{ 0u }; i < BULK_COUNT; ++i )
        {
            areg::MessageEnvelope request;
            AREG_CHILD_CHECK( router.receive( request ) );
            AREG_CHILD_CHECK( request.message_id( ) == FIRST + i );
            AREG_CHILD_CHECK( request.source( ) == COOKIE );
            AREG_CHILD_CHECK( request.target( ) == STUB );
            AREG_CHILD_CHECK( request.header( )->bufHeader.biUsed == BULK_PAYLOAD );
        }

        areg::Application::release( );
    );
}

/**
 * \brief   The application is released while the router does not read and the send queue holds requests.
 *          The release returns, and the router sees the requests that were sent whole and in order, then the
 *          connection closed.
 **/
TEST( FaultInjectIpcMoreTest, release_ends_a_send_blocked_on_a_router_that_does_not_read )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 261u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 262u };
        constexpr uint32_t FIRST{ areg::REQUEST_ID_FIRST + 100u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( SMALL_RCVBUF ) );
        AREG_CHILD_CHECK( start_application( router, COOKIE ) );
        AREG_CHILD_CHECK( post_bulk( STUB, FIRST ) );
        areg::Application::release( );

        uint32_t received{ 0u };
        areg::MessageEnvelope request;
        while ( router.receive( request ) )
        {
            AREG_CHILD_CHECK( request.message_id( ) == FIRST + received );
            ++received;
        }

        AREG_CHILD_CHECK( router.eof );
        AREG_CHILD_CHECK( received < BULK_COUNT );
    );
}

#if AREG_LOGGING

namespace
{
    constexpr ITEM_ID   COLLECTOR_COOKIE{ areg::COOKIE_REMOTE_SERVICE + 271u };

    //!< What the log observer reported to the application.
    struct ObserverState
    {
        std::atomic_uint32_t    connected   { 0u };             //!< Calls of the service connected notification with true.
        std::atomic_uint32_t    failed      { 0u };             //!< Calls of the messaging failed notification.
        std::mutex              lock;                           //!< Guards the texts.
        std::vector<std::string> texts;                         //!< The text of every log message reported.
        areg::SyncEvent         evConnected { true, true };     //!< Signaled when the connection is reported as established.
        areg::SyncEvent         evFailed    { true, true };     //!< Signaled by the messaging failed notification.
        areg::SyncEvent         evMessage   { true, true };     //!< Signaled by the log message notification.
    };

    ObserverState   gObserved;

    //!< The text of the one valid log message the collector sends.
    constexpr const char VALID_TEXT[]{ "valid log text" };
    //!< The text the observer reports itself when the connection is confirmed.
    constexpr const char CONNECTED_TEXT[]{ "Log observer connected to log collector service." };

    //!< The number of reported log messages that are neither the valid one nor the connection report.
    size_t foreign_messages( )
    {
        const std::lock_guard<std::mutex> guard( gObserved.lock );
        size_t count{ 0u };
        for ( const std::string & text : gObserved.texts )
            count += ( ( text == VALID_TEXT ) || ( text == CONNECTED_TEXT ) ) ? 0u : 1u;
        return count;
    }

    void observed_connected( bool isConnected, const char * /*address*/, uint16_t /*port*/ )
    {
        if ( isConnected )
        {
            gObserved.connected.fetch_add( 1u );
            gObserved.evConnected.set_signaled( );
        }
    }

    void observed_failed( )
    {
        gObserved.failed.fetch_add( 1u );
        gObserved.evFailed.set_signaled( );
    }

    void observed_message( const LogRecord * record )
    {
        const std::lock_guard<std::mutex> guard( gObserved.lock );
        gObserved.texts.emplace_back( record->msgLogText );
        if ( gObserved.texts.back( ) == VALID_TEXT )
            gObserved.evMessage.set_signaled( );
    }

    bool wait_observed( areg::SyncEvent & event )
    {
        return event.lock( static_cast<uint32_t>( IO_WAIT_S ) * 1000u );
    }

    //!< Connects the observer to the collector and waits until the collector has confirmed the client with the cookie.
    bool connect_observer( PeerRouter & collector, ITEM_ID cookie )
    {
        ObserverEvents events{ };
        events.evtServiceConnected  = &observed_connected;
        events.evtMessagingFailed   = &observed_failed;
        events.evtLogMessage        = &observed_message;
        return log_observer_initialize( &events, nullptr )
            && log_observer_connect_logger( nullptr, "127.0.0.1", collector.port )
            && collector.answer( cookie )
            && wait_observed( gObserved.evConnected );
    }

    //!< The message types that the observer does not take from the collector.
    constexpr areg::FuncIdRange IGNORED_BY_OBSERVER[]
    {
          areg::FuncIdRange::SystemServiceNotifyRegister
        , areg::FuncIdRange::SystemServiceNotifyDuplicate
        , areg::FuncIdRange::ServiceLogQueryScopes
        , areg::FuncIdRange::SystemServiceQueryInstances
        , areg::FuncIdRange::ServiceLastId
        , static_cast<areg::FuncIdRange>( UNKNOWN_ID )
    };

    //!< Builds a message of the log protocol from the log collector to the observer.
    areg::MessageEnvelope collector_message( areg::FuncIdRange id, uint32_t payload = 0u )
    {
        areg::MessageEnvelope message{ router_message( static_cast<uint32_t>( id ), areg::EventType::EventRemoteConnection, static_cast<uint32_t>( COLLECTOR_COOKIE ), payload ) };
        message.header( )->source = static_cast<uint32_t>( areg::COOKIE_LOGGER );
        return message;
    }
}

/**
 * \brief   The log collector sends messages that the observer does not take: types of its own requests, an
 *          unknown type, and log messages with an empty or a cut entry. The observer ignores them, reports no
 *          log message and no failure, stays on the connection, and reports the next valid log message once.
 **/
TEST( FaultInjectIpcMoreTest, log_observer_ignores_unexpected_collector_messages_and_reports_the_next_valid_log )
{
    AREG_EXPECT_IN_CHILD(
        PeerRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );
        AREG_CHILD_CHECK( connect_observer( collector, COLLECTOR_COOKIE ) );

        for ( const areg::FuncIdRange id : IGNORED_BY_OBSERVER )
        {
            AREG_CHILD_CHECK( collector.send( collector_message( id ) ) );
        }

        AREG_CHILD_CHECK( collector.send( collector_message( areg::FuncIdRange::ServiceLogMessage ) ) );
        AREG_CHILD_CHECK( collector.send( collector_message( areg::FuncIdRange::ServiceLogMessage, areg::log_entry_head( ) - 1u ) ) );

        areg::MessageEnvelope valid{ areg::make_log_message( areg::LogMessageType::MessageText, areg::LOG_SCOPE_ID_NONE, 0u, 0u, areg::LogPriority::PrioDebug, VALID_TEXT, static_cast<uint32_t>( std::strlen( VALID_TEXT ) ) ) };
        AREG_CHILD_CHECK( valid.is_valid( ) );
        valid.header( )->source = static_cast<uint32_t>( areg::COOKIE_LOGGER );
        reinterpret_cast<areg::LogEntry *>( valid.buffer( ) )->logModule[0] = '\0';
        AREG_CHILD_CHECK( collector.send( valid ) );

        AREG_CHILD_CHECK( wait_observed( gObserved.evMessage ) );
        AREG_CHILD_CHECK( foreign_messages( ) == 0u );
        AREG_CHILD_CHECK( gObserved.failed.load( ) == 0u );
        AREG_CHILD_CHECK( gObserved.connected.load( ) == 1u );

        log_observer_disconnect_logger( );
        log_observer_release( );
    );
}

/**
 * \brief   The log collector closes the connection in the middle of a message. The observer reports the
 *          failure, connects again, and asks the collector for the instances on the new connection.
 **/
TEST( FaultInjectIpcMoreTest, log_observer_reports_the_failure_and_connects_again_when_the_collector_closes_in_the_middle_of_a_message )
{
    AREG_EXPECT_IN_CHILD(
        PeerRouter collector;
        AREG_CHILD_CHECK( collector.open( ) );
        AREG_CHILD_CHECK( connect_observer( collector, COLLECTOR_COOKIE ) );

        const areg::MessageEnvelope message{ collector_message( areg::FuncIdRange::ServiceLogMessage, 64u ) };
        AREG_CHILD_CHECK( collector.send_raw( areg::wire_data( *message.header( ) ), areg::wire_size( *message.header( ) ) / 2u ) );
        collector.close_peer( );
        AREG_CHILD_CHECK( wait_observed( gObserved.evFailed ) );

        AREG_CHILD_CHECK( collector.answer( COLLECTOR_COOKIE + 1u ) && wait_observed( gObserved.evConnected ) );
        AREG_CHILD_CHECK( gObserved.connected.load( ) == 2u );
        areg::MessageEnvelope query;
        AREG_CHILD_CHECK( collector.receive( query ) );
        AREG_CHILD_CHECK( query.message_id( ) == areg::message_query_instances( ).messageId );
        AREG_CHILD_CHECK( query.source( ) == COLLECTOR_COOKIE + 1u );
        AREG_CHILD_CHECK( foreign_messages( ) == 0u );

        log_observer_disconnect_logger( );
        log_observer_release( );
    );
}

#endif  // AREG_LOGGING

/**
 * \brief   The system refuses to create the thread of the router client. The application is not connected
 *          to the router, and the next request to connect creates the client and connects it.
 **/
TEST( FaultInjectIpcMoreTest, router_client_connects_on_the_next_request_when_the_system_refuses_its_thread )
{
    AREG_EXPECT_IN_CHILD(
        constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 281u };
        constexpr ITEM_ID STUB{ areg::COOKIE_REMOTE_SERVICE + 282u };
        PeerRouter router;
        AREG_CHILD_CHECK( router.open( ) );
        areg::Application::setup( false, true, false, true, false );
        configure_router( router.port );

        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN ) );
            AREG_CHILD_CHECK( areg::Application::connect_router( ) );
            AREG_CHILD_CHECK( wait_thread_refused( ) );
        }

        AREG_CHILD_CHECK( areg::Application::router_state( ) != areg::RouterState::Connected );
        AREG_CHILD_CHECK( areg::Application::connect_router( ) );
        AREG_CHILD_CHECK( router.answer( COOKIE ) && wait_router_state( areg::RouterState::Connected ) );
        AREG_CHILD_CHECK( request_arrives( router, COOKIE, STUB, areg::REQUEST_ID_FIRST + 10u ) );
        areg::Application::release( );
    );
}

#else   // defined(__linux__)

TEST( FaultInjectIpcMoreTest, router_client_peer_input_is_tested_on_linux_only )
{
    GTEST_SKIP( ) << "the test router uses POSIX sockets and the fault points are replaced on Linux only";
}

#endif  // defined(__linux__)
