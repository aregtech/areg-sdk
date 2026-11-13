/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_posix_sync_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the Linux synchronization objects, threads, timers, the
 *              send backlog and the application setup when a system call fails. Each test runs
 *              in a child process.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if defined(__linux__)

#include "units/EventTestSupport.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/FatalError.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/TimerManagerEvent.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <future>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#if defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)
    #define AREG_SKIP_WITHOUT_ALLOCATION_FAULT( )   GTEST_SKIP( ) << "the sanitizer runtime defines the allocation operators"
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
        #define AREG_SKIP_WITHOUT_ALLOCATION_FAULT( ) GTEST_SKIP( ) << "the sanitizer runtime defines the allocation operators"
    #endif  // __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
#endif  // defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)

#ifndef AREG_SKIP_WITHOUT_ALLOCATION_FAULT
    #define AREG_SKIP_WITHOUT_ALLOCATION_FAULT( )
#endif  // AREG_SKIP_WITHOUT_ALLOCATION_FAULT

namespace
{
    //!< The message of a fault point that did not fail any call.
    constexpr const char * NOT_REPLACED{ "the fault library did not replace the system call" };

    //!< The limit of a wait for an outcome that must come.
    constexpr uint32_t  WAIT_MS     { 30'000u };

    //!< A timeout that no timer reaches during a test.
    constexpr uint32_t  NEVER_MS    { 3'600'000u };

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

    //!< A thread routine that returns at once.
    class ReturningConsumer final : public areg::ThreadConsumer
    {
    public:
        void on_run( ) override
        {
        }
    };

    //!< Starts and stops one thread, so a sanitizer runtime creates its helper thread before a fault is armed.
    void start_first_thread( )
    {
        ReturningConsumer consumer;
        areg::Thread warmup( consumer, "FaultInjectWarmup" );
        AREG_CHILD_CHECK( warmup.start( areg::WAIT_INFINITE ) );
        static_cast<void>( warmup.shutdown( areg::WAIT_INFINITE ) );
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

    //!< Returns true if the call returns before the wait limit. The value goes to 'out_result'.
    bool returns_in_time( bool (* call)( ), bool & out_result )
    {
        std::packaged_task<bool( )> task( call );
        std::future<bool> done{ task.get_future( ) };
        std::thread caller( std::move( task ) );
        if ( done.wait_for( std::chrono::milliseconds( WAIT_MS ) ) != std::future_status::ready )
        {
            caller.detach( );
            return false;
        }

        caller.join( );
        out_result = done.get( );
        return true;
    }

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
            socklen_t length{ sizeof( addr ) };
            const bool listening{ (::bind( listener, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0)
                               && (::listen( listener, 1 ) == 0)
                               && (::getsockname( listener, reinterpret_cast<sockaddr *>(&addr), &length ) == 0) };
            const int peer{ listening ? ::socket( AF_INET, SOCK_STREAM, 0 ) : -1 };
            const bool connected{ (peer >= 0) && (::connect( peer, reinterpret_cast<sockaddr *>(&addr), sizeof( addr ) ) == 0) };
            const int accepted{ connected ? ::accept( listener, nullptr, nullptr ) : -1 };
            ::close( listener );
            if ( accepted < 0 )
            {
                if ( peer >= 0 )
                {
                    ::close( peer );
                }

                return false;
            }

            client = static_cast<SOCKETHANDLE>(peer);
            server = static_cast<SOCKETHANDLE>(accepted);
            return true;
        }

        ~LoopPair( )
        {
            if ( client != areg::InvalidSocketHandle )
            {
                ::close( static_cast<int>(client) );
            }

            if ( server != areg::InvalidSocketHandle )
            {
                ::close( static_cast<int>(server) );
            }
        }
    };

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
        return (areg::receive_data_window( peer, received.data( ), size ) == static_cast<int32_t>(size))
            && (std::memcmp( received.data( ), areg::wire_data( *message.header( ) ), size ) == 0);
    }

