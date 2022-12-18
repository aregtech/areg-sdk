/************************************************************************
 * \file        pubservice/src/PublicServiceComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
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
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( examples_13_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
DEF_TRACE_SCOPE( examples_13_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );

Component * PublicServiceComponent::CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
{
    return DEBUG_NEW PublicServiceComponent(entry, owner);
}

void PublicServiceComponent::DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
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

void PublicServiceComponent::requestHelloWorld( unsigned int clientID )
{
    TRACE_SCOPE( examples_13_pubmesh_pubservice_PublicServiceComponent_requestHelloWorld );
    PublicHelloWorldService::requestHelloWorld( clientID );

    if ( mNumMessages == NEPublicHelloWorld::MaximumOutputs )
    {
        TRACE_WARN( "Reached maximum outputs [ %d ], preparing to shutdown", mNumMessages );
        // Notify the service unavailable state, so that the clients stop sending requests
        SystemShutdownStub::setServiceState( NESystemShutdown::eServiceState::ServiceShutdown );
    }
}

void PublicServiceComponent::requestSystemShutdown( void )
{
    TRACE_SCOPE( examples_13_pubmesh_pubservice_PublicServiceComponent_requestSystemShutdown );
    TRACE_WARN( "Processing the request to shutdown the system!" );
    Application::signalAppQuit( );
}
