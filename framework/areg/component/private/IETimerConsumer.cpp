/************************************************************************
 * \file        areg/component/private/IETimerConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Consumer.
 *
 ************************************************************************/
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"

//////////////////////////////////////////////////////////////////////////
// IETimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IETimerConsumer::IETimerConsumer( void )
    : IETimerEventConsumerBase  ( )
{
    ; // do nothing
}

IETimerConsumer::~IETimerConsumer( void )
{
    ; // do nothing
}

void IETimerConsumer::processEvent( const TimerEventData & /* data */ )
{
    ASSERT(false);
}

void IETimerConsumer::startEventProcessing( Event& eventElem )
{
    TimerEvent* timerEvent = static_cast<TimerEvent *>( RUNTIME_CAST(&eventElem, TimerEvent) );
    Timer *timer = timerEvent != NULL ? timerEvent->getData().getTimer() : NULL;
    if (timer != NULL )
    {
        processTimer(*timer);
    }
    else
    {
        OUTPUT_ERR("Ignoring to process timer event. Either wrong runtime class [ %s ] or invalid timer pointer [ %p ]", eventElem.getRuntimeClassName(), timer);
    }
}
