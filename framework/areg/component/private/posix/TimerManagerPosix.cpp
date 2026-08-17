/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/posix/TimerManagerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              Generic POSIX implementation: the deadlines are kept in the timers and
 *              watched by the manager loop (posix/TimerManagerBasePosix.cpp), so every
 *              expiry is processed on the manager thread. No OS timer object is created.
 *              Linux:  areg/component/private/linux/TimerManagerLinux.cpp
 *              macOS:  areg/component/private/macos/TimerManagerMacOS.cpp
 *
 ************************************************************************/

#if !defined(__linux__) && !defined(__APPLE__)
#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/Timer.hpp"

#include <vector>

namespace areg {

void TimerManager::_fire_expired(TIMERHANDLE handle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    ASSERT(posixTimer != nullptr);
    Timer * timer = mTimerResource.find_resource_object(handle);

    if ((timer != nullptr) && posixTimer->is_valid())
    {
        const timespec due{ posixTimer->due_time() };
        const uint32_t highValue = static_cast<uint32_t>(due.tv_sec);
        const uint32_t lowValue  = static_cast<uint32_t>(due.tv_nsec);

        posixTimer->timer_expired();
        _process_expired_timer(timer, handle, highValue, lowValue);
    }
}

bool TimerManager::_check_deadlines(const timespec & now, timespec & out_nextDue)
{
    // The resource lock is held across the walk so that nothing can be unregistered
    std::vector<TIMERHANDLE> expired;

    mTimerResource.lock();
    TIMERHANDLE handle{ nullptr };
    for (const Timer * timer = mTimerResource.resource_first_key(handle); timer != nullptr; timer = mTimerResource.resource_next_key(handle))
    {
        areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);

        timespec due{};
        if ((posixTimer != nullptr) && posixTimer->armed_due_time(due) && areg::os::deadline_reached(due, now))
        {
            expired.push_back(handle);
        }
    }
    mTimerResource.unlock();

    for (TIMERHANDLE expiredHandle : expired)
    {
        _fire_expired(expiredHandle);
    }

    bool hasNext{ false };

    mTimerResource.lock();
    handle = nullptr;
    for (const Timer* timer = mTimerResource.resource_first_key(handle); timer != nullptr; timer = mTimerResource.resource_next_key(handle))
    {
        areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);

        timespec due{};
        if ((posixTimer == nullptr) || !posixTimer->armed_due_time(due))
            continue;

        if (!hasNext || areg::os::deadline_earlier(due, out_nextDue))
        {
            out_nextDue = due;
            hasNext     = true;
        }
    }

    mTimerResource.unlock();
    return hasNext;
}

void TimerManager::_os_timer_stop(TIMERHANDLE timerHandle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(timerHandle);
    if (posixTimer != nullptr)
    {
        posixTimer->stop_timer();
    }
}

bool TimerManager::_os_timer_start(Timer & timer)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(timer.handle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime(CLOCK_REALTIME, &startTime);
    timer.timer_starting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    // Runs on the manager thread
    return posixTimer->start_timer(timer, 0, nullptr);
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
#endif  // !defined(__linux__) && !defined(__APPLE__)
