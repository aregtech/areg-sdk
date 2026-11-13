/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the paths taken when a system call fails. Each test
 *              arms a fault point in a child process and checks how areg reports the failure.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"
#include "fault-inject/FaultSocketPair.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/base/ThreadConsumer.hpp"

#if AREG_LOGGING
    #include "areglogger/client/LogObserverApi.h"
    #include "areglogger/client/private/LoggerClient.hpp"
    #include "logcollector/service/private/LogCollectorDatabase.hpp"

    #include <filesystem>
    #include <string>
    #include <system_error>
#endif  // AREG_LOGGING

#include <errno.h>

#if defined(__linux__)
    #include "aregextend/console/Console.hpp"
    #include "areg/component/DispatcherThread.hpp"
    #include "areg/component/Event.hpp"
    #include "areg/component/EventConsumer.hpp"
    #include "units/ServiceTestServer.hpp"
    #include "units/LogCapture.hpp"
    #include "units/LogCollectorFake.hpp"
    #include "areg/ipc/RemoteServiceDefs.hpp"
    #include "areg/logging/LogDatabaseEngine.hpp"
    #include "areg/logging/LogConfiguration.hpp"
    #include "areg/persist/ConfigManager.hpp"

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #include <algorithm>
    #include <chrono>
    #include <cstring>
    #include <future>
    #include <string>
    #include <vector>
#endif  // defined(__linux__)

namespace
{
#if defined(_WIN32)
    constexpr int THREAD_ERROR  { 8 };      // ERROR_NOT_ENOUGH_MEMORY
    constexpr int SOCKET_ERROR_ { 10024 };  // WSAEMFILE
    constexpr int FILE_ERROR    { 4 };      // ERROR_TOO_MANY_OPEN_FILES
    [[maybe_unused]] constexpr int REFUSED_ERROR { 10061 };  // WSAECONNREFUSED
    [[maybe_unused]] constexpr int RESET_ERROR   { 10054 };  // WSAECONNRESET
    [[maybe_unused]] constexpr int IN_USE_ERROR  { 10048 };  // WSAEADDRINUSE
    [[maybe_unused]] constexpr int WRITE_ERROR   { 112 };    // ERROR_DISK_FULL
    [[maybe_unused]] constexpr int READ_ERROR    { 30 };     // ERROR_READ_FAULT
#else   // defined(_WIN32)
    constexpr int THREAD_ERROR  { EAGAIN };
    constexpr int SOCKET_ERROR_ { EMFILE };
    constexpr int FILE_ERROR    { EMFILE };
    [[maybe_unused]] constexpr int REFUSED_ERROR { ECONNREFUSED };
    [[maybe_unused]] constexpr int RESET_ERROR   { ECONNRESET };
    [[maybe_unused]] constexpr int IN_USE_ERROR  { EADDRINUSE };
    [[maybe_unused]] constexpr int WRITE_ERROR   { ENOSPC };
    [[maybe_unused]] constexpr int READ_ERROR    { EIO };
#endif  // defined(_WIN32)

    //!< A thread routine that returns at once.
    class ReturningConsumer final : public areg::ThreadConsumer
    {
    public:
        void on_run( ) override
        {
        }
    };

    //!< The message of a fault point that did not fail any call.
    constexpr const char * NOT_REPLACED{ "the fault library did not replace the system call" };
}

TEST( FaultInjectTest, thread_start_fails_when_the_system_cannot_create_a_thread )
{
    AREG_EXPECT_IN_CHILD(
        ReturningConsumer consumer;
        // The thread sanitizer creates its helper thread at the first thread creation of a process.
        areg::Thread warmup( consumer, "FaultInjectWarmup" );
        AREG_CHILD_CHECK( warmup.start( areg::WAIT_INFINITE ) );
        static_cast<void>(warmup.shutdown( areg::WAIT_INFINITE ));

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, THREAD_ERROR ) );

        areg::Thread thread( consumer, "FaultInjectThread" );
        AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( thread.is_running( ) == false );

        AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) );
        static_cast<void>(thread.shutdown( areg::WAIT_INFINITE ));
    );
}

/**
 * \brief   Reproduces the defect: a failed start left the application in the failure state for
 *          ever, so the release did not stop it and the next setup could not start.
 **/
TEST( FaultInjectTest, failed_setup_is_released_and_the_next_setup_starts )
{
    AREG_EXPECT_IN_CHILD(
        ReturningConsumer consumer;
        areg::Thread warmup( consumer, "FaultInjectWarmupSetup" );
        AREG_CHILD_CHECK( warmup.start( areg::WAIT_INFINITE ) );
        static_cast<void>(warmup.shutdown( areg::WAIT_INFINITE ));

        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, THREAD_ERROR, 1u ) );
            areg::Application::setup( false, true, false, true, false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Failure );
            areg::Application::release( );
        }

        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Stopped );
        areg::Application::setup( false, true, false, true, false );
        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Ready );
        AREG_CHILD_CHECK( areg::Application::is_servicing_ready( ) );
        areg::Application::release( );
        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Stopped );
    );
}

TEST( FaultInjectTest, socket_create_returns_an_invalid_socket_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketCreate, SOCKET_ERROR_ ) );

        AREG_CHILD_CHECK( areg::is_valid_socket( areg::socket_create( ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketCreate ) == 1u && NOT_REPLACED );

        const SOCKETHANDLE second{ areg::socket_create( ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( second ) );
        areg::socket_close( second );
        areg::socket_release( );
    );
}

TEST( FaultInjectTest, file_open_fails_and_leaves_the_file_closed_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileOpen, FILE_ERROR ) );

        constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_file.txt", mode ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileOpen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( file.is_opened( ) == false );

        AREG_CHILD_CHECK( file.open( "fault_inject_file.txt", mode ) );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_file.txt" ));
    );
}

#if !defined(_WIN32)

TEST( FaultInjectTest, router_service_start_fails_when_the_process_list_and_the_service_manager_are_unavailable )
{
    AREG_EXPECT_IN_CHILD(
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::DirOpen, EMFILE ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ProcessSpawn, ENOENT ) );

        AREG_CHILD_CHECK( areg::Application::start_router_service( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ProcessSpawn ) == 1u && NOT_REPLACED );
#if defined(__linux__)
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::DirOpen ) == 1u && NOT_REPLACED );
#endif  // defined(__linux__)
    );
}

#endif  // !defined(_WIN32)

#if defined(__linux__) || defined(_WIN32)

TEST( FaultInjectTest, client_connect_fails_and_a_new_client_connects_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        fault_test::LoopListener listener;
        AREG_CHILD_CHECK( listener.open( ) );
        AREG_CHILD_CHECK( areg::socket_initialize( ) );

        areg_fault::FaultScope faults;
        areg::SocketClient refused( "127.0.0.1", listener.address.host_port( ) );
        AREG_CHILD_CHECK( refused.create_fd( ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketConnect, REFUSED_ERROR ) );
        AREG_CHILD_CHECK( refused.connect_to( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketConnect ) == 1u && NOT_REPLACED );
        refused.close( );

        areg::SocketClient client( "127.0.0.1", listener.address.host_port( ) );
        AREG_CHILD_CHECK( client.create_fd( ) );
        AREG_CHILD_CHECK( client.connect_to( ) );
        client.close( );
        areg::socket_release( );
    );
}

