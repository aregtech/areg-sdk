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

//////////////////////////////////////////////////////////////////////////
// TimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, implement Runtime
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(TimerEvent, TimerEventBase)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
TimerEvent::TimerEvent( const TimerEventData & data )
    : TimerEventBase(Event::EventType::EventCustomExternal, data)
{
    if (mData.mTimer != nullptr)
    {
        mData.mTimer->_queueTimer();
    }
}

TimerEvent::TimerEvent( areg::Timer &timer )
    : TimerEventBase(Event::EventType::EventCustomExternal, TimerEventData(timer))
{
    timer._queueTimer();
}

TimerEvent::TimerEvent(areg::Timer & timer, areg::DispatcherThread & target)
    : TimerEventBase(Event::EventType::EventCustomExternal, TimerEventData(timer))
{
    ASSERT(target.isRunning());

    setEventConsumer(static_cast<areg::EventConsumer *>(&timer.getConsumer()));
    registerForThread(&target);
    timer._queueTimer();
}

TimerEvent::~TimerEvent()
{
    if (mData.mTimer != nullptr)
    {
        mData.mTimer->_unqueueTimer();
        mData.mTimer = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, static methods
//////////////////////////////////////////////////////////////////////////
bool TimerEvent::sendEvent( areg::Timer & timer, id_type dispatchThreadId )
{
    return TimerEvent::sendEvent(timer, areg::DispatcherThread::getDispatcherThread(dispatchThreadId));
}

bool TimerEvent::sendEvent(areg::Timer & timer, areg::DispatcherThread & dispatchThread)
{
    bool result{ false };
    if ( dispatchThread.isRunning() )
    {
        TimerEvent* timerEvent = DEBUG_NEW TimerEvent(timer, dispatchThread);
        if (timerEvent != nullptr)
        {
            static_cast<Event *>(timerEvent)->deliverEvent();
            result = true;
        }
    }

    return result;
}
