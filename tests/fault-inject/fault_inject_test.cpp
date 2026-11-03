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

#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#include <errno.h>

namespace
{
#if defined(_WIN32)
    constexpr int THREAD_ERROR  { 8 };      // ERROR_NOT_ENOUGH_MEMORY
    constexpr int SOCKET_ERROR_ { 10024 };  // WSAEMFILE
    constexpr int FILE_ERROR    { 4 };      // ERROR_TOO_MANY_OPEN_FILES
#else   // defined(_WIN32)
    constexpr int THREAD_ERROR  { EAGAIN };
    constexpr int SOCKET_ERROR_ { EMFILE };
    constexpr int FILE_ERROR    { EMFILE };
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
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, THREAD_ERROR ) );

        ReturningConsumer consumer;
        areg::Thread thread( consumer, "FaultInjectThread" );
        AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u && NOT_REPLACED );
        AREG_CHILD_CHECK( thread.is_running( ) == false );

        AREG_CHILD_CHECK( thread.start( areg::WAIT_INFINITE ) );
        static_cast<void>(thread.shutdown( areg::WAIT_INFINITE ));
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
