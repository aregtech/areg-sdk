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
 *          provides push/pop/remove operations. The base class performs no locking — derived
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
     * \brief   Pushes an event into the queue and notifies the listener. If the queue is full
     *          and an event is evicted, it is either returned via removedEvent or destroyed.
     *
     * \param   eventElem       The event to enqueue.
     * \param[out] removedEvent If an event was evicted and this is not nullptr, receives it.
     **/
    void push_event(Event& eventElem, Event** removedEvent);

    /**
     * \brief   Pops the first event from the queue. Notifies the listener if the queue becomes
     *          empty.
     *
     * \return  The popped event pointer, or nullptr if the queue was empty.
     **/
    Event* pop_event() noexcept;

    /**
     * \brief   Removes all non-Exit events from the queue and notifies the listener.
     *
     * \param   keepSpecials    Reserved for future use; currently both values remove all events
     *                          except ExitPrio events, since non-Exit priorities are not used
     *                          in practice.
     **/
    void remove_events(bool keepSpecials) noexcept;

    /**
     * \brief   Removes all events with the specified runtime class ID (except ExitPrio) and
     *          notifies the listener.
     *
     * \param   eventClassId    Runtime class ID of events to remove.
     **/
    void remove_events(const RuntimeClassID& eventClassId) noexcept;

    /**
     * \brief   Removes every event from the queue (including Exit events) and resets the
     *          listener signal.
     **/
    void remove_all_events() noexcept;

//////////////////////////////////////////////////////////////////////////
// Members — protected so derived classes can implement locked variants
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

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

/**
 * \brief   Thread-safe event queue for external (cross-thread and IPC) events.
 *
 * Multiple producer threads push events concurrently; a single consumer thread (the
 * dispatcher's owner thread) pops them. All mutating operations acquire an internal
 * ResourceLock. lock_queue() / unlock_queue() expose the same lock for callers that
 * need to bundle several operations atomically (e.g., check mHasStarted, drain queue,
 * then push an ExitEvent).
 *
 * ResourceLock is recursive: nested locking from the same thread (e.g., lock_queue()
 * followed by push_event()) is safe.
 **/
class AREG_API ExternalEventQueue : public EventQueue
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the external event queue with a listener and optional capacity limit.
     *
     * \param   eventListener   Listener notified on push/empty transitions.
     * \param   maxQueue        Maximum event capacity. Pass areg::IGNORE_VALUE (0) to read from
     *                          application configuration or use an unlimited queue.
     **/
    ExternalEventQueue(QueueListener& eventListener, uint32_t maxQueue);

    virtual ~ExternalEventQueue();

//////////////////////////////////////////////////////////////////////////
// Thread-safe operations (lock the internal ResourceLock)
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Acquires the internal lock. Blocks until the lock is available. Recursive:
     *          the same thread may call lock_queue() multiple times without deadlocking.
     **/
    inline void lock_queue() noexcept;

    /**
     * \brief   Releases a single acquisition of the internal lock.
     **/
    inline void unlock_queue() noexcept;

    /**
     * \brief   Thread-safe push. Acquires the internal lock, pushes the event onto the FIFO
     *          queue, and notifies the listener — all under the same lock acquisition.
     *
     * \param   eventElem       The event to enqueue.
     * \param[out] removedEvent If an event was evicted and this is not nullptr, receives it.
     **/
    void push_event(Event& eventElem, Event** removedEvent);

    /**
     * \brief   Thread-safe pop. Acquires the internal lock, removes the front event, and
     *          notifies the listener if the queue becomes empty.
     *
     * \return  The popped event, or nullptr if the queue was empty.
     **/
    [[nodiscard]]
    Event* pop_event() noexcept;

    /**
     * \brief   Thread-safe removal of all non-Exit events. Acquires the internal lock.
     *
     * \param   keepSpecials    Passed to the base implementation (currently a no-op parameter;
     *                          all non-Exit events are removed regardless).
     **/
    void remove_events(bool keepSpecials) noexcept;

    /**
     * \brief   Thread-safe removal of events matching the given class ID. Acquires the internal
     *          lock.
     *
     * \param   eventClassId    Runtime class ID of events to remove.
     **/
    void remove_events(const RuntimeClassID& eventClassId) noexcept;

    /**
     * \brief   Thread-safe removal of every event, including Exit events. Acquires the internal
     *          lock.
     **/
    void remove_all_events() noexcept;

//////////////////////////////////////////////////////////////////////////
// Members
//////////////////////////////////////////////////////////////////////////
private:
    ResourceLock    mLock;  //!< Recursive lock protecting mStack from concurrent access.
    EventStack      mStack; //!< Backing FIFO queue; lock-free on its own.

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    ExternalEventQueue() = delete;
    AREG_NOCOPY_NOMOVE(ExternalEventQueue);
};

//////////////////////////////////////////////////////////////////////////
// InternalEventQueue class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Single-threaded event queue for internal (intra-thread) events.
 *
 * Proxy notification events and other intra-component signals are pushed and popped
 * exclusively within the owner dispatcher thread. No locking is performed; the
 * base EventQueue methods are used directly on the lock-free EventStack.
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
    explicit InternalEventQueue(uint32_t maxQueue);

    virtual ~InternalEventQueue();

//////////////////////////////////////////////////////////////////////////
// QueueListener override — intentional no-op
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

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool EventQueue::is_empty() const noexcept
{
    return mEventQueue.is_empty();
}

//////////////////////////////////////////////////////////////////////////
// ExternalEventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline void ExternalEventQueue::lock_queue() noexcept
{
    mLock.lock(areg::WAIT_INFINITE);
}

inline void ExternalEventQueue::unlock_queue() noexcept
{
    mLock.unlock();
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
