/************************************************************************
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event queue class
 *
 ************************************************************************/
#include "areg/component/private/EventQueue.hpp"

#include "areg/component/Event.hpp"
#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/private/IEQueueListener.hpp"

#include "areg/base/RuntimeClassID.hpp"

//////////////////////////////////////////////////////////////////////////
// EventQueue class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventQueue class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
EventQueue::EventQueue( IEQueueListener & eventListener, TEStack<Event *, Event *> & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
    ; // do nothing
}

EventQueue::~EventQueue( void )
{
    ; // the event queue should be emptied in derived objects
}

//////////////////////////////////////////////////////////////////////////
// EventQueue class, methods
//////////////////////////////////////////////////////////////////////////
void EventQueue::pushEvent( Event& evendElem )
{
    mEventListener.signalEvent( mEventQueue.pushLast(&evendElem) );
}

Event* EventQueue::popEvent( void )
{
    mEventQueue.lock();
    Event* result = mEventQueue.isEmpty() == false ? mEventQueue.popFirst() : NULL;
    if (mEventQueue.isEmpty())
    {
        mEventListener.signalEvent(0);
    }

    mEventQueue.unlock();

    return result;
}

void EventQueue::removeAllEvents(void)
{
    mEventQueue.lock();
    Event* exitEvent  = static_cast<Event *>(&ExitEvent::getExitEvent());
    while ( mEventQueue.isEmpty() == false )
    {
        Event * eventElem = mEventQueue.popFirst();
        if ( eventElem != NULL && eventElem != exitEvent )
            eventElem->destroy();
    }
    mEventListener.signalEvent( 0 );
    mEventQueue.unlock();
}

void EventQueue::removeEvents( bool keepSpecials /*= false*/ )
{
    mEventQueue.lock();
    bool hasEvents   = removePendingEvents( keepSpecials );
    mEventListener.signalEvent(hasEvents ? 1 : 0);
    mEventQueue.unlock();
}

int EventQueue::removeEvents( const RuntimeClassID & eventClassId )
{
    int removedCount = 0;

    if ( eventClassId != ExitEvent::getExitEvent().getRuntimeClassId() )
    {
        mEventQueue.lock();
        removedCount = mEventQueue.getSize();

        TENolockStack<Event *, Event *> tempQueue;        
        while ( mEventQueue.isEmpty() == false )
        {
            Event * elemEvent = mEventQueue.popFirst();
            if ( elemEvent != NULL )
            {
                if ( elemEvent->getRuntimeClassId() == eventClassId )
                    elemEvent->destroy();
                else
                    tempQueue.pushLast(elemEvent);
            }
        }
        removedCount -= tempQueue.getSize();

        while (tempQueue.isEmpty() == false )
            mEventQueue.pushLast( tempQueue.popFirst() );

        mEventListener.signalEvent(mEventQueue.getSize());
        mEventQueue.unlock();
    }

    return removedCount;
}

bool EventQueue::removePendingEvents( bool keepSpecials )
{
    mEventQueue.lock();

    TENolockStack<Event *, Event *> specials;
    Event* exitEvent  = static_cast<Event *>(&ExitEvent::getExitEvent());

    while (mEventQueue.isEmpty() == false)
    {
        Event * eventElem = mEventQueue.popFirst();
        if (eventElem != NULL)
        {
            ASSERT(exitEvent != NULL);
            if (eventElem == exitEvent)
            {
                // OUTPUT_DBG("Skip removing exit event!");
                specials.pushLast(eventElem);
                eventElem = NULL;
            }
            else if ( keepSpecials )
            {
                ServiceResponseEvent* respEvent = RUNTIME_CAST(eventElem, ServiceResponseEvent);
                if ( respEvent != NULL)
                {
                    unsigned int respId = respEvent->getResponseId();
                    if ( NEService::isConnectNotifyId(respId) )
                    {
                        OUTPUT_DBG("Keep response event with response ID NEService::SI_NOTIFY_CONNECT for target proxy [ %s ]"
                                        , ProxyAddress::convAddressToPath(respEvent->getTargetProxy()).getString());
                        specials.pushLast(eventElem);
                        eventElem = NULL;
                    }
                    else
                    {
                        ; // do nothing, should be removed!
                    }
                }
            }
        }

        if (eventElem != NULL)
            eventElem->destroy();
    }

    while (specials.isEmpty() == false )
        mEventQueue.pushLast( specials.popFirst() );

    mEventQueue.unlock();

    return (mEventQueue.isEmpty() == false);
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

ExternalEventQueue::ExternalEventQueue( IEQueueListener & eventListener )
    : EventQueue  ( eventListener, static_cast<TEStack<Event *, Event *> &>(self()) )
{
    ; // do nothing
}

ExternalEventQueue::~ExternalEventQueue(void)
{
    removePendingEvents( false );
    removeAll();
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

InternalEventQueue::InternalEventQueue(void)
    : EventQueue  ( static_cast<IEQueueListener &>(self()), static_cast<TEStack<Event *, Event *> &>(self()) )
{
    ; // do nothing
}

InternalEventQueue::~InternalEventQueue(void)
{
    removePendingEvents( false );
    removeAll();
}

void InternalEventQueue::signalEvent(int /* eventCount */)
{
    ; // do nothing
}
