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
#include "areg/base/FixedArray.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceItem.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/private/Watchdog.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "units/ServiceTestServer.hpp"

#if AREG_LOGGING
    #include "areg/ipc/RemoteServiceDefs.hpp"
    #include "areglogger/client/LogObserverApi.h"
    #include "areglogger/client/private/LoggerClient.hpp"
    #include "areglogger/client/private/ObserverMessageProcessor.hpp"
#endif  // AREG_LOGGING

#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#if defined(__linux__)
    #include <sys/wait.h>
    #include <unistd.h>
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
    constexpr char const    _hangModel[]    { "FaultHangModel" };
    constexpr char const    _hangThread[]   { "FaultHangThread" };
    constexpr char const    _longModel[]    { "FaultLongNameModelThatDoesNotFitTheSmallStringBuffer" };
    constexpr char const    _longThread[]   { "FaultLongNameThreadThatDoesNotFitTheSmallStringBuffer" };
    constexpr char const    _largeModel[]   { "FaultLargeModel" };
    constexpr char const    _hostModel[]    { "FaultHostModel" };
    constexpr char const    _hostThread[]   { "FaultHostThread" };
    constexpr char const    _hostWorker[]   { "FaultHostWorker" };
    constexpr char const    _hungModel[]    { "FaultHungModel" };
    constexpr char const    _hungThread[]   { "FaultHungThread" };
    constexpr char const    _hungWorker[]   { "FaultHungWorker" };
    constexpr char const    _managerThread[]{ "_AREG_SERVICE_MANAGER_THREAD_" };

    //!< The lifecycle deadline of the thread that hangs in its start, in milliseconds.
    constexpr uint32_t      HANG_DEADLINE_MS{ 50u };
    //!< The seconds after which a child that hangs is killed.
    constexpr uint32_t      CHILD_ALARM_S   { 20u };
    //!< One thread more than the first block of the records holds.
    constexpr uint32_t      LARGE_THREADS   { 9u };
    //!< The most allocations of one model load that are failed in turn.
    constexpr uint32_t      ATTEMPT_COUNT   { 400u };
    //!< The watchdog timeout of the worker thread, in milliseconds.
    constexpr uint32_t      WORKER_WATCH_MS { 10'000u };

    //!< Set by the handler that holds its thread.
    std::atomic_bool        gHolding        { false };
    //!< Signaled to let the held handler return.
    areg::SyncEvent         gRelease        { true, false };
    //!< Signaled when a new instance of the held thread started.
    areg::SyncEvent         gReplaced       { true, false };
    //!< Signaled when a component of the held thread was deleted.
    areg::SyncEvent         gDeleted        { true, false };
    //!< Signaled when the component that creates a worker thread is done.
    areg::SyncEvent         gHostDone       { true, false };
    //!< True if the new instance of the hung thread could create the worker of the same name.
    std::atomic_bool        gWorkerReplaced { false };
    //!< The worker threads that could not be created because an allocation failed.
    std::atomic<uint32_t>   gRefusedWorkers { 0u };

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

    //!< A component that hangs in its start until it is released.
    class HangComponent : public areg::Component
    {
    public:
        HangComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }

    protected:
        void startup_component( areg::ComponentThread & comThread ) override
        {
            areg::Component::startup_component( comThread );
            static_cast<void>( gRelease.lock( WAIT_MS ) );
        }
    };

    //!< A worker consumer that does nothing.
    class HostConsumer final : public areg::WorkerThreadConsumer
    {
    public:
        HostConsumer( )
            : areg::WorkerThreadConsumer( areg::String( _hostWorker ) )
        { }
    };

    //!< A component that creates a guarded worker with each allocation failing in turn.
    class HostComponent : public areg::Component
    {
    public:
        HostComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }

    protected:
        void startup_component( areg::ComponentThread & comThread ) override
        {
            areg::Component::startup_component( comThread );
            const areg::String name( _hostWorker );
            for ( uint32_t skip = 0u; skip < REQUEST_COUNT; ++ skip )
            {
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
                areg::WorkerThread * worker{ create_worker_thread( name, mConsumer, comThread, WORKER_WATCH_MS ) };
                const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
                ::areg_fault_disarm_all( );
                if ( worker == nullptr )
                {
                    AREG_CHILD_CHECK( failed == 1u );
                    gRefusedWorkers.fetch_add( 1u );
                    continue;
                }

                delete_worker_thread( name );
                if ( failed == 0u )
                    break;
            }

            gHostDone.set_signaled( );
        }

    private:
        HostConsumer mConsumer;
    };

    //!< A component with a worker, whose first user state holds its thread.
    class HungHostComponent : public areg::Component
    {
    public:
        HungHostComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }

    protected:
        void startup_component( areg::ComponentThread & comThread ) override
        {
            areg::Component::startup_component( comThread );
            areg::WorkerThread * const worker{ create_worker_thread( areg::String( _hungWorker ), mConsumer, comThread ) };
            if ( gHolding.load( ) )
            {
                gWorkerReplaced.store( worker != nullptr );
                gReplaced.set_signaled( );
            }
        }

        void shutdown_component( areg::ComponentThread & comThread ) override
        {
            delete_worker_thread( areg::String( _hungWorker ) );
            areg::Component::shutdown_component( comThread );
        }

        void on_user_state( uint32_t /*previous*/, uint32_t /*current*/ ) override
        {
            if ( gHolding.exchange( true ) == false )
            {
                static_cast<void>( gRelease.lock( WAIT_MS ) );
            }
        }

    private:
        HostConsumer mConsumer;
    };

    template<class ComponentType>
    areg::Component * _create_component( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    {
        return new ComponentType( entry, owner );
    }

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

    //!< Registers a model of one component thread once, with a component of the given type.
    bool _add_single_model( const char * modelName, const char * threadName, const char * roleName, FuncCreateComponent create, uint32_t deadline, areg::FailurePolicy policy )
    {
        const areg::String thread( threadName );
        areg::ComponentThreadEntry entry{ thread };
        entry.mLifecycleTimeout = deadline;
        entry.mFailurePolicy = policy;
        entry.add_component( areg::ComponentEntry( thread, areg::String( roleName ), create, &_delete_sweep ) );
        areg::Model model{ areg::String( modelName ) };
        model.add_thread( entry );
        return areg::ComponentLoader::add_model_unique( model );
    }

    bool _register_hang_model( )
    {
        static const bool _registered{ _add_single_model( _hangModel, _hangThread, "FaultHangRole", &_create_component<HangComponent>, HANG_DEADLINE_MS, areg::FailurePolicy::Restart ) };
        return _registered;
    }

    bool _register_long_model( )
    {
        static const bool _registered{ _add_single_model( _longModel, _longThread, "FaultLongRole", &_create_sweep, 0u, areg::FailurePolicy::Exit ) };
        return _registered;
    }

    bool _register_host_model( )
    {
        static const bool _registered{ _add_single_model( _hostModel, _hostThread, "FaultHostRole", &_create_component<HostComponent>, 0u, areg::FailurePolicy::Exit ) };
        return _registered;
    }

    //!< Registers once the model whose thread has a worker, with the watchdog and the Restart policy.
    bool _register_hung_model( )
    {
        static const bool _registered{ [] {
            const areg::String thread( _hungThread );
            areg::ComponentThreadEntry entry{ thread };
            entry.mWatchdogTimeout = HOLD_WATCH_MS;
            entry.mFailurePolicy = areg::FailurePolicy::Restart;
            entry.add_component( areg::ComponentEntry( thread, areg::String( "FaultHungRole" ), &_create_component<HungHostComponent>, &_delete_hold ) );
            areg::Model model( _hungModel );
            model.add_thread( entry );
            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

    //!< Registers once the model with more threads than the first block of the records holds.
    bool _register_large_model( )
    {
        static const bool _registered{ [] {
            areg::Model model{ areg::String( _largeModel ) };
            for ( uint32_t i = 0u; i < LARGE_THREADS; ++ i )
            {
                const std::string suffix{ std::to_string( i ) };
                const areg::String thread( ( std::string( "FaultLargeT" ) + suffix ).c_str( ) );
                areg::ComponentThreadEntry entry{ thread };
                entry.add_component( areg::ComponentEntry( thread, areg::String( ( std::string( "FaultLargeR" ) + suffix ).c_str( ) ), &_create_sweep, &_delete_sweep ) );
                model.add_thread( entry );
            }

            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

    //!< Sets up the system and loads the model with a thread that hangs, with one allocation failing.
    void _hang_lifecycle( uint32_t index )
    {
        ::alarm( CHILD_ALARM_S );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, index, 1u ) );
        areg::Application::setup( false, true, false, true, true );
        const bool loaded{ areg::Application::load_model( _hangModel, true ) };
        ::areg_fault_disarm_all( );
        gRelease.set_signaled( );
        AREG_CHILD_CHECK( loaded == false );
        static_cast<void>( areg::Application::unload_model( _hangModel ) );
        areg::Application::release( );
    }

    //!< Loads the model with long names and counts the throwing allocations of its unload.
    void _unload_allocates_nothing( )
    {
        areg::Application::setup( false, true, false, true, false );
        AREG_CHILD_CHECK( areg::Application::load_model( _longModel, true ) );
        areg::Thread * const manager{ areg::Thread::find_by_address( areg::ThreadAddress( _managerThread ) ) };
        AREG_CHILD_CHECK( manager != nullptr );

        ::areg_fault_count_throwing_new( static_cast<uint64_t>( manager->id( ) ) );
        AREG_CHILD_CHECK( areg::Application::unload_model( _longModel ) );
        const uint32_t counted{ ::areg_fault_throwing_new_count( ) };
        ::areg_fault_count_throwing_new( 0u );

        AREG_CHILD_CHECK( counted == 0u );
        areg::Application::release( );
    }

    //!< Fails each allocation of the load of the large model in turn: the model is loaded or left unloaded.
    void _large_load_survives( )
    {
        areg::Application::setup( false, true, false, true, false );
        areg_fault::FaultScope faults;
        uint32_t refused{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < ATTEMPT_COUNT ) && ( finished == false ); ++ skip )
        {
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            const bool loaded{ areg::Application::load_model( _largeModel, true ) };
            const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );
            if ( loaded )
            {
                AREG_CHILD_CHECK( areg::Application::model_state( _largeModel ) == areg::ModelState::Loaded );
                AREG_CHILD_CHECK( areg::Application::unload_model( _largeModel ) );
            }
            else
            {
                AREG_CHILD_CHECK( finished == false );
                AREG_CHILD_CHECK( areg::Application::model_state( _largeModel ) == areg::ModelState::Unloaded );
                ++ refused;
            }
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( refused != 0u );
        areg::Application::release( );
    }

    //!< A guard armed when the watchdog manager stops leaves its list and can be destroyed.
    void _guard_outlives_the_manager( )
    {
        areg::Application::setup( false, true, false, true, true );
        {
            areg::Watchdog guard{ areg::String( "FaultGuard" ) };
            AREG_CHILD_CHECK( guard.arm( WAIT_MS ) );
            areg::Application::stop_watchdog_manager( );
            AREG_CHILD_CHECK( guard.arm( WAIT_MS ) == false );
        }

        areg::Application::release( );
    }

    //!< Holds a thread with a worker until it is abandoned: the name of the worker is free for its new instance.
    void _abandoned_thread_releases_its_worker( )
    {
        areg::Application::setup( false, true, false, true, true );
        AREG_CHILD_CHECK( areg::Application::load_model( _hungModel, true ) );
        AREG_CHILD_CHECK( areg::Application::set_user_state( 1u ) );
        AREG_CHILD_CHECK( gReplaced.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( gWorkerReplaced.load( ) );
        gRelease.set_signaled( );
        AREG_CHILD_CHECK( gDeleted.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( areg::Application::unload_model( _hungModel ) );
        areg::Application::release( );
    }

    //!< Arms a guard repeatedly while every allocation fails.
    void _arming_allocates_nothing( )
    {
        areg::Application::setup( false, true, false, true, true );
        {
            areg::Watchdog guard{ areg::String( "FaultGuard" ) };
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
            for ( uint32_t i = 0u; i < 5u; ++ i )
            {
                AREG_CHILD_CHECK( guard.arm( WAIT_MS ) );
            }

            AREG_CHILD_CHECK( areg_fault::calls( areg_fault::FaultPoint::Allocation ) == 0u );
            guard.stop_guard( );
        }

        areg::Application::release( );
    }

    //!< Fails each allocation of a watchdog in turn: a watchdog without its parts reports it cannot be armed.
    void _watchdog_without_parts_cannot_arm( )
    {
        areg_fault::FaultScope faults;
        uint32_t refused{ 0u };
        for ( uint32_t skip = 0u; skip < REQUEST_COUNT; ++ skip )
        {
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            areg::Watchdog guard{ areg::String( "FaultGuard" ) };
            const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
            ::areg_fault_disarm_all( );
            if ( failed == 0u )
            {
                AREG_CHILD_CHECK( guard.can_arm( ) );
                break;
            }

            AREG_CHILD_CHECK( guard.can_arm( ) == false );
            ++ refused;
        }

        AREG_CHILD_CHECK( refused != 0u );
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
 * \brief   Reproduces the defect: an allocation that fails while the deadline guard is created or
 *          armed left a hung thread without a deadline, and the load never returned. Each
 *          allocation of the setup and the load fails in turn, and the load always ends.
 **/
TEST( FaultInjectSweepTest, failed_allocation_never_leaves_a_hung_thread_without_a_deadline )
{
    ASSERT_TRUE( _register_hang_model( ) );
    for ( uint32_t index = 0u; index < SWEEP_COUNT; ++ index )
    {
        SCOPED_TRACE( "failed allocation " + std::to_string( index ) );
        ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
        EXPECT_EXIT( {
            _hang_lifecycle( index );
            ::areg_child::pass( );
        }, _controlled_exit, "" );
    }
}

/**
 * \brief   Reproduces the defect: arming a guard allocated an event each time, so a guard could
 *          not be armed while the memory was short.
 **/
TEST( FaultInjectSweepTest, arming_a_guard_allocates_nothing )
{
    AREG_EXPECT_IN_CHILD( _arming_allocates_nothing( ) );
}

/**
 * \brief   A watchdog whose timer or arm event cannot be allocated reports that it cannot be
 *          armed, and the one built with every allocation available can.
 **/
TEST( FaultInjectSweepTest, watchdog_without_its_parts_cannot_be_armed )
{
    AREG_EXPECT_IN_CHILD( _watchdog_without_parts_cannot_arm( ) );
}

/**
 * \brief   Reproduces the defect: the controller copied the names of the models and threads with
 *          an allocation that aborts when it fails. Its unload path makes no such allocation.
 **/
TEST( FaultInjectSweepTest, unload_of_a_model_with_long_names_makes_no_throwing_allocation )
{
    ASSERT_TRUE( _register_long_model( ) );
    AREG_EXPECT_IN_CHILD( _unload_allocates_nothing( ) );
}

/**
 * \brief   A guard armed when the watchdog manager stops is taken out of its list, cannot be
 *          armed afterwards, and is destroyed without touching the stopped manager.
 **/
TEST( FaultInjectSweepTest, guard_armed_when_the_manager_stops_is_released )
{
    AREG_EXPECT_IN_CHILD( _guard_outlives_the_manager( ) );
}

/**
 * \brief   A hung thread that is abandoned takes its worker out of the thread registry, so the
 *          new instance of the thread can create the worker of the same name.
 **/
TEST( FaultInjectSweepTest, abandoned_thread_leaves_the_name_of_its_worker_free )
{
    ASSERT_TRUE( _register_hung_model( ) );
    AREG_EXPECT_IN_CHILD( _abandoned_thread_releases_its_worker( ) );
}

/**
 * \brief   A component thread object that cannot be allocated fails the load of its model; the
 *          next load of the same model starts the thread.
 **/
TEST( FaultInjectSweepTest, component_thread_that_cannot_be_allocated_fails_the_load )
{
    ASSERT_TRUE( _register_model( ) );
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, true, false, true, false );
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::AlignedAllocation, 0, 0u, 1u ) );
            AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) == false );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::AlignedAllocation ) == 1u );
        }
        AREG_CHILD_CHECK( areg::Application::model_state( _sweepModel ) == areg::ModelState::Unloaded );
        AREG_CHILD_CHECK( areg::Application::load_model( _sweepModel, true ) );
        areg::Application::unload_model( _sweepModel );
        areg::Application::release( );
    );
}

