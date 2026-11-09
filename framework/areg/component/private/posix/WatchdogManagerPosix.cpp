/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/posix/WatchdogManagerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Watchdog manager, generic POSIX implementation.
 *              Uses the same TimerPosix object and manager loop as the timer, see
 *              posix/TimerManagerPosix.cpp. No OS timer object is created.
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

namespace areg {

void WatchdogManager::_fire_expired(TIMERHANDLE handle)
{
    Watchdog * watchdog{ nullptr };
    Watchdog::WATCHDOG_ID watchdog_id{ 0u };
    uint32_t highValue{ 0u };
    uint32_t lowValue{ 0u };
    {
        // The watchdog may be destroyed after its timer was collected as expired.
        Lock resourceLock(mWatchdogLock);
        watchdog = _find_by_handle(handle);
        if (watchdog == nullptr)
            return;

        areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
        watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
        const timespec due{ posixTimer->due_time() };
        highValue = static_cast<uint32_t>(due.tv_sec);
        lowValue  = static_cast<uint32_t>(due.tv_nsec);

        // A watchdog guard is one-shot. Disarm it before it is processed
        posixTimer->stop_timer();
    }

    _process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
}

bool WatchdogManager::_check_deadlines(const timespec & now, timespec & out_nextDue)
{
    // Takes one expired watchdog at a time; a fired timer is no longer armed.
    for (;;)
    {
        TIMERHANDLE expired{ nullptr };
        do
        {
            Lock lock(mWatchdogLock);
            for (const Watchdog * watchdog = mWatchdogs; (watchdog != nullptr) && (expired == nullptr); watchdog = watchdog->mNextRegistered)
            {
                areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(watchdog->handle());

                timespec due{};
                if ((posixTimer != nullptr) && posixTimer->armed_due_time(due) && areg::os::deadline_reached(due, now))
                {
                    expired = watchdog->handle();
                }
            }
        } while (false);

        if (expired == nullptr)
            break;

        _fire_expired(expired);
    }

    bool hasNext{ false };

    Lock lock(mWatchdogLock);
    for (const Watchdog * watchdog = mWatchdogs; watchdog != nullptr; watchdog = watchdog->mNextRegistered)
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
