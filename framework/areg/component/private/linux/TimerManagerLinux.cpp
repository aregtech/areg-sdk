/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/linux/TimerManagerLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux-specific TimerManager implementation.
 *              timerfd + epoll timer start, stop, and expiration handling.
 *
 ************************************************************************/
#ifdef __linux__

#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/Timer.hpp"
#include <sys/epoll.h>

namespace areg {

void TimerManager::_on_timerfd_expired(TIMERHANDLE handle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    ASSERT(posixTimer != nullptr);
    Timer * timer = mTimerResource.find_resource_object(handle);

    if ((timer != nullptr) && posixTimer->is_valid())
    {
        const uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        const uint32_t lowValue  = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timer_expired();
        _process_expired_timer(timer, handle, highValue, lowValue);
    }
}

void TimerManager::_os_timer_stop(TIMERHANDLE timerHandle)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(timerHandle);
    if (posixTimer == nullptr)
        return;

    const int epollFd = TimerManager::instance().mEpollFd;
    const int timerFd = posixTimer->timer_fd();
    if ((epollFd >= 0) && (timerFd >= 0))
    {
        ::epoll_ctl(epollFd, EPOLL_CTL_DEL, timerFd, nullptr);
    }

    posixTimer->stop_timer();
}

bool TimerManager::_os_timer_start(Timer& timer)
{
    areg::os::TimerPosix * posixTimer = reinterpret_cast<areg::os::TimerPosix *>(timer.handle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime(CLOCK_REALTIME, &startTime);
    timer.timer_starting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    if (!posixTimer->start_timer(timer, 0, nullptr))
        return false;

    // Register the timerfd with the manager's epoll so the run_dispatcher loop
    // fires _on_timerfd_expired(handle) when the timer expires
    const int epollFd = TimerManager::instance().mEpollFd;
    if (epollFd >= 0)
    {
        struct epoll_event ev{};
        ev.events   = EPOLLIN;
        ev.data.ptr = reinterpret_cast<void *>(timer.handle());
        ::epoll_ctl(epollFd, EPOLL_CTL_ADD, posixTimer->timer_fd(), &ev);
    }

    return true;
}

} // namespace areg

#endif  // __linux__