/**
 * \brief   Each allocation of the load of a model that outgrows the first block of the records
 *          fails in turn: the model is loaded or left unloaded, and the load ends each time.
 **/
TEST( FaultInjectSweepTest, every_failed_allocation_of_a_large_model_is_survived )
{
    ASSERT_TRUE( _register_large_model( ) );
    AREG_EXPECT_IN_CHILD( _large_load_survives( ) );
}

/**
 * \brief   Each allocation of a guarded worker fails in turn: a worker that cannot be built
 *          completely is not created, and its creation reports it.
 **/
TEST( FaultInjectSweepTest, guarded_worker_that_cannot_be_built_is_not_created )
{
    ASSERT_TRUE( _register_host_model( ) );
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, true, false, true, true );
        AREG_CHILD_CHECK( areg::Application::load_model( _hostModel, true ) );
        AREG_CHILD_CHECK( gHostDone.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( gRefusedWorkers.load( ) != 0u );
        AREG_CHILD_CHECK( areg::Application::unload_model( _hostModel ) );
        areg::Application::release( );
    );
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

namespace
{
    //!< Fails each allocation of the argument conversion in turn, then converts with none failing.
    void _refused_argument_conversion_returns_null( )
    {
        char first[]    { "app" };
        char second[]   { "--console" };
        char* argv[]    { first, second };
        for ( uint32_t skip = 0u; skip < 3u; ++ skip )
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            AREG_CHILD_CHECK( areg::ext::convert_arguments<char>( argv, 2 ) == nullptr );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u );
        }

        char** result = areg::ext::convert_arguments<char>( argv, 2 );
        AREG_CHILD_CHECK( ( result != nullptr ) && ( std::strcmp( result[1], "--console" ) == 0 ) );
        areg::ext::delete_arguments( result, 2 );

        wchar_t wideFirst[] { L"app" };
        wchar_t wideSecond[]{ L"--console" };
        wchar_t* wideArgv[] { wideFirst, wideSecond };
        for ( uint32_t skip = 0u; skip < 3u; ++ skip )
        {
            areg_fault::FaultScope faults;
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            AREG_CHILD_CHECK( areg::ext::convert_arguments<wchar_t>( wideArgv, 2 ) == nullptr );
            AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 1u );
        }

        result = areg::ext::convert_arguments<wchar_t>( wideArgv, 2 );
        AREG_CHILD_CHECK( ( result != nullptr ) && ( std::strcmp( result[0], "app" ) == 0 ) && ( std::strcmp( result[1], "--console" ) == 0 ) );
        areg::ext::delete_arguments( result, 2 );
    }
}

