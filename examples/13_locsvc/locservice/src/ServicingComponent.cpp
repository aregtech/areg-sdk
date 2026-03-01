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


DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_requestHelloWorld);
DEF_LOG_SCOPE(examples_13_locservice_ServicingComponent_requestShutdownService);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component     ( entry, owner )
    , HelloWorldStub( static_cast<areg::Component &>(self()) )
    , mRemainRequest( HelloWorld::MaxMessages )
{
}

void ServicingComponent::requestHelloWorld(const areg::String & roleName)
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
        broadcastReachedMaximum( HelloWorld::MaxMessages );
    }
    else
    {
        LOG_WARN("Still wait [ %d ] requests to print Hello World.", mRemainRequest);
    }
}

void ServicingComponent::requestShutdownService()
{
    LOG_SCOPE(examples_13_locservice_ServicingComponent_requestShutdownService);
    LOG_DBG("The local client requests to shut down.");
    areg::Application::signal_app_quit( );
}
