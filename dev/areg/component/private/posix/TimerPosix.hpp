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

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////

typedef void (*FuncPosixTimerRoutine)( union sigval );

//////////////////////////////////////////////////////////////////////////
// TimerPosix class declaration.
//////////////////////////////////////////////////////////////////////////
class TimerPosix
{
//////////////////////////////////////////////////////////////////////////
// Friend class that can acss protected members.
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
    inline timer_t GetTimerId( void ) const;

    /**
     * \brief   Returns POSIX timer context.
     **/
    inline Timer * GetContext( void ) const;

    /**
     * \brief   Returns timeout due date and time when timer expired or should expire next.
     **/
    inline const timespec & GetDueTime( void ) const;

    /**
     * \brief   Returns period. If zero, the timer is nnot run. If TimerPosix::TIMER_PERIOD_ENDLESS 
     *          the timer is endless until it is not stopped. Any other value specifies the remaining
     *          period to run timer.
     **/
    inline unsigned int GetRemainPeriod( void ) const;

    /**
     * \brief   Returns true if timer is started.
     **/
    inline bool IsStarted( void ) const;

    /**
     * \brief   Returns true if timer is valid, i.e. the context, timer routine and ID are valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Creates, but do not start timer. Returns true if timer ID is valid.
     **/
    bool CreateTimer( FuncPosixTimerRoutine funcTimerRoutine );

    /**
     * \brief   Starts timer by specified timeout and period count values. If timer was not created, 
     *          it will create first then start if timeout or period values are not zero.
     * \param   msTimeout   The timeout of timer in milliseconds. If timeout is zero, the timer is stopped.
     * \param   period      The period count of timer before it is stopped.
     *                          -   If 0, the timer is initialized, but not started. Returns value is true.
     *                          -   If TimerPosix::TIMER_PERIOD_ENDLESS the timer starts and does not stop
     *                              until it is not manually stopped.
     *                          -   Any other value between 0 and TimerPosix::TIMER_PERIOD_ENDLESS specify
     *                              the period count before timer is stopped, i.e. the timer will run so long,
     *                              until the period count reaches 0 or it is not manually stopped.
     * \return  Returns true if timer was initialized and started with sucss. If period count value
     *          is set zero, the timer is initialized, but do not start and the return value is zero.
     **/

    bool StartTimer( Timer & context, FuncPosixTimerRoutine funcTimerRoutine );

    /**
     * \brief   Starts timer having default values. If timer was not created, it will create first then start,
     *          if neither timeout, not period values are zero.
     **/
    bool StartTimer( void );

    /**
     * \brief   Stops timer, resets timeout and period values.
     **/
    bool StopTimer( void );

    /**
     * \brief   Pause timer. So that, when it is restarted it will use same timeout and remaining period count.
     **/
    bool PauseTimer( void );

    /**
     * \brief   Destroys and invalidates the timer.
     **/
    void DestroyTimer( void );

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
    void TimerExpired( void );

//////////////////////////////////////////////////////////////////////////
// Internal private methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Creates the timer.
     **/
    inline bool createTimer( FuncPosixTimerRoutine funcTimerRoutine );

    /**
     * \brief   Starts the timer.
     **/
    inline bool startTimer( unsigned int msTimeout, unsigned int eventCount );

    /**
     * \brief   Stops the timer.
     **/
    inline void stopTimer( void );

    /**
     * \brief   Destroys the timer.
     **/
    inline void destroyTimer( void );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    timer_t                 mTimerId;

    Timer *               mContext;

    unsigned int            mEventCount;

    struct timespec         mDueTime;

    ITEM_ID                 mThreadId;

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

inline timer_t TimerPosix::GetTimerId(void) const
{   SpinLockIX lock(mLock); return mTimerId;                                              }

inline Timer * TimerPosix::GetContext(void) const
{   SpinLockIX lock(mLock); return mContext;                                              }

inline const timespec & TimerPosix::GetDueTime(void) const
{   SpinLockIX lock(mLock); return mDueTime;                                              }

inline unsigned int TimerPosix::GetRemainPeriod(void) const
{   SpinLockIX lock(mLock); return mContext->getEventCount();                             }

inline bool TimerPosix::IsStarted(void) const
{   SpinLockIX lock(mLock); return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));   }

inline bool TimerPosix::isValid(void) const
{   SpinLockIX lock(mLock); return ((mContext != NULL) && (mTimerId != 0));               }

#endif  // _POSIX
#endif  // AREG_COMPONENT_PRIVATE_TIMERPOSIX_HPP
