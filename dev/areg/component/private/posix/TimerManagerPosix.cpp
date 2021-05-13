/************************************************************************
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#ifdef  _POSIX

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include <signal.h>
#include <time.h>
#include <errno.h>

#define _MILLISECOND    10000

// Autoreset, initially not signaled
typedef struct S_PosixTimer
{
    timer_t     timerId;
    Timer*    timerOwner;
    uint32_t    timerTimeout;
    uint32_t    timerPeriod;
    time_t      timerDueSecs;
    long        timerDueNs;
    ITEM_ID     timerThreadId;
} sPosixTimer;

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

TIMERHANDLE TimerManager::_createWaitableTimer( const char * /* timerName */ )
{
    return static_cast<TIMERHANDLE>(DEBUG_NEW TimerPosix());
    /******************************
    sPosixTimer *posixTimer = new sPosixTimer;
    if ( posixTimer != NULL )
    {
        NEMemory::ZeroBuffer(static_cast<void *>(posixTimer), sizeof(sPosixTimer));
    }

    // return reinterpret_cast<TIMERHANDLE>( DEBUG_NEW WaitableTimerIX(true, false, timerName) );

    return reinterpret_cast<TIMERHANDLE>(posixTimer);
    ******************************/
}

void TimerManager::_stopSystemTimer( TIMERHANDLE timerHandle )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != NULL )
        posixTimer->StopTimer();

    /******************************
    ASSERT( timerHandle != static_cast<TIMERHANDLE>(NULL) );
    sPosixTimer * posixTimer = reinterpret_cast<sPosixTimer *>(timerHandle);
    if (posixTimer->timerId != static_cast<timer_t>(NULL))
    {
        itimerspec cancelSpec;
        NEMemory::ZeroBuffer(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        timer_settime(posixTimer->timerId, 0, &cancelSpec, NULL);
    }
    ******************************/
}

void TimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool /* cancelTimer */ )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != NULL )
        posixTimer->DestroyTimer();

    /******************************
    ASSERT( timerHandle != static_cast<TIMERHANDLE>(NULL) );
    sPosixTimer * posixTimer = reinterpret_cast<sPosixTimer *>(timerHandle);
    if (posixTimer->timerId != static_cast<timer_t>(NULL))
    {
        if (cancelTimer)
        {
            _stopSystemTimer(timerHandle);
        }

        timer_delete(posixTimer->timerId);
        posixTimer->timerId = static_cast<timer_t>(NULL);
    }

    delete posixTimer;
    ******************************/
}

bool TimerManager::_startSystemTimer( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    bool result = false;
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timerInfo.getHandle());
    Timer * whichTimer        = timerInfo.getTimer();
    if ((posixTimer != NULL) && (whichTimer != NULL))
    {
        if (posixTimer->CreateTimer(&TimerManager::_defaultPosixTimerExpiredRoutine))
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            timerInfo.isTimerStarting(ts.tv_sec, ts.tv_nsec);
            timerTable.setAt( whichTimer, timerInfo, true );

            result = true;
            if (posixTimer->StartTimer(*whichTimer, NULL) == false)
            {
                OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                            , whichTimer->getFireTime( )
                            , whichTimer->getName( )
                            , errno );

                result = false;
                timerInfo.mTimerState   = TimerInfo::TimerIdle;
                timerTable.setAt( whichTimer, timerInfo, true );
            }
        }
    }

    return result;

    /******************************
    sPosixTimer * posixTimer= reinterpret_cast<sPosixTimer *>(timerInfo.getHandle());
    if (posixTimer != NULL)
    {
        ASSERT(posixTimer->timerId == static_cast<timer_t>(NULL));

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        timerInfo.TimerIsStarting(ts.tv_nsec, ts.tv_sec);

        struct sigevent sigEvent;
        NEMemory::ZeroBuffer(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
        sigEvent.sigev_notify           = SIGEV_THREAD;
        sigEvent.sigev_value.sival_ptr  = static_cast<void *>(posixTimer);
        sigEvent.sigev_notify_function  = &TimerManager::_defaultPosixTimerExpiredRoutine;
        sigEvent.sigev_notify_attributes= NULL;

        if (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &posixTimer->timerId))
        {
            Timer * whichTimer    = timerInfo.getTimer();
            ASSERT( whichTimer != static_cast<Timer *>(NULL) );

            // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
            unsigned int countPeriod= whichTimer->GetEventCount( ) > 1 ? whichTimer->getFireTime( ) : 0;
            unsigned int msTimeout  = whichTimer->getFireTime();    // timer from now

            struct itimerspec interval;
            NEMemory::ZeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));
            NESynchTypesIX::ConvertTimeout(interval.it_value, msTimeout);
            if (countPeriod > 0)
            {
                interval.it_interval.tv_sec = interval.it_value.tv_sec;
                interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
            }

            posixTimer->timerOwner  = whichTimer;
            posixTimer->timerPeriod = countPeriod;
            posixTimer->timerTimeout= msTimeout;    // timer from now
            posixTimer->timerDueSecs= interval.it_value.tv_sec;
            posixTimer->timerDueNs  = interval.it_value.tv_nsec;

            timerTable.SetKey( whichTimer, timerInfo, true );
            if ( RETURNED_OK != timer_settime(posixTimer->timerId, 0, &interval, NULL) )
            {
                OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                            , whichTimer->getFireTime( )
                            , whichTimer->getTimerName( )
                            , errno );

                timerInfo.mTimerState   = TimerInfo::TimerIdle;
                timerTable.SetKey( whichTimer, timerInfo, true );
            }
            else
            {
                result = true;
            }
        }
    }

    return result;
    ******************************/
}

void TimerManager::_defaultPosixTimerExpiredRoutine( union sigval argSig )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    if (posixTimer != NULL && posixTimer->isValid())
    {
        TimerManager::getInstance().isTimerExpired(posixTimer->mContext, posixTimer->mDueTime.tv_sec, posixTimer->mDueTime.tv_nsec);
        posixTimer->TimerExpired();

        SynchLockAndWaitIX * lockAndWait = SynchLockAndWaitIX::_mapWaitIdResource.findResourceObject(posixTimer->mThreadId);
        if ( lockAndWait != NULL )
        {
            lockAndWait->_notifyAsynchSignal( );
        }
    }

    /******************************
    sPosixTimer *posixTimer = reinterpret_cast<sPosixTimer *>(argSig.sival_ptr);
    if (posixTimer != NULL)
    {
        if (posixTimer->timerId != static_cast<timer_t>(NULL))
        {
            if ((posixTimer->timerPeriod > 0) && (-- posixTimer->timerPeriod == 0))
            {
                TimerManager::_stopSystemTimer(static_cast<TIMERHANDLE>(posixTimer));
            }

            TimerManager::getInstance().isTimerExpired(posixTimer->timerOwner, posixTimer->timerDueNs, posixTimer->timerDueNs);

            timespec ts;
            ts.tv_sec   = posixTimer->timerDueSecs;
            ts.tv_nsec  = posixTimer->timerDueNs;
            NESynchTypesIX::ConvertTimeout(ts, posixTimer->timerTimeout);
            posixTimer->timerDueSecs= ts.tv_sec;
            posixTimer->timerDueNs  = ts.tv_nsec;
        }
    }
    ******************************/
}

#endif // _POSIX
