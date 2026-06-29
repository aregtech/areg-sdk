/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventQueue.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, bounded MPSC event queue implementation.
 ************************************************************************/
#include "areg/component/private/EventQueue.hpp"

#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/base/private/DebugDefs.hpp"

#include <chrono>
#include <type_traits>

// pop_event() is noexcept and returns the exit event by copying the cached singleton.
static_assert(std::is_nothrow_copy_constructible_v<areg::Event>, "Event copy must be noexcept for noexcept pop_event()");
static_assert(std::is_nothrow_move_assignable_v<areg::Event>, "Event move must be noexcept for the ring hand-off");

namespace
{
    //!< Upper bound on the adaptive pre-park spin (cpu_pause iterations). Caps the busy
    //!< interval to a few microseconds on a hit.
    constexpr uint32_t SPIN_BUDGET_MAX   { 8192u };
    //!< Seed/reward increment applied when a spin catches a message (grows toward the cap).
    constexpr uint32_t SPIN_GROW         { 256u };
    //!< Cheap probe length used when the budget has decayed to zero, to re-discover a
    //!< now-concurrent producer without committing to a full spin.
    constexpr uint32_t SPIN_PROBE        { 48u };
    //!< Park interval between zero-budget probes (power of two for mask arithmetic).
    constexpr uint32_t SPIN_PROBE_PERIOD { 256u };
    static_assert((SPIN_PROBE_PERIOD & (SPIN_PROBE_PERIOD - 1u)) == 0u, "SPIN_PROBE_PERIOD must be a power of two");
}

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
    , mExitTriggered    ( false )
    , mSlotEvent        ( true, true )      // auto-reset, initially non-signaled
    , mProducersWaiting ( 0u )
    , mSpinBudget       ( 0u )
    , mProbeCounter     ( 0u )
{
    mRing = new Cell[mCapacity];
    for (uint32_t i = 0u; i < mCapacity; ++i)
        mRing[i].sequence.store(i, std::memory_order_relaxed);
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
    , mExitTriggered    ( false )
    , mSlotEvent        ( areg::NullTag{} )     // no OS handle
    , mProducersWaiting ( 0u )
    , mSpinBudget       ( 0u )
    , mProbeCounter     ( 0u )
{
}

EventQueue::~EventQueue()
{
    delete[] mRing;
    mRing = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - consumer wait / exit control
//////////////////////////////////////////////////////////////////////////

bool EventQueue::wait_event(uint32_t timeout /*= areg::WAIT_INFINITE*/) noexcept
{
    if (has_pending())
        return true;

    // Phase 0: adaptive, self-tuning pre-park spin.
    uint32_t budget{ mSpinBudget };
    if ((budget == 0u) && (((++mProbeCounter) & (SPIN_PROBE_PERIOD - 1u)) == 0u))
        budget = SPIN_PROBE;    // occasional cheap probe to rediscover a now-busy peer

    for (uint32_t i = 0u; i < budget; ++i)
    {
        Thread::cpu_pause();
        if (has_pending())
        {
            // Hit: spinning paid off
            mSpinBudget += (mSpinBudget >> 2) + SPIN_GROW;
            if (mSpinBudget > SPIN_BUDGET_MAX)
                mSpinBudget = SPIN_BUDGET_MAX;
            return true;
        }
    }

    // Miss (or no spin)
    mSpinBudget = (mSpinBudget > 3u) ? (mSpinBudget - (mSpinBudget >> 2)) : 0u;

    // Lost-wakeup-free eventcount, reset the doorbell
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

void EventQueue::push_event(Event& eventElem, Event* removedEvent /*= nullptr*/)
{
    if (mRing == nullptr)
        return;

    const areg::EventPriority prio{ eventElem.event_priority() };

    if (prio == areg::EventPriority::ExitPrio)
    {
        // Exit is a sticky flag, never queued; the caller's event is dropped by its owner.
        trigger_exit();
        return;
    }

    if (prio >= areg::EventPriority::HighPrio)
    {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end() && it->event_priority() > prio)
            ++it;

        mPrioQueue.insert(it, std::move(eventElem));
        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
        _wake_consumer();
        return;
    }

    // Normal-priority: bounded ring (drop or block per policy).
    if (_ring_enqueue(eventElem))
    {
        _wake_consumer();
    }
    else if (removedEvent != nullptr)
    {
        *removedEvent = std::move(eventElem);   // not enqueued: hand back to caller
    }
}

uint32_t EventQueue::push_events(Event* eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u) || (mRing == nullptr))
        return 0u;

    uint32_t signalCount{ 0u };
    bool     exitRequested{ false };

    // Phase 1: insert all priority-lane events in ONE mPrioLock acquisition.
    if (eventElems[0].event_priority() > areg::EventPriority::NormalPrio)
    {
        Lock lock(mPrioLock);
        for (uint32_t i = 0u; i < count; ++i)
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
                auto it = mPrioQueue.begin();
                while (it != mPrioQueue.end() && it->event_priority() > prio)
                    ++it;

                mPrioQueue.insert(it, std::move(evt));
                ++signalCount;
            }
            else
            {
                break;
            }
        }

        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
    }

    // Phase 2: insert NormalPrio events into the ring; not-enqueued ones compact to the front.
    uint32_t removedCount{ 0u };
    for (uint32_t i = signalCount; i < count; ++i)
    {
        Event& evt = eventElems[i];
        if (!evt.is_valid())
            continue;

        if (_ring_enqueue(evt))
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
    if (mRing == nullptr)
        return Event{};

    // Exit preempts everything
    if (mExitTriggered.load(std::memory_order_acquire))
        return ExitEvent::exit_event();

    // Priority lane: always drained before the ring.
    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        if (!mPrioQueue.empty())
        {
            Event result{ std::move(mPrioQueue.front()) };
            mPrioQueue.pop_front();
            mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
            return result;
        }
    }

    Event result;
    if (_ring_try_dequeue(result))
        return result;

    return Event{};
}

