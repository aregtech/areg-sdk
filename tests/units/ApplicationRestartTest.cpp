/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ApplicationRestartTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. Several application sessions in one process:
 *              after every release the next setup starts as from scratch.
 ************************************************************************/

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Model.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/logging/areg_log.h"
#include "areg/persist/ConfigManager.hpp"

// Last of the framework headers: it brings the system socket headers.
#include "units/ApplicationRestartTestSupport.hpp"

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <thread>

namespace
{
    constexpr uint32_t REQUEST     { areg::REQUEST_ID_FIRST };
    constexpr uint32_t RESPONSE    { areg::RESPONSE_ID_FIRST };
    constexpr uint32_t ATTRIBUTE   { areg::ATTRIBUTE_ID_FIRST };
    constexpr uint32_t REQUESTS[]  { REQUEST };
    constexpr uint32_t RESPONSES[] { RESPONSE };
    constexpr uint32_t ATTRIBUTES[]{ ATTRIBUTE };
    constexpr uint32_t PARAMS[]    { 1u };

    const areg::InterfaceData RESTART_INTERFACE
    {
          "ApplicationRestartService", areg::Version( 1, 0, 0 ), areg::ServiceType::Local
        , 1, 1, 1, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSES, PARAMS
    };

    constexpr char MODEL_ONE[]      { "ApplicationRestartModelOne" };
    constexpr char MODEL_TWO[]      { "ApplicationRestartModelTwo" };
    constexpr char PROVIDER_THREAD[]{ "ApplicationRestartProviderThread" };
    constexpr char CONSUMER_THREAD[]{ "ApplicationRestartConsumerThread" };
    constexpr char PROVIDER_ROLE[]  { "ApplicationRestartProvider" };
    constexpr char CONSUMER_ROLE[]  { "ApplicationRestartConsumer" };

    constexpr uint32_t REQUEST_VALUE{ 1u };
    constexpr uint32_t ONE_HOUR_MS{ 3'600'000u };

    //!< What a session observed.
    enum class Seen : uint32_t
    {
          Connected
        , Response
        , Attribute
        , Tick
        , Built
        , Deleted
    };

    constexpr uint32_t SEEN_KINDS{ static_cast<uint32_t>(Seen::Deleted) + 1u };

    //!< The observations of the sessions, waited on by the main thread.
    class SessionLog
    {
    public:
        void record( Seen kind, uint32_t value = 0u )
        {
            {
                std::lock_guard<std::mutex> guard( mLock );
                mRows[static_cast<uint32_t>(kind)].push_back( value );
            }

            mChanged.notify_all( );
        }

        bool wait_for( Seen kind, std::size_t count )
        {
            std::unique_lock<std::mutex> guard( mLock );
            return mChanged.wait_for( guard, std::chrono::milliseconds( restart_test::WAIT_MS ), [&]( )
                {
                    return (mRows[static_cast<uint32_t>(kind)].size( ) >= count);
                } );
        }

        std::vector<uint32_t> rows( Seen kind )
        {
            std::lock_guard<std::mutex> guard( mLock );
            return mRows[static_cast<uint32_t>(kind)];
        }

        std::size_t count( Seen kind )
        {
            return rows( kind ).size( );
        }

        std::size_t total( )
        {
            std::lock_guard<std::mutex> guard( mLock );
            std::size_t result{ 0u };
            for ( const auto & rows : mRows )
            {
                result += rows.size( );
            }

            return result;
        }

        void clear( )
        {
            std::lock_guard<std::mutex> guard( mLock );
            for ( auto & rows : mRows )
            {
                rows.clear( );
            }
        }

    private:
        std::mutex              mLock;
        std::condition_variable mChanged;
        std::array<std::vector<uint32_t>, SEEN_KINDS> mRows;
    };

    //!< The data the components of a model read when they are created.
    struct SessionData
    {
        std::atomic<uint32_t>   tag;
        SessionLog *            log;
    };

    inline SessionData & session_data( const areg::ComponentEntry & entry )
    {
        return *std::any_cast<SessionData *>(entry.data( ));
    }
}

class ApplicationRestartRequest final : public areg::LocalRequestEvent
{
    AREG_DECLARE_RUNTIME_EVENT( ApplicationRestartRequest )
public:
    ApplicationRestartRequest( const areg::ProxyAddress & source, const areg::StubAddress & target, uint32_t request, uint32_t reserve )
        : LocalRequestEvent( source, target, request, reserve )
    {
        header( )->eventId = CLASS_ID;
    }

