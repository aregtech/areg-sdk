/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/MpscEventQueue.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lock-free MPSC event queue implementation.
 ************************************************************************/
#include "areg/component/private/MpscEventQueue.hpp"

#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/Event.hpp"

#include <algorithm>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - constructor / destructor
//////////////////////////////////////////////////////////////////////////

MpscEventQueue::MpscEventQueue(QueueListener& eventListener, uint32_t maxQueue)
    : mStub         ( new Node{} )
    , mTail         ( mStub )
    , mHead         ( mStub )
    , mFastCount    ( 0u )
    , mCapacity     ( MpscEventQueue::_calc_capacity(maxQueue) )
    , mPrioLock     ( )
    , mPrioQueue    ( )
    , mListener     ( eventListener )
    , mNodePoolLock ( )
    , mNodePoolHead ( nullptr )
    , mNodePoolSize ( 0u )
{
    // Node{} zero-initialises: mStub->next == nullptr, mStub->event == nullptr.
    // mHead == mTail == mStub: the fast lane is empty.

    // Pre-populate the node pool to avoid cold-start heap allocation pressure
    // during the first burst of push_event() calls. The constructor is
    // single-threaded so the pool can be filled without the lock.
    for (uint32_t i{ 0u }; i < NODE_POOL_PRE_ALLOC; ++i)
    {
        Node* node{ new (std::nothrow) Node{} };
        if (node == nullptr)
            break;

        node->next.store(mNodePoolHead, std::memory_order_relaxed);
        mNodePoolHead = node;
        ++mNodePoolSize;
    }
}

MpscEventQueue::~MpscEventQueue()
{
    remove_all_events();
    // After remove_all_events() the fast lane contains only mStub.
    delete mStub;
    mStub = nullptr;

    // Drain the node pool. No lock needed: destruction is single-threaded
    // and no producers can be running at this point.
    while (mNodePoolHead != nullptr)
    {
        Node* node{ mNodePoolHead };
        mNodePoolHead = node->next.load(std::memory_order_relaxed);
        delete node;
    }
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - is_empty
//////////////////////////////////////////////////////////////////////////

bool MpscEventQueue::is_empty() const noexcept
{
    // Fast check: if any normal-priority items are counted, not empty.
    if (mFastCount.load(std::memory_order_acquire) != 0u)
    {
        return false;
    }

    // Check the Vyukov list directly (handles the edge case where
    // mFastCount was decremented but the node is still being recycled).
    if (mHead->next.load(std::memory_order_acquire) != nullptr)
    {
        return false;
    }

    // Check the priority lane without locking. We may occasionally
    // return a stale result, but callers tolerate that:
    // - push_event always signals the consumer after modifying the deque.
    // - The dispatcher drain loop uses pick_event(), not is_empty().
    Lock lock(const_cast<SpinLock&>(mPrioLock));
    return mPrioQueue.empty();
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - push_event
//////////////////////////////////////////////////////////////////////////

void MpscEventQueue::push_event(Event& eventElem, Event** removedEvent)
{
    const areg::EventPriority prio{ eventElem.event_priority() };

    if (prio == areg::EventPriority::ExitPrio)
    {
        Lock lock(mPrioLock);
        mPrioQueue.push_front(&eventElem);
        // Always signal for ExitEvent -- must wake the dispatcher immediately.
        mListener.signal_event(1u);
        return;
    }

    if (prio >= areg::EventPriority::HighPrio)
    {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end()
               && (*it)->event_priority() == areg::EventPriority::ExitPrio)
        {
            ++it;
        }

        mPrioQueue.insert(it, &eventElem);
        // Always signal for HighPrio -- these are rare control events.
        mListener.signal_event(1u);
        return;
    }

    // Normal-priority: lock-free MPSC fast lane.
    const uint32_t current = mFastCount.load(std::memory_order_relaxed);
    if (current >= mCapacity)
    {
        if (removedEvent != nullptr)
            *removedEvent = &eventElem;
        else
            eventElem.destroy();

        return;
    }

    Node* node  = _alloc_node();
    node->event = &eventElem;
    node->next.store(nullptr, std::memory_order_relaxed);

    mFastCount.fetch_add(1u, std::memory_order_relaxed);
    _mpsc_push(node);
    mListener.signal_event(1u);
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - pop_event
//////////////////////////////////////////////////////////////////////////

Event* MpscEventQueue::pop_event() noexcept
{
    // Priority lane is always drained first: ExitEvent must be processed
    // before any normal work.
    {
        mPrioLock.lock();
        if (!mPrioQueue.empty())
        {
            Event* result = mPrioQueue.front();
            mPrioQueue.pop_front();
            uint32_t prio_remaining = static_cast<uint32_t>(mPrioQueue.size());
            mPrioLock.unlock();

            uint32_t fast_count = mFastCount.load(std::memory_order_acquire);
            mListener.signal_event(prio_remaining + fast_count);
            return result;
        }

        mPrioLock.unlock();
    }

    // Fast lane.
    Node* node = _mpsc_pop();
    if (node != nullptr)
    {
        Event* result = node->event;
        _free_node(node);
        mFastCount.fetch_sub(1u, std::memory_order_relaxed);
        // mEventQueue is already armed from the push; no re-signal needed here.
        // The signal resets on the next call when _mpsc_pop() returns nullptr.
        return result;
    }

    // Reset the signal (or keep it set if priority items remain).
    // This must happen BEFORE the double-check so that a concurrent producer's
    // signal_event(1) always fires AFTER our reset, never silently before it.
    mListener.signal_event(_prio_count());

    // Double-check: a producer may have completed a push between our _mpsc_pop()
    // above and the signal_event() call. Re-try once to catch that case.
    node = _mpsc_pop();
    if (node == nullptr)
    {
        return nullptr;
    }

    // A producer completed between our null check and the reset -- re-arm and return.
    Event* result = node->event;
    _free_node(node);
    const uint32_t old_fast = mFastCount.fetch_sub(1u, std::memory_order_relaxed);
    const uint32_t fast_remaining = (old_fast > 0u) ? (old_fast - 1u) : 0u;

    mListener.signal_event(fast_remaining + _prio_count());
    return result;
}

void MpscEventQueue::remove_events() noexcept
{
    do {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end())
        {
            Event* evt = *it;
            if (evt->event_priority() == areg::EventPriority::ExitPrio)
            {
                ++it;
            }
            else
            {
                evt->destroy();
                it = mPrioQueue.erase(it);
            }
        }
    } while (false);

    std::deque<Event*> drained;
    _mpsc_drain_to(drained);
    for (Event* evt : drained)
    {
        evt->destroy();
    }

    mFastCount.store(0u, std::memory_order_relaxed);
    mListener.signal_event(_prio_count());
}

void MpscEventQueue::remove_events(const RuntimeClassID& eventClassId) noexcept
{
    do {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end())
        {
            Event* evt = *it;
            if ((evt->event_priority() != areg::EventPriority::ExitPrio) &&
                (eventClassId == evt->class_id()))
            {
                evt->destroy();
                it = mPrioQueue.erase(it);
            }
            else
            {
                ++it;
            }
        }
    } while (false);

    std::deque<Event*> drained;
    _mpsc_drain_to(drained);

    uint32_t kept{ 0u };
    for (Event* evt : drained)
    {
        if (eventClassId == evt->class_id())
        {
            evt->destroy();
        }
        else
        {
            Node* node  = _alloc_node();
            node->event = evt;
            node->next.store(nullptr, std::memory_order_relaxed);
            _mpsc_push(node);
            ++kept;
        }
    }

    mFastCount.store(kept, std::memory_order_relaxed);
    mListener.signal_event(kept + _prio_count());
}

