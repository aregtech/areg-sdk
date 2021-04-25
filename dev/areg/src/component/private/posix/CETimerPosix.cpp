/************************************************************************
 * \file        areg/src/component/private/posix/CETimerPosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/component/private/posix/CETimerPosix.hpp"

#ifdef _POSIX
#include "areg/src/component/private/CETimerManager.hpp"
#include "areg/src/component/CETimer.hpp"
#include "areg/src/base/NEMemory.hpp"

CETimerPosix::CETimerPosix( void )
    : mTimerId      ( 0     )
    , mContext      ( NULL  )
    , mEventCount   (       )
    , mDueTime      (       )
    , mThreadId     ( CEThread::INVALID_THREAD_ID )
    , mLock         ( false )
{
    ; // do nothing
}

CETimerPosix::~CETimerPosix(void)
{
    DestroyTimer();
}

bool CETimerPosix::CreateTimer( FuncPosixTimerRoutine funcTimerRoutine )
{
    CESpinLockIX lock(mLock);

    if (mTimerId != 0)
        destroyTimer();

    if (funcTimerRoutine != NULL)
        createTimer(funcTimerRoutine);

    return (mTimerId != 0);
}


bool CETimerPosix::StartTimer( CETimer & context, FuncPosixTimerRoutine funcTimerRoutine )
{
    CESpinLockIX lock(mLock);

    if (funcTimerRoutine != NULL)
    {
        if (mTimerId != 0)
            destroyTimer();

        createTimer(funcTimerRoutine);
    }

    mContext    = &context;
    mEventCount = context.GetEventCount();

    return StartTimer();
}

bool CETimerPosix::StartTimer( void )
{
    CESpinLockIX lock(mLock);

    bool result = false;
    if ((mTimerId != 0) && (mContext != NULL))
    {
        if ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0))
            stopTimer();

        unsigned int msTimeout = mContext->GetFireTime();
        unsigned int eventCount= mContext->GetEventCount();

        if ((msTimeout != 0) && (eventCount != 0) && (mEventCount != 0))
        {
            result = startTimer(msTimeout, eventCount);
        }
    }

    return result;
}

bool CETimerPosix::PauseTimer(void)
{
    CESpinLockIX lock(mLock);
    
    if ( (mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0) )
        stopTimer();

    return (mTimerId != 0);
}

bool CETimerPosix::StopTimer(void)
{
    CESpinLockIX lock(mLock);

    if ( (mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0) )
        stopTimer();

    mEventCount = 0;

    return (mTimerId != 0);
}

void CETimerPosix::DestroyTimer(void)
{
    CESpinLockIX lock(mLock);

    if (mDueTime.tv_sec != 0 || mDueTime.tv_nsec != 0)
        stopTimer();

    if (mTimerId != 0)
        destroyTimer();

    mContext    = NULL;
    mEventCount = 0;
}

void CETimerPosix::TimerExpired(void)
{
    CESpinLockIX lock(mLock);

    if ((mTimerId != 0) && (mEventCount != 0))
    {
        if ((mEventCount != CETimer::CONTINUOUSLY) && (-- mEventCount == 0) )
        {
            stopTimer();
        }
        else
        {
            NESynchTypesIX::ConvertTimeout(mDueTime, mContext->GetFireTime());
        }
    }
}

bool CETimerPosix::createTimer( FuncPosixTimerRoutine funcTimerRoutine )
{
    struct sigevent sigEvent;
    NEMemory::ZeroBuffer(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = funcTimerRoutine;
    sigEvent.sigev_notify_attributes= NULL;

    return (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId));
}

bool CETimerPosix::startTimer(unsigned int msTimeout, unsigned int eventCount)
{
    bool result = true;

    struct itimerspec interval;
    NEMemory::ZeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));
    NESynchTypesIX::ConvertTimeout(interval.it_value, msTimeout);
    if (eventCount > 1)
    {
        interval.it_interval.tv_sec = interval.it_value.tv_sec;
        interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
    }

    clock_gettime(CLOCK_REALTIME, &mDueTime);
    NESynchTypesIX::ConvertTimeout(mDueTime, msTimeout);

    mThreadId   = CEThread::GetCurrentThreadId();
    if (RETURNED_OK != timer_settime(mTimerId, 0, &interval, NULL))
    {
        result          = false;
        mDueTime.tv_sec = 0;
        mDueTime.tv_nsec= 0;
    }

    return result;
}

void CETimerPosix::stopTimer(void)
{
    struct itimerspec interval;
    NEMemory::ZeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));
    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;

    timer_settime(mTimerId, 0, &interval, NULL);
}

void CETimerPosix::destroyTimer(void)
{
    if ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0))
        stopTimer();

    if (mTimerId != 0)
        timer_delete(mTimerId);

    mTimerId = 0;
}

#endif // _POSIX
