/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/linux/WatchdogManagerLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux-specific WatchdogManager implementation.
 *              timerfd + epoll watchdog start, stop, and expiration handling.
 *
 ************************************************************************/
#ifdef __linux__

#include "areg/component/private/WatchdogManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/private/Watchdog.hpp"
#include <sys/epoll.h>

namespace areg {

void WatchdogManager::_on_timerfd_expired(TIMERHANDLE handle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    ASSERT(posixTimer != nullptr);

    Watchdog::WATCHDOG_ID watchdog_id = static_cast<Watchdog::WATCHDOG_ID>(posixTimer->context_id());
    Watchdog::GUARD_ID    guardId     = Watchdog::make_guard_id(watchdog_id);
    Watchdog *            watchdog    = mWatchdogResource.find_resource_object(guardId);

    if (watchdog != nullptr)
    {
        const uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        const uint32_t lowValue  = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);

        // Watchdog timer is one-shot, remove from epoll before stopping
        const int epollFd = WatchdogManager::instance().mEpollFd;
        const int timerFd = posixTimer->timer_fd();
        if ((epollFd >= 0) && (timerFd >= 0))
        {
            ::epoll_ctl(epollFd, EPOLL_CTL_DEL, timerFd, nullptr);
        }

        posixTimer->stop_timer();
        _process_expired_timer(watchdog, watchdog_id, highValue, lowValue);
    }
}

void WatchdogManager::_os_timer_stop(TIMERHANDLE timerHandle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(timerHandle);
    if (posixTimer == nullptr)
        return;

    const int epollFd = WatchdogManager::instance().mEpollFd;
    const int timerFd = posixTimer->timer_fd();
    if ((epollFd >= 0) && (timerFd >= 0))
    {
        ::epoll_ctl(epollFd, EPOLL_CTL_DEL, timerFd, nullptr);
    }

    posixTimer->stop_timer();
}

bool WatchdogManager::_os_timer_start(Watchdog& watchdog)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(watchdog.handle());
    if (posixTimer == nullptr)
        return false;

    const Watchdog::WATCHDOG_ID watchdog_id = watchdog.watchdog_id();

    if (!posixTimer->start_timer(watchdog, watchdog_id, nullptr))
        return false;

    // Register with the watchdog manager's epoll loop
    const int epollFd = WatchdogManager::instance().mEpollFd;
    if (epollFd >= 0)
    {
        struct epoll_event ev{};
        ev.events   = EPOLLIN;
        ev.data.ptr = reinterpret_cast<void *>(watchdog.handle());
        ::epoll_ctl(epollFd, EPOLL_CTL_ADD, posixTimer->timer_fd(), &ev);
    }

    return true;
}

} // namespace areg

#endif  // __linux__
