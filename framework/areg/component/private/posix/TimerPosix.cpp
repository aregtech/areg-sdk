/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerPosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/posix/TimerPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <signal.h>

#include "areg/base/private/posix/NESynchTypesIX.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEMemory.hpp"

//////////////////////////////////////////////////////////////////////////
// TimerPosix class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Timer invalid ID
    const timer_t	INVALID_POSIX_TIMER_ID	= reinterpret_cast<timer_t>(~0);
}

//////////////////////////////////////////////////////////////////////////
// TimerPosix methods
//////////////////////////////////////////////////////////////////////////

TimerPosix::TimerPosix( void )
    : mTimerId      ( INVALID_POSIX_TIMER_ID  )
    , mContext      ( nullptr  )
    , mEventCount   (       )
    , mDueTime      (       )
    , mThreadId     ( Thread::INVALID_THREAD_ID )
    , mLock         ( )
{
}

TimerPosix::~TimerPosix(void)
{
    SpinAutolockIX lock(mLock);
    _destroyTimer();
}

bool TimerPosix::createTimer( FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockIX lock(mLock);

    bool result = false;

    if (mTimerId != INVALID_POSIX_TIMER_ID)
    {
        _destroyTimer();
    }

    if (funcTimer != nullptr)
    {
        result = _createTimer(funcTimer);
    }

    return result;
}

bool TimerPosix::startTimer( Timer & context, FuncPosixTimerRoutine funcTimer )
{
	SpinAutolockIX lock(mLock);

    if (funcTimer != nullptr)
    {
        if (INVALID_POSIX_TIMER_ID != 0)
        {
            _destroyTimer();
        }

        _createTimer(funcTimer);
    }

    return startTimer(context);
}

bool TimerPosix::startTimer( Timer & context )
{
	SpinAutolockIX lock(mLock);

    return _startTimer(&context);
}


bool TimerPosix::startTimer( void )
{
	SpinAutolockIX lock(mLock);

    return _startTimer(nullptr);
}

bool TimerPosix::pauseTimer(void)
{
	SpinAutolockIX lock(mLock);
    
    if ( _isStarted() )
    {
        _stopTimer();
    }

    return (mTimerId != INVALID_POSIX_TIMER_ID);
}

bool TimerPosix::stopTimer(void)
{
	SpinAutolockIX lock(mLock);

    if ( _isStarted() )
    {
        _stopTimer();
    }

    return (mTimerId != INVALID_POSIX_TIMER_ID);
}

void TimerPosix::destroyTimer(void)
{
	SpinAutolockIX lock(mLock);

    _destroyTimer();

    mContext    = nullptr;
    mEventCount = 0;
}

void TimerPosix::timerExpired(void)
{
	SpinAutolockIX lock(mLock);

    if ((mTimerId != INVALID_POSIX_TIMER_ID) && (mEventCount != 0))
    {
        if ((mEventCount != Timer::CONTINUOUSLY) && (-- mEventCount == 0) )
        {
            ASSERT(_isStarted());
            _stopTimer();
        }
        else
        {
            NESynchTypesIX::convTimeout(mDueTime, mContext->getFireTime());
        }
    }
}

bool TimerPosix::_createTimer( FuncPosixTimerRoutine funcTimer )
{
    struct sigevent sigEvent;
    NEMemory::memZero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));

    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = funcTimer;
    sigEvent.sigev_notify_attributes= nullptr;

    return (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId));
}

inline bool TimerPosix::_startTimer( Timer * context )
{
    bool result = false;
    if (context != nullptr)
    {
    	mContext    = context;
    }

    if ((mTimerId != INVALID_POSIX_TIMER_ID) && (mContext != nullptr))
    {
        if (_isStarted())
        {
            _stopTimer();
        }

        unsigned int msTimeout = mContext->getFireTime();
        unsigned int eventCount= mContext->getEventCount();

        if ((msTimeout != 0) && (eventCount != 0))
        {
            result = _startTimer(msTimeout, eventCount);
        }
    }

    return result;
}

bool TimerPosix::_startTimer(unsigned int msTimeout, unsigned int eventCount)
{
    bool result = true;

    struct itimerspec interval;
    NEMemory::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));
    NESynchTypesIX::convTimeout(interval.it_value, msTimeout);
    if (eventCount > 1)
    {
        interval.it_interval.tv_sec = interval.it_value.tv_sec;
        interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
    }

    clock_gettime(CLOCK_REALTIME, &mDueTime);
    NESynchTypesIX::convTimeout(mDueTime, msTimeout);

    mEventCount = eventCount;
    mThreadId   = Thread::getCurrentThreadId();
    if (RETURNED_OK != timer_settime(mTimerId, 0, &interval, nullptr))
    {
        result          = false;
        mThreadId       = 0;
        mEventCount     = 0;
        mDueTime.tv_sec = 0;
        mDueTime.tv_nsec= 0;
    }

    return result;
}

void TimerPosix::_stopTimer(void)
{
    ASSERT(mTimerId != INVALID_POSIX_TIMER_ID);

    struct itimerspec interval;
    NEMemory::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));

    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
    mEventCount     = 0;
    timer_settime(mTimerId, 0, &interval, nullptr);
}

void TimerPosix::_destroyTimer(void)
{
    if (_isStarted())
    {
        _stopTimer();
    }

    if (mTimerId != INVALID_POSIX_TIMER_ID)
    {
        timer_delete(mTimerId);
        mTimerId    = INVALID_POSIX_TIMER_ID;
    }
}

#endif // defined(_POSIX) || defined(POSIX)
