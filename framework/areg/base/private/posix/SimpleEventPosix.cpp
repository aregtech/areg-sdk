/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SimpleEventPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lightweight single-object event - POSIX futex implementation.
 ************************************************************************/
/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SimpleEvent.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SyncDefsPosix.hpp"

#include <chrono>
#include <climits>

// --------------------------------------------------------------------------
// Platform-specific wait / wake primitives
// --------------------------------------------------------------------------
#if defined(__APPLE__)

extern "C" {
    int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout_us);
    int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);
}

#elif defined(__CYGWIN__)

#ifndef NOMINMAX
    #define NOMINMAX
#endif  // NOMINMAX
#include <windows.h>

#elif defined(__linux__)

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#endif  // defined(__APPLE__) / defined(__CYGWIN__) / defined(__linux__)

namespace {

//! __ulock_wake flag to release every parked waiter (equal to ULF_WAKE_ALL on macOS).
constexpr uint32_t SIMPLE_ULOCK_WAKE_ALL { 0x00000100u };

/**
 * \brief   Parks the calling thread while the futex word equals zero, for up to timeoutMs.
 *          Returns when the word changes, the timeout elapses, or the call is interrupted;
 *          the caller's outer loop re-checks the word. A signaler that flips the word before
 *          this thread actually sleeps is caught by the kernel value-compare (no lost wakeup).
 *
 * \param   word        The futex state word (0 == non-signaled).
 * \param   timeoutMs   Bounded sleep in milliseconds, or areg::WAIT_INFINITE.
 **/
inline void _wait_zero( std::atomic<uint32_t> & word, uint32_t timeoutMs ) noexcept
{
#if defined(__linux__)

    struct timespec   ts {};
    struct timespec * pts { nullptr };
    if (timeoutMs != areg::WAIT_INFINITE)
    {
        ts.tv_sec  = static_cast<time_t>(timeoutMs / 1000u);
        ts.tv_nsec = static_cast<long>(timeoutMs % 1000u) * 1000000L;
        pts = &ts;
    }

    ::syscall(SYS_futex, &word, FUTEX_WAIT_PRIVATE, 0u, pts, nullptr, 0);

#elif defined(__APPLE__)

    uint32_t us { 0u };     // 0 == infinite for __ulock_wait
    if (timeoutMs != areg::WAIT_INFINITE)
    {
        const uint64_t micros { static_cast<uint64_t>(timeoutMs) * 1000u };
        us = static_cast<uint32_t>(std::min<uint64_t>(micros, static_cast<uint64_t>(UINT32_MAX)));
        if (us == 0u)
            us = 1u;        // avoid accidental infinite for sub-millisecond rounding
    }

    ::__ulock_wait(areg::os::APPLE_ULOCK_COMPARE_AND_WAIT, &word, 0u, us);

#else   // Cygwin

    uint32_t expected { 0u };
    const DWORD ms { (timeoutMs == areg::WAIT_INFINITE) ? INFINITE : static_cast<DWORD>(timeoutMs) };
    ::WaitOnAddress(&word, &expected, sizeof(expected), ms);

#endif  // defined(__linux__) / defined(__APPLE__) / Cygwin
}

/**
 * \brief   Wakes one parked waiter (auto-reset) or all parked waiters (manual-reset).
 *
 * \param   word        The futex state word.
 * \param   wakeAll     true => release every parked waiter; false => release a single waiter.
 **/
inline void _wake_waiters( std::atomic<uint32_t> & word, bool wakeAll ) noexcept
{
#if defined(__linux__)

    ::syscall(SYS_futex, &word, FUTEX_WAKE_PRIVATE, wakeAll ? INT_MAX : 1, nullptr, nullptr, 0);

#elif defined(__APPLE__)

    const uint32_t op { areg::os::APPLE_ULOCK_COMPARE_AND_WAIT | (wakeAll ? SIMPLE_ULOCK_WAKE_ALL : 0u) };
    ::__ulock_wake(op, &word, 0u);

#else   // Cygwin

    if (wakeAll)
        ::WakeByAddressAll(&word);
    else
        ::WakeByAddressSingle(&word);

#endif  // defined(__linux__) / defined(__APPLE__) / Cygwin
}

} // anonymous namespace

//////////////////////////////////////////////////////////////////////////
// SimpleEvent class implementation (POSIX)
//////////////////////////////////////////////////////////////////////////

SimpleEvent::SimpleEvent( bool initLock /*= true*/, bool autoReset /*= true*/ ) noexcept
    : mState     ( initLock ? 0u : 1u )
    , mValid     ( true )
    , mAutoReset ( autoReset )
{
}

SimpleEvent::SimpleEvent( areg::NullTag ) noexcept
    : mState     ( 0u )
    , mValid     ( false )
    , mAutoReset ( true )
{
}

SimpleEvent::~SimpleEvent()
{
}

bool SimpleEvent::lock( uint32_t timeout /*= areg::WAIT_INFINITE*/ ) noexcept
{
    if (!mValid)
        return false;

    using Clock = std::chrono::steady_clock;
    const bool             infinite { timeout == areg::WAIT_INFINITE };
    const Clock::time_point deadline { infinite
                                     ? Clock::time_point::max()
                                     : Clock::now() + std::chrono::milliseconds(timeout) };

    for ( ; ; )
    {
        uint32_t state { mState.load(std::memory_order_acquire) };
        if (state != 0u)
        {
            if (!mAutoReset)
                return true;    // manual-reset: stays signaled until reset()

            // auto-reset: consume the signal (1 -> 0). Retry if another waiter won the race.
            if (mState.compare_exchange_weak(state, 0u, std::memory_order_acq_rel, std::memory_order_relaxed))
                return true;

            continue;
        }

        if (timeout == areg::DO_NOT_WAIT)
            return false;

        uint32_t remainMs { areg::WAIT_INFINITE };
        if (!infinite)
        {
            const Clock::time_point now { Clock::now() };
            if (now >= deadline)
                return false;

            const int64_t ms { std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now).count() };
            remainMs = static_cast<uint32_t>(ms <= 0 ? 1 : ms);
        }

        _wait_zero(mState, remainMs);
    }
}

bool SimpleEvent::set_signaled() noexcept
{
    if (!mValid)
        return false;

    const uint32_t prev { mState.exchange(1u, std::memory_order_release) };

    // Only the non-signaled -> signaled edge needs a wake syscall: if it was already
    // signaled, any parked waiter was already released by the edge that set it (or will
    // fail its pre-sleep value-compare). Auto-reset releases one waiter; manual-reset all.
    if (prev == 0u)
        _wake_waiters(mState, !mAutoReset);

    return true;
}

bool SimpleEvent::reset() noexcept
{
    if (!mValid)
        return false;

    mState.store(0u, std::memory_order_release);
    return true;
}

#endif  // defined(_POSIX) || defined(POSIX)
