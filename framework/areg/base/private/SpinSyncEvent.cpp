/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SpinSyncEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, platform-independent SpinSyncEvent implementation.
 *              Constructor, signaling, spin phase, waiter-list management, wait_any.
 *              OS-level sleep / wake primitives are in the per-platform files:
 *                areg/base/private/win32/SpinSyncEventWin32.cpp
 *                areg/base/private/posix/SpinSyncEventPosix.cpp
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/SpinSyncEvent.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent constructor
//////////////////////////////////////////////////////////////////////////

SpinSyncEvent::SpinSyncEvent( bool     initLock  /* = true  */
                            , bool     autoReset /* = true  */
                            , uint32_t spinCount /* = DEFAULT_SPIN_COUNT */ ) noexcept
    : mState        (initLock ? 0u : 1u)
    , mSleeperCount (0)
    , mAutoReset    (autoReset)
    , mSpinCount    (spinCount)
    , mWaiters      (nullptr)
{
    mWaitersLock.clear(std::memory_order_relaxed);
}

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent signal / reset
//////////////////////////////////////////////////////////////////////////

bool SpinSyncEvent::set_signaled() noexcept
{
    mState.store(1u, std::memory_order_release);

    // Only invoke the OS wake primitive when threads are actually sleeping.
    // Threads still in the spin phase observe mState != 0 on their next load
    // without any OS involvement.
    if (mSleeperCount.load(std::memory_order_acquire) > 0)
    {
        _os_wake();
    }

    _notify_waiters();
    return true;
}

bool SpinSyncEvent::reset() noexcept
{
    mState.store(0u, std::memory_order_release);
    return true;
}

void SpinSyncEvent::pulse_event() noexcept
{
    mState.store(1u, std::memory_order_release);
    _os_wake_all();
    _notify_waiters();
    mState.store(0u, std::memory_order_release);
}

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent::_do_wait — spin phase + OS sleep fallback
//////////////////////////////////////////////////////////////////////////

