/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableTimerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Timer implementation.
 *              Uses POSIX timer_create() + SIGEV_THREAD for all non-Apple POSIX
 *              platforms (Linux, Cygwin, FreeBSD, etc.).
 *              macOS uses GCD dispatch_source -- see macos/WaitableTimerMacOS.cpp.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/WaitableTimerPosix.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

//////////////////////////////////////////////////////////////////////////
// Non-Apple POSIX: constructor and timer operations
// using POSIX timer_create + SIGEV_THREAD (Linux, Cygwin, FreeBSD, etc.)
//////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Thread.hpp"
#include <signal.h>

namespace areg::os {

void WaitableTimerPosix::_posix_timer_routine(signal_value si)
{
    WaitableTimerPosix * timer = reinterpret_cast<WaitableTimerPosix *>(si.sival_ptr);
    if (timer != nullptr)
    {
        timer->_timer_expired();
    }
}

WaitableTimerPosix::WaitableTimerPosix(bool is_auto_reset /*= false*/)
    : WaitablePosix  ( areg::os::SyncKind::SoWaitTimer )
    , mObjectLock    ( areg::os::SyncKind::SoMutex, false )
    , mResetInfo     ( is_auto_reset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
    , mTimerId       ( static_cast<timer_t>(0) )
    , mTimeout       ( 0 )
    , mIsSignaled    ( false )
    , mFiredCount    ( 0 )
    , mDueTime       ( {0, 0} )
    , mThreadId      ( Thread::INVALID_THREAD_ID )
{
    struct sigevent sigEvent;
    areg::mem_zero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify            = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr   = static_cast<void *>(this);
    sigEvent.sigev_notify_function   = &WaitableTimerPosix::_posix_timer_routine;
    sigEvent.sigev_notify_attributes = nullptr;

    if (areg::RETURNED_OK != ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId))
    {
        mTimerId = static_cast<timer_t>(0);
    }
}

bool WaitableTimerPosix::set_timer(uint32_t msTimeout, bool is_periodic)
{
    bool result = false;
    ObjectLockPosix lock(mObjectLock);

    _stop_timer();

    if ((mTimerId != static_cast<timer_t>(0)) && (msTimeout != 0))
    {
        struct itimerspec interval;
        areg::mem_zero(static_cast<void *>(&interval), sizeof(struct itimerspec));
        areg::os::conv_timeout(interval.it_value, msTimeout);
        if (is_periodic)
        {
            interval.it_interval.tv_sec  = interval.it_value.tv_sec;
            interval.it_interval.tv_nsec = interval.it_value.tv_nsec;
        }

        mDueTime.tv_sec  = interval.it_value.tv_sec;
        mDueTime.tv_nsec = interval.it_value.tv_nsec;
        mTimeout         = msTimeout;
        mIsSignaled      = false;
        mThreadId        = Thread::current_thread_id();
        result           = true;

        if (areg::RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr))
        {
            result = false;
            _reset_timer();
        }
    }

    return result;
}

bool WaitableTimerPosix::is_valid() const noexcept
{
    ObjectLockPosix lock(mObjectLock);
    return (mTimerId != static_cast<timer_t>(0));
}

void WaitableTimerPosix::_reset_timer() noexcept
{
    _stop_timer();

    if (mTimerId != static_cast<timer_t>(0))
    {
        ::timer_delete(mTimerId);
    }

    mTimerId  = static_cast<timer_t>(0);
    mThreadId = 0;
}

void WaitableTimerPosix::_stop_timer() noexcept
{
    if ((mTimerId != static_cast<timer_t>(0)) && (mTimeout != 0))
    {
        mDueTime.tv_sec  = 0;
        mDueTime.tv_nsec = 0;
        itimerspec cancelSpec;
        areg::mem_zero(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        ::timer_settime(mTimerId, 0, &cancelSpec, nullptr);
    }

    mTimeout    = 0;
    mFiredCount = 0;
    mThreadId   = 0;
}

void WaitableTimerPosix::_timer_expired() noexcept
{
    bool sendSignal = false;

    do
    {
        ObjectLockPosix lock(mObjectLock);

        if (mTimerId != static_cast<timer_t>(0))
        {
            ++mFiredCount;
            areg::os::conv_timeout(mDueTime, mTimeout);
            mIsSignaled = true;
            sendSignal  = true;
        }

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
        SyncLockAndWaitPosix::notify_async_signal(mThreadId);
    }
}

} // namespace areg::os

#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// Common POSIX: shared methods for all POSIX platforms
//////////////////////////////////////////////////////////////////////////

namespace areg::os {

WaitableTimerPosix::~WaitableTimerPosix()
{
    _reset_timer();
}

bool WaitableTimerPosix::stop_timer() noexcept
{
    bool sendSignal = false;
    do
    {
        ObjectLockPosix lock(mObjectLock);
        sendSignal  = (mIsSignaled == false);
        _stop_timer();
        mIsSignaled = true;
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return true;
}

bool WaitableTimerPosix::cancel_timer() noexcept
{
    bool sendSignal = false;
    do
    {
        ObjectLockPosix lock(mObjectLock);
        sendSignal  = (mIsSignaled == false);
        _reset_timer();
        mIsSignaled = true;
    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return true;
}

bool WaitableTimerPosix::check_signaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(mObjectLock);
    return mIsSignaled;
}

bool WaitableTimerPosix::notify_request_ownership(pthread_t /* ownerThread */)
{
    return true;
}

bool WaitableTimerPosix::can_signal_threads() const noexcept
{
    return true;
}

void WaitableTimerPosix::notify_released_threads(int32_t /* numThreads */)
{
    ObjectLockPosix lock(mObjectLock);
    if (mResetInfo == areg::os::ResetMode::Automatic)
    {
        mIsSignaled = false;
    }
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