/**
 * \brief   Reproduces the defect: the conversion of the command line used a throwing allocation,
 *          so a failed one ended the process. Each failed allocation now returns null, for narrow
 *          and wide arguments.
 **/
TEST( FaultInjectSweepTest, argument_conversion_that_cannot_allocate_returns_null )
{
    AREG_EXPECT_IN_CHILD( _refused_argument_conversion_returns_null( ) );
}

#endif  // defined(AREG_FAULT_SWEEP_TESTS)

namespace
{
#if defined(AREG_FAULT_SWEEP_TESTS)
    //!< Publishes the address of an object so the compiler cannot elide its allocation.
    template<typename OBJECT>
    inline void _keep_allocation( const OBJECT & obj )
    {
        asm volatile( "" : : "r"( &obj ) : "memory" );
    }

    //!< Fails every nothrow allocation while a fixed array is resized and copied.
    void _fixed_array_survives_failed_allocation( )
    {
        areg_fault::FaultScope faults;
        const int32_t source[]{ 1, 2, 3 };
        const int32_t longer[]{ 9, 8, 7, 6, 5 };
        areg::FixedArray<int32_t> values( source, 3u );
        const areg::FixedArray<int32_t> other( longer, 5u );

        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, 0u, 0u ) );
        const bool resized{ values.resize( 8u ) };
        _keep_allocation( values );
        AREG_CHILD_CHECK( resized == false );
        const bool copied{ values.copy( other ) };
        _keep_allocation( values );
        AREG_CHILD_CHECK( copied == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 2u );
        AREG_CHILD_CHECK( values.size( ) == 3u && values[ 0 ] == 1 && values[ 1 ] == 2 && values[ 2 ] == 3 );

        AREG_CHILD_CHECK( values.resize( 2u ) );
        AREG_CHILD_CHECK( areg_fault::calls( areg_fault::FaultPoint::Allocation ) == 2u );
        AREG_CHILD_CHECK( values.size( ) == 2u && values[ 1 ] == 2 );

        const areg::FixedArray<int32_t> failedCopy( other );
        const areg::FixedArray<int32_t> failedList( longer, 5u );
        const areg::FixedArray<int32_t> failedCount( 4u );
        _keep_allocation( failedCopy );
        _keep_allocation( failedList );
        _keep_allocation( failedCount );
        AREG_CHILD_CHECK( failedCopy.size( ) == 0u && failedList.size( ) == 0u && failedCount.size( ) == 0u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 5u );

        ::areg_fault_disarm_all( );
        AREG_CHILD_CHECK( values.resize( 4u ) && values.size( ) == 4u && values[ 1 ] == 2 );
        AREG_CHILD_CHECK( values.copy( other ) && values == other );
    }

    //!< Fails one aligned allocation of the setup and the load, then lets the system stop; a thread without a queue cannot be stopped.
    void _setup_with_failed_aligned_allocation( uint32_t index )
    {
        ::alarm( CHILD_ALARM_S );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::AlignedAllocation, 0, index, 1u ) );
        areg::Application::setup( false, true, false, true, false );
        static_cast<void>( areg::Application::load_model( _sweepModel, true ) );
        ::areg_fault_disarm_all( );
        static_cast<void>( areg::Application::unload_model( _sweepModel ) );
        areg::Application::release( );
    }

    //!< Starts a pool server and checks that every pair of the pool runs and stops with the server.
    void _pool_server_starts_every_pair( )
    {
        constexpr uint32_t pairs{ 2u };
        areg::Application::setup( false, false, false, true, false );
        service_test::TestServer server( service_test::Policy::Accept, pairs );
        server.apply_connection_data( areg::String( "127.0.0.1" ), static_cast<uint16_t>( 21000u + (static_cast<uint32_t>( ::getpid( ) ) % 3000u) ) );

        AREG_CHILD_CHECK( server.connect_service_host( ) && server.wait_started( 1u ) );
        AREG_CHILD_CHECK( server.active_client_pair_count( ) == pairs );

        server.disconnect_service_host( );
        server.wait_to_complete( );
        AREG_CHILD_CHECK( server.active_client_pair_count( ) == 0u );
        areg::Application::release( );
    }
