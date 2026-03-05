/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager Base class.
 *
 ************************************************************************/
#include "areg/component/private/TimerManagerBase.hpp"
#include "areg/component/private/ExitEvent.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(TimerManagerBase, DispatcherThread)

TimerManagerBase::TimerManagerBase(const String& threadName)
    : DispatcherThread              (threadName, areg::STACK_SIZE_DEFAULT, areg::QUEUE_SIZE_MAXIMUM)
    , TimerManagerEventConsumer   ( )
{
}

bool TimerManagerBase::post_event(Event& eventElem)
{
    return (AREG_RUNTIME_CAST(&eventElem, TimerManagerEvent) != nullptr) && EventDispatcher::post_event(eventElem);
}

bool TimerManagerBase::run_dispatcher()
{
    ready_for_events( true );

    SyncObject* syncObjects[] = { &mEventExit, &mEventQueue };
    MultiLock multiLock(syncObjects, 2, false);
    int32_t whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
    const ExitEvent& exitEvent = ExitEvent::exit_event();

    do
    {
        whichEvent = multiLock.lock(areg::WAIT_INFINITE, false, true);
        Event* eventElem = whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) ? pick_event() : nullptr;
        if (static_cast<const Event*>(eventElem) != static_cast<const Event*>(&exitEvent))
        {
            if (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue))
            {
                // proceed one external event.
                if (prepare_dispatch_event(eventElem))
                {
                    dispatch_event(*eventElem);
                }

                post_dispatch_event(eventElem);

                ASSERT(static_cast<EventQueue&>(mInternalEvents).is_empty());
            }
        }
        else
        {
            whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::EventSignal::Queue) || (whichEvent == MultiLock::LOCK_INDEX_COMPLETION));

    ready_for_events(false);
    remove_all_events();

    ASSERT(static_cast<EventQueue&>(mInternalEvents).is_empty());

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

void TimerManagerBase::ready_for_events(bool is_ready)
{
    if (is_ready)
    {
        TimerManagerEvent::add_listener(static_cast<TimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
    }
    else
    {
        TimerManagerEvent::remove_listener(static_cast<TimerManagerEventConsumer&>(self()), static_cast<DispatcherThread&>(self()));
    }

    DispatcherThread::ready_for_events( true );
}

bool TimerManagerBase::start_manager_thread()
{
    ASSERT(is_ready() || (is_running() == false));
    return (is_ready() || (start(areg::WAIT_INFINITE) && wait_start(areg::WAIT_INFINITE)));
}

void TimerManagerBase::stop_manager_thread(bool waitComplete)
{
    if (waitComplete)
    {
        shutdown(areg::WAIT_INFINITE);
    }
    else
    {
        trigger_exit();
    }
}

void TimerManagerBase::wait_completion()
{
    shutdown(areg::WAIT_INFINITE);
}

} // namespace areg
