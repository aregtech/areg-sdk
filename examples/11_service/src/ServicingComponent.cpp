/************************************************************************
 * \file        src/ServicingComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "src/ServicingComponent.hpp"
#include "areg/logging/areg_log.h"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

DEF_LOG_SCOPE(examples_11_service_ServicingComponent_startupServiceInterface);
DEF_LOG_SCOPE(examples_11_service_ServicingComponent_shutdownServiceIntrface);
DEF_LOG_SCOPE(examples_11_service_ServicingComponent_processTimer);

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & ownerThread)
    : areg::Component ( entry, ownerThread )
    , areg::StubBase  ( self(), areg::empty_interface() )

    , mTimer    ( self(), "ServicingTimer" )
    , mCount    ( 0 )
{
}

void ServicingComponent::startup_service_interface(areg::Component & holder)
{
    LOG_SCOPE(examples_11_service_ServicingComponent_startupServiceInterface);
    LOG_INFO("The service [ %s ] of component [ %s ] has been started", areg::StubBase::address().service_name().as_string(), holder.role_name().as_string());

    areg::StubBase::startup_service_interface(holder);
    mTimer.start_timer(TIMER_TIMEOUT, TIMER_EVENTS);

    printf("Local servicing started, waits for [ %u ] ms to stop and exit application...\n", TIMER_TIMEOUT * TIMER_EVENTS);
}

void ServicingComponent::shutdown_service_interface(areg::Component & holder)
{
    LOG_SCOPE(examples_11_service_ServicingComponent_shutdownServiceIntrface);
    LOG_WARN("The service [ %s ] of component [ %s ] is shutting down", areg::StubBase::address().service_name().as_string(), holder.role_name().as_string());

    mTimer.stop_timer();
    areg::StubBase::shutdown_service_interface(holder);

    std::cout << "Local servicing stopped..." << std::endl;
}

void ServicingComponent::process_timer(areg::Timer & timer)
{
    LOG_SCOPE(examples_11_service_ServicingComponent_processTimer);
    LOG_DBG("The timer [ %s ] has expired", timer.name().as_string());

    ASSERT(&timer == &mTimer);

    ++ mCount; // increase timer count.
    ASSERT(mCount <= TIMER_EVENTS);

    LOG_DBG("Timer timeout [ %u ] ms, the timer state [ %s ], triggered [ %d ] times, remain [ %d ] times before complete"
                , timer.timeout()
                , timer.is_active() ? "ACTIVE" : "INACTIVE"
                , mCount
                , (TIMER_EVENTS - mCount));

    if (mTimer.is_active())
    {
    	ASSERT(TIMER_EVENTS > mCount);
        std::cout << "Hello Service!" << std::endl;
    }
    else
    {
        std::cout << "Goodbye Service!" << std::endl;
        ASSERT(mCount == TIMER_EVENTS);

        LOG_INFO("The timer is not active anymore, signaling quit event");
        areg::Application::signal_app_quit();
    }
}
