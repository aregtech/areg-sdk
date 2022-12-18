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

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <stdlib.h>

DEF_TRACE_SCOPE(examples_19_pubservice_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_19_pubservice_ServicingComponent_requestStartSleep);
DEF_TRACE_SCOPE(examples_19_pubservice_ServicingComponent_requestStopService);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , HelloWatchdogStub ( static_cast<Component &>(self()) )
{
}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(examples_19_pubservice_ServicingComponent_startupServiceInterface);
    printf("-------------------------------------\n");
    printf("Start service [ %s ] with role [ %s ]\n", HelloWatchdogStub::getServiceName().getString(), getRoleName().getString());

    HelloWatchdogStub::startupServiceInterface(holder);
    setServiceState(NEHelloWatchdog::eState::Initialized);
}

void ServicingComponent::requestStartSleep( unsigned int timeoutSleep )
{
    TRACE_SCOPE(examples_19_pubservice_ServicingComponent_requestStartSleep);

    TRACE_DBG("Received request to sleep [ %u ] ms, the watchdog timeout is [ %u ]", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog);

    if (getServiceState() != NEHelloWatchdog::eState::Stopped )
    {
        printf( "Hello Watchdog! Sleep [ %u ] ms, watchdog timeout [ %u ]\n", timeoutSleep, NEHelloWatchdog::TimeoutWatchdog );
        setServiceState( NEHelloWatchdog::eState::Started );
        Thread::sleep( timeoutSleep );
        responseStartSleep( timeoutSleep );
    }
    else
    {
        TRACE_DBG("Ignoring request to sleep, the service is in stopped state");
        responseStartSleep( 0 );
    }
}

void ServicingComponent::requestStopService( void )
{
    TRACE_SCOPE( examples_19_pubservice_ServicingComponent_requestStopService );
    TRACE_DBG("Received request to stop service");
    printf("Requested to stop the service.\n");
    setServiceState( NEHelloWatchdog::eState::Stopped );
}

void ServicingComponent::requestShutdownService( void )
{
    TRACE_SCOPE( examples_19_pubservice_ServicingComponent_requestStopService );
    TRACE_DBG("Shutdown the service");
    printf( "Shutdown the service and quit application.\n" );
    setServiceState( NEHelloWatchdog::eState::Stopped );
    Application::signalAppQuit();
}
