/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/macos/TimerPosixMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS-specific TimerPosix private methods.
 *              Uses Grand Central Dispatch (GCD) dispatch_source for timers.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/TimerBase.hpp"
#include "areg/base/MemoryDefs.hpp"
#include <dispatch/dispatch.h>

namespace {

    //!< Invalid dispatch source.
    constexpr dispatch_source_t                 INVALID_DISPATCH_SOURCE { nullptr };
    //!< Invalid dispatch queue.
    constexpr dispatch_queue_t                  INVALID_DISPATCH_QUEUE  { nullptr };
    //!< Invalid timer callback.
    constexpr areg::os::FuncPosixTimerRoutine   INVALID_TIMER_CALLBACK  { nullptr };

} // namespace

namespace areg::os {

bool TimerPosix::_create_timer(FuncPosixTimerRoutine funcTimer) noexcept
{
    mTimerCallback = funcTimer;
    mTimerQueue = dispatch_queue_create("areg.component.timer", DISPATCH_QUEUE_SERIAL);
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
}

bool TimerPosix::_start_timer() noexcept
{
    bool result = false;

    if ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mContext != nullptr) && (mTimerCallback != INVALID_TIMER_CALLBACK))
    {
        if (_is_started())
        {
            _stop_timer();
        }

        uint32_t msTimeout  = mContext->timeout();
        uint32_t eventCount = mContext->event_count();

        if ((msTimeout != 0) && (eventCount != 0))
        {
#ifdef DISPATCH_TIMER_STRICT
            mTimerSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, DISPATCH_TIMER_STRICT, mTimerQueue);
#else
            mTimerSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, mTimerQueue);
#endif
            if (mTimerSource != INVALID_DISPATCH_SOURCE)
            {
                uint64_t interval_ns = static_cast<uint64_t>(msTimeout) * NSEC_PER_MSEC;
                uint64_t leeway_ns   = 0;   // strict timing — no leeway

                dispatch_source_set_timer(mTimerSource,
                                          dispatch_time(DISPATCH_TIME_NOW, interval_ns),
                                          (eventCount > 1) ? interval_ns : DISPATCH_TIME_FOREVER,
                                          leeway_ns);

                FuncPosixTimerRoutine callback = mTimerCallback;
                TimerPosix* timerPtr = this;
                dispatch_source_set_event_handler(mTimerSource, ^{
                    if (callback != nullptr)
                    {
                        callback(timerPtr);
                    }
                });

                if (areg::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &mDueTime))
                {
                    areg::os::conv_timeout(mDueTime, msTimeout);
                    result = true;
                    dispatch_resume(mTimerSource);
                }
                else
                {
                    dispatch_source_cancel(mTimerSource);
                    dispatch_release(mTimerSource);
                    mTimerSource = INVALID_DISPATCH_SOURCE;
                }
            }
        }
    }

    return result;
}

void TimerPosix::_stop_timer() noexcept
{
    if (mTimerSource != INVALID_DISPATCH_SOURCE)
    {
        dispatch_source_cancel(mTimerSource);
        dispatch_release(mTimerSource);
        mTimerSource = INVALID_DISPATCH_SOURCE;
    }

    mDueTime.tv_sec  = 0;
    mDueTime.tv_nsec = 0;
}

void TimerPosix::_destroy_timer() noexcept
{
    if (_is_started())
    {
        _stop_timer();
    }

    if (mTimerQueue != INVALID_DISPATCH_QUEUE)
    {
        dispatch_release(mTimerQueue);
        mTimerQueue = INVALID_DISPATCH_QUEUE;
    }

    mTimerCallback = INVALID_TIMER_CALLBACK;
}

} // namespace areg::os

#endif  // __APPLE__
