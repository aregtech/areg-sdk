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

SortedEventStack::SortedEventStack(uint32_t maxQueue)
    : areg::ConcurrentStack<areg::Event*>( )
    , mMaxQueueSize      (SortedEventStack::_calcQueueSize(maxQueue))
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

void SortedEventStack::deleteAllEvents()
{
    areg::Lock lock( mSyncObject );

    while ( mValueList.empty( ) == false )
    {
        areg::Event * evt = mValueList.back( );
        ASSERT( evt != nullptr );
        evt->destroy( );
        mValueList.pop_back( );
    }
}

uint32_t SortedEventStack::deleteAllLowerPriority(areg::Event::EventPriority eventPrio)
{
    areg::Lock lock(mSyncObject);

    while (mValueList.empty() == false)
    {
        areg::Event* evt = mValueList.back();
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

uint32_t SortedEventStack::deleteAllExceptClass(const areg::RuntimeClassID& eventClassId)
{
    areg::Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == areg::Event::EventPriority::ExitPrio)
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

uint32_t SortedEventStack::deleteAllMatchPriority(areg::Event::EventPriority eventPrio)
{
    areg::Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == areg::Event::EventPriority::ExitPrio)
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

uint32_t SortedEventStack::deleteAllMatchClass(const areg::RuntimeClassID& eventClassId)
{
    areg::Lock lock(mSyncObject);

    auto end = mValueList.end();
    for (auto it = mValueList.begin(); it != end; )
    {
        if ((*it)->getEventPriority() == areg::Event::EventPriority::ExitPrio)
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

uint32_t SortedEventStack::pushEvent(areg::Event * newEvent, areg::Event** removedEvent)
{
    ASSERT(newEvent != nullptr);
    areg::Lock lock(mSyncObject);
    switch (newEvent->getEventPriority())
    {
    case areg::Event::EventPriority::LowPrio:
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

    case areg::Event::EventPriority::NormalPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            areg::Event* removed = mValueList.back();
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

        _insertAfterPrio(newEvent, areg::Event::EventPriority::NormalPrio);
        break;

    case areg::Event::EventPriority::HighPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            areg::Event* removed = mValueList.back();
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

        _insertBeforePrio(newEvent, areg::Event::EventPriority::NormalPrio);
        break;

    case areg::Event::EventPriority::CriticalPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            areg::Event* removed = mValueList.back();
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

        _insertBeforePrio(newEvent, areg::Event::EventPriority::HighPrio);
        break;

    case areg::Event::EventPriority::ExitPrio:
        if (mValueList.size() >= mMaxQueueSize)
        {
            ASSERT(mValueList.empty() == false);
            areg::Event* removed = mValueList.back();
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

    case areg::Event::EventPriority::UndefinedPrio: // fall through
    case areg::Event::EventPriority::IgnorePrio:    // fall through
    default:
        ASSERT(false);
        break;
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t  SortedEventStack::popEvent(areg::Event** stackEvent)
{
    ASSERT(stackEvent != nullptr);

    areg::Lock lock(mSyncObject);
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

inline void SortedEventStack::_insertAtEnd(areg::Event* newEvent)
{
    mValueList.push_back(newEvent);
}

inline void SortedEventStack::_insertAfterPrio(areg::Event* newEvent, areg::Event::EventPriority eventPrio)
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

inline void SortedEventStack::_insertBeforePrio(areg::Event* newEvent, areg::Event::EventPriority eventPrio)
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

inline void SortedEventStack::_insertAtBegin(areg::Event* newEvent)
{
    mValueList.push_front(newEvent);
}

inline constexpr uint32_t SortedEventStack::_calcQueueSize(uint32_t requestedSize)
{
    if (requestedSize == areg::IGNORE_VALUE)
        requestedSize = areg::Application::getConfigManager().getDefaultMessageQueueSize();

    return (requestedSize != areg::IGNORE_VALUE ? std::max(MIN_QUEUE_SIZE, requestedSize) : MAX_QUEUE_SIZE);
}
