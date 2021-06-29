/************************************************************************
 * \file        areg/component/private/EventDispatcher.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/EventDispatcher.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcher::EventDispatcher( const char* name )
    : EventDispatcherBase   ( name )
    , IEThreadConsumer      (  )
    , IEEventRouter         (  )

    , mDispatcherThread     ( NULL )
{
    ; // do nothing
}

EventDispatcher::~EventDispatcher( void )
{
    mDispatcherThread   = NULL;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class, methods
//////////////////////////////////////////////////////////////////////////
bool EventDispatcher::onThreadRegistered( Thread * threadObj )
{
    mDispatcherThread = RUNTIME_CAST(threadObj, DispatcherThread);
    ASSERT(mDispatcherThread != NULL);

    EventDispatcherBase::removeAllEvents( );
    return EventDispatcherBase::mEventExit.resetEvent();
}

void EventDispatcher::onThreadUnregistering( void )
{
    stopDispatcher();
    mDispatcherThread   = NULL;
}

void EventDispatcher::onThreadRuns( void )
{
    ASSERT(mDispatcherThread != NULL);
    startDispatcher();
}

int EventDispatcher::onThreadExit( void )
{
    stopDispatcher( );
    mDispatcherThread   = NULL;
    return IEThreadConsumer::EXIT_NORMAL;
}

bool EventDispatcher::postEvent( Event& eventElem )
{
    bool result = mDispatcherThread != NULL ? queueEvent(eventElem) : false;
    if (result == false)
    {
        OUTPUT_ERR("Failed to queue event of type [ %s ], going to destroy", eventElem.getRuntimeClassName());
        eventElem.destroy();
    }
    return result;
}
