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

#include "areg/base/private/DebugDefs.hpp"

#include <atomic>
#include <chrono>
#if defined(__GLIBC__)
    #include <malloc.h>     // malloc_trim(): hand a drained backlog's pages back to the OS
#endif  // __GLIBC__

namespace
{
    constexpr uint32_t      HEAP_TRIM_EVENT_THRESHOLD   { 100000u };    //!< events drained since last trim

    //!< Returns freed heap pages to the OS.
    //!< glibc keeps a slow consumer's drained backlog mapped at the RSS high-water mark (per-arena retention);
    //!< malloc_trim(0) releases it.
    inline void _release_heap( void ) noexcept
    {
#if defined(__GLIBC__)
        ::malloc_trim( 0 );
#endif  // __GLIBC__
    }

}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcherBase::EventDispatcherBase(const String & name, uint32_t maxQeueue)
    : mDispatcherName( name )
    , mExternalEvents( maxQeueue )
    , mInternalEvents( )
    , mHasStarted    ( false )
    , mConsumerMap   ( )
{
}

EventDispatcherBase::~EventDispatcherBase()
{
    mHasStarted = false;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, methods
//////////////////////////////////////////////////////////////////////////

bool EventDispatcherBase::start_dispatcher()
{
    mExternalEvents.reset_exit();
    return run_dispatcher( );
}

void EventDispatcherBase::stop_dispatcher() noexcept
{
    mExternalEvents.trigger_exit();
}

void EventDispatcherBase::exit_dispatcher() noexcept
{
    mInternalEvents.remove_all_events();
    mExternalEvents.remove_all_events();

    signal_exit_event();
}

void EventDispatcherBase::shutdown_dispatcher() noexcept
{
    stop_dispatcher();
}

bool EventDispatcherBase::queue_event( Event& eventElem )
{
    areg::EventType eventType = eventElem.event_type();
    ASSERT(areg::is_valid(eventType)); // eventType == 0 means event was never properly initialized
    if (mHasStarted.load(std::memory_order_relaxed))
    {
        if (areg::is_external(eventType))
        {
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

    bool isExit{ false };               // true once the ExitEvent is dequeued -> leave the loop
    uint64_t processedSinceTrim{ 0u };  // events drained since the last heap trim (this thread)

    do
    {
        // Tight drain loop: process all available events before considering a wait.
        for (;;)
        {
            Event eventElem = pick_event();  // returns Event by value; invalid if queue empty
            if (!eventElem.is_valid() || eventElem.is_exit_prio())
            {
                isExit = eventElem.is_exit_prio();
                break;
            }

            if ( prepare_dispatch_event(eventElem) )
            {
#if defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
                const uint64_t _ltDisp{ AREG_LT_NOW() };
                dispatch_event(eventElem);
                AREG_LT_SAMPLE(areg::LtStage::CompDispatch, AREG_LT_NOW() - _ltDisp);
#else   // defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
                dispatch_event(eventElem);
#endif  // defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
            }

            post_dispatch_event(eventElem);

            // Drain internal events generated by the dispatch above
            while (!mInternalEvents.is_empty())
            {
                Event intEvent{ mInternalEvents.pop_event() };
                if (prepare_dispatch_event(intEvent))
                    dispatch_event(intEvent);
            }

            ++processedSinceTrim;
        }

        // Queue drained, this dispatcher is going idle.
        if ( isExit || processedSinceTrim >= HEAP_TRIM_EVENT_THRESHOLD )
        {
            _release_heap();
            processedSinceTrim = 0u;
        }

        if (isExit)
            break;

        // Block until a producer pushes or exit is triggered (queue owns the wake-up).
        mExternalEvents.wait_event(areg::WAIT_INFINITE);

    } while (true);

    ready_for_events(false);
    remove_all_events();
    _clean();

    return isExit;
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
    signal_exit_event();
    return true;
}

} // namespace areg
