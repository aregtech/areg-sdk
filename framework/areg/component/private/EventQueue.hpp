#ifndef AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, event queue classes for internal and external event dispatch.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/private/EventStack.hpp"
#include "areg/component/private/QueueListener.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Event;
    class RuntimeClassID;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventQueue class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Base class for event queues. Wraps a EventStack and a QueueListener and
 *          provides push/pop/remove operations. The base class performs no locking -- derived
 *          classes are responsible for thread safety.
 *
 * \note    push_event() and pop_event() are non-virtual. ExternalEventQueue hides them with
 *          locked versions. All callers in EventDispatcherBase use the concrete derived types
 *          directly, so no virtual dispatch is required on the hot path.
 **/
class AREG_API EventQueue
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the queue with the given listener and backing stack.
     *
     * \param   eventListener   Listener to notify on push/remove operations.
     * \param   messageQueue    Backing FIFO stack for this queue instance.
     **/
    EventQueue(QueueListener& eventListener, EventStack& messageQueue);

    virtual ~EventQueue() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the queue contains no events.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Moves an event into the queue (O(1) shared_ptr transfer; no payload copy) and
     *          notifies the listener. The caller's event is left in a moved-from (empty) state.
     *
     * \param   eventElem       The event to enqueue; moved in.
     **/
    inline void push_event(Event& eventElem);

    /**
     * \brief   Pops the first event from the queue by move. Notifies the listener.
     *          Precondition: !is_empty().
     *
     * \return  The popped Event value.
     **/
    [[nodiscard]]
    inline Event pop_event() noexcept;

    /**
     * \brief   Removes all non-Exit events from the queue and notifies the listener.
     **/
    inline void remove_events() noexcept;

    /**
     * \brief   Removes all events with the specified runtime class ID (except ExitPrio) and
     *          notifies the listener.
     *
     * \param   eventClassId    Runtime class ID of events to remove.
     **/
    inline void remove_events(uint32_t eventClassId) noexcept;

    /**
     * \brief   Removes every event from the queue (including Exit events) and resets the
     *          listener signal.
     **/
    inline void remove_all_events() noexcept;

//////////////////////////////////////////////////////////////////////////
// Members -- protected so derived classes can implement locked variants
//////////////////////////////////////////////////////////////////////////
protected:
    QueueListener&  mEventListener; //!< Listener notified on queue state changes.
    EventStack&     mEventQueue;    //!< Backing FIFO stack (no internal lock).

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    EventQueue() = delete;
    AREG_NOCOPY_NOMOVE(EventQueue);
};

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Single-threaded event queue for internal (intra-thread) events.
 *
 *          Proxy notification events and other intra-component signals are pushed and popped
 *          exclusively within the owner dispatcher thread. No locking is performed; the
 *          base EventQueue methods are used directly on the lock-free EventStack.
 **/
class AREG_API InternalEventQueue final : public    EventQueue
                                        , private   QueueListener
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the internal event queue with an optional capacity limit.
     *
     * \param   maxQueue    Maximum event capacity. Pass areg::IGNORE_VALUE (0) to read from
     *                      application configuration or use an unlimited queue.
     **/
    explicit InternalEventQueue();

    virtual ~InternalEventQueue();

//////////////////////////////////////////////////////////////////////////
// QueueListener override -- intentional no-op
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   No-op: the internal queue does not drive a SyncEvent. The dispatcher checks
     *          the internal queue proactively after each external event, so no signal is
     *          needed.
     **/
    void signal_event(uint32_t eventCount) final;

    /**
     * \brief   Returns a reference to this object (used in constructor initializer list).
     **/
    inline InternalEventQueue& self() noexcept;

//////////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////////
private:
    EventStack    mStack; //!< Backing FIFO queue; accessed from a single thread.

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(InternalEventQueue);
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventQueue::is_empty() const noexcept
{
    return mEventQueue.is_empty();
}

inline void EventQueue::push_event(Event& eventElem)
{
    mEventListener.signal_event(mEventQueue.push_event(eventElem));
}

inline Event EventQueue::pop_event() noexcept
{
    Event result{ mEventQueue.pop_event() };
    mEventListener.signal_event(mEventQueue.count());
    return result;
}

inline void EventQueue::remove_all_events() noexcept
{
    mEventQueue.delete_all_events();
    mEventListener.signal_event(0);
}

inline void EventQueue::remove_events() noexcept
{
    const uint32_t remain = mEventQueue.delete_except_exit();
    mEventListener.signal_event(remain);
}

inline void EventQueue::remove_events(uint32_t eventClassId) noexcept
{
    const uint32_t remain = mEventQueue.delete_matching(eventClassId);
    mEventListener.signal_event(remain);
}

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline InternalEventQueue& InternalEventQueue::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EVENTQUEUE_HPP
