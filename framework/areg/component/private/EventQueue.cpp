/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, bounded MPSC event queue implementation.
 ************************************************************************/
#include "areg/component/private/EventQueue.hpp"

#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/DebugDefs.hpp"

#include <chrono>
#include <type_traits>

// pop_event() is noexcept and returns the exit event by copying the cached singleton.
static_assert(std::is_nothrow_copy_constructible_v<areg::Event>, "Event copy must be noexcept for noexcept pop_event()");
static_assert(std::is_nothrow_move_assignable_v<areg::Event>, "Event move must be noexcept for the ring hand-off");

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventQueue - constructor / destructor
//////////////////////////////////////////////////////////////////////////

EventQueue::EventQueue(uint32_t maxQueue, bool dropOnFull /*= false*/, uint32_t waitMs /*= areg::QUEUE_DEFAULT_FULL_WAIT_MS*/)
    : mCapacity         ( EventQueue::_calc_capacity(maxQueue) )
    , mMask             ( static_cast<size_t>(mCapacity) - 1u )
    , mDropOnFull       ( dropOnFull )
    , mWaitMs           ( waitMs )
    , mRing             ( nullptr )
    , mEnqueuePos       ( 0u )
    , mDequeuePos       ( 0u )
    , mPrioLock         ( )
    , mPrioQueue        ( )
    , mPrioCount        ( 0u )
    , mQueueEvent       ( true, false )     // manual-reset, initially non-signaled
    , mConsumerParked   ( false )
    , mExitState        ( EventQueue::EXIT_NONE )
    , mSlotEvent        ( true, true )      // auto-reset, initially non-signaled
    , mMaxWaitMs        ( 0u )
    , mProducersWaiting ( 0u )
{
}

EventQueue::EventQueue( areg::NullTag ) noexcept
    : mCapacity         ( 0u )
    , mMask             ( 0u )
    , mDropOnFull       ( false )
    , mWaitMs           ( 0u )
    , mRing             ( nullptr )
    , mEnqueuePos       ( 0u )
    , mDequeuePos       ( 0u )
    , mPrioLock         ( )
    , mPrioQueue        ( )
    , mPrioCount        ( 0u )
    , mQueueEvent       ( areg::NullTag{} )     // no OS handle
    , mConsumerParked   ( false )
    , mExitState        ( EventQueue::EXIT_NONE )
    , mSlotEvent        ( areg::NullTag{} )     // no OS handle
    , mMaxWaitMs        ( 0u )
    , mProducersWaiting ( 0u )
{
}