#endif  // defined(AREG_FAULT_SWEEP_TESTS)
}

/**
 * \brief   A pool server starts every pair of its pool and stops them with the server.
 **/
#if defined(AREG_FAULT_SWEEP_TESTS)
TEST( FaultInjectSweepTest, pool_server_starts_and_stops_every_pair )
{
    AREG_EXPECT_IN_CHILD( _pool_server_starts_every_pair( ) );
}
#endif  // defined(AREG_FAULT_SWEEP_TESTS)

#if defined(AREG_FAULT_SWEEP_TESTS)
/**
 * \brief   A fixed array whose allocation fails reports it and keeps its previous contents.
 **/
TEST( FaultInjectSweepTest, fixed_array_that_cannot_allocate_stays_unchanged )
{
    AREG_EXPECT_IN_CHILD( _fixed_array_survives_failed_allocation( ) );
}
#endif  // defined(AREG_FAULT_SWEEP_TESTS)

#if defined(AREG_FAULT_SWEEP_TESTS) && AREG_LOGGING

namespace
{
    uint32_t    gConnectCalls   { 0u };     //!< Calls of the instances connected callback.
    uint32_t    gConnectCount   { 0u };     //!< The instances of the last call of the callback.
    ITEM_ID     gConnectFirst   { 0u };     //!< The cookie of the first instance of the last call.

    void _on_instances_connected( const LogInstance * instances, uint32_t count )
    {
        ++ gConnectCalls;
        gConnectCount = count;
        gConnectFirst = count != 0u ? instances[ 0 ].liCookie : 0u;
    }