    ApplicationRestartRequest( const areg::SharedBuffer & data, const areg::ProxyAddress & source, const areg::StubAddress & target, uint32_t request )
        : LocalRequestEvent( data, source, target, request )
    {
        header( )->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT( ApplicationRestartRequest, areg::LocalRequestEvent )

class ApplicationRestartNotify final : public areg::LocalNotifyRequestEvent
{
    AREG_DECLARE_RUNTIME_EVENT( ApplicationRestartNotify )
public:
    ApplicationRestartNotify( const areg::ProxyAddress & source, const areg::StubAddress & target, uint32_t message, areg::RequestType type )
        : LocalNotifyRequestEvent( source, target, message, type )
    {
        header( )->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT( ApplicationRestartNotify, areg::LocalNotifyRequestEvent )

class ApplicationRestartResponse final : public areg::LocalResponseEvent
{
    AREG_DECLARE_RUNTIME_EVENT( ApplicationRestartResponse )
public:
    ApplicationRestartResponse( const areg::ProxyAddress & target, uint32_t message, areg::ResultType result, uint32_t reserve = 0u )
        : LocalResponseEvent( target, result, message, areg::SEQUENCE_NUMBER_NOTIFY, reserve )
    {
        header( )->eventId = CLASS_ID;
    }

    ApplicationRestartResponse( const areg::SharedBuffer & data, const areg::ProxyAddress & target, uint32_t message, areg::ResultType result )
        : LocalResponseEvent( data, target, result, message )
    {
        header( )->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT( ApplicationRestartResponse, areg::LocalResponseEvent )

namespace
{
    class RestartProxy final : public areg::ProxyBase
    {
    public:
        RestartProxy( const areg::String & role, areg::DispatcherThread * owner )
            : ProxyBase         ( role, RESTART_INTERFACE, owner )
            , mResponseValue    ( 0u )
            , mAttributeValue   ( 0u )
        {
        }

        static areg::ProxyBase * create( const areg::String & role, areg::DispatcherThread * owner )
        {
            return new RestartProxy( role, owner );
        }

        void request( uint32_t value, areg::NotificationConsumer & listener )
        {
            auto event = create_request_event( REQUEST, sizeof( value ) );
            event.write_stream( ) << value;
            send_request_event( event, &listener );
        }

        uint32_t response_value( ) const
        {
            return mResponseValue;
        }

        uint32_t attribute_value( ) const
        {
            return mAttributeValue;
        }

    protected:
        void process_response_event( areg::ServiceResponseEvent & event ) override
        {
            const areg::ResultType result{ static_cast<areg::ResultType>(event.envelope( ).result( )) };
            uint32_t message{ event.response_id( ) };
            if ( message == REQUEST )
            {
                message = RESPONSE;
            }

            const bool isOk{ (result == areg::ResultType::DataOK) || (result == areg::ResultType::RequestOK) };
            if ( isOk )
            {
                event.read_stream( ) >> (message == ATTRIBUTE ? mAttributeValue : mResponseValue);
            }

            set_state( message, isOk ? areg::DataState::DataIsOK : areg::DataState::DataIsInvalid );
            notify_listeners( message, result, event.sequence_number( ) );
        }

        void process_custom_event( areg::Event & ) override
        {
        }

        void register_service_listeners( ) override
        {
            ProxyBase::register_service_listeners( );
            ApplicationRestartResponse::add_listener( *this, proxy_dispatcher_thread( ) );
        }

        void unregister_service_listeners( ) noexcept override
        {
            ApplicationRestartResponse::remove_listener( *this, proxy_dispatcher_thread( ) );
            ProxyBase::unregister_service_listeners( );
        }

        ServiceAvailableEvent create_service_available( areg::NotificationConsumer & listener ) override
        {
            return ServiceAvailableEvent( listener );
        }

        areg::NotificationEvent create_client_notification( const areg::NotificationEventData & data ) const override
        {
            return areg::NotificationEvent( data );
        }

        areg::ServiceRequestEvent create_request( const areg::SharedBuffer & data, uint32_t request ) override
        {
            return ApplicationRestartRequest( data, proxy_address( ), stub_address( ), request );
        }

        areg::ServiceRequestEvent create_request( uint32_t request, uint32_t reserve ) override
        {
            return ApplicationRestartRequest( proxy_address( ), stub_address( ), request, reserve );
        }

        areg::ServiceRequestEvent create_notification_request( uint32_t message, areg::RequestType type ) override
        {
            return ApplicationRestartNotify( proxy_address( ), stub_address( ), message, type );
        }

        areg::ServiceResponseEvent create_request_failed( const areg::ProxyAddress & target, uint32_t message, areg::ResultType reason
                                                        , const SequenceNumber & sequence ) const override
        {
            ApplicationRestartResponse result( target, message, reason );
            result.set_sequence_number( sequence );
            return result;
        }

    private:
        uint32_t mResponseValue;
        uint32_t mAttributeValue;
    };

    //!< A provider that answers with its session tag, ticks once and keeps a timer armed.
    class RestartProvider final : public areg::Component, public areg::StubBase, public areg::TimerConsumer
    {
    public:
        RestartProvider( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : Component     ( entry, owner )
            , StubBase      ( static_cast<areg::Component &>(*this), RESTART_INTERFACE )
            , TimerConsumer ( )
            , mData         ( session_data( entry ) )
            , mTag          ( mData.tag.load( ) )
            , mTick         ( *this, "ApplicationRestartTick" )
            , mArmed        ( *this, "ApplicationRestartArmed" )
        {
            mData.log->record( Seen::Built, mTag );
        }

        ~RestartProvider( ) override
        {
            mData.log->record( Seen::Deleted, mTag );
        }

        void startup_service_interface( areg::Component & holder ) override
        {
            StubBase::startup_service_interface( holder );
            ApplicationRestartRequest::add_listener( static_cast<areg::StubBase &>(*this), holder.master_thread( ) );
            ApplicationRestartNotify::add_listener( static_cast<areg::StubBase &>(*this), holder.master_thread( ) );
            static_cast<void>(mTick.start_timer( 1u, holder.master_thread( ), areg::TimerBase::ONE_TIME ));
            static_cast<void>(mArmed.start_timer( ONE_HOUR_MS, holder.master_thread( ), areg::TimerBase::ONE_TIME ));
        }

        void shutdown_service_interface( areg::Component & holder ) noexcept override
        {
            mTick.stop_timer( );
            mArmed.stop_timer( );
            ApplicationRestartRequest::remove_listener( static_cast<areg::StubBase &>(*this), holder.master_thread( ) );
            ApplicationRestartNotify::remove_listener( static_cast<areg::StubBase &>(*this), holder.master_thread( ) );
            StubBase::shutdown_service_interface( holder );
        }

        void send_notification( uint32_t message ) override
        {
            areg::SharedBuffer data;
            data << mTag;
            send_update_event( message, data, areg::ResultType::DataOK );
        }

        void error_request( uint32_t, bool ) override
        {
        }

    protected:
        areg::ServiceResponseEvent create_response( const areg::ProxyAddress & target, uint32_t message, areg::ResultType result
                                                  , const areg::SharedBuffer & data ) const override
        {
            return ApplicationRestartResponse( data, target, message, result );
        }

        areg::ServiceResponseEvent create_response_event( const areg::ProxyAddress & target, uint32_t message
                                                        , areg::ResultType result, uint32_t reserve ) const override
        {
            return ApplicationRestartResponse( target, message, result, reserve );
        }

        void process_request_event( areg::ServiceRequestEvent & event ) override
        {
            uint32_t value{ 0u };
            event.read_stream( ) >> value;
            Listener listener( event.request_id( ), event.sequence_number( ), event.event_source( ) );
            if ( can_execute_request( listener, RESPONSE, event.sequence_number( ) ) )
            {
                StubListenerList listeners;
                auto response = prepare_response_event( RESPONSE, areg::ResultType::RequestOK, sizeof( value ), listeners );
                if ( response.is_valid( ) )
                {
                    response.write_stream( ) << (value * 10u + mTag);
                    send_response_notification( listeners, response );
                }
            }
        }

        void process_attribute_event( areg::ServiceRequestEvent & event ) override
        {
            const uint32_t message{ event.request_id( ) };
            const areg::ProxyAddress source{ event.event_source( ) };
            if ( event.request_type( ) == areg::RequestType::StartNotify )
            {
                if ( add_notification_listener( message, source ) && (message == ATTRIBUTE) )
                {
                    send_notification( message );
                }
            }
            else if ( event.request_type( ) == areg::RequestType::StopNotify )
            {
                remove_notification_listener( message, source );
            }
            else if ( event.request_type( ) == areg::RequestType::RemoveAllNotify )
            {
                clear_all_listeners( source );
            }
        }

        void process_timer( areg::Timer & timer ) override
        {
            if ( &timer == &mTick )
            {
                mData.log->record( Seen::Tick, mTag );
            }
        }

    private:
        SessionData &   mData;
        const uint32_t  mTag;
        areg::Timer     mTick;
        areg::Timer     mArmed;
    };

    //!< A consumer that requests and subscribes as soon as the service connects.
    class RestartConsumer final : public areg::Component, public areg::ProxyListener
    {
    public:
        RestartConsumer( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
            : Component ( entry, owner )
            , mData     ( session_data( entry ) )
            , mTag      ( mData.tag.load( ) )
            , mProxy    ( areg::ProxyBase::acquire_proxy( areg::String( PROVIDER_ROLE ), RESTART_INTERFACE, *this, RestartProxy::create, owner ) )
        {
            mData.log->record( Seen::Built, mTag );
        }

        ~RestartConsumer( ) override
        {
            mProxy->free_proxy( *this );
            mData.log->record( Seen::Deleted, mTag );
        }

        bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & ) override
        {
            if ( areg::is_service_connected( status ) )
            {
                mData.log->record( Seen::Connected, mTag );
                proxy( ).request( REQUEST_VALUE, *this );
                proxy( ).set_notification( ATTRIBUTE, this );
            }

            return true;
        }

        void process_notification_event( areg::NotificationEvent & event ) override
        {
            const areg::NotificationEventData & data = std::as_const( event ).data( );
            process_notification( data.notify_id( ), data.notify_type( ), data.sequence( ) );
        }

        void process_notification( uint32_t message, areg::ResultType result, SequenceNumber ) override
        {
            if ( (message == ATTRIBUTE) && (result == areg::ResultType::DataOK) )
            {
                mData.log->record( Seen::Attribute, proxy( ).attribute_value( ) );
            }
            else if ( (message == RESPONSE) && (result == areg::ResultType::RequestOK) )
            {
                mData.log->record( Seen::Response, proxy( ).response_value( ) );
            }
        }

    private:
        RestartProxy & proxy( )
        {
            return *static_cast<RestartProxy *>(mProxy.get( ));
        }

        SessionData &                       mData;
        const uint32_t                      mTag;
        std::shared_ptr<areg::ProxyBase>    mProxy;
    };

    template <typename ComponentType>
    areg::ComponentThreadEntry restart_thread( const char * thread, const char * role, SessionData & data )
    {
        areg::ComponentEntry entry( areg::String( thread ), areg::String( role )
            , [](const areg::ComponentEntry & item, areg::ComponentThread & owner) -> areg::Component * { return new ComponentType( item, owner ); }
            , [](areg::Component & component, const areg::ComponentEntry &) { delete &component; } );
        entry.set_data( &data );
        areg::ComponentThreadEntry result{ areg::String( thread ) };
        result.mMaxQueue = 512;
        result.mDropOnFull = areg::Bool::False;
        result.add_component( entry );
        return result;
    }

    //!< Registers a model that holds one provider and one consumer of the service.
    void add_restart_model( const char * name, SessionData & data )
    {
        areg::Model model{ areg::String( name ) };
        model.add_thread( restart_thread<RestartProvider>( PROVIDER_THREAD, PROVIDER_ROLE, data ) );
        model.add_thread( restart_thread<RestartConsumer>( CONSUMER_THREAD, CONSUMER_ROLE, data ) );
        AREG_CHILD_CHECK( areg::ComponentLoader::add_model_unique( model ) );
    }

    //!< Loads the model and checks the calls of one complete session.
    void exchange( const char * model, SessionData & data )
    {
        const uint32_t tag{ data.tag.load( ) };
        SessionLog & log{ *data.log };

        AREG_CHILD_CHECK( areg::Application::load_model( model, true ) );
        AREG_CHILD_CHECK( areg::Application::is_model_loaded( model ) );
        AREG_CHILD_CHECK( log.wait_for( Seen::Connected, 1u ) );
        AREG_CHILD_CHECK( log.wait_for( Seen::Response, 1u ) );
        AREG_CHILD_CHECK( log.wait_for( Seen::Attribute, 1u ) );
        AREG_CHILD_CHECK( log.wait_for( Seen::Tick, 1u ) );
        for ( const uint32_t value : log.rows( Seen::Response ) )
        {
            AREG_CHILD_CHECK( value == (REQUEST_VALUE * 10u + tag) );
        }

        for ( const Seen kind : { Seen::Attribute, Seen::Tick, Seen::Connected } )
        {
            for ( const uint32_t value : log.rows( kind ) )
            {
                AREG_CHILD_CHECK( value == tag );
            }
        }
    }

    //!< Ends the child if a component of the service still exists.
    void check_no_component_left( )
    {
        AREG_CHILD_CHECK( areg::Component::find_by_name( areg::String( PROVIDER_ROLE ) ) == nullptr );
        AREG_CHILD_CHECK( areg::Component::find_by_name( areg::String( CONSUMER_ROLE ) ) == nullptr );
        AREG_CHILD_CHECK( restart_test::thread_exists( PROVIDER_THREAD ) == false );
        AREG_CHILD_CHECK( restart_test::thread_exists( CONSUMER_THREAD ) == false );
        AREG_CHILD_CHECK( areg::Application::thread_state( PROVIDER_THREAD ) == areg::ThreadState::Unknown );
        AREG_CHILD_CHECK( areg::Application::thread_state( CONSUMER_THREAD ) == areg::ThreadState::Unknown );
        const areg::StubAddress stub( RESTART_INTERFACE, areg::String( PROVIDER_ROLE ), areg::String( PROVIDER_THREAD ) );
        AREG_CHILD_CHECK( areg::StubBase::registered_address( stub ).is_valid( ) == false );
        const areg::ProxyAddress proxy( RESTART_INTERFACE, areg::String( PROVIDER_ROLE ), areg::String( CONSUMER_THREAD ) );
        AREG_CHILD_CHECK( areg::ProxyBase::find_proxy( proxy ) == nullptr );
    }

    //!< Starts a session with the service manager, the timers and the watchdog.
    void start_service_session( const std::string & config, bool logging = false, bool routing = false )
    {
        areg::Application::setup( logging, true, routing, true, true, config.c_str( ) );
        AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Ready );
    }

    //!< A session flag set: logging, servicing, routing, timer, watchdog.
    void setup_flags( uint32_t mask, const std::string & config )
    {
        areg::Application::setup( (mask & 1u) != 0u, (mask & 2u) != 0u, (mask & 4u) != 0u, (mask & 8u) != 0u, (mask & 16u) != 0u, config.c_str( ) );
    }

    DEF_LOG_SCOPE( areg_unit_tests_ApplicationRestart, session_rows );

    //!< Writes one row in the log.
    void log_row( uint32_t session )
    {
        LOG_SCOPE( areg_unit_tests_ApplicationRestart, session_rows );
        LOG_DBG( "restart-row-%u", session );
        static_cast<void>(session);
    }

    //!< The name of the scope that writes the rows.
    constexpr char ROW_SCOPE[]  { "areg_unit_tests_ApplicationRestart.session_rows" };

    //!< The configuration line that enables the scope that writes the rows.
    constexpr char ROW_SCOPE_ON[]{ "log::*::scope::areg_unit_tests_ApplicationRestart.session_rows = DEBUG\n" };
}

//!< Defines a test whose body runs in a child process.
#define RESTART_TEST( suite, name )                                     \
    static void suite##_##name##_body( );                               \
    TEST( suite, name )                                                 \
    {                                                                   \
        AREG_EXPECT_IN_CHILD( suite##_##name##_body( ); );              \
    }                                                                   \
    static void suite##_##name##_body( )

/************************************************************************
 * Smoke: the sessions themselves.
 ************************************************************************/

/**
 * \brief   Every combination of the setup flags starts again after a release, and each release
 *          leaves the process without a framework thread.
 **/
RESTART_TEST( ApplicationRestartSmokeTest, every_service_combination_starts_again_after_release )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    for ( uint32_t mask = 0u; mask < 32u; ++mask )
    {
        for ( uint32_t session = 0u; session < 2u; ++session )
        {
            restart_test::check_system_is_stopped( );
            setup_flags( mask, config );
            AREG_CHILD_CHECK( areg::Application::system_state( ) == areg::AppState::Ready );
            areg::Application::release( );
            restart_test::check_system_is_stopped( );
        }
    }
}

/**
 * \brief   The quit signal of a session does not end the next one.
 **/
RESTART_TEST( ApplicationRestartSmokeTest, quit_signal_of_a_session_does_not_reach_the_next )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    areg::Application::setup( false, false, false, true, false, config.c_str( ) );
    areg::Application::signal_quit( );
    AREG_CHILD_CHECK( areg::Application::wait_quit( 0u ) );
    areg::Application::release( );

    areg::Application::setup( false, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::Application::wait_quit( 0u ) == false );
    areg::Application::release( );
}

/**
 * \brief   The user state of a session is 0 in the next session.
 **/
RESTART_TEST( ApplicationRestartSmokeTest, user_state_of_a_session_is_not_seen_by_the_next )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    restart_test::check_system_is_stopped( );
    AREG_CHILD_CHECK( areg::Application::user_state( ) == 0u );
    start_service_session( config );
    AREG_CHILD_CHECK( areg::Application::set_user_state( 7u ) );
    AREG_CHILD_CHECK( areg::Application::user_state( ) == 0u || areg::Application::user_state( ) == 7u );
    areg::Application::release( );
    AREG_CHILD_CHECK( areg::Application::user_state( ) == 0u );
}

/************************************************************************
 * Regression: models, services and timers across sessions.
 ************************************************************************/

/**
 * \brief   A model left loaded at the release is unloaded and works when it is loaded again.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, model_left_loaded_works_again_in_the_next_session )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    SessionLog log;
    SessionData data{ { 1u }, &log };
    add_restart_model( MODEL_ONE, data );

    for ( uint32_t session = 1u; session <= 3u; ++session )
    {
        log.clear( );
        data.tag.store( session );
        start_service_session( config );
        exchange( MODEL_ONE, data );
        areg::Application::release( );

        restart_test::check_system_is_stopped( );
        check_no_component_left( );
        AREG_CHILD_CHECK( areg::Application::model_state( MODEL_ONE ) == areg::ModelState::Unloaded );
        AREG_CHILD_CHECK( log.count( Seen::Built ) == 2u );
        AREG_CHILD_CHECK( log.count( Seen::Deleted ) == 2u );
    }

    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
}

/**
 * \brief   The next session runs another model on the same thread and role names.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, next_session_runs_a_different_model )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    SessionLog first;
    SessionData firstData{ { 1u }, &first };
    add_restart_model( MODEL_ONE, firstData );
    start_service_session( config );
    exchange( MODEL_ONE, firstData );
    areg::Application::release( );
    check_no_component_left( );
    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );

    SessionLog second;
    SessionData secondData{ { 2u }, &second };
    add_restart_model( MODEL_TWO, secondData );
    start_service_session( config );
    exchange( MODEL_TWO, secondData );
    areg::Application::release( );
    check_no_component_left( );
    areg::ComponentLoader::remove_component_model( areg::String( MODEL_TWO ) );
}

/**
 * \brief   Nothing a session left behind arrives in the next one.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, nothing_of_a_session_arrives_in_the_next )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    SessionLog first;
    SessionData firstData{ { 1u }, &first };
    add_restart_model( MODEL_ONE, firstData );
    start_service_session( config );
    exchange( MODEL_ONE, firstData );
    areg::Application::release( );
    const std::size_t firstRows{ first.total( ) };

    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
    SessionLog second;
    SessionData secondData{ { 2u }, &second };
    add_restart_model( MODEL_ONE, secondData );
    start_service_session( config );
    exchange( MODEL_ONE, secondData );
    areg::Application::release( );

    AREG_CHILD_CHECK( first.total( ) == firstRows );
    AREG_CHILD_CHECK( second.count( Seen::Connected ) == 1u );
    AREG_CHILD_CHECK( second.count( Seen::Response ) == 1u );
    AREG_CHILD_CHECK( second.count( Seen::Tick ) == 1u );
}

namespace
{
    //!< A dispatcher thread that counts its timer expiries.
    class TimerOwner final : public areg::DispatcherThread, public areg::TimerConsumer
    {
    public:
        TimerOwner( )
            : DispatcherThread  ( "ApplicationRestartTimerOwner", areg::DEFAULT_STACK_SIZE, 16u )
            , TimerConsumer     ( )
            , mTimer            ( *this, "ApplicationRestartOwnedTimer" )
            , mExpired          ( true, true )
            , mCalls            ( 0u )
        {
        }

        bool run_thread( )
        {
            return start( restart_test::WAIT_MS ) && wait_start( restart_test::WAIT_MS );
        }

        void stop_thread( )
        {
            mTimer.stop_timer( );
            shutdown( areg::WAIT_INFINITE );
        }

        bool post_event( areg::Event & event ) final
        {
            return areg::EventDispatcher::post_event( event );
        }

        areg::Timer             mTimer;
        areg::SyncEvent         mExpired;
        std::atomic<uint32_t>   mCalls;

    private:
        void process_timer( areg::Timer & ) final
        {
            mCalls.fetch_add( 1u );
            mExpired.set_signaled( );
        }
    };
}

/**
 * \brief   A timer armed when the session ended does not fire in the next session and can be
 *          armed again there.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, timer_armed_at_release_is_gone_and_can_be_armed_again )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    TimerOwner owner;
    AREG_CHILD_CHECK( owner.run_thread( ) );

    areg::Application::setup( false, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( owner.mTimer.start_timer( ONE_HOUR_MS, owner, areg::TimerBase::ONE_TIME ) );
    areg::Application::release( );

    areg::Application::setup( false, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( owner.mCalls.load( ) == 0u );
    AREG_CHILD_CHECK( owner.mTimer.start_timer( 1u, owner, areg::TimerBase::ONE_TIME ) );
    AREG_CHILD_CHECK( owner.mExpired.lock( restart_test::WAIT_MS ) );
    AREG_CHILD_CHECK( owner.mCalls.load( ) == 1u );
    areg::Application::release( );
    owner.stop_thread( );
}

/**
 * \brief   Reproduces the defect: the service manager of the second session ended when the
 *          application disconnected the router, because the mark of the first shutdown stayed.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, disconnecting_the_router_does_not_stop_the_next_session )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort router;
    restart_test::ConfigText text;
    text.routerPort = router.port( );
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };
    SessionLog log;
    SessionData data{ { 1u }, &log };
    add_restart_model( MODEL_ONE, data );

    for ( uint32_t session = 1u; session <= 2u; ++session )
    {
        start_service_session( config, false, true );
        AREG_CHILD_CHECK( router.client_connects( restart_test::WAIT_MS ) );
        areg::Application::disconnect_router( );
        AREG_CHILD_CHECK( areg::Application::load_model( MODEL_ONE, true ) );
        AREG_CHILD_CHECK( areg::Application::is_model_loaded( MODEL_ONE ) );
        AREG_CHILD_CHECK( restart_test::thread_exists( restart_test::SERVICE_THREAD ) );
        areg::Application::release( );
        restart_test::check_system_is_stopped( );
    }

    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
}

/**
 * \brief   Asking to stop a router connection that was never started changes nothing.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, disconnect_without_a_connection_changes_nothing )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort router;
    restart_test::ConfigText text;
    text.routerPort = router.port( );
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };
    start_service_session( config );
    areg::Application::disconnect_router( );
    AREG_CHILD_CHECK( areg::Application::connect_router( ) );
    AREG_CHILD_CHECK( router.client_connects( restart_test::WAIT_MS ) );
    areg::Application::release( );
    restart_test::check_system_is_stopped( );
}

/**
 * \brief   A connection requested while the router client stops starts after the stop.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, connect_requested_while_the_router_stops_is_applied )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort router;
    restart_test::ConfigText text;
    text.routerPort = router.port( );
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };
    start_service_session( config, false, true );
    AREG_CHILD_CHECK( router.client_connects( restart_test::WAIT_MS ) );
    areg::Application::disconnect_router( );
    static_cast<void>(areg::Application::connect_router( ));
    AREG_CHILD_CHECK( router.client_connects( restart_test::WAIT_MS ) );
    areg::Application::release( );
    restart_test::check_system_is_stopped( );
}

/**
 * \brief   The thread that releases need not be the thread that set up.
 **/
RESTART_TEST( ApplicationRestartRegressionTest, session_set_up_and_released_on_different_threads )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    SessionLog log;
    SessionData data{ { 1u }, &log };
    add_restart_model( MODEL_ONE, data );
    for ( uint32_t session = 1u; session <= 3u; ++session )
    {
        log.clear( );
        data.tag.store( session );
        std::thread( [&config]( ) { start_service_session( config ); } ).join( );
        exchange( MODEL_ONE, data );
        std::thread( []( ) { areg::Application::release( ); } ).join( );
        restart_test::check_system_is_stopped( );
        check_no_component_left( );
    }

    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
}

/************************************************************************
 * Configuration across sessions.
 ************************************************************************/

/**
 * \brief   Reproduces the defect: the second setup kept the properties of the first and ignored
 *          what the file holds now.
 **/
RESTART_TEST( ApplicationRestartConfigTest, next_setup_reads_the_file_again )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.routerPort = 1111u;
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };
    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::Application::config_manager( ).remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 1111u );
    areg::Application::release( );

