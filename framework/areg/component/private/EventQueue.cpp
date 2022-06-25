/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
EventQueue::EventQueue( IEQueueListener & eventListener, TEStack<Event *> & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
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
    Event* result = mEventQueue.isEmpty() == false ? mEventQueue.popFirst() : nullptr;
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
        if ( eventElem != nullptr && eventElem != exitEvent )
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

        TENolockStack<Event *> tempQueue;        
        while ( mEventQueue.isEmpty() == false )
        {
            Event * elemEvent = mEventQueue.popFirst();
            if ( elemEvent != nullptr )
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

    TENolockStack<Event *> specials;
    Event* exitEvent  = static_cast<Event *>(&ExitEvent::getExitEvent());

    while (mEventQueue.isEmpty() == false)
    {
        Event * eventElem = mEventQueue.popFirst();
        if (eventElem != nullptr)
        {
            ASSERT(exitEvent != nullptr);
            if (eventElem == exitEvent)
            {
                // OUTPUT_DBG("Skip removing exit event!");
                specials.pushLast(eventElem);
                eventElem = nullptr;
            }
            else if ( keepSpecials )
            {
                ServiceResponseEvent* respEvent = RUNTIME_CAST(eventElem, ServiceResponseEvent);
                if ( respEvent != nullptr)
                {
                    unsigned int respId = respEvent->getResponseId();
                    if ( NEService::isConnectNotifyId(respId) )
                    {
                        OUTPUT_DBG("Keep response event with response ID NEService::SI_NOTIFY_CONNECT for target proxy [ %s ]"
                                        , ProxyAddress::convAddressToPath(respEvent->getTargetProxy()).getString());
                        specials.pushLast(eventElem);
                        eventElem = nullptr;
                    }
                    else
                    {
                        ; // do nothing, should be removed!
                    }
                }
            }
        }

        if (eventElem != nullptr)
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
    : EventQueue( eventListener, static_cast<TEStack<Event *> &>(mStack) )
    , mStack    ( )
{
}

ExternalEventQueue::~ExternalEventQueue(void)
{
    removePendingEvents( false );
    mStack.clear();
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

InternalEventQueue::InternalEventQueue(void)
    : EventQueue( static_cast<IEQueueListener &>(self()), static_cast<TEStack<Event *> &>(mStack) )
    , mStack    ( )
{
}

InternalEventQueue::~InternalEventQueue(void)
{
    removePendingEvents( false );
    mStack.clear();
}

void InternalEventQueue::signalEvent(uint32_t /* eventCount */)
{
}

inline InternalEventQueue & InternalEventQueue::self( void )
{
    return (*this);
}
