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

DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_clientConnected);
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );

PublicServiceComponent::PublicServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component                 ( entry, owner )
    , SystemShutdownStub        ( static_cast<areg::Component &>(self()) )
    , PublicHelloWorldService   ( static_cast<areg::Component &>(self()) )
    , mLocalClient              ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), PublicServiceComponent::LOCAL_TIMEOUT )
{
    // initially set invalid that the connected clients ignore the value.
    SystemShutdownStub::invalidateServiceState( );
}

void PublicServiceComponent::startup_component( areg::ComponentThread & comThread )
{
    areg::Component::startup_component( comThread );

    // Notify service is available and ready to operate.
    SystemShutdownStub::setServiceState( SystemShutdown::RunState::ServiceReady );
}

bool PublicServiceComponent::client_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    LOG_SCOPE(examples_16_pubmesh_pubservice_PublicServiceComponent_clientConnected);
    LOG_INFO("The consumer [ %s ] is [ %s ]", areg::ProxyAddress::conv_address_to_path(client).as_string(), areg::as_string(status));

    bool result{ true };
    if (SystemShutdownStub::client_connected(client, status))
    {
        if (status == areg::ServiceConnectionState::Connected)
        {
            if (SystemShutdownStub::isServiceStateValid() == false)
            {
                SystemShutdown::RunState state = mNumMessages >= PublicHelloWorld::MaximumOutputs ?
                    SystemShutdown::RunState::Shutdown :
                    SystemShutdown::RunState::ServiceReady;
                LOG_INFO("The service state is invalid, updating to the state [ %s ]", SystemShutdown::as_string(state));
                SystemShutdownStub::setServiceState(state);
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

void PublicServiceComponent::requestHelloWorld( uint32_t clientID )
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
    PublicHelloWorldService::requestHelloWorld( clientID );

    if ( mNumMessages >= PublicHelloWorld::MaximumOutputs )
    {
        LOG_WARN( "Reached maximum outputs [ %d ], preparing to shutdown", mNumMessages );
        printf( ">>> Reached maximum outputs [ %d ] <<<\n", mNumMessages );
        // Notify the service unavailable state, so that the clients stop sending requests
        SystemShutdownStub::setServiceState( SystemShutdown::RunState::Shutdown );
    }
}

void PublicServiceComponent::requestSystemShutdown()
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );
    LOG_WARN( "No more service connected consumers. Processing the request to shutdown the system!" );
    printf("Processing the system shutdown!\n");
    areg::Application::signal_quit();
}
