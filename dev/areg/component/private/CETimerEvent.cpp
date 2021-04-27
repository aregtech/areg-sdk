/************************************************************************
 * \file        areg/component/CETimerEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer Event Consumer.
 *
 ************************************************************************/
#include "areg/component/CETimerEvent.hpp"
#include "areg/component/CEDispatcherThread.hpp"

//////////////////////////////////////////////////////////////////////////
// IETimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IETimerConsumer::IETimerConsumer( void )
    : TEEventConsumer<CETimerEventData, const CETimerEventData &>    ( )
{
    ; // do nothing
}

IETimerConsumer::~IETimerConsumer( void )
{
    ; // do nothing
}

void IETimerConsumer::ProcessEvent( const CETimerEventData & /* data */ )
{
    ASSERT(false);
}

void IETimerConsumer::StartEventProcessing( CEEvent& eventElem )
{
    CETimerEvent* timerEvent = static_cast<CETimerEvent *>( RUNTIME_CAST(&eventElem, CETimerEvent) );
    CETimer *timer = timerEvent != NULL ? timerEvent->GetData().GetTimer() : NULL;
    if (timer != NULL )
    {
        ProcessTimer(*timer);
    }
    else
    {
        OUTPUT_ERR("Ignoring to process timer event. Either wrong runtime class [ %s ] or invalid timer pointer [ %p ]", eventElem.GetRuntimeClassName(), timer);
    }
}