    //////////////////////////////////////////////////////////////////////////
    // The wait state of a thread that waits for all of several objects
    //////////////////////////////////////////////////////////////////////////

    void wait_all_is_fatal_while_the_wait_state_cannot_be_created( areg_fault::FaultPoint point )
    {
        areg::SyncEvent first( false, false );
        areg::SyncEvent second( false, false );
        areg::SyncObject * objects[]{ &first, &second };
        areg::MultiLock multi( objects, 2, false );

        expect_fatal( "a thread cannot wait" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( point, EAGAIN ) );
        static_cast<void>( multi.lock( areg::DO_NOT_WAIT, true ) );
        AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( false && "the wait for all objects returned" );
    }

    //////////////////////////////////////////////////////////////////////////
    // The lock of a synchronization object
    //////////////////////////////////////////////////////////////////////////

    void mutex_is_fatal_while_its_lock_cannot_be_created( areg_fault::FaultPoint point )
    {
        expect_fatal( "a mutex cannot be created" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( point, ENOMEM ) );
        areg::Mutex mutex( false );
        AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( false && "the mutex was created" );
    }

    //////////////////////////////////////////////////////////////////////////
    // The service of the waitable timers
    //////////////////////////////////////////////////////////////////////////

    void timers_are_not_set_while_the_timer_service_cannot_be_created( areg_fault::FaultPoint point, uint32_t skip )
    {
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( point, ENOMEM, skip ) );
        areg::SyncTimer timer( 1u, false, true );
        AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );

        AREG_CHILD_CHECK( timer.set_timer( ) == false );
        AREG_CHILD_CHECK( timer.lock( areg::DO_NOT_WAIT ) == false );

        areg::SyncTimer other( 1u, true, true );
        AREG_CHILD_CHECK( other.set_timer( ) == false );
        AREG_CHILD_CHECK( other.lock( areg::DO_NOT_WAIT ) == false );
    }

    void timer_is_not_set_while_the_timer_thread_attributes_cannot_be_created_and_works_after( )
    {
        start_first_thread( );
        areg::SyncTimer timer( 1u, false, true );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadAttrInit, ENOMEM ) );
        AREG_CHILD_CHECK( timer.set_timer( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadAttrInit ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( timer.lock( areg::DO_NOT_WAIT ) == false );

        AREG_CHILD_CHECK( timer.set_timer( ) );
        AREG_CHILD_CHECK( timer.lock( WAIT_MS ) );
    }

    void timer_set_again_while_armed_fires( )
    {
        areg::SyncTimer timer( NEVER_MS, false, true );
        AREG_CHILD_CHECK( timer.set_timer( ) );

        areg::SyncTimer shorter( 1u, false, true );
        AREG_CHILD_CHECK( shorter.set_timer( ) );
        AREG_CHILD_CHECK( shorter.set_timer( ) );
        AREG_CHILD_CHECK( shorter.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( shorter.lock( areg::DO_NOT_WAIT ) == false );
        AREG_CHILD_CHECK( timer.lock( areg::DO_NOT_WAIT ) == false );
    }

    //////////////////////////////////////////////////////////////////////////
    // Threads
    //////////////////////////////////////////////////////////////////////////

    void thread_does_not_start_while_its_attributes_cannot_be_created_and_starts_after( )
    {
        start_first_thread( );
        ReturningConsumer consumer;
        areg::Thread thread( consumer, "FaultInjectAttr" );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadAttrInit, ENOMEM ) );
            AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadAttrInit ) == 1u && NOT_REPLACED );
            AREG_CHILD_CHECK( thread.is_valid( ) == false );
        }

        AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) );
        static_cast<void>( thread.shutdown( areg::WAIT_INFINITE ) );
    }

    //////////////////////////////////////////////////////////////////////////
    // The timer manager
    //////////////////////////////////////////////////////////////////////////

    bool _start_timer_manager( )
    {
        return areg::TimerManager::start_timer_manager( );
    }

    void timer_manager_does_not_start_while_its_descriptors_cannot_be_created_and_starts_after( areg_fault::FaultPoint point, uint32_t skip )
    {
        bool started{ true };
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( point, EMFILE, skip ) );
            AREG_CHILD_CHECK( returns_in_time( &_start_timer_manager, started ) );
            AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( started == false );
        AREG_CHILD_CHECK( areg::TimerManager::is_manager_started( ) == false );

        AREG_CHILD_CHECK( returns_in_time( &_start_timer_manager, started ) );
        AREG_CHILD_CHECK( started );
        areg::TimerManager::stop_timer_manager( true );
    }

    //!< Starts the timer manager and a dispatcher thread for the timers of a test.
    struct TimerRun
    {
        EventTestDispatcher dispatcher  { "FaultInjectTimerThread" };
        TimerListener       consumer    { };

        bool start( )
        {
            return areg::TimerManager::start_timer_manager( ) && dispatcher.start_ready( );
        }

        ~TimerRun( )
        {
            dispatcher.shutdown( areg::WAIT_INFINITE );
            areg::TimerManager::stop_timer_manager( true );
        }
    };

    void timer_whose_system_timer_fails_stays_inactive_and_starts_after( areg_fault::FaultPoint point )
    {
        TimerRun run;
        AREG_CHILD_CHECK( run.start( ) );
        areg::Timer refused( run.consumer, "FaultInjectRefused" );
        areg::Timer marker( run.consumer, "FaultInjectMarker" );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( point, EMFILE ) );
            AREG_CHILD_CHECK( refused.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( marker.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( run.consumer.wait_count( marker, 1u ) );
            AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( refused.is_active( ) == false );
        AREG_CHILD_CHECK( run.consumer.count( refused ) == 0u );

        AREG_CHILD_CHECK( refused.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
        AREG_CHILD_CHECK( run.consumer.wait_count( refused, 1u ) );
        refused.stop_timer( );
        marker.stop_timer( );
    }

    void interrupted_wait_of_the_timer_manager_is_repeated_and_the_timer_fires( )
    {
        TimerRun run;
        AREG_CHILD_CHECK( run.start( ) );
        areg::Timer timer( run.consumer, "FaultInjectInterrupted" );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, EINTR ) );
        AREG_CHILD_CHECK( timer.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
        AREG_CHILD_CHECK( run.consumer.wait_count( timer, 1u ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollWait ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg::TimerManager::is_manager_started( ) );
        timer.stop_timer( );
    }

    void failed_wait_of_the_timer_manager_stops_it_and_it_starts_again( )
    {
        TimerRun run;
        AREG_CHILD_CHECK( run.start( ) );
        areg::DispatcherThread * const timerThread{ areg::DispatcherThread::find_consumer_thread( areg::TimerManagerEvent::CLASS_ID ) };
        AREG_CHILD_CHECK( timerThread != nullptr );
        areg::Timer timer( run.consumer, "FaultInjectFailedWait" );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, EBADF ) );
            static_cast<void>( timer.start_timer( NEVER_MS, run.dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( timerThread->areg::Thread::wait_completion( WAIT_MS ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollWait ) == 1u && NOT_REPLACED );
        }

        AREG_CHILD_CHECK( areg::TimerManager::is_manager_started( ) == false );
        timer.stop_timer( );
        areg::TimerManager::wait_timer_manager( );

        AREG_CHILD_CHECK( areg::TimerManager::start_timer_manager( ) );
        areg::Timer after( run.consumer, "FaultInjectAfterWait" );
        AREG_CHILD_CHECK( after.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
        AREG_CHILD_CHECK( run.consumer.wait_count( after, 1u ) );
        after.stop_timer( );
    }

    void timer_started_before_a_failed_manager_wait_fires_or_is_not_active( )
    {
        TimerRun run;
        AREG_CHILD_CHECK( run.start( ) );
        areg::DispatcherThread * const timerThread{ areg::DispatcherThread::find_consumer_thread( areg::TimerManagerEvent::CLASS_ID ) };
        AREG_CHILD_CHECK( timerThread != nullptr );
        areg::Timer lost( run.consumer, "FaultInjectLost" );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::EpollWait, EBADF ) );
            AREG_CHILD_CHECK( lost.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
            AREG_CHILD_CHECK( timerThread->areg::Thread::wait_completion( WAIT_MS ) );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::EpollWait ) == 1u && NOT_REPLACED );
        }

        areg::TimerManager::wait_timer_manager( );
        AREG_CHILD_CHECK( areg::TimerManager::start_timer_manager( ) );
        areg::Timer marker( run.consumer, "FaultInjectMarkerLost" );
        AREG_CHILD_CHECK( marker.start_timer( 1u, run.dispatcher, areg::Timer::ONE_TIME ) );
        AREG_CHILD_CHECK( run.consumer.wait_count( marker, 1u ) );
        AREG_CHILD_CHECK( (run.consumer.count( lost ) == 1u) || (lost.is_active( ) == false) );
        lost.stop_timer( );
        marker.stop_timer( );
    }

    //////////////////////////////////////////////////////////////////////////
    // The send backlog
    //////////////////////////////////////////////////////////////////////////

    void backlog_without_its_wake_descriptor_writes_and_wakes_its_send_thread( areg_fault::FaultPoint point )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SendQueueGate gate{ };
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( point, EMFILE ) );
        areg::ext::SendBacklog backlog{ gate };
        AREG_CHILD_CHECK( areg_fault::failed( point ) == 1u && NOT_REPLACED );

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

    void inline_write_repeats_an_interrupted_send_and_keeps_nothing_when_the_socket_is_full( )
    {
        LoopPair pair;
        AREG_CHILD_CHECK( pair.open( ) );
        areg::SendQueueGate gate{ };
        areg::ext::SendBacklog backlog{ gate };
        const areg::MessageEnvelope message{ make_message( 256u ) };
        AREG_CHILD_CHECK( message.header( ) != nullptr );

        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EINTR ) );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 7u, pair.server, message ) == 1 );
        }

        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( peer_receives( pair.client, message ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, EAGAIN ) );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 7u, pair.server, message ) == 0 );
        }

        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( backlog.is_empty( ) );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SocketSend, ECONNRESET ) );
        {
            areg::SocketWriteGuard guard{ pair.server };
            AREG_CHILD_CHECK( backlog.write_inline( 7u, pair.server, message ) == -1 );
        }

        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SocketSend ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( backlog.is_empty( ) );
    }

    //////////////////////////////////////////////////////////////////////////
    // The application
    //////////////////////////////////////////////////////////////////////////

    void signal_is_left_alone_while_its_disposition_cannot_be_read( )
    {
        struct sigaction before{ };
        AREG_CHILD_CHECK( ::sigaction( SIGPIPE, nullptr, &before ) == 0 );
        AREG_CHILD_CHECK( before.sa_handler == SIG_DFL );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::SignalAction, EINVAL ) );
            areg::Application::setup( false, false, false, false, false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::SignalAction ) == 1u && NOT_REPLACED );
        }

        struct sigaction during{ };
        AREG_CHILD_CHECK( ::sigaction( SIGPIPE, nullptr, &during ) == 0 );
        AREG_CHILD_CHECK( during.sa_handler == SIG_DFL );

        areg::Application::release( );
        struct sigaction after{ };
        AREG_CHILD_CHECK( ::sigaction( SIGPIPE, nullptr, &after ) == 0 );
        AREG_CHILD_CHECK( after.sa_handler == SIG_DFL );
    }

    void router_service_start_fails_when_no_process_can_be_read_and_the_service_manager_is_unavailable( )
    {
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::FileOpen, EACCES, 0u, 0u ) );
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ProcessSpawn, ENOENT ) );

        AREG_CHILD_CHECK( areg::Application::start_router_service( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::FileOpen ) != 0u && NOT_REPLACED );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ProcessSpawn ) == 1u && NOT_REPLACED );
    }
}

