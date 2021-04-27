/************************************************************************
 * \file        areg/component/private/CETimerEventData.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Data class.
 *
 ************************************************************************/
#include "areg/component/private/CETimerEventData.hpp"
#include "areg/component/CETimer.hpp"
#include "areg/component/CETimerEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CETimerEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETimerEventData class, operators
//////////////////////////////////////////////////////////////////////////
const CETimerEventData & CETimerEventData::operator = ( const CETimerEventData & src )
{
    mTimer = src.mTimer;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// CETimerEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETimerEvent class, implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CETimerEvent, CEThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// CETimerEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CETimerEvent::CETimerEvent( const CETimerEventData & data )
    : TEEvent<CETimerEventData, const CETimerEventData &>  (CEEvent::EventCustomExternal, data)
{
    if (GetData().mTimer != NULL)
        GetData().mTimer->TimerIsQueued();
}

CETimerEvent::CETimerEvent( CETimer &timer )
    : TEEvent<CETimerEventData, const CETimerEventData &>  (CEEvent::EventCustomExternal, CETimerEventData(timer))
{
    timer.TimerIsQueued();
}

CETimerEvent::~CETimerEvent( void )
{
    if (GetData().mTimer != NULL)
        GetData().mTimer->TimerIsUnqueued();
}

//////////////////////////////////////////////////////////////////////////
// CETimerEvent class, static methods
//////////////////////////////////////////////////////////////////////////
bool CETimerEvent::SendEvent( CETimer & timer, ITEM_ID dispatchThreadId )
{
    bool result = false;

    CEDispatcherThread& dispatchThread = CEDispatcherThread::GetDispatcherThread(dispatchThreadId);
    if ( dispatchThread.IsValid() )
    {
        CETimerEvent* timerEvent = DEBUG_NEW CETimerEvent(timer);
        if (timerEvent != NULL)
        {
            static_cast<CEEvent *>(timerEvent)->SetEventConsumer(static_cast<IEEventConsumer *>(&timer.GetTimerConsumer()));
            static_cast<CEEvent *>(timerEvent)->RegisterForThread(&dispatchThread);
            static_cast<CEEvent *>(timerEvent)->SendEvent();
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
