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
#include "areg/component/IETimerConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// TimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(TimerEvent, TimerEventBase)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
TimerEvent::TimerEvent( const TimerEventData & data )
    : TimerEventBase(Event::eEventType::EventCustomExternal, data)
{
    if (mData.mTimer != nullptr)
    {
        mData.mTimer->_queueTimer();
    }
}

TimerEvent::TimerEvent( Timer &timer )
    : TimerEventBase(Event::eEventType::EventCustomExternal, TimerEventData(timer))
{
    timer._queueTimer();
}

TimerEvent::TimerEvent(Timer & timer, DispatcherThread & target)
    : TimerEventBase(Event::eEventType::EventCustomExternal, TimerEventData(timer))
{
    ASSERT(target.isRunning());

    setEventConsumer(static_cast<IEEventConsumer *>(&timer.getConsumer()));
    registerForThread(&target);
    timer._queueTimer();
}

TimerEvent::~TimerEvent( void )
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
bool TimerEvent::sendEvent( Timer & timer, id_type dispatchThreadId )
{
    return TimerEvent::sendEvent(timer, DispatcherThread::getDispatcherThread(dispatchThreadId));
}

bool TimerEvent::sendEvent(Timer & timer, DispatcherThread & dispatchThread)
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