    //!< Returns the notification of the collector that two sources are connected.
    areg::MessageEnvelope _connected_sources( ITEM_ID first )
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances( ) };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK( message.init_envelope( header, 512u ) != nullptr );
        message << areg::RemoteConnectionState::Connected;
        message << 2u;
        for ( ITEM_ID cookie = first; cookie < first + 2u; ++ cookie )
        {
            areg::ConnectedInstance instance;
            instance.ciSource   = areg::MessageSource::SourceClient;
            instance.ciBitness  = areg::InstanceBitness::Bitness64;
            instance.ciCookie   = cookie;
            instance.ciTimestamp= 1u;
            instance.ciInstance = "sweep_instance";
            instance.ciLocation = "/sweep/location";
            message << instance;
        }

        message.move_to_begin( );
        return message;
    }

    //!< Fails the allocations of a notification of connected sources in turn.
    void _refused_instance_list_drops_only_the_notification( )
    {
        ObserverEvents events{ };
        events.evtInstConnected = &_on_instances_connected;
        areg::logger::LoggerClient::instance( ).set_callbacks( &events );
        areg::logger::ObserverMessageProcessor processor( areg::logger::LoggerClient::instance( ) );

        uint32_t dropped{ 0u };
        bool completed{ false };
        for ( uint32_t skip = 0u; ( skip < REQUEST_COUNT ) && ( completed == false ); ++ skip )
        {
            const ITEM_ID first{ 100u + skip * 10u };
            const areg::MessageEnvelope message{ _connected_sources( first ) };
            gConnectCalls = 0u;
            {
                areg_fault::FaultScope faults;
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
                processor.notify_connected_clients( message );
                completed = ( areg_fault::failed( areg_fault::FaultPoint::Allocation ) == 0u );
            }

            AREG_CHILD_CHECK( gConnectCalls <= 1u );
            if ( gConnectCalls == 1u )
            {
                AREG_CHILD_CHECK( gConnectCount == 2u );
                AREG_CHILD_CHECK( gConnectFirst == first );
            }
            else if ( completed == false )
            {
                ++ dropped;
            }
            else
            {
                AREG_CHILD_CHECK( gConnectCalls == 1u );
            }
        }

        AREG_CHILD_CHECK( dropped != 0u );
        AREG_CHILD_CHECK( completed );
        AREG_CHILD_CHECK( gConnectCalls == 1u && gConnectCount == 2u );
    }
}

/**
 * \brief   The list of connected sources for the callback cannot be allocated: the notification is
 *          dropped without a crash, a notification that follows is delivered whole.
 **/
TEST( FaultInjectSweepTest, instance_list_that_cannot_be_allocated_drops_only_the_notification )
{
    AREG_EXPECT_IN_CHILD( _refused_instance_list_drops_only_the_notification( ); );
}

#endif  // defined(AREG_FAULT_SWEEP_TESTS) && AREG_LOGGING

#if defined(AREG_FAULT_SWEEP_TESTS)

namespace
{
    constexpr char const    _alignedModel[]     { "FaultAlignedHostModel" };
    constexpr char const    _alignedThread[]    { "FaultAlignedHostThread" };
    constexpr char const    _alignedWorker[]    { "FaultAlignedWorker" };

    //!< Signaled when the component that creates the worker with a failing allocation is done.
    areg::SyncEvent         gAlignedDone        { true, false };

    //!< An event that carries a tag.
    struct TagEvent : public areg::Event
    {
        explicit TagEvent( uint32_t tag )
            : areg::Event( areg::EventType::EventCustomExternal, areg::EventPriority::NormalPrio )
        {
            set_event_id( tag );
        }
    };

    //!< A worker consumer of the component below.
    class AlignedConsumer final : public areg::WorkerThreadConsumer
    {
    public:
        AlignedConsumer( )
            : areg::WorkerThreadConsumer( areg::String( _alignedWorker ) )
        { }
    };

    //!< A component that creates a worker whose first aligned allocation fails.
    class AlignedHostComponent : public areg::Component
    {
    public:
        AlignedHostComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component( entry, owner )
        { }

