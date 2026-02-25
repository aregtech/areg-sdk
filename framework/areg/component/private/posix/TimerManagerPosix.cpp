/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/UtilityDefs.hpp"

#ifndef __APPLE__
    #include <signal.h>
#endif  // !__APPLE__
#include <time.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// POSIX specific methods
//////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
void TimerManager::_posix_timer_expired( TimerPosix* posixTimer )
{
    TimerManager & timerManager = TimerManager::instance( );
    ASSERT( posixTimer != nullptr );
    Timer * timer = timerManager.mTimerResource.find_resource_object( reinterpret_cast<TIMERHANDLE>(posixTimer) );

    if ( (timer != nullptr) && (posixTimer->is_valid( )) )
    {
        uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timer_expired( );
        timerManager._process_expired_timer( timer, reinterpret_cast<TIMERHANDLE>(posixTimer), highValue, lowValue );
    }
}
#else   // !__APPLE__
void TimerManager::_posix_timer_expired( union sigval argSig )
{
    TimerManager & timerManager = TimerManager::instance( );
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    ASSERT( posixTimer != nullptr );
    Timer * timer = timerManager.mTimerResource.find_resource_object( reinterpret_cast<TIMERHANDLE>(posixTimer) );

    if ( (timer != nullptr) && (posixTimer->is_valid( )) )
    {
        uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timer_expired( );
        timerManager._process_expired_timer( timer, reinterpret_cast<TIMERHANDLE>(posixTimer), highValue, lowValue );
    }
}
#endif  // __APPLE__

void TimerManager::_os_timer_stop( TIMERHANDLE timerHandle )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != nullptr )
    {
        posixTimer->stop_timer();
    }
}

bool TimerManager::_os_timer_start( Timer & timer )
{
    bool result{ false };
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timer.handle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime( CLOCK_REALTIME, &startTime );
    timer.timer_starting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    if (posixTimer->start_timer(timer, 0, &TimerManager::_posix_timer_expired))
    {
        result = true;
    }

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
