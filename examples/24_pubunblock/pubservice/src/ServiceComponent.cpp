/************************************************************************
 * \file        pubservice/src/ServiceComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is demonstration of manual unlocking of the request.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/ServiceComponent.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/logging/GELog.h"

#include <iostream>

DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_startupServiceInterface );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestIdentifier );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestHelloUblock );
DEF_LOG_SCOPE( examples_24_pubservice_ServiceComponent_processTimer );

ServiceComponent::ServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , HelloUnblockStub  ( static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )

    , mSessionList      ( )
    , mTimer            ( static_cast<areg::TimerConsumer &>(self()), entry.mRoleName )
{
    setHelloServiceState( HelloUnblock::RunState::ServiceUndefined );
}

void ServiceComponent::startupServiceInterface( areg::Component & holder )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_startupServiceInterface );

    HelloUnblockStub::startupServiceInterface( holder );
    setHelloServiceState( HelloUnblock::RunState::ServiceActive );
    LOG_DBG( "The service [ %s ] is up and running", getRoleName( ).getString( ) );
}

void ServiceComponent::requestIdentifier()
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestIdentifier );
    uint32_t clientId = areg::generateUniqueId( );
    LOG_DBG( "Generated ID for the clinet: %u ", clientId );
    responseIdentifier( clientId );
}

void ServiceComponent::requestHelloUblock( uint32_t clientId, uint32_t seqNr )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_requestHelloUblock );

    ASSERT( clientId != HelloUnblock::InvalidId );

    areg::String timestamp( areg::DateTime::getNow( ).formatTime( ) );
    uint32_t sessionId = unblockCurrentRequest( );

    LOG_DBG( "Received request: client [ %u ], sequence Nr [ %u ], unlock session [ %u ]", clientId, seqNr, sessionId );
    if ( mSessionList.isEmpty() )
    {
        ASSERT( mTimer.isActive( ) == false );
        LOG_DBG( "First request with valid ID, trigger timer to send response every [ %u ] ms", HelloUnblock::ServiceTimeout );
        mTimer.startTimer( HelloUnblock::ServiceTimeout, areg::Timer::CONTINUOUSLY );
    }

    mSessionList.pushLast( SessionEtnry{ clientId, seqNr, sessionId } );
    std::cout << ">>> Request at [ " << timestamp << " ]: "
              << " Client = " << clientId << ", "
              << " Sequence = " << seqNr << ", "
              << " Session = " << sessionId << std::endl;
}

void ServiceComponent::processTimer( areg::Timer & /* timer */ )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent_processTimer );
    SessionEtnry entry = mSessionList.popFirst( );

    LOG_DBG( "Prepared session [ %u ] to send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );

    if ( prepareResponse( entry.id ) )
    {
        areg::String timestamp( areg::DateTime::getNow( ).formatTime( ) );
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
        setHelloServiceState( HelloUnblock::RunState::Shutdown );

        LOG_WARN( "No more saved sessions in the list, quit application!" );
        areg::Application::signalAppQuit( );
    }
}
