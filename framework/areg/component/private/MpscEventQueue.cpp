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
    : mTail         ( nullptr )
    , mHead         ( nullptr )
    , mFastCount    ( 0u )
    , mCapacity     ( MpscEventQueue::_calc_capacity(maxQueue) )
    , mPrioLock     ( )
    , mPrioQueue    ( )
    , mListener     ( eventListener )
    , mPool         ( PoolTop{} )
    , mPoolSize     ( 0u )
{
    static_assert(sizeof(PoolTop) == sizeof(Node*) + sizeof(uintptr_t), "PoolTop must have no padding; ABA counter type mismatch for this word size");

    Node* stub{ new Node{} };
    mTail.store(stub, std::memory_order_relaxed);
    mHead = stub;

    // Pre-populate pool as a plain linked list (single-threaded construction).
    Node* poolHead{ nullptr };
    uint32_t poolCount{ 0u };
    for (uint32_t i{ 0u }; i < NODE_POOL_PRE_ALLOC; ++i)
    {
        Node* node{ new (std::nothrow) Node{} };
        if (node == nullptr)
            break;

        node->next.store(poolHead, std::memory_order_relaxed);
        poolHead = node;
        ++poolCount;
    }

    mPool.store(PoolTop{ poolHead, 0u }, std::memory_order_relaxed);
    mPoolSize.store(poolCount, std::memory_order_relaxed);
}

MpscEventQueue::~MpscEventQueue()
{
    remove_all_events();
    delete mHead;   // mHead is always the current sentinel node
    mHead = nullptr;

    PoolTop top{ mPool.load(std::memory_order_relaxed) };
    while (top.head != nullptr)
    {
        Node* node{ top.head };
        top.head = node->next.load(std::memory_order_relaxed);
        delete node;
    }
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - is_empty
//////////////////////////////////////////////////////////////////////////

bool MpscEventQueue::is_empty() const noexcept
{
    if (mFastCount.load(std::memory_order_acquire) != 0u)
    {
        return false;
    }

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
        mListener.signal_event(1u);
        return;
    }

    if (prio >= areg::EventPriority::HighPrio)
    {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end() && (*it)->event_priority() > prio)
            ++it;

        mPrioQueue.insert(it, &eventElem);
        mListener.signal_event(1u);
        return;
    }

    // Normal-priority: lock-free MPSC fast lane.
    const uint32_t prev{ mFastCount.fetch_add(1u, std::memory_order_relaxed) };
    if (prev >= mCapacity)
    {
        mFastCount.fetch_sub(1u, std::memory_order_relaxed);
        if (removedEvent != nullptr)
            *removedEvent = &eventElem;
        else
            eventElem.destroy();

        return;
    }

    Node* node{ _alloc_node() };
    if (node == nullptr)
    {
        mFastCount.fetch_sub(1u, std::memory_order_relaxed);
        if (removedEvent != nullptr)
            *removedEvent = &eventElem;
        else
            eventElem.destroy();

        return;
    }

    node->event = &eventElem;
    node->next.store(nullptr, std::memory_order_relaxed);
    _mpsc_push(node);
    mListener.signal_event(1u);
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - pop_event
//////////////////////////////////////////////////////////////////////////

Event* MpscEventQueue::pop_event() noexcept
{
    // Priority lane: always drained before the fast lane.
    mPrioLock.lock();
    if (!mPrioQueue.empty())
    {
        Event* result{ mPrioQueue.front() };
        mPrioQueue.pop_front();
        const uint32_t prioRemaining{ static_cast<uint32_t>(mPrioQueue.size()) };
        mPrioLock.unlock();

        mListener.signal_event(prioRemaining + mFastCount.load(std::memory_order_acquire));
        return result;
    }

    mPrioLock.unlock();

    // Fast lane (consumer thread only).
    Node* node{ _mpsc_pop() };
    if (node != nullptr)
    {
        Event* result{ node->event };
        _free_node(node);
        const uint32_t remaining{ mFastCount.fetch_sub(1u, std::memory_order_relaxed) - 1u };
        mListener.signal_event(remaining);
        return result;
    }

    return nullptr;
}

void MpscEventQueue::remove_events() noexcept
{
    mPrioLock.lock();
    Event* exit{ nullptr };
    uint32_t prioCount{ 0u };
    for (Event* evt : mPrioQueue)
    {
        if (evt->event_priority() == areg::EventPriority::ExitPrio)
        {
            if (exit != nullptr)
                exit->destroy();    // no-op for ExitEvent
            exit = evt;
        }
        else
        {
            evt->destroy();
        }
    }

    mPrioQueue.clear();
    if (exit != nullptr)
    {
        mPrioQueue.push_back(exit);
        prioCount = 1u;
    }
    mPrioLock.unlock();

    std::vector<Event*> drained;
    _mpsc_drain_to(drained);
    for (Event* evt : drained)
        evt->destroy();

    mFastCount.store(0u, std::memory_order_relaxed);
    mListener.signal_event(prioCount);
}

