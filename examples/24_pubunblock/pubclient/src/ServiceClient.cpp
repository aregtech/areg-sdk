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

DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_serviceConnected );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_responseIdentifier );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_responseHelloUnblock );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_requestHelloUblockFailed );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_onHelloServiceStateUpdate );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceClient_processTimer );

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
    LOG_SCOPE( examples_24_pubservice_ServiceClient_serviceConnected );
    bool result = HelloUnblockConsumerBase::service_connected( status, proxy );
    mClientId = HelloUnblock::InvalidId;
    notifyOnHelloServiceStateUpdate( is_connected( ) );
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

void ServiceClient::responseIdentifier( uint32_t clientId )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient_responseIdentifier );
    ASSERT( mClientId == HelloUnblock::InvalidId );
    ASSERT( mSequenceId == 0 );
    ASSERT( mRespReceived == 0 );

    // Reduce the request count. Then there must be only 2 listeners left: connection and HelloServiceState data update.
    ASSERT(--mReqCount == proxy()->listener_count());
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

void ServiceClient::responseHelloUnblock( uint32_t clientId, uint32_t seqNr )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient_responseHelloUnblock );
    ASSERT( mSequenceList.is_empty( ) == false );

    // Reduce the request count. Then the total number of listeners should be no less than 2 or number of requests plus 2.
    ASSERT(-- mReqCount == proxy()->listener_count());
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

void ServiceClient::requestHelloUblockFailed( areg::ResultType FailureReason )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient_requestHelloUblockFailed );
    LOG_WARN( "The request HelloUnblock failed with reason [ %s ]", areg::as_string( FailureReason ) );
    // Make sure it does not fail with reason 'request is busy'
    ASSERT( FailureReason != areg::ResultType::RequestBusy );
}

void ServiceClient::onHelloServiceStateUpdate( HelloUnblock::RunState HelloServiceState, areg::DataState state )
{
    LOG_SCOPE( examples_24_pubservice_ServiceClient_onHelloServiceStateUpdate );
    LOG_DBG( "Service state [ %s ], data state [ %s ]", HelloUnblock::as_string( HelloServiceState ), areg::as_string( state ) );

    if (state == areg::DataState::DataIsOK)
    {
        if ( HelloServiceState == HelloUnblock::RunState::ServiceActive )
        {
            ASSERT( mClientId == HelloUnblock::InvalidId );
            // send the request only if client has no valid ID
            requestIdentifier( );
            LOG_DBG("Service is active, requesting ID");

            // Increase the number of listeners. It shouldn't be more than 3: connection, HelloServiceState update and 1 more request
            ASSERT(++mReqCount == proxy()->listener_count());
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
    LOG_SCOPE( examples_24_pubservice_ServiceClient_processTimer );
    ASSERT( mSequenceId < HelloUnblock::MaxMessages );
    
    requestHelloUblock(mClientId, ++mSequenceId);
    ASSERT(++mReqCount == proxy()->listener_count());
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
