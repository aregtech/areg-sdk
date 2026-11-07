#ifndef AREG_TESTS_UNITS_CHILDPROCESS_HPP
#define AREG_TESTS_UNITS_CHILDPROCESS_HPP

/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ChildProcess.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Runs a test body in a new process started from the
 *              test executable, on Linux, macOS and Windows.
 *
 *              AREG_EXPECT_IN_CHILD( statements ) runs the statements in a child process and
 *              passes if they return. The local objects of the statements are destroyed before
 *              the child exits. Inside the child, AREG_CHILD_CHECK( condition ) reports a
 *              failed condition on the standard error output and ends the child with
 *              CHILD_CHECK_FAILED; the parent then prints that output. Assertions of googletest
 *              do not report from a child, use AREG_CHILD_CHECK there.
 *
 *              AREG_EXPECT_CHILD_DIES( statements, pattern ) passes if the child ends abnormally
 *              and its standard error output matches the pattern.
 *
 *              A child starts from main() of the test executable and runs only the current test,
 *              so it can set up and release the application once more. The child runs the test
 *              body from its start, so the code before the macro runs in both processes.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"

#include <cstdio>
#include <cstdlib>

namespace areg_child
{
    //!< The exit code of a child whose check failed.
    constexpr int CHILD_CHECK_FAILED{ 97 };

    /**
     * \brief   Reports the failed condition on the standard error output and ends the child.
     * \param   expression  The text of the condition.
     * \param   file        The source file of the check.
     * \param   line        The line of the check.
     **/
    [[noreturn]] inline void fail( const char * expression, const char * file, int line )
    {
        std::fprintf( stderr, "child check failed: %s (%s:%d)\n", expression, file, line );
        std::fflush( stderr );
        std::_Exit( CHILD_CHECK_FAILED );
    }

    /**
     * \brief   Ends the child with exit code 0. Runs the exit handlers, which write the coverage data.
     **/
    [[noreturn]] inline void pass( )
    {
        std::fflush( stdout );
        std::fflush( stderr );
        std::exit( 0 );
    }
}

//!< Ends the child with CHILD_CHECK_FAILED if the condition is false.
#define AREG_CHILD_CHECK( condition )                                               \
    do                                                                              \
    {                                                                               \
        if ( !( condition ) )                                                       \
            ::areg_child::fail( #condition, __FILE__, __LINE__ );                   \
    } while ( false )

#if GTEST_HAS_DEATH_TEST

    //!< Runs the statements in a child process; passes if they return.
    #define AREG_EXPECT_IN_CHILD( ... )                                             \
        do                                                                          \
        {                                                                           \
            ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";               \
            EXPECT_EXIT( { { __VA_ARGS__; } ::areg_child::pass( ); }                \
                       , ::testing::ExitedWithCode( 0 ), "" );                      \
        } while ( false )

    //!< Runs the statements in a child process; passes if it dies with the pattern on stderr.
    #define AREG_EXPECT_CHILD_DIES( statements, pattern )                           \
        do                                                                          \
        {                                                                           \
            ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";               \
            EXPECT_DEATH( { statements; }, pattern );                               \
        } while ( false )

#else   // GTEST_HAS_DEATH_TEST

    #define AREG_EXPECT_IN_CHILD( ... )                                             \
        GTEST_SKIP( ) << "child processes are not supported on this platform"

    #define AREG_EXPECT_CHILD_DIES( statements, pattern )                           \
        GTEST_SKIP( ) << "child processes are not supported on this platform"

#endif  // GTEST_HAS_DEATH_TEST

#endif  // AREG_TESTS_UNITS_CHILDPROCESS_HPP
