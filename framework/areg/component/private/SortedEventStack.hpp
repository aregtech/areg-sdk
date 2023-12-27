#ifndef AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
#define AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SortedEventStack.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, sorted event stack objects for internal and external event queues.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEStack.hpp"
#include "areg/component/Event.hpp"

class RuntimeClassID;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

/**
 * \brief   Sorted stack to store events by priority.
 *          Each time when event is pushed, it checks the priority
 *          of the event then inserts in stack, so that the events
 *          are sorted by the priority.
 *          In the stack the priorities are sorted in the following way:
 *          | Critical | High | Normal | Low |
 * 
 *              -   The "Critical" priority events are placed at the begin to be processed as soon as possible.
 *                  This priority is reserved for developers.
 *              -   The "Hight" priority events are placed before normal events, since they have some priorities.
 *                  For example, the connection events have high priority so that they are processed before normal events.
 *              -   The "Normal" priority events are usual events, which are processed in FIFO stack.
 *              -   The "Low" priority events are placed at the end to be processed only when the thread completed all jobs.
 * 
 *          The "Exit" events have reserved "Exit" priority. This priority is only for internal use and should not be used
 *          by other developers. The "Exit" events should be immediately processed and they are not removed from the 
 *          stack until they are not processed by thread dispatcher.
 **/
class SortedEventStack  : protected TELockStack<Event *>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SortedEventStack( void ) = default;

    ~SortedEventStack(void);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Deletes all events from the stack, except "Exit" event if present.
     **/
    void deleteAllEvents(void);

    /**
     * \brief   Deletes all events with priorities lower than the specified, except "Exit" event.
     * \param   eventPrio   The priority to check. Set Event::eEventPriority::EventPriorityIgnore
     *                      to remove all events. Only "Exit" events are untouched if they are present.
     * \return  Returns number of elements in the stack. Returns zero if empty.
     **/
    uint32_t deleteAllLowerPriority(Event::eEventPriority eventPrio);

    /**
     * \brief   Deletes all events, except those that are with the specified class ID, except "Exit" event.
     * \param   eventClassId    The class ID of the event to ignore to delete.
     * \return  Returns number of elements in the stack. Returns zero if empty.
     **/
    uint32_t deleteAllExceptClass(const RuntimeClassID& eventClassId);

    /**
     * \brief   Deletes all events with the specified priority, except "Exit" event, which should be processed.
     * \param   eventPrio   The priority of events to delete.
     * \return  Returns number of elements in the stack. Returns zero if empty.
     **/
    uint32_t deleteAllMatchPriority(Event::eEventPriority eventPrio);

    /**
     * \brief   Deletes all events, which match the specified class ID.
     * \param   eventClassId    The class ID of the event to delete.
     * \return  Returns number of elements in the stack. Returns zero if empty.
     **/
    uint32_t deleteAllMatchClass(const RuntimeClassID& eventClassId);

    /**
     * \brief   Pushes the event in the stack considering the priority, so that the events
     *          with the higher priority can be processed earlier.
     * \param   newEvent    The pointer to the event with the priority.
     * \return  Returns the number of elements in the stack.
     **/
    uint32_t pushEvent(Event * newEvent);

    /**
     * \brief   Pops the event from the FIFO stack.
     * \param   newEvent    The pointer to the previously allocated event object.
     * \return  Returns the number of elements in the stack.
     **/
    uint32_t popEvent(Event** stackEvent);

    /**
     * \brief   Returns true if the stack is empty.
     **/
    inline bool isEmpty(void) const;

    /**
     * \brief   Returns the number of elements in the stack.
     **/
    inline uint32_t getCount(void) const;

    /**
     * \brief   Locks the stack, so that the all other threads cannot access.
     * \return  Returns true, if succeeded to lock the stack.
     **/
    inline bool lockStack(void);

    /**
     * \brief   Unlocks the stack, so that all other threads cann access.
     **/
    inline void unlockStack(void);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Inserts the event at the end of the stack. No priority is considered.
     *          For example, the events with low priority are inserted at the end of the stack.
     **/
    inline void _insertAtEnd(Event * newEvent);

    /**
     * \brief   Inserts the event object at the end of the event elements list of specified priority.
     *          The function compares the priority of the events and inserts the element
     *          at the end of the list with events of the specified priority.
     *          The search is started at the end.
     * \param   newEvent    The event object to insert.
     * \param   eventPrio   The priority of event to search in the stack.
     **/
    inline void _insertAfterPrio(Event * newEvent, Event::eEventPriority eventPrio);

    /**
     * \brief   Inserts the event object before the elements of specified are queued.
     *          The function compares the priority of the events and inserts the element
     *          at the position before the events of specified priority start.
     *          The search is started at the begin.
     * \param   newEvent    The event object to insert.
     * \param   eventPrio   The priority of event to search in the stack.
     **/
    inline void _insertBeforePrio(Event * newEvent, Event::eEventPriority eventPrio);

    /**
     * \brief   Inserts the event element at the very begin of the stack, so that it is
     *          processed as soon as possible.
     **/
    inline void _insertAtBegin(Event * newEvent);

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(SortedEventStack);
};

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER


//////////////////////////////////////////////////////////////////////////
// SortedEventStack class inline implementation.
//////////////////////////////////////////////////////////////////////////

inline bool SortedEventStack::isEmpty(void) const
{
    Lock lock(mSynchObject);
    return mValueList.empty();
}

inline uint32_t SortedEventStack::getCount(void) const
{
    return static_cast<uint32_t>(mValueList.size());
}

inline bool SortedEventStack::lockStack(void)
{
    return lock();
}

inline void SortedEventStack::unlockStack(void)
{
    unlock();
}

#endif  // AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