    text.routerPort = 2222u;
    sandbox.write( "areg.init", text.text( ) );
    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::Application::config_manager( ).remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 2222u );
    areg::Application::release( );
}

/**
 * \brief   Reproduces the defect: the second setup with another file stayed on the first file.
 **/
RESTART_TEST( ApplicationRestartConfigTest, next_setup_can_use_another_file )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.routerPort = 1111u;
    restart_test::ConfigText textB;
    textB.routerPort = 2222u;
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    areg::Application::setup( false, false, false, false, false, fileA.c_str( ) );
    areg::Application::release( );
    areg::Application::setup( false, false, false, false, false, fileB.c_str( ) );

    const areg::ConfigManager & config{ areg::Application::config_manager( ) };
    AREG_CHILD_CHECK( std::filesystem::equivalent( config.config_file( ).as_string( ), fileB ) );
    AREG_CHILD_CHECK( config.remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 2222u );
    areg::Application::release( );
}

/**
 * \brief   Changes made at run time, temporary or not, do not survive the release.
 **/
RESTART_TEST( ApplicationRestartConfigTest, run_time_changes_do_not_survive_the_release )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.routerPort = 1111u;
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };
    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    areg::ConfigManager & manager{ areg::Application::config_manager( ) };
    manager.set_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, 3333u, false );
    manager.set_service_address( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, "10.1.2.3", true );
    areg::Application::release( );

    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    AREG_CHILD_CHECK( manager.remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 1111u );
    AREG_CHILD_CHECK( manager.remote_service_address( areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip ) == "127.0.0.1" );
    areg::Application::release( );
}

