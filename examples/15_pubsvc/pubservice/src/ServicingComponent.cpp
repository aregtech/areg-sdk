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
#include "areg/logging/areg_log.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_LOG_SCOPE(examples_15_pubservice_ServicingComponent, request_hello_world);
DEF_LOG_SCOPE(examples_15_pubservice_ServicingComponent, request_shutdown_service);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component     ( entry, owner )
    , HelloWorldProviderBase( static_cast<areg::Component &>(self()) )
    , mClientList   ( )
    , mRemainRequest( HelloWorld::MaxMessages )
{
}

void ServicingComponent::request_hello_world(const areg::String & roleName)
{
    LOG_SCOPE( examples_15_pubservice_ServicingComponent, request_hello_world );

    HelloWorld::sConnectedClient theClient;
    ClientList::LISTPOS pos = mClientList.first_position();
    for ( ; mClientList.is_valid_position(pos); pos = mClientList.next_position(pos))
    {
        const HelloWorld::sConnectedClient & client = mClientList.value_at(pos);
        if (roleName == client.ccName)
        {
            LOG_DBG("Found connected client [ %s ] with ID [ %u ] in the list.", client.ccName.as_string(), client.ccID);
            theClient = client;
            break;
        }
    }

    if (!mClientList.is_valid_position(pos))
    {
        theClient = HelloWorld::sConnectedClient( areg::generate_unique_id(), roleName );
        mClientList.push_first( theClient );
        LOG_INFO( "The new client component [ %s ] with ID [ %u ] sent a request", roleName.as_string( ), theClient.ccID );
    }

    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

    response_hello_world( theClient );
    if ( mRemainRequest == 0 )
    {
        LOG_INFO( "Reached maximum to output messages, this should trigger the shutdown procedure." );
        broadcast_reached_maximum( HelloWorld::MaxMessages );
    }
    else
    {
        LOG_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

#if AREG_LOGGING
void ServicingComponent::request_shutdown_service(uint32_t clientID, const areg::String & roleName)
{
    LOG_SCOPE( examples_15_pubservice_ServicingComponent, request_shutdown_service );
    LOG_DBG("A client [ %s ] with ID [ %u ] requests to shut down.", roleName.as_string(), clientID);
    areg::Application::signal_quit( );
}
#else   // AREG_LOGGING
void ServicingComponent::request_shutdown_service(uint32_t /*clientID*/, const areg::String & /*roleName*/)
{
    areg::Application::signal_quit( );
}
#endif  // AREG_LOGGING
