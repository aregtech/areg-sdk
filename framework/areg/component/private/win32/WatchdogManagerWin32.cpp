/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/win32/WatchdogManagerWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Thread Watchdog Manager.
 *              Controlling, triggering and stopping timer to check threads.
 *              Windows OS specific calls.
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"
#include "areg/component/private/Watchdog.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <Windows.h>

namespace {

struct Win32TimerHandle
{
    HANDLE      timerHandle { nullptr };    //!< Standard WaitableTimer for APC delivery.
    PTP_TIMER   timerPool   { nullptr };    //!< Thread-pool timer (unused by WatchdogManager).
};

} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

void WatchdogManager::_os_timer_stop( TIMERHANDLE handle )
{
    const Win32TimerHandle * h = static_cast<const Win32TimerHandle *>(handle);
    if ( (h != nullptr) && (h->timerHandle != nullptr) )
    {
        ::CancelWaitableTimer(h->timerHandle);
    }
}

bool WatchdogManager::_os_timer_start( Watchdog & watchdog )
{
    Win32TimerHandle * h = static_cast<Win32TimerHandle *>(watchdog.handle());
    ASSERT( (h != nullptr) && (h->timerHandle != nullptr) );

    // One-shot watchdog timer — no period.
    const int64_t due_time = -(static_cast<int64_t>(watchdog.timeout() * areg::MILLISEC_TO_100NS));
    LARGE_INTEGER timeTrigger{};
    timeTrigger.LowPart  = static_cast<DWORD>(areg::lo_dword(static_cast<uint64_t>(due_time)));
    timeTrigger.HighPart = static_cast<LONG >(areg::hi_dword(static_cast<uint64_t>(due_time)));

    // Standard WaitableTimer supports APC callbacks (unlike HIGH_RES handles).
    return (::SetWaitableTimerEx( h->timerHandle
                                , &timeTrigger
                                , 0     // one-shot: no period
                                , (PTIMERAPCROUTINE)(&WatchdogManager::_windows_watchdog_expired)
                                , reinterpret_cast<void*>(watchdog.make_watchdog_id(watchdog.id(), watchdog.sequence()))
                                , nullptr   // WakeContext: unused
                                , 0         // TolerableDelay: 0 = maximum precision
                                ) == TRUE);
}

void WatchdogManager::_windows_watchdog_expired( void * argPtr,
                                                  unsigned long lowValue,
                                                  unsigned long highValue ) noexcept
{
    ASSERT(argPtr != nullptr);
    WatchdogManager & watchdogManager = WatchdogManager::instance();
    const Watchdog::WATCHDOG_ID watchdog_id = reinterpret_cast<Watchdog::WATCHDOG_ID>(argPtr);
    const Watchdog::GUARD_ID    guardId     = Watchdog::make_guard_id(watchdog_id);
    Watchdog * watchdog = watchdogManager.mWatchdogResource.find_resource_object(guardId);
    if (watchdog != nullptr)
    {
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

} // namespace areg
#endif // _WIN32
