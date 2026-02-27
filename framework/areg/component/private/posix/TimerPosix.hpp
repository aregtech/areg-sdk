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
#include <signal.h>

#ifdef __APPLE__
    #include <dispatch/dispatch.h>
#endif  // __APPLE__

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////
namespace areg { class TimerBase; }
namespace areg::os { class TimerPosix; }
namespace areg { class TimerManager; }
namespace areg { class WatchdogManager; }

namespace areg::os
{ 
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
     * \brief   POSIX specific timer object used by timer manager.
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

        /**
         * \brief   Initializes the POSIX timer object. Sets context, timeout and period.
         * \param   context         The timer context to set. If this value is nullptr, the timer is invalid
         *                          and it will never start.
         * \param   timerRoutine    The pointer to timer routine to trigger when expired. If this value
         *                          is nullptr, the timer is not started.
         * \param   msTimeout       Timeout in milliseconds when the timer should expire.
         * \param   period          The period count. The timer can be trigger either only on (value is 1),
         *                          or until period is not expired (value is any number),
         *                          or endless until it is not stopped (value TIMER_PERIOD_ENDLESS).
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
         * \brief   Returns POSIX timer ID.
         **/
        inline timer_t getTimerId() const;
    #endif  // !__APPLE__

        /**
         * \brief   Returns POSIX timer context pointer.
         **/
        inline void * getContext() const;

        /**
         * \brief   Returns POSIX timer context ID.
         **/
        inline id_type getContextId() const;

        /**
         * \brief   Returns timeout due date and time when timer expired or should expire next.
         **/
        inline const timespec & getDueTime() const;


        /**
         * \brief   Returns true if timer is valid, i.e. the timer ID and context are valid.
         **/
        inline bool isValid() const;

        /**
         * \brief   Creates, but do not start timer.
         *          This call will create a time to handle in a separate thread.
         * \param   funcTimer   The callback function to trigger when timer expires.
         * \return  Returns true if succeeded to created timer.
         **/
        bool createTimer( areg::os::FuncPosixTimerRoutine funcTimer );

        /**
         * \brief   Creates and starts timer with timeout and period count values specified in
         *          the give Timer object. If the specified timeout or period values in
         *          the Timer object are zero, the timer is created, but not started.
         *          This function creates a timer to handle in a separate thread.
         * \param   context     The timer object that contains timeout and period information.
         * \param   contextId   The timer context ID to set.
         * \param   funcTimer   The callback function to trigger when timer expires.
         * \return  Returns true if timer is created and started with success.
         **/
        bool startTimer( areg::TimerBase & context, id_type contextId, areg::os::FuncPosixTimerRoutine funcTimer );

        /**
         * \brief   Restarts the timer if the timeout and the period values are not zero.
         *          The timer should be initialized before calling this method.
         * \return  Returns true if timer is restarted with success.
         **/
        bool restartTimer();

        /**
         * \brief   Stops timer, resets timeout and period values.
         **/
        bool stopTimer();

        /**
         * \brief   Pause timer. So that, when it is restarted it will use same timeout and remaining period count.
         **/
        bool pauseTimer();

        /**
         * \brief   Destroys and invalidates the timer.
         **/
        void destroyTimer();

        /**
         * \brief   Called by timer manager when timer is expired. Returns true if timer 
         *          can continue running. Returns false if timer should be stopped.
         *          The timer can run if it is periodic and the period count is greater 
         *          than zero. The timer is stopped if period count is zero.
         * \param   timeoutMs   The timeout in milliseconds when timer expired.
         * \return  Returns true if timer can continue running. Returns false if timer 
         *          should be stopped.
         **/
        void timerExpired();

    //////////////////////////////////////////////////////////////////////////
    // Internal private methods.
    //////////////////////////////////////////////////////////////////////////
    private:

        /**
         * \brief   Creates and initializes the timer that is handled in a separate thread.
         * \param   funcTimer   The callback function to trigger when timer expires.
         * \return  Returns true if succeeded to create timer.
         **/
        inline bool _createTimer( areg::os::FuncPosixTimerRoutine funcTimer );

        /**
         * \brief	Initializes and starts the timer.
         * \return	Returns true if timer succeeded to start.
         **/
        inline bool _startTimer();

        /**
         * \brief   Stops the timer.
         **/
        inline void _stopTimer();

        /**
         * \brief   Destroys the timer.
         **/
        inline void _destroyTimer();

        /**
         * \brief   Returns true if timer is started. The timer considered started if
         *          due time is not zero.
         **/
        inline bool _isStarted() const;

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
        areg::os::FuncPosixTimerRoutine   mTimerCallback;
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
        areg::TimerBase *             mContext;

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
        mutable areg::os::SpinLockPosix      mLock;

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
    inline timer_t TimerPosix::getTimerId() const
    {
    	SpinAutolockPosix lock(mLock);
        return mTimerId;
    }
    #endif  // !__APPLE__

    inline void * TimerPosix::getContext() const
    {
    	SpinAutolockPosix lock(mLock);
        return mContext;
    }

    inline id_type TimerPosix::getContextId() const
    {
        SpinAutolockPosix lock(mLock);
        return mContextId;
    }

    inline const timespec & TimerPosix::getDueTime() const
    {
    	SpinAutolockPosix lock(mLock);
        return mDueTime;
    }

    inline bool TimerPosix::isValid() const
    {
    	SpinAutolockPosix lock(mLock);
    #ifdef __APPLE__
        return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerQueue != nullptr));
    #else   // !__APPLE__
        return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerId != 0));
    #endif  // __APPLE__
    }

    inline bool TimerPosix::_isStarted() const
    {
        return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));
    }

    #endif  // defined(_POSIX) || defined(POSIX)

} // namespace areg::os
#endif  // AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