EventQueue::~EventQueue()
{
    release_lanes();
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - lane lifecycle
//////////////////////////////////////////////////////////////////////////

void EventQueue::acquire_lanes() noexcept
{
    if (mCapacity == 0u)
        return;

    if (mRing.load(std::memory_order_relaxed) != nullptr)
    {
        // The ring outlives a stop, so starting again only opens it for producers.
        static_cast<void>(mEnqueuePos.fetch_and(~EventQueue::RING_CLOSED, std::memory_order_release));
        return;
    }

    Cell* const ring{ new Cell[mCapacity] };
    for (uint32_t i = 0u; i < mCapacity; ++i)
        ring[i].sequence.store(i, std::memory_order_relaxed);

    mEnqueuePos.store(0u, std::memory_order_relaxed);
    mDequeuePos.store(0u, std::memory_order_relaxed);

    {
        Lock lock(mPrioLock);
        mPrioQueue.emplace();
        mPrioCount.store(0u, std::memory_order_relaxed);
    }

    mRing.store(ring, std::memory_order_release);
}

void EventQueue::close_lanes() noexcept
{
    const size_t closing{ mEnqueuePos.fetch_or(EventQueue::RING_CLOSED, std::memory_order_acq_rel) };
    Cell* const  ring   { mRing.load(std::memory_order_acquire) };
    if ((ring == nullptr) || ((closing & EventQueue::RING_CLOSED) != 0u))
        return;

    mSlotEvent.set_signaled();  // a producer parked on a full ring never gets its slot now

    // A slot taken before the close is published within a move and a store, so every
    // slot up to the closing cursor falls quiet after a bounded spin.
    const size_t claimed{ closing & ~EventQueue::RING_CLOSED };
    for (size_t pos = mDequeuePos.load(std::memory_order_relaxed); pos != claimed; ++pos)
    {
        const Cell& cell{ ring[pos & mMask] };
        uint32_t spin{ 0u };
        while ((cell.sequence.load(std::memory_order_acquire) == pos) && (spin < EventQueue::CLOSE_SPIN_LIMIT))
        {
            if (spin < EventQueue::CLOSE_SPIN_PAUSES)
                Thread::cpu_pause();
            else
                Thread::switch_thread();

            ++spin;
        }
    }
}

void EventQueue::release_lanes() noexcept
{
    close_lanes();

    Cell* const ring{ mRing.exchange(nullptr, std::memory_order_acq_rel) };

    {
        Lock lock(mPrioLock);
        mPrioQueue.reset();     // each element's ~Event() releases its payload
        mPrioCount.store(0u, std::memory_order_relaxed);
    }

    delete[] ring;              // each cell's ~Event() releases its payload
    mEnqueuePos.store(0u, std::memory_order_relaxed);
    mDequeuePos.store(0u, std::memory_order_relaxed);
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - consumer wait / exit control
//////////////////////////////////////////////////////////////////////////

bool EventQueue::wait_event(uint32_t timeout /*= areg::WAIT_INFINITE*/) noexcept
{
    if (has_pending())
        return true;

    // Lost-wakeup-free eventcount: reset the doorbell, then re-check
    mQueueEvent.reset();
    mConsumerParked.store(true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_seq_cst);
    if (has_pending())
    {
        mConsumerParked.store(false, std::memory_order_relaxed);
        return true;
    }

    const bool signaled{ mQueueEvent.lock(timeout) };
    mConsumerParked.store(false, std::memory_order_relaxed);
    return signaled || has_pending();
}

inline void EventQueue::_wake_consumer() noexcept
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    if (mConsumerParked.load(std::memory_order_relaxed))
        mQueueEvent.set_signaled();
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - push
//////////////////////////////////////////////////////////////////////////

bool EventQueue::push_event(Event& eventElem, Event* removedEvent /*= nullptr*/)
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if (ring == nullptr)
        return false;

    const areg::EventPriority prio{ eventElem.event_priority() };

    if (prio == areg::EventPriority::ExitPrio)
    {
        // Exit is a sticky flag, never queued; the caller's event is dropped by its owner.
        trigger_exit();
        return true;
    }

    if (prio >= areg::EventPriority::HighPrio)
    {
        Lock lock(mPrioLock);
        if (mPrioQueue.has_value() && (!is_closed()))
        {
            auto it = mPrioQueue->begin();
            while (it != mPrioQueue->end() && it->event_priority() >= prio)
                ++it;

            mPrioQueue->insert(it, std::move(eventElem));
            mPrioCount.store(static_cast<uint32_t>(mPrioQueue->size()), std::memory_order_relaxed);
            _wake_consumer();
            return true;
        }
    }

    // Normal-priority: bounded ring (drop or block per policy).
    if (_ring_enqueue(ring, eventElem))
    {
        _wake_consumer();
        return true;
    }

    if (removedEvent != nullptr)
    {
        *removedEvent = std::move(eventElem);   // not enqueued: hand back to caller
    }

    return false;
}

uint32_t EventQueue::push_events(Event* eventElems, uint32_t count)
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if ((eventElems == nullptr) || (count == 0u) || (ring == nullptr))
        return 0u;

    uint32_t signalCount{ 0u };
    bool     exitRequested{ false };

    // Phase 1: insert all priority-lane events in ONE mPrioLock acquisition.
    if (eventElems[0].event_priority() > areg::EventPriority::NormalPrio)
    {
        Lock lock(mPrioLock);
        const bool prioOpen{ mPrioQueue.has_value() && (!is_closed()) };
        for (uint32_t i = 0u; (i < count) && prioOpen; ++i)
        {
            Event& evt = eventElems[i];
            if (!evt.is_valid())
                break;

            const areg::EventPriority prio{ evt.event_priority() };
            if (prio == areg::EventPriority::ExitPrio)
            {
                evt.destroy_event();    // exit is sticky (flag), never queued
                exitRequested = true;
                ++signalCount;
            }
            else if (prio >= areg::EventPriority::HighPrio)
            {
                // '>=' keeps equal priorities in posting order -- see push_event().
                auto it = mPrioQueue->begin();
                while (it != mPrioQueue->end() && it->event_priority() >= prio)
                    ++it;

                mPrioQueue->insert(it, std::move(evt));
                ++signalCount;
            }
            else
            {
                break;
            }
        }

        mPrioCount.store(mPrioQueue.has_value() ? static_cast<uint32_t>(mPrioQueue->size()) : 0u
                        , std::memory_order_relaxed);
    }

    // Phase 2: insert NormalPrio events into the ring; not-enqueued ones compact to the front.
    uint32_t removedCount{ 0u };
    for (uint32_t i = signalCount; i < count; ++i)
    {
        Event& evt = eventElems[i];
        if (!evt.is_valid())
            continue;

        if (_ring_enqueue(ring, evt))
        {
            ++signalCount;
        }
        else
        {
            if (removedCount < i)
                eventElems[removedCount] = std::move(evt);
            ++removedCount;
        }
    }

    if (exitRequested)
    {
        trigger_exit();
    }
    else if (signalCount != 0u)
    {
        _wake_consumer();
    }

    return removedCount;
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - pop
//////////////////////////////////////////////////////////////////////////

Event EventQueue::pop_event() noexcept
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if (ring == nullptr)
        return Event{};

    // Immediate exit preempts everything
    if (is_exit_triggered())
        return ExitEvent::exit_event();

    // Priority lane: always drained before the ring.
    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        if (mPrioQueue.has_value() && !mPrioQueue->empty())
        {
            Event result{ std::move(mPrioQueue->front()) };
            mPrioQueue->pop_front();
            mPrioCount.store(static_cast<uint32_t>(mPrioQueue->size()), std::memory_order_relaxed);
            return result;
        }
    }

    Event result;
    if (_ring_try_dequeue(ring, result))
        return result;

    if ((mExitState.load(std::memory_order_acquire) & EventQueue::EXIT_DRAINED) != 0u)
        return ExitEvent::exit_event();

    return Event{};
}