/**
 * \brief   A file loaded before the setup stays the configuration of that setup.
 **/
RESTART_TEST( ApplicationRestartConfigTest, file_loaded_after_release_is_the_configuration_of_the_next_setup )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.routerPort = 1111u;
    restart_test::ConfigText textB;
    textB.routerPort = 2222u;
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    areg::Application::setup( false, false, false, false, false, fileA.c_str( ) );
    areg::Application::release( );
    AREG_CHILD_CHECK( areg::Application::load_configuration( fileB.c_str( ) ) );
    areg::ConfigManager & manager{ areg::Application::config_manager( ) };
    manager.set_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, 4444u, true );

    areg::Application::setup( false, false, false, false, false, fileA.c_str( ) );
    AREG_CHILD_CHECK( std::filesystem::equivalent( manager.config_file( ).as_string( ), fileB ) );
    AREG_CHILD_CHECK( manager.remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 4444u );
    areg::Application::release( );
}

/**
 * \brief   The reset drops every property, and the next load reads a file again.
 **/
RESTART_TEST( ApplicationRestartConfigTest, reset_drops_the_properties_and_the_next_load_reads_a_file )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.routerPort = 1111u;
    restart_test::ConfigText textB;
    textB.routerPort = 2222u;
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    AREG_CHILD_CHECK( areg::Application::load_configuration( fileA.c_str( ) ) );
    areg::ConfigManager & manager{ areg::Application::config_manager( ) };
    manager.set_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, 5555u, true );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) );
    AREG_CHILD_CHECK( areg::Application::is_configured( ) == false );
    AREG_CHILD_CHECK( manager.config_file( ).is_empty( ) );
    AREG_CHILD_CHECK( manager.readonly_properties( ).is_empty( ) );
    AREG_CHILD_CHECK( manager.module_properties( ).is_empty( ) );

    AREG_CHILD_CHECK( areg::Application::load_configuration( fileB.c_str( ) ) );
    AREG_CHILD_CHECK( manager.remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 2222u );
}

