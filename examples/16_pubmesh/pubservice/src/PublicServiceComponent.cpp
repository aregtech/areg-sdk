/************************************************************************
 * \file        pubservice/src/PublicServiceComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/PublicServiceComponent.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/logging/areg_log.h"

DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_client_connected);
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_request_hello_world );
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_request_system_shutdown );

PublicServiceComponent::PublicServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component                 ( entry, owner )
    , SystemShutdownProviderBase        ( static_cast<areg::Component &>(self()) )
    , PublicHelloWorldService   ( static_cast<areg::Component &>(self()) )
    , mLocalClient              ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), PublicServiceComponent::LOCAL_TIMEOUT )
{
    // initially set invalid that the connected clients ignore the value.
    SystemShutdownProviderBase::invalidate_service_state( );
}

void PublicServiceComponent::startup_component( areg::ComponentThread & comThread )
{
    areg::Component::startup_component( comThread );

    // Notify service is available and ready to operate.
    SystemShutdownProviderBase::set_service_state( SystemShutdown::RunState::ServiceReady );
}

bool PublicServiceComponent::client_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    LOG_SCOPE(examples_16_pubmesh_pubservice_PublicServiceComponent_client_connected);
    LOG_INFO("The consumer [ %s ] is [ %s ]", areg::ProxyAddress::to_path(client).as_string(), areg::as_string(status));

    bool result{ true };
    if (SystemShutdownProviderBase::client_connected(client, status))
    {
        if (status == areg::ServiceConnectionState::Connected)
        {
            if (SystemShutdownProviderBase::is_service_state_valid() == false)
            {
                SystemShutdown::RunState state = mNumMessages >= PublicHelloWorld::MaximumOutputs ?
                    SystemShutdown::RunState::Shutdown :
                    SystemShutdown::RunState::ServiceReady;
                LOG_INFO("The service state is invalid, updating to the state [ %s ]", SystemShutdown::as_string(state));
                SystemShutdownProviderBase::set_service_state(state);
            }
        }
    }
    else if (PublicHelloWorldService::client_connected(client, status) == false)
    {
        LOG_WARN("Unexpected service consumer is connected!");
        result = false;
    }

    return result;
}

void PublicServiceComponent::request_hello_world( uint32_t clientID )
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_request_hello_world );
    PublicHelloWorldService::request_hello_world( clientID );

    if ( mNumMessages >= PublicHelloWorld::MaximumOutputs )
    {
        LOG_WARN( "Reached maximum outputs [ %d ], preparing to shutdown", mNumMessages );
        printf( ">>> Reached maximum outputs [ %d ] <<<\n", mNumMessages );
        // Notify the service unavailable state, so that the clients stop sending requests
        SystemShutdownProviderBase::set_service_state( SystemShutdown::RunState::Shutdown );
    }
}

void PublicServiceComponent::request_system_shutdown()
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_request_system_shutdown );
    LOG_WARN( "No more service connected consumers. Processing the request to shutdown the system!" );
    printf("Processing the system shutdown!\n");
    areg::Application::signal_quit();
}