void MpscEventQueue::remove_all_events() noexcept
{
    do {
        Lock lock(mPrioLock);
        for (Event* evt : mPrioQueue)
        {
            evt->destroy();
        }

        mPrioQueue.clear();
    } while (false);

    std::deque<Event*> drained;
    _mpsc_drain_to(drained);
    for (Event* evt : drained)
    {
        evt->destroy();
    }

    mFastCount.store(0u, std::memory_order_relaxed);
    mListener.signal_event(0u);
}

uint32_t MpscEventQueue::push_events(Event** eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u))
        return 0u;

    uint32_t signalCount{ 0u };
    uint32_t removedCount{ 0u };

    // Phase 1: insert all priority-lane events in ONE mPrioLock acquisition.
    // Mirrors push_event() for each priority class but avoids per-event lock/unlock overhead.
    Event* evt{ eventElems[0] };
    if ((evt != nullptr) && (evt->event_priority() > areg::EventPriority::NormalPrio))
    {
        Lock lock(mPrioLock);
        for (uint32_t i = 0u; i < count; ++i)
        {
            evt = eventElems[i];
            if (evt == nullptr)
                break;

            const areg::EventPriority prio{ evt->event_priority() };
            if (prio == areg::EventPriority::ExitPrio)
            {
                mPrioQueue.push_front(evt);
                eventElems[i] = nullptr;    // mark consumed
                ++signalCount;
            }
            else if (prio >= areg::EventPriority::HighPrio)
            {
                // Insert immediately after any ExitPrio entries.
                auto it = mPrioQueue.begin();
                while (it != mPrioQueue.end()
                       && (*it)->event_priority() == areg::EventPriority::ExitPrio)
                {
                    ++it;
                }

                mPrioQueue.insert(it, evt);
                eventElems[i] = nullptr;    // mark consumed
                ++signalCount;
            }
            else
            {
                break; // We reach normal priority, 
            }
        }
    }

    // Phase 2: insert below-HighPrio events into the lock-free fast lane.
    // Events that exceed the capacity limit are compacted into eventElems[0..removedCount).
    for (uint32_t i = signalCount; i < count; ++i)
    {
        evt = eventElems[i];
        if (evt == nullptr)
            continue;   // consumed in phase 1 or was nullptr on input

        if (mFastCount.load(std::memory_order_relaxed) >= mCapacity)
        {
            eventElems[removedCount++] = evt;
        }
        else
        {
            Node* node{ _alloc_node() };
            node->event = evt;
            node->next.store(nullptr, std::memory_order_relaxed);
            mFastCount.fetch_add(1u, std::memory_order_relaxed);
            _mpsc_push(node);
            ++signalCount;
        }
    }

    if (signalCount != 0u)
        mListener.signal_event(signalCount);

    return removedCount;
}

