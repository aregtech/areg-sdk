/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        fault-inject/fault_inject_alloc_sweep_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a model lifecycle in which each nothrow allocation fails in
 *              turn, and a model whose component thread cannot be created.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include <cerrno>
#include <string>

#if defined(__linux__)
    #include <sys/wait.h>
#endif  // defined(__linux__)

// The sanitizer runtimes define the global allocation functions themselves.
#if defined(__linux__) && !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
    #if defined(__has_feature)
        #if !__has_feature(address_sanitizer) && !__has_feature(thread_sanitizer) && !__has_feature(memory_sanitizer)
            #define AREG_FAULT_SWEEP_TESTS  1
        #endif  // !__has_feature(...)
    #else   // defined(__has_feature)
        #define AREG_FAULT_SWEEP_TESTS      1
    #endif  // defined(__has_feature)
#endif  // defined(__linux__) && !sanitizer

namespace
{
    constexpr char const    _sweepModel[]   { "FaultSweepModel" };
    constexpr char const    _sweepThread[]  { "FaultSweepThread" };

#if defined(AREG_FAULT_SWEEP_TESTS)
    //!< The allocations the sweep fails, one per child; a lifecycle makes no more.
    constexpr uint32_t      SWEEP_COUNT     { 64u };

    //!< The exit code of the failure policy Exit.
    constexpr int           POLICY_EXIT_CODE{ 70 };

    //!< True if the child returned, or was ended by the failure policy Exit after a lost lifecycle event.
    bool _controlled_exit( int status )
    {
        return WIFEXITED( status ) && ( ( WEXITSTATUS( status ) == 0 ) || ( WEXITSTATUS( status ) == POLICY_EXIT_CODE ) );
    }
#endif  // defined(AREG_FAULT_SWEEP_TESTS)

    //!< A component that does nothing.
    class SweepComponent : public areg::Component
    {
    public:
        SweepComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }
    };

    areg::Component * _create_sweep( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    {
        return new SweepComponent( entry, owner );
    }

    void _delete_sweep( areg::Component & comp, const areg::ComponentEntry & /*entry*/ )
    {
        delete &comp;
    }

    //!< Registers the model of this file once.
    bool _register_model( )
    {
        static const bool _registered{ [] {
            const areg::String thread( _sweepThread );
            areg::ComponentThreadEntry entry{ thread };
            entry.add_component( areg::ComponentEntry( thread, areg::String( "FaultSweepComponent" ), &_create_sweep, &_delete_sweep ) );
            areg::Model model( _sweepModel );
            model.add_thread( entry );
            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

#if defined(AREG_FAULT_SWEEP_TESTS)
    //!< Sets up the system, loads and unloads the model, and releases the system.
    void _lifecycle( )
    {
        areg::Application::setup( false, true, false, true, false );
        static_cast<void>( areg::Application::load_model( _sweepModel, true ) );
        areg::Application::unload_model( _sweepModel );
        areg::Application::release( );
    }

    //!< Returns the number of nothrow allocations of one lifecycle.
    uint32_t _count_allocations( )
    {
        areg_fault::FaultScope faults;
        if ( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0xFFFF'FFFFu, 1u ) == false )
            return 0u;

        _lifecycle( );
        return areg_fault::calls( areg_fault::FaultPoint::Allocation );
    }
#endif  // defined(AREG_FAULT_SWEEP_TESTS)
}

/**
 * \brief   A component thread the system cannot create fails the load of its model, which is
 *          left unloaded; the next load of the same model starts the thread.
 **/
TEST( FaultInjectSweepTest, model_whose_thread_cannot_be_created_fails_to_load_and_loads_later )
{
    ASSERT_TRUE( _register_model( ) );
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, true, false, true, false );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::ThreadCreate, EAGAIN ) );
            AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::ThreadCreate ) == 1u );
        }
        AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Unloaded );
        AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) );
        areg::Application::unload_model( _sweepModel );
        AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Unloaded );
        areg::Application::release( );
    );
}

#if defined(AREG_FAULT_SWEEP_TESTS)

/**
 * \brief   Each nothrow allocation of a model lifecycle fails in turn, one per child: the load
 *          may fail, but the process neither crashes nor hangs. It returns, or a lost lifecycle
 *          event ends it through the failure policy Exit.
 **/
TEST( FaultInjectSweepTest, every_failed_allocation_of_a_model_lifecycle_is_survived )
{
    ASSERT_TRUE( _register_model( ) );
    const uint32_t allocations{ _count_allocations( ) };
    EXPECT_GT( allocations, 0u );
    EXPECT_LE( allocations, SWEEP_COUNT );

    for ( uint32_t index = 0u; index < SWEEP_COUNT; ++ index )
    {
        SCOPED_TRACE( "failed allocation " + std::to_string( index ) + " of " + std::to_string( allocations ) );
        ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
        EXPECT_EXIT( {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, index, 1u ) );
            _lifecycle( );
            ::areg_child::pass( );
        }, _controlled_exit, "" );
    }
}

#endif  // defined(AREG_FAULT_SWEEP_TESTS)
