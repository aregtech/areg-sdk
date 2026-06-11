#ifndef AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
#define AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerEventData.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Event Data class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Event.hpp"
namespace areg {

/************************************************************************
 * Declared classes
 ************************************************************************/
class TimerEvent;

/************************************************************************
 * Dependencies
 ************************************************************************/
class Timer;

//////////////////////////////////////////////////////////////////////////
// TimerEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Timer event containing an expired timer, created and sent by the Timer Manager for
 *          dispatch to the timer consumer's owner thread.
 **/
class AREG_API TimerEvent final : public Event
{
//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(TimerEvent)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and sends a timer event to the specified dispatcher thread.
     *
     * \param   timer       The expired timer object.
     * \param   threadNum   The unique number of the dispatcher thread to receive the event.
     * \return  Returns true if the event was sent successfully; false otherwise.
     **/
    static bool send_event( Timer & timer, UniqueNumber threadNum);

    /**
     * \brief   Creates and sends a timer event to the specified dispatcher thread.
     *
     * \param   timer               The expired timer object.
     * \param   dispatchThread      The dispatcher thread object to receive the event.
     * \param   prio                The event priority (default: NormalPrio).
     * \return  Returns true if the event was sent successfully; false otherwise.
     **/
    static bool send_event( Timer & timer, DispatcherThread & dispatchThread, areg::EventPriority prio = areg::EventPriority::NormalPrio);

    /**
     * \brief   Returns the Timer pointer from a TimerEvent's payload area.
     *          Returns nullptr if the event has no payload or is not a TimerEvent.
     **/
    [[nodiscard]]
    static Timer * timer_from_event( const Event & evt ) noexcept;

    /**
     * \brief   Must be called by the consumer after dispatching the timer callback.
     *          TimerEvent is a friend of Timer, so this method has access.
     **/
    static void unqueue_timer( Timer & timer ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructs the timer event, stores Timer* at payload_ptr(),
     *          sets the consumer, and registers for the target thread.
     *
     * \param   timer   The timer object.
     * \param   target  The target dispatcher thread.
     * \param   prio    The event priority.
     **/
    TimerEvent( Timer & timer, DispatcherThread & target, areg::EventPriority prio = areg::DefaultPriority);

    ~TimerEvent() override = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TimerEvent() = delete;
};

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_TIMEREVENTDATA_HPP