void MpscEventQueue::remove_events(const RuntimeClassID& eventClassId) noexcept
{
    mPrioLock.lock();
    auto it = mPrioQueue.begin();
    while (it != mPrioQueue.end())
    {
        Event* evt{ *it };
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

    mPrioLock.unlock();

    // Drain fast lane: matching events destroyed inside; non-matching collected.
    std::vector<Event*> kept;
    _mpsc_drain_to(kept, eventClassId);

    uint32_t re_pushed{ 0u };
    for (Event* evt : kept)
    {
        Node* node{ _alloc_node() };
        if (node != nullptr)
        {
            node->event = evt;
            node->next.store(nullptr, std::memory_order_relaxed);
            _mpsc_push(node);
            ++re_pushed;
        }
        else
        {
            evt->destroy();
        }
    }

    mFastCount.store(re_pushed, std::memory_order_relaxed);
    mListener.signal_event(re_pushed + _prio_count());
}

void MpscEventQueue::remove_all_events() noexcept
{
    mPrioLock.lock();
    for (Event* evt : mPrioQueue)
        evt->destroy();

    mPrioQueue.clear();
    mPrioLock.unlock();

    std::vector<Event*> drained;
    _mpsc_drain_to(drained);
    for (Event* evt : drained)
        evt->destroy();

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
                eventElems[i] = nullptr;
                ++signalCount;
            }
            else if (prio >= areg::EventPriority::HighPrio)
            {
                auto it = mPrioQueue.begin();
                while (it != mPrioQueue.end() && (*it)->event_priority() > prio)
                    ++it;

                mPrioQueue.insert(it, evt);
                eventElems[i] = nullptr;
                ++signalCount;
            }
            else
            {
                break;
            }
        }
    }

    // Phase 2: insert below-HighPrio events into the lock-free fast lane.
    for (uint32_t i = signalCount; i < count; ++i)
    {
        evt = eventElems[i];
        if (evt == nullptr)
            continue;

        const uint32_t prev{ mFastCount.fetch_add(1u, std::memory_order_relaxed) };
        if (prev >= mCapacity)
        {
            mFastCount.fetch_sub(1u, std::memory_order_relaxed);
            eventElems[removedCount++] = evt;
            continue;
        }

        Node* node{ _alloc_node() };
        if (node == nullptr)
        {
            mFastCount.fetch_sub(1u, std::memory_order_relaxed);
            eventElems[removedCount++] = evt;
            continue;
        }

        node->event = evt;
        node->next.store(nullptr, std::memory_order_relaxed);
        _mpsc_push(node);
        ++signalCount;
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
    uint32_t prioCount{ 0u };

    // Phase 1: drain the priority lane with ONE mPrioLock acquisition.
    {
        Lock lock(mPrioLock);
        while (!mPrioQueue.empty() && (popped < count))
        {
            eventElems[popped++] = mPrioQueue.front();
            mPrioQueue.pop_front();
        }

        prioCount = static_cast<uint32_t>(mPrioQueue.size());
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
    mListener.signal_event(fast_remaining + prioCount);

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
    Node* next = mHead->next.load(std::memory_order_acquire);
    if (next != nullptr)
    {
        Node* head = mHead;
        head->event = next->event;
        next->event = nullptr;
        mHead = next;
        return head;
    }

    return nullptr;
}

void MpscEventQueue::_mpsc_drain_to(std::vector<Event*>& out) noexcept
{
    out.reserve(mFastCount.load(std::memory_order_relaxed));
    for (;;)
    {
        Node* node{ _mpsc_pop() };
        if (node == nullptr)
            break;

        out.push_back(node->event);
        _free_node(node);
    }
}

void MpscEventQueue::_mpsc_drain_to(std::vector<Event*>& out, const RuntimeClassID& classId) noexcept
{
    const uint32_t id{ static_cast<uint32_t>(classId) };
    if (id == 0)
    {
        _mpsc_drain_to(out);
    }
    else
    {
        out.reserve(mFastCount.load(std::memory_order_relaxed));
        for (;;)
        {
            Node* node{ _mpsc_pop() };
            if (node == nullptr)
                break;

            Event* evt{ node->event };
            _free_node(node);
            if (static_cast<uint32_t>(evt->class_id()) == id)
                evt->destroy();
            else
                out.push_back(evt);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - lock-free node pool (DCAS LIFO)
//////////////////////////////////////////////////////////////////////////

inline MpscEventQueue::Node* MpscEventQueue::_alloc_node() noexcept
{
    PoolTop cur{ mPool.load(std::memory_order_acquire) };
    while (cur.head != nullptr)
    {
        const PoolTop next{ cur.head->next.load(std::memory_order_relaxed), cur.seq + 1u };
        if (mPool.compare_exchange_weak(cur, next, std::memory_order_acq_rel, std::memory_order_acquire))
        {
            mPoolSize.fetch_sub(1u, std::memory_order_relaxed);
            cur.head->next.store(nullptr, std::memory_order_relaxed);
            cur.head->event = nullptr;
            return cur.head;
        }
    }

    return new (std::nothrow) Node{};
}

inline void MpscEventQueue::_free_node(Node* node) noexcept
{
    node->event = nullptr;

    const uint32_t oldSize{ mPoolSize.fetch_add(1u, std::memory_order_relaxed) };
    if (oldSize >= NODE_POOL_MAX)
    {
        mPoolSize.fetch_sub(1u, std::memory_order_relaxed);
        delete node;
        return;
    }

    PoolTop cur{ mPool.load(std::memory_order_acquire) };
    PoolTop next;
    do {
        node->next.store(cur.head, std::memory_order_relaxed);
        next = PoolTop{ node, cur.seq + 1u };
    } while (!mPool.compare_exchange_weak(cur, next, std::memory_order_acq_rel, std::memory_order_acquire));
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

    if (requested == 0u)
        return MAX_CAPACITY;
    else if (requested < MIN_CAPACITY)
        return MIN_CAPACITY;
    else
        return requested;
}

} // namespace areg
