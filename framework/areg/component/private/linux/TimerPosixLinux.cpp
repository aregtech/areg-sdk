/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/linux/TimerPosixLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Linux-specific TimerPosix private methods.
 *              Uses timerfd for non-blocking timer management.
 *
 ************************************************************************/
#ifdef __linux__

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/TimerBase.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include <sys/timerfd.h>
#include <unistd.h>

namespace areg::os {

bool TimerPosix::_create_timer(FuncPosixTimerRoutine /*funcTimer*/) noexcept
{
    mTimerFd = ::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    return (mTimerFd >= 0);
}

bool TimerPosix::_start_timer() noexcept
{
    bool result = false;

    if ((mTimerFd >= 0) && (mContext != nullptr))
    {
        if (_is_started())
        {
            _stop_timer();
        }

        uint32_t msTimeout  = mContext->timeout();
        uint32_t eventCount = mContext->event_count();

        if ((msTimeout != 0) && (eventCount != 0))
        {
            struct itimerspec interval;
            areg::mem_zero(static_cast<void *>(&interval), sizeof(struct itimerspec));
            areg::os::conv_timeout(interval.it_value, msTimeout);
            if (eventCount > 1)
            {
                interval.it_interval.tv_sec  = interval.it_value.tv_sec;
                interval.it_interval.tv_nsec = interval.it_value.tv_nsec;
            }

            if (areg::RETURNED_OK == ::clock_gettime(CLOCK_REALTIME, &mDueTime))
            {
                areg::os::conv_timeout(mDueTime, msTimeout);
                result = true;

                if (areg::RETURNED_OK != ::timerfd_settime(mTimerFd, 0, &interval, nullptr))
                {
                    result           = false;
                    mDueTime.tv_sec  = 0;
                    mDueTime.tv_nsec = 0;
                    mContextId       = 0;
                }
            }
        }
    }

    return result;
}

void TimerPosix::_stop_timer() noexcept
{
    ASSERT(mTimerFd >= 0);

    struct itimerspec zero;
    areg::mem_zero(static_cast<void *>(&zero), sizeof(struct itimerspec));

    mDueTime.tv_sec  = 0;
    mDueTime.tv_nsec = 0;
    ::timerfd_settime(mTimerFd, 0, &zero, nullptr);
}

void TimerPosix::_destroy_timer() noexcept
{
    if (_is_started())
    {
        _stop_timer();
    }

    if (mTimerFd >= 0)
    {
        ::close(mTimerFd);
        mTimerFd = -1;
    }
}

} // namespace areg::os

#endif  // __linux__
