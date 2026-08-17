/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/posix/WatchdogManagerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Watchdog manager, generic POSIX implementation.
 *              The watchdog uses the same TimerPosix object and the same manager loop as
 *              the timer, so it is kept in step with posix/TimerManagerPosix.cpp: the
 *              deadline lives in the watchdog timer, the loop watches it, and the expiry
 *              is processed on the watchdog manager thread. No OS timer object is created.
 *              Linux:  areg/component/private/linux/WatchdogManagerLinux.cpp
 *              macOS:  areg/component/private/macos/WatchdogManagerMacOS.cpp
 *
 ************************************************************************/

#if !defined(__linux__) && !defined(__APPLE__)
#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/private/Watchdog.hpp"

#include <vector>

namespace areg {

void WatchdogManager::_fire_expired(TIMERHANDLE handle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    ASSERT(posixTimer != nullptr);

    Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
    Watchdog::GUARD_ID    guardId     = Watchdog::make_guard_id(watchdog_id);
    Watchdog *            watchdog    = mWatchdogResource.find_resource_object(guardId);

    if (watchdog != nullptr)
    {
        const timespec due{ posixTimer->due_time() };
        const uint32_t highValue = static_cast<uint32_t>(due.tv_sec);
        const uint32_t lowValue  = static_cast<uint32_t>(due.tv_nsec);

        // A watchdog guard is one-shot. Disarm it before it is processed
        posixTimer->stop_timer();
        _process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

bool WatchdogManager::_check_deadlines(const timespec & now, timespec & out_nextDue)
{
    std::vector<TIMERHANDLE> expired;

    mWatchdogResource.lock();
    Watchdog::GUARD_ID guardId{ 0 };
    for (const Watchdog * watchdog = mWatchdogResource.resource_first_key(guardId); watchdog != nullptr; watchdog = mWatchdogResource.resource_next_key(guardId))
    {
        TIMERHANDLE handle{ watchdog->handle() };
        areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);

        timespec due{};
        if ((posixTimer != nullptr) && posixTimer->armed_due_time(due) && areg::os::deadline_reached(due, now))
        {
            expired.push_back(handle);
        }
    }
    mWatchdogResource.unlock();

    for (TIMERHANDLE expiredHandle : expired)
    {
        _fire_expired(expiredHandle);
    }

    bool hasNext{ false };

    mWatchdogResource.lock();
    guardId = 0;
    for (const Watchdog * watchdog = mWatchdogResource.resource_first_key(guardId); watchdog != nullptr; watchdog = mWatchdogResource.resource_next_key(guardId))
    {
        areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(watchdog->handle());

        timespec due{};
        if ((posixTimer == nullptr) || !posixTimer->armed_due_time(due))
            continue;

        if (!hasNext || areg::os::deadline_earlier(due, out_nextDue))
        {
            out_nextDue = due;
            hasNext     = true;
        }
    }
    mWatchdogResource.unlock();

    return hasNext;
}

void WatchdogManager::_os_timer_stop(TIMERHANDLE handle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    if (posixTimer != nullptr)
    {
        posixTimer->stop_timer();
    }
}

bool WatchdogManager::_os_timer_start(Watchdog & watchdog)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(watchdog.handle());
    if (posixTimer != nullptr)
    {
        // Runs on the watchdog manager thread
        const Watchdog::WATCHDOG_ID watchdog_id = watchdog.watchdog_id();
        return posixTimer->start_timer(watchdog, watchdog_id, nullptr);
    }

    return false;
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
#endif  // !defined(__linux__) && !defined(__APPLE__)
