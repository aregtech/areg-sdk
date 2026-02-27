/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event queue class
 *
 ************************************************************************/
#include "areg/component/private/EventQueue.hpp"

#include "areg/component/Event.hpp"
#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/private/QueueListener.hpp"

#include "areg/base/RuntimeClassID.hpp"

//////////////////////////////////////////////////////////////////////////
// EventQueue class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventQueue class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
EventQueue::EventQueue( QueueListener & eventListener, areg::SortedEventStack & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
}

//////////////////////////////////////////////////////////////////////////
// EventQueue class, methods
//////////////////////////////////////////////////////////////////////////
void EventQueue::pushEvent( areg::Event& evendElem, areg::Event** removedEvent )
{
    mEventListener.signalEvent( mEventQueue.pushEvent(&evendElem, removedEvent) );
}

areg::Event* EventQueue::popEvent()
{
    areg::Event* result{ nullptr };
    uint32_t size = mEventQueue.popEvent(&result);
    if (size == 0)
    {
        mEventListener.signalEvent(0);
    }

    return result;
}

void EventQueue::removeAllEvents()
{
    mEventQueue.deleteAllEvents();
    mEventListener.signalEvent(0);
}

void EventQueue::removeEvents( bool keepSpecials /*= false*/ )
{
    uint32_t remain = mEventQueue.deleteAllLowerPriority(keepSpecials ? areg::Event::EventPriority::HighPrio : areg::Event::EventPriority::CriticalPrio);
    mEventListener.signalEvent(remain);
}

void EventQueue::removeEvents( const areg::RuntimeClassID & eventClassId )
{
    uint32_t remain = mEventQueue.deleteAllMatchClass(eventClassId);
    mEventListener.signalEvent(remain);
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

ExternalEventQueue::ExternalEventQueue( QueueListener & eventListener, uint32_t maxQueue)
    : EventQueue( eventListener, mStack )
    , mStack    ( maxQueue)
{
}

ExternalEventQueue::~ExternalEventQueue()
{
    mStack.deleteAllEvents();
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

InternalEventQueue::InternalEventQueue(uint32_t maxQueue)
    : EventQueue( static_cast<QueueListener &>(self()), mStack )
    , mStack    ( maxQueue )
{
}

InternalEventQueue::~InternalEventQueue()
{
    mStack.deleteAllEvents();
}

void InternalEventQueue::signalEvent(uint32_t /* eventCount */)
{
}

inline InternalEventQueue & InternalEventQueue::self()
{
    return (*this);
}
