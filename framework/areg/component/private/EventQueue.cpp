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
EventQueue::EventQueue( QueueListener & eventListener, SortedEventStack & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
}

//////////////////////////////////////////////////////////////////////////
// EventQueue class, methods
//////////////////////////////////////////////////////////////////////////
void EventQueue::push_event( Event& evendElem, Event** removedEvent )
{
    mEventListener.signal_event( mEventQueue.push_event(&evendElem, removedEvent) );
}

Event* EventQueue::pop_event()
{
    Event* result{ nullptr };
    uint32_t size = mEventQueue.pop_event(&result);
    if (size == 0)
    {
        mEventListener.signal_event(0);
    }

    return result;
}

void EventQueue::remove_all_events()
{
    mEventQueue.delete_all_events();
    mEventListener.signal_event(0);
}

void EventQueue::remove_events( bool keepSpecials /*= false*/ )
{
    uint32_t remain = mEventQueue.delete_lower_priority(keepSpecials ? Event::EventPriority::HighPrio : Event::EventPriority::CriticalPrio);
    mEventListener.signal_event(remain);
}

void EventQueue::remove_events( const RuntimeClassID & eventClassId )
{
    uint32_t remain = mEventQueue.delete_matching_class(eventClassId);
    mEventListener.signal_event(remain);
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
    mStack.delete_all_events();
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
    mStack.delete_all_events();
}

void InternalEventQueue::signal_event(uint32_t /* eventCount */)
{
}

inline InternalEventQueue & InternalEventQueue::self()
{
    return (*this);
}
