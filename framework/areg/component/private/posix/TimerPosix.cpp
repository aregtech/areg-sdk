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
#include "areg/base/private/posix/NESyncTypesIX.hpp"
#include "areg/base/NEMemory.hpp"

#ifndef __APPLE__
    #include <signal.h>
#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// TimerPosix class implementation
//////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
namespace
{
    //!< Invalid dispatch source.
    constexpr dispatch_source_t     INVALID_DISPATCH_SOURCE     { nullptr };
    //!< Invalid dispatch queue.
    constexpr dispatch_queue_t      INVALID_DISPATCH_QUEUE      { nullptr };
    //!< Invalid timer callback.
    constexpr FuncPosixTimerRoutine INVALID_TIMER_CALLBACK      { nullptr };
}
#else   // !__APPLE__
namespace
{
    //!< Timer invalid ID
    const timer_t	INVALID_POSIX_TIMER_ID  { reinterpret_cast<timer_t>(~0ul) };
}
#endif  // __APPLE__

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
    _destroyTimer();
}

bool TimerPosix::createTimer( FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockPosix lock(mLock);
#ifdef __APPLE__
    return ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mTimerCallback != INVALID_TIMER_CALLBACK)) ||
           ((funcTimer != INVALID_TIMER_CALLBACK) && _createTimer(funcTimer));
#else   // !__APPLE__
    return ((mTimerId != INVALID_POSIX_TIMER_ID) || ((funcTimer != nullptr) && _createTimer(funcTimer)));
#endif  // __APPLE__
}

bool TimerPosix::startTimer( TimerBase & context, id_type contextId, FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockPosix lock(mLock);

    mContext    = &context;
    mContextId  = contextId;

#ifdef __APPLE__
	if ((mTimerQueue == INVALID_DISPATCH_QUEUE) && (funcTimer != INVALID_TIMER_CALLBACK))
	{
	    _createTimer(funcTimer);
	}

	return ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mTimerCallback != INVALID_TIMER_CALLBACK) && _startTimer());
#else   // !__APPLE__
	if ((mTimerId == INVALID_POSIX_TIMER_ID) && (funcTimer != nullptr))
	{
	    _createTimer(funcTimer);
	}

	return ((mTimerId != INVALID_POSIX_TIMER_ID) && _startTimer());
#endif  // __APPLE__
}

bool TimerPosix::restartTimer()
{
	SpinAutolockPosix lock(mLock);
    return _startTimer();
}

bool TimerPosix::pauseTimer()
{
	SpinAutolockPosix lock(mLock);

    if ( _isStarted() )
    {
        _stopTimer();
    }

#ifdef __APPLE__
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    return (mTimerId != INVALID_POSIX_TIMER_ID);
#endif  // __APPLE__
}

bool TimerPosix::stopTimer()
{
	SpinAutolockPosix lock(mLock);

    if ( _isStarted() )
    {
        _stopTimer();
    }

#ifdef __APPLE__
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    return (mTimerId != INVALID_POSIX_TIMER_ID);
#endif  // __APPLE__
}

void TimerPosix::destroyTimer()
{
	SpinAutolockPosix lock(mLock);

    _destroyTimer();

    mContext    = nullptr;
    mContextId = 0u;
}

void TimerPosix::timerExpired()
{
    SpinAutolockPosix lock(mLock);
    if (mContext != nullptr)
    {
        if (mContext->getEventCount() > TimerBase::ONE_TIME)
        {
            NESyncTypesIX::convTimeout(mDueTime, mContext->getTimeout());
        }
        else if (_isStarted())
        {
            _stopTimer();
        }
    }
}

bool TimerPosix::_createTimer( FuncPosixTimerRoutine funcTimer )
{
#ifdef __APPLE__
    mTimerCallback = funcTimer;
    mTimerQueue = dispatch_queue_create("areg.component.timer", DISPATCH_QUEUE_SERIAL);
    return (mTimerQueue != INVALID_DISPATCH_QUEUE);
#else   // !__APPLE__
    struct sigevent sigEvent;
    NEMemory::memZero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));

    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = funcTimer;
    sigEvent.sigev_notify_attributes= nullptr;

    return (NECommon::RETURNED_OK == ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId));
#endif  // __APPLE__
}

inline bool TimerPosix::_startTimer()
{
    bool result = false;

#ifdef __APPLE__
    if ((mTimerQueue != INVALID_DISPATCH_QUEUE) && (mContext != nullptr) && (mTimerCallback != INVALID_TIMER_CALLBACK))
    {
        if (_isStarted())
        {
            _stopTimer();
        }

        unsigned int msTimeout = mContext->getTimeout();
        unsigned int eventCount= mContext->getEventCount();

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

                if (NECommon::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &mDueTime))
                {
                    NESyncTypesIX::convTimeout(mDueTime, msTimeout);
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
        if (_isStarted())
        {
            _stopTimer();
        }


        unsigned int msTimeout = mContext->getTimeout();
        unsigned int eventCount= mContext->getEventCount();

        if ((msTimeout != 0) && (eventCount != 0))
        {
            struct itimerspec interval;
            NEMemory::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));
            NESyncTypesIX::convTimeout(interval.it_value, msTimeout);
            if (eventCount > 1)
            {
                interval.it_interval.tv_sec = interval.it_value.tv_sec;
                interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
            }

            if (NECommon::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &mDueTime))
            {
                NESyncTypesIX::convTimeout(mDueTime, msTimeout);
                result = true;

                if (NECommon::RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr))
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

void TimerPosix::_stopTimer()
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
    NEMemory::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));

    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
    ::timer_settime(mTimerId, 0, &interval, nullptr);
#endif  // __APPLE__
}

void TimerPosix::_destroyTimer()
{
    if (_isStarted())
    {
        _stopTimer();
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

#endif // defined(_POSIX) || defined(POSIX)
