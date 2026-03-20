/************************************************************************
 * \file        locservice/src/ServicingComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locservice/src/ServicingComponent.hpp"
#include "areg/logging/areg_log.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_request_hello_world);
DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_request_shutdown_service);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component     ( entry, owner )
    , HelloWorldProviderBase( static_cast<areg::Component &>(self()) )
    , mRemainRequest( HelloWorld::MaxMessages )
{
}

void ServicingComponent::request_hello_world(const areg::String & roleName)
{
    LOG_SCOPE(examples_13_locservice_ServicingComponent_request_hello_world);
    
    std::cout
        << "\"Hello client [ "
        << roleName
        << " ]!\", remain to process [ "
        << --mRemainRequest
        << " ]" << std::endl;

    response_hello_world( );
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

void ServicingComponent::request_shutdown_service()
{
    LOG_SCOPE(examples_13_locservice_ServicingComponent_request_shutdown_service);
    LOG_DBG("The local client requests to shut down.");
    areg::Application::signal_quit( );
}