uint32_t EventQueue::pop_events(Event* eventElems, uint32_t count)
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if ((eventElems == nullptr) || (count == 0u) || (ring == nullptr))
        return 0u;

    // Immediate exit preempts every lane
    if (is_exit_triggered())
    {
        eventElems[0] = ExitEvent::exit_event();
        return 1u;
    }

    uint32_t popped{ 0u };

    // Phase 1: drain the priority lane with ONE mPrioLock acquisition.
    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        while (mPrioQueue.has_value() && !mPrioQueue->empty() && (popped < count))
        {
            eventElems[popped++] = std::move(mPrioQueue->front());
            mPrioQueue->pop_front();
        }

        mPrioCount.store(mPrioQueue.has_value() ? static_cast<uint32_t>(mPrioQueue->size()) : 0u
                        , std::memory_order_relaxed);
    }

    // Phase 2: drain the ring into the remaining slots (consumer thread only).
    while (popped < count)
    {
        if (!_ring_try_dequeue(ring, eventElems[popped]))
            break;
        ++popped;
    }

    if ((popped == 0u) && ((mExitState.load(std::memory_order_acquire) & EventQueue::EXIT_DRAINED) != 0u))
    {
        eventElems[0] = ExitEvent::exit_event();
        popped = 1u;
    }

    return popped;
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - remove
//////////////////////////////////////////////////////////////////////////

void EventQueue::remove_events(const uint32_t eventClassId) noexcept
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if ((ring == nullptr) || is_closed())
        return;     // a closed queue keeps nothing: remove_all_events() empties it

    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        if (mPrioQueue.has_value())
        {
            auto it = mPrioQueue->begin();
            while (it != mPrioQueue->end())
            {
                if ((!it->is_exit_prio()) && (eventClassId == it->event_id()))
                {
                    it = mPrioQueue->erase(it); // erase() destroys the element -> payload released
                }
                else
                {
                    ++it;
                }
            }

            mPrioCount.store(static_cast<uint32_t>(mPrioQueue->size()), std::memory_order_relaxed);
        }
    }

    // Drain the ring, keep non-matching events
    std::vector<Event> kept;
    for (;;)
    {
        Event evt;
        if (!_ring_try_dequeue(ring, evt))
            break;

        if (evt.event_id() != eventClassId)
            kept.push_back(std::move(evt));
    }

    for (Event& e : kept)
    {
        VERIFY(_ring_try_enqueue(ring, e));
    }
}

void EventQueue::remove_all_events() noexcept
{
    Cell* const ring{ mRing.load(std::memory_order_acquire) };
    if (ring == nullptr)
        return;

    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        if (mPrioQueue.has_value())
            mPrioQueue->clear();    // each element's ~Event() releases its payload

        mPrioCount.store(0u, std::memory_order_relaxed);
    }

    for (;;)
    {
        Event evt;
        if (!_ring_try_dequeue(ring, evt))
            break;
    }
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - Vyukov bounded ring
//////////////////////////////////////////////////////////////////////////

