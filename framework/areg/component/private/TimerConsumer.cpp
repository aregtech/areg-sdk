/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/TimerConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer Consumer class implementation.
 ************************************************************************/
#include "areg/component/TimerConsumer.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// TimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void TimerConsumer::start_event_processing(Event& eventElem)
{
    Timer* timer{ TimerEvent::timer_from_event(eventElem) };
    if (timer != nullptr)
    {
        process_timer(*timer);
        TimerEvent::unqueue_timer(*timer);
    }
}

} // namespace areg