bool SpinSyncEvent::_do_wait(uint32_t timeout_ms) noexcept
{
    if (timeout_ms == areg::DO_NOT_WAIT)
        return _try_consume();

    using Clock = std::chrono::steady_clock;
    using Ms = std::chrono::milliseconds;

    const bool infinite { timeout_ms == areg::WAIT_INFINITE };
    const Clock::time_point start { infinite ? Clock::time_point{} : Clock::now() };

    // Spin phase:
    // Busy-wait for mSpinCount iterations. Signals arriving within this window are
    // caught with a simple atomic load — no syscall, no scheduler round-trip.
    for (uint32_t i { 0u }; i < mSpinCount; ++i)
    {
        if (_try_consume())
            return true;

        _spin_hint();
    }

    // final check before transitioning to OS sleep.
    if (_try_consume())
        return true;

    // OS sleep phase:
    // Compute remaining budget so the OS sleep does not overshoot the caller's deadline.
    uint32_t sleep_ms { timeout_ms };
    if (!infinite)
    {
        const auto elapsed  { std::chrono::duration_cast<Ms>(Clock::now() - start) };
        const int64_t remaining { static_cast<int64_t>(timeout_ms) - elapsed.count() };
        if (remaining <= 0)
            return false;

        sleep_ms = static_cast<uint32_t>(std::min<int64_t>(remaining, static_cast<int64_t>(areg::WAIT_INFINITE - 1u)));
    }

    // Record this thread as sleeping so set_signaled() calls _os_wake().
    mSleeperCount.fetch_add(1, std::memory_order_relaxed);
    const bool result { _os_wait(sleep_ms) };
    mSleeperCount.fetch_sub(1, std::memory_order_relaxed);

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent::wait_any
//////////////////////////////////////////////////////////////////////////

int32_t SpinSyncEvent::wait_any( SpinSyncEvent** events
                               , int32_t         count
                               , uint32_t        timeout    /* = areg::WAIT_INFINITE */
                               , uint32_t        spin_count /* = DEFAULT_SPIN_COUNT */ ) noexcept
{
    if ((events == nullptr) || (count <= 0) || (count > MAX_WAIT_ANY))
        return WAIT_INDEX_TIMEOUT;

    // agg and sleeping share one 64-byte cache line, the signaler touches both
    alignas(64) std::atomic<uint32_t> agg     { 0u };
                std::atomic<bool>    sleeping { false };
    WaiterNode nodes[MAX_WAIT_ANY];

    // Register every node and check state.
    for (int32_t i { 0 }; i < count; ++i)
    {
        nodes[i].mAggWord  = &agg;
        nodes[i].mBitMask  = (1u << static_cast<uint32_t>(i));
        nodes[i].mNext     = nullptr;
        nodes[i].mSleeping = &sleeping;

        events[i]->_register_waiter(&nodes[i]);

        if (events[i]->mState.load(std::memory_order_acquire) != 0u)
        {
            agg.fetch_or(nodes[i].mBitMask, std::memory_order_release);
        }
    }

    if (agg.load(std::memory_order_acquire) == 0u)
    {
        // Spin phase on the aggregator word:
        // Any event firing ORs a bit into agg; we see it here without OS involvement.
        for (uint32_t i { 0u };
             (i < spin_count) && (agg.load(std::memory_order_relaxed) == 0u);
             ++i)
        {
            _spin_hint();
        }

        if (agg.load(std::memory_order_acquire) == 0u)
        {
            sleeping.store(true, std::memory_order_release);
            if (agg.load(std::memory_order_acquire) == 0u)
            {
                _os_wait_on(&agg, timeout);
            }

            sleeping.store(false, std::memory_order_release);
        }
    }

    for (int32_t i { 0 }; i < count; ++i)
    {
        events[i]->_unregister_waiter(&nodes[i]);
    }

    const uint32_t bits { agg.load(std::memory_order_acquire) };
    if (bits == 0u)
        return WAIT_INDEX_TIMEOUT;

    for (int32_t i { 0 }; i < count; ++i)
    {
        if ((bits & (1u << static_cast<uint32_t>(i))) != 0u)
            return i;
    }

    return WAIT_INDEX_TIMEOUT;
}

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent waiter-list management (private helpers)
//////////////////////////////////////////////////////////////////////////

void SpinSyncEvent::_register_waiter(WaiterNode* node) noexcept
{
    while (mWaitersLock.test_and_set(std::memory_order_acquire))
    {
    }

    node->mNext = mWaiters;
    mWaiters    = node;

    mWaitersLock.clear(std::memory_order_release);
}

void SpinSyncEvent::_unregister_waiter(WaiterNode* node) noexcept
{
    while (mWaitersLock.test_and_set(std::memory_order_acquire))
    {
    }

    WaiterNode** cur { &mWaiters };
    while (*cur != nullptr)
    {
        if (*cur == node)
        {
            *cur        = node->mNext;
            node->mNext = nullptr;
            break;
        }

        cur = &(*cur)->mNext;
    }

    mWaitersLock.clear(std::memory_order_release);
}

void SpinSyncEvent::_notify_waiters() noexcept
{
    // Rationale: _os_wake_on() (WakeByAddressAll / FUTEX_WAKE / __ulock_wake) is
    // non-blocking — it only marks threads as ready-to-run in the kernel run queue;
    // it does NOT wait for the woken thread to execute.
    while (mWaitersLock.test_and_set(std::memory_order_acquire))
    {
    }

    for (WaiterNode* cur { mWaiters }; cur != nullptr; cur = cur->mNext)
    {
        cur->mAggWord->fetch_or(cur->mBitMask, std::memory_order_release);
        if ((cur->mSleeping != nullptr) && cur->mSleeping->load(std::memory_order_acquire))
        {
            _os_wake_on(cur->mAggWord);
        }
    }

    mWaitersLock.clear(std::memory_order_release);
}

} // namespace areg
