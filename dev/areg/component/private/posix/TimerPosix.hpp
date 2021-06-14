#ifndef AREG_COMPONENT_PRIVATE_TIMERPOSIX_HPP
#define AREG_COMPONENT_PRIVATE_TIMERPOSIX_HPP
/************************************************************************
 * \file        areg/component/private/posix/TimerPosix.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef _POSIX
#include "areg/base/private/posix/CriticalSectionIX.hpp"
#include "areg/component/Timer.hpp"
#include <time.h>
#include <signal.h>

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////

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
// Friend class that can access protected members.
//////////////////////////////////////////////////////////////////////////
    friend class TimerManager;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Initializes the POSIX timer object. Sets context, timeout and period.
     * \param   context         The timer context to set. If this value is NULL, the timer is invalid
     *                          and it will never start.
     * \param   timerRoutine    The pointer to timer routine to trigger when expired. If this value
     *                          is NULL, the timer is not started.
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
     * \brief   Returns POSIX timer context.
     **/
    inline Timer * getContext( void ) const;

    /**
     * \brief   Returns timeout due date and time when timer expired or should expire next.
     **/
    inline const timespec & getDueTime( void ) const;

    /**
     * \brief   Returns period. If zero, the timer is nnot run. If TimerPosix::TIMER_PERIOD_ENDLESS 
     *          the timer is endless until it is not stopped. Any other value specifies the remaining
     *          period to run timer.
     **/
    inline unsigned int getRemainPeriod( void ) const;

    /**
     * \brief   Returns true if timer is started.
     **/
    inline bool isStarted( void ) const;

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
     *          the Timer object are zero, the timer is created, but not not started.
     *          This function creates a timer to handle in a separate thread.
     * \param   context     The timer object that contains timeout and period information.
     * \param   funcTimer   Pointer to the timer handling function triggered when timer expires.
     * \return  Returns true if timer is created and started with success.
     **/
    bool startTimer( Timer & context, FuncPosixTimerRoutine funcTimer );

    /**
     * \brief   Starts initialized timer if the timeout and the period values are not zero.
     *          The timer should be initialized before calling this method.
     * \param   context     The timer object that contains timeout and period information.
     * \return  Returns true if timer is started with success.
     */
    bool startTimer( Timer & context );

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
     * \param   Returns true if timer can continue running. Returns false if timer 
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
     * \brief   Starts the initialized timer.
     * \param   msTimeout   The timeout of timer is milliseconds.
     * \param   eventCount  The number of periods to trigger.
     * \return  Returns true if succeeded to start timer.
     **/
    inline bool _startTimer( unsigned int msTimeout, unsigned int eventCount );

    /**
     * \brief   Stops the timer.
     **/
    inline void _stopTimer( void );

    /**
     * \brief   Destroys the timer.
     **/
    inline void _destroyTimer( void );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The timer ID, set when creates timer.
     */
    timer_t                 mTimerId;

    /**
     * \brief   The valid pointer to Timer object that contains timeout and period information.
     */
    Timer *                 mContext;

    /**
     * \brief   The number of events to trigger timer. Should be more than zero.
     */
    unsigned int            mEventCount;

    /**
     * \brief   The timer timeout information.
     */
    struct timespec         mDueTime;

    /**
     * \brief   The ID of thread where timer is started.
     */
    ITEM_ID                 mThreadId;

    /**
     * \brief   Synchronization object.
     */
    mutable CriticalSectionIX mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    TimerPosix( const TimerPosix & /*src*/ );
    const TimerPosix & operator = ( const TimerPosix & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// TimerPosix class inline methods
//////////////////////////////////////////////////////////////////////////

inline timer_t TimerPosix::getTimerId(void) const
{
    SpinLockIX lock(mLock);
    return mTimerId;
}

inline Timer * TimerPosix::getContext(void) const
{
    SpinLockIX lock(mLock);
    return mContext;
}

inline const timespec & TimerPosix::getDueTime(void) const
{
    SpinLockIX lock(mLock);
    return mDueTime;
}

inline unsigned int TimerPosix::getRemainPeriod(void) const
{
    SpinLockIX lock(mLock);
    return mContext->getEventCount();
}

inline bool TimerPosix::isStarted(void) const
{
    SpinLockIX lock(mLock);
    return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));
}

inline bool TimerPosix::isValid(void) const
{
    SpinLockIX lock(mLock);
    return ((mContext != NULL) && (mTimerId != 0));
}

#endif  // _POSIX
#endif  // AREG_COMPONENT_PRIVATE_TIMERPOSIX_HPP
