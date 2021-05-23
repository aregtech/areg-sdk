/************************************************************************
 * \file        ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "ServicingComponent.hpp"
#include "areg/trace/GETrace.h"
#include "areg/component/ComponentThread.hpp"

extern SynchEvent gExit; //!< The global event to quit application.

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(owner, entry.mRoleName.getString(), entry.getComponentData());
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

DEF_TRACE_SCOPE(examples_09_duplicate_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_09_duplicate_ServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_09_duplicate_ServicingComponent_startupComponent);
DEF_TRACE_SCOPE(examples_09_duplicate_ServicingComponent_shutdownComponent);
DEF_TRACE_SCOPE(examples_09_duplicate_ServicingComponent_processTimer);

ServicingComponent::ServicingComponent(ComponentThread & masterThread, const char * const roleName, NEMemory::uAlign OPTIONAL data)
    : Component ( masterThread, roleName)
    , StubBase  ( self(), NEService::getEmptyInterface() )

    , mTimer    ( self(), roleName )
    , mCount    ( 0 )
{

}

ServicingComponent::~ServicingComponent(void)
{
}

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

void ServicingComponent::startupServiceInterface(Component & holder)
{
    TRACE_SCOPE(examples_09_duplicate_ServicingComponent_startupServiceInterface);
    TRACE_INFO("The service [ %s ] of component [ %s ] has been started", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());

    StubBase::startupServiceInterface(holder);

    mTimer.startTimer(TIMER_TIMEOUT, TIMER_EVENTS);
}

void ServicingComponent::shutdownServiceIntrface(Component & holder)
{
    TRACE_SCOPE(examples_09_duplicate_ServicingComponent_shutdownServiceIntrface);
    TRACE_WARN("The service [ %s ] of component [ %s ] is shutting down", StubBase::getAddress().getServiceName().getString(), holder.getRoleName().getString());
    
    mTimer.stopTimer();
    StubBase::shutdownServiceIntrface(holder);
}

void ServicingComponent::startupComponent(ComponentThread & comThread)
{
    TRACE_SCOPE(examples_09_duplicate_ServicingComponent_startupComponent);
    TRACE_INFO("The component [ %s ] started in thread [ %s ], this starts services", Component::getRoleName().getString(), comThread.getName().getString());

    Component::startupComponent(comThread);
}

void ServicingComponent::shutdownComponent(ComponentThread & comThread)
{
    TRACE_SCOPE(examples_09_duplicate_ServicingComponent_shutdownComponent);
    TRACE_WARN("The component [ %s ] in thread [ %s ] is stopped, services unavailable", Component::getRoleName().getString(), comThread.getName().getString());

    Component::shutdownComponent( comThread );
}

void ServicingComponent::processTimer(Timer & timer)
{
    TRACE_SCOPE(examples_09_duplicate_ServicingComponent_processTimer);
    TRACE_DBG("The timer [ %s ] has expired", timer.getName().getString());

    ASSERT(&timer == &mTimer);
    
    ++ mCount; // increase timer count.
    ASSERT(mCount <= TIMER_EVENTS);

    TRACE_DBG("Timer timeout [ %u ] ms, the timer state [ %s ], triggered [ %d ] times, remain [ %d ] times before complete"
                , timer.getTimeout()
                , timer.isActive() ? "ACTIVE" : "INACTIVE"
                , mCount
                , (TIMER_EVENTS - mCount));

    if (mTimer.isActive() == false)
    {
        ASSERT(mCount == TIMER_EVENTS);

        TRACE_INFO("The timer is not actuve anymore, signaling quit event");
        gExit.setEvent();
    }
}
