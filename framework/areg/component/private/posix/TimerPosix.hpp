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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

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
class TimerBase;
class TimerPosix;

#ifdef __APPLE__
/**
 * \brief   The macOS timer callback function type triggered when timer expires.
 *          The callback receives the pointer to TimerPosix object.
 * \param   timerPtr    Pointer to the TimerPosix object that expired.
 */
typedef void (*FuncPosixTimerRoutine)(TimerPosix* timerPtr);
#else   // !__APPLE__
/**
 * \brief   The POSIX timer routing method triggered in a separate thread.
 */
typedef void (*FuncPosixTimerRoutine)( union sigval );
#endif  // __APPLE__

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
    friend class TimerManager;
    friend class WatchdogManager;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates and initializes the POSIX timer object.
     *
     * \param   context         The timer context; if null, the timer is invalid.
     * \param   timerRoutine    The callback function to execute on timer expiration.
     * \param   msTimeout       The timeout in milliseconds.
     * \param   period          The number of times to trigger; 1 for single shot, >1 for limited
     *                          repetitions, TIMER_PERIOD_ENDLESS for continuous.
     **/
    TimerPosix();

    /**
     * \brief   Destructor.
     **/
    ~TimerPosix();

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations.
//////////////////////////////////////////////////////////////////////////
public:

#ifndef __APPLE__
    /**
     * \brief   Returns the POSIX timer identifier.
     **/
    inline timer_t timer_id() const;
#endif  // !__APPLE__

    /**
     * \brief   Returns the timer context pointer.
     **/
    inline void * context() const;

    /**
     * \brief   Returns the timer context identifier.
     **/
    inline id_type context_id() const;

    /**
     * \brief   Returns the due date and time for the next timer expiration.
     **/
    inline const timespec & due_time() const;


    /**
     * \brief   Returns true if the timer is valid (has valid ID and context).
     **/
    inline bool is_valid() const;

    /**
     * \brief   Creates the timer without starting it.
     *
     * \param   funcTimer       The callback function to execute on timer expiration.
     * \return  Returns true if timer creation succeeded; false otherwise.
     **/
    bool create_timer( FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Creates and starts the timer with the timeout and period from the given context.
     *
     * \param   context         The timer object containing timeout and period values.
     * \param   contextId       The context identifier to associate with the timer.
     * \param   funcTimer       The callback function to execute on timer expiration.
     * \return  Returns true if timer creation and start succeeded; false otherwise.
     **/
    bool start_timer( TimerBase & context, id_type contextId, FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Restarts the timer if timeout and period are not zero.
     *
     * \return  Returns true if timer restart succeeded; false otherwise.
     **/
    bool restart_timer();

    /**
     * \brief   Stops the timer and resets timeout and period values.
     **/
    bool stop_timer();

    /**
     * \brief   Pauses the timer while preserving timeout and remaining period.
     **/
    bool pause_timer();

    /**
     * \brief   Destroys and invalidates the timer.
     **/
    void destroy_timer();

    /**
     * \brief   Called by the timer manager when the timer expires; returns true if timer can
     *          continue running.
     *
     * \param   timeoutMs       The timeout in milliseconds when timer expired.
     * \return  Returns true if timer is periodic and period count is greater than zero; false if
     *          timer should stop.
     **/
    void timer_expired();

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
    inline bool _create_timer( FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Internal method to start the timer.
     *
     * \return  Returns true if timer start succeeded; false otherwise.
     **/
    inline bool _start_timer();

    /**
     * \brief   Internal method to stop the timer.
     **/
    inline void _stop_timer();

    /**
     * \brief   Internal method to destroy the timer.
     **/
    inline void _destroy_timer();

    /**
     * \brief   Returns true if the timer is currently started (due time is not zero).
     **/
    inline bool _is_started() const;

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
#else   // !__APPLE__
    /**
     * \brief   The timer ID, set when creates timer.
     */
    timer_t                 mTimerId;
#endif  // __APPLE__

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
    mutable SpinLockPosix      mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( TimerPosix );
};

//////////////////////////////////////////////////////////////////////////
// TimerPosix class inline methods
//////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__
inline timer_t TimerPosix::timer_id() const
{
	SpinAutolockPosix lock(mLock);
    return mTimerId;
}
#endif  // !__APPLE__

inline void * TimerPosix::context() const
{
	SpinAutolockPosix lock(mLock);
    return mContext;
}

inline id_type TimerPosix::context_id() const
{
    SpinAutolockPosix lock(mLock);
    return mContextId;
}

inline const timespec & TimerPosix::due_time() const
{
	SpinAutolockPosix lock(mLock);
    return mDueTime;
}

inline bool TimerPosix::is_valid() const
{
	SpinAutolockPosix lock(mLock);
#ifdef __APPLE__
    return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerQueue != nullptr));
#else   // !__APPLE__
    return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerId != 0));
#endif  // __APPLE__
}

inline bool TimerPosix::_is_started() const
{
    return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