/**
 * \brief   The reset is refused while the system runs and changes nothing.
 **/
RESTART_TEST( ApplicationRestartConfigTest, reset_is_refused_while_the_system_runs )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.routerPort = 1111u;
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };
    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) == false );
    AREG_CHILD_CHECK( areg::Application::is_configured( ) );
    AREG_CHILD_CHECK( areg::Application::config_manager( ).remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 1111u );
    areg::Application::release( );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) );
}

/**
 * \brief   A file can be switched after a reset; the switch reaches the next setup.
 **/
RESTART_TEST( ApplicationRestartConfigTest, switch_of_file_after_reset_reaches_the_next_setup )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.routerPort = 1111u;
    restart_test::ConfigText textB;
    textB.routerPort = 2222u;
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    AREG_CHILD_CHECK( areg::Application::load_configuration( fileA.c_str( ) ) );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) );
    AREG_CHILD_CHECK( areg::Application::load_configuration( fileB.c_str( ) ) );
    areg::Application::setup( false, false, false, false, false, fileA.c_str( ) );
    const areg::ConfigManager & manager{ areg::Application::config_manager( ) };
    AREG_CHILD_CHECK( std::filesystem::equivalent( manager.config_file( ).as_string( ), fileB ) );
    AREG_CHILD_CHECK( manager.remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 2222u );
    areg::Application::release( );
}

