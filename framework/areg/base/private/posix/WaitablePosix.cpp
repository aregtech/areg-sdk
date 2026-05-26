/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitablePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Mutex wrapper class.
 *
 ************************************************************************/

#if   defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"

// --------------------------------------------------------------------------
// Platform-specific wake primitive used by notify_any_waiters()
// --------------------------------------------------------------------------
#if defined(__APPLE__)

extern "C" {
    int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);
}

#elif defined(__CYGWIN__)

#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>

#elif defined(__linux__)

#  include <linux/futex.h>
#  include <sys/syscall.h>
#  include <unistd.h>

#endif  // defined(__APPLE__) / defined(__CYGWIN__) / defined(__linux__)

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// SyncWaitable class implementation
//////////////////////////////////////////////////////////////////////////

WaitablePosix::WaitablePosix( areg::os::SyncKind syncType )
    : SyncObjectPosix ( syncType )
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    , mWaiters        { nullptr }
#endif
    , mValid          { true }
    , mWaitAllCount   { 0u }
{
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    mWaitersLock.clear(std::memory_order_relaxed);
#endif
}

WaitablePosix::~WaitablePosix()
{
    ASSERT(SyncLockAndWaitPosix::is_waitable_registered(*this) == false);
}

void WaitablePosix::free_resources()
{
    mValid.store(false, std::memory_order_relaxed);
    SyncLockAndWaitPosix::event_remove(*this);
}

//////////////////////////////////////////////////////////////////////////
// WaitAny intrusive-list operations (Linux / macOS / Cygwin)
//////////////////////////////////////////////////////////////////////////

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

namespace {

inline void _wake_one_waiter(std::atomic<uint32_t>* word) noexcept
{
#if defined(__linux__)
    ::syscall(SYS_futex, word, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
#elif defined(__APPLE__)
    ::__ulock_wake(areg::os::APPLE_ULOCK_COMPARE_AND_WAIT, word, 0u);
#else  // Cygwin
    ::WakeByAddressSingle(word);
#endif
}

} // anonymous namespace

void WaitablePosix::register_waiter( WaiterNode* node ) noexcept
{
    while (mWaitersLock.test_and_set(std::memory_order_acquire)) {}

    node->mNext = mWaiters.load(std::memory_order_relaxed);
    mWaiters.store(node, std::memory_order_release);

    mWaitersLock.clear(std::memory_order_release);
}

void WaitablePosix::unregister_waiter( WaiterNode* node ) noexcept
{
    while (mWaitersLock.test_and_set(std::memory_order_acquire)) {}

    WaiterNode* head = mWaiters.load(std::memory_order_relaxed);
    if (head == node)
    {
        mWaiters.store(node->mNext, std::memory_order_relaxed);
    }
    else
    {
        WaiterNode* prev = head;
        while ((prev != nullptr) && (prev->mNext != node))
        {
            prev = prev->mNext;
        }

        if (prev != nullptr)
        {
            prev->mNext = node->mNext;
        }
    }

    node->mNext = nullptr;
    mWaitersLock.clear(std::memory_order_release);
}

int32_t WaitablePosix::notify_any_waiters() noexcept
{
    // Collect wake targets while holding the spinlock, then issue wake syscalls
    // outside. This prevents unregister_waiter() from spinning on mWaitersLock
    // for the entire duration of each kernel wake syscall (~100-500 ns each).
    std::atomic<uint32_t>* toWake[areg::MAXIMUM_WAITING_OBJECTS];
    int32_t wakeCount{ 0 };

    while (mWaitersLock.test_and_set(std::memory_order_acquire)) {}

    WaiterNode* node{ mWaiters.load(std::memory_order_relaxed) };
    while (node != nullptr)
    {
        WaiterNode* const next{ node->mNext };

        // If the CAS fails, the waiter already received a signal or timed out, skip it
        uint32_t expected{ SYNC_FIRE_INVALID };
        if (node->mFiredWord->compare_exchange_strong(
                expected, node->mFiredValue,
                std::memory_order_acq_rel,
                std::memory_order_relaxed))
        {
            toWake[wakeCount++] = node->mFiredWord;

            // For objects that allow only one concurrent owner (e.g., Mutex), wake only one waiter per signal.
            if (!can_signal_threads())
            {
                break;
            }
        }

        node = next;
    }

    // Release spinlock BEFORE wake syscalls: the woken thread calls unregister_waiter()
    // immediately on resume; it must not spin waiting for us to finish kernel calls.
    mWaitersLock.clear(std::memory_order_release);

    for (int32_t i{ 0 }; i < wakeCount; ++i)
    {
        _wake_one_waiter(toWake[i]);
    }

    return wakeCount;
}

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

} // namespace areg::os
#endif  //  defined(_POSIX) || defined(POSIX)
