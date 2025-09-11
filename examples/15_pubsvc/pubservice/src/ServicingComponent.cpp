/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/logging/GELog.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestHelloWorld);
DEF_LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestShutdownService);

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component     ( entry, owner )
    , HelloWorldStub( static_cast<Component &>(self()) )
    , mClientList   ( )
    , mRemainRequest( NEHelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const String & roleName)
{
    LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestHelloWorld);

    NEHelloWorld::sConnectedClient theClient;
    ClientList::LISTPOS pos = mClientList.firstPosition();
    for ( ; mClientList.isValidPosition(pos); pos = mClientList.nextPosition(pos))
    {
        const NEHelloWorld::sConnectedClient & client = mClientList.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            LOG_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);
            theClient = client;
            break;
        }
    }

    if ( mClientList.isInvalidPosition(pos))
    {
        theClient = NEHelloWorld::sConnectedClient( NEUtilities::generateUniqueId(), roleName );
        mClientList.pushFirst( theClient );
        LOG_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.getString( ), theClient.ccID );
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
        LOG_INFO( "Reached maximum to output messages, this should trigger the shutdown procedure." );
        broadcastReachedMaximum( NEHelloWorld::MaxMessages );
    }
    else
    {
        LOG_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

#if AREG_LOGS
void ServicingComponent::requestShutdownService(unsigned int clientID, const String & roleName)
{
    LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestShutdownService);
    LOG_DBG("A client [ %s ] with ID [ %u ] requests to shut down.", roleName.getString(), clientID);
    Application::signalAppQuit( );
}
#else   // AREG_LOGS
void ServicingComponent::requestShutdownService(unsigned int /*clientID*/, const String & /*roleName*/)
{
    Application::signalAppQuit( );
}
#endif  // AREG_LOGS