TEST( FaultInjectPosixSyncTest, wait_all_is_fatal_while_the_attributes_of_the_wait_lock_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( wait_all_is_fatal_while_the_wait_state_cannot_be_created( areg_fault::FaultPoint::MutexAttrInit ) );
}

TEST( FaultInjectPosixSyncTest, wait_all_is_fatal_while_the_wait_lock_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( wait_all_is_fatal_while_the_wait_state_cannot_be_created( areg_fault::FaultPoint::MutexInit ) );
}

TEST( FaultInjectPosixSyncTest, wait_all_is_fatal_while_the_attributes_of_the_wait_condition_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( wait_all_is_fatal_while_the_wait_state_cannot_be_created( areg_fault::FaultPoint::CondAttrInit ) );
}

TEST( FaultInjectPosixSyncTest, wait_all_is_fatal_while_the_wait_condition_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( wait_all_is_fatal_while_the_wait_state_cannot_be_created( areg_fault::FaultPoint::CondInit ) );
}

TEST( FaultInjectPosixSyncTest, mutex_is_fatal_while_the_attributes_of_its_lock_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( mutex_is_fatal_while_its_lock_cannot_be_created( areg_fault::FaultPoint::MutexAttrInit ) );
}

TEST( FaultInjectPosixSyncTest, mutex_is_fatal_while_the_type_of_its_lock_cannot_be_set )
{
    AREG_EXPECT_IN_CHILD( mutex_is_fatal_while_its_lock_cannot_be_created( areg_fault::FaultPoint::MutexAttrSetType ) );
}