    protected:
        void startup_component( areg::ComponentThread & comThread ) override
        {
            areg::Component::startup_component( comThread );
            const areg::String name( _alignedWorker );
            const areg::ThreadAddress address( name );
            {
                areg_fault::FaultScope faults;
                AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::AlignedAllocation, 0, 0u, 1u ) );
                AREG_CHILD_CHECK( create_worker_thread( name, mConsumer, comThread ) == nullptr );
                AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::AlignedAllocation ) == 1u );
            }

            AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == nullptr );
            areg::WorkerThread * const worker{ create_worker_thread( name, mConsumer, comThread ) };
            AREG_CHILD_CHECK( worker != nullptr );
            AREG_CHILD_CHECK( worker->is_ready( ) );
            AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == worker );
            AREG_CHILD_CHECK( create_worker_thread( name, mConsumer, comThread ) == worker );
            delete_worker_thread( name );
            AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == nullptr );
            gAlignedDone.set_signaled( );
        }

    private:
        AlignedConsumer mConsumer;
    };

    bool _register_aligned_host_model( )
    {
        static const bool _registered{ _add_single_model( _alignedModel, _alignedThread, "FaultAlignedHostRole", &_create_component<AlignedHostComponent>, 0u, areg::FailurePolicy::Exit ) };
        return _registered;
    }

    //!< A queue whose ring cannot be allocated holds no lanes and can acquire them again.
    void _queue_without_ring_acquires_it_later( )
    {
        areg::EventQueue queue( 32u );
        areg_fault::FaultScope faults;
        AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::AlignedAllocation, 0, 0u, 1u ) );
        AREG_CHILD_CHECK( queue.acquire_lanes( ) == false );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::AlignedAllocation ) == 1u );

        areg::Event event{ TagEvent( 7u ) };
        AREG_CHILD_CHECK( queue.push_event( event ) == false );
        AREG_CHILD_CHECK( event.is_valid( ) );
        AREG_CHILD_CHECK( queue.has_pending( ) == false );
        AREG_CHILD_CHECK( queue.pop_event( ).is_valid( ) == false );

        queue.exit_queue( true );
        AREG_CHILD_CHECK( queue.pop_event( ).is_exit_prio( ) );
        queue.reset_exit( );

        AREG_CHILD_CHECK( queue.acquire_lanes( ) );
        AREG_CHILD_CHECK( queue.push_event( event ) );
        AREG_CHILD_CHECK( queue.pop_event( ).event_id( ) == 7u );
        AREG_CHILD_CHECK( areg_fault::failed( areg_fault::FaultPoint::AlignedAllocation ) == 1u );
    }

    constexpr char const    _guardedModel[]     { "FaultGuardedModel" };
    constexpr char const    _guardedThread[]    { "FaultGuardedThread" };

    //!< Registers once the model of one component thread with a watchdog.
    bool _register_guarded_model( )
    {
        static const bool _registered{ [] {
            const areg::String thread( _guardedThread );
            areg::ComponentThreadEntry entry{ thread };
            entry.mWatchdogTimeout = WORKER_WATCH_MS;
            entry.add_component( areg::ComponentEntry( thread, areg::String( "FaultGuardedRole" ), &_create_sweep, &_delete_sweep ) );
            areg::Model model( _guardedModel );
            model.add_thread( entry );
            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

    //!< Fails each allocation of the point in the load of a model in turn: the load fails with no thread left, or succeeds, and the next load works.
    void _single_load_survives( areg_fault::FaultPoint point, const char * modelName, const char * threadName )
    {
        areg::Application::setup( false, true, false, true, true );
        areg_fault::FaultScope faults;
        const areg::ThreadAddress address( threadName );
        uint32_t refused{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < ATTEMPT_COUNT ) && ( finished == false ); ++ skip )
        {
            AREG_CHILD_CHECK( areg_fault::arm( point, 0, skip, 1u ) );
            const bool loaded{ areg::Application::load_model( modelName, true ) };
            const uint32_t failed{ areg_fault::failed( point ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );
            if ( loaded )
            {
                AREG_CHILD_CHECK( areg::Application::model_state( modelName ) == areg::ModelState::Loaded );
                AREG_CHILD_CHECK( areg::Application::unload_model( modelName ) );
            }
            else
            {
                AREG_CHILD_CHECK( finished == false );
                AREG_CHILD_CHECK( areg::Application::model_state( modelName ) == areg::ModelState::Unloaded );
                AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == nullptr );
                ++ refused;
            }

            AREG_CHILD_CHECK( areg::Application::load_model( modelName, true ) );
            AREG_CHILD_CHECK( areg::Application::unload_model( modelName ) );
            AREG_CHILD_CHECK( areg::Application::model_state( modelName ) == areg::ModelState::Unloaded );
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( refused != 0u );
        areg::Application::release( );
    }
}

/**
 * \brief   A queue whose ring cannot be allocated reports it, holds no lanes (refuses an event
 *          and keeps it, ends its consumer on exit) and acquires the lanes on the next call.
 **/
TEST( FaultInjectSweepTest, event_queue_that_cannot_allocate_its_ring_acquires_it_later )
{
    AREG_EXPECT_IN_CHILD( _queue_without_ring_acquires_it_later( ) );
}

/**
 * \brief   A worker thread object that cannot be allocated is not created and not registered; the
 *          next creation of the same name starts a ready worker, and deleting it frees the name.
 **/
TEST( FaultInjectSweepTest, worker_thread_that_cannot_be_allocated_is_not_created )
{
    ASSERT_TRUE( _register_aligned_host_model( ) );
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup( false, true, false, true, false );
        AREG_CHILD_CHECK( areg::Application::load_model( _alignedModel, true ) );
        AREG_CHILD_CHECK( gAlignedDone.lock( WAIT_MS ) );
        AREG_CHILD_CHECK( areg::Application::unload_model( _alignedModel ) );
        areg::Application::release( );
    );
}

/**
 * \brief   Each nothrow allocation of the load of a model fails in turn: a failed load leaves the
 *          model unloaded and no thread of it running, and the next load succeeds.
 **/
TEST( FaultInjectSweepTest, every_failed_allocation_of_a_single_thread_load_leaves_nothing_running )
{
    ASSERT_TRUE( _register_model( ) );
    AREG_EXPECT_IN_CHILD( _single_load_survives( areg_fault::FaultPoint::Allocation, _sweepModel, _sweepThread ) );
}

/**
 * \brief   Each aligned allocation of the load of a model fails in turn, with the same outcome.
 **/
TEST( FaultInjectSweepTest, every_failed_aligned_allocation_of_a_single_thread_load_leaves_nothing_running )
{
    ASSERT_TRUE( _register_model( ) );
    AREG_EXPECT_IN_CHILD( _single_load_survives( areg_fault::FaultPoint::AlignedAllocation, _sweepModel, _sweepThread ) );
}

/**
 * \brief   The same sweep for a model whose thread has a watchdog: the watchdog that cannot be built
 *          fails the load, and nothing of the thread stays running.
 **/
TEST( FaultInjectSweepTest, every_failed_allocation_of_a_guarded_thread_load_leaves_nothing_running )
{
    ASSERT_TRUE( _register_guarded_model( ) );
    AREG_EXPECT_IN_CHILD( _single_load_survives( areg_fault::FaultPoint::Allocation, _guardedModel, _guardedThread ) );
}

TEST( FaultInjectSweepTest, every_failed_aligned_allocation_of_a_guarded_thread_load_leaves_nothing_running )
{
    ASSERT_TRUE( _register_guarded_model( ) );
    AREG_EXPECT_IN_CHILD( _single_load_survives( areg_fault::FaultPoint::AlignedAllocation, _guardedModel, _guardedThread ) );
}

/**
 * \brief   A dispatcher whose ring cannot be allocated must not run without a queue: after any
 *          one failed aligned allocation the system must stop and the child must exit with 0.
 *          Reproduces the defect: such a thread reported started, and its stop never arrived.
 **/
TEST( FaultInjectSweepTest, bug_902_setup_with_a_failed_ring_allocation_must_stop )
{
    ASSERT_TRUE( _register_model( ) );
    for ( uint32_t index = 0u; index < REQUEST_COUNT; ++ index )
    {
        SCOPED_TRACE( "failed aligned allocation " + std::to_string( index ) );
        ::testing::GTEST_FLAG( death_test_style ) = "threadsafe";
        EXPECT_EXIT( {
            _setup_with_failed_aligned_allocation( index );
            ::areg_child::pass( );
        }, _controlled_exit, "" );
    }
}

namespace
{
    //!< The service, role and thread names of the addresses; longer than one block of the stream.
    constexpr char const    _dataService[]  { "FaultDataServiceNameThatIsLongerThanTheFirstBlockOfTheStream" };
    constexpr char const    _dataRole[]     { "FaultDataRoleNameThatIsLongerThanTheFirstBlockOfTheStream" };
    constexpr char const    _dataThread[]   { "FaultDataThreadNameThatIsLongerThanTheFirstBlockOfTheStream" };

    //!< Returns the bytes of the event data; empty if the data is not valid.
    std::vector<uint8_t> _data_bytes( const areg::ServiceManagerEventData & data )
    {
        std::vector<uint8_t> bytes;
        if ( data.is_valid( ) )
        {
            const areg::InStream & stream{ data.read_stream( ) };
            stream.reset( );
            uint8_t part[ 64 ];
            for ( uint32_t got = stream.read( part, sizeof( part ) ); got != 0u; got = stream.read( part, sizeof( part ) ) )
            {
                bytes.insert( bytes.end( ), part, part + got );
            }

            stream.reset( );
        }

        return bytes;
    }

    //!< Fails each allocation of the factory in turn: its data is either not valid or holds every field.
    template<typename Factory>
    void _event_data_complete_or_invalid( Factory factory )
    {
        const std::vector<uint8_t> expected{ _data_bytes( factory( ) ) };
        AREG_CHILD_CHECK( expected.empty( ) == false );
        areg_fault::FaultScope faults;
        uint32_t refused{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < REQUEST_COUNT ) && ( finished == false ); ++ skip )
        {
            AREG_CHILD_CHECK( areg_fault::arm( areg_fault::FaultPoint::Allocation, 0, skip, 1u ) );
            const areg::ServiceManagerEventData data{ factory( ) };
            const uint32_t failed{ areg_fault::failed( areg_fault::FaultPoint::Allocation ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );
            if ( data.is_valid( ) == false )
                ++ refused;
            else
                AREG_CHILD_CHECK( _data_bytes( data ) == expected );
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( refused != 0u );
    }

    //!< Sweeps the factories of the service manager data that write more than one field.
    void _service_manager_data_complete_or_invalid( )
    {
        const areg::ServiceItem service( areg::String( _dataService ), areg::Version( 1u, 2u, 3u ), areg::ServiceType::Public );
        areg::StubAddress stub( service, areg::String( _dataRole ), areg::String( _dataThread ) );
        stub.set_cookie( areg::COOKIE_REMOTE_SERVICE + 920u );
        areg::ProxyAddress proxy( service, areg::String( _dataRole ), areg::String( _dataThread ) );
        proxy.set_cookie( areg::COOKIE_REMOTE_SERVICE + 921u );
        const areg::Channel channel( 0x9201u, 0x9202u, 0x9203u );
        constexpr areg::DisconnectReason reason{ areg::DisconnectReason::ProviderDisconnected };

        _event_data_complete_or_invalid( [&stub]( ) { return areg::ServiceManagerEventData::register_stub( stub ); } );
        _event_data_complete_or_invalid( [&stub]( ) { return areg::ServiceManagerEventData::unregister_stub( stub, reason ); } );
        _event_data_complete_or_invalid( [&proxy]( ) { return areg::ServiceManagerEventData::register_proxy( proxy ); } );
        _event_data_complete_or_invalid( [&proxy]( ) { return areg::ServiceManagerEventData::unregister_proxy( proxy, reason ); } );
        _event_data_complete_or_invalid( [&channel]( ) { return areg::ServiceManagerEventData::register_connection( channel ); } );
        _event_data_complete_or_invalid( [&channel]( ) { return areg::ServiceManagerEventData::unregister_connection( channel ); } );
        _event_data_complete_or_invalid( [&channel]( ) { return areg::ServiceManagerEventData::lost_connection( channel ); } );
        _event_data_complete_or_invalid( []( ) { return areg::ServiceManagerEventData::start_connection( areg::RemoteServiceKind::Router, 1u ); } );
        _event_data_complete_or_invalid( []( ) { return areg::ServiceManagerEventData::model_request( true, 7u, 9u ); } );
        _event_data_complete_or_invalid( []( ) { return areg::ServiceManagerEventData::watchdog_expired( 11u, true ); } );
    }
}

/**
 * \brief   Reproduces the defect: a nothrow allocation that failed after the first fields of an
 *          address were written was not noticed, and the half written data was posted as valid.
 *          Expected: the data of each factory is either not valid or holds every field.
 **/
TEST( FaultInjectSweepTest, bug_920_service_manager_data_whose_allocation_fails_is_complete_or_not_valid )
{
    AREG_EXPECT_IN_CHILD( _service_manager_data_complete_or_invalid( ) );
}

#if defined(AREG_FAULT_HELLO)

#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"
#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"

namespace
{
    constexpr char const    _serviceModel[]     { "FaultServiceModel" };
    constexpr char const    _serviceThread[]    { "FaultServiceThread" };
    constexpr char const    _serviceRole[]      { "FaultHelloRole" };
    //!< The most allocations of one lifecycle with services that are failed in turn.
    constexpr uint32_t      SERVICE_ATTEMPTS    { 2'000u };

    //!< Signaled when the consumer got a response.
    areg::SyncEvent         gResponse           { true, false };

    //!< A local provider of the hello service; it answers every request.
    class SweepProvider final : public    areg::Component
                              , protected HelloServiceProviderBase
    {
    public:
        SweepProvider( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
        { }

    protected:
        void request_hello_service( ) override
        {
            response_hello_service( );
        }
    };

    //!< A consumer of the local provider; it sends a request when connected and signals the response.
    class SweepConsumer final : public    areg::Component
                              , protected HelloServiceConsumerBase
    {
    public:
        SweepConsumer( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : areg::Component           ( entry, owner )
            , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
        { }

    protected:
        bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) override
        {
            const bool result{ HelloServiceConsumerBase::service_connected( status, proxy ) };
            if ( areg::is_service_connected( status ) )
                request_hello_service( );

            return result;
        }

        void response_hello_service( ) override
        {
            gResponse.set_signaled( );
        }
    };

    //!< Registers once the model of one thread with a provider and its consumer.
    bool _register_service_model( )
    {
        static const bool _registered{ [] {
            const areg::String thread( _serviceThread );
            areg::ComponentThreadEntry entry{ thread };
            areg::ComponentEntry provider( thread, areg::String( _serviceRole ), &_create_component<SweepProvider>, &_delete_sweep );
            provider.add_supported_service( areg::ServiceEntry( areg::String( HelloService::ServiceName ), HelloService::InterfaceVersion ) );
            areg::ComponentEntry consumer( thread, areg::String( "FaultHelloConsumer" ), &_create_component<SweepConsumer>, &_delete_sweep );
            consumer.add_dependency_service( areg::DependencyEntry( areg::String( _serviceRole ) ) );
            entry.add_component( provider );
            entry.add_component( consumer );
            areg::Model model( _serviceModel );
            model.add_thread( entry );
            return areg::ComponentLoader::add_model_unique( model );
        }() };
        return _registered;
    }

    //!< Loads the model, waits until the consumer got a response from the provider, and unloads it.
    bool _service_round( )
    {
        gResponse.reset( );
        const bool loaded{ areg::Application::load_model( _serviceModel, true ) };
        const bool served{ loaded && gResponse.lock( WAIT_MS ) };
        return areg::Application::unload_model( _serviceModel ) && served
            && ( areg::Application::model_state( _serviceModel ) == areg::ModelState::Unloaded );
    }

    //!< Fails each allocation of the point in a load and unload of the model in turn: a failed load leaves no thread, and the next round is served.
    void _service_lifecycle_survives( areg_fault::FaultPoint point )
    {
        areg::Application::setup( false, true, false, true, false );
        areg_fault::FaultScope faults;
        const areg::ThreadAddress address( _serviceThread );
        uint32_t refused{ 0u };
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < SERVICE_ATTEMPTS ) && ( finished == false ); ++ skip )
        {
            AREG_CHILD_CHECK( areg_fault::arm( point, 0, skip, 1u ) );
            const bool loaded{ areg::Application::load_model( _serviceModel, true ) };
            if ( loaded == false )
                ++ refused;
            const bool unloaded{ areg::Application::unload_model( _serviceModel ) };
            const uint32_t failed{ areg_fault::failed( point ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );
            if ( unloaded == false )
            {
                // An unload request that cannot be built changes nothing.
                AREG_CHILD_CHECK( areg::Application::model_state( _serviceModel ) == areg::ModelState::Loaded );
                AREG_CHILD_CHECK( areg::Application::unload_model( _serviceModel ) );
            }

            AREG_CHILD_CHECK( areg::Application::model_state( _serviceModel ) == areg::ModelState::Unloaded );
            AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == nullptr );
            AREG_CHILD_CHECK( _service_round( ) );
        }

        AREG_CHILD_CHECK( finished );
        AREG_CHILD_CHECK( refused != 0u );
        areg::Application::release( );
    }
}

/**
 * \brief   Reproduces the defects: each nothrow allocation of the load and unload of a model with
 *          a local provider and its consumer fails in turn (component thread, components, stub,
 *          proxy, the records and the events of the service manager). A proxy without its state
 *          tables was used, and a service manager request without its data registered an empty
 *          address. Expected: a failed load leaves no thread, and the next load is served.
 **/
TEST( FaultInjectSweepTest, bug_917_every_failed_allocation_of_a_service_model_lifecycle_leaves_the_next_round_served )
{
    ASSERT_TRUE( _register_service_model( ) );
    AREG_EXPECT_IN_CHILD( _service_lifecycle_survives( areg_fault::FaultPoint::Allocation ) );
}

namespace
{
    //!< Fails each allocation of the point in the unload of a served model in turn: the registry holds no stale entry, and the next round is served.
    void _service_unload_survives( areg_fault::FaultPoint point )
    {
        areg::Application::setup( false, true, false, true, false );
        areg_fault::FaultScope faults;
        const areg::ThreadAddress address( _serviceThread );
        bool finished{ false };
        for ( uint32_t skip = 0u; ( skip < SERVICE_ATTEMPTS ) && ( finished == false ); ++ skip )
        {
            gResponse.reset( );
            AREG_CHILD_CHECK( areg::Application::load_model( _serviceModel, true ) );
            AREG_CHILD_CHECK( gResponse.lock( WAIT_MS ) );

            AREG_CHILD_CHECK( areg_fault::arm( point, 0, skip, 1u ) );
            const bool unloaded{ areg::Application::unload_model( _serviceModel ) };
            const uint32_t failed{ areg_fault::failed( point ) };
            ::areg_fault_disarm_all( );
            finished = ( failed == 0u );
            if ( unloaded == false )
                AREG_CHILD_CHECK( areg::Application::unload_model( _serviceModel ) );

            AREG_CHILD_CHECK( areg::Application::model_state( _serviceModel ) == areg::ModelState::Unloaded );
            AREG_CHILD_CHECK( areg::Thread::find_by_address( address ) == nullptr );
            AREG_CHILD_CHECK( _service_round( ) );
        }

        AREG_CHILD_CHECK( finished );
        areg::Application::release( );
    }
}

/**
 * \brief   Each nothrow allocation of the unload of a served model with a local provider and its
 *          consumer fails in turn: the model is unloaded, and the next load connects and serves
 *          the consumer.
 **/
TEST( FaultInjectSweepTest, every_failed_allocation_of_a_served_model_unload_leaves_the_next_round_served )
{
    ASSERT_TRUE( _register_service_model( ) );
    AREG_EXPECT_IN_CHILD( _service_unload_survives( areg_fault::FaultPoint::Allocation ) );
}

TEST( FaultInjectSweepTest, every_failed_aligned_allocation_of_a_service_model_lifecycle_leaves_the_next_round_served )
{
    ASSERT_TRUE( _register_service_model( ) );
    AREG_EXPECT_IN_CHILD( _service_lifecycle_survives( areg_fault::FaultPoint::AlignedAllocation ) );
}

#endif  // defined(AREG_FAULT_HELLO)

#endif  // defined(AREG_FAULT_SWEEP_TESTS)
