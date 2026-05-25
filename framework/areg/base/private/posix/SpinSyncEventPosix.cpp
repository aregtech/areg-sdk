/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SpinSyncEventPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, SpinSyncEvent OS sleep/wake primitives — POSIX implementation.
 *              Linux:  futex(FUTEX_WAIT_PRIVATE / FUTEX_WAKE_PRIVATE) — kernel 2.6+.
 *              macOS:  __ulock_wait / __ulock_wake (private but ABI-stable, macOS 10.12+).
 *
 *              The spin phase and sleeper-count logic live in SpinSyncEvent.cpp.
 *              These functions implement only the OS-level sleep and wake, entered after
 *              the spin phase has already run.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/SpinSyncEvent.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <climits>
#include <cstdint>

// --------------------------------------------------------------------------
// macOS: private __ulock API
// --------------------------------------------------------------------------
#if defined(__APPLE__)

#include <cstdint>

extern "C"
{
    int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout_us);
    int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);
}

namespace
{
    constexpr uint32_t UL_COMPARE_AND_WAIT  { 1u };
    constexpr uint32_t ULF_WAKE_ALL         { 0x00000100u };
}  // namespace

// --------------------------------------------------------------------------
// Linux: futex syscall
// --------------------------------------------------------------------------
#else  // Linux

#include <linux/futex.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#endif  // defined(__APPLE__)

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent — POSIX / macOS OS sleep/wake primitives
//////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// macOS implementation
// --------------------------------------------------------------------------
#if defined(__APPLE__)

bool SpinSyncEvent::_os_wait(uint32_t timeout_ms) noexcept
{
    using Clock = std::chrono::steady_clock;
    using Ms    = std::chrono::milliseconds;
    using Us    = std::chrono::microseconds;

    if (timeout_ms == areg::DO_NOT_WAIT)
        return _try_consume();

    const bool  infinite { timeout_ms == areg::WAIT_INFINITE };
    const auto  deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(timeout_ms) };

    for (;;)
    {
        if (_try_consume())
            return true;

        uint32_t timeout_us { 0u };   // 0 = infinite for __ulock_wait
        if (!infinite)
        {
            const auto now       { Clock::now() };
            if (now >= deadline)
                return false;

            const auto remaining { std::chrono::duration_cast<Us>(deadline - now) };
            timeout_us = static_cast<uint32_t>(
                std::min<int64_t>(remaining.count(), static_cast<int64_t>(UINT32_MAX)));

            if (timeout_us == 0u)
                timeout_us = 1u;  // never accidentally convert to "infinite"
        }

        const int rc { __ulock_wait(UL_COMPARE_AND_WAIT, &mState, 0u, timeout_us) };
        if ((rc == 0) || (errno == EAGAIN))
            continue;   // woken or value already changed — re-check via _try_consume
        if (errno == ETIMEDOUT)
            return false;
        // EINTR: interrupted by signal — retry
    }
}

void SpinSyncEvent::_os_wake() noexcept
{
    if (mAutoReset)
        __ulock_wake(UL_COMPARE_AND_WAIT, &mState, 0u);
    else
        __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, &mState, 0u);
}

void SpinSyncEvent::_os_wake_all() noexcept
{
    __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, &mState, 0u);
}

bool SpinSyncEvent::_os_wait_on(std::atomic<uint32_t>* word, uint32_t timeout_ms) noexcept
{
    using Clock = std::chrono::steady_clock;
    using Ms    = std::chrono::milliseconds;
    using Us    = std::chrono::microseconds;

    if (timeout_ms == areg::DO_NOT_WAIT)
        return (word->load(std::memory_order_acquire) != 0u);

    const bool  infinite { timeout_ms == areg::WAIT_INFINITE };
    const auto  deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(timeout_ms) };

    for (;;)
    {
        if (word->load(std::memory_order_acquire) != 0u)
            return true;

        uint32_t timeout_us { 0u };
        if (!infinite)
        {
            const auto now       { Clock::now() };
            if (now >= deadline)
                return false;

            const auto remaining { std::chrono::duration_cast<Us>(deadline - now) };
            timeout_us = static_cast<uint32_t>(
                std::min<int64_t>(remaining.count(), static_cast<int64_t>(UINT32_MAX)));

            if (timeout_us == 0u)
                timeout_us = 1u;
        }

        const int rc { __ulock_wait(UL_COMPARE_AND_WAIT, word, 0u, timeout_us) };
        if ((rc == 0) || (errno == EAGAIN))
            continue;
        if (errno == ETIMEDOUT)
            return false;
    }
}

