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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/private/SimpleEvent.hpp"

#include <atomic>
#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
#include "areg/component/Event.hpp"

/**
 * \brief   Alignment of one ring slot, in bytes. One cache line keeps the hand-off
 *          state of neighbouring slots on separate lines, so two producers writing
 *          to adjacent slots do not fight over the same line. Define it as 8 to pack
 *          the ring tightly on a target where the few extra bytes per slot matter
 *          more than producer throughput.
 **/
#ifndef AREG_MPSC_CELL_ALIGN
    #define AREG_MPSC_CELL_ALIGN    64
#endif  // AREG_MPSC_CELL_ALIGN

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
 *          Both lanes are allocated by acquire_lanes(), which the owner dispatcher
 *          calls before it accepts events, and are freed by release_lanes(). A queue
 *          that holds no lanes takes no event: every operation is a safe no-op. The
 *          exit flag and the two wake-ups live in the object and stay usable at any
 *          time. A dispatcher that stops keeps its ring and reuses it when it starts
 *          again, so a restart allocates nothing.
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
     *          One slot occupies whole cache lines, so a producer publishing into
     *          one slot never dirties the line of a neighbouring slot.
     **/
    struct alignas(AREG_MPSC_CELL_ALIGN) Cell
    {
        std::atomic<size_t> sequence { 0u };
        Event               event    {     };
#if defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
        uint64_t            lt_ns    { 0u };    //!< enqueue timestamp (ns), carried to dequeue.
#endif
    };

    static constexpr uint32_t   RING_WAIT_RECHECK_MS  { 1u };     //!< Producer block re-check interval.

    //!< Top bit of the enqueue cursor. Set while the queue is closed, so a producer
    //!< that reloads the ticket sees the refusal in the value it already needs.
    static constexpr size_t     RING_CLOSED   { static_cast<size_t>(1u) << ((sizeof(size_t) * 8u) - 1u) };

    static constexpr uint32_t   CLOSE_SPIN_PAUSES { 64u };      //!< CPU-pause spins before yielding.
    static constexpr uint32_t   CLOSE_SPIN_LIMIT  { 4096u };    //!< Total spins before giving up on a slot.

    static constexpr uint8_t    EXIT_NONE     { 0u };     //!< The queue keeps running.
    static constexpr uint8_t    EXIT_NOW      { 1u };     //!< Stop at once, queued events are dropped.
    static constexpr uint8_t    EXIT_DRAINED  { 2u };     //!< Stop when the lanes are empty.

    //////////////////////////////////////////////////////////////////////////
    // 128 bytes separates the producer written enqueue cursor from the
    // consumer written dequeue cursor. This covers the widest hardware cache
    // line in use: x86/x86-64 and ARM32 use 32-64 bytes; Apple Silicon uses
    // 128 bytes. Padding is conservative on 32-bit targets (wastes ~248 B)
    // but keeps the layout correct across all supported platforms.
    //////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t   AREG_MPSC_CACHE_LINE_SIZE{ 128u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs the queue. The lanes stay empty until acquire_lanes() is called.
     *
     * \param   maxQueue    Ring size for normal-priority events; rounded up to a
     *                      power of two (minimum 32). Pass areg::IGNORE_VALUE (0)
     *                      to use the default ring size. The cells are allocated by
     *                      acquire_lanes(), not here.
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
     * \brief   Null constructor: creates a hollow queue that can never hold lanes.
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
     * \brief   Allocates the ring and the priority lane, and opens the queue for
     *          producers. A dispatcher that stops and starts again reuses the ring it
     *          already holds, so the restart only reopens it.
     *
     * \note    Call from the owner dispatcher, under lock_queue(), before the
     *          dispatcher reports itself started. It does not touch the exit flag:
     *          the owner clears that with reset_exit() before it starts.
     **/
    void acquire_lanes() noexcept;

    /**
     * \brief   Closes the queue: every further push_event() and push_events() is
     *          refused, and the call returns once every producer that had already
     *          taken a ring slot has left it. The lanes stay allocated.
     *
     * \note    Call from the owner dispatcher when it stops reporting itself started.
     *          After it returns the ring holds no producer, so the owner can drain it.
     **/
    void close_lanes() noexcept;

    /**
     * \brief   Returns true while the queue is closed for producers.
     **/
    [[nodiscard]]
    inline bool is_closed() const noexcept;

    /**
     * \brief   Closes the queue, destroys every queued event and frees the ring and
     *          the priority lane. After this the queue takes no event until
     *          acquire_lanes() is called again.
     *
     * \note    close_lanes() shuts the producers out, but a producer that had already
     *          read the ring pointer may still be about to touch it. Call this only
     *          where the owner thread is known to be gone, as the destructor is.
     **/
    void release_lanes() noexcept;

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
     * \brief   Returns the longest time, in milliseconds, that any producer had to wait for a
     *          free slot since this was last called, and resets the record to zero.
     *
     * \note    The queue only records the wait, the caller reports it. This queue also serves
     *          the log manager, so logging from here could feed its queue and recurse.
     *
     * \return  The longest wait in milliseconds, or 0 if no producer had to wait.
     **/
    [[nodiscard]]
    inline uint32_t extract_max_wait_ms() noexcept;

    /**
     * \brief   Requests exit: sets the sticky exit flag, wakes the consumer
     *          blocked in wait_event() and any producers blocked on a full ring.
     *          After this, pop_event() returns the singleton ExitEvent until
     *          reset_exit() is called.
     **/
    inline void trigger_exit() noexcept;

    /**
     * \brief   Requests exit, but only after the queued events are processed.
     **/
    inline void trigger_exit_drained() noexcept;

    /**
     * \brief   Clears the sticky exit flags. Must be called only when the owner
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
     * \return  true if the queue took the event, false if it could not. An ExitPrio event counts
     *          as taken: it is never queued, it sets the sticky exit flag instead. A false result
     *          is the only signal a producer gets, so the caller must pass it on.
     **/
    bool push_event(Event& eventElem, Event* removedEvent = nullptr);

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
     * \brief   One attempt to publish \a eventElem into \a ring.
     *          Lock-free, safe from any producer. Returns false when the ring is full.
     **/
    bool _ring_try_enqueue(Cell* ring, Event& eventElem) noexcept;

    /**
     * \brief   Publishes \a eventElem into \a ring honoring the full-ring policy:
     *          drop, or block up to mWaitMs (abortable by exit). Returns false if
     *          not enqueued.
     **/
    bool _ring_enqueue(Cell* ring, Event& eventElem) noexcept;

    /**
     * \brief   Blocks up to mWaitMs for a free slot in \a ring, then publishes
     *          \a eventElem. Returns false on timeout or exit.
     **/
    bool _ring_wait_enqueue(Cell* ring, Event& eventElem) noexcept;

    /**
     * \brief   Consumer-only dequeue of the next \a ring event into \a result.
     *          Returns false when the head slot is not yet published (ring empty).
     **/
    bool _ring_try_dequeue(Cell* ring, Event& result) noexcept;

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
    const uint32_t          mCapacity;   //!< Ring size (power of two). 0 for a hollow queue.
    const size_t            mMask;       //!< mCapacity - 1 (index mask).
    const bool              mDropOnFull; //!< true: drop on full; false: block up to mWaitMs.
    const uint32_t          mWaitMs;     //!< Lossless-mode full-ring block timeout (ms).

    //!< Array of mCapacity cells while the lanes are held, nullptr otherwise.
    std::atomic<Cell*>      mRing;

    //!< Producer-written enqueue cursor - own cache line.
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<size_t> mEnqueuePos;

    //!< Consumer-written dequeue cursor - own cache line.
    alignas(AREG_MPSC_CACHE_LINE_SIZE) std::atomic<size_t> mDequeuePos;

    //!< Priority lane - Critical at front, then descending priority order.
    SpinLock                mPrioLock;  //!< Recursive guard for mPrioQueue
    std::optional<std::deque<Event>>
                            mPrioQueue; //!< [Critical-][High-] ordered (stored by value)
    std::atomic_uint32_t    mPrioCount; //!< The number of elements in mPrioQueue

    //!< Consumer wake-up doorbell. Manual-reset: set by push/trigger_exit, reset only by wait_event.
    SimpleEvent             mQueueEvent;
    //!< Set by the consumer while parked in wait_event(); read by producers so the doorbell
    //!< is rung only when a waiter actually needs it (eventcount discipline, lost-wakeup-free).
    std::atomic<bool>       mConsumerParked;
    //!< Sticky exit state, a combination of EXIT_NOW and EXIT_DRAINED.
    std::atomic_uint8_t     mExitState;

    //!< Producer wake-up (auto-reset): signalled by the consumer when a slot is freed.
    SimpleEvent             mSlotEvent;
    //!< Longest producer wait, in milliseconds, since it was last read.
    std::atomic<uint32_t>   mMaxWaitMs;
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