TEST( FaultInjectTest, send_data_reports_an_error_and_a_later_send_succeeds_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        fault_test::LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        const SOCKETHANDLE client{ pair.client };
        const SOCKETHANDLE server{ pair.server };

        areg_fault::FaultScope faults;
        uint8_t data[4]{ };
        data[0] = 1u;
        data[3] = 4u;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, RESET_ERROR ) );
        AREG_CHILD_CHECK( areg::send_data( client, data, sizeof( data ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::send_data( client, data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );
        uint8_t received[4]{ };
        AREG_CHILD_CHECK( areg::receive_data( server, received, sizeof( received ) ) == static_cast<int32_t>(sizeof( received )) );
        AREG_CHILD_CHECK( received[0] == 1u && received[3] == 4u );

        pair.close( );
        areg::socket_release( );
    );
}

TEST( FaultInjectTest, receive_data_reports_an_error_and_the_next_receive_gets_the_data_when_the_system_refuses )
{
    AREG_EXPECT_IN_CHILD(
        fault_test::LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        const SOCKETHANDLE client{ pair.client };
        const SOCKETHANDLE server{ pair.server };

        uint8_t data[4]{ };
        data[0] = 5u;
        data[3] = 8u;
        AREG_CHILD_CHECK( areg::send_data( client, data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketRecv, RESET_ERROR ) );
        uint8_t received[4]{ };
        AREG_CHILD_CHECK( areg::receive_data( server, received, sizeof( received ) ) < 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketRecv ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( areg::receive_data( server, received, sizeof( received ) ) == static_cast<int32_t>(sizeof( received )) );
        AREG_CHILD_CHECK( received[0] == 5u && received[3] == 8u );

        pair.close( );
        areg::socket_release( );
    );
}

TEST( FaultInjectTest, server_create_fails_and_a_new_server_binds_when_bind_is_refused )
{
    AREG_EXPECT_IN_CHILD(
        uint16_t port{ 0u };
        {
            fault_test::LoopListener probe;
            AREG_CHILD_CHECK( probe.open( ) );
            port = probe.address.host_port( );
        }

        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        areg_fault::FaultScope faults;
        areg::SocketServer refused;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketBind, IN_USE_ERROR ) );
        AREG_CHILD_CHECK( refused.create( "127.0.0.1", port ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketBind ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( refused.is_valid( ) == false );

        areg::SocketServer server;
        AREG_CHILD_CHECK( server.create( "127.0.0.1", port ) );
        server.close( );
        areg::socket_release( );
    );
}

TEST( FaultInjectTest, server_listen_fails_and_a_second_listen_succeeds_when_listen_is_refused )
{
    AREG_EXPECT_IN_CHILD(
        uint16_t port{ 0u };
        {
            fault_test::LoopListener probe;
            AREG_CHILD_CHECK( probe.open( ) );
            port = probe.address.host_port( );
        }

        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        areg_fault::FaultScope faults;
        areg::SocketServer server;
        AREG_CHILD_CHECK( server.create( "127.0.0.1", port ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketListen, IN_USE_ERROR ) );
        AREG_CHILD_CHECK( server.listen( 4 ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketListen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( server.listen( 4 ) );
        server.close( );
        areg::socket_release( );
    );
}

#endif  // defined(__linux__) || defined(_WIN32)

#if defined(__linux__)

namespace
{
    //!< A free loopback port chosen by the system.
    struct Listener
    {
        uint16_t    port    { 0u };

        bool open( )
        {
            fault_test::LoopListener listener;
            if ( listener.open( ) == false )
                return false;

            port = listener.address.host_port( );
            return true;
        }
    };
}

namespace
{
    constexpr auto START_DEADLINE{ std::chrono::seconds( 30 ) };
    constexpr uint32_t CHILD_DEADLINE_SEC{ 120u };

    //!< Runs on the dispatcher of the server, so it runs after every start posted before it.
    class StartFence final : public areg::EventConsumer
    {
    public:
        std::promise<void> reached;

        void start_event_processing( areg::Event & ) override
        {
            reached.set_value( );
        }
    };

    //!< Returns true when the dispatcher of the server finished what was posted to it.
    bool wait_start_finished( )
    {
        StartFence fence;
        auto ready = fence.reached.get_future( );
        areg::Event event( areg::EventType::EventCustomExternal );
        event.set_event_consumer( &fence );
        auto & dispatcher = areg::DispatcherThread::dispatcher_thread( areg::ThreadAddress( "TestCommServer" ) );
        if ( static_cast<areg::EventDispatcher &>(dispatcher).post_event( event ) == false )
            return false;

        return ( ready.wait_for( START_DEADLINE ) == std::future_status::ready );
    }

    bool connect_client( uint16_t port )
    {
        areg::SocketClient client( "127.0.0.1", port );
        const bool result{ client.create( ) };
        client.close( );
        return result;
    }

    //!< A server of the loopback address on a free port, in a process that can run timers.
    struct ServerFixture
    {
        uint16_t                port;
        service_test::TestServer server;

        explicit ServerFixture( uint32_t pairs = 0u )
            : port( 0u )
            , server( service_test::Policy::Accept, pairs )
        {
            // A child that hangs is killed, so the test fails.
            alarm( CHILD_DEADLINE_SEC );
            Listener probe;
            AREG_CHILD_CHECK( probe.open( ) );
            port = probe.port;
            server.apply_connection_data( areg::String( "127.0.0.1" ), port );
        }

        void stop( )
        {
            server.disconnect_service_host( );
            server.wait_to_complete( );
        }

        //!< Returns true when a retry of the server opened the port.
        bool wait_listening( )
        {
            const auto deadline{ std::chrono::steady_clock::now( ) + START_DEADLINE };
            while ( ( server.is_host_connected( ) == false ) && ( std::chrono::steady_clock::now( ) < deadline ) )
            {
                areg::Thread::sleep( areg::WAIT_10_MILLISECONDS );
            }

            return ( wait_start_finished( ) && connect_client( port ) );
        }
    };
}

// Reproduces the defect: the service reported started before it listened, so the first client was refused.
TEST( FaultInjectTest, service_accepts_a_client_as_soon_as_its_start_finished )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture;
        AREG_CHILD_CHECK( areg_fault::delay( areg_fault::FaultPoint::SocketListen, 300u ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( connect_client( fixture.port ) );
        fixture.stop( );
        areg::Application::release( );
    );
}

TEST( FaultInjectTest, service_listens_again_after_each_restart )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture;
        AREG_CHILD_CHECK( areg_fault::delay( areg_fault::FaultPoint::SocketListen, 100u ) );
        for ( uint32_t round = 0u; round < 3u; ++ round )
        {
            AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
            AREG_CHILD_CHECK( wait_start_finished( ) );
            AREG_CHILD_CHECK( connect_client( fixture.port ) );
            fixture.stop( );
            AREG_CHILD_CHECK( connect_client( fixture.port ) == false );
        }

        areg::Application::release( );
    );
}

// Reproduces the defect: a refused listen ended the receive thread, the start could wait for ever and nothing retried.
TEST( FaultInjectTest, service_start_finishes_and_retries_when_listen_is_refused )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketListen, EADDRINUSE ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketListen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( fixture.server.is_host_pending( ) );
        AREG_CHILD_CHECK( fixture.wait_listening( ) );
        fixture.stop( );
        areg::Application::release( );
    );
}

