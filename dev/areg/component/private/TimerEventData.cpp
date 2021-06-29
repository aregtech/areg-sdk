/************************************************************************
 * \file        areg/component/private/TimerEventData.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Data class.
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
    : TimerEventBase(Event::EventCustomExternal, data)
{
    if (mData.mTimer != NULL)
        mData.mTimer->queueTimer();
}

TimerEvent::TimerEvent( Timer &timer )
    : TimerEventBase(Event::EventCustomExternal, TimerEventData(timer))
{
    timer.queueTimer();
}

TimerEvent::TimerEvent(Timer & timer, DispatcherThread & target)
    : TimerEventBase(Event::EventCustomExternal, TimerEventData(timer))
{
    ASSERT(target.isRunning());

    setEventConsumer(static_cast<IEEventConsumer *>(&timer.getConsumer()));
    registerForThread(&target);
    timer.queueTimer();
}

TimerEvent::~TimerEvent( void )
{
    if (mData.mTimer != NULL)
        mData.mTimer->unqueueTimer();
    mData.mTimer = NULL;
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, static methods
//////////////////////////////////////////////////////////////////////////
bool TimerEvent::sendEvent( Timer & timer, ITEM_ID dispatchThreadId )
{
    return TimerEvent::sendEvent(timer, DispatcherThread::getDispatcherThread(dispatchThreadId));
}

bool TimerEvent::sendEvent(Timer & timer, DispatcherThread & dispatchThread)
{
    bool result = false;
    if ( dispatchThread.isRunning() )
    {
        TimerEvent* timerEvent = DEBUG_NEW TimerEvent(timer, dispatchThread);
        if (timerEvent != NULL)
        {
            static_cast<Event *>(timerEvent)->sendEvent();
            result = true;
        }
        else
        {
            OUTPUT_ERR("Cound not create Timer Event. Ignoring sending event");
        }
    }
    else
    {
        OUTPUT_ERR("Invalid Dispatcher Thread. Ignoring sending event");
    }

    return result;
}
