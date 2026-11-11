/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_posix_base_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the POSIX waitable timer service when the system refuses
 *              the timer thread, and when the process forks. Each test runs in a child process.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if !defined(_WIN32)

#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>

#if defined(__SANITIZE_THREAD__)
    #define AREG_SKIP_UNDER_TSAN( )     GTEST_SKIP( ) << "the ThreadSanitizer does not start a thread in a child forked from a multithreaded process"
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define AREG_SKIP_UNDER_TSAN( ) GTEST_SKIP( ) << "the ThreadSanitizer does not start a thread in a child forked from a multithreaded process"
    #endif  // __has_feature(thread_sanitizer)
#endif  // defined(__SANITIZE_THREAD__)

#ifndef AREG_SKIP_UNDER_TSAN
    #define AREG_SKIP_UNDER_TSAN( )
#endif  // AREG_SKIP_UNDER_TSAN

namespace
{
    //!< The message of a fault point that did not fail any call.
    constexpr const char * NOT_REPLACED{ "the fault library did not replace the system call" };

    //!< A timeout that no timer reaches during a test.
    constexpr uint32_t  TIMER_NEVER_MS  { 3'600'000u };

    //!< The limit of a wait for a timer that must fire.
    constexpr uint32_t  TIMER_WAIT_MS   { 30'000u };

    //!< The seconds after which a forked process that does not end is killed.
    constexpr unsigned  FORK_LIMIT_SEC  { 60u };

    //!< Forks through a function pointer, which the coverage instrumentation does not wrap.
    pid_t fork_process( )
    {
        pid_t (* volatile forkCall)( ) { &::fork };
        return forkCall( );
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

    void timer_is_not_armed_while_the_timer_thread_cannot_start_and_works_after( )
    {
        start_first_thread( );
        areg::SyncTimer timer( 1u, false, true );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN ) );
        AREG_CHILD_CHECK( timer.set_timer( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( timer.lock( areg::DO_NOT_WAIT ) == false );

        AREG_CHILD_CHECK( timer.set_timer( ) );
        AREG_CHILD_CHECK( timer.lock( TIMER_WAIT_MS ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u );
    }

    void timers_work_in_the_parent_and_in_a_forked_child( )
    {
        areg::SyncTimer armed( TIMER_NEVER_MS, true, true );
        AREG_CHILD_CHECK( armed.set_timer( ) );

        const pid_t pid{ fork_process( ) };
        AREG_CHILD_CHECK( pid >= 0 );
        if ( pid == 0 )
        {
            ::alarm( FORK_LIMIT_SEC );
            areg::SyncTimer timer( 1u, false, true );
            const bool fired{ timer.set_timer( ) && timer.lock( TIMER_WAIT_MS ) };
            std::exit( fired ? 0 : 1 );
        }

        areg::SyncTimer parentTimer( 1u, false, true );
        AREG_CHILD_CHECK( parentTimer.set_timer( ) );
        AREG_CHILD_CHECK( parentTimer.lock( TIMER_WAIT_MS ) );

        int status{ 0 };
        AREG_CHILD_CHECK( ::waitpid( pid, &status, 0 ) == pid );
        AREG_CHILD_CHECK( WIFEXITED( status ) && ( WEXITSTATUS( status ) == 0 ) );
        AREG_CHILD_CHECK( armed.lock( areg::DO_NOT_WAIT ) == false );
    }
}

TEST( FaultInjectPosixBaseTest, timer_is_not_armed_while_the_timer_thread_cannot_start_and_works_after )
{
    AREG_EXPECT_IN_CHILD( timer_is_not_armed_while_the_timer_thread_cannot_start_and_works_after( ) );
}

TEST( FaultInjectPosixBaseTest, timers_work_in_the_parent_and_in_a_forked_child )
{
    AREG_SKIP_UNDER_TSAN( );
    AREG_EXPECT_IN_CHILD( timers_work_in_the_parent_and_in_a_forked_child( ) );
}

#endif  // !defined(_WIN32)
