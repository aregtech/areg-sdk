#ifndef AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
#define AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/SortedEventStack.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, sorted event stack objects for internal and external event queues.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Stack.hpp"
#include "areg/component/Event.hpp"
namespace areg {

class RuntimeClassID;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

/**
 * \brief   A priority queue for events. Events are sorted by priority: Critical (processed first) >
 *          High > Normal > Low > Exit. Exit events are processed immediately and are not removed
 *          until handled.
 **/
class SortedEventStack  : protected ConcurrentStack<Event *>
{
    //!< The maximum size of the event queue stack
    static constexpr uint32_t   MAX_QUEUE_SIZE  { std::numeric_limits<uint32_t>::max() };
    //!< The minimum size of the event queue stack
    static constexpr uint32_t   MIN_QUEUE_SIZE  { 32 };
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SortedEventStack( uint32_t maxQueue );

    ~SortedEventStack();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Deletes all events from the stack, except Exit events.
     **/
    void delete_all_events();

    /**
     * \brief   Deletes all events with priorities lower than the specified priority, except Exit
     *          events.
     *
     * \param   eventPrio       The priority threshold. Events with lower priority are deleted. Use
     *                          areg::EventPriority::IgnorePrio to remove all events except Exit
     *                          events.
     * \return  Returns the number of remaining events in the stack.
     **/
    uint32_t delete_lower_priority(areg::EventPriority eventPrio);

    /**
     * \brief   Deletes all events except those with the specified class ID and Exit events.
     *
     * \param   eventClassId    The class ID of events to keep.
     * \return  Returns the number of remaining events in the stack.
     **/
    uint32_t delete_except_class(const RuntimeClassID& eventClassId);

    /**
     * \brief   Deletes all events with the specified priority, except Exit events.
     *
     * \param   eventPrio       The priority of events to delete.
     * \return  Returns the number of remaining events in the stack.
     **/
    uint32_t delete_matching_priority(areg::EventPriority eventPrio);

    /**
     * \brief   Deletes all events with the specified class ID.
     *
     * \param   eventClassId    The class ID of events to delete.
     * \return  Returns the number of remaining events in the stack.
     **/
    uint32_t delete_matching_class(const RuntimeClassID& eventClassId);

    /**
     * \brief   Pushes an event onto the stack in priority order.
     *
     * \param   newEvent        The event to push. Higher-priority events are inserted before
     *                          lower-priority ones.
     * \param[out] removedEvent    On output, contains a pointer to any event removed (due to stack
     *                             overflow).
     * \return  Returns the number of events in the stack.
     **/
    uint32_t push_event(Event * newEvent, Event** removedEvent);

    /**
     * \brief   Pops the first event from the stack (FIFO order).
     *
     * \param[out] stackEvent      On output, contains a pointer to the popped event (may be nullptr
     *                             if stack is empty). Must not be nullptr itself.
     * \return  Returns the number of remaining events in the stack.
     **/
    uint32_t pop_event(Event** stackEvent);

    /**
     * \brief   Returns the maximum size of the stack.
     **/
    inline constexpr uint32_t max_size() const;

    /**
     * \brief   Returns true if the stack is empty.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the number of events in the stack.
     **/
    [[nodiscard]]
    inline uint32_t count() const noexcept;

    /**
     * \brief   Locks the stack to prevent access from other threads.
     *
     * \return  Returns true if the stack was successfully locked; false otherwise.
     **/
    inline bool lock_stack();

    /**
     * \brief   Unlocks the stack to allow access from other threads.
     **/
    inline void unlock_stack();

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Inserts an event at the end of the stack without considering priority.
     *
     * \param   newEvent    The event to insert. Used for low-priority events.
     **/
    inline void _insert_at_end(Event * newEvent);

    /**
     * \brief   Inserts an event at the end of all events with the specified priority.
     *
     * \param   newEvent        The event to insert.
     * \param   eventPrio       The priority of events after which to insert.
     **/
    inline void _insert_after_prio(Event * newEvent, areg::EventPriority eventPrio);

    /**
     * \brief   Inserts an event before all events with the specified priority.
     *
     * \param   newEvent        The event to insert.
     * \param   eventPrio       The priority of events before which to insert.
     **/
    inline void _insert_before_prio(Event * newEvent, areg::EventPriority eventPrio);

    /**
     * \brief   Inserts an event at the very beginning of the stack for immediate processing.
     *
     * \param   newEvent    The event to insert.
     **/
    inline void _insert_at_begin(Event * newEvent);

    /**
     * \brief   Calculates the appropriate queue size based on the requested size.
     *
     * \param   requestedSize       The requested queue size.
     * \return  Returns the calculated queue size.
     **/
    inline static constexpr uint32_t _calc_queue_size(uint32_t requestedSize);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    const uint32_t    mMaxQueueSize; //!< The maximum size of the stack.

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(SortedEventStack);
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// SortedEventStack class inline implementation.
//////////////////////////////////////////////////////////////////////////

inline bool SortedEventStack::is_empty() const noexcept
{
    Lock lock(lockable());
    return mValueList.empty();
}

inline uint32_t SortedEventStack::count() const noexcept
{
    return static_cast<uint32_t>(mValueList.size());
}

inline bool SortedEventStack::lock_stack()
{
    return lock();
}

inline void SortedEventStack::unlock_stack()
{
    unlock();
}

inline constexpr uint32_t SortedEventStack::max_size() const
{
    return mMaxQueueSize;
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_SORTEDEVENTSTACK_HPP