inline uint32_t EventQueue::extract_max_wait_ms() noexcept
{
    // While no producer waits this is a plain load and the cache line stays shared. The
    // read-modify-write, which dirties it, happens only after a producer really had to wait.
    return (mMaxWaitMs.load(std::memory_order_relaxed) != 0u)
                ? mMaxWaitMs.exchange(0u, std::memory_order_relaxed)
                : 0u;
}

inline bool EventQueue::is_exit_triggered() const noexcept
{
    return (mExitState.load(std::memory_order_acquire) & EventQueue::EXIT_NOW) != 0u;
}

inline bool EventQueue::is_closed() const noexcept
{
    return (mEnqueuePos.load(std::memory_order_acquire) & EventQueue::RING_CLOSED) != 0u;
}

inline bool EventQueue::has_pending() const noexcept
{
    const Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if (ring == nullptr)
        return is_exit_triggered();

    const size_t pos{ mDequeuePos.load(std::memory_order_relaxed) };
    return (ring[pos & mMask].sequence.load(std::memory_order_acquire) == (pos + 1u))
        || (mPrioCount.load(std::memory_order_relaxed) != 0u)
        || (mExitState.load(std::memory_order_acquire) != EventQueue::EXIT_NONE);
}

inline void EventQueue::trigger_exit() noexcept
{
    mExitState.store(EventQueue::EXIT_NOW, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    mQueueEvent.set_signaled();     // wake the consumer
    mSlotEvent.set_signaled();      // wake any producer blocked on a full ring
}

inline void EventQueue::trigger_exit_drained() noexcept
{
    static_cast<void>(mExitState.fetch_or(EventQueue::EXIT_DRAINED, std::memory_order_release));
    std::atomic_thread_fence(std::memory_order_seq_cst);
    mQueueEvent.set_signaled();     // wake the consumer
    mSlotEvent.set_signaled();      // wake any producer blocked on a full ring
}

inline void EventQueue::reset_exit() noexcept
{
    mExitState.store(EventQueue::EXIT_NONE, std::memory_order_release);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EventQueue_HPP