TEST( FaultInjectPosixSyncTest, mutex_is_fatal_while_its_lock_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( mutex_is_fatal_while_its_lock_cannot_be_created( areg_fault::FaultPoint::MutexInit ) );
}

TEST( FaultInjectPosixSyncTest, timers_are_not_set_while_the_attributes_of_the_timer_condition_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( timers_are_not_set_while_the_timer_service_cannot_be_created( areg_fault::FaultPoint::CondAttrInit, 0u ) );
}

TEST( FaultInjectPosixSyncTest, timers_are_not_set_while_the_clock_of_the_timer_condition_cannot_be_set )
{
    AREG_EXPECT_IN_CHILD( timers_are_not_set_while_the_timer_service_cannot_be_created( areg_fault::FaultPoint::CondAttrSetClock, 0u ) );
}

TEST( FaultInjectPosixSyncTest, timers_are_not_set_while_the_wake_condition_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( timers_are_not_set_while_the_timer_service_cannot_be_created( areg_fault::FaultPoint::CondInit, 0u ) );
}

TEST( FaultInjectPosixSyncTest, timers_are_not_set_while_the_done_condition_cannot_be_created )
{
    AREG_EXPECT_IN_CHILD( timers_are_not_set_while_the_timer_service_cannot_be_created( areg_fault::FaultPoint::CondInit, 1u ) );
}