void SpinSyncEvent::_os_wake_on(std::atomic<uint32_t>* word) noexcept
{
    __ulock_wake(UL_COMPARE_AND_WAIT | ULF_WAKE_ALL, word, 0u);
}

// --------------------------------------------------------------------------
// Linux futex implementation
// --------------------------------------------------------------------------
#else  // !defined(__APPLE__)

namespace
{
    inline int _futex(void* addr, int op, uint32_t val, const struct timespec* timeout) noexcept
    {
        return static_cast<int>(syscall(SYS_futex, addr, op, val, timeout, nullptr, 0u));
    }
}  // namespace

bool SpinSyncEvent::_os_wait(uint32_t timeout_ms) noexcept
{
    using Clock = std::chrono::steady_clock;
    using Ms    = std::chrono::milliseconds;
    using Ns    = std::chrono::nanoseconds;

    if (timeout_ms == areg::DO_NOT_WAIT)
        return _try_consume();

    const bool  infinite { timeout_ms == areg::WAIT_INFINITE };
    const auto  deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(timeout_ms) };

    for (;;)
    {
        if (_try_consume())
            return true;

        struct timespec         ts  { };
        const struct timespec*  pts { nullptr };
        if (!infinite)
        {
            const auto now       { Clock::now() };
            if (now >= deadline)
                return false;

            const auto remaining { std::chrono::duration_cast<Ns>(deadline - now) };
            ts.tv_sec  = static_cast<time_t>(remaining.count() / 1'000'000'000LL);
            ts.tv_nsec = static_cast<long>  (remaining.count() % 1'000'000'000LL);
            pts = &ts;
        }

        // FUTEX_WAIT_PRIVATE: sleep while *addr == 0.
        // Returns EAGAIN immediately if *addr != 0 at call time.
        const int rc { _futex(&mState, FUTEX_WAIT_PRIVATE, 0u, pts) };
        if ((rc == 0) || (errno == EAGAIN))
            continue;   // woken or value changed — re-check
        if (errno == ETIMEDOUT)
            return false;
        // EINTR: retry
    }
}

void SpinSyncEvent::_os_wake() noexcept
{
    const int count { mAutoReset ? 1 : INT_MAX };
    _futex(&mState, FUTEX_WAKE_PRIVATE, static_cast<uint32_t>(count), nullptr);
}

void SpinSyncEvent::_os_wake_all() noexcept
{
    _futex(&mState, FUTEX_WAKE_PRIVATE, static_cast<uint32_t>(INT_MAX), nullptr);
}

bool SpinSyncEvent::_os_wait_on(std::atomic<uint32_t>* word, uint32_t timeout_ms) noexcept
{
    using Clock = std::chrono::steady_clock;
    using Ms    = std::chrono::milliseconds;
    using Ns    = std::chrono::nanoseconds;

    if (timeout_ms == areg::DO_NOT_WAIT)
        return (word->load(std::memory_order_acquire) != 0u);

    const bool  infinite { timeout_ms == areg::WAIT_INFINITE };
    const auto  deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(timeout_ms) };

    for (;;)
    {
        if (word->load(std::memory_order_acquire) != 0u)
            return true;

        struct timespec         ts  { };
        const struct timespec*  pts { nullptr };
        if (!infinite)
        {
            const auto now       { Clock::now() };
            if (now >= deadline)
                return false;

            const auto remaining { std::chrono::duration_cast<Ns>(deadline - now) };
            ts.tv_sec  = static_cast<time_t>(remaining.count() / 1'000'000'000LL);
            ts.tv_nsec = static_cast<long>  (remaining.count() % 1'000'000'000LL);
            pts = &ts;
        }

        const int rc { _futex(word, FUTEX_WAIT_PRIVATE, 0u, pts) };
        if ((rc == 0) || (errno == EAGAIN))
            continue;
        if (errno == ETIMEDOUT)
            return false;
    }
}

void SpinSyncEvent::_os_wake_on(std::atomic<uint32_t>* word) noexcept
{
    _futex(word, FUTEX_WAKE_PRIVATE, static_cast<uint32_t>(INT_MAX), nullptr);
}

#endif  // defined(__APPLE__)

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