#if AREG_LOGGING
// A new error is reported, a repeated one is not, and the start is reported when a retry succeeds.
TEST( FaultInjectTest, service_reports_each_new_start_error_once )
{
    AREG_EXPECT_IN_CHILD(
        log_capture::LogCapture capture;
        AREG_CHILD_CHECK( capture.start( "areg_aregextend_service_ServiceCommunicationBase.report_start", static_cast<uint32_t>(areg::LogPriority::PrioInfo) ) );
        areg_fault::FaultScope faults;
        {
            ServerFixture fixture;
            const SOCKETHANDLE holder{ areg::server_connect( areg::String( "127.0.0.1" ), fixture.port ) };
            AREG_CHILD_CHECK( areg::is_valid_socket( holder ) && areg::server_listen( holder ) );
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketBind, EACCES ) );
            AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
            const auto deadline{ std::chrono::steady_clock::now( ) + START_DEADLINE };
            while ( ( fixture.server.start_failures( ) < 3u ) && ( std::chrono::steady_clock::now( ) < deadline ) )
            {
                areg::Thread::sleep( areg::WAIT_10_MILLISECONDS );
            }

            AREG_CHILD_CHECK( fixture.server.start_failures( ) >= 3u );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketBind ) == 1u );
            areg::socket_close( holder );
            AREG_CHILD_CHECK( fixture.wait_listening( ) );
            fixture.stop( );
        }

        const std::string text{ capture.stop( ) };
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, "row:Failed to start the server" ) == 2u );
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, ( "OS error [ " + std::to_string( EACCES ) + " ]" ).c_str( ) ) == 1u );
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, ( "OS error [ " + std::to_string( EADDRINUSE ) + " ]" ).c_str( ) ) == 1u );
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, "row:The server" ) == 1u );
    );
}

// A refused listen has no OS error at the start: it is still reported, as error 0.
TEST( FaultInjectTest, service_reports_a_refused_listen )
{
    AREG_EXPECT_IN_CHILD(
        log_capture::LogCapture capture;
        AREG_CHILD_CHECK( capture.start( "areg_aregextend_service_ServiceCommunicationBase.report_start", static_cast<uint32_t>(areg::LogPriority::PrioInfo) ) );
        areg_fault::FaultScope faults;
        {
            ServerFixture fixture;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketListen, EADDRINUSE ) );
            AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
            AREG_CHILD_CHECK( wait_start_finished( ) );
            AREG_CHILD_CHECK( fixture.wait_listening( ) );
            fixture.stop( );
        }

        const std::string text{ capture.stop( ) };
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, "row:Failed to start the server" ) == 1u );
        AREG_CHILD_CHECK( log_capture::LogCapture::count( text, "OS error [ 0 ]" ) == 1u );
    );
}
#endif  // AREG_LOGGING

TEST( FaultInjectTest, service_with_a_pool_retries_when_listen_is_refused )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture( 2u );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketListen, EADDRINUSE ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketListen ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( fixture.server.is_host_pending( ) );
        AREG_CHILD_CHECK( fixture.wait_listening( ) );
        fixture.stop( );
        areg::Application::release( );
    );
}

// Reproduces the defect: a failed start left the send thread running, so every retry failed.
TEST( FaultInjectTest, service_retries_when_its_receive_thread_cannot_start )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture;
        // The dispatcher and the send thread start, the receive thread does not.
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN, 2u ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u );
        AREG_CHILD_CHECK( fixture.server.is_host_pending( ) );
        AREG_CHILD_CHECK( fixture.wait_listening( ) );
        fixture.stop( );
        areg::Application::release( );
    );
}

TEST( FaultInjectTest, service_retries_when_a_pool_receive_thread_cannot_start )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture( 1u );
        // The dispatcher and the send thread of the pair start, its receive thread does not.
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN, 2u ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u );
        AREG_CHILD_CHECK( fixture.server.is_host_pending( ) );
        AREG_CHILD_CHECK( areg::Thread::find_by_address( areg::ThreadAddress( "AregPoolSend_0" ) ) == nullptr );
        AREG_CHILD_CHECK( fixture.wait_listening( ) );
        fixture.stop( );
        areg::Application::release( );
    );
}

TEST( FaultInjectTest, service_stops_cleanly_while_listen_keeps_being_refused )
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, false, false, true, false );
        areg_fault::FaultScope faults;
        ServerFixture fixture;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketListen, EADDRINUSE, 0u, 0u ) );
        AREG_CHILD_CHECK( fixture.server.connect_service_host( ) );
        AREG_CHILD_CHECK( wait_start_finished( ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketListen ) >= 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( fixture.server.is_host_connected( ) == false );
        fixture.stop( );
        AREG_CHILD_CHECK( connect_client( fixture.port ) == false );
        areg::Application::release( );
    );
}

TEST( FaultInjectTest, multiplexer_register_fails_and_leaves_the_socket_unregistered_when_epoll_ctl_is_refused )
{
    AREG_EXPECT_IN_CHILD(
        AREG_CHILD_CHECK( areg::socket_initialize( ) );
        areg::SocketMultiplexer multiplexer;
        const SOCKETHANDLE handle{ areg::socket_create( ) };
        AREG_CHILD_CHECK( areg::is_valid_socket( handle ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollCtl, ENOMEM ) );
        AREG_CHILD_CHECK( multiplexer.register_socket( handle, false ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollCtl ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( multiplexer.is_registered( handle ) == false );

        AREG_CHILD_CHECK( multiplexer.register_socket( handle, false ) );
        AREG_CHILD_CHECK( multiplexer.is_registered( handle ) );
        multiplexer.unregister_socket( handle );
        areg::socket_close( handle );
        areg::socket_release( );
    );
}

#endif  // defined(__linux__)

#if defined(__linux__) || defined(_WIN32)

TEST( FaultInjectTest, file_write_reports_no_bytes_and_a_later_write_succeeds_when_write_fails )
{
    AREG_EXPECT_IN_CHILD(
        constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_write.txt", mode ) );
        uint8_t data[4]{ };
        data[0] = 1u;

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileWrite, WRITE_ERROR ) );
        AREG_CHILD_CHECK( file.write( data, sizeof( data ) ) == 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileWrite ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( file.write( data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_write.txt" ));
    );
}

TEST( FaultInjectTest, file_read_reports_no_bytes_and_a_later_read_returns_the_data_when_read_fails )
{
    AREG_EXPECT_IN_CHILD(
        constexpr uint32_t wmode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        constexpr uint32_t rmode{ static_cast<uint32_t>(areg::File::OpenMode::Read) };
        uint8_t data[4]{ };
        data[0] = 3u;
        data[3] = 6u;
        {
            areg::File out;
            AREG_CHILD_CHECK( out.open( "fault_inject_read.txt", wmode ) );
            AREG_CHILD_CHECK( out.write( data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );
            out.close( );
        }

        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_read.txt", rmode ) );
        uint8_t in[4]{ };
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileRead, READ_ERROR ) );
        AREG_CHILD_CHECK( file.read( in, sizeof( in ) ) == 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileRead ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( file.read( in, sizeof( in ) ) == static_cast<int32_t>(sizeof( in )) );
        AREG_CHILD_CHECK( in[0] == 3u && in[3] == 6u );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_read.txt" ));
    );
}

#endif  // defined(__linux__) || defined(_WIN32)

#if defined(__linux__)

//////////////////////////////////////////////////////////////////////////
// The console of aregextend.
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Makes a new pseudo terminal the controlling terminal, the standard input and the standard output.
    void _take_terminal( )
    {
        AREG_CHILD_CHECK( (::getsid( 0 ) == ::getpid( )) || (::setsid( ) != -1) );
        const int master{ ::posix_openpt( O_RDWR | O_NOCTTY ) };
        AREG_CHILD_CHECK( (master >= 0) && (::grantpt( master ) == 0) && (::unlockpt( master ) == 0) );
        const char * name{ ::ptsname( master ) };
        AREG_CHILD_CHECK( name != nullptr );
        const int slave{ ::open( name, O_RDWR ) };
        AREG_CHILD_CHECK( slave >= 0 );
        ::ioctl( slave, TIOCSCTTY, 0 );
        AREG_CHILD_CHECK( (::dup2( slave, 0 ) == 0) && (::dup2( slave, 1 ) == 1) );
    }

    //!< Makes a pipe holding the text the standard input.
    void _input_from( const char * text )
    {
        int fds[2]{ -1, -1 };
        AREG_CHILD_CHECK( ::pipe( fds ) == 0 );
        const ssize_t size{ static_cast<ssize_t>(::strlen( text )) };
        AREG_CHILD_CHECK( ::write( fds[1], text, static_cast<size_t>(size) ) == size );
        ::close( fds[1] );
        AREG_CHILD_CHECK( ::dup2( fds[0], 0 ) == 0 );
        ::close( fds[0] );
    }

    //!< Returns true if the terminal input echoes and reads lines.
    bool _is_line_mode( )
    {
        termios modes{ };
        return (::tcgetattr( 0, &modes ) == 0) && ((modes.c_lflag & (ICANON | ECHO)) == (ICANON | ECHO));
    }

    //!< Returns true if the action of the signal is the default one.
    bool _is_default( int sig )
    {
        struct sigaction action{ };
        return (::sigaction( sig, nullptr, &action ) == 0) && ((action.sa_flags & SA_SIGINFO) == 0) && (action.sa_handler == SIG_DFL);
    }
}

TEST( FaultInjectTest, console_keeps_the_terminal_modes_when_reading_them_fails )
{
    AREG_EXPECT_IN_CHILD(
        _take_terminal( );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::TermGet, EIO ) );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::TermGet ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( console.is_terminal( ) );
        AREG_CHILD_CHECK( _is_line_mode( ) );
        AREG_CHILD_CHECK( _is_default( SIGINT ) && _is_default( SIGTERM ) );
        console.uninitialize( );
        AREG_CHILD_CHECK( _is_line_mode( ) );
    );
}