bool EventQueue::_ring_try_enqueue(Cell* ring, Event& eventElem) noexcept
{
    ASSERT(ring != nullptr);

    size_t pos{ mEnqueuePos.load(std::memory_order_relaxed) };
    Cell*  cell{ nullptr };
    for (;;)
    {
        if ((pos & EventQueue::RING_CLOSED) != 0u)
            return false;   // closed: no new slot is handed out

        cell = &ring[pos & mMask];
        const size_t   seq{ cell->sequence.load(std::memory_order_acquire) };
        const intptr_t dif{ static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos) };
        if (dif == 0)
        {
            if (mEnqueuePos.compare_exchange_weak(pos, pos + 1u, std::memory_order_relaxed))
                break;
        }
        else if (dif < 0)
        {
            return false;   // ring full
        }
        else
        {
            pos = mEnqueuePos.load(std::memory_order_relaxed);
        }
    }

    cell->event = std::move(eventElem);
#if defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
    cell->lt_ns = AREG_LT_NOW();    // stamp before publishing; visible to consumer via the release store
#endif
    cell->sequence.store(pos + 1u, std::memory_order_release);
    return true;
}

bool EventQueue::_ring_enqueue(Cell* ring, Event& eventElem) noexcept
{
    ASSERT(ring != nullptr);

    if (_ring_try_enqueue(ring, eventElem))
        return true;

    return mDropOnFull ? false : _ring_wait_enqueue(ring, eventElem);
}

bool EventQueue::_ring_wait_enqueue(Cell* ring, Event& eventElem) noexcept
{
    // Lossless: block up to mWaitMs for a free slot; abortable by exit.
    const auto waitBegin{ std::chrono::steady_clock::now() };
    const auto deadline{ waitBegin + std::chrono::milliseconds(mWaitMs) };
    mProducersWaiting.fetch_add(1u, std::memory_order_relaxed);
    bool enqueued{ false };
    for (;;)
    {
        // Only the immediate exit or a close aborts the wait.
        if (is_exit_triggered() || is_closed())
            break;
        if (_ring_try_enqueue(ring, eventElem))
        {
            enqueued = true;
            break;
        }
        if (std::chrono::steady_clock::now() >= deadline)
            break;
        mSlotEvent.lock(RING_WAIT_RECHECK_MS);   // woken by a consumer pop or the re-check timeout
    }
    mProducersWaiting.fetch_sub(1u, std::memory_order_relaxed);

    // Only recorded, never logged here: this queue also serves the log manager.
    const uint32_t waited{ static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::steady_clock::now() - waitBegin).count()) };
    uint32_t recorded{ mMaxWaitMs.load(std::memory_order_relaxed) };
    while ((waited > recorded) &&
           (mMaxWaitMs.compare_exchange_weak(recorded, waited, std::memory_order_relaxed) == false))
    {
    }

    return enqueued;
}

bool EventQueue::_ring_try_dequeue(Cell* ring, Event& result) noexcept
{
    ASSERT(ring != nullptr);

    const size_t pos{ mDequeuePos.load(std::memory_order_relaxed) };
    Cell&        cell{ ring[pos & mMask] };
    const size_t seq{ cell.sequence.load(std::memory_order_acquire) };
    if (seq != (pos + 1u))
        return false;   // head not yet published

    result = std::move(cell.event);
#if defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)
    AREG_LT_SAMPLE(areg::LtStage::MpscHandoff, AREG_LT_NOW() - cell.lt_ns);
#endif
    cell.sequence.store(pos + mMask + 1u, std::memory_order_release);   // free the slot for the next lap
    mDequeuePos.store(pos + 1u, std::memory_order_relaxed);

    if (mProducersWaiting.load(std::memory_order_relaxed) != 0u)
        mSlotEvent.set_signaled();   // a slot freed: wake a blocked producer
    return true;
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - capacity helpers
//////////////////////////////////////////////////////////////////////////

uint32_t EventQueue::_round_up_pow2(uint32_t value) noexcept
{
    if (value <= 1u)
        return 1u;

    --value;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1u;
}

uint32_t EventQueue::_calc_capacity(uint32_t requested) noexcept
{
    // 0/IGNORE_VALUE and the "unlimited" sentinel (QUEUE_SIZE_MAXIMUM) map to the default ring.
    if ((requested == 0u) || (requested > areg::QUEUE_MAX_RING_CAPACITY))
        return areg::QUEUE_DEFAULT_RING_CAPACITY;
    else if (requested < areg::QUEUE_MIN_RING_CAPACITY)
        return areg::QUEUE_MIN_RING_CAPACITY;
    else
        return _round_up_pow2(requested);
}

} // namespace areg
