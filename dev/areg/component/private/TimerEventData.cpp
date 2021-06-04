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
// TimerEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerEventData class, operators
//////////////////////////////////////////////////////////////////////////
const TimerEventData & TimerEventData::operator = ( const TimerEventData & src )
{
    mTimer = src.mTimer;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(TimerEvent, ThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
TimerEvent::TimerEvent( const TimerEventData & data )
    : TEEvent<TimerEventData, const TimerEventData &>  (Event::EventCustomExternal, data)
{
    if (getData().mTimer != NULL)
        getData().mTimer->queueTimer();
}

TimerEvent::TimerEvent( Timer &timer )
    : TEEvent<TimerEventData, const TimerEventData &>  (Event::EventCustomExternal, TimerEventData(timer))
{
    timer.queueTimer();
}

TimerEvent::~TimerEvent( void )
{
    if (getData().mTimer != NULL)
        getData().mTimer->unqueueTimer();
}

//////////////////////////////////////////////////////////////////////////
// TimerEvent class, static methods
//////////////////////////////////////////////////////////////////////////
bool TimerEvent::sendEvent( Timer & timer, ITEM_ID dispatchThreadId )
{
    bool result = false;

    DispatcherThread& dispatchThread = DispatcherThread::getDispatcherThread(dispatchThreadId);
    if ( dispatchThread.isRunning() )
    {
        TimerEvent* timerEvent = DEBUG_NEW TimerEvent(timer);
        if (timerEvent != NULL)
        {
            static_cast<Event *>(timerEvent)->setEventConsumer(static_cast<IEEventConsumer *>(&timer.getConsumer()));
            static_cast<Event *>(timerEvent)->registerForThread(&dispatchThread);
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
