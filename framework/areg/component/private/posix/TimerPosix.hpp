#ifndef AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
#define AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerPosix.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/SpinLockIX.hpp"
#include <sys/types.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////
class TimerBase;

/**
 * \brief   The POSIX timer routing method triggered in a separate thread.
 */
typedef void (*FuncPosixTimerRoutine)( union sigval );

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
    friend class TimerManager;

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
    TimerPosix( void );
    
    /**
     * \brief   Destructor.
     **/
    ~TimerPosix( void );

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns POSIX timer ID.
     **/
    inline timer_t getTimerId( void ) const;

    /**
     * \brief   Returns POSIX timer context pointer.
     **/
    inline void * getContext( void ) const;

    /**
     * \brief   Returns POSIX timer context ID.
     **/
    inline id_type getContextId( void ) const;

    /**
     * \brief   Returns timeout due date and time when timer expired or should expire next.
     **/
    inline const timespec & getDueTime( void ) const;


    /**
     * \brief   Returns true if timer is valid, i.e. the timer ID and context are valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Creates, but do not start timer.
     *          This call will create a time to handle in a separate thread.
     * \param   funcTimer   Pointer to the function triggered when timer is expired.
     * \return  Returns true if succeeded to created timer.
     **/
    bool createTimer( FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Creates and starts timer with timeout and period count values specified in
     *          the give Timer object. If the specified timeout or period values in
     *          the Timer object are zero, the timer is created, but not started.
     *          This function creates a timer to handle in a separate thread.
     * \param   context     The timer object that contains timeout and period information.
     * \param   funcTimer   Pointer to the timer handling function triggered when timer expires.
     * \return  Returns true if timer is created and started with success.
     **/
    bool startTimer( TimerBase & context, FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Starts initialized timer if the timeout and the period values are not zero.
     *          The timer should be initialized before calling this method.
     * \param   context     The timer object that contains timeout and period information.
     * \return  Returns true if timer is started with success.
     */
    bool startTimer( TimerBase & context );

    /**
     * \brief   Starts the timer that was initialized. This function ignores starting timer
     *          if either it was not initialized, or timeout is zero, or the period is zero.
     *          The timer, timeout and periodic values should be set before starting timer.
     * \return  Returns true if timer is initialized and started with success.
     **/
    bool startTimer( void );

    /**
     * \brief   Stops timer, resets timeout and period values.
     **/
    bool stopTimer( void );

    /**
     * \brief   Pause timer. So that, when it is restarted it will use same timeout and remaining period count.
     **/
    bool pauseTimer( void );

    /**
     * \brief   Destroys and invalidates the timer.
     **/
    void destroyTimer( void );

//////////////////////////////////////////////////////////////////////////
// Protected methods.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Called by timer manager when timer is expired. Returns true if timer 
     *          can continue running. Returns false if timer should be stopped.
     *          The timer can run if it is periodic and the period count is greater 
     *          than zero. The timer is stopped if period count is zero.
     * \param   timeoutMs   The timeout in milliseconds when timer expired.
     * \return  Returns true if timer can continue running. Returns false if timer 
     *          should be stopped.
     **/
    void timerExpired( void );

//////////////////////////////////////////////////////////////////////////
// Internal private methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Creates and initializes the timer that is handled in a separate thread.
     * \param   funcTimer   The pointer to timer handling function triggered
     *                      by system when timer is expired. This method is
     *                      triggered in a separate thread.
     * \return  Returns true if succeeded to create timer.
     **/
    inline bool _createTimer( FuncPosixTimerRoutine funcTimer );

    /**
     * \brief	Initializes and starts the timer.
     * \param	context	The pointer to timer context object.
     * \return	Returns true if timer succeeded to start.
     **/
    inline bool _startTimer( TimerBase * context );

    /**
     * \brief   Stops the timer.
     **/
    inline void _stopTimer( void );

    /**
     * \brief   Destroys the timer.
     **/
    inline void _destroyTimer( void );

    /**
     * \brief   Returns true if timer is started. The timer considered started if
     *          due time is not zero.
     **/
    inline bool _isStarted( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The timer ID, set when creates timer.
     */
    timer_t                 mTimerId;

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
     * \brief   The ID of thread where timer is started.
     */
    id_type                 mThreadId;

    /**
     * \brief   Synchronization object.
     */
    mutable SpinLockIX      mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TimerPosix );
};

//////////////////////////////////////////////////////////////////////////
// TimerPosix class inline methods
//////////////////////////////////////////////////////////////////////////

inline timer_t TimerPosix::getTimerId(void) const
{
	SpinAutolockIX lock(mLock);
    return mTimerId;
}

inline void * TimerPosix::getContext(void) const
{
	SpinAutolockIX lock(mLock);
    return mContext;
}

inline id_type TimerPosix::getContextId(void) const
{
    SpinAutolockIX lock(mLock);
    return mContextId;
}

inline const timespec & TimerPosix::getDueTime(void) const
{
	SpinAutolockIX lock(mLock);
    return mDueTime;
}

inline bool TimerPosix::isValid(void) const
{
	SpinAutolockIX lock(mLock);
    return (((mContext != nullptr) || (mContextId != 0u)) && (mTimerId != 0));
}

inline bool TimerPosix::_isStarted(void) const
{
    return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));
}

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_COMPONENT_PRIVATE_POSIX_TIMERPOSIX_HPP
