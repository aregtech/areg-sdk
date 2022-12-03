/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"
#include <signal.h>
#include <time.h>
#include <errno.h>

DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__osSystemTimerStart );
DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__posixTimerExpiredRoutine );

//////////////////////////////////////////////////////////////////////////
// POSIX specific methods
//////////////////////////////////////////////////////////////////////////

void TimerManager::_posixTimerExpiredRoutine( union sigval argSig )
{
    TRACE_SCOPE( areg_component_private_posix_TimerManager__posixTimerExpiredRoutine );

    TimerManager & timerManager = TimerManager::getInstance( );
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    ASSERT( posixTimer != nullptr );
    Timer * timer = timerManager.mTimerResource.findResourceObject( reinterpret_cast<TIMERHANDLE>(posixTimer) );

    if ( (timer != nullptr) && (posixTimer->isValid( )) )
    {
        TRACE_DBG( "Timer [ %s ] expired at [ %u ] sec and [ %u ] ns"
            , timer->getName( ).getString( )
            , posixTimer->getDueTime( ).tv_sec
            , posixTimer->getDueTime( ).tv_nsec );

        unsigned int highValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_sec);
        unsigned int lowValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timerExpired( );
        timerManager._processExpiredTimer( timer, reinterpret_cast<TIMERHANDLE>(posixTimer), highValue, lowValue );
    }
    else
    {
        TRACE_WARN( "Ignore handling timer [ %p ], it is [ %s ]", posixTimer, timer == nullptr ? "NOT REGISTERED ANYMORE" : "ALREADY INVALID" );
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
    TRACE_SCOPE(areg_component_private_posix_TimerManager__osSystemTimerStart );

    bool result{ false };
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timer.getHandle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime( CLOCK_MONOTONIC, &startTime );
    timer.timerStarting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    if (posixTimer->startTimer(timer, 0, &TimerManager::_posixTimerExpiredRoutine))
    {
        result = true;
        TRACE_DBG("Started timer [ %s ], expire time at [ %u ]sec : [ %u ] ns"
                        , timer.getName().getString()
                        , posixTimer->getDueTime().tv_sec
                        , posixTimer->getDueTime().tv_nsec);
    }
    else
    {
        TRACE_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                        , timer.getTimeout( )
                        , timer.getName( ).getString()
                        , static_cast<id_type>(errno) );
    }

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