TEST( FaultInjectTest, console_leaves_the_signals_when_setting_key_mode_fails )
{
    AREG_EXPECT_IN_CHILD(
        _take_terminal( );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::TermSet, EIO ) );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::TermSet ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( console.is_terminal( ) );
        AREG_CHILD_CHECK( _is_line_mode( ) );
        AREG_CHILD_CHECK( _is_default( SIGINT ) && _is_default( SIGCONT ) );
        console.uninitialize( );
        AREG_CHILD_CHECK( _is_line_mode( ) );
    );
}

TEST( FaultInjectTest, console_reads_lines_without_an_interrupt_pipe )
{
    AREG_EXPECT_IN_CHILD(
        _input_from( "first\nsecond\n" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::PipeCreate, EMFILE ) );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::PipeCreate ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( console.enable_console_input( true ) );
        AREG_CHILD_CHECK( console.wait_for_input( nullptr ) == "first" );
        char buffer[16]{ };
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "second" ) == 0) );
        AREG_CHILD_CHECK( console.enable_console_input( false ) );
        console.uninitialize( );
    );
}

TEST( FaultInjectTest, console_read_retries_an_interrupted_wait_and_ends_on_a_failed_wait )
{
#if defined(__SANITIZE_ADDRESS__)
    GTEST_SKIP( ) << "the AddressSanitizer interceptor of poll() does not reach the replaced poll()";
#endif  // defined(__SANITIZE_ADDRESS__)

    AREG_EXPECT_IN_CHILD(
        _input_from( "ab\ncd\n" );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        areg_fault::FaultScope faults;
        char buffer[16]{ };
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, EINTR ) );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "ab" ) == 0) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Poll, EBADF ) );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Poll ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "cd" ) == 0) );
        console.uninitialize( );
    );
}

TEST( FaultInjectTest, console_read_retries_an_interrupted_read_and_ends_on_a_failed_read )
{
    AREG_EXPECT_IN_CHILD(
        _input_from( "cd\nef\ngh\n" );
        areg::ext::Console & console{ areg::ext::Console::instance( ) };
        areg_fault::FaultScope faults;
        char buffer[16]{ };
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileRead, EINTR ) );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "cd" ) == 0) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileRead ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileRead, EAGAIN ) );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "ef" ) == 0) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileRead, EIO ) );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileRead ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::ext::Console::read_console_data( buffer, sizeof( buffer ) ) && (::strcmp( buffer, "gh" ) == 0) );
        console.uninitialize( );
    );
}

#endif  // defined(__linux__)

// The sanitizer runtimes define the global allocation functions themselves.
#if defined(__linux__) && !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
    #if defined(__has_feature)
        #if !__has_feature(address_sanitizer) && !__has_feature(thread_sanitizer) && !__has_feature(memory_sanitizer)
            #define AREG_FAULT_ALLOC_TESTS  1
        #endif  // !__has_feature(...)
    #else   // defined(__has_feature)
        #define AREG_FAULT_ALLOC_TESTS      1
    #endif  // defined(__has_feature)
#endif  // defined(__linux__) && !sanitizer

#if defined(AREG_FAULT_ALLOC_TESTS)

TEST( FaultInjectTest, shared_buffer_reserve_returns_zero_and_a_later_reserve_succeeds_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::SharedBuffer buffer;
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( buffer.reserve( 4096u, false ) == 0u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( buffer.is_valid( ) == false );
        AREG_CHILD_CHECK( buffer.size_used( ) == 0u );

        AREG_CHILD_CHECK( buffer.reserve( 4096u, false ) >= 4096u );
        AREG_CHILD_CHECK( buffer.is_valid( ) );
    );
}

TEST( FaultInjectTest, write_invert_writes_nothing_and_a_later_write_succeeds_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( "fault_inject_invert.txt", mode ) );

        uint8_t data[4]{ };
        data[0] = 1u;
        data[3] = 4u;
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( file.write_invert( data, sizeof( data ) ) == 0 );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( file.write_invert( data, sizeof( data ) ) == static_cast<int32_t>(sizeof( data )) );
        file.close( );
        static_cast<void>(areg::File::delete_file( "fault_inject_invert.txt" ));
    );
}

namespace
{
    constexpr uint32_t BUFFER_MODE{ static_cast<uint32_t>(areg::FileBase::OpenMode::Write)
                                  | static_cast<uint32_t>(areg::FileBase::OpenMode::Read)
                                  | static_cast<uint32_t>(areg::FileBase::OpenMode::Binary) };
}

//!< A search whose window cannot be allocated finds nothing; the next search finds the data.
TEST( FaultInjectTest, file_search_finds_nothing_when_its_window_cannot_be_allocated )
{
    AREG_EXPECT_IN_CHILD(
        areg::FileBuffer file( BUFFER_MODE );
        AREG_CHILD_CHECK( file.open( ) );
        constexpr char text[]{ "alpha-beta-gamma" };
        constexpr uint32_t length{ static_cast<uint32_t>(sizeof( text ) - 1u) };
        AREG_CHILD_CHECK( file.write( reinterpret_cast<const uint8_t *>(text), length ) == length );
        const uint8_t * beta{ reinterpret_cast<const uint8_t *>("beta") };
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
            AREG_CHILD_CHECK( file.search_data( 0u, beta, 4u ) == areg::Cursor::INVALID_CURSOR_POSITION );
            AREG_CHILD_CHECK( file.search_text( 0u, "gamma", true ) == areg::Cursor::INVALID_CURSOR_POSITION );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 2u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( file.search_data( 0u, beta, 4u ) == 6u );
        AREG_CHILD_CHECK( file.search_text( 0u, "gamma", true ) == 11u );
    );
}

