/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/private/win32/Win32Timer.hpp"

namespace {

/************************************************************************
 * \brief   Thread-pool timer callback, run in a Windows thread-pool thread when a
 *          watchdog expires. \a pv is the areg::os::Win32TimerHandle* passed as the
 *          context of CreateThreadpoolTimer().
 *
 * \note    A waitable timer completion routine cannot be used here: it arrives as an
 *          APC on the thread that armed the timer, which does not wait alertably.
 ************************************************************************/
VOID CALLBACK _tp_watchdog_callback( PTP_CALLBACK_INSTANCE /*instance*/,
                                     PVOID                  pv,
                                     PTP_TIMER              /*timer*/ ) noexcept
{
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);
    areg::WatchdogManager::_windows_watchdog_expired(pv, ft.dwLowDateTime, ft.dwHighDateTime);
}

} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

void WatchdogManager::_os_timer_stop( TIMERHANDLE handle )
{
    areg::os::Win32TimerHandle * h = static_cast<areg::os::Win32TimerHandle *>(handle);
    if ( (h != nullptr) && (h->timerPool != nullptr) )
    {
        // Disarm: a null due-time stops the timer without closing it.
        ::SetThreadpoolTimer(h->timerPool, nullptr, 0, 0);
    }
}

bool WatchdogManager::_os_timer_start( Watchdog & watchdog )
{
    areg::os::Win32TimerHandle * h = static_cast<areg::os::Win32TimerHandle *>(watchdog.handle());
    ASSERT( h != nullptr );

    if (h->timerPool == nullptr)
    {
        h->timerPool = ::CreateThreadpoolTimer(_tp_watchdog_callback, h, nullptr);
        if (h->timerPool == nullptr)
            return false;
    }

    // The callback context is fixed when the pool timer is created
    h->contextId = static_cast<uintptr_t>(watchdog.make_watchdog_id(watchdog.id(), watchdog.sequence()));

    // Relative due time: a negative 100-ns value means "fire that long from now".
    const int64_t due = -(static_cast<int64_t>(watchdog.timeout() * areg::MILLISEC_TO_100NS));
    FILETIME dueTime;
    dueTime.dwLowDateTime  = static_cast<DWORD>(areg::lo_dword(static_cast<uint64_t>(due)));
    dueTime.dwHighDateTime = static_cast<DWORD>(areg::hi_dword(static_cast<uint64_t>(due)));

    ::SetThreadpoolTimer(h->timerPool, &dueTime, 0, 0);
    return true;
}

void WatchdogManager::_windows_watchdog_expired( void * argPtr,
                                                 unsigned long lowValue,
                                                 unsigned long highValue ) noexcept
{
    ASSERT(argPtr != nullptr);
    WatchdogManager & watchdogManager = WatchdogManager::instance();
    const areg::os::Win32TimerHandle * h = static_cast<const areg::os::Win32TimerHandle *>(argPtr);
    const Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(h->contextId);
    const Watchdog::GUARD_ID    guardId     = Watchdog::make_guard_id(watchdog_id);
    Watchdog * watchdog = watchdogManager.mWatchdogResource.find_resource_object(guardId);
    if (watchdog != nullptr)
    {
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

} // namespace areg
#endif // _WIN32
