#ifndef AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
#define AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/posix/TimerPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SpinLockPosix.hpp"
#include <sys/types.h>
#include <time.h>

#ifdef __APPLE__
    #include <dispatch/dispatch.h>
#endif  // __APPLE__

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////
namespace areg {
    class TimerBase;
    class TimerManager;
    class WatchdogManager;
} // namespace areg

namespace areg::os {
    class TimerPosix;

#ifdef __APPLE__
/**
 * \brief   macOS callback type: receives a typed TimerPosix pointer (GCD path).
 */
typedef void (*FuncPosixTimerRoutine)(areg::os::TimerPosix* timerPtr);
#elif defined(__linux__)
/**
 * \brief   Linux callback type: unused on Linux (timerfd/epoll path, no callback needed).
 */
typedef void (*FuncPosixTimerRoutine)( void * );
#else   // Generic POSIX (Cygwin, FreeBSD, etc.)
/**
 * \brief   Generic POSIX callback type: unused, the manager owns the deadlines and fires
 *          them on its own thread. Kept so that the signatures stay common to all POSIX.
 */
typedef void (*FuncPosixTimerRoutine)( void * );

/**
 * \brief   Reads the clock the deadlines of this platform are expressed in. Monotonic, so
 *          a wall clock adjustment can neither delay nor duplicate an expiry.
 *
 * \param   out_time    On return, the current time.
 **/
void deadline_now( struct timespec & out_time ) noexcept;

/**
 * \brief   Milliseconds from \a now until \a due, rounded up so that a wait never returns
 *          before the deadline. Returns 0 when the due time has been reached already.
 *
 * \param   now         The current time, read with deadline_now().
 * \param   due         The due time to measure to.
 **/
[[nodiscard]]
uint32_t deadline_remaining_ms( const struct timespec & now, const struct timespec & due ) noexcept;

/**
 * \brief   Returns true if \a due has been reached at \a now.
 **/
[[nodiscard]]
bool deadline_reached( const struct timespec & due, const struct timespec & now ) noexcept;

/**
 * \brief   Returns true if \a first is earlier than \a second.
 **/
[[nodiscard]]
bool deadline_earlier( const struct timespec & first, const struct timespec & second ) noexcept;

#endif  // __APPLE__ / __linux__ / POSIX

//////////////////////////////////////////////////////////////////////////
// TimerPosix class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX-specific timer implementation for handling timer callbacks and state management.
 **/
class TimerPosix
{
//////////////////////////////////////////////////////////////////////////
// Friend class and constants
//////////////////////////////////////////////////////////////////////////
    friend class areg::TimerManager;
    friend class areg::WatchdogManager;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    TimerPosix();

    ~TimerPosix();

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations.
//////////////////////////////////////////////////////////////////////////
public:

#ifdef __linux__
    /**
     * \brief   Returns the Linux timerfd file descriptor, or -1 if not created.
     **/
    inline int timer_fd() const noexcept;
#endif  // __linux__

    /**
     * \brief   Returns the timer context pointer.
     **/
    inline void * context() const noexcept;

    /**
     * \brief   Returns the timer context identifier.
     **/
    inline id_type context_id() const noexcept;

    /**
     * \brief   Returns the due date and time for the next timer expiration.
     **/
    inline const timespec & due_time() const noexcept;

#if !defined(__linux__) && !defined(__APPLE__)
    /**
     * \brief   Reports whether the timer is armed and, when it is, the due time to watch.
     *          Both are read under one lock, so the caller can never see an armed flag that
     *          belongs to a different due time.
     *
     * \param   out_dueTime     On return, the due time. Untouched when the timer is not armed.
     * \return  Returns true when the timer is armed.
     **/
    [[nodiscard]]
    inline bool armed_due_time( struct timespec & out_dueTime ) const noexcept;
#endif  // generic POSIX

    /**
     * \brief   Returns true if the timer is valid (has valid ID and context).
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Creates the timer without starting it.
     *
     * \param   funcTimer       The callback function to execute on timer expiration.
     * \return  Returns true if timer creation succeeded; false otherwise.
     **/
    bool create_timer( FuncPosixTimerRoutine funcTimer ) noexcept;

    /**
     * \brief   Creates and starts the timer with the timeout and period from the given context.
     *
     * \param   context         The timer object containing timeout and period values.
     * \param   contextId       The context identifier to associate with the timer.
     * \param   funcTimer       The callback function to execute on timer expiration.
     * \return  Returns true if timer creation and start succeeded; false otherwise.
     **/
    bool start_timer( TimerBase & context, id_type contextId, FuncPosixTimerRoutine funcTimer ) noexcept;

    /**
     * \brief   Restarts the timer if timeout and period are not zero.
     *
     * \return  Returns true if timer restart succeeded; false otherwise.
     **/
    bool restart_timer() noexcept;

