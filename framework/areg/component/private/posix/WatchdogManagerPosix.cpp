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
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

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

namespace areg {

//////////////////////////////////////////////////////////////////////////
// POSIX specific methods
//////////////////////////////////////////////////////////////////////////

void WatchdogManager::_os_timer_stop(TIMERHANDLE handle)
{
    areg::os::TimerPosix* posixTimer = reinterpret_cast<areg::os::TimerPosix*>(handle);
    if (posixTimer != nullptr)
    {
        posixTimer->stop_timer();
    }
}

bool WatchdogManager::_os_timer_start(Watchdog& watchdog)
{
    bool result = false;
    areg::os::TimerPosix* posixTimer = reinterpret_cast<areg::os::TimerPosix*>(watchdog.handle());
    if (posixTimer != nullptr)
    {
        Watchdog::WATCHDOG_ID watchdog_id = watchdog.watchdog_id();
        if (posixTimer->start_timer(watchdog, watchdog_id, &WatchdogManager::_posix_watchdog_expired))
        {
            result = true;
        }
    }

    return result;
}

#ifdef __APPLE__
void WatchdogManager::_posix_watchdog_expired(areg::os::TimerPosix* posixTimer)
{
    WatchdogManager& watchdogManager = WatchdogManager::instance();
    ASSERT(posixTimer != nullptr);
    Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
    Watchdog::GUARD_ID guardId  = Watchdog::make_guard_id(watchdog_id);
    Watchdog* watchdog          = watchdogManager.mWatchdogResource.find_resource_object(guardId);

    if (watchdog != nullptr)
    {
        uint32_t highValue 	= static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue  	= static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->stop_timer();
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}
#else   // !__APPLE__
void WatchdogManager::_posix_watchdog_expired(signal_value argSig)
{
    WatchdogManager& watchdogManager = WatchdogManager::instance();
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(argSig.sival_ptr);
    ASSERT(posixTimer != nullptr);
    Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
    Watchdog::GUARD_ID guardId  = Watchdog::make_guard_id(watchdog_id);
    Watchdog* watchdog          = watchdogManager.mWatchdogResource.find_resource_object(guardId);

    if (watchdog != nullptr)
    {
        uint32_t highValue 	= static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue  	= static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->stop_timer();
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}
#endif  // __APPLE__

} // namespace areg
#endif  // defined(_POSIX) || defined(POSIX)