/**
 * \brief   A release before any setup keeps the configuration that was loaded.
 **/
RESTART_TEST( ApplicationRestartConfigTest, release_without_a_session_keeps_the_loaded_configuration )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.routerPort = 1111u;
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };
    AREG_CHILD_CHECK( areg::Application::load_configuration( config.c_str( ) ) );
    areg::Application::release( );
    AREG_CHILD_CHECK( areg::Application::is_configured( ) );
    AREG_CHILD_CHECK( areg::Application::config_manager( ).remote_service_port( areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip ) == 1111u );
}

/**
 * \brief   A named element stored by the application stays readable after the release.
 **/
RESTART_TEST( ApplicationRestartConfigTest, stored_element_stays_readable_after_the_release )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "a.init", restart_test::ConfigText{ }.text( ) ) };
    areg::Application::setup( false, false, false, false, false, config.c_str( ) );
    areg::Primitive value{ };
    value.valInt.mElement = 41;
    static_cast<void>(areg::Application::store_element( "ApplicationRestartExitCode", value ));
    areg::Application::release( );
    AREG_CHILD_CHECK( areg::Application::stored_element( "ApplicationRestartExitCode" ).valInt.mElement == 41 );
}

/************************************************************************
 * Connections after a switch of the configuration.
 ************************************************************************/

/**
 * \brief   Reproduces the defect: the router client of the next session connected to the
 *          address of the previous configuration.
 **/
RESTART_TEST( ApplicationRestartNetworkTest, next_session_connects_to_the_router_of_the_new_file )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort routerA;
    restart_test::TcpPort routerB;
    restart_test::ConfigText textA;
    textA.routerPort = routerA.port( );
    restart_test::ConfigText textB;
    textB.routerPort = routerB.port( );
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    start_service_session( fileA, false, true );
    AREG_CHILD_CHECK( routerA.client_connects( restart_test::WAIT_MS ) );
    areg::Application::release( );

    start_service_session( fileB, false, true );
    AREG_CHILD_CHECK( routerB.client_connects( restart_test::WAIT_MS ) );
    AREG_CHILD_CHECK( routerA.client_connects( 0u ) == false );
    areg::Application::release( );
}

#if AREG_LOGGING

/**
 * \brief   Reproduces the defect: the remote logger of the next session connected to the log
 *          collector of the previous configuration.
 **/
RESTART_TEST( ApplicationRestartNetworkTest, next_session_connects_to_the_log_collector_of_the_new_file )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort collectorA;
    restart_test::TcpPort collectorB;
    restart_test::ConfigText textA;
    textA.loggerPort = collectorA.port( );
    restart_test::ConfigText textB;
    textB.loggerPort = collectorB.port( );
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    start_service_session( fileA, true, false );
    AREG_CHILD_CHECK( collectorA.client_connects( restart_test::WAIT_MS ) );
    areg::Application::release( );

    start_service_session( fileB, true, false );
    AREG_CHILD_CHECK( collectorB.client_connects( restart_test::WAIT_MS ) );
    AREG_CHILD_CHECK( collectorA.client_connects( 0u ) == false );
    areg::Application::release( );
}

/************************************************************************
 * Logging across sessions.
 ************************************************************************/