    /**
     * \brief   Stops the timer and resets timeout and period values.
     **/
    bool stop_timer() noexcept;

    /**
     * \brief   Pauses the timer while preserving timeout and remaining period.
     **/
    bool pause_timer() noexcept;

    /**
     * \brief   Destroys and invalidates the timer.
     **/
    void destroy_timer() noexcept;

    /**
     * \brief   Called by the timer manager when the timer expires; returns true if timer can
     *          continue running.
     *
     * \param   timeoutMs       The timeout in milliseconds when timer expired.
     * \return  Returns true if timer is periodic and period count is greater than zero; false if
     *          timer should stop.
     **/
    void timer_expired() noexcept;

//////////////////////////////////////////////////////////////////////////
// Internal private methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Internal method to create and initialize the timer.
     *
     * \param   funcTimer       The callback function to execute on timer expiration.
     * \return  Returns true if timer creation succeeded; false otherwise.
     **/
    bool _create_timer( FuncPosixTimerRoutine funcTimer ) noexcept;

    /**
     * \brief   Internal method to start the timer.
     *
     * \return  Returns true if timer start succeeded; false otherwise.
     **/
    bool _start_timer() noexcept;

    /**
     * \brief   Internal method to stop the timer.
     **/
    void _stop_timer() noexcept;

    /**
     * \brief   Internal method to destroy the timer.
     **/
    void _destroy_timer() noexcept;

    /**
     * \brief   Returns true if the timer is currently started (due time is not zero).
     **/
    inline bool _is_started() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
#ifdef __APPLE__
    /**
     * \brief   GCD dispatch timer source for macOS.
     */
    dispatch_source_t       mTimerSource;
    /**
     * \brief   GCD dispatch queue for timer.
     */
    dispatch_queue_t        mTimerQueue;
    /**
     * \brief   The callback function to call when timer expires.
     */
    FuncPosixTimerRoutine   mTimerCallback;
#elif defined(__linux__)
    /**
     * \brief   Linux timerfd file descriptor (-1 when not created).
     */
    int                     mTimerFd;
#else   // Generic POSIX (Cygwin, FreeBSD, etc.)
    /**
     * \brief   True while the timer is armed, i.e. while mDueTime carries a deadline the
     *          manager loop has to watch. There is no OS timer object on this platform.
     */
    bool                    mArmed;
#endif  // __APPLE__ / __linux__ / POSIX

    /**
     * \brief   The context pointer passed to POSIX timer, set when using Timer object.
     *          Otherwise, should be nullptr.
     */
    TimerBase *             mContext;

    /**
     * \brief   The context ID passed to POSIX timer, set when using Watchdog object.
     *          Otherwise, should be zero.
     **/
    id_type                 mContextId;

    /**
     * \brief   The timer timeout information.
     */
    struct timespec         mDueTime;

    /**
     * \brief   Synchronization object.
     */
    mutable SpinLockPosix  mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( TimerPosix );
};

//////////////////////////////////////////////////////////////////////////
// TimerPosix class inline methods
//////////////////////////////////////////////////////////////////////////

#ifdef __linux__
inline int TimerPosix::timer_fd() const noexcept
{
    SpinAutolockPosix lock(mLock);
    return mTimerFd;
}
#endif  // __linux__

inline void * TimerPosix::context() const noexcept
{
	SpinAutolockPosix lock(mLock);
    return mContext;
}

inline id_type TimerPosix::context_id() const noexcept
{
    SpinAutolockPosix lock(mLock);
    return mContextId;
}

inline const timespec & TimerPosix::due_time() const noexcept
{
	SpinAutolockPosix lock(mLock);
    return mDueTime;
}

inline bool TimerPosix::is_valid() const noexcept
{
    SpinAutolockPosix lock(mLock);
#ifdef __APPLE__
    return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerQueue != nullptr));
#elif defined(__linux__)
    return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerFd >= 0));
#else   // Generic POSIX
    // There is no OS timer object to check: a timer with a context is usable as it is.
    return ((mContext != nullptr) || (mContextId != 0u));
#endif  // __APPLE__ / __linux__ / POSIX
}

#if !defined(__linux__) && !defined(__APPLE__)
inline bool TimerPosix::armed_due_time( struct timespec & out_dueTime ) const noexcept
{
    SpinAutolockPosix lock(mLock);
    if (mArmed)
    {
        out_dueTime = mDueTime;
    }

    return mArmed;
}
#endif  // generic POSIX

inline bool TimerPosix::_is_started() const noexcept
{
#if !defined(__linux__) && !defined(__APPLE__)
    // The due time is an absolute point on a monotonic clock here, so 'not zero' is not a
    // statement about being armed. The flag is.
    return mArmed;
#else   // !generic POSIX
    return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));
#endif  // generic POSIX
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
