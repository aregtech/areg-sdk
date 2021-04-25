#ifndef AREG_COMPONENT_PRIVATE_CETIMERPOSIX_HPP
#define AREG_COMPONENT_PRIVATE_CETIMERPOSIX_HPP
/************************************************************************
 * \file        areg/src/component/private/posix/CETimerPosix.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX specific timer information
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX
#include "areg/src/base/private/posix/CECriticalSectionIX.hpp"
#include "areg/src/component/CETimer.hpp"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// Dependency.
//////////////////////////////////////////////////////////////////////////

typedef void (*FuncPosixTimerRoutine)( union sigval );

//////////////////////////////////////////////////////////////////////////
// CETimerPosix class declaration.
//////////////////////////////////////////////////////////////////////////
class CETimerPosix
{
//////////////////////////////////////////////////////////////////////////
// Friend class that can access protected members.
//////////////////////////////////////////////////////////////////////////
    friend class CETimerManager;

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
     * \param   period          The period count. The timer can be trigger either only once (value is 1),
     *                          or until period is not expired (value is any number),
     *                          or endless until it is not stopped (value TIMER_PERIOD_ENDLESS).
     **/
    CETimerPosix( void );
    
    /**
     * \brief   Destructor.
     **/
    ~CETimerPosix( void );

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
    inline CETimer * GetContext( void ) const;

    /**
     * \brief   Returns timeout due date and time when timer expired or should expire next.
     **/
    inline const timespec & GetDueTime( void ) const;

    /**
     * \brief   Returns period. If zero, the timer is cennot run. If CETimerPosix::TIMER_PERIOD_ENDLESS 
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
    inline bool IsValid( void ) const;

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
     *                          -   If CETimerPosix::TIMER_PERIOD_ENDLESS the timer starts and does not stop
     *                              until it is not manually stopped.
     *                          -   Any other value between 0 and CETimerPosix::TIMER_PERIOD_ENDLESS specify
     *                              the period count before timer is stopped, i.e. the timer will run so long,
     *                              until the period count reaches 0 or it is not manually stopped.
     * \return  Returns true if timer was initialized and started with success. If period count value
     *          is set zero, the timer is initialized, but do not start and the return value is zero.
     **/

    bool StartTimer( CETimer & context, FuncPosixTimerRoutine funcTimerRoutine );

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

    CETimer *               mContext;

    unsigned int            mEventCount;

    struct timespec         mDueTime;

    ITEM_ID                 mThreadId;

    mutable CECriticalSectionIX mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    CETimerPosix( const CETimerPosix & /*src*/ );
    const CETimerPosix & operator = ( const CETimerPosix & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CETimerPosix class inline methods
//////////////////////////////////////////////////////////////////////////

inline timer_t CETimerPosix::GetTimerId(void) const
{   CESpinLockIX lock(mLock); return mTimerId;                                              }

inline CETimer * CETimerPosix::GetContext(void) const
{   CESpinLockIX lock(mLock); return mContext;                                              }

inline const timespec & CETimerPosix::GetDueTime(void) const
{   CESpinLockIX lock(mLock); return mDueTime;                                              }

inline unsigned int CETimerPosix::GetRemainPeriod(void) const
{   CESpinLockIX lock(mLock); return mContext->GetEventCount();                             }

inline bool CETimerPosix::IsStarted(void) const
{   CESpinLockIX lock(mLock); return ((mDueTime.tv_sec != 0) || (mDueTime.tv_nsec != 0));   }

inline bool CETimerPosix::IsValid(void) const
{   CESpinLockIX lock(mLock); return ((mContext != NULL) && (mTimerId != 0));               }

#endif  // _POSIX
#endif  // AREG_COMPONENT_PRIVATE_CETIMERPOSIX_HPP
