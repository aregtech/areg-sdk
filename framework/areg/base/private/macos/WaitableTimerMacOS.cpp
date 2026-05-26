/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/macos/WaitableTimerMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS Waitable Timer implementation.
 *              Uses GCD dispatch_source for timer delivery.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/base/private/posix/WaitableTimerPosix.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include "areg/base/Thread.hpp"

namespace areg::os {

WaitableTimerPosix::WaitableTimerPosix(bool is_auto_reset /*= false*/)
    : WaitablePosix ( areg::os::SyncKind::SoWaitTimer )
    , mObjectLock   ( areg::os::SyncKind::SoMutex, false )
    , mResetInfo    ( is_auto_reset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
    , mTimerSource  ( nullptr )
    , mTimerQueue   ( nullptr )
    , mTimeout      ( 0 )
    , mIsSignaled   ( false )
    , mFiredCount   ( 0 )
    , mDueTime      ( {0, 0} )
    , mThreadId(Thread::INVALID_THREAD_ID)
{
    mTimerQueue = dispatch_queue_create("areg.waitable.timer", DISPATCH_QUEUE_SERIAL);
}

bool WaitableTimerPosix::set_timer(uint32_t msTimeout, bool is_periodic)
{
    bool result = false;
    ObjectLockPosix lock(mObjectLock);

    _stop_timer();

    if ((mTimerQueue != nullptr) && (msTimeout != 0))
    {
        mTimerSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, mTimerQueue);
        if (mTimerSource != nullptr)
        {
            const uint64_t interval_ns = static_cast<uint64_t>(msTimeout) * NSEC_PER_MSEC;

            dispatch_source_set_timer(mTimerSource,
                                      dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(interval_ns)),
                                      is_periodic ? interval_ns : DISPATCH_TIME_FOREVER,
                                      0);  // leeway = 0: maximum precision

            dispatch_source_set_event_handler(mTimerSource, ^{
                this->_timer_expired();
            });

            areg::os::conv_timeout(mDueTime, msTimeout);
            mTimeout    = msTimeout;
            mIsSignaled = false;
            mThreadId   = Thread::current_thread_id();
            result      = true;

            dispatch_resume(mTimerSource);
        }
    }

    return result;
}

bool WaitableTimerPosix::is_valid() const noexcept
{
    ObjectLockPosix lock(*this);
    return (mTimerQueue != nullptr);
}

void WaitableTimerPosix::_reset_timer() noexcept
{
    _stop_timer();

    if (mTimerQueue != nullptr)
    {
        dispatch_release(mTimerQueue);
        mTimerQueue = nullptr;
    }

    mThreadId = 0;
}

void WaitableTimerPosix::_stop_timer() noexcept
{
    if (mTimerSource != nullptr)
    {
        dispatch_source_cancel(mTimerSource);
        dispatch_release(mTimerSource);
        mTimerSource = nullptr;
    }

    mDueTime.tv_sec  = 0;
    mDueTime.tv_nsec = 0;
    mTimeout         = 0;
    mFiredCount      = 0;
    mThreadId        = 0;
}

void WaitableTimerPosix::_timer_expired() noexcept
{
    bool sendSignal = false;

    do
    {
        ObjectLockPosix lock(mObjectLock);

        if (mTimerSource != nullptr)
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

#endif  // __APPLE__
