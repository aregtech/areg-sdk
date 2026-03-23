/************************************************************************
 * \file        pubclient/src/ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client implementation
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, response_identifier);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, response_hello_unblock);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, request_hello_ublock_failed);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, on_hello_service_state_update);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceClient, process_timer);

ServiceClient::ServiceClient( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component             ( entry, owner )
    , HelloUnblockConsumerBase( entry.mDependencyServices[0], static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )

    , mClientId             ( HelloUnblock::InvalidId )
    , mSequenceId           ( 0 )
    , mRespReceived         ( 0 )
    , mSequenceList         ( )
    , mTimer                ( static_cast<areg::TimerConsumer &>(self()), role_name() )
#ifdef DEBUG
    , mReqCount             ( 0 )
#endif // DEBUG
{
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, service_connected );
    bool result = HelloUnblockConsumerBase::service_connected( status, proxy );
    mClientId = HelloUnblock::InvalidId;
    notify_on_hello_service_state_update( is_connected( ) );
    if ( is_connected( ) == false )
    {
        ASSERT((status == areg::ServiceConnectionState::ConnectionLost) || (mReqCount == 2)); // Only in debug build
        mTimer.stop_timer( );
        areg::Application::signal_quit( );
    }
    else
    {
#ifdef DEBUG
        mReqCount = 2;  // Initially, 2 listeners: connection and HelloServiceState data update.
#endif // DEBUG

    }

    return result;
}

void ServiceClient::response_identifier( uint32_t clientId )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, response_identifier );
    ASSERT( mClientId == HelloUnblock::InvalidId );
    ASSERT( mSequenceId == 0 );
    ASSERT( mRespReceived == 0 );

    // Reduce the request count. Then there must be only 2 listeners left: connection and HelloServiceState data update.
    ASSERT(--mReqCount == service_proxy()->listener_count());
    ASSERT(mReqCount == 2);

    mClientId = clientId;

    LOG_DBG( "Received ID [ %u ] for the client, sending first request with sequence [ %u ]", mClientId, mSequenceId );
    areg::String timestamp(areg::DateTime::now().format_time());
    std::cout << ">>> Request at [ " << timestamp << " ]:"
              << " Client = " << mClientId
              << " Sequence = " << mSequenceId
              << std::endl;

    ASSERT(mSequenceId == 0);
    mTimer.start_timer(HelloUnblock::ClientTimeot, areg::Timer::CONTINUOUSLY);
}

void ServiceClient::response_hello_unblock( uint32_t clientId, uint32_t seqNr )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, response_hello_unblock );
    ASSERT( mSequenceList.is_empty( ) == false );

    // Reduce the request count. Then the total number of listeners should be no less than 2 or number of requests plus 2.
    ASSERT(-- mReqCount == service_proxy()->listener_count());
    ASSERT((mReqCount >= 2) && (mReqCount <= (mSequenceId + 2)));

    uint32_t savedNr = mSequenceList.pop_first( );
    areg::String timestamp( areg::DateTime::now( ).format_time( ) );

    ++ mRespReceived;

    if (savedNr != seqNr)
    {
        // Don't remove this 'if'-condition to not get warning in Release build!
        // In Release, to escape warning of unused variable 'savedNr',
        // instead of ASSERT(savedNr == seqNr),
        // use 'if'-condition and ASSERT(false)
        ASSERT(false);
    }
    ASSERT(mRespReceived == seqNr);
    ASSERT(mClientId == clientId);

    std::cout << "<<< Response at [ " << timestamp << " ]:"
        << " Client = " << clientId
        << " Sequence Nr = " << seqNr
        << " Pending = " << mSequenceList.size()
        << " Sent = " << mSequenceId
        << " Received = " << mRespReceived
        << std::endl;
}

void ServiceClient::request_hello_ublock_failed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, request_hello_ublock_failed );
    LOG_WARN( "The request HelloUnblock failed with reason [ %s ]", areg::as_string( FailureReason ) );
    // Make sure it does not fail with reason 'request is busy'
    ASSERT( FailureReason != areg::ResultType::RequestBusy );
}

void ServiceClient::on_hello_service_state_update( HelloUnblock::RunState HelloServiceState, areg::DataState state )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, on_hello_service_state_update );
    LOG_DBG( "Service state [ %s ], data state [ %s ]", HelloUnblock::as_string( HelloServiceState ), areg::as_string( state ) );

    if (state == areg::DataState::DataIsOK)
    {
        if ( HelloServiceState == HelloUnblock::RunState::ServiceActive )
        {
            ASSERT( mClientId == HelloUnblock::InvalidId );
            // send the request only if client has no valid ID
            request_identifier( );
            LOG_DBG("Service is active, requesting ID");

            // Increase the number of listeners. It shouldn't be more than 3: connection, HelloServiceState update and 1 more request
            ASSERT(++mReqCount == service_proxy()->listener_count());
            ASSERT(mReqCount == 3);
        }
        else if ( HelloServiceState == HelloUnblock::RunState::Shutdown )
        {
            LOG_WARN( "The service is unavailable, shutting down application." );
            mTimer.stop_timer( );
            areg::Application::signal_quit( );
        }
    }
}

void ServiceClient::process_timer( areg::Timer & /* timer */ )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient, process_timer );
    ASSERT( mSequenceId < HelloUnblock::MaxMessages );
    
    request_hello_ublock(mClientId, ++mSequenceId);
    ASSERT(++mReqCount == service_proxy()->listener_count());
    mSequenceList.push_last( mSequenceId );

    areg::String timestamp( areg::DateTime::now( ).format_time( ) );
    LOG_DBG( "Client [ %u ] timeout, sending next request with sequence [ %u ]", mClientId, mSequenceId );
    std::cout << ">>> Request at [ " << timestamp << " ]:"
              << " Client = " << mClientId
              << " Sequence = " << mSequenceId << std::endl;

    if ( mSequenceId == HelloUnblock::MaxMessages )
    {
        LOG_DBG( "Reached maximum requests. Stop the timer" );
        mTimer.stop_timer();
    }
}
