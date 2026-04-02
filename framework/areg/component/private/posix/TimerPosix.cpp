/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX specific timer information.
 *              Common methods shared by all POSIX platforms, plus generic POSIX
 *              (non-Linux, non-Apple) private helpers using timer_create + SIGEV_THREAD.
 *              Linux-specific (timerfd):  linux/TimerPosixLinux.cpp
 *              macOS-specific (GCD):      macos/TimerPosixMacOS.cpp
 *
 ************************************************************************/
#include "areg/component/private/posix/TimerPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/TimerBase.hpp"
#include "areg/component/private/Watchdog.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/MemoryDefs.hpp"

//////////////////////////////////////////////////////////////////////////
// Generic POSIX private helpers: _create_timer, _start_timer, _stop_timer,
// _destroy_timer — using POSIX timer_create + SIGEV_THREAD.
// Covers Cygwin, FreeBSD, and any other non-Linux non-Apple POSIX platform.
//////////////////////////////////////////////////////////////////////////

#if !defined(__linux__) && !defined(__APPLE__)

#include <signal.h>

namespace areg::os {

void _posix_timer_expired_cb(union sigval si)
{
    TimerPosix * timer = reinterpret_cast<TimerPosix *>(si.sival_ptr);
    if (timer != nullptr)
    {
        timer->timer_expired();
        if (timer->mTimerCallback != nullptr)
        {
            timer->mTimerCallback(static_cast<void *>(timer));
        }
    }
}

bool TimerPosix::_create_timer(FuncPosixTimerRoutine funcTimer) noexcept
{
    if (mTimerId != static_cast<timer_t>(0))
        return true;    // already created

    struct sigevent sigEvent;
    areg::mem_zero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify            = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr   = static_cast<void *>(this);
    sigEvent.sigev_notify_function   = &_posix_timer_expired_cb;
    sigEvent.sigev_notify_attributes = nullptr;

    if (areg::RETURNED_OK != ::timer_create(CLOCK_MONOTONIC, &sigEvent, &mTimerId))
    {
        mTimerId = static_cast<timer_t>(0);
        return false;
    }

    mTimerCallback = funcTimer;
    return true;
}

bool TimerPosix::_start_timer() noexcept
{
    if ((mTimerId == static_cast<timer_t>(0)) || (mContext == nullptr))
        return false;

    areg::os::conv_timeout(mDueTime, mContext->timeout());

    struct itimerspec spec;
    areg::mem_zero(static_cast<void *>(&spec), sizeof(spec));
    spec.it_value.tv_sec  = mDueTime.tv_sec;
    spec.it_value.tv_nsec = mDueTime.tv_nsec;

    if (mContext->event_count() > TimerBase::ONE_TIME)
    {
        spec.it_interval.tv_sec  = mDueTime.tv_sec;
        spec.it_interval.tv_nsec = mDueTime.tv_nsec;
    }

    return (areg::RETURNED_OK == ::timer_settime(mTimerId, 0, &spec, nullptr));
}

void TimerPosix::_stop_timer() noexcept
{
    if (mTimerId != static_cast<timer_t>(0))
    {
        struct itimerspec cancelSpec;
        areg::mem_zero(static_cast<void *>(&cancelSpec), sizeof(cancelSpec));
        ::timer_settime(mTimerId, 0, &cancelSpec, nullptr);
    }

    mDueTime.tv_sec  = 0;
    mDueTime.tv_nsec = 0;
}

void TimerPosix::_destroy_timer() noexcept
{
    if (mTimerId != static_cast<timer_t>(0))
    {
        _stop_timer();
        ::timer_delete(mTimerId);
        mTimerId       = static_cast<timer_t>(0);
        mTimerCallback = nullptr;
    }
}

} // namespace areg::os

#endif  // !defined(__linux__) && !defined(__APPLE__)

//////////////////////////////////////////////////////////////////////////
// Common POSIX: public methods shared by all platforms
//////////////////////////////////////////////////////////////////////////

