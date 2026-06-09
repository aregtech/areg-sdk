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
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// TimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

AREG_IMPLEMENT_RUNTIME_EVENT(TimerEvent, Event)

//////////////////////////////////////////////////////////////////////////
// TimerEvent constructors / destructor
//////////////////////////////////////////////////////////////////////////

TimerEvent::TimerEvent(Timer & timer, DispatcherThread & target, areg::EventPriority prio /*= areg::DefaultPriority*/)
    : Event(areg::EventType::EventCustomExternal, static_cast<uint32_t>(sizeof(void*)), prio)
{
    ASSERT(target.is_running());
    set_event_id( TimerEvent::CLASS_ID );
    // Store Timer* at payload_ptr() (LOCAL-ONLY, same-process only, never serialized).
    *reinterpret_cast<Timer**>(payload_ptr()) = &timer;
    set_event_consumer( static_cast<EventConsumer*>( &timer.consumer() ) );
    register_for_thread( &target );
    timer._queue_timer();
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent static methods
//////////////////////////////////////////////////////////////////////////

bool TimerEvent::send_event( Timer & timer, id_type dispatchThreadId )
{
    return TimerEvent::send_event(timer, DispatcherThread::dispatcher_thread(dispatchThreadId));
}

bool TimerEvent::send_event(Timer & timer, DispatcherThread & dispatchThread, areg::EventPriority prio /*= areg::EventPriority::NormalPrio*/)
{
    if (!dispatchThread.is_running())
        return false;

    TimerEvent timerEvent(timer, dispatchThread, prio);
    const bool result = dispatchThread.event_dispatcher().post_event(timerEvent);
    if (!result)
    {
        timer._unqueue_timer();
    }

    return result;
}

Timer * TimerEvent::timer_from_event( const Event & evt ) noexcept
{
    if (evt.event_id() != TimerEvent::CLASS_ID)
        return nullptr;

    // Timer* is at payload_ptr(). Cast to TimerEvent to access the protected method.
    const uint8_t* data{ static_cast<const TimerEvent&>(evt).payload_ptr() };
    return (data != nullptr) ? *reinterpret_cast<Timer* const*>(data) : nullptr;
}

void TimerEvent::unqueue_timer( Timer & timer ) noexcept
{
    timer._unqueue_timer();
}

} // namespace areg
