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
#include "areg/logging/areg_log.h"

#include <iostream>

DEF_LOG_SCOPE(examples_24_pubservice_ServiceComponent, startup_service_interface);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceComponent, request_identifier);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceComponent, request_hello_ublock);
DEF_LOG_SCOPE(examples_24_pubservice_ServiceComponent, process_timer);

ServiceComponent::ServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component         ( entry, owner )
    , HelloUnblockProviderBase  ( static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )

    , mSessionList      ( )
    , mTimer            ( static_cast<areg::TimerConsumer &>(self()), entry.mRoleName )
{
    set_hello_service_state( HelloUnblock::RunState::ServiceUndefined );
}

void ServiceComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent, startup_service_interface );

    HelloUnblockProviderBase::startup_service_interface( holder );
    set_hello_service_state( HelloUnblock::RunState::ServiceActive );
    LOG_DBG( "The service [ %s ] is up and running", role_name( ).as_string( ) );
}

void ServiceComponent::request_identifier()
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent, request_identifier );
    uint32_t clientId = areg::generate_unique_id( );
    LOG_DBG( "Generated ID for the client: %u ", clientId );
    response_identifier( clientId );
}

void ServiceComponent::request_hello_ublock( uint32_t clientId, uint32_t seqNr )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent, request_hello_ublock );

    ASSERT( clientId != HelloUnblock::InvalidId );

    areg::String timestamp( areg::DateTime::now( ).format_time( ) );
    uint32_t sessionId = unblock_current_request( );

    LOG_DBG( "Received request: client [ %u ], sequence Nr [ %u ], unlock session [ %u ]", clientId, seqNr, sessionId );
    if ( mSessionList.is_empty() )
    {
        ASSERT( mTimer.is_active( ) == false );
        LOG_DBG( "First request with valid ID, trigger timer to send response every [ %u ] ms", HelloUnblock::ServiceTimeout );
        mTimer.start_timer( HelloUnblock::ServiceTimeout, areg::Timer::CONTINUOUSLY );
    }

    mSessionList.push_last( SessionEtnry{ clientId, seqNr, sessionId } );
    std::cout << ">>> Request at [ " << timestamp << " ]: "
              << " Client = " << clientId << ", "
              << " Sequence = " << seqNr << ", "
              << " Session = " << sessionId << std::endl;
}

void ServiceComponent::process_timer( areg::Timer & /* timer */ )
{
    LOG_SCOPE( examples_24_pubservice_ServiceComponent, process_timer );
    SessionEtnry entry = mSessionList.pop_first( );

    LOG_DBG( "Prepared session [ %u ] to send response to client [ %u ], sequence [ %u ]", entry.id, entry.clientId, entry.seqNr );

    if ( prepare_response( entry.id ) )
    {
        areg::String timestamp( areg::DateTime::now( ).format_time( ) );
        response_hello_unblock( entry.clientId, entry.seqNr );
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

    if ( mSessionList.is_empty( ) )
    {
        mTimer.stop_timer( );
        set_hello_service_state( HelloUnblock::RunState::Shutdown );

        LOG_WARN( "No more saved sessions in the list, quit application!" );
        areg::Application::signal_quit( );
    }
}
