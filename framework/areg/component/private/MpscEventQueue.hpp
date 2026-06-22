#ifndef AREG_COMPONENT_PRIVATE_MPSCEVENTQUEUE_HPP
#define AREG_COMPONENT_PRIVATE_MPSCEVENTQUEUE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/MpscEventQueue.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lock-free MPSC (Multi-Producer, Single-Consumer) event queue.
 *              Based on Dmitry Vyukov's MPSC queue algorithm.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <cstdint>
#include <deque>
#include <limits>
#include <new>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
#include "areg/component/Event.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4324 4251)
#endif  // _MSC_VER

/**
 * \brief   Lock-free MPSC (multiple-producer / single-consumer) event queue.
 *
 *          Drop-in replacement for ExternalEventQueue. The hot push path
 *          performs a single atomic exchange instead of acquiring a recursive
 *          OS mutex, eliminating kernel-mode transitions per normal event.
 *
 *          HighPrio and CriticalPrio events use a separate SpinLock-guarded
 *          priority deque to preserve front-insertion ordering semantics.
 *          SpinLock is recursive (owns mOwner + mCount) - same-thread
 *          re-entry is safe.
 *
 *          The queue fully owns the consumer wake-up: a private manual-reset
 *          SyncEvent doorbell is set on every push, reset only by the consumer
 *          inside wait_event() (lost-wakeup-free eventcount discipline), and
 *          forced signaled by trigger_exit(). ExitPrio is never queued.
 *
 * \note    pop_event() and wait_event() must be called from a single consumer
 *          thread only. push_event() is safe from any number of producer threads.
 **/
class AREG_API MpscEventQueue
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Singly-linked node for the Vyukov MPSC algorithm.
     *
     * One Node is allocated per enqueued normal-priority event and freed
     * after it is consumed. A permanent stub/sentinel is pre-allocated in
     * the constructor and lives for the lifetime of the queue.
     *
     * Invariants:
     *   mHead - last consumed node (initially the stub).
     *           mHead->next is the next item to dequeue.
     *           Written by the consumer thread only.
     *   mTail - last published node (initially the stub).
     *           Producers atomically exchange mTail on each push.
     **/
    struct Node
    {
        std::atomic<Node*>  next    { nullptr };
        Event               event   {         };    //!< default (empty) on the stub node.
#ifdef AREG_LATENCY_TRACE
        uint64_t            lt_ns   { 0u };          //!< Phase-0 latency trace: enqueue timestamp (ns). Carried to dequeue.
#endif
    };

    /**
     * \brief   Tagged-pointer head for the lock-free LIFO node pool.
     *          The sequence counter prevents ABA on the CAS loop.
     *          Requires 16-byte native CAS (x86-64 cmpxchg16b / ARM64 CASP).
     **/
#if defined(_WIN64) || defined(__LP64__) || defined(__aarch64__)
    struct alignas(16) PoolTop
    {
        Node* head{ nullptr };
        uintptr_t seq{ 0u };
    };
#else
     // 32-bit: 64-bit CAS (cmpxchg8b on x86 / LDREXD+STREXD on ARMv6K+)
    struct alignas(8) PoolTop
    {
        Node* head{ nullptr };
        uint32_t  seq{ 0u };
    };