TEST( FaultInjectPosixSyncTest, timer_is_not_set_while_the_timer_thread_attributes_cannot_be_created_and_works_after )
{
    AREG_EXPECT_IN_CHILD( timer_is_not_set_while_the_timer_thread_attributes_cannot_be_created_and_works_after( ) );
}

TEST( FaultInjectPosixSyncTest, timer_set_again_while_armed_fires )
{
    AREG_EXPECT_IN_CHILD( timer_set_again_while_armed_fires( ) );
}

TEST( FaultInjectPosixSyncTest, thread_does_not_start_while_its_attributes_cannot_be_created_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( thread_does_not_start_while_its_attributes_cannot_be_created_and_starts_after( ) );
}

TEST( FaultInjectPosixSyncTest, timer_manager_does_not_start_while_its_epoll_set_cannot_be_created_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_manager_does_not_start_while_its_descriptors_cannot_be_created_and_starts_after( areg_fault::FaultPoint::EpollCreate, 0u ) );
}

TEST( FaultInjectPosixSyncTest, timer_manager_does_not_start_while_its_command_descriptor_cannot_be_created_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_manager_does_not_start_while_its_descriptors_cannot_be_created_and_starts_after( areg_fault::FaultPoint::EventCreate, 0u ) );
}

TEST( FaultInjectPosixSyncTest, timer_manager_does_not_start_while_its_exit_descriptor_cannot_be_created_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_manager_does_not_start_while_its_descriptors_cannot_be_created_and_starts_after( areg_fault::FaultPoint::EventCreate, 1u ) );
}

