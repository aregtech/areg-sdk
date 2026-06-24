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
 * \brief       Areg Platform, bounded MPSC (Multi-Producer, Single-Consumer) event queue.
 *              Normal lane: Dmitry Vyukov bounded array ring.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <cstdint>
#include <deque>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
#include "areg/component/Event.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventQueue class declaration
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4324 4251)
#endif  // _MSC_VER

/**
 * \brief   Bounded MPSC (multiple-producer / single-consumer) event queue.
 *
 *          Normal-priority events use a fixed array ring (Dmitry Vyukov bounded
 *          MPSC): push is a single CAS on a ticket plus a release store, pop is a
 *          single acquire load -- no node allocation and no heap traffic on the
 *          hot path. The ring size is fixed at construction (power of two).
 *
 *          HighPrio and CriticalPrio events use a separate SpinLock-guarded
 *          priority deque (unbounded, never dropped) to preserve front-insertion
 *          ordering semantics.
 *
 *          When the ring is full the behaviour is selected at construction:
 *            - dropOnFull == false (default): the producer blocks up to waitMs for
 *              a free slot, then fails the enqueue (lossless; request/response safe).
 *              The wait is aborted by trigger_exit().
 *            - dropOnFull == true: the incoming event is rejected (drop-newest),
 *              for best-effort / latest-value streams (e.g. broadcasts).
 *
 *          The queue owns the consumer wake-up (a manual-reset SyncEvent doorbell,
 *          lost-wakeup-free eventcount discipline) and the producer wake-up (an
 *          auto-reset SyncEvent signalled when a slot is freed). ExitPrio is never
 *          queued; it sets the sticky exit flag.
 *
 * \note    pop_event() and wait_event() must be called from a single consumer
 *          thread only. push_event() is safe from any number of producer threads.
 **/
class AREG_API EventQueue
{
    friend class EventDispatcherBase;

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   One ring slot. \a sequence carries the Vyukov hand-off state:
     *          == enqueue cursor -> free to claim; == cursor+1 -> published.
     **/
    struct Cell
    {
        std::atomic<size_t> sequence { 0u };
        Event               event    {     };
#ifdef AREG_LATENCY_TRACE
        uint64_t            lt_ns    { 0u };    //!< enqueue timestamp (ns), carried to dequeue.
#endif
    };

    static constexpr uint32_t   RING_WAIT_RECHECK_MS  { 1u };     //!< Producer block re-check interval.

