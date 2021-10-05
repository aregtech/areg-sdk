/************************************************************************
 * \file        src/ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "src/ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(owner, entry.mRoleName.getString(), entry.getComponentData());
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

DEF_TRACE_SCOPE(examples_08_service_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_08_service_ServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_08_service_ServicingComponent_processTimer);

ServicingComponent::ServicingComponent(ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data)
    : Component ( masterThread, roleName)
    , StubBase  ( self(), NEService::getEmptyInterface() )

    , mTimer    ( self(), "ServicingTimer" )
    , mCount    ( 0 )
{
}

void ServicingComponent::startupServiceInterface(Component & holder)
{
    TRACE_SCOPE(examples_08_service_ServicingComponent_startupServiceInterface);
    TRACE_INFO("The service [ %s ] of component [ %s ] has been started", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());

    StubBase::startupServiceInterface(holder);
    mTimer.startTimer(TIMER_TIMEOUT, TIMER_EVENTS);

    printf("Local servicing started, waits for [ %d ] ms to stop and exit application...\n", TIMER_TIMEOUT * TIMER_EVENTS);
}

void ServicingComponent::shutdownServiceIntrface(Component & holder)
{
    TRACE_SCOPE(examples_08_service_ServicingComponent_shutdownServiceIntrface);
    TRACE_WARN("The service [ %s ] of component [ %s ] is shutting down", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());
    
    mTimer.stopTimer();
    StubBase::shutdownServiceIntrface(holder);

    printf("Local servicing stopped...\n");
}

void ServicingComponent::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_08_service_ServicingComponent_processTimer);
    TRACE_DBG("The timer [ %s ] has expired", timer.getName().getString());

    ASSERT(&timer == &mTimer);
    
    ++ mCount; // increase timer count.
    ASSERT(mCount <= TIMER_EVENTS);

    TRACE_DBG("Timer timeout [ %u ] ms, the timer state [ %s ], triggered [ %d ] times, remain [ %d ] times before complete"
                , timer.getTimeout()
                , timer.isActive() ? "ACTIVE" : "INACTIVE"
                , mCount
                , (TIMER_EVENTS - mCount));

    if (mTimer.isActive())
    {
        printf("Hello Service!\n");
    }
    else
    {
        ASSERT(mCount == TIMER_EVENTS);

        printf("Goodbye Service...\n");

        TRACE_INFO("The timer is not active anymore, signaling quit event");
        Application::signalAppQuit();
    }
}

//////////////////////////////////////////////////////////////////////////
// These methods must exist, but can have empty body
//////////////////////////////////////////////////////////////////////////
void ServicingComponent::sendNotification(unsigned int msgId)
{
}

void ServicingComponent::errorRequest(unsigned int msgId, bool msgCancel)
{
}

void ServicingComponent::processRequestEvent(ServiceRequestEvent & eventElem)
{
}

void ServicingComponent::processAttributeEvent(ServiceRequestEvent & eventElem)
{
}