uint32_t EventQueue::pop_events(Event* eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u) || (mRing == nullptr))
        return 0u;

    // Exit preempts every lane
    if (mExitTriggered.load(std::memory_order_acquire))
    {
        eventElems[0] = ExitEvent::exit_event();
        return 1u;
    }

    uint32_t popped{ 0u };

    // Phase 1: drain the priority lane with ONE mPrioLock acquisition.
    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        while (!mPrioQueue.empty() && (popped < count))
        {
            eventElems[popped++] = std::move(mPrioQueue.front());
            mPrioQueue.pop_front();
        }

        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
    }

    // Phase 2: drain the ring into the remaining slots (consumer thread only).
    while (popped < count)
    {
        if (!_ring_try_dequeue(eventElems[popped]))
            break;
        ++popped;
    }

    return popped;
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - remove
//////////////////////////////////////////////////////////////////////////

void EventQueue::remove_events(const uint32_t eventClassId) noexcept
{
    if (mRing == nullptr)
        return;

    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end())
        {
            if ((!it->is_exit_prio()) && (eventClassId == it->event_id()))
            {
                it = mPrioQueue.erase(it);  // erase() destroys the element -> payload released
            }
            else
            {
                ++it;
            }
        }

        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
    }

    // Drain the ring: keep non-matching events; a matching event is released by the Event
    // destructor at the end of its iteration. 'evt' MUST stay loop-scoped -- hoisting it out
    // would let the next move-assignment overwrite a match without running its destructor.
    std::vector<Event> kept;
    for (;;)
    {
        Event evt;
        if (!_ring_try_dequeue(evt))
            break;

        if (evt.event_id() != eventClassId)
            kept.push_back(std::move(evt));
    }

    for (Event& e : kept)
    {
        // The ring was just fully drained and no producer runs concurrently, so a free
        // slot is guaranteed -- the enqueue cannot fail.
        [[maybe_unused]] const bool ok{ _ring_try_enqueue(e) };
        ASSERT(ok);
    }
}

void EventQueue::remove_all_events() noexcept
{
    if (mRing == nullptr)
        return;

    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);
        mPrioQueue.clear();     // each element's ~Event() releases its payload
        mPrioCount.store(0u, std::memory_order_relaxed);
    }

    for (;;)
    {
        Event evt;
        if (!_ring_try_dequeue(evt))
            break;
    }
}

//////////////////////////////////////////////////////////////////////////
// EventQueue - Vyukov bounded ring
//////////////////////////////////////////////////////////////////////////

bool EventQueue::_ring_try_enqueue(Event& eventElem) noexcept
{
    ASSERT(mRing != nullptr);

    size_t pos{ mEnqueuePos.load(std::memory_order_relaxed) };
    Cell*  cell{ nullptr };
    for (;;)
    {
        cell = &mRing[pos & mMask];
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
#ifdef AREG_LATENCY_TRACE
    cell->lt_ns = AREG_LT_NOW();    // stamp before publishing; visible to consumer via the release store
#endif
    cell->sequence.store(pos + 1u, std::memory_order_release);
    return true;
}

bool EventQueue::_ring_enqueue(Event& eventElem) noexcept
{
    ASSERT(mRing != nullptr);

    if (_ring_try_enqueue(eventElem))
        return true;

    if (mDropOnFull)
        return false;   // drop-newest

    // Lossless: block up to mWaitMs for a free slot; abortable by exit.
    const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(mWaitMs) };
    mProducersWaiting.fetch_add(1u, std::memory_order_relaxed);
    bool enqueued{ false };
    for (;;)
    {
        if (mExitTriggered.load(std::memory_order_acquire))
            break;
        if (_ring_try_enqueue(eventElem))
        {
            enqueued = true;
            break;
        }
        if (std::chrono::steady_clock::now() >= deadline)
            break;
        mSlotEvent.lock(RING_WAIT_RECHECK_MS);   // woken by a consumer pop or the re-check timeout
    }
    mProducersWaiting.fetch_sub(1u, std::memory_order_relaxed);
    return enqueued;
}

bool EventQueue::_ring_try_dequeue(Event& result) noexcept
{
    ASSERT(mRing != nullptr);

    const size_t pos{ mDequeuePos.load(std::memory_order_relaxed) };
    Cell&        cell{ mRing[pos & mMask] };
    const size_t seq{ cell.sequence.load(std::memory_order_acquire) };
    if (seq != (pos + 1u))
        return false;   // head not yet published

    result = std::move(cell.event);
#ifdef AREG_LATENCY_TRACE
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
