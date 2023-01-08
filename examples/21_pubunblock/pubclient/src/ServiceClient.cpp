/************************************************************************
 * \file        pubclient/src/ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of manual unlbocking of the request.
 *              The service client implementation
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_serviceConnected );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseIdentifier );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseHelloUnblock );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_requestHelloUblockFailed );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_onHelloServiceStateUpdate );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceClient_processTimer );

Component * ServiceClient::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ServiceClient( entry, owner );
}

void ServiceClient::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

ServiceClient::ServiceClient( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component             ( entry, owner )
    , HelloUnblockClientBase( entry.mDependencyServices[0], static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mClientId             ( NEHelloUnblock::InvalidId )
    , mSequenceId           ( 0 )
    , mSequenceList         ( )
    , mTimer                ( static_cast<IETimerConsumer &>(self()), getRoleName() )
{
}

bool ServiceClient::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_serviceConnected );
    bool result{ false };
    if ( HelloUnblockClientBase::serviceConnected( isConnected, proxy ) )
    {
        result = true;
        mClientId = NEHelloUnblock::InvalidId;
        notifyOnHelloServiceStateUpdate( isConnected );
        if ( isConnected == false )
        {
            mTimer.stopTimer( );
            Application::signalAppQuit( );
        }
    }

    return result;
}

void ServiceClient::responseIdentifier( unsigned int clientId )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseIdentifier );
    ASSERT( mClientId == NEHelloUnblock::InvalidId );
    ASSERT( mSequenceId == 0 );
    mClientId = clientId;
    requestHelloUblock(mClientId, ++mSequenceId);
    mSequenceList.pushLast( mSequenceId );

    String timestamp( DateTime::getNow( ).formatTime( ) );
    TRACE_DBG( "Received ID [ %u ] for the client, sending first request with sequence [ %u ]", mClientId, mSequenceId );
    std::cout << ">>> Request at [ " << timestamp << " ]:"
              << " Client = " << mClientId
              << " Sequence = " << mSequenceId << std::endl;

    if ( mSequenceId < NEHelloUnblock::MaxMessages )
    {
        mTimer.startTimer( NEHelloUnblock::ClientTimeot, Timer::CONTINUOUSLY );
    }
}

void ServiceClient::responseHelloUnblock( unsigned int clientId, unsigned int seqNr )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseHelloUnblock );
    ASSERT( mSequenceList.isEmpty( ) == false );
    uint32_t savedNr = mSequenceList.popFirst( );
    String timestamp( DateTime::getNow( ).formatTime( ) );

    ASSERT( mClientId == clientId );

    std::cout << "<<< Response at [ " << timestamp << " ]:"
              << " Client = " << clientId
              << " Sequence Nr = " << seqNr 
              << " Pending = " << mSequenceList.getSize() << std::endl;

    if (savedNr != seqNr)
    {
        ASSERT( false );
        std::cerr << "No matching of sequence number! Saved = " << savedNr << ", in response = " << seqNr << std::endl;
    }
}

void ServiceClient::requestHelloUblockFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_requestHelloUblockFailed );
    TRACE_WARN( "The reuqest HelloUnblock failed with reason [ %s ]", NEService::getString( FailureReason ) );
    // Make sure it does not fail with reason 'request is busy'
    ASSERT( FailureReason != NEService::eResultType::RequestBusy );
}

void ServiceClient::onHelloServiceStateUpdate( NEHelloUnblock::eServiceState HelloServiceState, NEService::eDataStateType state )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_onHelloServiceStateUpdate );
    TRACE_DBG( "Service state [ %s ], data state [ %s ]", NEHelloUnblock::getString( HelloServiceState ), NEService::getString( state ) );

    if (state == NEService::eDataStateType::DataIsOK)
    {
        if ( HelloServiceState == NEHelloUnblock::eServiceState::ServiceActive )
        {
            ASSERT( mClientId == NEHelloUnblock::InvalidId );
            // send the request only if client has no valid ID
            requestIdentifier( );
            TRACE_DBG( "Service is active, requesting ID" );
        }
        else if ( HelloServiceState == NEHelloUnblock::eServiceState::ServiceShutdown )
        {
            TRACE_WARN( "The service is unavailable, shutting down application." );
            mTimer.stopTimer( );
            Application::signalAppQuit( );
        }
    }
}

void ServiceClient::processTimer( Timer & timer )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_processTimer );
    ASSERT( mSequenceId < NEHelloUnblock::MaxMessages );
    
    requestHelloUblock(mClientId, ++mSequenceId);
    mSequenceList.pushLast( mSequenceId );

    String timestamp( DateTime::getNow( ).formatTime( ) );
    TRACE_DBG( "Client [ %u ] timeout, sending next request with sequence [ %u ]", mClientId, mSequenceId );
    std::cout << ">>> Request at [ " << timestamp << " ]:"
              << " Client = " << mClientId
              << " Sequence = " << mSequenceId << std::endl;

    if ( mSequenceId == NEHelloUnblock::MaxMessages )
    {
        TRACE_DBG( "Reached maximum requests. Stop the timer" );
        mTimer.stopTimer();
    }
}
