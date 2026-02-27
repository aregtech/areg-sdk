/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
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

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component     ( entry, owner )
    , HelloWorldStub( static_cast<areg::Component &>(self()) )
    , mClientList   ( )
    , mRemainRequest( HelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const areg::String & roleName)
{
    LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestHelloWorld);

    HelloWorld::sConnectedClient theClient;
    areg::ClientList::LISTPOS pos = mClientList.firstPosition();
    for ( ; mClientList.isValidPosition(pos); pos = mClientList.nextPosition(pos))
    {
        const HelloWorld::sConnectedClient & client = mClientList.valueAtPosition(pos);
        if (roleName == client.ccName)
        {
            LOG_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.getString(), client.ccID);
            theClient = client;
            break;
        }
    }

    if ( mClientList.isInvalidPosition(pos))
    {
        theClient = HelloWorld::sConnectedClient( areg::generateUniqueId(), roleName );
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
        broadcastReachedMaximum( HelloWorld::MaxMessages );
    }
    else
    {
        LOG_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

#if AREG_LOGS
void ServicingComponent::requestShutdownService(uint32_t clientID, const areg::String & roleName)
{
    LOG_SCOPE(examples_15_pubservice_ServicingComponent_requestShutdownService);
    LOG_DBG("A client [ %s ] with ID [ %u ] requests to shut down.", roleName.getString(), clientID);
    areg::Application::signalAppQuit( );
}
#else   // AREG_LOGS
void ServicingComponent::requestShutdownService(uint32_t /*clientID*/, const areg::String & /*roleName*/)
{
    areg::Application::signalAppQuit( );
}
#endif  // AREG_LOGS
