/************************************************************************
 * \file        pubservice/src/ServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is demonstration of manual unlocking of the request.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/ServiceComponent.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"

#include <iostream>

DEF_TRACE_SCOPE( examples_21_pubservice_ServiceComponent_startupServiceInterface );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceComponent_requestIdentifier );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceComponent_requestHelloUblock );
DEF_TRACE_SCOPE( examples_21_pubservice_ServiceComponent_processTimer );

Component * ServiceComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW ServiceComponent( entry, owner );
}

void ServiceComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
{
    delete (&compObject);
}

ServiceComponent::ServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component         ( entry, owner )
    , HelloUnblockStub  ( static_cast<Component &>(self()) )
    , IETimerConsumer   ( )

    , mSessionList      ( )
    , mTimer            ( static_cast<IETimerConsumer &>(self()), entry.mRoleName )
{
    setHelloServiceState( NEHelloUnblock::eServiceState::ServiceUndefined );
}

void ServiceComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceComponent_startupServiceInterface );

    HelloUnblockStub::startupServiceInterface( holder );
    setHelloServiceState( NEHelloUnblock::eServiceState::ServiceActive );
    TRACE_DBG( "The service [ %s ] is up and running", getRoleName( ).getString( ) );
}

void ServiceComponent::requestIdentifier( void )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceComponent_requestIdentifier );
    uint32_t clientId = NEUtilities::generateUniqueId( );
    TRACE_DBG( "Generated ID for the clinet: %u ", clientId );
    responseIdentifier( clientId );
}

void ServiceComponent::requestHelloUblock( unsigned int clientId, unsigned int seqNr )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceComponent_requestHelloUblock );

    ASSERT( clientId != NEHelloUnblock::InvalidId );

    String timestamp( DateTime::getNow( ).formatTime( ) );
    uint32_t sessionId = unblockCurrentRequest( );

    TRACE_DBG( "Received request: client [ %u ], sequence Nr [ %u ], unlock session [ %u ]", clientId, seqNr, sessionId );
    if ( mSessionList.isEmpty() )
    {
        ASSERT( mTimer.isActive( ) == false );
        TRACE_DBG( "First request with valid ID, trigger timer to send response every [ %u ] ms", NEHelloUnblock::ServiceTimeout );
        mTimer.startTimer( NEHelloUnblock::ServiceTimeout, Timer::CONTINUOUSLY );
    }

    mSessionList.pushLast( SessionEtnry{ clientId, seqNr, sessionId } );
    std::cout << ">>> Request at [ " << timestamp << " ]: "
              << " Client = " << clientId << ", "
              << " Sequence = " << seqNr << ", "
              << " Session = " << sessionId << std::endl;
}

void ServiceComponent::processTimer( Timer & timer )
{
    TRACE_SCOPE( examples_21_pubservice_ServiceComponent_processTimer );
    SessionEtnry entry = mSessionList.popFirst( );

    TRACE_DBG( "Prepared session [ %u ] to send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );

    if ( prepareResponse( entry.id ) )
    {
        String timestamp( DateTime::getNow( ).formatTime( ) );
        responseHelloUnblock( entry.clientId, entry.seqNr );
        TRACE_DBG( "Succeeded to send response to client [ %u ], sequence [ %u ]", entry.clientId, entry.seqNr );

        std::cout << "<<< Response at [ " << timestamp << " ]: "
                  << " Clinet = " << entry.clientId
                  << " Sequence = " << entry.seqNr << std::endl;
    }
    else
    {
        ASSERT( false );
        TRACE_ERR( "There is no session [ %u ], cannot send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );
    }

    if ( mSessionList.isEmpty( ) )
    {
        mTimer.stopTimer( );
        setHelloServiceState( NEHelloUnblock::eServiceState::ServiceShutdown );

        TRACE_WARN( "No more saved sessions in the list, quit application!" );
        Application::signalAppQuit( );
    }
}
