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

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/logging/areg_log.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>

DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent_startupServiceInterface);
DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent_requestStartSleep);
DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent_requestStopService);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component         ( entry, owner )
    , HelloWatchdogStub ( static_cast<areg::Component &>(self()) )
{
}

void ServicingComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE(examples_22_pubservice_ServicingComponent_startupServiceInterface);
    printf("-------------------------------------\n");
    printf("Start service [ %s ] with role [ %s ]\n", HelloWatchdogStub::service_name().as_string(), role_name().as_string());

    HelloWatchdogStub::startup_service_interface(holder);
    setServiceState(HelloWatchdog::ComponentState::Initialized);
}

void ServicingComponent::requestStartSleep( uint32_t timeoutSleep )
{
    LOG_SCOPE(examples_22_pubservice_ServicingComponent_requestStartSleep);

    LOG_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, HelloWatchdog::TimeoutWatchdog);

    if (getServiceState() != HelloWatchdog::ComponentState::Stopped )
    {
        printf( "Hello Watchdog! Sleep [ %u ] ms, watchdog timeout [ %u ]\n", timeoutSleep, HelloWatchdog::TimeoutWatchdog );
        setServiceState( HelloWatchdog::ComponentState::Started );
        areg::Thread::sleep( timeoutSleep );
        responseStartSleep( timeoutSleep );
    }
    else
    {
        LOG_DBG("Ignoring request to sleep, the service is in stopped state");
        responseStartSleep( 0 );
    }
}

void ServicingComponent::requestStopService()
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent_requestStopService );
    LOG_DBG("Received request to stop service");
    printf("Requested to stop the service.\n");
    setServiceState( HelloWatchdog::ComponentState::Stopped );
}

void ServicingComponent::requestShutdownService()
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent_requestStopService );
    LOG_DBG("Shutdown the service");
    printf( "Shutdown the service and quit application.\n" );
    setServiceState( HelloWatchdog::ComponentState::Stopped );
    areg::Application::signal_quit();
}
