/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/win32/SimpleEventWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lightweight single-object event - Windows implementation.
 ************************************************************************/
/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/SimpleEvent.hpp"

#ifdef  _WIN32

// WaitOnAddress and WakeByAddress are declared for Windows 8 and newer only. The rest of the
// framework targets Windows 7, so the level is raised for this translation unit alone.
#undef  _WIN32_WINNT
#define _WIN32_WINNT    0x0602

#ifndef NOMINMAX
    #define NOMINMAX
#endif  // NOMINMAX
#include <Windows.h>

#include <chrono>


namespace {

/**
 * \brief   Parks the calling thread while the wait word equals zero, for up to timeoutMs.
 *          Returns when the word changes, the timeout elapses, or the call is interrupted;
 *          the caller's outer loop re-checks the word. A signaler that flips the word before
 *          this thread actually sleeps is caught by the kernel value-compare (no lost wakeup).
 *
 * \param   word        The wait state word (0 == non-signaled).
 * \param   timeoutMs   Bounded sleep in milliseconds, or areg::WAIT_INFINITE.
 **/
inline void _wait_zero( std::atomic<uint32_t> & word, uint32_t timeoutMs ) noexcept
{
    uint32_t expected { 0u };
    const DWORD ms { (timeoutMs == areg::WAIT_INFINITE) ? INFINITE : static_cast<DWORD>(timeoutMs) };
    ::WaitOnAddress(&word, &expected, sizeof(expected), ms);
}

/**
 * \brief   Wakes one parked waiter (auto-reset) or all parked waiters (manual-reset).
 *
 * \param   word        The wait state word.
 * \param   wakeAll     true => release every parked waiter; false => release a single waiter.
 **/
inline void _wake_waiters( std::atomic<uint32_t> & word, bool wakeAll ) noexcept
{
    if (wakeAll)
        ::WakeByAddressAll(&word);
    else
        ::WakeByAddressSingle(&word);
}

} // anonymous namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SimpleEvent class implementation (Windows)
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
    const Clock::time_point deadline { infinite ? Clock::time_point::max() : Clock::now() + std::chrono::milliseconds(timeout) };

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

    // Only the non-signaled
    if (mState.exchange(1u, std::memory_order_release) == 0u)
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

} // namespace areg

#endif  // _WIN32
