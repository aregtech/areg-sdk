/************************************************************************
 * \file        locservice/src/ServicingComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "locservice/src/ServicingComponent.hpp"
#include "areg/logging/GELog.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>


DEF_LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_startupServiceInterface);
DEF_LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_requestStartSleep);

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , HelloWatchdogStub ( static_cast<Component &>(self()) )
{
}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_startupServiceInterface);
    printf("-------------------------------------\n");
    printf("Start service [ %s ] with role [ %s ]\n", HelloWatchdogStub::getServiceName().getString(), getRoleName().getString());

    HelloWatchdogStub::startupServiceInterface(holder);
    setServiceState(NEHelloWatchdog::eState::Initialized);
}

void ServicingComponent::requestStartSleep( unsigned int timeoutSleep )
{
    LOG_SCOPE(examples_21_locwatchdog_ServicingComponent_requestStartSleep);

    LOG_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog);
    printf("Hello Watchdog! Sleep [ %u ] ms, watchdog timeout [ %u ]\n", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog);

    setServiceState( NEHelloWatchdog::eState::Started );

    Thread::sleep(timeoutSleep);

    responseStartSleep(timeoutSleep);
}
