/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/macos/WatchdogManagerMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS-specific WatchdogManager implementation.
 *              GCD dispatch_source watchdog start, stop, and expiration handling.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/component/private/WatchdogManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/private/Watchdog.hpp"
#include <time.h>

namespace areg {

void WatchdogManager::_posix_watchdog_expired(areg::os::TimerPosix* posixTimer) noexcept
{
    WatchdogManager& watchdogManager = WatchdogManager::instance();
    ASSERT(posixTimer != nullptr);
    Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
    Watchdog::GUARD_ID    guardId     = Watchdog::make_guard_id(watchdog_id);
    Watchdog*             watchdog    = watchdogManager.mWatchdogResource.find_resource_object(guardId);

    if (watchdog != nullptr)
    {
        uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue  = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->stop_timer();
        watchdogManager._process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

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

} // namespace areg

#endif  // __APPLE__
