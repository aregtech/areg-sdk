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
#include "areg/component/ExitEvent.hpp"

// Linux-specific includes are in linux/TimerManagerBaseLinux.cpp.

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(TimerManagerBase, DispatcherThread)

TimerManagerBase::TimerManagerBase(const String& threadName, uint32_t stackSizeKb)
    : DispatcherThread          (threadName, stackSizeKb, areg::QUEUE_SIZE_MAXIMUM)
    , TimerManagerEventConsumer ( )
#ifdef __linux__
    , mEpollFd  (-1)
    , mCommandFd(-1)
    , mExitFd   (-1)
#endif  // __linux__
{
}

#ifndef __linux__

bool TimerManagerBase::post_event(Event& eventElem)
{
    return (eventElem.event_id() == TimerManagerEvent::CLASS_ID) && EventDispatcher::post_event(eventElem);
}

bool TimerManagerBase::run_dispatcher()
{
    ready_for_events(true);

    SyncEvent* events[2] { &mEventExit, &mEventQueue };
    int32_t whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);

    do
    {
        whichEvent = SyncEvent::wait_any(events, 2, areg::WAIT_INFINITE);
        if (whichEvent != static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue))
        {
            break;
        }

        // process all queued timer events without
        // re-entering the kernel wait between consecutive events.
        for (;;)
        {
            Event eventElem = pick_event();

            if (eventElem.is_exit_prio())
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                break;
            }

            if (!eventElem.is_valid())
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue);
                break;
            }

            if (prepare_dispatch_event(eventElem))
            {
                dispatch_event(eventElem);
            }

            post_dispatch_event(eventElem);

            ASSERT(static_cast<EventQueue&>(mInternalEvents).is_empty());
        }

    } while (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue));

    ready_for_events(false);
    remove_all_events();

    ASSERT(static_cast<EventQueue&>(mInternalEvents).is_empty());

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
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

#endif  // !__linux__

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

    DispatcherThread::ready_for_events(is_ready);
}

bool TimerManagerBase::start_manager_thread()
{
    return (is_ready() || (start(areg::DO_NOT_WAIT) && wait_start(areg::WAIT_INFINITE)));
}

bool TimerManagerBase::wait_completion(uint32_t waitForCompleteMs /*= areg::WAIT_INFINITE*/)
{
    return (shutdown(waitForCompleteMs) == Thread::ThreadCompletion::Completed);
}

} // namespace areg
