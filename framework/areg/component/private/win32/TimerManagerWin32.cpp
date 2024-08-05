/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              Windows OS specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#ifdef  _WINDOWS

#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"

#include <Windows.h>


//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Windows OS timer routine function. Triggered, when one of timer is expired.
 * \param   argPtr      The pointer of argument passed to timer expired callback function
 * \param   lowValue    The low value of timer expiration
 * \param   highValue   The high value of timer expiration.
 **/
void TimerManager::_windowsTimerExpiredRoutine( void * argPtr, unsigned long lowValue, unsigned long highValue )
{
    TimerManager & timerManager = TimerManager::getInstance( );
    ASSERT( argPtr != nullptr );
    TIMERHANDLE handle = reinterpret_cast<void *>(argPtr);
    Timer * timer = timerManager.mTimerResource.findResourceObject( handle );
    if ( timer != nullptr )
    {
        timerManager._processExpiredTimer( timer, handle, highValue, lowValue );
    }
}

void TimerManager::_osSsystemTimerStop( TIMERHANDLE timerHandle )
{

    ASSERT( timerHandle != nullptr );
    ::CancelWaitableTimer( static_cast<HANDLE>(timerHandle) );
}

bool TimerManager::_osSystemTimerStart( Timer & timer )
{
    ASSERT(timer.getHandle() != nullptr);

    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    long period = timer.getEventCount() > 1 ? static_cast<long>(timer.getTimeout()) : 0;
    int64_t dueTime = static_cast<int64_t>(static_cast<TIME64>(timer.getTimeout()) * NEUtilities::MILLISEC_TO_100NS);  // timer from now
    dueTime *= static_cast<int64_t>(-1);
    LARGE_INTEGER timeTrigger;
    timeTrigger.LowPart  = static_cast<DWORD>(MACRO_64_LO_BYTE32(dueTime));
    timeTrigger.HighPart = static_cast<LONG >(MACRO_64_HI_BYTE32(dueTime));

    FILETIME fileTime;
    ::GetSystemTimeAsFileTime( &fileTime );
    timer.timerStarting( fileTime.dwHighDateTime, fileTime.dwLowDateTime, reinterpret_cast<ptr_type>(timer.getHandle()) );

    return ( ::SetWaitableTimer(  timer.getHandle()
                                , &timeTrigger
                                , period
                                , (PTIMERAPCROUTINE)(&TimerManager::_windowsTimerExpiredRoutine)
                                , static_cast<void *>(timer.getHandle()), FALSE ) == TRUE );
}

#endif // _WINDOWS
