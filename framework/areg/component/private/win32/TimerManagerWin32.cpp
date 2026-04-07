/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/win32/TimerManagerWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              Windows OS specific calls.
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <Windows.h>

namespace {

struct Win32TimerHandle
{
    HANDLE      timerHandle { nullptr };    //!< Standard WaitableTimer for WatchdogManager APC delivery.
    PTP_TIMER   timerPool   { nullptr };    //!< Thread-pool timer for TimerManager (created lazily).
};

/************************************************************************
 * \brief   Thread-pool timer callback. Runs in a Windows thread-pool
 *          thread when a timer fires.
 *
 *  pv is the Win32TimerHandle* that was passed as context to
 *  CreateThreadpoolTimer — identical to the TIMERHANDLE stored as the
 *  key in TimerManager::mTimerResource, so the timer lookup works
 *  without an extra map.
 ************************************************************************/
VOID CALLBACK _tp_timer_callback( PTP_CALLBACK_INSTANCE /*instance*/,
                                   PVOID                  pv,
                                   PTP_TIMER              /*timer*/ ) noexcept
{
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    areg::TimerManager::_windows_timer_expired(pv, ft.dwLowDateTime, ft.dwHighDateTime);
}

} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

void TimerManager::_windows_timer_expired( void * argPtr,
                                            unsigned long lowValue,
                                            unsigned long highValue ) noexcept
{
    TimerManager & timerManager = TimerManager::instance( );
    ASSERT( argPtr != nullptr );
    TIMERHANDLE handle  = static_cast<TIMERHANDLE>(argPtr);
    Timer * timer       = timerManager.mTimerResource.find_resource_object( handle );
    if ( timer != nullptr )
    {
        timerManager._process_expired_timer( timer, handle, highValue, lowValue );
    }
}

void TimerManager::_os_timer_stop( TIMERHANDLE timerHandle )
{
    Win32TimerHandle * h = static_cast<Win32TimerHandle *>(timerHandle);
    if ( (h != nullptr) && (h->timerPool != nullptr) )
    {
        // Disarm: passing nullptr due-time stops the timer without closing it.
        ::SetThreadpoolTimer(h->timerPool, nullptr, 0, 0);
    }
}

bool TimerManager::_os_timer_start( Timer & timer )
{
    Win32TimerHandle * h = static_cast<Win32TimerHandle *>(timer.handle());
    ASSERT(h != nullptr);

    // Create the thread-pool timer on first use.
    // Context = the TIMERHANDLE (Win32TimerHandle*) — used for lookup in the callback.
    if (h->timerPool == nullptr)
    {
        h->timerPool = ::CreateThreadpoolTimer(_tp_timer_callback, timer.handle(), nullptr);
        if (h->timerPool == nullptr)
            return false;
    }

    FILETIME fileTime;
    ::GetSystemTimeAsFileTime(&fileTime);
    timer.timer_starting(fileTime.dwHighDateTime, fileTime.dwLowDateTime, reinterpret_cast<ptr_type>(timer.handle()));

    // Relative due time: negative 100-ns value means "fire this many 100-ns from now".
    // SetThreadpoolTimer interprets negative FILETIME the same way SetWaitableTimer does.
    const int64_t due = -(static_cast<int64_t>(timer.timeout() * areg::MILLISEC_TO_100NS));
    FILETIME dueTime;
    dueTime.dwLowDateTime  = static_cast<DWORD>(areg::lo_dword(static_cast<uint64_t>(due)));
    dueTime.dwHighDateTime = static_cast<DWORD>(areg::hi_dword(static_cast<uint64_t>(due)));

    const DWORD period = timer.event_count() > 1 ? static_cast<DWORD>(timer.timeout()) : 0u;

    // SetThreadpoolTimer has no return value; failure is silent.
    // The thread-pool scheduler uses high-resolution clocks internally.
    ::SetThreadpoolTimer(h->timerPool, &dueTime, period, 0);
    return true;
}

} // namespace areg
#endif // _WIN32
