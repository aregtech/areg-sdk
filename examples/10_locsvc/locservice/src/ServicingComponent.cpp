/************************************************************************
 * \file        locservice/src/ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locservice/src/ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestShutdownService);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
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
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestHelloWorld);
    
    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

    responseHelloWorld( );
    if ( mRemainRequest == 0 )
    {
        TRACE_INFO( "Reached maximum to output messages, this should trigger the shutdown procedure." );
        broadcastReachedMaximum( NEHelloWorld::MaxMessages );
    }
    else
    {
        TRACE_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

void ServicingComponent::requestShutdownService( void )
{
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestShutdownService);
    TRACE_DBG("The local client requests to shut down.");
    Application::signalAppQuit( );
}
