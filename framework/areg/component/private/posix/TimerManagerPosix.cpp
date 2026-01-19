/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include <signal.h>
#include <time.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// POSIX specific methods
//////////////////////////////////////////////////////////////////////////

void TimerManager::_posixTimerExpiredRoutine( union sigval argSig )
{
    TimerManager & timerManager = TimerManager::getInstance( );
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    ASSERT( posixTimer != nullptr );
    Timer * timer = timerManager.mTimerResource.findResourceObject( reinterpret_cast<TIMERHANDLE>(posixTimer) );

    if ( (timer != nullptr) && (posixTimer->isValid( )) )
    {
        unsigned int highValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_sec);
        unsigned int lowValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timerExpired( );
        timerManager._processExpiredTimer( timer, reinterpret_cast<TIMERHANDLE>(posixTimer), highValue, lowValue );
    }
}

void TimerManager::_osSsystemTimerStop( TIMERHANDLE timerHandle )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != nullptr )
    {
        posixTimer->stopTimer();
    }
}

bool TimerManager::_osSystemTimerStart( Timer & timer )
{
    bool result{ false };
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timer.getHandle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime( CLOCK_REALTIME, &startTime );
    timer.timerStarting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    if (posixTimer->startTimer(timer, 0, &TimerManager::_posixTimerExpiredRoutine))
    {
        result = true;
    }

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
