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


DEF_LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_startupServiceInterface);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_request_start_sleep);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component         ( entry, owner )
    , HelloWatchdogProviderBase ( static_cast<areg::Component &>(self()) )
{
}

void ServicingComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_startupServiceInterface);
    printf("-------------------------------------\n");
    printf("Start service [ %s ] with role [ %s ]\n", HelloWatchdogProviderBase::service_name().as_string(), role_name().as_string());

    HelloWatchdogProviderBase::startup_service_interface(holder);
    set_service_state(HelloWatchdog::ComponentState::Initialized);
}

void ServicingComponent::request_start_sleep( uint32_t timeoutSleep )
{
    LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_request_start_sleep);

    LOG_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, HelloWatchdog::TimeoutWatchdog);
    printf("Hello Watchdog! Sleep [ %u ] ms, watchdog timeout [ %u ]\n", timeoutSleep, HelloWatchdog::TimeoutWatchdog);

    set_service_state( HelloWatchdog::ComponentState::Started );

    areg::Thread::sleep(timeoutSleep);

    response_start_sleep(timeoutSleep);
}
