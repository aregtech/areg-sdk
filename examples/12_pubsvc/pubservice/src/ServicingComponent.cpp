/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_TRACE_SCOPE(examples_12_pubservice_ServicingComponent_requestHelloWorld);
DEF_TRACE_SCOPE(examples_12_pubservice_ServicingComponent_requestShutdownService);

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
    , mClientList   ( )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const String & roleName)
{
    TRACE_SCOPE(examples_12_pubservice_ServicingComponent_requestHelloWorld);

    NEHelloWorld::sConnectedClient theClient;
    ClientList::LISTPOS pos = mClientList.firstPosition();
    for ( ; mClientList.isValidPosition(pos); pos = mClientList.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = mClientList.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);
            theClient = client;
            break;
        }
    }

    if ( mClientList.isInvalidPosition(pos))
    {
        theClient = NEHelloWorld::sConnectedClient( NEUtilities::generateUniqueId(), roleName );
        mClientList.pushFirst( theClient );
        TRACE_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.getString( ), theClient.ccID );
    }

    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

    responseHelloWorld( theClient );
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

void ServicingComponent::requestShutdownService(unsigned int clientID, const String & roleName)
{
    TRACE_SCOPE(examples_12_pubservice_ServicingComponent_requestShutdownService);
    TRACE_DBG("A client [ %s ] with ID [ %u ] requests to shut down.", roleName.getString(), clientID);
    Application::signalAppQuit( );
}
