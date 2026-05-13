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
 * 
 *   Fast lane  - Dmitry Vyukov MPSC (Multi-Producer, Single-Consumer) linked-list (lock-free).
 *                Push: one atomic exchange on mTail  (any producer thread).
 *                Pop:  wait-free read of mHead->next (consumer thread only).
 *                Used for Normal-priority events (the overwhelming majority).
 *
 *   Priority lane - SpinLock-guarded deque for ExitPrio and HighPrio events.
 *                   SpinLock is recursive (same-thread re-entry safe) and is
 *                   used by lock_queue()/unlock_queue() so stop_dispatcher()
 *                   can atomically check mHasStarted and push ExitEvent.
 *
 * The public interface mirrors ExternalEventQueue so EventDispatcherBase
 * can select between the two at compile time without any caller changes.
 *
 * THREAD SAFETY:
 *   push_event()                  - safe from any number of producer threads.
 *   pop_event()                   - single consumer thread only.
 *   remove_*()                    - control path only; no concurrent push_event().
 *   lock_queue() / unlock_queue() - SpinLock; recursive; used by
 *                                   stop_dispatcher() for atomic check+push.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/private/QueueListener.hpp"

#include <atomic>
#include <cstdint>
#include <deque>
#include <limits>
#include <new>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Event;
    class RuntimeClassID;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
    #pragma warning(push)
    // C4324: structure padded due to alignas - intentional for cache-line separation.
    // C4251: STL/atomic private members need dll-interface - safe to suppress for private members.
    #pragma warning(disable: 4324 4251)
#endif  // _MSC_VER

/**
 * \brief   Lock-free MPSC (multiple-producer / single-consumer) event queue.
 *
 *          Drop-in replacement for ExternalEventQueue. The hot push path
 *          performs a single atomic exchange instead of acquiring a recursive
 *          OS mutex, eliminating kernel-mode transitions per normal event.
 *
 *          ExitPrio and HighPrio events use a separate SpinLock-guarded
 *          priority deque to preserve front-insertion ordering semantics.
 *          SpinLock is recursive (owns mOwner + mCount) - same-thread
 *          re-entry is safe, which is required by stop_dispatcher().
 *
 * \note    pop_event() must be called from a single consumer thread only.
 *          push_event() is safe from any number of producer threads.
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
        Event*              event   { nullptr };    //!< nullptr on the stub node.
    };

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
     * \param   eventListener   Listener notified on push / empty transitions.
     * \param   maxQueue        Soft capacity limit for normal-priority events.
     *                          Pass areg::IGNORE_VALUE (0) to read from
     *                          application configuration or use unlimited.
     **/
    MpscEventQueue(QueueListener& eventListener, uint32_t maxQueue);

    ~MpscEventQueue();

