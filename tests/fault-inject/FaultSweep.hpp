#ifndef AREG_TESTS_FAULT_INJECT_FAULTSWEEP_HPP
#define AREG_TESTS_FAULT_INJECT_FAULTSWEEP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/FaultSweep.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Allocation sweeps that fail one allocation per child
 *              process, and the fatal error of a synchronization object without memory, which
 *              ends a child as a result of the sweep. Linux only.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#if defined(__linux__)

#include "areg/base/FatalError.hpp"

#include <sys/wait.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace areg_sweep
{
    //!< The outcome of one step of a sweep, which fails one allocation.
    enum class StepResult : int
    {
          Survived  = 0     //!< The allocation failed and the operation succeeded.
        , SyncFatal = 3     //!< A synchronization object could not be allocated: the fatal error ended the child.
        , Done      = 4     //!< The armed allocation was not made: the sweep is complete.
        , Refused   = 5     //!< The allocation failed and the operation reported it.
    };

    //!< The fatal errors of a synchronization object whose OS object cannot be allocated.
    constexpr const char * const SYNC_FATAL_REASONS[]
    {
          "no memory, a Mutex cannot be created"
        , "no memory, a SyncEvent cannot be created"
        , "no memory, a Semaphore cannot be created"
        , "no memory, a CriticalSection cannot be created"
        , "no memory, a SyncTimer cannot be created"
    };

    //!< Ends the child with StepResult::SyncFatal for the fatal error of a synchronization object, otherwise as failed.
    inline void exit_on_sync_fatal( const char * reason ) noexcept
    {
        ::areg_fault_disarm_all( );
        for ( const char * expected : SYNC_FATAL_REASONS )
        {
            if ( ( reason != nullptr ) && ( std::strcmp( reason, expected ) == 0 ) )
            {
                std::_Exit( static_cast<int>( StepResult::SyncFatal ) );
            }
        }

        std::fprintf( stderr, "child check failed: unexpected fatal error '%s'\n", reason != nullptr ? reason : "" );
        std::fflush( stderr );
        std::_Exit( ::areg_child::CHILD_CHECK_FAILED );
    }

    //!< Lets the fatal error of a synchronization object without memory end the child.
    inline void accept_sync_fatal( )
    {
        static_cast<void>( areg::set_fatal_handler( &exit_on_sync_fatal ) );
    }

    //!< Ends the child with the result of the step. Runs the exit handlers.
    [[noreturn]] inline void exit_with( StepResult result )
    {
        std::fflush( stdout );
        std::fflush( stderr );
        std::exit( static_cast<int>( result ) );
    }

    //!< The exit status of the last child of a sweep, set in the parent.
    inline int gChildStatus { -1 };

    //!< True if the child returned normally or ended with the fatal error of a synchronization object.
    inline bool survived_or_sync_fatal( int status )
    {
        return WIFEXITED( status ) && ( ( WEXITSTATUS( status ) == 0 ) || ( WEXITSTATUS( status ) == static_cast<int>( StepResult::SyncFatal ) ) );
    }

    //!< Keeps the status of the child; true if it ended with a step result.
    inline bool step_exit( int status )
    {
        gChildStatus = status;
        if ( WIFEXITED( status ) == false )
            return false;

        const int code{ WEXITSTATUS( status ) };
        return ( code == static_cast<int>( StepResult::Survived ) ) || ( code == static_cast<int>( StepResult::SyncFatal ) )
            || ( code == static_cast<int>( StepResult::Done ) )     || ( code == static_cast<int>( StepResult::Refused ) );
    }

    //!< True if the last child of the sweep ended with the result.
    inline bool child_result_is( StepResult result )
    {
        return WIFEXITED( gChildStatus ) && ( WEXITSTATUS( gChildStatus ) == static_cast<int>( result ) );
    }

    /**
     * \brief   Runs the step in one child per allocation, the allocation at index skip failing,
     *          until a step reports that the armed allocation was not made. At least one step
     *          must end with the required result. Must be the last death test of its test.
     *
     * \param   limit       The most steps of the sweep.
     * \param   required    The result at least one step must end with.
     * \param   step        Called in the child with the index of the failing allocation; returns the result.
     **/
    template<typename Step>
    void sweep_in_children( uint32_t limit, StepResult required, Step step )
    {
        uint32_t matched{ 0u };
        bool done{ false };
        for ( uint32_t skip = 0u; ( skip < limit ) && ( done == false ); ++ skip )
        {
            SCOPED_TRACE( "failed allocation " + std::to_string( skip ) );
            gChildStatus = -1;
            ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
            EXPECT_EXIT( {
                accept_sync_fatal( );
                exit_with( step( skip ) );
            }, step_exit, "" );
            done = child_result_is( StepResult::Done );
            matched += child_result_is( required ) ? 1u : 0u;
        }

        EXPECT_TRUE( done );
        EXPECT_NE( matched, 0u );
    }
}

#endif  // defined(__linux__)

#endif  // AREG_TESTS_FAULT_INJECT_FAULTSWEEP_HPP
