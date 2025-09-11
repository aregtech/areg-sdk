/************************************************************************
 * \file        pubservice/src/ServiceComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
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
#include "areg/logging/GELog.h"

#include <iostream>

DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_startupServiceInterface );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestIdentifier );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestHelloUblock );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_processTimer );

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
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_startupServiceInterface );

    HelloUnblockStub::startupServiceInterface( holder );
    setHelloServiceState( NEHelloUnblock::eServiceState::ServiceActive );
    LOG_DBG( "The service [ %s ] is up and running", getRoleName( ).getString( ) );
}

void ServiceComponent::requestIdentifier( void )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestIdentifier );
    uint32_t clientId = NEUtilities::generateUniqueId( );
    LOG_DBG( "Generated ID for the clinet: %u ", clientId );
    responseIdentifier( clientId );
}

void ServiceComponent::requestHelloUblock( unsigned int clientId, unsigned int seqNr )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestHelloUblock );

    ASSERT( clientId != NEHelloUnblock::InvalidId );

    String timestamp( DateTime::getNow( ).formatTime( ) );
    uint32_t sessionId = unblockCurrentRequest( );

    LOG_DBG( "Received request: client [ %u ], sequence Nr [ %u ], unlock session [ %u ]", clientId, seqNr, sessionId );
    if ( mSessionList.isEmpty() )
    {
        ASSERT( mTimer.isActive( ) == false );
        LOG_DBG( "First request with valid ID, trigger timer to send response every [ %u ] ms", NEHelloUnblock::ServiceTimeout );
        mTimer.startTimer( NEHelloUnblock::ServiceTimeout, Timer::CONTINUOUSLY );
    }

    mSessionList.pushLast( SessionEtnry{ clientId, seqNr, sessionId } );
    std::cout << ">>> Request at [ " << timestamp << " ]: "
              << " Client = " << clientId << ", "
              << " Sequence = " << seqNr << ", "
              << " Session = " << sessionId << std::endl;
}

void ServiceComponent::processTimer( Timer & /* timer */ )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_processTimer );
    SessionEtnry entry = mSessionList.popFirst( );

    LOG_DBG( "Prepared session [ %u ] to send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );

    if ( prepareResponse( entry.id ) )
    {
        String timestamp( DateTime::getNow( ).formatTime( ) );
        responseHelloUnblock( entry.clientId, entry.seqNr );
        LOG_DBG( "Succeeded to send response to client [ %u ], sequence [ %u ]", entry.clientId, entry.seqNr );

        std::cout << "<<< Response at [ " << timestamp << " ]: "
                  << " Client = " << entry.clientId
                  << " Sequence = " << entry.seqNr << std::endl;
    }
    else
    {
        ASSERT( false );
        LOG_ERR( "There is no session [ %u ], cannot send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );
    }

    if ( mSessionList.isEmpty( ) )
    {
        mTimer.stopTimer( );
        setHelloServiceState( NEHelloUnblock::eServiceState::ServiceShutdown );

        LOG_WARN( "No more saved sessions in the list, quit application!" );
        Application::signalAppQuit( );
    }
}
