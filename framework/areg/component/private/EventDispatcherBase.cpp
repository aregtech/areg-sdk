/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcherBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher Base class
 *
 ************************************************************************/
#include "areg/component/private/EventDispatcherBase.hpp"

#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/ExitEvent.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcherBase::EventDispatcherBase(const String & name, uint32_t maxQeueue)
    : QueueListener  ( )

    , mDispatcherName( name )
    , mExternalEvents( static_cast<QueueListener &>(self()), maxQeueue)
    , mInternalEvents( )
    , mHasStarted    ( false )
    , mConsumerMap   ( )
    , mEventExit     ( false, false )
    , mEventQueue    ( true, false )
{
}

EventDispatcherBase::~EventDispatcherBase()
{
    mHasStarted = false;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, methods
//////////////////////////////////////////////////////////////////////////

bool EventDispatcherBase::is_exit_event( const Event * anEvent ) const
{
    return (anEvent != nullptr) && anEvent->is_exit_prio();
}

bool EventDispatcherBase::start_dispatcher()
{
    mEventExit.reset( );
    return run_dispatcher( );
}

void EventDispatcherBase::stop_dispatcher() noexcept
{
    mExternalEvents.lock_queue( );
    if ( mHasStarted )
    {
        Event exit{ ExitEvent::exit_event() };
        mExternalEvents.push_event(exit);
    }

    mEventExit.set_signaled();
    mExternalEvents.unlock_queue( );
}

void EventDispatcherBase::exit_dispatcher() noexcept
{
    mInternalEvents.remove_all_events();
    mExternalEvents.remove_all_events();

    mEventExit.set_signaled();
}

void EventDispatcherBase::shutdown_dispatcher() noexcept
{
    stop_dispatcher();
}

bool EventDispatcherBase::queue_event( Event& eventElem )
{
    areg::EventType eventType = eventElem.event_type();
    if (mHasStarted.load(std::memory_order_relaxed))
    {
        if (areg::is_external(eventType))
        {
            // MOVE the event into the queue (O(1) shared_ptr transfer).
            // After this call, eventElem is in a moved-from (empty) state.
            mExternalEvents.push_event(eventElem);
            return true;
        }

        if (areg::is_internal(eventType))
        {
            mInternalEvents.push_event(eventElem);
            return true;
        }
    }

    return false;
}

bool EventDispatcherBase::register_event_consumer( const uint32_t whichClass, EventConsumer& whichConsumer )
{
    mConsumerMap.lock();
    bool result = false;
    EventConsumerList* listConsumers = mConsumerMap.find_resource(whichClass);
    if (listConsumers == nullptr || !listConsumers->exist(whichConsumer))
    {
        mConsumerMap.register_resource_object(whichClass, &whichConsumer);
        result = true;
    }
    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::unregister_event_consumer( const uint32_t whichClass, EventConsumer & whichConsumer )
{
    mConsumerMap.lock();
    bool result = mConsumerMap.exist(whichClass);
    if (result)
    {
        mConsumerMap.unregister_resource_object(whichClass, &whichConsumer, true);
    }
    mConsumerMap.unlock();
    return result;
}


int32_t EventDispatcherBase::remove_consumer( EventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    int32_t result = 0;
    std::vector<uint32_t> toRemove;

    for (const auto & entry : mConsumerMap.data())
    {
        EventConsumerList & list = const_cast<EventConsumerList &>(entry.second);
        ASSERT(list.is_empty() == false);
        result += list.remove_consumer(whichConsumer) ? 1 : 0;
        if (list.is_empty())
        {
            toRemove.push_back(entry.first);
        }
    }

    for (uint32_t key : toRemove)
    {
        EventConsumerList removed = mConsumerMap.unregister_resource(key);
        removed.remove_all_consumers();
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::run_dispatcher()
{
    ready_for_events( true );

    SyncEvent* events[2] { &mEventExit, &mEventQueue };
    int32_t whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);

    do
    {
        whichEvent = SyncEvent::wait_any(events, 2, areg::WAIT_INFINITE);

        if (    whichEvent != static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit)
             && whichEvent != static_cast<int32_t>(EventDispatcherBase::EventSignal::Queue) )
        {
            continue;
        }

        // Tight drain loop: process all available events before returning to wait.
        for (;;)
        {
            Event eventElem = pick_event();  // returns Event by value; invalid if queue empty

            if (eventElem.is_exit_prio())
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                break;
            }

            if (!eventElem.is_valid())
            {
                break;
            }

            if ( prepare_dispatch_event(eventElem) )
            {
                dispatch_event(eventElem);
            }

            post_dispatch_event(eventElem);

            // Drain internal events generated by the dispatch above
            while (!mInternalEvents.is_empty())
            {
                Event intEvent{ mInternalEvents.pop_event() };
                if (prepare_dispatch_event(intEvent))
                    dispatch_event(intEvent);
                // destructor releases intEvent at end of loop body
            }
        }

    } while (whichEvent != static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));

    ready_for_events(false);
    remove_all_events();
    _clean();

    return (whichEvent == static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit));
}

void EventDispatcherBase::ready_for_events( bool is_ready )
{
    mExternalEvents.lock_queue( );
    mHasStarted = is_ready;
    mExternalEvents.unlock_queue( );
}

Event EventDispatcherBase::pick_event() noexcept
{
    return mExternalEvents.pop_event();
}

bool EventDispatcherBase::prepare_dispatch_event( Event& eventElem ) noexcept
{
    return eventElem.is_valid();
}

void EventDispatcherBase::post_dispatch_event( Event& eventElem )
{
    // Event is now a value type — just release its buffer.
    // ExitEvent copies are also released (their destroy_event() is a no-op in the base).
    eventElem.destroy_event();
}

bool EventDispatcherBase::dispatch_event( Event& eventElem )
{
    EventConsumer* consumer = eventElem.event_consumer();
    if ( consumer != nullptr)
    {
        eventElem.dispatch_self(consumer);
        return true;
    }
    else
    {
        EventConsumerList* listConsumers = mConsumerMap.find_resource(eventElem.event_id());
        if (listConsumers != nullptr)
        {
            for (auto entry : listConsumers->data())
            {
                eventElem.dispatch_self(entry);
            }

            return !listConsumers->is_empty();
        }
    }

    return false;
}

bool EventDispatcherBase::has_registered_consumer( uint32_t whichClass ) const
{
    return mConsumerMap.exist(whichClass);
}

inline void EventDispatcherBase::_clean() noexcept
{
    mConsumerMap.lock();

    while (!mConsumerMap.is_empty())
    {
        uint32_t key = mConsumerMap.data().begin()->first;
        EventConsumerList removed = mConsumerMap.unregister_resource(key);
        removed.remove_all_consumers();
    }

    mConsumerMap.unlock();
}

bool EventDispatcherBase::pulse_exit()
{
    return mEventExit.set_signaled();
}

} // namespace areg
