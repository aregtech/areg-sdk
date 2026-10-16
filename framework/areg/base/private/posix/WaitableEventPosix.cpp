/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/posix/WaitableEventPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Event class.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitableEventPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitableEventPosix class implementation
//////////////////////////////////////////////////////////////////////////

WaitableEventPosix::WaitableEventPosix( bool isInitSignaled, bool isAutoReset )
    : WaitablePosix  ( areg::os::SyncKind::SoWaitEvent )
    , mEventReset    ( isAutoReset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
    , mIsSignaled    ( isInitSignaled )
{
}

bool WaitableEventPosix::check_signaled(pthread_t /*contextThread*/) const
{
    return mIsSignaled.load(std::memory_order_acquire);
}

bool WaitableEventPosix::notify_request_ownership(pthread_t /* ownerThread */)
{
    if (mEventReset == areg::os::ResetMode::Automatic)
    {
        // Atomically take ownership
        bool expected{ true };
        return mIsSignaled.compare_exchange_strong(expected, false, std::memory_order_acq_rel, std::memory_order_relaxed);
    }

    return true;
}

bool WaitableEventPosix::can_signal_threads() const noexcept
{
    return true;
}

void WaitableEventPosix::notify_released_threads(int32_t numThreads)
{
    // Auto-reset is handled atomically in notify_request_ownership(), no need to reset here.
    AREG_OUTPUT_DBG("There were [ %d ] released threads from waitable event [ %p ].", numThreads, this);
}

bool WaitableEventPosix::set_signaled() noexcept
{
    if (!is_valid())
        return false;

    // Notify on the false->true edge, AND whenever a waiter is parked even if already signaled.
    bool notify{ !mIsSignaled.exchange(true, std::memory_order_acq_rel) };
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    notify = notify || has_waiters();
#endif
    if (notify)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return true;
}

void WaitableEventPosix::pulse_event() noexcept
{
    if (!is_valid())
        return;

    bool expected{ false };
    if (mIsSignaled.compare_exchange_strong(expected, true, std::memory_order_acq_rel, std::memory_order_relaxed))
    {
        AREG_OUTPUT_DBG("Pulsing event [ %s ]", name().as_string());
        SyncLockAndWaitPosix::event_signaled(*this);
        mIsSignaled.store(false, std::memory_order_release);
    }
}

} // namespace areg::os
#endif  // defined(_POSIX) || defined(POSIX)
