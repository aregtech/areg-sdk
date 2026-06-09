#ifndef AREG_COMPONENT_TIMERCONSUMER_HPP
#define AREG_COMPONENT_TIMERCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/TimerConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Event Consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/private/TimerEventData.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Timer;
    class TimerEventData;
    class DispatcherThread;
} // namespace areg

namespace areg {

/**
 * \brief   The timer Event is triggered when a Timer is expired and it should
 *          be processed in dispatcher thread.
 * 
 *          For details of setting up and starting the timer, see Timer class. 
 * \see Timer
 **/

//////////////////////////////////////////////////////////////////////////
// TimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for objects that consume and process timer events. Inherit and override
 *          process_timer() to handle timer expiration.
 **/
class AREG_API TimerConsumer : public EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    TimerConsumer() = default;
    virtual ~TimerConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when a timer expires. Override to process the expired timer.
     *
     * \param   timer       The timer object that has expired.
     **/
    virtual void process_timer( Timer & timer ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden overrides
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Triggered by the dispatcher when starting to process a timer event.
     *
     * \param   eventElem       The timer event being processed. Ignored if not a TimerEvent.
     **/
    inline void start_event_processing( Event & eventElem) final;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( TimerConsumer );
};

//////////////////////////////////////////////////////////////////////////
// TimerConsumer inline methods
//////////////////////////////////////////////////////////////////////////
inline void TimerConsumer::start_event_processing(Event& eventElem)
{
    Timer* timer{ TimerEvent::timer_from_event(eventElem) };
    if (timer != nullptr)
    {
        process_timer(*timer);
        TimerEvent::unqueue_timer(*timer);
    }
}

} // namespace areg
#endif  // AREG_COMPONENT_TIMERCONSUMER_HPP
