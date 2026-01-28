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

SortedEventStack::SortedEventStack(uint32_t maxQueue)
    : TELockStack<Event*>( )
    , mMaxQueueSize      (SortedEventStack::_calcQueueSize(maxQueue))
{
}

SortedEventStack::~SortedEventStack(void)
{
    for (auto evt : mValueList)
    {
        evt->destroy();
    }

    mValueList.clear();
}

void SortedEventStack::deleteAllEvents(void)
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

uint32_t SortedEventStack::deleteAllLowerPriority(Event::eEventPriority eventPrio)
{
    Lock lock(mSyncObject);

    while (mValueList.empty() == false)
    {
        Event* evt = mValueList.back();
        ASSERT(evt != nullptr);
        if (evt->getEventPriority() < eventPrio)
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

uint32_t SortedEventStack::deleteAllExceptClass(const RuntimeClassID& eventClassId)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == Event::eEventPriority::EventPriorityExit)
        {
            it = std::next(it);
        }
        else if (eventClassId != (*it)->getRuntimeClassId())
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

uint32_t SortedEventStack::deleteAllMatchPriority(Event::eEventPriority eventPrio)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == Event::eEventPriority::EventPriorityExit)
        {
            it = std::next(it);
        }
        else if (eventPrio == (*it)->getEventPriority())
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

uint32_t SortedEventStack::deleteAllMatchClass(const RuntimeClassID& eventClassId)
{
    Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == Event::eEventPriority::EventPriorityExit)
        {
            it = std::next(it);
        }
        else if (eventClassId == (*it)->getRuntimeClassId())
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

uint32_t SortedEventStack::pushEvent(Event * newEvent, Event** OUT removedEvent)
{
    ASSERT(newEvent != nullptr);
    Lock lock(mSyncObject);
    switch (newEvent->getEventPriority())
    {
    case Event::eEventPriority::EventPriorityLow:
        if (mValueList.size() < mMaxQueueSize)
        {
            _insertAtEnd(newEvent);
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

    case Event::eEventPriority::EventPriorityNormal:
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

        _insertAfterPrio(newEvent, Event::eEventPriority::EventPriorityNormal);
        break;

    case Event::eEventPriority::EventPriorityHigh:
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

        _insertBeforePrio(newEvent, Event::eEventPriority::EventPriorityNormal);
        break;

    case Event::eEventPriority::EventPriorityCritical:
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

        _insertBeforePrio(newEvent, Event::eEventPriority::EventPriorityHigh);
        break;

    case Event::eEventPriority::EventPriorityExit:
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

        _insertAtBegin(newEvent);
        break;

    case Event::eEventPriority::EventPriorityUndefined: // fall through
    case Event::eEventPriority::EventPriorityIgnore:    // fall through
    default:
        ASSERT(false);
        break;
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t  SortedEventStack::popEvent(Event** OUT stackEvent)
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

inline void SortedEventStack::_insertAtEnd(Event* newEvent)
{
    mValueList.push_back(newEvent);
}

inline void SortedEventStack::_insertAfterPrio(Event* newEvent, Event::eEventPriority eventPrio)
{
    auto it = mValueList.end();
    if (mValueList.empty() == false)
    {
        const auto begin = mValueList.begin();
        it = std::prev(it);
        while ( (it != begin) && ((*it)->getEventPriority() < eventPrio) )
        {
            it = std::prev(it);
        }

        if ((*it)->getEventPriority() >= eventPrio)
        {
            it = std::next(it);
        }
    }

    mValueList.insert(it, newEvent);
}

inline void SortedEventStack::_insertBeforePrio(Event* newEvent, Event::eEventPriority eventPrio)
{
    auto it = mValueList.begin();
    if (mValueList.empty() == false)
    {
        const auto end = mValueList.end();
        while ((it != end) && ((*it)->getEventPriority() > eventPrio))
        {
            it = std::next(it);
        }
    }

    mValueList.insert(it, newEvent);
}

inline void SortedEventStack::_insertAtBegin(Event* newEvent)
{
    mValueList.push_front(newEvent);
}

inline constexpr uint32_t SortedEventStack::_calcQueueSize(uint32_t requestedSize)
{
    if (requestedSize == NECommon::IGNORE_VALUE)
        requestedSize = Application::getConfigManager().getDefaultMessageQueueSize();

    return (requestedSize != NECommon::IGNORE_VALUE ? MACRO_MAX(MIN_QUEUE_SIZE, requestedSize) : MAX_QUEUE_SIZE);
}