/**
 * \brief   Reproduces the defect: the second session kept writing to the file and with the
 *          layout of the first configuration.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, next_session_uses_the_file_and_layout_of_the_new_file )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.logFile = sandbox.path( "a.log" );
    textA.layout = "A-%m%n";
    textA.extra = ROW_SCOPE_ON;
    restart_test::ConfigText textB;
    textB.logFile = sandbox.path( "b.log" );
    textB.layout = "B-%m%n";
    textB.extra = ROW_SCOPE_ON;
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    areg::Application::setup( true, false, false, true, false, fileA.c_str( ) );
    log_row( 1u );
    areg::Application::release( );

    areg::Application::setup( true, false, false, true, false, fileB.c_str( ) );
    log_row( 2u );
    areg::Application::release( );

    const std::string logA{ restart_test::read_text( textA.logFile ) };
    const std::string logB{ restart_test::read_text( textB.logFile ) };
    AREG_CHILD_CHECK( restart_test::contains( logA, "A-restart-row-1" ) );
    AREG_CHILD_CHECK( restart_test::contains( logA, "row-2" ) == false );
    AREG_CHILD_CHECK( restart_test::contains( logB, "B-restart-row-2" ) );
    AREG_CHILD_CHECK( restart_test::contains( logB, "A-restart" ) == false );
}

/**
 * \brief   Reproduces the defect: a scope priority of the previous configuration stayed for a
 *          scope the new configuration does not name.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, scope_priorities_of_the_old_file_are_gone )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText textA;
    textA.logFile = sandbox.path( "a.log" );
    textA.extra = ROW_SCOPE_ON;
    restart_test::ConfigText textB;
    textB.logFile = sandbox.path( "b.log" );
    const std::string fileA{ sandbox.write( "a.init", textA.text( ) ) };
    const std::string fileB{ sandbox.write( "b.init", textB.text( ) ) };

    areg::Application::setup( true, false, false, true, false, fileA.c_str( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) != static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    areg::Application::release( );

    areg::Application::setup( true, false, false, true, false, fileB.c_str( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    log_row( 2u );
    areg::Application::release( );
    AREG_CHILD_CHECK( restart_test::contains( restart_test::read_text( textB.logFile ), "restart-row-2" ) == false );
}

/**
 * \brief   A scope priority set at run time ends with the session.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, scope_priority_set_at_run_time_ends_with_the_session )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    areg::Application::setup( true, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::set_scope_priority( ROW_SCOPE, static_cast<uint32_t>(areg::LogPriority::PrioDebug) ) != static_cast<uint32_t>(areg::LogPriority::PrioInvalid) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioDebug) );
    areg::Application::release( );

    areg::Application::setup( true, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    areg::Application::release( );
}

/**
 * \brief   A log source stopped or paused by an observer is active in the next session.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, source_state_of_a_session_does_not_reach_the_next )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    text.extra = ROW_SCOPE_ON;
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    for ( const areg::LogSourceState state : { areg::LogSourceState::Stopped, areg::LogSourceState::Paused } )
    {
        areg::Application::setup( true, false, false, true, false, config.c_str( ) );
        static_cast<void>(areg::set_source_state( state ));
        AREG_CHILD_CHECK( areg::source_state( ) == state );
        areg::Application::release( );

        areg::Application::setup( true, false, false, true, false, config.c_str( ) );
        AREG_CHILD_CHECK( areg::source_state( ) == areg::LogSourceState::Active );
        AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) != static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
        areg::Application::release( );
    }
}

/**
 * \brief   The reset forgets the scope priorities derived from the dropped configuration.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, reset_forgets_the_configured_scope_priorities )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    text.extra = ROW_SCOPE_ON;
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    AREG_CHILD_CHECK( areg::Application::load_configuration( config.c_str( ) ) );
    AREG_CHILD_CHECK( areg::restore_logging( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) != static_cast<uint32_t>(areg::LogPriority::PrioNotset) );

    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    AREG_CHILD_CHECK( areg::is_configured( ) == false );
}

/**
 * \brief   The reset is refused while logging runs, and works after it stopped.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, reset_is_refused_while_the_logging_runs )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    AREG_CHILD_CHECK( areg::start_logging( config.c_str( ) ) );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) == false );
    AREG_CHILD_CHECK( areg::is_configured( ) );
    areg::stop_logging( true );
    AREG_CHILD_CHECK( areg::Application::reset_configuration( ) );
    AREG_CHILD_CHECK( areg::is_configured( ) == false );
}

/**
 * \brief   Logging forced in a session does not stay forced in the next one.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, forced_logging_of_a_session_does_not_reach_the_next )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    areg::Application::setup( false, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::force_start_logging( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) != static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    areg::Application::release( );

    areg::Application::setup( true, false, false, true, false, config.c_str( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    areg::Application::release( );
}

/**
 * \brief   The restore puts the configured priorities back, and the scopes stay registered.
 **/
RESTART_TEST( ApplicationRestartLoggingTest, restore_sets_the_configured_priorities_and_keeps_the_scopes )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "a.log" );
    text.extra = ROW_SCOPE_ON;
    const std::string config{ sandbox.write( "a.init", text.text( ) ) };

    areg::Application::setup( true, false, false, true, false, config.c_str( ) );
    const uint32_t configured{ areg::scope_priority( ROW_SCOPE ) };
    AREG_CHILD_CHECK( configured != static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    static_cast<void>(areg::set_scope_priority( ROW_SCOPE, static_cast<uint32_t>(areg::LogPriority::PrioNotset) ));
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
    AREG_CHILD_CHECK( areg::restore_logging( ) );
    AREG_CHILD_CHECK( areg::scope_priority( ROW_SCOPE ) == configured );
    log_row( 1u );
    areg::Application::release( );
    AREG_CHILD_CHECK( restart_test::contains( restart_test::read_text( text.logFile ), "restart-row-1" ) );
}

#endif  // AREG_LOGGING

/************************************************************************
 * Stress: many sessions, with work running around them.
 ************************************************************************/

namespace
{
#if defined(__SANITIZE_THREAD__) || defined(__SANITIZE_ADDRESS__)
    constexpr uint32_t STRESS_CYCLES{ 40u };
#else
    constexpr uint32_t STRESS_CYCLES{ 200u };
#endif

    //!< The sessions with foreign threads logging, each one drains a full log queue.
    constexpr uint32_t FOREIGN_THREAD_CYCLES{ 12u };
}

/**
 * \brief   The whole service session repeats without a leftover.
 **/
