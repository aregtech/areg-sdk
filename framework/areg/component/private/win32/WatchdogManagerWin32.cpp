/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManagerWin.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Thread Watchdog Manager.
 *              Controlling, triggering and stopping timer to check threads.
 *              Windows OS specific calls.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#ifdef  _WIN32

#include "areg/component/private/Watchdog.hpp"
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

void WatchdogManager::_os_timer_stop(TIMERHANDLE handle)
{

    if (handle != nullptr)
    {
        ::CancelWaitableTimer(handle);
    }
}

bool WatchdogManager::_os_timer_start(Watchdog& watchdog)
{
    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    long period = 0;
    int64_t due_time = static_cast<int64_t>(static_cast<TIME64>(watchdog.timeout()) * areg::MILLISEC_TO_100NS);  // timer from now
    due_time *= static_cast<int64_t>(-1);
    LARGE_INTEGER timeTrigger{ };
    timeTrigger.LowPart  = static_cast<DWORD>(areg::lo_dword(due_time));
    timeTrigger.HighPart = static_cast<LONG >(areg::hi_dword(due_time));

    return (::SetWaitableTimer(   watchdog.handle()
                                , &timeTrigger
                                , period
                                , (PTIMERAPCROUTINE)(&WatchdogManager::_windows_watchdog_expired)
                                , reinterpret_cast<void*>(watchdog.make_watchdog_id(watchdog.id(), watchdog.sequence())), FALSE) == TRUE);
}

/**
 * \brief   Windows OS timer routine function. Triggered, when one of timer is expired.
 * \param   argPtr      The pointer of argument passed to timer expired callback function
 * \param   lowValue    The low value of timer expiration
 * \param   highValue   The high value of timer expiration.
 **/
void WatchdogManager::_windows_watchdog_expired(void* argPtr, unsigned long lowValue, unsigned long highValue)
{
    ASSERT(argPtr != nullptr);
    WatchdogManager& watchdogManager = WatchdogManager::instance();
    Watchdog::WATCHDOG_ID watchdog_id = reinterpret_cast<Watchdog::WATCHDOG_ID>(argPtr);
    Watchdog::GUARD_ID guardId = Watchdog::make_guard_id(watchdog_id);
    Watchdog* watchdog = watchdogManager.mWatchdogResource.find_resource_object(guardId);
    if (watchdog != nullptr)
    {
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

} // namespace areg
#endif // _WIN32
