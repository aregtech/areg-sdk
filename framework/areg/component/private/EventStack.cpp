/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventStack.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, non-concurrent FIFO event queue implementation.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/EventStack.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/Event.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventStack — constructor / destructor
//////////////////////////////////////////////////////////////////////////

EventStack::EventStack(uint32_t maxQueue) noexcept
    : Stack<Event*>   ( )
    , mMaxQueueSize   ( EventStack::_calc_queue_size(maxQueue) )
{
}

EventStack::~EventStack()
{
    for (Event* evt : mValueList)
    {
        evt->destroy();
    }

    mValueList.clear();
}

//////////////////////////////////////////////////////////////////////////
// EventStack — bulk-removal operations
//////////////////////////////////////////////////////////////////////////

void EventStack::delete_all_events() noexcept
{
    for (Event* evt : mValueList)
    {
        evt->destroy();
    }

    mValueList.clear();
}

uint32_t EventStack::delete_except_exit() noexcept
{
    auto it = mValueList.begin();
    while (it != mValueList.end())
    {
        if ((*it)->event_priority() == areg::EventPriority::ExitPrio)
        {
            ++it;
        }
        else
        {
            (*it)->destroy();
            it = mValueList.erase(it);
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t EventStack::delete_matching_class(const RuntimeClassID& eventClassId) noexcept
{
    auto it = mValueList.begin();
    while (it != mValueList.end())
    {
        Event* evt = *it;
        if ((evt->event_priority() != areg::EventPriority::ExitPrio) && (eventClassId == evt->class_id()))
        {
            evt->destroy();
            it = mValueList.erase(it);
        }
        else
        {
            ++it;
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t EventStack::delete_except_class(const RuntimeClassID& eventClassId) noexcept
{
    auto it = mValueList.begin();
    while (it != mValueList.end())
    {
        Event* evt = *it;
        if ((evt->event_priority() != areg::EventPriority::ExitPrio) && (eventClassId != evt->class_id()))
        {
            evt->destroy();
            it = mValueList.erase(it);
        }
        else
        {
            ++it;
        }
    }

    return static_cast<uint32_t>(mValueList.size());
}

//////////////////////////////////////////////////////////////////////////
// EventStack — push / pop
//////////////////////////////////////////////////////////////////////////

uint32_t EventStack::push_event(Event* newEvent, Event** removedEvent) noexcept
{
    ASSERT(newEvent != nullptr);

    if (newEvent->event_priority() == areg::EventPriority::ExitPrio)
    {
        // Exit events always go to the front and bypass the capacity limit.
        mValueList.push_front(newEvent);
    }
    else
    {
        // Normal events are FIFO. Evict the oldest (back) entry if the queue is at capacity.
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(!mValueList.empty());
            Event* evicted = mValueList.back();

            // Do not evict an Exit event — they must not be lost.
            // If the back is an Exit event the queue is full of exits (pathological case);
            // drop the incoming event instead.
            if (evicted->event_priority() == areg::EventPriority::ExitPrio)
            {
                if (removedEvent != nullptr)
                    *removedEvent = newEvent;
                else
                    newEvent->destroy();

                return static_cast<uint32_t>(mValueList.size());
            }

            mValueList.pop_back();
            if (removedEvent != nullptr)
                *removedEvent = evicted;
            else
                evicted->destroy();
        }

        mValueList.push_back(newEvent);
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t EventStack::pop_event(Event** stackEvent) noexcept
{
    ASSERT(stackEvent != nullptr);

    if (!mValueList.empty())
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

//////////////////////////////////////////////////////////////////////////
// EventStack — private helper
//////////////////////////////////////////////////////////////////////////

uint32_t EventStack::_calc_queue_size(uint32_t requested) noexcept
{
    if (requested == areg::IGNORE_VALUE)
        requested = Application::config_manager().message_queue_size();

    return (requested != areg::IGNORE_VALUE ? std::max(MIN_QUEUE_SIZE, requested) : MAX_QUEUE_SIZE);
}

} // namespace areg
