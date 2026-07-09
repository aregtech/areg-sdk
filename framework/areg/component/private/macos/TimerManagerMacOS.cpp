/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/macos/TimerManagerMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS-specific TimerManager implementation.
 *              GCD dispatch_source timer start, stop, and expiration handling.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/Timer.hpp"
#include <time.h>

namespace areg {

void TimerManager::_posix_timer_expired(areg::os::TimerPosix* posixTimer)
{
    TimerManager& timerManager = TimerManager::instance();
    ASSERT(posixTimer != nullptr);
    Timer* timer = timerManager.mTimerResource.find_resource_object(reinterpret_cast<TIMERHANDLE>(posixTimer));

    if ((timer != nullptr) && posixTimer->is_valid())
    {
        uint32_t highValue = static_cast<uint32_t>(posixTimer->mDueTime.tv_sec);
        uint32_t lowValue  = static_cast<uint32_t>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timer_expired();
        timerManager._process_expired_timer(timer, reinterpret_cast<TIMERHANDLE>(posixTimer), highValue, lowValue);
    }
}

void TimerManager::_os_timer_stop(TIMERHANDLE timerHandle)
{
    areg::os::TimerPosix* posixTimer = reinterpret_cast<areg::os::TimerPosix*>(timerHandle);
    if (posixTimer != nullptr)
    {
        posixTimer->stop_timer();
    }
}

bool TimerManager::_os_timer_start(Timer& timer)
{
    bool result{ false };
    areg::os::TimerPosix* posixTimer = reinterpret_cast<areg::os::TimerPosix*>(timer.handle());
    ASSERT(posixTimer != nullptr);

    struct timespec startTime;
    ::clock_gettime(CLOCK_REALTIME, &startTime);
    timer.timer_starting(startTime.tv_sec, startTime.tv_nsec, reinterpret_cast<ptr_type>(posixTimer));

    if (posixTimer->start_timer(timer, 0, &TimerManager::_posix_timer_expired))
    {
        result = true;
    }

    return result;
}

} // namespace areg

#endif  // __APPLE__
