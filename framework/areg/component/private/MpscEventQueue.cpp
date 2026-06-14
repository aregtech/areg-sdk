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
#include "areg/base/private/DebugDefs.hpp"

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
    , mPrioCount    ( 0u )
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
    _delete_nodes();
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
    return ((mFastCount.load(std::memory_order_acquire) == 0u) && (mPrioCount.load(std::memory_order_relaxed) == 0u));
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - push_event
//////////////////////////////////////////////////////////////////////////

void MpscEventQueue::push_event(Event& eventElem, Event* removedEvent /*= nullptr*/)
{
    const areg::EventPriority prio{ eventElem.event_priority() };

    if (prio == areg::EventPriority::ExitPrio)
    {
        Lock lock(mPrioLock);
        mPrioQueue.push_front(std::move(eventElem));
        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()), std::memory_order_relaxed);
        mListener.signal_event(1u);
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
        mListener.signal_event(1u);
        return;
    }

    // Normal-priority: lock-free MPSC fast lane.
    const uint32_t prev{ mFastCount.load(std::memory_order_relaxed) };
    if (prev >= mCapacity)
    {
        if (removedEvent != nullptr)
            *removedEvent = std::move(eventElem);
        // else: eventElem stays unmodified (capacity overflow -- caller decides)
        return;
    }

    Node* node{ _alloc_node() };
    if (node == nullptr)
    {
        if (removedEvent != nullptr)
            *removedEvent = std::move(eventElem);
        return;
    }

    node->event = std::move(eventElem);
    node->next.store(nullptr, std::memory_order_relaxed);
#ifdef AREG_LATENCY_TRACE
    node->lt_ns = AREG_LT_NOW();    // stamp before publishing; visible to consumer via the release in _mpsc_push
#endif
    _mpsc_push(node);
    mFastCount.fetch_add(1u, std::memory_order_relaxed);
    mListener.signal_event(1u);
}

//////////////////////////////////////////////////////////////////////////
// MpscEventQueue - pop_event
//////////////////////////////////////////////////////////////////////////

Event MpscEventQueue::pop_event() noexcept
{
    // Priority lane: always drained before the fast lane.
    if (mPrioCount.load(std::memory_order_relaxed) != 0u)
    {
        Lock lock(mPrioLock);

        if (!mPrioQueue.empty())
        {
            Event result{ std::move(mPrioQueue.front()) };
            mPrioQueue.pop_front();
            const uint32_t prioRemaining{ static_cast<uint32_t>(mPrioQueue.size()) };
            mPrioCount.store(prioRemaining, std::memory_order_relaxed);
            mListener.signal_event(prioRemaining + mFastCount.load(std::memory_order_acquire));
            return result;
        }
    }

    // Fast lane (consumer thread only).
    Node* node{ _mpsc_pop() };
    if (node != nullptr)
    {
        Event result{ std::move(node->event) };
#ifdef AREG_LATENCY_TRACE
        AREG_LT_SAMPLE(areg::LtStage::MpscHandoff, AREG_LT_NOW() - node->lt_ns);
#endif
        _free_node(node);
        const uint32_t remaining{ mFastCount.fetch_sub(1u, std::memory_order_relaxed) - 1u };
        // Include the priority lane so the queue event is not reset while an Exit/High event waits there.
        mListener.signal_event(remaining + mPrioCount.load(std::memory_order_acquire));
        return result;
    }

    return Event{};
}

void MpscEventQueue::remove_events() noexcept
{
    mPrioLock.lock();
    uint32_t prioCount{ 0u };
    // Preserve at most one ExitPrio event; destroy the rest.
    Event exitEvt{};
    for (Event& evt : mPrioQueue)
    {
        if (evt.is_exit_prio())
        {
            exitEvt = std::move(evt);
        }
        else
        {
            evt.destroy_event();
        }
    }

    mPrioQueue.clear();
    if (exitEvt.is_valid())
    {
        mPrioQueue.push_back(std::move(exitEvt));
        prioCount = 1u;
    }

    mPrioCount.store(prioCount, std::memory_order_relaxed);
    mPrioLock.unlock();

    std::vector<Event> drained;
    _mpsc_drain_to(drained);
    for (Event& evt : drained)
        evt.destroy_event();

    mFastCount.store(0u, std::memory_order_relaxed);
    mListener.signal_event(prioCount);
}

void MpscEventQueue::remove_events(const uint32_t eventClassId) noexcept
{
    if (mPrioCount.load(std::memory_order_relaxed) != 0)
    {
        Lock lock(mPrioLock);
        auto it = mPrioQueue.begin();
        while (it != mPrioQueue.end())
        {
            if ((!it->is_exit_prio()) && (eventClassId == it->event_id()))
            {
                it->destroy_event();
                it = mPrioQueue.erase(it);
            }
            else
            {
                ++it;
            }
        }

        mPrioCount.store(static_cast<uint32_t>(mPrioQueue.size()));
    }

    // Drain fast lane: matching events destroyed inside; non-matching collected and re-pushed.
    std::vector<Event> kept;
    _mpsc_drain_to(kept, eventClassId);

    uint32_t re_pushed{ 0u };
    for (Event& evt : kept)
    {
        Node* node{ _alloc_node() };
        if (node != nullptr)
        {
            node->event = std::move(evt);
            node->next.store(nullptr, std::memory_order_relaxed);
            _mpsc_push(node);
            ++re_pushed;
        }
        else
        {
            evt.destroy_event();
        }
    }

    mFastCount.store(re_pushed, std::memory_order_relaxed);
    mListener.signal_event(re_pushed + mPrioCount.load(std::memory_order_relaxed));
}

