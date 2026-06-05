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

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventStack class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A non-concurrent FIFO queue for Event values.
 *          Events are stored by value; the destructor is called automatically on removal.
 *          There is NO internal synchronization. For multi-threaded access,
 *          the caller must hold an external lock before calling any mutating method.
 **/
class AREG_API EventStack final : protected Stack<Event>
{
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< Hard upper bound -- effectively "unlimited" when assigned as mMaxQueueSize.
    static constexpr uint32_t MAX_QUEUE_SIZE { std::numeric_limits<uint32_t>::max() };
    //!< Minimum enforced queue capacity when a non-zero limit is requested.
    static constexpr uint32_t MIN_QUEUE_SIZE { 32u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the queue with an optional capacity limit.
     **/
    explicit EventStack() noexcept;

    ~EventStack() noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations -- caller must hold an external lock when shared across threads
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
    uint32_t delete_matching(uint32_t eventClassId) noexcept;

    /**
     * \brief   Destroys all events whose runtime class ID does NOT match \a eventClassId, except
     *          Exit events.
     *
     * \param   eventClassId    Runtime class ID of events to keep.
     * \return  Number of remaining events after the operation.
     **/
    uint32_t delete_except(uint32_t eventClassId) noexcept;

    /**
     * \brief   Pushes an event onto the queue by moving it in (O(1) shared_ptr transfer; no payload copy).
     *          The caller's event is left in a moved-from (empty/invalid) state after the push.
     *
     * \param   event   The event to push; moved in.
     * \return  Number of events in the queue after the push.
     **/
    inline uint32_t push_event(Event& event) noexcept;

    /**
     * \brief   Removes and returns the first event by move (FIFO order).
     *          Precondition: !is_empty().
     *
     * \return  The popped Event value.
     **/
    [[nodiscard]]
    inline Event pop_event() noexcept;

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
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(EventStack);
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventStack inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventStack::is_empty() const noexcept
{
    return mValueList.empty();
}

inline uint32_t EventStack::count() const noexcept
{
    return static_cast<uint32_t>(mValueList.size());
}

inline uint32_t EventStack::push_event(Event& event) noexcept
{
    mValueList.push_back(std::move(event));
    return static_cast<uint32_t>(mValueList.size());
}

inline Event EventStack::pop_event() noexcept
{
    ASSERT(!mValueList.empty());
    Event result{ std::move(mValueList.front()) };
    mValueList.pop_front();
    return result;
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EVENTSTACK_HPP