TEST( FaultInjectPosixSyncTest, timer_whose_system_timer_cannot_be_created_stays_inactive_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_whose_system_timer_fails_stays_inactive_and_starts_after( areg_fault::FaultPoint::TimerFdCreate ) );
}

TEST( FaultInjectPosixSyncTest, timer_whose_system_timer_cannot_be_armed_stays_inactive_and_starts_after )
{
    AREG_EXPECT_IN_CHILD( timer_whose_system_timer_fails_stays_inactive_and_starts_after( areg_fault::FaultPoint::TimerFdSet ) );
}

TEST( FaultInjectPosixSyncTest, interrupted_wait_of_the_timer_manager_is_repeated_and_the_timer_fires )
{
    AREG_EXPECT_IN_CHILD( interrupted_wait_of_the_timer_manager_is_repeated_and_the_timer_fires( ) );
}

TEST( FaultInjectPosixSyncTest, failed_wait_of_the_timer_manager_stops_it_and_it_starts_again )
{
    AREG_EXPECT_IN_CHILD( failed_wait_of_the_timer_manager_stops_it_and_it_starts_again( ) );
}

TEST( FaultInjectPosixSyncTest, DISABLED_bug_924_timer_started_before_a_failed_manager_wait_fires_or_is_not_active )
{
    AREG_EXPECT_IN_CHILD( timer_started_before_a_failed_manager_wait_fires_or_is_not_active( ) );
}

TEST( FaultInjectPosixSyncTest, backlog_without_its_wake_descriptor_writes_and_wakes_its_send_thread )
{
    AREG_EXPECT_IN_CHILD( backlog_without_its_wake_descriptor_writes_and_wakes_its_send_thread( areg_fault::FaultPoint::EventCreate ) );
}

TEST( FaultInjectPosixSyncTest, backlog_without_memory_for_its_wait_object_writes_and_wakes_its_send_thread )
{
    AREG_SKIP_WITHOUT_ALLOCATION_FAULT( );
    AREG_EXPECT_IN_CHILD( backlog_without_its_wake_descriptor_writes_and_wakes_its_send_thread( areg_fault::FaultPoint::Allocation ) );
}

TEST( FaultInjectPosixSyncTest, inline_write_repeats_an_interrupted_send_and_keeps_nothing_when_the_socket_is_full )
{
    AREG_EXPECT_IN_CHILD( inline_write_repeats_an_interrupted_send_and_keeps_nothing_when_the_socket_is_full( ) );
}

TEST( FaultInjectPosixSyncTest, signal_is_left_alone_while_its_disposition_cannot_be_read )
{
    AREG_EXPECT_IN_CHILD( signal_is_left_alone_while_its_disposition_cannot_be_read( ) );
}

TEST( FaultInjectPosixSyncTest, router_service_start_fails_when_no_process_can_be_read_and_the_service_manager_is_unavailable )
{
    AREG_EXPECT_IN_CHILD( router_service_start_fails_when_no_process_can_be_read_and_the_service_manager_is_unavailable( ) );
}

#endif  // defined(__linux__)
