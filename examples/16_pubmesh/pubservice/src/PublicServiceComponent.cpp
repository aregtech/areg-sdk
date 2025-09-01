/************************************************************************
 * \file        pubservice/src/PublicServiceComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              to output message and shutdown.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "pubservice/src/PublicServiceComponent.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_clientConnected);
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
DEF_LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );

Component * PublicServiceComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW PublicServiceComponent(entry, owner);
}

void PublicServiceComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /* entry */ )
{
    delete (&compObject);
}

PublicServiceComponent::PublicServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component                 ( entry, owner )
    , SystemShutdownStub        ( static_cast<Component &>(self()) )
    , PublicHelloWorldService   ( static_cast<Component &>(self()) )
    , mLocalClient              ( entry.mDependencyServices[0], static_cast<Component &>(self()), PublicServiceComponent::LOCAL_TIMEOUT )
{
    // initially set invalid that the connected clients ignore the value.
    SystemShutdownStub::invalidateServiceState( );
}

void PublicServiceComponent::startupComponent( ComponentThread & comThread )
{
    Component::startupComponent( comThread );

    // Notify service is available and ready to operate.
    SystemShutdownStub::setServiceState( NESystemShutdown::eServiceState::ServiceReady );
}

bool PublicServiceComponent::clientConnected(const ProxyAddress & client, NEService::eServiceConnection status)
{
    LOG_SCOPE(examples_16_pubmesh_pubservice_PublicServiceComponent_clientConnected);
    LOG_INFO("The consumer [ %s ] is [ %s ]", ProxyAddress::convAddressToPath(client).getString(), NEService::getString(status));

    bool result{ true };
    if (SystemShutdownStub::clientConnected(client, status))
    {
        if (status == NEService::eServiceConnection::ServiceConnected)
        {
            if (SystemShutdownStub::isServiceStateValid() == false)
            {
                NESystemShutdown::eServiceState state = mNumMessages >= NEPublicHelloWorld::MaximumOutputs ?
                    NESystemShutdown::eServiceState::ServiceShutdown :
                    NESystemShutdown::eServiceState::ServiceReady;
                LOG_INFO("The service state is invalid, updating to the state [ %s ]", NESystemShutdown::getString(state));
                SystemShutdownStub::setServiceState(state);
            }
        }
    }
    else if (PublicHelloWorldService::clientConnected(client, status) == false)
    {
        LOG_WARN("Unexpected service consumer is connected!");
        result = false;
    }

    return result;
}

void PublicServiceComponent::requestHelloWorld( unsigned int clientID )
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
    PublicHelloWorldService::requestHelloWorld( clientID );

    if ( mNumMessages >= NEPublicHelloWorld::MaximumOutputs )
    {
        LOG_WARN( "Reached maximum outputs [ %d ], preparing to shutdown", mNumMessages );
        printf( ">>> Reached maximum outputs [ %d ] <<<\n", mNumMessages );
        // Notify the service unavailable state, so that the clients stop sending requests
        SystemShutdownStub::setServiceState( NESystemShutdown::eServiceState::ServiceShutdown );
    }
}

void PublicServiceComponent::requestSystemShutdown( void )
{
    LOG_SCOPE( examples_16_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );
    LOG_WARN( "No more service connected consumers. Processing the request to shutdown the system!" );
    printf("Processing the system shutdown!\n");
    Application::signalAppQuit();
    Thread::switchThread();
}
