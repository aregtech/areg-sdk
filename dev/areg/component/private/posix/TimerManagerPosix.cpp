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
    Timer*      timerOwner;
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
}

void TimerManager::_stopSystemTimer( TIMERHANDLE timerHandle )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != NULL )
        posixTimer->stopTimer();
}

void TimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool /* cancelTimer */ )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != NULL )
        posixTimer->destroyTimer();
}

bool TimerManager::_startSystemTimer( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    bool result = false;
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timerInfo.getHandle());
    Timer * whichTimer        = timerInfo.getTimer();
    if ((posixTimer != NULL) && (whichTimer != NULL))
    {
        if (posixTimer->createTimer(&TimerManager::_defaultPosixTimerExpiredRoutine))
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            timerInfo.isTimerStarting(ts.tv_sec, ts.tv_nsec);
            timerTable.registerObject( whichTimer, timerInfo );

            result = true;
            if (posixTimer->startTimer(*whichTimer, NULL) == false)
            {
                OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                            , whichTimer->getFireTime( )
                            , whichTimer->getName( )
                            , static_cast<id_type>(errno) );

                result = false;
                timerInfo.mTimerState   = TimerInfo::TimerIdle;
                timerTable.updateObject( whichTimer, timerInfo );
            }
        }
    }

    return result;
}

void TimerManager::_defaultPosixTimerExpiredRoutine( union sigval argSig )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    if (posixTimer != NULL && posixTimer->isValid())
    {
        TimerManager::getInstance()._timerExpired(posixTimer->mContext, posixTimer->mDueTime.tv_sec, posixTimer->mDueTime.tv_nsec);
        posixTimer->timerExpired();

        SynchLockAndWaitIX * lockAndWait = SynchLockAndWaitIX::_mapWaitIdResource.findResourceObject(posixTimer->mThreadId);
        if ( lockAndWait != NULL )
        {
            lockAndWait->_notifyAsynchSignal( );
        }
    }
}

#endif // _POSIX
