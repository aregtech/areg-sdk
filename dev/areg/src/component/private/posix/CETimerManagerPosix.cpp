/************************************************************************
 * \file        areg/src/component/private/CETimerManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/src/component/private/CETimerManager.hpp"

#ifdef  _POSIX

#include "areg/src/component/private/posix/CETimerPosix.hpp"
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"
#include "areg/src/component/CETimer.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include <signal.h>
#include <time.h>
#include <errno.h>

#define _MILLISECOND    10000

// Autoreset, initially not signaled
typedef struct S_PosixTimer
{
    timer_t     timerId;
    CETimer*    timerOwner;
    uint32_t    timerTimeout;
    uint32_t    timerPeriod;
    time_t      timerDueSecs;
    long        timerDueNs;
    ITEM_ID     timerThreadId;
} sPosixTimer;

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

TIMERHANDLE CETimerManager::_createWaitableTimer( const char * /* timerName */ )
{
    return static_cast<TIMERHANDLE>(DEBUG_NEW CETimerPosix());
    /******************************
    sPosixTimer *posixTimer = new sPosixTimer;
    if ( posixTimer != NULL )
    {
        NEMemory::ZeroBuffer(static_cast<void *>(posixTimer), sizeof(sPosixTimer));
    }

    // return reinterpret_cast<TIMERHANDLE>( DEBUG_NEW CEWaitableTimerIX(true, false, timerName) );

    return reinterpret_cast<TIMERHANDLE>(posixTimer);
    ******************************/
}

void CETimerManager::_stopSystemTimer( TIMERHANDLE timerHandle )
{
    CETimerPosix * posixTimer = reinterpret_cast<CETimerPosix *>(timerHandle);
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

void CETimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool /* cancelTimer */ )
{
    CETimerPosix * posixTimer = reinterpret_cast<CETimerPosix *>(timerHandle);
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

bool CETimerManager::_startSystemTimer( CETimerInfo & timerInfo, MapTimerTable & timerTable )
{
    bool result = false;
    CETimerPosix * posixTimer   = reinterpret_cast<CETimerPosix *>(timerInfo.GetHandle());
    CETimer * whichTimer        = timerInfo.GetTimer();
    if ((posixTimer != NULL) && (whichTimer != NULL))
    {
        if (posixTimer->CreateTimer(&CETimerManager::_defaultPosixTimerExpiredRoutine))
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            timerInfo.TimerIsStarting(ts.tv_sec, ts.tv_nsec);
            timerTable.SetKey( whichTimer, timerInfo, true );

            result = true;
            if (posixTimer->StartTimer(*whichTimer, NULL) == false)
            {
                OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                            , whichTimer->GetFireTime( )
                            , whichTimer->GetTimerName( )
                            , errno );

                result = false;
                timerInfo.mTimerState   = CETimerInfo::TimerIdle;
                timerTable.SetKey( whichTimer, timerInfo, true );
            }
        }
    }

    return result;

    /******************************
    sPosixTimer * posixTimer= reinterpret_cast<sPosixTimer *>(timerInfo.GetHandle());
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
        sigEvent.sigev_notify_function  = &CETimerManager::_defaultPosixTimerExpiredRoutine;
        sigEvent.sigev_notify_attributes= NULL;

        if (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &posixTimer->timerId))
        {
            CETimer * whichTimer    = timerInfo.GetTimer();
            ASSERT( whichTimer != static_cast<CETimer *>(NULL) );

            // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
            unsigned int countPeriod= whichTimer->GetEventCount( ) > 1 ? whichTimer->GetFireTime( ) : 0;
            unsigned int msTimeout  = whichTimer->GetFireTime();    // timer from now

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
                            , whichTimer->GetFireTime( )
                            , whichTimer->GetTimerName( )
                            , errno );

                timerInfo.mTimerState   = CETimerInfo::TimerIdle;
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

void CETimerManager::_defaultPosixTimerExpiredRoutine( union sigval argSig )
{
    CETimerPosix * posixTimer = reinterpret_cast<CETimerPosix *>(argSig.sival_ptr);
    if (posixTimer != NULL && posixTimer->IsValid())
    {
        CETimerManager::GetTimerManager().TimerIsExpired(posixTimer->mContext, posixTimer->mDueTime.tv_sec, posixTimer->mDueTime.tv_nsec);
        posixTimer->TimerExpired();

        CESynchLockAndWaitIX * lockAndWait = CESynchLockAndWaitIX::_mapWaitIdResource.FindResourceObject(posixTimer->mThreadId);
        if ( lockAndWait != NULL )
        {
            lockAndWait->NotifyAsynchSignal( );
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
                CETimerManager::_stopSystemTimer(static_cast<TIMERHANDLE>(posixTimer));
            }

            CETimerManager::GetTimerManager().TimerIsExpired(posixTimer->timerOwner, posixTimer->timerDueNs, posixTimer->timerDueNs);

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
