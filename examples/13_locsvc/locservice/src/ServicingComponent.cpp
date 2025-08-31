/************************************************************************
 * \file        locservice/src/ServicingComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locservice/src/ServicingComponent.hpp"
#include "areg/logging/GELog.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_requestHelloWorld);
DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_requestShutdownService);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & /* entry */)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component     ( entry, owner )
    , HelloWorldStub( static_cast<Component &>(self()) )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const String & roleName)
{
    LOG_SCOPE(examples_13_locservice_ServicingComponent_requestHelloWorld);
    
    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

    responseHelloWorld( );
    if ( mRemainRequest == 0 )
    {
        LOG_INFO( "Reached maximum to output messages, this should trigger the shutdown procedure." );
        broadcastReachedMaximum( NEHelloWorld::MaxMessages );
    }
    else
    {
        LOG_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

void ServicingComponent::requestShutdownService( void )
{
    LOG_SCOPE(examples_13_locservice_ServicingComponent_requestShutdownService);
    LOG_DBG("The local client requests to shut down.");
    Application::signalAppQuit( );
}