//!< A block read that cannot allocate its buffer reads nothing and keeps the cursor; the next read gets the block.
TEST( FaultInjectTest, file_block_read_keeps_the_cursor_when_its_buffer_cannot_be_allocated )
{
    AREG_EXPECT_IN_CHILD(
        areg::FileBuffer file( BUFFER_MODE );
        AREG_CHILD_CHECK( file.open( ) );
        areg::SharedBuffer block;
        const uint8_t * bytes{ reinterpret_cast<const uint8_t *>("\x01\x02\x03\x04\x05") };
        constexpr uint32_t count{ 5u };
        AREG_CHILD_CHECK( block.write( bytes, count ) == count );
        AREG_CHILD_CHECK( file.write( block ) == count + sizeof( int32_t ) );
        AREG_CHILD_CHECK( file.move_to_begin( ) );

        areg::SharedBuffer target;
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
            AREG_CHILD_CHECK( file.read( target ) == 0u );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( file.position( ) == 0u );
        AREG_CHILD_CHECK( target.is_valid( ) == false );
        AREG_CHILD_CHECK( file.read( target ) == count + sizeof( int32_t ) );
        AREG_CHILD_CHECK( target.size_used( ) == count );
        AREG_CHILD_CHECK( ( target.buffer( )[0] == 1u ) && ( target.buffer( )[4] == 5u ) );
    );
}

//!< A buffer that cannot grow reports the failure and keeps its data and cursor.
TEST( FaultInjectTest, file_buffer_reserve_keeps_the_data_when_growth_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::FileBuffer file( BUFFER_MODE );
        AREG_CHILD_CHECK( file.open( ) );
        const uint8_t * bytes{ reinterpret_cast<const uint8_t *>("\x07\x08\x09") };
        constexpr uint32_t count{ 3u };
        AREG_CHILD_CHECK( file.write( bytes, count ) == count );
        const uint32_t cursor{ file.position( ) };
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
            AREG_CHILD_CHECK( file.reserve( 64u * 1024u ) == areg::INVALID_SIZE );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( file.position( ) == cursor );
        AREG_CHILD_CHECK( file.length( ) == count );
        AREG_CHILD_CHECK( file.data_buffer( )[2] == 9u );
        AREG_CHILD_CHECK( file.reserve( 64u * 1024u ) == cursor );
        AREG_CHILD_CHECK( file.length( ) == count );
    );
}

/**
 * \brief   Reproduces the defect: a failed growth released the block, so the bytes already written were lost.
 **/
