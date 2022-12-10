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
    : Component     ( owner, entry.mRoleName)
    , HelloWorldStub( static_cast<Component &>(self()) )
    , mGnerateID    ( 0 )
    , mClientList   ( )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const String & roleName)
{
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestHelloWorld);
    
    ClientList::LISTPOS pos = mClientList.firstPosition();
    for ( ; mClientList.isValidPosition(pos); pos = mClientList.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = mClientList.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            TRACE_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);
            break;
        }
    }

    if ( mClientList.isInvalidPosition(pos))
    {
        responseHelloWorld( NEHelloWorld::sConnectedClient{ ++mGnerateID, roleName} );
        TRACE_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.getString( ), mGnerateID );
    }
    else
    {
        responseHelloWorld( mClientList.valueAtPosition( pos ) );
    }

    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

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
    TRACE_SCOPE(examples_10_locservice_ServicingComponent_requestShutdownService);
    TRACE_DBG("A client [ %s ] with ID [ %u ] requests to shut down.", roleName.getString(), clientID);
    Application::signalAppQuit( );
}
