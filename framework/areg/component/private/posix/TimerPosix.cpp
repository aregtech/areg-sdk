/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/posix/TimerPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/TimerBase.hpp"
#include "areg/component/private/Watchdog.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/MemoryDefs.hpp"

#ifndef __APPLE__
    #include <signal.h>
#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// TimerPosix class implementation
//////////////////////////////////////////////////////////////////////////

namespace {
#ifdef __APPLE__

    //!< Invalid dispatch source.
    constexpr dispatch_source_t     INVALID_DISPATCH_SOURCE     { nullptr };
    //!< Invalid dispatch queue.
    constexpr dispatch_queue_t      INVALID_DISPATCH_QUEUE      { nullptr };
    //!< Invalid timer callback.
    constexpr FuncPosixTimerRoutine INVALID_TIMER_CALLBACK      { nullptr };

#else   // !__APPLE__

    //!< Timer invalid ID
    const timer_t	INVALID_POSIX_TIMER_ID  { reinterpret_cast<timer_t>(~0ul) };

#endif  // __APPLE__
} // namespace

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// TimerPosix methods
//////////////////////////////////////////////////////////////////////////

TimerPosix::TimerPosix()
#ifdef __APPLE__
    : mTimerSource  ( INVALID_DISPATCH_SOURCE )
    , mTimerQueue   ( INVALID_DISPATCH_QUEUE  )
    , mTimerCallback( INVALID_TIMER_CALLBACK  )
#else   // !__APPLE__
    : mTimerId      ( INVALID_POSIX_TIMER_ID  )
#endif  // __APPLE__
    , mContext      ( nullptr   )
    , mContextId    ( 0u        )
    , mDueTime      (           )
    , mLock         (           )
{
}

TimerPosix::~TimerPosix()
{
    SpinAutolockPosix lock(mLock);
    _destroy_timer();
}

bool TimerPosix::create_timer( FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockPosix lock(mLock);
#ifdef __APPLE__
    return ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mTimerCallback != INVALID_TIMER_CALLBACK)) ||
           ((funcTimer != INVALID_TIMER_CALLBACK) && _create_timer(funcTimer));
#else   // !__APPLE__
    return ((mTimerId != INVALID_POSIX_TIMER_ID) || ((funcTimer != nullptr) && _create_timer(funcTimer)));
#endif  // __APPLE__
}

bool TimerPosix::start_timer( TimerBase & context, id_type contextId, FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockPosix lock(mLock);

    mContext    = &context;
    mContextId  = contextId;

#ifdef __APPLE__
	if ((mTimerQueue == INVALID_DISPATCH_QUEUE) && (funcTimer != INVALID_TIMER_CALLBACK))
	{
	    _create_timer(funcTimer);
	}

	return ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mTimerCallback != INVALID_TIMER_CALLBACK) && _start_timer());
#else   // !__APPLE__
	if ((mTimerId == INVALID_POSIX_TIMER_ID) && (funcTimer != nullptr))
	{
	    _create_timer(funcTimer);
	}

	return ((mTimerId != INVALID_POSIX_TIMER_ID) && _start_timer());
#endif  // __APPLE__
}

bool TimerPosix::restart_timer()
{
	SpinAutolockPosix lock(mLock);
    return _start_timer();
}

bool TimerPosix::pause_timer()
{
	SpinAutolockPosix lock(mLock);

    if ( _is_started() )
    {
        _stop_timer();
    }

#ifdef __APPLE__
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    return (mTimerId != INVALID_POSIX_TIMER_ID);
#endif  // __APPLE__
}

bool TimerPosix::stop_timer()
{
	SpinAutolockPosix lock(mLock);

    if ( _is_started() )
    {
        _stop_timer();
    }

#ifdef __APPLE__
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    return (mTimerId != INVALID_POSIX_TIMER_ID);
#endif  // __APPLE__
}

void TimerPosix::destroy_timer()
{
	SpinAutolockPosix lock(mLock);

    _destroy_timer();

    mContext    = nullptr;
    mContextId = 0u;
}

void TimerPosix::timer_expired()
{
    SpinAutolockPosix lock(mLock);
    if (mContext != nullptr)
    {
        if (mContext->event_count() > TimerBase::ONE_TIME)
        {
            areg::os::conv_timeout(mDueTime, mContext->timeout());
        }
        else if (_is_started())
        {
            _stop_timer();
        }
    }
}

