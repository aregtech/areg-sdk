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

DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent, startup_service_interface);
DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent, request_start_sleep);
DEF_LOG_SCOPE(examples_22_pubservice_ServicingComponent, request_stop_service);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component         ( entry, owner )
    , HelloWatchdogProviderBase ( static_cast<areg::Component &>(self()) )
{
}

void ServicingComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent, startup_service_interface );
    printf("-------------------------------------\n");
    printf("Start service [ %s ] with role [ %s ]\n", HelloWatchdogProviderBase::service_name().as_string(), role_name().as_string());

    HelloWatchdogProviderBase::startup_service_interface(holder);
    set_service_state(HelloWatchdog::ComponentState::Initialized);
}

void ServicingComponent::request_start_sleep( uint32_t timeoutSleep )
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent, request_start_sleep );

    LOG_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, HelloWatchdog::TimeoutWatchdog);

    if (service_state() != HelloWatchdog::ComponentState::Stopped )
    {
        printf( "Hello Watchdog! Sleep [ %u ] ms, watchdog timeout [ %u ]\n", timeoutSleep, HelloWatchdog::TimeoutWatchdog );
        set_service_state( HelloWatchdog::ComponentState::Started );
        areg::Thread::sleep( timeoutSleep );
        response_start_sleep( timeoutSleep );
    }
    else
    {
        LOG_DBG("Ignoring request to sleep, the service is in stopped state");
        response_start_sleep( 0 );
    }
}

void ServicingComponent::request_stop_service()
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent, request_stop_service );
    LOG_DBG("Received request to stop service");
    printf("Requested to stop the service.\n");
    set_service_state( HelloWatchdog::ComponentState::Stopped );
}

void ServicingComponent::request_shutdown_service()
{
    LOG_SCOPE( examples_22_pubservice_ServicingComponent, request_stop_service );
    LOG_DBG("Shutdown the service");
    printf( "Shutdown the service and quit application.\n" );
    set_service_state( HelloWatchdog::ComponentState::Stopped );
    areg::Application::signal_quit();
}