    //////////////////////////////////////////////////////////////////////////
    // 128 bytes separates the producer-written enqueue cursor from the
    // consumer-written dequeue cursor (covers x86-64 64-byte and ARM 128-byte lines).
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t   AREG_MPSC_CACHE_LINE_SIZE{ 128u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs the queue and allocates the fixed ring.
     *
     * \param   maxQueue    Ring size for normal-priority events; rounded up to a
     *                      power of two (minimum 32). Pass areg::IGNORE_VALUE (0)
     *                      to use the default ring size.
     * \param   dropOnFull  false (default): producer blocks up to \a waitMs for a
     *                      slot, then fails the enqueue. true: drop the incoming
     *                      event when the ring is full.
     * \param   waitMs      Lossless-mode block timeout in milliseconds (only used
     *                      when dropOnFull == false).
     **/
    explicit EventQueue(uint32_t maxQueue, bool dropOnFull = false, uint32_t waitMs = areg::QUEUE_DEFAULT_FULL_WAIT_MS);

    ~EventQueue();

protected:
    /**
     * \brief   Null constructor: creates a hollow queue with no ring buffer and no OS sync handles.
     *          All operations are safe no-ops. Used only by EventDispatcherBase(NullTag) to build
     *          zero-allocation sentinel dispatcher objects.
     **/
    explicit EventQueue( areg::NullTag ) noexcept;

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
     * \brief   Requests exit: sets the sticky exit flag, wakes the consumer
     *          blocked in wait_event() and any producers blocked on a full ring.
     *          After this, pop_event() returns the singleton ExitEvent until
     *          reset_exit() is called.
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
     *          (empty/invalid) state after a successful push. If the ring is full and the event
     *          cannot be enqueued (drop mode, or block-mode timeout/exit) it is NOT queued.
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
     *          Each event is moved into the queue. Events that cannot be enqueued (ring full in
     *          drop mode, or block-mode timeout/exit) remain and are compacted into the front of
     *          \a eventElems, with that count returned.
     *
     * \param[in,out]   eventElems  Array of Event values sorted by priority. Events that could
     *                              not be enqueued are moved to the front of the array.
     *                              Must not be nullptr.
     * \param           count       Number of valid Events in \a eventElems.
     * \return  Number of events NOT enqueued. 0 = all enqueued.
     **/
    uint32_t push_events(Event* eventElems, uint32_t count);

    /**
     * \brief   Dequeues up to \a count events into the caller-supplied Event array.
     *          Priority lane is always drained before the ring.
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
     * \brief   One producer attempt to publish \a eventElem into the ring.
     *          Lock-free, safe from any producer. Returns false when the ring is full.
     **/
    [[nodiscard]]
    bool _ring_try_enqueue(Event& eventElem) noexcept;

    /**
     * \brief   Publishes \a eventElem honoring the full-ring policy: drop, or block
     *          up to mWaitMs (abortable by exit). Returns false if not enqueued.
     **/
    bool _ring_enqueue(Event& eventElem) noexcept;

    /**
     * \brief   Consumer-only dequeue of the next ring event into \a result.
     *          Returns false when the head slot is not yet published (ring empty).
     **/
    [[nodiscard]]
    bool _ring_try_dequeue(Event& result) noexcept;

    /**
     * \brief   Rings the consumer doorbell, but only when the consumer is parked
     *          (eventcount discipline). The full fence pairs with wait_event()'s arm
     *          so a consumer that is in the act of parking is never missed.
     **/
    void _wake_consumer() noexcept;

    /**
     * \brief   Rounds \a value up to the next power of two.
     **/
    [[nodiscard]]
    static uint32_t _round_up_pow2(uint32_t value) noexcept;

    /**
     * \brief   Computes the ring size: 0 -> default; otherwise round-up-pow2(max(32, requested)).
     **/
    [[nodiscard]]
    static uint32_t _calc_capacity(uint32_t requested) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    //!< Read-only after construction.
    const uint32_t          mCapacity;   //!< Ring size (power of two).
    const size_t            mMask;       //!< mCapacity - 1 (index mask).
    const bool              mDropOnFull; //!< true: drop on full; false: block up to mWaitMs.
    const uint32_t          mWaitMs;     //!< Lossless-mode full-ring block timeout (ms).
    Cell*                   mRing;       //!< Fixed array of mCapacity cells.

    //!< Producer-written enqueue cursor - own cache line.
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<size_t> mEnqueuePos;

    //!< Consumer-written dequeue cursor - own cache line.
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<size_t> mDequeuePos;

    //!< Priority lane - Critical at front, then descending priority order.
    SpinLock                mPrioLock;  //!< Recursive guard for mPrioQueue
    std::deque<Event>       mPrioQueue; //!< [Critical-][High-] ordered (stored by value)
    std::atomic_uint32_t    mPrioCount; //!< The number of elements in mPrioQueue

    //!< Consumer wake-up doorbell. Manual-reset: set by push/trigger_exit, reset only by wait_event.
    SyncEvent               mQueueEvent;
    //!< Set by the consumer while parked in wait_event(); read by producers so the doorbell
    //!< is rung only when a waiter actually needs it (eventcount discipline, lost-wakeup-free).
    std::atomic<bool>       mConsumerParked;
    //!< Sticky exit state. When set, pop_event() returns the singleton ExitEvent.
    std::atomic<bool>       mExitTriggered;

    //!< Producer wake-up (auto-reset): signalled by the consumer when a slot is freed.
    SyncEvent               mSlotEvent;
    //!< Number of producers blocked on a full ring (so the consumer signals only when needed).
    std::atomic<uint32_t>   mProducersWaiting;

//////////////////////////////////////////////////////////////////////////
// Forbidden
//////////////////////////////////////////////////////////////////////////
private:
    EventQueue() = delete;
    AREG_NOCOPY_NOMOVE(EventQueue);
};

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// EventQueue inline implementation
//////////////////////////////////////////////////////////////////////////

inline void EventQueue::lock_queue() noexcept
{
    mPrioLock.lock();
}

inline void EventQueue::unlock_queue() noexcept
{
    mPrioLock.unlock();
}

inline bool EventQueue::is_exit_triggered() const noexcept
{
    return mExitTriggered.load(std::memory_order_acquire);
}

inline bool EventQueue::has_pending() const noexcept
{
    if (mRing == nullptr)
        return is_exit_triggered();

    const size_t pos{ mDequeuePos.load(std::memory_order_relaxed) };
    return (mRing[pos & mMask].sequence.load(std::memory_order_acquire) == (pos + 1u))
        || (mPrioCount.load(std::memory_order_relaxed) != 0u)
        || is_exit_triggered();
}

inline void EventQueue::trigger_exit() noexcept
{
    mExitTriggered.store(true, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    mQueueEvent.set_signaled();     // wake the consumer
    mSlotEvent.set_signaled();      // wake any producer blocked on a full ring
}

inline void EventQueue::reset_exit() noexcept
{
    mExitTriggered.store(false, std::memory_order_release);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EventQueue_HPP
