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
 * \brief       Areg Platform, event queue implementation.
 *
 ************************************************************************/
#include "areg/component/private/EventQueue.hpp"

#include "areg/component/Event.hpp"
#include "areg/base/RuntimeClassID.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventQueue -- constructor / destructor
//////////////////////////////////////////////////////////////////////////

EventQueue::EventQueue(QueueListener& eventListener, EventStack& eventQueue)
    : mEventListener    ( eventListener )
    , mEventQueue       ( eventQueue )
{
}

//////////////////////////////////////////////////////////////////////////
// EventQueue -- base operations (no locking; called either lock-free from
// InternalEventQueue, or under ExternalEventQueue::mLock from its overrides)
//////////////////////////////////////////////////////////////////////////

void EventQueue::push_event(Event& eventElem, Event** removedEvent)
{
    const uint32_t count = mEventQueue.push_event(&eventElem, removedEvent);
    mEventListener.signal_event(count);
}

Event* EventQueue::pop_event() noexcept
{
    Event* result{ nullptr };
    const uint32_t size = mEventQueue.pop_event(&result);
    if (size == 0)
    {
        mEventListener.signal_event(0);
    }

    return result;
}

void EventQueue::remove_all_events() noexcept
{
    mEventQueue.delete_all_events();
    mEventListener.signal_event(0);
}

void EventQueue::remove_events() noexcept
{
    const uint32_t remain = mEventQueue.delete_except_exit();
    mEventListener.signal_event(remain);
}

void EventQueue::remove_events(const RuntimeClassID& eventClassId) noexcept
{
    const uint32_t remain = mEventQueue.delete_matching(eventClassId);
    mEventListener.signal_event(remain);
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue -- constructor / destructor
//////////////////////////////////////////////////////////////////////////

ExternalEventQueue::ExternalEventQueue(QueueListener& eventListener, uint32_t maxQueue)
    : EventQueue    ( eventListener, mStack )
    , mLock         ( )
    , mStack        ( maxQueue )
{
}

ExternalEventQueue::~ExternalEventQueue()
{
    mStack.delete_all_events();
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue -- locked push / pop
//
// These methods hide EventQueue::push_event / pop_event. All callers in
// EventDispatcherBase hold mExternalEvents as the concrete ExternalEventQueue
// type, so the non-virtual hiding resolves to these locked versions on every
// call-site. No virtual dispatch overhead is incurred on the hot path.
//
// signal_event() is called inside the lock to prevent a TOCTOU race between
// pop detecting an empty queue and a concurrent push setting the SyncEvent:
// if signal_event(0) ran outside the lock a concurrent push could set the
// SyncEvent and then have it immediately cleared by the racing signal_event(0).
//////////////////////////////////////////////////////////////////////////

void ExternalEventQueue::push_event(Event& eventElem, Event** removedEvent)
{
    Lock lock(mLock);
    const uint32_t count = mStack.push_event(&eventElem, removedEvent);
    mEventListener.signal_event(count);
}

Event* ExternalEventQueue::pop_event() noexcept
{
    Event* result{ nullptr };
    uint32_t size{ 0 };

    do
    {
        Lock lock(mLock);
        size = mStack.pop_event(&result);
        if (size == 0)
        {
            mEventListener.signal_event(0);
        }
    } while (false);

    return result;
}

void ExternalEventQueue::remove_events() noexcept
{
    Lock lock(mLock);
    EventQueue::remove_events();
}

void ExternalEventQueue::remove_events(const RuntimeClassID& eventClassId) noexcept
{
    Lock lock(mLock);
    EventQueue::remove_events(eventClassId);
}

void ExternalEventQueue::remove_all_events() noexcept
{
    Lock lock(mLock);
    EventQueue::remove_all_events();
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue -- constructor / destructor
//////////////////////////////////////////////////////////////////////////

InternalEventQueue::InternalEventQueue(uint32_t maxQueue)
    : EventQueue    ( static_cast<QueueListener&>(self()), mStack )
    , mStack        ( maxQueue )
{
}

InternalEventQueue::~InternalEventQueue()
{
    mStack.delete_all_events();
}

void InternalEventQueue::signal_event(uint32_t /* eventCount */)
{
    // Intentional no-op, no SyncEvent signaling is needed
}

} // namespace areg