#endif

    //////////////////////////////////////////////////////////////////////////
    // NODE_POOL_PRE_ALLOC nodes are pre-populated in the constructor to
    // avoid cold-start heap pressure during the initial message burst.
    // NODE_POOL_MAX caps the pool depth so idle queues do not hold excessive memory.
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t   NODE_POOL_PRE_ALLOC { 128u  }; //!< Nodes pre-allocated in constructor.
    static constexpr uint32_t   NODE_POOL_MAX       { 1024u }; //!< Maximum pool depth.

    //////////////////////////////////////////////////////////////////////////
    // AREG_MPSC_CACHE_LINE_SIZE
    //
    // Separates producer-written (mTail) from consumer-written (mHead) to
    // prevent false sharing on multi-core hardware.
    //
    // 128 bytes covers both x86/x86-64 (64-byte lines, wastes 64 bytes of
    // padding but harmless) and Apple M-series / future ARMs (128-byte lines).
    // Change this constant if targeting a platform with a different cache line.
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t   AREG_MPSC_CACHE_LINE_SIZE{ 128u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs the queue and pre-allocates the stub node.
     *
     * \param   maxQueue        Soft capacity limit for normal-priority events.
     *                          Pass areg::IGNORE_VALUE (0) to read from
     *                          application configuration or use unlimited.
     **/
    explicit MpscEventQueue(uint32_t maxQueue);

    ~MpscEventQueue();

//////////////////////////////////////////////////////////////////////////
// Public interface - mirrors ExternalEventQueue
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Acquires the priority-lane SpinLock (recursive).
     *          Used by the dispatcher to gate mHasStarted against producers
     *          without a TOCTOU race.
     **/
    inline void lock_queue() noexcept;

    /**
     * \brief   Releases one acquisition of the priority-lane SpinLock.
     **/
    inline void unlock_queue() noexcept;

    /**
     * \brief   Returns true if the consumer has something to pop: a queued event
     *          or a pending exit. Non-blocking; safe from any thread.
     **/
    inline bool has_pending() const noexcept;

    /**
     * \brief   Returns true if exit was triggered for this queue.
     **/
    inline bool is_exit_triggered() const noexcept;

    /**
     * \brief   Requests exit: sets the sticky exit flag and wakes every consumer
     *          blocked in wait_event(). After this, pop_event() returns the
     *          singleton ExitEvent until reset_exit() is called.
     **/
    inline void trigger_exit() noexcept;

    /**
     * \brief   Clears the sticky exit flag. Must be called only when the owner
     *          dispatcher (re)starts, single-threaded with respect to the queue.
     **/
    inline void reset_exit() noexcept;

    /**
     * \brief   Blocks the single consumer thread until the queue has something to
     *          pop (a queued event or a pending exit), or the timeout elapses.
     *
     * \param   timeout     Milliseconds to wait. areg::WAIT_INFINITE blocks until signaled.
     *                      areg::DO_NOT_WAIT polls without blocking.
     * \return  true if there is something to pop (or the doorbell was signaled);
     *          false on timeout.
     **/
    bool wait_event(uint32_t timeout = areg::WAIT_INFINITE) noexcept;

    /**
     * \brief   Queues an event by moving it into the queue. The event's shared buffer is
     *          transferred (O(1) -- no data copy). The caller's event is left in a moved-from
     *          (empty/invalid) state after a successful push. On capacity overflow or node
     *          allocation failure the event is NOT queued and remains in eventElem.
     *
     * \param   eventElem       Event to queue (moved in on success).
     * \param[out] removedEvent If non-null, receives the event on overflow/failure instead of
     *                          discarding it. Pass nullptr to discard automatically.
     **/
    void push_event(Event& eventElem, Event* removedEvent = nullptr);

    /**
     * \brief   Dequeues the next event. Priority lane is always drained first.
     *          Returns an invalid Event (is_valid() == false) when both lanes are empty.
     **/
    [[nodiscard]]
    Event pop_event() noexcept;

    /**
     * \brief   Destroys all non-Exit events in both lanes.
     *          Must not be called concurrently with push_event().
     **/
    void remove_events() noexcept;

    /**
     * \brief   Destroys all events whose runtime class ID matches
     *          \a eventClassId (except ExitPrio) in both lanes.
     *          Must not be called concurrently with push_event().
     **/
    void remove_events(const uint32_t eventClassId) noexcept;

    /**
     * \brief   Destroys every event in both lanes.
     *          Must not be called concurrently with push_event().
     **/
    void remove_all_events() noexcept;

    /**
     * \brief   Enqueues up to \a count events with a single priority-lane lock acquisition.
     *          Each event is moved into the queue. On capacity overflow the event remains in its
     *          original slot (not moved) and that slot index is compacted into the front of
     *          \a eventElems, with the overflow count returned.
     *
     * \param[in,out]   eventElems  Array of Event values sorted by priority. Events that could
     *                              not be enqueued due to capacity overflow are moved to the front
     *                              of the array. Must not be nullptr.
     * \param           count       Number of valid Events in \a eventElems.
     * \return  Number of events NOT enqueued (capacity overflow). 0 = all enqueued.
     **/
    uint32_t push_events(Event* eventElems, uint32_t count);

    /**
     * \brief   Dequeues up to \a count events into the caller-supplied Event array.
     *          Priority lane is always drained before the fast lane.
     *
     * \param[out]      eventElems  Caller-supplied array of at least \a count Event objects.
     *                              On output, \a eventElems[0..<returnValue>) hold the dequeued
     *                              events (moved in), highest-priority first.
     *                              Must not be nullptr.
     * \param           count       Maximum number of events to dequeue.
     * \return  Number of events dequeued. 0 if both lanes are empty.
     **/
    uint32_t pop_events(Event* eventElems, uint32_t count);

//////////////////////////////////////////////////////////////////////////
// Private helpers
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Publishes \a node onto the MPSC fast lane (Vyukov algorithm).
     *          One atomic exchange - safe from multiple producer threads.
     **/
    inline void _mpsc_push(Node* node) noexcept;

    /**
     * \brief   Removes one node from the MPSC fast lane.
     *          Consumer thread only. Returns nullptr when the lane is empty.
     *          Caller must call _free_node() on the returned node.
     **/
    [[nodiscard]]
    inline Node* _mpsc_pop() noexcept;

    /**
     * \brief   Drains the entire fast lane into \a out (Events moved by value).
     *          Safe only when no producers are active (control path).
     **/
    void _mpsc_drain_to(std::vector<Event>& out) noexcept;

    /**
     * \brief   Drains the entire fast lane into \a out, keeping only events whose class
     *          ID does NOT match \a classId (matching events are destroyed immediately).
     *          Safe only when no producers are active (control path).
     **/
    void _mpsc_drain_to(std::vector<Event>& out, const uint32_t classId) noexcept;

    /**
     * \brief   Returns a Node from the lock-free pool, or allocates a new one if the pool is empty.
     **/
    [[nodiscard]]
    Node* _alloc_node() noexcept;

    /**
     * \brief   Returns a Node to the lock-free pool. If the pool is at capacity, deletes the node.
     **/
    void _free_node(Node* node) noexcept;

    /**
     * \brief   Returns the number of elements in priority queue.
     **/
    [[nodiscard]]
    inline uint32_t _prio_count() noexcept;

    inline void _delete_nodes() noexcept;

    /**
     * \brief   Computes the effective capacity limit.
     *          0 -> unlimited (UINT32_MAX cap); any other value -> max(32, requested).
     **/
    [[nodiscard]]
    static uint32_t _calc_capacity(uint32_t requested) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    //!<  Producer-written tail - own cache line to avoid false sharing with the consumer's mHead reads
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<Node*> mTail;

    //!< Consumer-written head - own cache line to avoid false sharing with producer writes to mTail
    //!< mHead is always the current sentinel; deleted in the destructor
    alignas(AREG_MPSC_CACHE_LINE_SIZE) Node*              mHead;

    //!< Fast-lane item count. Incremented by producers, decremented by the consumer
    std::atomic<uint32_t>   mFastCount; //!< Current fast-lane depth
    const uint32_t          mCapacity;  //!< Soft cap (MAX_UINT = unlimited)

    //!< Priority lane - Critical at front, then descending priority order.
    SpinLock                mPrioLock;  //!< Recursive guard for mPrioQueue
    std::deque<Event>       mPrioQueue; //!< [Critical-][High-] ordered (stored by value)
    std::atomic_uint32_t    mPrioCount; //!< The number of elements in mPrioQueue

    //!< Consumer wake-up doorbell. Manual-reset: set by push/trigger_exit, reset only by wait_event.
    SyncEvent               mQueueEvent;
    //!< Sticky exit state. When set, pop_event() returns the singleton ExitEvent.
    std::atomic<bool>       mExitTriggered;

    //////////////////////////////////////////////////////////////////////////
    // Lock-free LIFO via tagged-pointer CAS (PoolTop: head + ABA counter).
    // Requires 16-byte native CAS (x86-64 cmpxchg16b / ARM64 CASP).
    // Both producers (_alloc_node) and the consumer (_free_node) operate
    // concurrently without a spinlock.
    //////////////////////////////////////////////////////////////////////////

    // Node pool, recycles Nodes to avoid per-event heap alloc/free.
    alignas(16) std::atomic<PoolTop>    mPool;      //!< Lock-free LIFO pool head + ABA counter.
    std::atomic<uint32_t>               mPoolSize;  //!< Current number of pooled nodes.

//////////////////////////////////////////////////////////////////////////
// Forbidden
//////////////////////////////////////////////////////////////////////////
private:
    MpscEventQueue() = delete;
    AREG_NOCOPY_NOMOVE(MpscEventQueue);
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline void MpscEventQueue::lock_queue() noexcept
{
    mPrioLock.lock();
}

inline void MpscEventQueue::unlock_queue() noexcept
{
    mPrioLock.unlock();
}

inline bool MpscEventQueue::is_exit_triggered() const noexcept
{
    return mExitTriggered.load(std::memory_order_acquire);
}

inline bool MpscEventQueue::has_pending() const noexcept
{
    return (mFastCount.load(std::memory_order_acquire) != 0u) || (mPrioCount.load(std::memory_order_relaxed) != 0u) || is_exit_triggered();
}

inline void MpscEventQueue::trigger_exit() noexcept
{
    mExitTriggered.store(true, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    mQueueEvent.set_signaled();
}

inline void MpscEventQueue::reset_exit() noexcept
{
    mExitTriggered.store(false, std::memory_order_release);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_MPSCEVENTQUEUE_HPP