//////////////////////////////////////////////////////////////////////////
// Public interface - mirrors ExternalEventQueue
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Acquires the priority-lane SpinLock (recursive).
     *          Used by stop_dispatcher() to atomically check mHasStarted
     *          and push the ExitEvent without a TOCTOU race.
     **/
    inline void lock_queue() noexcept;

    /**
     * \brief   Releases one acquisition of the priority-lane SpinLock.
     **/
    inline void unlock_queue() noexcept;

    /**
     * \brief   Returns true when both lanes are empty.
     *          Acquires mPrioLock briefly; safe from any thread.
     **/
    bool is_empty() const noexcept;

    /**
     * \brief   Enqueues an event.
     *
     * ExitPrio events go to the front of the priority lane (bypass capacity).
     * HighPrio events are inserted after any ExitPrio entries.
     * Normal-priority events use the lock-free fast lane.
     *
     * When the fast lane is at capacity the incoming event is dropped
     * (drop-newest policy; the producer must not pop to avoid a mHead race).
     *
     * \param   eventElem       Event to enqueue. Must not be nullptr.
     * \param[out] removedEvent Receives the dropped event when the queue is
     *                          full and this is not nullptr; otherwise the
     *                          dropped event is destroyed immediately.
     **/
    void push_event(Event& eventElem, Event** removedEvent);

    /**
     * \brief   Dequeues the next event. Priority lane is always drained first.
     *
     * \return  Next event pointer, or nullptr when both lanes are empty.
     **/
    [[nodiscard]]
    Event* pop_event() noexcept;

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
    void remove_events(const RuntimeClassID& eventClassId) noexcept;

    /**
     * \brief   Destroys every event in both lanes.
     *          Must not be called concurrently with push_event().
     **/
    void remove_all_events() noexcept;

    /**
     * \brief   Enqueues up to \a count events with a single priority-lane lock acquisition.
     *          Each event is routed by its priority: ExitPrio events are placed at the front of
     *          the priority lane; events with HighPrio or above (but not ExitPrio) are inserted
     *          after any existing ExitPrio entries; events below HighPrio go to the lock-free
     *          fast lane. Normal-priority events that exceed the queue capacity are NOT enqueued
     *          and are returned to the caller via \a eventElems. The events in \a eventElems list
     *          should be already sorted by priorities.
     *
     * \param[in,out]   eventElems  On input: sorted by priority array of \a count event pointers to enqueue.
     *                              On output: \a eventElems[0..<returnValue>) contains the events
     *                              that could not be enqueued (Normal-priority capacity overflow).
     *                              The remainder of the array is undefined. Must not be nullptr.
     * \param           count       Number of valid pointers in \a eventElems.
     * \return  Number of events that were NOT enqueued due to capacity overflow.
     *          Returns 0 if all events were enqueued successfully.
     *          The returned value is always <= \a count.
     **/
    uint32_t push_events(Event** eventElems, uint32_t count);

    /**
     * \brief   Dequeues up to \a count events with a single priority-lane lock acquisition.
     *          The priority lane (ExitPrio first, then higher-priority events) is always drained
     *          before the lock-free fast lane. The first element in \a eventElems has the highest
     *          priority among all dequeued events; remaining elements follow in priority order.
     *
     * \param[out]      eventElems  Caller-supplied array of at least \a count pointers.
     *                              On output, \a eventElems[0..<returnValue>) contains the
     *                              dequeued events, ordered highest-priority-first.
     *                              Must not be nullptr.
     * \param           count       Maximum number of events to dequeue; size of \a eventElems.
     * \return  Number of events dequeued and written to \a eventElems.
     *          Returns 0 if both lanes are empty.
     *          The returned value is always <= \a count.
     **/
    uint32_t pop_events(Event** eventElems, uint32_t count);

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
    inline Node* _mpsc_pop() noexcept;

    /**
     * \brief   Drains the entire fast lane into \a out.
     *          Safe only when no producers are active (control path).
     **/
    void _mpsc_drain_to(std::deque<Event*>& out) noexcept;

    /**
     * \brief   Returns a Node from the pool, or allocates a new one if the pool is empty.
     **/
    Node* _alloc_node() noexcept;

    /**
     * \brief   Returns a Node to the pool. If the pool is already at capacity, deletes the node.
     **/
    void _free_node(Node* node) noexcept;

    inline uint32_t _prio_count() noexcept;

    /**
     * \brief   Computes the effective capacity limit.
     *          0 -> unlimited (UINT32_MAX cap); any other value -> max(32, requested).
     **/
    static uint32_t _calc_capacity(uint32_t requested) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    //--------------------------------------------------------------------
    // Stub / sentinel node - anchors the list when empty.
    // Allocated once in the constructor; freed only in the destructor.
    //--------------------------------------------------------------------
    alignas(AREG_MPSC_CACHE_LINE_SIZE) Node*              mStub;

    //--------------------------------------------------------------------
    // Producer-written tail - own cache line to avoid false sharing with
    // the consumer's mHead reads.
    //--------------------------------------------------------------------
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<Node*> mTail;

    //--------------------------------------------------------------------
    // Consumer-written head - own cache line to avoid false sharing with
    // producer writes to mTail.
    //--------------------------------------------------------------------
    alignas(AREG_MPSC_CACHE_LINE_SIZE) Node*              mHead;

    //--------------------------------------------------------------------
    // Fast-lane item count.
    // Incremented by producers (fetch_add), decremented by the consumer.
    //--------------------------------------------------------------------
    std::atomic<uint32_t>   mFastCount; //!< Current fast-lane depth.
    const uint32_t          mCapacity;  //!< Soft cap (MAX_UINT = unlimited).

    //--------------------------------------------------------------------
    // Priority lane - ExitPrio at front, HighPrio after, Normal in fast.
    // mPrioLock is recursive and also serves as lock_queue()/unlock_queue().
    //--------------------------------------------------------------------
    SpinLock                mPrioLock;  //!< Recursive guard for mPrioQueue.
    std::deque<Event*>      mPrioQueue; //!< [Exit-][High-] ordered.

    //--------------------------------------------------------------------
    // Listener - receives signal_event() on push and on becoming empty.
    //--------------------------------------------------------------------
    QueueListener&          mListener;

    //--------------------------------------------------------------------
    // Node pool -- recycles Nodes to avoid per-event heap alloc/free.
    //
    // Nodes are linked via their  pointer while in the pool (the
    // same field used in the MPSC queue; a node is never in both
    // simultaneously). Both producers (_alloc_node) and the consumer
    // (_free_node) touch the pool, so access is guarded by mNodePoolLock.
    //
    // NODE_POOL_PRE_ALLOC nodes are pre-populated in the constructor to
    // avoid cold-start heap pressure during the initial message burst.
    // NODE_POOL_MAX caps the pool depth so idle queues do not hold
    // excessive memory.
    //--------------------------------------------------------------------
    static constexpr uint32_t   NODE_POOL_PRE_ALLOC { 64u  }; //!< Nodes pre-allocated in constructor.
    static constexpr uint32_t   NODE_POOL_MAX       { 256u }; //!< Maximum pool depth.

    SpinLock    mNodePoolLock;                  //!< Guards mNodePoolHead / mNodePoolSize.
    Node*       mNodePoolHead   { nullptr };    //!< Head of the pooled-node free list.
    uint32_t    mNodePoolSize   { 0u };         //!< Current number of pooled nodes.

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

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_MPSCEVENTQUEUE_HPP