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


DEF_TRACE_SCOPE(examples_18_locwatchdog_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_18_locwatchdog_ServicingComponent_requestStartSleep);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName)
    , HelloWatchdogStub ( static_cast<Component &>(self()) )
{

}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_18_locwatchdog_ServicingComponent_startupServiceInterface);
    setServiceState(NEHelloWatchdog::eState::Initialized);
}

void ServicingComponent::requestStartSleep( unsigned int timeoutSleep )
{
    TRACE_SCOPE(examples_18_locwatchdog_ServicingComponent_requestStartSleep);

    TRACE_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog);
    printf("Hello Watchdog!!! Sleep [ %u ] ms, watchdog timeout [ %u ]", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog);

    setServiceState( NEHelloWatchdog::eState::Started );

    Thread::sleep(timeoutSleep);

    responseStartSleep(timeoutSleep);
}
