/************************************************************************
 * \file        areg/component/private/posix/TimerPosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/posix/TimerPosix.hpp"

#ifdef _POSIX
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEMemory.hpp"

TimerPosix::TimerPosix( void )
    : mTimerId      ( 0     )
    , mContext      ( NULL  )
    , mEventCount   (       )
    , mDueTime      (       )
    , mThreadId     ( Thread::INVALID_THREAD_ID )
    , mLock         ( false )
{
    ; // do nothing
}

TimerPosix::~TimerPosix(void)
{
    mLock.lock();
    _destroyTimer();
    mLock.unlock();
}

bool TimerPosix::createTimer( FuncPosixTimerRoutine funcTimer )
{
    SpinLockIX lock(mLock);

    if (mTimerId != 0)
        _destroyTimer();

    if (funcTimer != NULL)
        _createTimer(funcTimer);

    return (mTimerId != 0);
}

bool TimerPosix::startTimer( Timer & context, FuncPosixTimerRoutine funcTimer )
{
    SpinLockIX lock(mLock);

    if (funcTimer != NULL)
    {
        if (mTimerId != 0)
        {
            _destroyTimer();
        }

        _createTimer(funcTimer);
    }

    return startTimer(context);
}

bool TimerPosix::startTimer( Timer & context )
{
    SpinLockIX lock(mLock);
    mContext    = &context;
    return startTimer();
}


bool TimerPosix::startTimer( void )
{
    SpinLockIX lock(mLock);

    bool result = false;
    if ((mTimerId != 0) && (mContext != NULL))
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

bool TimerPosix::pauseTimer(void)
{
    SpinLockIX lock(mLock);
    
    if ( _isStarted() )
    {
        _stopTimer();
    }

    return (mTimerId != 0);
}

bool TimerPosix::stopTimer(void)
{
    SpinLockIX lock(mLock);

    if ( _isStarted() )
    {
        _stopTimer();
    }

    return (mTimerId != 0);
}

void TimerPosix::destroyTimer(void)
{
    SpinLockIX lock(mLock);

    _destroyTimer();

    mContext    = NULL;
    mEventCount = 0;
}

void TimerPosix::timerExpired(void)
{
    SpinLockIX lock(mLock);

    if ((mTimerId != 0) && (mEventCount != 0))
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
    NEMemory::zeroBuffer(static_cast<void *>(&sigEvent), sizeof(struct sigevent));

    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = funcTimer;
    sigEvent.sigev_notify_attributes= NULL;

    return (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId));
}

bool TimerPosix::_startTimer(unsigned int msTimeout, unsigned int eventCount)
{
    bool result = true;

    struct itimerspec interval;
    NEMemory::zeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));
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
    if (RETURNED_OK != timer_settime(mTimerId, 0, &interval, NULL))
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
    ASSERT(mTimerId != 0);

    struct itimerspec interval;
    NEMemory::zeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));

    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
    mEventCount     = 0;

    timer_settime(mTimerId, 0, &interval, NULL);
}

void TimerPosix::_destroyTimer(void)
{
    if (_isStarted())
    {
        _stopTimer();
    }

    if (mTimerId != 0)
    {
        timer_delete(mTimerId);
        mTimerId    = 0;
    }
}

#endif // _POSIX
