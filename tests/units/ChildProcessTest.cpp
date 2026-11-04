/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ChildProcessTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the child process template of units/ChildProcess.hpp.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/ChildProcess.hpp"
#include "areg/appbase/Application.hpp"

#include "gtest/gtest-spi.h"

#include <cstdio>

namespace
{
    //!< Changed only by a child.
    int gChildTouched{ 0 };
}

TEST( ChildProcessTest, statements_run_in_another_process )
{
    AREG_EXPECT_IN_CHILD(
        gChildTouched = 1;
        AREG_CHILD_CHECK( gChildTouched == 1 );
    );

    EXPECT_EQ( gChildTouched, 0 );
}

TEST( ChildProcessTest, failed_check_is_reported_with_its_condition )
{
    EXPECT_NONFATAL_FAILURE( AREG_EXPECT_IN_CHILD( AREG_CHILD_CHECK( gChildTouched == 3 ) ), "child check failed: gChildTouched == 3" );
}

TEST( ChildProcessTest, abnormal_end_is_reported_with_its_output )
{
    AREG_EXPECT_CHILD_DIES( std::fprintf( stderr, "child aborts\n" ); std::abort( ), "child aborts" );
}

TEST( ChildProcessTest, each_child_sets_up_and_releases_the_application )
{
    for ( int round = 0; round < 2; ++ round )
    {
        AREG_EXPECT_IN_CHILD(
            areg::Application::setup( false, false, false, true, false );
            AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Ready );
            areg::Application::release( );
        );
    }
}
