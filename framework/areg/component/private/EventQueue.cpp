/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
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
EventQueue::EventQueue( IEQueueListener & eventListener, SortedEventStack & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
}

//////////////////////////////////////////////////////////////////////////
// EventQueue class, methods
//////////////////////////////////////////////////////////////////////////
void EventQueue::pushEvent( Event& evendElem )
{
    mEventListener.signalEvent( mEventQueue.pushEvent(&evendElem) );
}

Event* EventQueue::popEvent( void )
{
    Event* result{ nullptr };
    uint32_t size = mEventQueue.popEvent(&result);
    if (size == 0)
    {
        mEventListener.signalEvent(0);
    }

    return result;
}

void EventQueue::removeAllEvents(void)
{
    mEventQueue.deleteAllEvents();
    mEventListener.signalEvent(0);
}

void EventQueue::removeEvents( bool keepSpecials /*= false*/ )
{
    uint32_t remain = mEventQueue.deleteAllLowerPriority(keepSpecials ? Event::eEventPriority::EventPriorityHigh : Event::eEventPriority::EventPriorityCritical);
    mEventListener.signalEvent(remain);
}

void EventQueue::removeEvents( const RuntimeClassID & eventClassId )
{
    uint32_t remain = mEventQueue.deleteAllMatchClass(eventClassId);
    mEventListener.signalEvent(remain);
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

ExternalEventQueue::ExternalEventQueue( IEQueueListener & eventListener )
    : EventQueue( eventListener, mStack )
    , mStack    ( )
{
}

ExternalEventQueue::~ExternalEventQueue(void)
{
    mStack.deleteAllEvents();
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

InternalEventQueue::InternalEventQueue(void)
    : EventQueue( static_cast<IEQueueListener &>(self()), mStack )
    , mStack    ( )
{
}

InternalEventQueue::~InternalEventQueue(void)
{
    mStack.deleteAllEvents();
}

void InternalEventQueue::signalEvent(uint32_t /* eventCount */)
{
}

inline InternalEventQueue & InternalEventQueue::self( void )
{
    return (*this);
}
