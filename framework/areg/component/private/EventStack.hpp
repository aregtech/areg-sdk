#ifndef AREG_COMPONENT_PRIVATE_EVENTSTACK_HPP
#define AREG_COMPONENT_PRIVATE_EVENTSTACK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventStack.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, non-concurrent FIFO event queue with front-insertion for Exit events.
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

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventStack class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A non-concurrent FIFO queue for Event pointers.
 *
 * All events are appended at the back (FIFO order). The only exception is
 * ExitPrio events: they are inserted at the front so the dispatcher processes
 * them immediately, ahead of any pending work.
 *
 * There is NO internal synchronization. For multi-threaded access (ExternalEventQueue),
 * the caller must hold an external lock before calling any mutating method. For
 * single-threaded use (InternalEventQueue), no lock is required.
 *
 * The queue enforces an optional maximum capacity. When the queue is full and a
 * non-Exit event is pushed, the oldest event at the back is evicted and either
 * returned to the caller or destroyed. Exit events always bypass the size limit.
 **/
class EventStack final : protected Stack<Event*>
{
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< Hard upper bound — effectively "unlimited" when assigned as mMaxQueueSize.
    static constexpr uint32_t MAX_QUEUE_SIZE { std::numeric_limits<uint32_t>::max() };
    //!< Minimum enforced queue capacity when a non-zero limit is requested.
    static constexpr uint32_t MIN_QUEUE_SIZE { 32u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the queue with an optional capacity limit.
     *
     * \param   maxQueue    Maximum number of events. Pass areg::IGNORE_VALUE (0) to read the
     *                      limit from the application configuration; if the configuration also
     *                      returns IGNORE_VALUE, the queue is unlimited.
     **/
    explicit EventStack(uint32_t maxQueue) noexcept;

    ~EventStack();

//////////////////////////////////////////////////////////////////////////
// Operations — caller must hold an external lock when shared across threads
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Destroys every event in the queue, including Exit events.
     **/
    void delete_all_events() noexcept;

    /**
     * \brief   Destroys all non-Exit events. Exit events are preserved at the front.
     *
     * \return  Number of remaining events (Exit events only, in practice).
     **/
    uint32_t delete_except_exit() noexcept;

    /**
     * \brief   Destroys all events whose runtime class ID matches \a eventClassId, except Exit events.
     *
     * \param   eventClassId    Runtime class ID to match and remove.
     * \return  Number of remaining events after the operation.
     **/
    uint32_t delete_matching_class(const RuntimeClassID& eventClassId) noexcept;

    /**
     * \brief   Destroys all events whose runtime class ID does NOT match \a eventClassId, except
     *          Exit events.
     *
     * \param   eventClassId    Runtime class ID of events to keep.
     * \return  Number of remaining events after the operation.
     **/
    uint32_t delete_except_class(const RuntimeClassID& eventClassId) noexcept;

    /**
     * \brief   Pushes an event onto the queue.
     *
     * Exit events (ExitPrio) are always inserted at the front and bypass the capacity limit.
     * All other events are appended at the back. If the queue is at capacity, the oldest event
     * at the back is evicted: it is returned via \a removedEvent if non-null, or destroyed
     * immediately otherwise.
     *
     * \param   newEvent        The event to push. Must not be nullptr.
     * \param[out] removedEvent If an event was evicted and this is not nullptr, receives it.
     *                          Caller takes ownership of the evicted event.
     * \return  Number of events in the queue after the push.
     **/
    uint32_t push_event(Event* newEvent, Event** removedEvent) noexcept;

    /**
     * \brief   Removes and returns the first event (FIFO order).
     *
     * \param[out] stackEvent   Receives the popped event, or nullptr if the queue is empty.
     *                          Must not be nullptr.
     * \return  Number of remaining events.
     **/
    [[nodiscard]]
    uint32_t pop_event(Event** stackEvent) noexcept;

    /**
     * \brief   Returns the configured maximum queue capacity.
     **/
    [[nodiscard]]
    inline constexpr uint32_t max_size() const noexcept;

    /**
     * \brief   Returns true if the queue contains no events.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the number of events currently in the queue.
     **/
    [[nodiscard]]
    inline uint32_t count() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Private helpers
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Computes the effective queue size from the requested value. Reads the application
     *          configuration when the caller passes areg::IGNORE_VALUE. Returns MAX_QUEUE_SIZE
     *          (unlimited) if neither the argument nor the configuration specifies a limit.
     **/
    static uint32_t _calc_queue_size(uint32_t requested) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    const uint32_t mMaxQueueSize; //!< Effective capacity limit for this queue instance.

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(EventStack);
};

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventStack inline implementation
//////////////////////////////////////////////////////////////////////////

inline constexpr uint32_t EventStack::max_size() const noexcept
{
    return mMaxQueueSize;
}

inline bool EventStack::is_empty() const noexcept
{
    return mValueList.empty();
}

inline uint32_t EventStack::count() const noexcept
{
    return static_cast<uint32_t>(mValueList.size());
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EVENTSTACK_HPP
