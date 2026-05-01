/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerEventData.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Event Data class.
 *
 ************************************************************************/
#include "areg/component/private/TimerEventData.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// TimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(TimerEvent, areg::Event)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
TimerEvent::TimerEvent( const TimerEventData & data, areg::EventPriority prio /*= areg::DefaultPriority*/ )
    : areg::Event(areg::EventType::EventCustomExternal, prio)
    , mData(data)
{
    if (mData.mTimer != nullptr)
    {
        mData.mTimer->_queue_timer();
    }
}

TimerEvent::TimerEvent( Timer & timer, areg::EventPriority prio /*= areg::DefaultPriority*/ )
    : areg::Event(areg::EventType::EventCustomExternal, prio)
    , mData(timer)
{
    timer._queue_timer();
}

TimerEvent::TimerEvent(Timer & timer, DispatcherThread & target, areg::EventPriority prio /*= areg::DefaultPriority*/)
    : areg::Event(areg::EventType::EventCustomExternal, prio)
    , mData(timer)
{
    ASSERT(target.is_running());

    set_event_consumer(static_cast<EventConsumer *>(&timer.consumer()));
    register_for_thread(&target);
    timer._queue_timer();
}

TimerEvent::~TimerEvent()
{
    if (mData.mTimer != nullptr)
    {
        mData.mTimer->_unqueue_timer();
        mData.mTimer = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, static methods
//////////////////////////////////////////////////////////////////////////
bool TimerEvent::send_event( Timer & timer, id_type dispatchThreadId )
{
    return TimerEvent::send_event(timer, DispatcherThread::dispatcher_thread(dispatchThreadId));
}

bool TimerEvent::send_event(Timer & timer, DispatcherThread & dispatchThread, areg::EventPriority prio /*= areg::EventPriority::NormalPrio*/)
{
    if (!dispatchThread.is_running())
        return false;
    
    TimerEvent* timerEvent = DEBUG_NEW TimerEvent(timer, dispatchThread, prio);
    if (timerEvent == nullptr)
        return false;

    Event & event = static_cast<Event &>(*timerEvent);
    const bool result = dispatchThread.event_dispatcher().post_event(event);
    if (!result)
    {
        event.destroy();
    }

    return result;
}

} // namespace areg
