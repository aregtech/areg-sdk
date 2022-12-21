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
    return DEBUG_NEW ServicingComponent( entry, owner, entry.getComponentData());
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

DEF_TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_processTimer);

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & ownerThread, NEMemory::uAlign OPT data)
    : Component ( entry, ownerThread )
    , StubBase  ( self(), NEService::getEmptyInterface() )

    , mTimer    ( self(), entry.mRoleName )
    , mCount    ( 0 )
{
}

void ServicingComponent::startupServiceInterface(Component & holder)
{
    TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_startupServiceInterface);
    TRACE_INFO("The service [ %s ] of component [ %s ] has been started", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());

    StubBase::startupServiceInterface(holder);
    mTimer.startTimer(TIMER_TIMEOUT, TIMER_EVENTS);

    printf("Local servicing started, waits for [ %d ] ms to stop and exit application...\n", TIMER_TIMEOUT * TIMER_EVENTS);
}

void ServicingComponent::shutdownServiceIntrface(Component & holder)
{
    TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_shutdownServiceIntrface);
    TRACE_WARN("The service [ %s ] of component [ %s ] is shutting down", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());

    mTimer.stopTimer();
    StubBase::shutdownServiceIntrface(holder);

    std::cout << "Local servicing stopped..." << std::endl;
}

void ServicingComponent::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_09_svcmulti_ServicingComponent_processTimer);
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
    	ASSERT(TIMER_EVENTS > mCount);
        printf( "Hello from Service [ %s ]!\n", getRoleName( ).getString());
    }
    else
    {
        std::cout << "Goodbye Service!" << std::endl;
        ASSERT(mCount == TIMER_EVENTS);

        TRACE_INFO("The timer is not active anymore, signaling quit event");
        Application::signalAppQuit();
    }
}