TEST( FaultInjectTest, shared_buffer_reserve_keeps_the_written_data_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::SharedBuffer buffer;
        uint8_t data[8]{ };
        data[0] = 9u;
        data[7] = 7u;
        AREG_CHILD_CHECK( buffer.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( buffer.reserve( 1024u * 1024u, true ) == 0u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( buffer.is_valid( ) );
        AREG_CHILD_CHECK( buffer.size_used( ) == sizeof( data ) );
        AREG_CHILD_CHECK( buffer.position( ) == sizeof( data ) );
        AREG_CHILD_CHECK( (buffer.buffer( )[0] == 9u) && (buffer.buffer( )[7] == 7u) );
    );
}

/**
 * \brief   Reproduces the defect: a write that could not grow the block lost the bytes written before it.
 **/
TEST( FaultInjectTest, shared_buffer_write_keeps_the_written_data_when_growth_fails )
{
    AREG_EXPECT_IN_CHILD(
        static uint8_t large[1024u * 1024u]{ };
        areg::SharedBuffer buffer;
        uint8_t data[8]{ };
        data[0] = 9u;
        data[7] = 7u;
        AREG_CHILD_CHECK( buffer.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( buffer.write( large, sizeof( large ) ) == 0u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( buffer.size_used( ) == sizeof( data ) );
        AREG_CHILD_CHECK( buffer.position( ) == sizeof( data ) );

        AREG_CHILD_CHECK( buffer.write( large, sizeof( large ) ) == sizeof( large ) );
        AREG_CHILD_CHECK( buffer.size_used( ) == sizeof( data ) + sizeof( large ) );
        AREG_CHILD_CHECK( (buffer.buffer( )[0] == 9u) && (buffer.buffer( )[7] == 7u) );
    );
}

/**
 * \brief   Reproduces the defect: a failed reserve without a copy dropped the data, although nothing replaced it.
 **/
TEST( FaultInjectTest, shared_buffer_reserve_without_copy_changes_nothing_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::SharedBuffer buffer;
        uint8_t data[8]{ };
        data[0] = 9u;
        AREG_CHILD_CHECK( buffer.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( buffer.reserve( 1024u * 1024u, false ) == 0u );
        AREG_CHILD_CHECK( buffer.size_used( ) == sizeof( data ) );
        AREG_CHILD_CHECK( buffer.buffer( )[0] == 9u );
    );
}

/**
 * \brief   Reproduces the defect: a message that could not grow stayed valid and went on with the next field, so it was sent half written.
 **/
TEST( FaultInjectTest, message_that_cannot_grow_is_invalid_and_rejects_later_writes )
{
    AREG_EXPECT_IN_CHILD(
        static uint8_t large[1024u * 1024u]{ };
        areg::MessageEnvelope msg( true, areg::BLOCK_SIZE );
        uint8_t data[8]{ };
        AREG_CHILD_CHECK( msg.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( msg.write( large, sizeof( large ) ) == 0u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( msg.is_valid( ) == false );

        AREG_CHILD_CHECK( msg.write( data, sizeof( data ) ) == 0u );
        AREG_CHILD_CHECK( msg.is_valid( ) == false );
    );
}

//!< A buffer that cannot grow reports it and keeps the data, whichever way it grows.
TEST( FaultInjectTest, shared_buffer_growth_failures_keep_the_data )
{
    AREG_EXPECT_IN_CHILD(
        static uint8_t large[1024u * 1024u]{ };
        areg::SharedBuffer buffer;
        uint8_t data[8]{ };
        data[0] = 9u;
        AREG_CHILD_CHECK( buffer.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( buffer.ensure_size( sizeof( large ) ) == false );
        AREG_CHILD_CHECK( buffer.insert_at( large, sizeof( large ), 1u ) == 0u );
        AREG_CHILD_CHECK( buffer.insert_at( large, sizeof( large ), sizeof( data ) ) == 0u );
        AREG_CHILD_CHECK( buffer.size_used( ) == sizeof( data ) );
        AREG_CHILD_CHECK( buffer.buffer( )[0] == 9u );
    );
}

//!< A buffer without a block stays without one when it cannot get the first block.
TEST( FaultInjectTest, empty_buffer_stays_empty_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        uint8_t data[8]{ };
        areg::SharedBuffer buffer;
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( buffer.write( data, sizeof( data ) ) == 0u );
        AREG_CHILD_CHECK( buffer.ensure_size( 8u ) == false );
        AREG_CHILD_CHECK( buffer.insert_at( data, sizeof( data ), 0u ) == 0u );
        AREG_CHILD_CHECK( buffer.is_valid( ) == false );
    );
}

//!< The largest buffer is asked for at the limit, and a failure is reported.
TEST( FaultInjectTest, growth_to_the_limit_fails_cleanly )
{
    AREG_EXPECT_IN_CHILD(
        areg::SharedBuffer buffer;
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( buffer.ensure_size( areg::MAX_BUF_LENGTH ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 1u && NOT_REPLACED );
    );
}

//!< A clone that cannot get a block is an empty buffer, the source stays.
TEST( FaultInjectTest, clone_is_empty_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        uint8_t data[8]{ };
        areg::SharedBuffer shared;
        AREG_CHILD_CHECK( shared.write( data, sizeof( data ) ) == sizeof( data ) );
        areg::BufferBase base;
        AREG_CHILD_CHECK( base.write( data, sizeof( data ) ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( shared.clone( ).is_valid( ) == false );
        AREG_CHILD_CHECK( base.clone( ).is_valid( ) == false );
        AREG_CHILD_CHECK( shared.size_used( ) == sizeof( data ) );
    );
}

//!< A copying read that cannot get a block changes neither the source position nor the target.
TEST( FaultInjectTest, buffer_read_keeps_the_position_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        uint8_t data[8]{ };
        areg::SharedBuffer inner;
        AREG_CHILD_CHECK( inner.write( data, sizeof( data ) ) == sizeof( data ) );
        areg::BufferBase source;
        AREG_CHILD_CHECK( source.write( inner ) == sizeof( data ) );
        source.reset( );

        areg::SharedBuffer target;
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( source.read( target ) == 0u );
        AREG_CHILD_CHECK( source.position( ) == 0u );
        AREG_CHILD_CHECK( target.is_valid( ) == false );
    );
}

namespace
{
    //! Makes the protected write view of a buffer reachable for a test.
    class ViewProbe : public areg::SharedBuffer
    {
    public:
        using areg::SharedBuffer::write_view;
    };

    //! Makes the block setup of a plain buffer reachable for a test.
    class PlainProbe : public areg::BufferBase
    {
    public:
        using areg::BufferBase::write_view;
    };
}

//!< A write view that cannot grow gives no address and keeps the data.
TEST( FaultInjectTest, write_view_is_null_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        uint8_t data[8]{ };
        ViewProbe shared;
        AREG_CHILD_CHECK( shared.write( data, sizeof( data ) ) == sizeof( data ) );
        PlainProbe plain;

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( shared.write_view( 1024u * 1024u ) == nullptr );
        AREG_CHILD_CHECK( plain.write_view( 16u ) == nullptr );
        AREG_CHILD_CHECK( shared.size_used( ) == sizeof( data ) );
    );
}

//!< An envelope that cannot get a block for its header is not valid, and a clone of a valid one neither.
TEST( FaultInjectTest, envelope_without_memory_is_invalid )
{
    AREG_EXPECT_IN_CHILD(
        areg::MessageEnvelope source( true, 1024u * 1024u );
        AREG_CHILD_CHECK( source.is_valid( ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( source.clone( ).is_valid( ) == false );

        areg::EventHeader header{ };
        areg::MessageEnvelope built( header, 1024u * 1024u, 1024u * 1024u );
        AREG_CHILD_CHECK( built.is_valid( ) == false );
    );
}

//!< Every log protocol message built without memory is invalid, so no caller sends a half-made message.
TEST( FaultInjectTest, log_protocol_messages_without_memory_are_invalid )
{
    AREG_EXPECT_IN_CHILD(
        const areg::ScopeList scopes;
        const areg::ScopeNames names;
        const areg::LogEntry entry( areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioInfo, "fault", 5u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        AREG_CHILD_CHECK( areg::message_register_scopes( 1001u, 1002u, scopes ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_update_scopes( 1001u, 1002u, names ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_update_scope( 1001u, 1002u, "scope", 1u, 1u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_query_instances( 1001u, 1002u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_query_scopes( 1001u, 1002u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_scopes_updated( 1001u, 1002u, scopes ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_save_configuration( 1001u, 1002u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_configuration_saved( ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_restore_configuration( 1001u, 1002u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_configuration_restored( ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_update_source_state( 1001u, 1002u, areg::LogSourceState::Paused ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::message_source_state_updated( 1001u, 1002u, areg::LogSourceState::Paused, 1003u ).is_valid( ) == false );
        AREG_CHILD_CHECK( areg::create_log_message( entry, areg::LogDataType::Remote, 1001u ).is_valid( ) == false );
#if AREG_LOGGING
        AREG_CHILD_CHECK( areg::make_log_message( areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioInfo, "fault", 5u ).is_valid( ) == false );
#endif  // AREG_LOGGING
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 1u && NOT_REPLACED );
    );
}

//!< A block goes back to the heap when the cache cannot get a magazine for it.
TEST( FaultInjectTest, envelope_block_is_freed_when_the_cache_has_no_magazine )
{
    AREG_EXPECT_IN_CHILD(
        areg::MessageEnvelope env( true, 256u );
        AREG_CHILD_CHECK( env.is_valid( ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        env.invalidate( );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) >= 1u && NOT_REPLACED );
    );
}

/**
 * \brief   Reproduces the defect: a failed reserve on a view released the block the view reads.
 **/
TEST( FaultInjectTest, shared_buffer_view_stays_readable_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::SharedBuffer inner;
        uint8_t data[4]{ };
        data[0] = 5u;
        AREG_CHILD_CHECK( inner.write( data, sizeof( data ) ) == sizeof( data ) );

        areg::SharedBuffer outer;
        AREG_CHILD_CHECK( outer.write( inner ) == sizeof( data ) );
        outer.move_to_begin( );
        areg::SharedBuffer view;
        AREG_CHILD_CHECK( outer.read( view ) == sizeof( data ) );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( view.reserve( 1024u * 1024u, true ) == 0u );
        AREG_CHILD_CHECK( view.size_used( ) == sizeof( data ) );
        AREG_CHILD_CHECK( view.buffer( )[0] == 5u );
    );
}

//!< A ring whose storage cannot be allocated has no capacity and rejects pushes.
TEST( FaultInjectTest, ring_stack_has_no_capacity_when_the_storage_cannot_be_allocated )
{
    AREG_EXPECT_IN_CHILD(
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        areg::RingStack<int32_t> stack( 4u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( stack.capacity( ) == 0u );
        AREG_CHILD_CHECK( stack.push( 1 ) == 0u );
        AREG_CHILD_CHECK( stack.is_empty( ) );

        AREG_CHILD_CHECK( stack.reserve( 4u ) == 4u );
        AREG_CHILD_CHECK( stack.push( 1 ) == 1u );
        AREG_CHILD_CHECK( stack.pop( ) == 1 );
    );
}

//!< A ring that cannot grow on a push keeps its values, and a later push grows it.
TEST( FaultInjectTest, ring_stack_keeps_values_when_growth_on_push_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::RingStack<int32_t> stack( 2u, areg::OverlapPolicy::Resize );
        AREG_CHILD_CHECK( stack.push( 1 ) == 1u );
        AREG_CHILD_CHECK( stack.push( 2 ) == 2u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( stack.push( 3 ) == 2u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( stack.capacity( ) == 2u );
        AREG_CHILD_CHECK( stack.size( ) == 2u );
        AREG_CHILD_CHECK( stack[0] == 1 && stack[1] == 2 );

        AREG_CHILD_CHECK( stack.push( 3 ) == 3u );
        AREG_CHILD_CHECK( stack.pop( ) == 1 && stack.pop( ) == 2 && stack.pop( ) == 3 );
    );
}

//!< A reserve that cannot allocate returns the old capacity and keeps the values.
TEST( FaultInjectTest, ring_stack_reserve_keeps_values_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::RingStack<int32_t> stack( 2u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( stack.push( 1 ) == 1u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        AREG_CHILD_CHECK( stack.reserve( 16u ) == 2u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( stack.size( ) == 1u && stack[0] == 1 );

        AREG_CHILD_CHECK( stack.reserve( 16u ) == 16u );
        AREG_CHILD_CHECK( stack.pop( ) == 1 );
    );
}

//!< A free_extra that cannot allocate keeps the capacity and the values.
TEST( FaultInjectTest, ring_stack_free_extra_keeps_values_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::RingStack<int32_t> stack( 8u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( stack.push( 1 ) == 1u );
        AREG_CHILD_CHECK( stack.push( 2 ) == 2u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        stack.free_extra( );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( stack.capacity( ) == 8u );
        AREG_CHILD_CHECK( stack.size( ) == 2u && stack[0] == 1 && stack[1] == 2 );

        stack.free_extra( );
        AREG_CHILD_CHECK( stack.capacity( ) == 2u );
        AREG_CHILD_CHECK( stack.pop( ) == 1 && stack.pop( ) == 2 );
    );
}

//!< Reproduces the defect: a copy emptied the target before it allocated, so a failed allocation lost the values.
TEST( FaultInjectTest, ring_stack_copy_keeps_target_values_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::RingStack<int32_t> source( 8u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( source.push( 7 ) == 1u );
        areg::RingStack<int32_t> target( 2u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( target.push( 1 ) == 1u );
        AREG_CHILD_CHECK( target.push( 2 ) == 2u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        target.copy( source );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( target.size( ) == 2u );
        AREG_CHILD_CHECK( target[0] == 1 && target[1] == 2 );
    );
}

//!< A copy constructor that cannot allocate gives a valid empty ring.
TEST( FaultInjectTest, ring_stack_copy_constructor_gives_an_empty_ring_when_allocation_fails )
{
    AREG_EXPECT_IN_CHILD(
        areg::RingStack<int32_t> source( 4u, areg::OverlapPolicy::Stop );
        AREG_CHILD_CHECK( source.push( 7 ) == 1u );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0 ) );
        areg::RingStack<int32_t> copy( source );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( copy.is_empty( ) );
        AREG_CHILD_CHECK( source.size( ) == 1u && source[0] == 7 );
        AREG_CHILD_CHECK( copy.reserve( 4u ) == 4u );
        AREG_CHILD_CHECK( copy.push( 3 ) == 1u );
    );
}

#endif  // defined(AREG_FAULT_ALLOC_TESTS)

#if AREG_LOGGING

namespace
{
    uint32_t    gDbCreated  { 0u };     //!< Calls of the database created notification.

    void on_db_created( const char * /*dbLocation*/ )
    {
        ++ gDbCreated;
    }

    //!< Starts and stops a thread, so that a sanitizer creates its helper thread before a fault is armed.
    void warm_up_threads( )
    {
        ReturningConsumer consumer;
        areg::Thread warmup( consumer, "FaultInjectWarmupLogging" );
        AREG_CHILD_CHECK( warmup.start( areg::WAIT_INFINITE ) );
        static_cast<void>(warmup.shutdown( areg::WAIT_INFINITE ));
    }

    //!< Returns a database file in a private directory of the process.
    std::filesystem::path private_database( const char * name )
    {
        std::filesystem::path dir{ std::filesystem::temp_directory_path( ) / ("areg-fault-logging-" + std::to_string( areg::Process::instance( ).id( ) )) };
        std::error_code error;
        std::filesystem::create_directories( dir, error );
        return (dir / name);
    }

    //!< Removes the private directory of the process.
    void remove_private_directory( const std::filesystem::path & file )
    {
        std::error_code error;
        std::filesystem::remove_all( file.parent_path( ), error );
    }
}

/**
 * \brief   The writer thread of the log collector database cannot be created (the thread of the
 *          checkpointer, created before it, passes). The start is refused, the
 *          database stays inactive and has no path, and the next start works.
 **/
TEST( FaultInjectTest, log_collector_database_stays_stopped_when_its_writer_thread_cannot_start )
{
    AREG_EXPECT_IN_CHILD(
        warm_up_threads( );
        const std::filesystem::path file{ private_database( "writer.sqlog" ) };
        {
            LogCollectorDatabase database;
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, THREAD_ERROR, 1u ) );
            AREG_CHILD_CHECK( database.start( areg::String( file.string( ).c_str( ) ) ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( database.is_active( ) == false );
            AREG_CHILD_CHECK( database.database_path( ).is_empty( ) );
            database.save_instance_disconnected( 1u );
            database.stop( );

            AREG_CHILD_CHECK( database.start( areg::String( file.string( ).c_str( ) ) ) );
            AREG_CHILD_CHECK( database.is_active( ) );
            AREG_CHILD_CHECK( database.database_path( ).is_empty( ) == false );
            database.stop( );
            AREG_CHILD_CHECK( database.is_active( ) == false );
        }

        remove_private_directory( file );
    );
}

/**
 * \brief   The writer thread of the database of the log observer cannot be created. The database is
 *          refused and closed, no creation is reported, and opening it again works.
 **/
TEST( FaultInjectTest, log_observer_database_is_closed_when_its_writer_thread_cannot_start )
{
    AREG_EXPECT_IN_CHILD(
        warm_up_threads( );
        areg::Application::setup( false, true, false, true, false );
        const std::filesystem::path file{ private_database( "observer.sqlog" ) };
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance( ) };
        ObserverEvents events{ };
        events.evtLogDbCreated = &on_db_created;
        client.set_callbacks( &events );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, THREAD_ERROR, 1u ) );
            AREG_CHILD_CHECK( client.open_logging_database( file.string( ).c_str( ) ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( gDbCreated == 0u );
        }

        AREG_CHILD_CHECK( client.open_logging_database( file.string( ).c_str( ) ) );
        AREG_CHILD_CHECK( gDbCreated == 1u );
        client.close_logging_database( );
        areg::Application::release( );
        remove_private_directory( file );
    );
}

#endif  // AREG_LOGGING

#if defined(__linux__) && AREG_LOGGING

namespace
{
    void write_remote_row( const char * text )
    {
        const areg::LogEntry entry( areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioInfo
                                  , text, static_cast<uint32_t>(std::strlen( text )) );
        areg::log_local( entry );
    }

    //!< A log database that keeps nothing and tells a waiting thread which row the logging thread handled.
    class FenceDatabase final : public areg::LogDatabaseEngine
    {
    public:
        bool is_operable( ) const noexcept override { return true; }
        bool connect( const areg::String &, areg::DatabaseRole ) override { return true; }
        void disconnect( ) override { }
        bool execute( const areg::String & ) override { return true; }
        bool begin( ) override { return true; }
        bool commit( bool ) override { return true; }
        bool rollback( ) override { return true; }
        bool tables_initialized( ) const noexcept override { return true; }
        bool log_instance_connected( const areg::ConnectedInstance &, const areg::DateTime & ) override { return true; }
        bool log_instance_disconnected( const ITEM_ID &, const areg::DateTime & ) override { return true; }
        bool log_scope_activate( const areg::ScopeEntry &, const ITEM_ID &, const areg::DateTime & ) override { return true; }
        bool log_scope_activate( const areg::String &, uint32_t, uint32_t, const ITEM_ID &, const areg::DateTime & ) override { return true; }
        uint32_t log_scopes_activate( const areg::ScopeNames &, const ITEM_ID &, const areg::DateTime & ) override { return 0u; }
        bool log_scopes_deactivate( const ITEM_ID &, const areg::DateTime & ) override { return true; }
        bool log_scope_deactivate( const ITEM_ID &, uint32_t, const areg::DateTime & ) override { return true; }

        bool log_message( const areg::LogEntry & entry ) override
        {
            if ( std::string( entry.logMessage, areg::log_message_size( entry ) ) == "fence" )
                mFence.set_value( );

            return true;
        }

        //!< Returns true once the logging thread handled the row "fence".
        bool wait_fence( )
        {
            return mFence.get_future( ).wait_for( std::chrono::seconds( CHILD_DEADLINE_SEC ) ) == std::future_status::ready;
        }

    private:
        std::promise<void>  mFence;
    };

    //!< The rows written while the collector is not reachable; the last one is the fence.
    std::vector<std::string> kept_rows( )
    {
        return { "kept-1", "kept-2", "kept-3", "kept-4", "kept-5", "kept-6", "fence" };
    }
}

/**
 * \brief   Reproduces the defect: a send that failed on a connected source was not reported, so the
 *          source never connected again and the row was lost. The row is kept in the remote queue and
 *          reaches the collector after the reconnect, before the later rows.
 **/
TEST( FaultInjectTest, remote_log_row_whose_send_fails_is_delivered_after_the_reconnect )
{
    AREG_EXPECT_IN_CHILD(
        alarm( CHILD_DEADLINE_SEC );
        log_fake::RuntimeCollector collector;
        AREG_CHILD_CHECK( collector.open( ) );
        areg::ConfigManager & manager{ areg::Application::config_manager( ) };
        manager.set_configuration( areg::ListProperties( ), areg::ListProperties( ) );
        manager.set_service_enable( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true );
        areg::LogConfiguration config;
        for ( const auto target : { areg::LogTarget::File, areg::LogTarget::Debug, areg::LogTarget::Database } )
            config.set_log_enabled( target, false );
        config.set_status( true );
        config.set_log_enabled( areg::LogTarget::Remote, true );
        config.set_tcp_enable( true, true );
        config.set_tcp_address( "127.0.0.1", true );
        config.set_tcp_port( collector.port( ), true );
        config.set_stack_size( 8u );
        areg::Application::setup( false, false, false, true, false );
        AREG_CHILD_CHECK( areg::start_logging( ) );
        AREG_CHILD_CHECK( collector.handshake( ) );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EPIPE ) );
            write_remote_row( "send-fails" );
            AREG_CHILD_CHECK( collector.handshake_next_source( ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        }

        AREG_CHILD_CHECK( collector.next_row( "send-fails" ) );
        write_remote_row( "after-reconnect" );
        AREG_CHILD_CHECK( collector.next_row( "after-reconnect" ) );
        areg::stop_logging( true );
        areg::Application::release( );
    );
}

/**
 * \brief   Reproduces the defect: rows kept for a collector that was not reachable are sent by the send
 *          thread after the connection, and when that send failed the send thread wrote the remote queue
 *          while the logging thread read it, and gave back every row but the first without its text.
 *          The first row always reaches the collector after the reconnect, every row that arrives has
 *          its text, and the rows arrive once and in the order they were written.
 **/
TEST( FaultInjectTest, remote_log_rows_whose_replay_fails_are_delivered_whole_after_the_reconnect )
{
    AREG_EXPECT_IN_CHILD(
        alarm( CHILD_DEADLINE_SEC );
        log_fake::RuntimeCollector collector;
        AREG_CHILD_CHECK( collector.open( false ) );
        areg::ConfigManager & manager{ areg::Application::config_manager( ) };
        manager.set_configuration( areg::ListProperties( ), areg::ListProperties( ) );
        manager.set_service_enable( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true );
        areg::LogConfiguration config;
        for ( const auto target : { areg::LogTarget::File, areg::LogTarget::Debug } )
            config.set_log_enabled( target, false );
        FenceDatabase database;
        areg::set_db_engine( &database );
        config.set_log_enabled( areg::LogTarget::Database, true );
        config.set_status( true );
        config.set_log_enabled( areg::LogTarget::Remote, true );
        config.set_tcp_enable( true, true );
        config.set_tcp_address( "127.0.0.1", true );
        config.set_tcp_port( collector.port( ), true );
        config.set_stack_size( 16u );
        areg::Application::setup( false, false, false, true, false );
        AREG_CHILD_CHECK( areg::start_logging( ) );

        const std::vector<std::string> written( kept_rows( ) );
        for ( const std::string & row : written )
            write_remote_row( row.c_str( ) );

        AREG_CHILD_CHECK( database.wait_fence( ) );
        {
            // The connect request, the reply of the collector and the scopes of the source pass.
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EPIPE, 3u, 0u ) );
            AREG_CHILD_CHECK( collector.listen( ) );
            AREG_CHILD_CHECK( collector.handshake_next_source( ) );
            AREG_CHILD_CHECK( collector.wait_peer_closed( ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) != 0u );
        }

        AREG_CHILD_CHECK( collector.logs( ).empty( ) );
        AREG_CHILD_CHECK( collector.handshake_next_source( ) );
        write_remote_row( "after-reconnect" );
        AREG_CHILD_CHECK( collector.read_until_row( "after-reconnect" ) );
        AREG_CHILD_CHECK( collector.malformed_rows( ) == 0u );

        const std::vector<std::string> & rows{ collector.logs( ) };
        AREG_CHILD_CHECK( (rows.size( ) >= 2u) && (rows.front( ) == "kept-1") && (rows.back( ) == "after-reconnect") );
        auto expected{ written.begin( ) };
        for ( size_t i{ 0u }; (i + 1u) < rows.size( ); ++i )
        {
            expected = std::find( expected, written.end( ), rows[i] );
            AREG_CHILD_CHECK( expected != written.end( ) );
            ++expected;
        }

        areg::stop_logging( true );
        areg::Application::release( );
        areg::set_db_engine( nullptr );
    );
}

/**
 * \brief   Reproduces the defect: a message of the source that is not a log row, here its list of scopes,
 *          was kept in the remote queue when its send failed, and was sent again after the reconnect as
 *          a log row with a part of its data overwritten. The next connection sends the scopes once,
 *          and the first message after them is the first row.
 **/
TEST( FaultInjectTest, remote_log_message_that_is_no_row_is_not_replayed_after_the_reconnect )
{
    AREG_EXPECT_IN_CHILD(
        alarm( CHILD_DEADLINE_SEC );
        log_fake::RuntimeCollector collector;
        AREG_CHILD_CHECK( collector.open( ) );
        areg::ConfigManager & manager{ areg::Application::config_manager( ) };
        manager.set_configuration( areg::ListProperties( ), areg::ListProperties( ) );
        manager.set_service_enable( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true );
        areg::LogConfiguration config;
        for ( const auto target : { areg::LogTarget::File, areg::LogTarget::Debug, areg::LogTarget::Database } )
            config.set_log_enabled( target, false );
        config.set_status( true );
        config.set_log_enabled( areg::LogTarget::Remote, true );
        config.set_tcp_enable( true, true );
        config.set_tcp_address( "127.0.0.1", true );
        config.set_tcp_port( collector.port( ), true );
        config.set_stack_size( 8u );
        {
            // The connect request and the reply of the collector pass, the scopes of the source fail.
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EPIPE, 2u, 1u ) );
            areg::Application::setup( false, false, false, true, false );
            AREG_CHILD_CHECK( areg::start_logging( ) );
            AREG_CHILD_CHECK( collector.handshake( ) == false );
            AREG_CHILD_CHECK( collector.wait_peer_closed( ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u );
        }

        AREG_CHILD_CHECK( collector.handshake_next_source( ) );
        write_remote_row( "first-row" );
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK( collector.read( message ) );
        AREG_CHILD_CHECK( message.message_id( ) == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage) );
        AREG_CHILD_CHECK( (collector.logs( ).size( ) == 1u) && (collector.logs( ).front( ) == "first-row") );
        areg::stop_logging( true );
        areg::Application::release( );
    );
}

#endif  // defined(__linux__) && AREG_LOGGING