void MpscEventQueue::remove_all_events() noexcept
{
    if (mPrioCount.load(std::memory_order_relaxed))
    {
        Lock lock(mPrioLock);
        for (Event& evt : mPrioQueue)
            evt.destroy_event();

        mPrioQueue.clear();
        mPrioCount.store(0u, std::memory_order_relaxed);
    }

    std::vector<Event> drained;
    _mpsc_drain_to(drained);
    for (Event& evt : drained)
        evt.destroy_event();

    mFastCount.store(0u, std::memory_order_relaxed);
    mListener.signal_event(0u);
}

uint32_t MpscEventQueue::push_events(Event* eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u))
        return 0u;

    uint32_t signalCount{ 0u };

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
                mPrioQueue.push_front(std::move(evt));
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

    // Phase 2: insert NormalPrio events into the lock-free fast lane.
    uint32_t removedCount{ 0u };
    for (uint32_t i = signalCount; i < count; ++i)
    {
        Event& evt = eventElems[i];
        if (!evt.is_valid())
            continue;

        const uint32_t prev{ mFastCount.fetch_add(1u, std::memory_order_relaxed) };
        if (prev >= mCapacity)
        {
            if (removedCount < i)
                eventElems[removedCount] = std::move(evt);
            ++removedCount;
            mFastCount.fetch_sub(1u, std::memory_order_relaxed);
            continue;
        }

        Node* node{ _alloc_node() };
        if (node == nullptr)
        {
            if (removedCount < i)
                eventElems[removedCount] = std::move(evt);
            ++removedCount;
            mFastCount.fetch_sub(1u, std::memory_order_relaxed);
            continue;
        }

        node->event = std::move(evt);
        node->next.store(nullptr, std::memory_order_relaxed);
        _mpsc_push(node);
        ++signalCount;
    }

    if (signalCount != 0u)
        mListener.signal_event(signalCount);

    return removedCount;
}

uint32_t MpscEventQueue::pop_events(Event* eventElems, uint32_t count)
{
    if ((eventElems == nullptr) || (count == 0u))
        return 0u;

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

    // Phase 2: drain fast lane into remaining output slots (consumer thread only).
    while (popped < count)
    {
        Node* node{ _mpsc_pop() };
        if (node == nullptr)
            break;

        eventElems[popped++] = std::move(node->event);
        _free_node(node);
        mFastCount.fetch_sub(1u, std::memory_order_relaxed);
    }

    if (popped == 0u)
        return 0u;

    const uint32_t fast_remaining{ mFastCount.load(std::memory_order_acquire) };
    mListener.signal_event(fast_remaining + mPrioCount.load(std::memory_order_relaxed));

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
        head->event = std::move(next->event);  // move Event value into sentinel
#ifdef AREG_LATENCY_TRACE
        head->lt_ns = next->lt_ns;             // carry enqueue stamp onto the returned node
#endif
        // next->event is now empty (moved-from)
        mHead = next;
        return head;
    }

    return nullptr;
}

void MpscEventQueue::_mpsc_drain_to(std::vector<Event>& out) noexcept
{
    out.reserve(mFastCount.load(std::memory_order_relaxed));
    for (;;)
    {
        Node* node{ _mpsc_pop() };
        if (node == nullptr)
            break;

        out.push_back(std::move(node->event));
        _free_node(node);
    }
}

void MpscEventQueue::_mpsc_drain_to(std::vector<Event>& out, const uint32_t classId) noexcept
{
    if (classId == 0u)
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

            if (node->event.event_id() == classId)
                node->event.destroy_event();  // discard matching events
            else
                out.push_back(std::move(node->event));

            _free_node(node);
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
            cur.head->event.destroy_event();  // reset the Event value to empty
            return cur.head;
        }
    }

    return new (std::nothrow) Node{};
}

inline void MpscEventQueue::_free_node(Node* node) noexcept
{
    node->event.destroy_event();  // reset the Event value to empty before pooling

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
    return mPrioCount.load(std::memory_order_relaxed);
}

inline void MpscEventQueue::_delete_nodes() noexcept
{
    mPrioLock.lock();
    for (Event& evt : mPrioQueue)
        evt.destroy_event();  // release payload; Event destructor handles the rest

    mPrioQueue.clear();
    mPrioCount.store(0u, std::memory_order_relaxed);
    mPrioLock.unlock();

    // Drain fast lane -- Event values in nodes go out of scope when node is deleted.
    for (;;)
    {
        Node* next = mHead->next.load(std::memory_order_acquire);
        if (next == nullptr)
            break;

        Node* head = mHead;
        head->event = std::move(next->event);  // move Event into head (sentinel)
        mHead = next;
        head->event.destroy_event();
        delete head;
    }

    mFastCount.store(0u, std::memory_order_relaxed);
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
