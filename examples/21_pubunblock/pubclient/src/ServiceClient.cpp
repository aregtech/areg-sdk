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
    , mRespReceived         ( 0 )
    , mSequenceList         ( )
    , mTimer                ( static_cast<IETimerConsumer &>(self()), getRoleName() )
#ifdef DEBUG
    , mReqCount             ( 0 )
#endif // DEBUG
{
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_serviceConnected );
    bool result = HelloUnblockClientBase::serviceConnected( status, proxy );
    mClientId = NEHelloUnblock::InvalidId;
    notifyOnHelloServiceStateUpdate( isConnected( ) );
    if ( isConnected( ) == false )
    {
        ASSERT((status == NEService::eServiceConnection::ServiceConnectionLost) || (mReqCount == 2)); // Only in debug build
        mTimer.stopTimer( );
        Application::signalAppQuit( );
    }
    else
    {
#ifdef DEBUG
        mReqCount = 2;  // Initially, 2 listeners: connection and HelloServiceState data update.
#endif // DEBUG

    }

    return result;
}

void ServiceClient::responseIdentifier( unsigned int clientId )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseIdentifier );
    ASSERT( mClientId == NEHelloUnblock::InvalidId );
    ASSERT( mSequenceId == 0 );
    ASSERT( mRespReceived == 0 );

    // Reduce the request count. Then there must be only 2 listeners left: connection and HelloServiceState data update.
    ASSERT(--mReqCount == getProxy()->getListenerCount());
    ASSERT(mReqCount == 2);

    mClientId = clientId;

    TRACE_DBG( "Received ID [ %u ] for the client, sending first request with sequence [ %u ]", mClientId, mSequenceId );
    String timestamp(DateTime::getNow().formatTime());
    std::cout << ">>> Request at [ " << timestamp << " ]:"
              << " Client = " << mClientId
              << " Sequence = " << mSequenceId
              << std::endl;

    ASSERT(mSequenceId == 0);
    mTimer.startTimer(NEHelloUnblock::ClientTimeot, Timer::CONTINUOUSLY);
}

void ServiceClient::responseHelloUnblock( unsigned int clientId, unsigned int seqNr )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_responseHelloUnblock );
    ASSERT( mSequenceList.isEmpty( ) == false );

    // Reduce the request count. Then the total number of listeners should be no less than 2 or number of requests plus 2.
    ASSERT(-- mReqCount == getProxy()->getListenerCount());
    ASSERT((mReqCount >= 2) && (mReqCount <= (mSequenceId + 2)));

    uint32_t savedNr = mSequenceList.popFirst( );
    String timestamp( DateTime::getNow( ).formatTime( ) );

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
        << " Pending = " << mSequenceList.getSize()
        << " Sent = " << mSequenceId
        << " Received = " << mRespReceived
        << std::endl;
}

void ServiceClient::requestHelloUblockFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceClient_requestHelloUblockFailed );
    TRACE_WARN( "The request HelloUnblock failed with reason [ %s ]", NEService::getString( FailureReason ) );
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
            TRACE_DBG("Service is active, requesting ID");

            // Increase the number of listeners. It shouldn't be more than 3: connection, HelloServiceState update and 1 more request
            ASSERT(++mReqCount == getProxy()->getListenerCount());
            ASSERT(mReqCount == 3);
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
    ASSERT(++mReqCount == getProxy()->getListenerCount());
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