namespace areg::os {

TimerPosix::TimerPosix()
#ifdef __APPLE__
    : mTimerSource  ( nullptr )
    , mTimerQueue   ( nullptr )
    , mTimerCallback( nullptr )
#elif defined(__linux__)
    : mTimerFd      ( -1      )
#else   // Generic POSIX
    : mTimerId      ( static_cast<timer_t>(0) )
    , mTimerCallback( nullptr )
#endif  // __APPLE__ / __linux__ / POSIX
    , mContext      ( nullptr )
    , mContextId    ( 0u      )
    , mDueTime      (         )
    , mLock         (         )
{
}

TimerPosix::~TimerPosix()
{
    SpinAutolockPosix lock(mLock);
    _destroy_timer();
}

bool TimerPosix::create_timer(FuncPosixTimerRoutine funcTimer) noexcept
{
    SpinAutolockPosix lock(mLock);
#ifdef __APPLE__
    return ((mTimerQueue != nullptr) && (mTimerCallback != nullptr)) ||
           ((funcTimer != nullptr) && _create_timer(funcTimer));
#elif defined(__linux__)
    // funcTimer is unused on Linux — timerfd is polled via epoll, no callback needed.
    return (mTimerFd >= 0) || _create_timer(funcTimer);
#else   // Generic POSIX
    return (mTimerId != static_cast<timer_t>(0)) ||
           ((funcTimer != nullptr) && _create_timer(funcTimer));
#endif  // __APPLE__ / __linux__ / POSIX
}

bool TimerPosix::start_timer(TimerBase& context, id_type contextId, FuncPosixTimerRoutine funcTimer) noexcept
{
    SpinAutolockPosix lock(mLock);

    mContext   = &context;
    mContextId = contextId;

#ifdef __APPLE__
    if ((mTimerQueue == nullptr) && (funcTimer != nullptr))
    {
        _create_timer(funcTimer);
    }

    return ((mTimerQueue != nullptr) && (mTimerCallback != nullptr) && _start_timer());
#elif defined(__linux__)
    if (mTimerFd < 0)
    {
        _create_timer(nullptr);
    }

    return (mTimerFd >= 0) && _start_timer();
#else   // Generic POSIX
    if ((mTimerId == static_cast<timer_t>(0)) && (funcTimer != nullptr))
    {
        _create_timer(funcTimer);
    }

    return (mTimerId != static_cast<timer_t>(0)) && _start_timer();
#endif  // __APPLE__ / __linux__ / POSIX
}

bool TimerPosix::restart_timer() noexcept
{
    SpinAutolockPosix lock(mLock);
    return _start_timer();
}

bool TimerPosix::pause_timer() noexcept
{
    SpinAutolockPosix lock(mLock);

    if (_is_started())
    {
        _stop_timer();
    }

#ifdef __APPLE__
    return (mTimerQueue != nullptr);
#elif defined(__linux__)
    return (mTimerFd >= 0);
#else   // Generic POSIX
    return (mTimerId != static_cast<timer_t>(0));
#endif  // __APPLE__ / __linux__ / POSIX
}

bool TimerPosix::stop_timer() noexcept
{
    SpinAutolockPosix lock(mLock);

    if (_is_started())
    {
        _stop_timer();
    }

#ifdef __APPLE__
    return (mTimerQueue != nullptr);
#elif defined(__linux__)
    return (mTimerFd >= 0);
#else   // Generic POSIX
    return (mTimerId != static_cast<timer_t>(0));
#endif  // __APPLE__ / __linux__ / POSIX
}

void TimerPosix::destroy_timer() noexcept
{
    SpinAutolockPosix lock(mLock);

    _destroy_timer();

    mContext   = nullptr;
    mContextId = 0u;
}

void TimerPosix::timer_expired() noexcept
{
    SpinAutolockPosix lock(mLock);
    if (mContext == nullptr)
        return;

    if (mContext->event_count() > TimerBase::ONE_TIME)
    {
        areg::os::conv_timeout(mDueTime, mContext->timeout());
    }
    else if (_is_started())
    {
        _stop_timer();
    }
}

} // namespace areg::os

#endif // defined(_POSIX) || defined(POSIX)
