/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SortedEventStack.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, External and internal event stack objects.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/component/private/SortedEventStack.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/Event.hpp"

#include <algorithm>
namespace areg {

SortedEventStack::SortedEventStack(uint32_t maxQueue)
    : ConcurrentStack<Event*>( )
    , mMaxQueueSize      (SortedEventStack::_calc_queue_size(maxQueue))
{
}

SortedEventStack::~SortedEventStack()
{
    for (auto evt : mValueList)
    {
        evt->destroy();
    }

    mValueList.clear();
}

void SortedEventStack::delete_all_events()
{
    Lock lock( mSyncObject );

    while ( mValueList.empty( ) == false )
    {
        Event * evt = mValueList.back( );
        ASSERT( evt != nullptr );
        evt->destroy( );
        mValueList.pop_back( );
    }
}

uint32_t SortedEventStack::delete_lower_priority(Event::EventPriority eventPrio)
{
    Lock lock(mSyncObject);

    while (mValueList.empty() == false)
    {
        Event* evt = mValueList.back();
        ASSERT(evt != nullptr);
        if (evt->event_priority() < eventPrio)
        {
            evt->destroy();
            mValueList.pop_back();
        }
        else
        {
            // Since the queue is sorted, ignore the rest
            break;
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t SortedEventStack::delete_except_class(const RuntimeClassID& eventClassId)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->event_priority() == Event::EventPriority::ExitPrio)
        {
            it = std::next(it);
        }
        else if (eventClassId != (*it)->runtime_class_id())
        {
            (*it)->destroy();
            it = mValueList.erase(it);
        }
        else
        {
            it = std::next(it);
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t SortedEventStack::delete_matching_priority(Event::EventPriority eventPrio)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->event_priority() == Event::EventPriority::ExitPrio)
        {
            it = std::next(it);
        }
        else if (eventPrio == (*it)->event_priority())
        {
            (*it)->destroy();
            it = mValueList.erase(it);
        }
        else
        {
            it = std::next(it);
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t SortedEventStack::delete_matching_class(const RuntimeClassID& eventClassId)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->event_priority() == Event::EventPriority::ExitPrio)
        {
            it = std::next(it);
        }
        else if (eventClassId == (*it)->runtime_class_id())
        {
            (*it)->destroy();
            it = mValueList.erase(it);
        }
        else
        {
            it = std::next(it);
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t SortedEventStack::push_event(Event * newEvent, Event** removedEvent)
{
    ASSERT(newEvent != nullptr);
    Lock lock(mSyncObject);
    switch (newEvent->event_priority())
    {
    case Event::EventPriority::LowPrio:
        if (mValueList.size() < mMaxQueueSize)
        {
            _insert_at_end(newEvent);
        }
        else if (removedEvent != nullptr)
        {
            *removedEvent = newEvent;
        }
        else
        {
            newEvent->destroy();
        }
        break;

    case Event::EventPriority::NormalPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            Event* removed = mValueList.back();
            mValueList.pop_back();
            if (removedEvent != nullptr)
            {
                *removedEvent = removed;
            }
            else
            {
                removed->destroy();
            }
        }

        _insert_after_prio(newEvent, Event::EventPriority::NormalPrio);
        break;

    case Event::EventPriority::HighPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            Event* removed = mValueList.back();
            mValueList.pop_back();
            if (removedEvent != nullptr)
            {
                *removedEvent = removed;
            }
            else
            {
                removed->destroy();
            }
        }

        _insert_before_prio(newEvent, Event::EventPriority::NormalPrio);
        break;

    case Event::EventPriority::CriticalPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            Event* removed = mValueList.back();
            mValueList.pop_back();
            if (removedEvent != nullptr)
            {
                *removedEvent = removed;
            }
            else
            {
                removed->destroy();
            }
        }

        _insert_before_prio(newEvent, Event::EventPriority::HighPrio);
        break;

    case Event::EventPriority::ExitPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            Event* removed = mValueList.back();
            mValueList.pop_back();
            if (removedEvent != nullptr)
            {
                *removedEvent = removed;
            }
            else
            {
                removed->destroy();
            }
        }

        _insert_at_begin(newEvent);
        break;

    case Event::EventPriority::UndefinedPrio: // fall through
    case Event::EventPriority::IgnorePrio:    // fall through
    default:
        ASSERT(false);
        break;
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t  SortedEventStack::pop_event(Event** stackEvent)
{
    ASSERT(stackEvent != nullptr);

    Lock lock(mSyncObject);
    if (mValueList.empty() == false)
    {
        *stackEvent = mValueList.front();
        mValueList.pop_front();
    }
    else
    {
        *stackEvent = nullptr;
    }

    return static_cast<uint32_t>(mValueList.size());
}

inline void SortedEventStack::_insert_at_end(Event* newEvent)
{
    mValueList.push_back(newEvent);
}

inline void SortedEventStack::_insert_after_prio(Event* newEvent, Event::EventPriority eventPrio)
{
    auto it = mValueList.end();
    if (mValueList.empty() == false)
    {
        const auto begin = mValueList.begin();
        it = std::prev(it);
        while ( (it != begin) && ((*it)->event_priority() < eventPrio) )
        {
            it = std::prev(it);
        }

        if ((*it)->event_priority() >= eventPrio)
        {
            it = std::next(it);
        }
    }

    mValueList.insert(it, newEvent);
}

inline void SortedEventStack::_insert_before_prio(Event* newEvent, Event::EventPriority eventPrio)
{
    auto it = mValueList.begin();
    if (mValueList.empty() == false)
    {
        const auto end = mValueList.end();
        while ((it != end) && ((*it)->event_priority() > eventPrio))
        {
            it = std::next(it);
        }
    }

    mValueList.insert(it, newEvent);
}

inline void SortedEventStack::_insert_at_begin(Event* newEvent)
{
    mValueList.push_front(newEvent);
}

inline constexpr uint32_t SortedEventStack::_calc_queue_size(uint32_t requestedSize)
{
    if (requestedSize == areg::IGNORE_VALUE)
        requestedSize = Application::config_manager().message_queue_size();

    return (requestedSize != areg::IGNORE_VALUE ? std::max(MIN_QUEUE_SIZE, requestedSize) : MAX_QUEUE_SIZE);
}

} // namespace areg
