/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/component/TimerBase.hpp"
#include "areg/component/private/Watchdog.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"

//////////////////////////////////////////////////////////////////////////
// Generic POSIX private helpers: _create_timer, _start_timer, _stop_timer,
// _destroy_timer -- a deadline, no OS timer object.
// Covers Cygwin, FreeBSD, and any other non-Linux non-Apple POSIX platform.
//
// This used to be a 'timer_create' + 'SIGEV_THREAD' backend. The OS then ran the
// expiry on a thread it created and destroyed itself, and that thread took this
// object's lock, the manager's resource lock, the timer lock and the event queue of
// the owning dispatcher, and allocated memory on the way. A thread destroyed inside
// any of those regions leaves the lock owned by a thread that no longer exists: on
// Cygwin that hung 'Application::release()' in 'TimerManager::wait_timer_manager()',
// with the manager thread blocked on the first line of TimerPosix::stop_timer().
//
// The deadline is now kept here and watched by the manager loop -- see
// posix/TimerManagerBasePosix.cpp -- so every one of those locks is taken only by a
// thread areg creates and stops. Nothing here calls into the OS while holding a lock,
// because nothing here calls into the OS at all.
//////////////////////////////////////////////////////////////////////////

#if !defined(__linux__) && !defined(__APPLE__)

namespace areg::os {

void deadline_now(struct timespec & out_time) noexcept
{
    if (areg::RETURNED_OK != ::clock_gettime(CLOCK_MONOTONIC, &out_time))
    {
        out_time.tv_sec  = 0;
        out_time.tv_nsec = 0;
    }
}

bool deadline_earlier(const struct timespec & first, const struct timespec & second) noexcept
{
    return (first.tv_sec != second.tv_sec) ? (first.tv_sec < second.tv_sec)
                                           : (first.tv_nsec < second.tv_nsec);
}

bool deadline_reached(const struct timespec & due, const struct timespec & now) noexcept
{
    return !deadline_earlier(now, due);
}

uint32_t deadline_remaining_ms(const struct timespec & now, const struct timespec & due) noexcept
{
    if (deadline_reached(due, now))
        return 0u;

    constexpr int64_t NS_PER_SEC { static_cast<int64_t>(areg::SEC_TO_NS) };
    constexpr int64_t NS_PER_MS  { static_cast<int64_t>(areg::MILLISEC_TO_NS) };

    const int64_t ns{ (static_cast<int64_t>(due.tv_sec)  - static_cast<int64_t>(now.tv_sec)) * NS_PER_SEC +
                      (static_cast<int64_t>(due.tv_nsec) - static_cast<int64_t>(now.tv_nsec)) };

    const int64_t ms{ (ns + NS_PER_MS - 1) / NS_PER_MS };
    if (ms <= 0)
        return 1u;

    return ms < static_cast<int64_t>(areg::WAIT_INFINITE) ? static_cast<uint32_t>(ms)
                                                          : (areg::WAIT_INFINITE - 1u);
}

bool TimerPosix::_create_timer(FuncPosixTimerRoutine /* funcTimer */) noexcept
{
    return true;    // Nothing to create
}

bool TimerPosix::_start_timer() noexcept
{
    if (mContext == nullptr)
        return false;

    // An absolute point on the monotonic clock
    areg::os::deadline_now(mDueTime);
    areg::os::conv_timeout(mDueTime, mContext->timeout());
    mArmed = true;

    return true;
}

void TimerPosix::_stop_timer() noexcept
{
    mArmed           = false;
    mDueTime.tv_sec  = 0;
    mDueTime.tv_nsec = 0;
}

void TimerPosix::_destroy_timer() noexcept
{
    _stop_timer();
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
    : mArmed        ( false   )
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
    // funcTimer is unused on Linux -- timerfd is polled via epoll, no callback needed.
    return (mTimerFd >= 0) || _create_timer(funcTimer);
#else   // Generic POSIX
    // funcTimer is unused: the manager loop owns the deadlines and fires them itself.
    return _create_timer(funcTimer);
#endif  // __APPLE__ / __linux__ / POSIX
}

bool TimerPosix::start_timer(TimerBase& context, id_type contextId, [[maybe_unused]] FuncPosixTimerRoutine funcTimer) noexcept
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
    return _start_timer();
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
    // No OS timer object: the timer stays usable and can be armed again.
    return true;
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
    // No OS timer object: the timer stays usable and can be armed again.
    return true;
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
