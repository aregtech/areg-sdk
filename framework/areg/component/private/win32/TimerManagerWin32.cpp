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
 *              Windows OS specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#ifdef  _WIN32

#include "areg/component/Timer.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

namespace areg {

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Windows OS timer routine function. Triggered, when one of timer is expired.
 * \param   argPtr      The pointer of argument passed to timer expired callback function
 * \param   lowValue    The low value of timer expiration
 * \param   highValue   The high value of timer expiration.
 **/
void TimerManager::_windows_timer_expired( void * argPtr, unsigned long lowValue, unsigned long highValue )
{
    TimerManager & timerManager = TimerManager::instance( );
    ASSERT( argPtr != nullptr );
    TIMERHANDLE handle = reinterpret_cast<void *>(argPtr);
    Timer * timer = timerManager.mTimerResource.find_resource_object( handle );
    if ( timer != nullptr )
    {
        timerManager._process_expired_timer( timer, handle, highValue, lowValue );
    }
}

void TimerManager::_os_timer_stop( TIMERHANDLE timerHandle )
{

    ASSERT( timerHandle != nullptr );
    ::CancelWaitableTimer( static_cast<HANDLE>(timerHandle) );
}

bool TimerManager::_os_timer_start( Timer & timer )
{
    ASSERT(timer.handle() != nullptr);

    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    long period = timer.event_count() > 1 ? static_cast<long>(timer.timeout()) : 0;
    int64_t due_time = static_cast<int64_t>(static_cast<TIME64>(timer.timeout()) * areg::MILLISEC_TO_100NS);  // timer from now
    due_time *= static_cast<int64_t>(-1);
    LARGE_INTEGER timeTrigger;
    timeTrigger.LowPart  = static_cast<DWORD>(areg::lo_dword(due_time));
    timeTrigger.HighPart = static_cast<LONG >(areg::hi_dword(due_time));

    FILETIME fileTime;
    ::GetSystemTimeAsFileTime( &fileTime );
    timer.timer_starting( fileTime.dwHighDateTime, fileTime.dwLowDateTime, reinterpret_cast<ptr_type>(timer.handle()) );

    return ( ::SetWaitableTimer(  timer.handle()
                                , &timeTrigger
                                , period
                                , (PTIMERAPCROUTINE)(&TimerManager::_windows_timer_expired)
                                , static_cast<void *>(timer.handle()), FALSE ) == TRUE );
}

} // namespace areg
#endif // _WIN32