bool TimerPosix::_create_timer( FuncPosixTimerRoutine funcTimer )
{
#ifdef __APPLE__
    mTimerCallback = funcTimer;
    mTimerQueue = dispatch_queue_create("areg.component.timer", DISPATCH_QUEUE_SERIAL);
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    struct sigevent sigEvent;
    areg::mem_zero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));

    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = funcTimer;
    sigEvent.sigev_notify_attributes= nullptr;

    return (areg::RETURNED_OK == ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId));
#endif  // __APPLE__
}

inline bool TimerPosix::_start_timer()
{
    bool result = false;

#ifdef __APPLE__
    if ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mContext != nullptr) && (mTimerCallback != INVALID_TIMER_CALLBACK))
    {
        if (_is_started())
        {
            _stop_timer();
        }

        uint32_t msTimeout = mContext->timeout();
        uint32_t eventCount= mContext->event_count();

        if ((msTimeout != 0) && (eventCount != 0))
        {
            mTimerSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, mTimerQueue);
            if (mTimerSource != INVALID_DISPATCH_SOURCE)
            {
                uint64_t interval_ns = static_cast<uint64_t>(msTimeout) * NSEC_PER_MSEC;
                uint64_t leeway_ns   = NSEC_PER_MSEC; // 1ms leeway

                dispatch_source_set_timer(mTimerSource,
                                          dispatch_time(DISPATCH_TIME_NOW, interval_ns),
                                          (eventCount > 1) ? interval_ns : DISPATCH_TIME_FOREVER,
                                          leeway_ns);

                // Capture the callback and 'this' pointer to call when timer fires.
                // This allows TimerManager and WatchdogManager to process the expired timer.
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
#else   // !__APPLE__
    if ((mTimerId != INVALID_POSIX_TIMER_ID) && (mContext != nullptr))
    {
        if (_is_started())
        {
            _stop_timer();
        }


        uint32_t msTimeout = mContext->timeout();
        uint32_t eventCount= mContext->event_count();

        if ((msTimeout != 0) && (eventCount != 0))
        {
            struct itimerspec interval;
            areg::mem_zero(static_cast<void *>(&interval), sizeof(struct itimerspec));
            areg::os::conv_timeout(interval.it_value, msTimeout);
            if (eventCount > 1)
            {
                interval.it_interval.tv_sec = interval.it_value.tv_sec;
                interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
            }

            if (areg::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &mDueTime))
            {
                areg::os::conv_timeout(mDueTime, msTimeout);
                result = true;

                if (areg::RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr))
                {
                    result          = false;
                    mDueTime.tv_sec = 0;
                    mDueTime.tv_nsec= 0;
                    mContextId 		= 0;
                }
            }
        }
    }
#endif  // __APPLE__

    return result;
}

void TimerPosix::_stop_timer()
{
#ifdef __APPLE__
    if (mTimerSource != INVALID_DISPATCH_SOURCE)
    {
        dispatch_source_cancel(mTimerSource);
        dispatch_release(mTimerSource);
        mTimerSource = INVALID_DISPATCH_SOURCE;
    }

    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
#else   // !__APPLE__
    ASSERT(mTimerId != INVALID_POSIX_TIMER_ID);

    struct itimerspec interval;
    areg::mem_zero(static_cast<void *>(&interval), sizeof(struct itimerspec));

    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
    ::timer_settime(mTimerId, 0, &interval, nullptr);
#endif  // __APPLE__
}

void TimerPosix::_destroy_timer()
{
    if (_is_started())
    {
        _stop_timer();
    }

#ifdef __APPLE__
    if (mTimerQueue != INVALID_DISPATCH_QUEUE)
    {
        dispatch_release(mTimerQueue);
        mTimerQueue = INVALID_DISPATCH_QUEUE;
    }

    mTimerCallback = INVALID_TIMER_CALLBACK;
#else   // !__APPLE__
    if (mTimerId != INVALID_POSIX_TIMER_ID)
    {
        ::timer_delete(mTimerId);
        mTimerId    = INVALID_POSIX_TIMER_ID;
    }
#endif  // __APPLE__
}

} // namespace areg::os

#endif // defined(_POSIX) || defined(POSIX)