RESTART_TEST( ApplicationRestartStressTest, service_session_repeats_without_a_leftover )
{
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };
    SessionLog log;
    SessionData data{ { 1u }, &log };
    add_restart_model( MODEL_ONE, data );

    for ( uint32_t cycle = 1u; cycle <= STRESS_CYCLES; ++cycle )
    {
        log.clear( );
        data.tag.store( cycle );
        start_service_session( config );
        exchange( MODEL_ONE, data );
        areg::Application::release( );
        restart_test::check_system_is_stopped( );
        check_no_component_left( );
        AREG_CHILD_CHECK( log.count( Seen::Deleted ) == 2u );
    }

    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
}

/**
 * \brief   The session with logging and a pending router connection repeats without a leftover.
 **/
RESTART_TEST( ApplicationRestartStressTest, session_with_logging_and_routing_repeats_without_a_leftover )
{
    const restart_test::Sandbox sandbox;
    restart_test::TcpPort router;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "stress.log" );
    text.routerPort = router.port( );
    text.extra = ROW_SCOPE_ON;
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };

    for ( uint32_t cycle = 1u; cycle <= STRESS_CYCLES; ++cycle )
    {
        start_service_session( config, true, true );
        log_row( cycle );
        AREG_CHILD_CHECK( router.client_connects( restart_test::WAIT_MS ) );
        areg::Application::release( );
        restart_test::check_system_is_stopped( );

#if AREG_LOGGING
        const std::string logged{ restart_test::read_text( text.logFile ) };
        AREG_CHILD_CHECK( restart_test::contains( logged, ("restart-row-" + std::to_string( cycle )).c_str( ) ) );
#endif  // AREG_LOGGING
    }
}

/**
 * \brief   Foreign threads that read and log while the sessions start and end do no harm.
 **/
RESTART_TEST( ApplicationRestartStressTest, foreign_threads_run_across_the_sessions )
{
    const restart_test::Sandbox sandbox;
    restart_test::ConfigText text;
    text.logFile = sandbox.path( "stress.log" );
    text.extra = ROW_SCOPE_ON;
    const std::string config{ sandbox.write( "areg.init", text.text( ) ) };

    std::atomic<bool> stop{ false };
    std::array<std::thread, 4> threads;
    for ( auto & worker : threads )
    {
        worker = std::thread( [&stop]( )
            {
                uint32_t row{ 0u };
                while ( stop.load( ) == false )
                {
                    log_row( ++row );
                    static_cast<void>(areg::Application::system_state( ));
                    static_cast<void>(areg::Application::is_servicing_ready( ));
                    static_cast<void>(restart_test::thread_exists( restart_test::SERVICE_THREAD ));
                    std::this_thread::yield( );
                }
            } );
    }

    for ( uint32_t cycle = 0u; cycle < FOREIGN_THREAD_CYCLES; ++cycle )
    {
        start_service_session( config, true, false );
        areg::Application::release( );
    }

    stop.store( true );
    for ( auto & worker : threads )
    {
        worker.join( );
    }

    restart_test::check_system_is_stopped( );
}

/************************************************************************
 * Exit: the singletons outlive the sessions.
 ************************************************************************/

namespace
{
    std::mutex              _exitLock;
    std::condition_variable _exitChanged;
    bool                    _exitThreadReady{ false };
    bool                    _exitAsked{ false };
    bool                    _exitThreadDone{ false };

    //!< Reads the framework singletons; ends the process if one of them is no longer usable.
    void touch_singletons( )
    {
        if ( (areg::Application::system_state( ) != areg::AppState::Stopped)
          || areg::Component::find_by_name( areg::String( PROVIDER_ROLE ) ) != nullptr
          || areg::Application::is_model_loaded( "ApplicationRestartNoModel" )
          || (areg::Application::thread_state( PROVIDER_THREAD ) != areg::ThreadState::Unknown)
          || (areg::Process::instance( ).app_name( ).is_empty( ))
          || restart_test::thread_exists( restart_test::SERVICE_THREAD ) )
        {
            std::fputs( "a singleton is not usable at exit\n", stderr );
            std::_Exit( areg_child::CHILD_CHECK_FAILED );
        }

        areg::Application::signal_quit( );
    }

    //!< Touches the singletons when its thread ends.
    struct ExitingThreadGuard
    {
        ~ExitingThreadGuard( )
        {
            touch_singletons( );
            std::lock_guard<std::mutex> guard( _exitLock );
            _exitThreadDone = true;
            _exitChanged.notify_all( );
        }
    };

    //!< Lets the thread end and waits for it, then touches the singletons again.
    void on_process_exit( )
    {
        std::unique_lock<std::mutex> guard( _exitLock );
        _exitAsked = true;
        _exitChanged.notify_all( );
        if ( _exitChanged.wait_for( guard, std::chrono::milliseconds( restart_test::WAIT_MS ), []( ) { return _exitThreadDone; } ) == false )
        {
            std::fputs( "the thread-local destructor did not run during the exit\n", stderr );
            std::_Exit( areg_child::CHILD_CHECK_FAILED );
        }

        guard.unlock( );
        touch_singletons( );
    }
}

/**
 * \brief   After the release the singletons stay usable in an exit handler and in the
 *          thread-local destructor of a thread that ends during the exit.
 **/
RESTART_TEST( ApplicationRestartExitTest, singletons_stay_usable_while_the_process_exits )
{
    // The thread-local object is created before the exit handler is registered, so the
    // C++ runtime still runs its destructor while the handler waits.
    std::thread( []( )
        {
            thread_local ExitingThreadGuard exitGuard;
            static_cast<void>(&exitGuard);
            std::unique_lock<std::mutex> guard( _exitLock );
            _exitThreadReady = true;
            _exitChanged.notify_all( );
            _exitChanged.wait( guard, []( ) { return _exitAsked; } );
        } ).detach( );

    {
        std::unique_lock<std::mutex> guard( _exitLock );
        AREG_CHILD_CHECK( _exitChanged.wait_for( guard, std::chrono::milliseconds( restart_test::WAIT_MS ), []( ) { return _exitThreadReady; } ) );
    }

    std::atexit( &on_process_exit );
    const restart_test::Sandbox sandbox;
    const std::string config{ sandbox.write( "areg.init", restart_test::ConfigText{ }.text( ) ) };

    SessionLog log;
    SessionData data{ { 1u }, &log };
    add_restart_model( MODEL_ONE, data );
    start_service_session( config );
    exchange( MODEL_ONE, data );
    areg::Application::release( );
    areg::ComponentLoader::remove_component_model( areg::String( MODEL_ONE ) );
}
