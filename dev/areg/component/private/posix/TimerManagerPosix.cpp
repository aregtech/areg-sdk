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
#include "areg/trace/GETrace.h"
#include <signal.h>
#include <time.h>
#include <errno.h>

DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__startSystemTimer);
DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__defaultPosixTimerExpiredRoutine);

// Auto-reset, initially not signaled
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
    {
        posixTimer->destroyTimer();
        delete posixTimer;
    }
}

bool TimerManager::_startSystemTimer( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    TRACE_SCOPE(areg_component_private_posix_TimerManager__startSystemTimer);

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
            if (posixTimer->startTimer(*whichTimer) == false)
            {
                TRACE_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                                , whichTimer->getFireTime( )
                                , whichTimer->getName( ).getString()
                                , static_cast<id_type>(errno) );

                result = false;
                timerInfo.mTimerState   = TimerInfo::TimerIdle;
                timerTable.updateObject( whichTimer, timerInfo );
            }
            else
            {
                TRACE_DBG("Started timer [ %s ], start time: [ %u ] sec, [ %u ] ns, expire time [ %u ] sec, [ %u ] ns"
                            , whichTimer->getName().getString()
                            , ts.tv_sec
                            , ts.tv_nsec
                            , posixTimer->getDueTime().tv_sec
                            , posixTimer->getDueTime().tv_nsec);
            }
        }
    }

    return result;
}

void TimerManager::_defaultPosixTimerExpiredRoutine( union sigval argSig )
{
    TRACE_SCOPE(areg_component_private_posix_TimerManager__defaultPosixTimerExpiredRoutine);

    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    if (posixTimer != NULL && posixTimer->isValid())
    {
        TRACE_DBG("Timer [ %s ] expired at [ %u ] sec and [ %u ] ns, going to notify thread [ %u ]"
                        , posixTimer->mContext->getName().getString()
                        , posixTimer->getDueTime().tv_sec
                        , posixTimer->getDueTime().tv_nsec
                        , static_cast<unsigned int>(posixTimer->mThreadId));

        TimerManager::getInstance()._timerExpired(posixTimer->mContext, posixTimer->mDueTime.tv_sec, posixTimer->mDueTime.tv_nsec);
        posixTimer->timerExpired();

        TRACE_DBG("Notify lock and wait");

        SynchLockAndWaitIX * lockAndWait = SynchLockAndWaitIX::_mapWaitIdResource.findResourceObject(posixTimer->mThreadId);
        if ( lockAndWait != NULL )
        {
            lockAndWait->_notifyAsynchSignal( );
        }
        else
        {
            TRACE_WARN("Lock and Wait object of thread [ %u ] not foind, ignore timer expire event.", static_cast<unsigned int>(posixTimer->mThreadId));
        }
    }
}

#endif // _POSIX
