/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SortedEventStack.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, External and internal event stack objects.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/component/private/SortedEventStack.hpp"

#include "areg/component/Event.hpp"

SortedEventStack::~SortedEventStack(void)
{
    mValueList.clear();
}

void SortedEventStack::deleteAllEvents(void)
{
    deleteAllLowerPriority(Event::eEventPriority::EventPriorityIgnore);
}

uint32_t SortedEventStack::deleteAllLowerPriority(Event::eEventPriority eventPrio)
{
    Lock lock(mSynchObject);

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
    Lock lock(mSynchObject);

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
    Lock lock(mSynchObject);

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
    Lock lock(mSynchObject);

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

uint32_t SortedEventStack::pushEvent(Event * newEvent)
{
    ASSERT(newEvent != nullptr);
    Lock lock(mSynchObject);
    switch (newEvent->getEventPriority())
    {
    case Event::eEventPriority::EventPriorityLow:
        _insertAtEnd(newEvent);
        break;

    case Event::eEventPriority::EventPriorityNormal:
        _insertAfterPrio(newEvent, Event::eEventPriority::EventPriorityNormal);
        break;

    case Event::eEventPriority::EventPriorityHigh:
        _insertBeforePrio(newEvent, Event::eEventPriority::EventPriorityNormal);
        break;

    case Event::eEventPriority::EventPriorityCritical:
        _insertBeforePrio(newEvent, Event::eEventPriority::EventPriorityHigh);
        break;

    case Event::eEventPriority::EventPriorityExit:
        _insertAtBegin(newEvent);
        break;

    default:
        ASSERT(false);
        break;
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t  SortedEventStack::popEvent(Event** stackEvent)
{
    ASSERT(stackEvent != nullptr);

    Lock lock(mSynchObject);
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
