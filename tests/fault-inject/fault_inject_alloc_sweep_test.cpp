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
 *              turn, a model whose component thread cannot be created, and the lifecycle
 *              requests and reports whose allocation fails.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "fault-inject/FaultInject.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include <atomic>
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

    //!< The allocations of one request that are failed in turn; a request makes no more.
    constexpr uint32_t      REQUEST_COUNT   { 16u };
    //!< The longest wait of a child for an event, in milliseconds.
    constexpr uint32_t      WAIT_MS         { 30'000u };
    //!< The watchdog timeout of the held thread, in milliseconds.
    constexpr uint32_t      HOLD_WATCH_MS   { 100u };
    constexpr char const    _holdModel[]    { "FaultHoldModel" };
    constexpr char const    _holdThread[]   { "FaultHoldThread" };

    //!< Set by the handler that holds its thread.
    std::atomic_bool        gHolding        { false };
    //!< Signaled to let the held handler return.
    areg::SyncEvent         gRelease        { true, false };
    //!< Signaled when a new instance of the held thread started.
    areg::SyncEvent         gReplaced       { true, false };
    //!< Signaled when a component of the held thread was deleted.
    areg::SyncEvent         gDeleted        { true, false };

    //!< True if the child returned normally.
    bool _controlled_exit( int status )
    {
        return WIFEXITED( status ) && ( WEXITSTATUS( status ) == 0 );
    }

    //!< A component whose first user state fails the next allocation and holds the thread.
    class HoldComponent : public areg::Component
    {
    public:
        HoldComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }

    protected:
        void startup_component( areg::ComponentThread & comThread ) override
        {
            areg::Component::startup_component( comThread );
            if ( gHolding.load( ) )
            {
                gReplaced.set_signaled( );
            }
        }

        void on_user_state( uint32_t /*previous*/, uint32_t /*current*/ ) override
        {
            if ( gHolding.exchange( true ) == false )
            {
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 1u ) );
                static_cast<void>( gRelease.lock( WAIT_MS ) );
            }
        }
    };

    areg::Component * _create_hold( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    {
        return new HoldComponent( entry, owner );
    }

    void _delete_hold( areg::Component & comp, const areg::ComponentEntry & /*entry*/ )
    {
        delete &comp;
        gDeleted.set_signaled( );
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

    //!< Registers the model with the watchdog and the Restart policy once.
    bool _register_hold_model( )
    {
        static const bool _registered{ [] {
            const areg::String thread( _holdThread );
            areg::ComponentThreadEntry entry{ thread };
            entry.mWatchdogTimeout = HOLD_WATCH_MS;
            entry.mFailurePolicy = areg::FailurePolicy::Restart;
            entry.add_component( areg::ComponentEntry( thread, areg::String( "FaultHoldComponent" ), &_create_hold, &_delete_hold ) );
            areg::Model model( _holdModel );
            model.add_thread( entry );
            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

    //!< Fails each allocation of the requests in turn: a refused request returns false and changes nothing.
    void _refused_requests_change_nothing( )
    {
        areg::Application::setup( false, true, false, true, false );
        AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) );
        const uint32_t userState{ areg::Application::user_state( ) };

        uint32_t refusedState{ 0u };
        uint32_t refusedUnload{ 0u };
        {
            areg_fault::FaultScope faults;
            for ( uint32_t skip = 0u; skip < REQUEST_COUNT; ++ skip )
            {
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
                const bool unloaded{ areg::Application::unload_model( _sweepModel ) };
                const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
                ::areg_fault_disarm_all( );
                if ( failed == 0u )
                {
                    AREG_CHILD_CHECK( unloaded );
                    break;
                }

                if ( unloaded == false )
                {
                    AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Loaded );
                    ++ refusedUnload;
                    continue;
                }

                AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Unloaded );
                AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) );
            }

            for ( uint32_t skip = 0u; skip < REQUEST_COUNT; ++ skip )
            {
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
                if ( areg::Application::set_user_state( userState + 1u ) )
                    break;

                AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u );
                AREG_CHILD_CHECK( areg::Application::user_state( ) == userState );
                ++ refusedState;
            }
        }

        AREG_CHILD_CHECK( refusedState != 0u );
        AREG_CHILD_CHECK( refusedUnload != 0u );
        AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Unloaded );
        areg::Application::release( );
    }

    //!< Holds a thread until its watchdog expires, with the first report of the expiry refused.
    void _refused_watchdog_report_is_retried( )
    {
        areg_fault::FaultScope faults;
        areg::Application::setup( false, true, false, true, true );
        AREG_CHILD_CHECK( areg::Application::load_model( _holdModel, true ) );
        AREG_CHILD_CHECK( areg::Application::set_user_state( 1u ) );
        AREG_CHILD_CHECK( gReplaced.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u );
        gRelease.set_signaled( );
        AREG_CHILD_CHECK( gDeleted.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( areg::Application::unload_model( _holdModel ) );
        areg::Application::release( );
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
 * \brief   Reproduces the defect: each nothrow allocation of a model lifecycle fails in turn, one
 *          per child. The load may fail, but no lifecycle event is lost: the process neither
 *          crashes, nor hangs, nor is ended by the failure policy.
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

/**
 * \brief   Reproduces the defect: a request that cannot be built or posted reports it to the
 *          caller, and the model and the user state stay as they were.
 **/
TEST( FaultInjectSweepTest, lifecycle_request_that_cannot_be_posted_returns_false_and_changes_nothing )
{
    ASSERT_TRUE( _register_model( ) );
    AREG_EXPECT_IN_CHILD( _refused_requests_change_nothing( ) );
}

/**
 * \brief   Reproduces the defect: the report of an expired watchdog that cannot be built is sent
 *          again later, and the failure policy of the hung thread still runs.
 **/
TEST( FaultInjectSweepTest, watchdog_report_that_cannot_be_built_once_still_runs_the_policy )
{
    ASSERT_TRUE( _register_hold_model( ) );
    AREG_EXPECT_IN_CHILD( _refused_watchdog_report_is_retried( ) );
}

#endif  // defined(AREG_FAULT_SWEEP_TESTS)
