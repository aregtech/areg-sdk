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
    return (anEvent == static_cast<const Event *>(&ExitEvent::exit_event( )));
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
        mExternalEvents.push_event( ExitEvent::exit_event( ), nullptr );
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
            mExternalEvents.push_event(eventElem, nullptr);
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

bool EventDispatcherBase::register_event_consumer( const RuntimeClassID& whichClass, EventConsumer& whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    EventConsumerList* listConsumers = mConsumerMap.find_resource_object(whichClass);
    if (listConsumers == nullptr)
    {
        listConsumers   = DEBUG_NEW EventConsumerList();
        if (listConsumers != nullptr)
            mConsumerMap.register_resource_object(whichClass, listConsumers);
    }

    if ( (listConsumers != nullptr) && (listConsumers->exist(whichConsumer) == false) )
    {
        result = listConsumers->add_consumer(whichConsumer);
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::unregister_event_consumer( const RuntimeClassID & whichClass, EventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    EventConsumerList* listConsumers = mConsumerMap.find_resource_object(whichClass);
    if (listConsumers != nullptr)
    {
        result = listConsumers->remove_consumer(whichConsumer);
        if (listConsumers->is_empty())
        {
            mConsumerMap.unregister_resource_object(whichClass);
            delete listConsumers;
        }
    }
    else
    {
        // AAvetyan:    The reason why it does not find, because it is cleaned in _clean() function.
        //              This is mainly happening in component, which has server interface implementation.
        //              To make graceful shutdown, in _clean() method should be set filtering.
        //              But the consumer map indeed at this point is empty and the consumer is unregistered.
        mConsumerMap.unregister_resource_object(whichClass);
    }

    mConsumerMap.unlock();
    return result;
}


int32_t EventDispatcherBase::remove_consumer( EventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    int32_t result = 0;
    LinkedList<RuntimeClassID> removedList;
    RuntimeClassID     Key(RuntimeClassID::empty_id());
    EventConsumerList* Value = nullptr;

    Value = mConsumerMap.resource_first_key(Key);
    while (Value != nullptr)
    {
        ASSERT(Value->is_empty() == false);
        result += Value->remove_consumer(whichConsumer) ? 1 : 0;
        if (Value->is_empty())
        {
            removedList.push_first(Key);
        }

        Value = mConsumerMap.resource_next_key(Key);
    }

    while (removedList.remove_last(Key))
    {
        Value   = mConsumerMap.unregister_resource_object(Key);
        ASSERT(Value != nullptr);
        delete Value;
        Value = nullptr;
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::run_dispatcher()
{
    ready_for_events( true );

    SyncEvent* events[2] { &mEventExit, &mEventQueue };
    int32_t whichEvent      = static_cast<int32_t>(EventDispatcherBase::EventSignal::Error);
    const ExitEvent& exitEvent = ExitEvent::exit_event();

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
            Event* eventElem = pick_event();

            if ( static_cast<const Event*>(eventElem) == static_cast<const Event*>(&exitEvent) )
            {
                whichEvent = static_cast<int32_t>(EventDispatcherBase::EventSignal::Exit);
                break;
            }

            if (eventElem == nullptr)
            {
                break;
            }

            if ( prepare_dispatch_event(eventElem) )
            {
                dispatch_event(*eventElem);
            }

            post_dispatch_event(eventElem);

            // Drain internal events generated by the dispatch above
            Event* intEvent = mInternalEvents.is_empty() ? nullptr : mInternalEvents.pop_event();
            while (intEvent != nullptr)
            {
                if ( prepare_dispatch_event(intEvent) )
                {
                    dispatch_event(*intEvent);
                }

                post_dispatch_event(intEvent);
                intEvent = mInternalEvents.is_empty() ? nullptr : mInternalEvents.pop_event();
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

Event* EventDispatcherBase::pick_event() noexcept
{
    return mExternalEvents.pop_event();
}

bool EventDispatcherBase::prepare_dispatch_event( Event* eventElem ) noexcept
{
    return (eventElem != nullptr);
}

void EventDispatcherBase::post_dispatch_event( Event* eventElem )
{
    if (eventElem != nullptr)
    {
        eventElem->destroy();
    }
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
        EventConsumerList* listConsumers = mConsumerMap.find_resource_object(eventElem.class_id());
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

bool EventDispatcherBase::has_registered_consumer( const RuntimeClassID& whichClass ) const
{
    return mConsumerMap.exist(whichClass);
}

inline void EventDispatcherBase::_clean() noexcept
{
    mConsumerMap.lock();

    RuntimeClassID     Key(RuntimeClassID::empty_id());
    while (mConsumerMap.is_empty() == false)
    {
        mConsumerMap.resource_first_key(Key);
        EventConsumerList* Value =  mConsumerMap.unregister_resource_object(Key);
        Value->remove_all_consumers();
        delete Value;
    }

    mConsumerMap.unlock();
}

bool EventDispatcherBase::pulse_exit()
{
    return mEventExit.set_signaled();
}

} // namespace areg