uint32_t MpscEventQueue::pop_events(Event** eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u))
        return 0u;

    uint32_t popped{ 0u };
    bool prio_exhausted{ false };

    // Phase 1: drain the priority lane with ONE mPrioLock acquisition.
    {
        Lock lock(mPrioLock);
        while (!mPrioQueue.empty() && (popped < count))
        {
            eventElems[popped++] = mPrioQueue.front();
            mPrioQueue.pop_front();
        }

        prio_exhausted = mPrioQueue.empty();
    }

    // Phase 2: drain fast lane into remaining output slots (consumer thread only).
    while (popped < count)
    {
        Node* node{ _mpsc_pop() };
        if (node == nullptr)
            break;

        eventElems[popped++] = node->event;
        _free_node(node);
        mFastCount.fetch_sub(1u, std::memory_order_relaxed);
    }

    if (popped == 0u)
        return 0u;

    const uint32_t fast_remaining{ mFastCount.load(std::memory_order_acquire) };
    const uint32_t prio_remaining{ prio_exhausted ? 0u : _prio_count() };
    mListener.signal_event(fast_remaining + prio_remaining);

    return popped;
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - Vyukov MPSC algorithm
//////////////////////////////////////////////////////////////////////////

inline void MpscEventQueue::_mpsc_push(Node* node) noexcept
{
    Node* prev = mTail.exchange(node, std::memory_order_acq_rel);
    prev->next.store(node, std::memory_order_release);
}

inline MpscEventQueue::Node* MpscEventQueue::_mpsc_pop() noexcept
{
    Node* head = mHead;
    Node* next = head->next.load(std::memory_order_acquire);

    if (next == nullptr)
    {
        return nullptr;
    }

    // Vyukov trick: recycle the old stub as the node to return.
    // Transfer the real payload from the first live node (next) onto
    // the old stub (head), then advance mHead and mStub to next.
    // The caller receives head (old stub) carrying the event and must
    // call _free_node() on it.  next becomes the new stub and must
    // never be freed until the destructor.
    head->event = next->event;
    next->event = nullptr;
    mHead       = next;
    mStub       = next;   // keep mStub in sync with the current sentinel

    return head;
}

void MpscEventQueue::_mpsc_drain_to(std::deque<Event*>& out) noexcept
{
    for (;;)
    {
        Node* node = _mpsc_pop();
        if (node == nullptr)
        {
            break;
        }

        out.push_back(node->event);
        _free_node(node);
    }
}

MpscEventQueue::Node* MpscEventQueue::_alloc_node() noexcept
{
    {
        Lock guard(mNodePoolLock);
        if (mNodePoolHead != nullptr)
        {
            Node* node{ mNodePoolHead };
            mNodePoolHead = node->next.load(std::memory_order_relaxed);
            --mNodePoolSize;
            node->next.store(nullptr, std::memory_order_relaxed);
            node->event = nullptr;
            return node;
        }
    }

    return new (std::nothrow) Node{};
}

void MpscEventQueue::_free_node(Node* node) noexcept
{
    // Clear payload before pooling so stale event pointers are never visible.
    node->event = nullptr;

    {
        Lock guard(mNodePoolLock);
        if (mNodePoolSize < NODE_POOL_MAX)
        {
            node->next.store(mNodePoolHead, std::memory_order_relaxed);
            mNodePoolHead = node;
            ++mNodePoolSize;
            return;
        }
    }

    delete node;
}

inline uint32_t MpscEventQueue::_prio_count() noexcept
{
    Lock lock(mPrioLock);
    return static_cast<uint32_t>(mPrioQueue.size());
}

uint32_t MpscEventQueue::_calc_capacity(uint32_t requested) noexcept
{
    constexpr uint32_t MAX_CAPACITY { std::numeric_limits<uint32_t>::max() };
    constexpr uint32_t MIN_CAPACITY { 32u };

    // 0 means explicitly unlimited: the queue grows without bound.
    // This allows callers such as SEND_THREAD_QUEUE_LIMIT = 0 to opt out
    // of capacity capping without touching the application configuration.
    if (requested == 0u)
        return MAX_CAPACITY;

    return std::max(MIN_CAPACITY, requested);
}

} // namespace areg