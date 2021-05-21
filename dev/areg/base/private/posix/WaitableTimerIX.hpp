#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
/************************************************************************
 * \file        areg/base/private/posix/WaitableTimerIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#ifdef _POSIX
#include "areg/base/private/posix/IEWaitableBaseIX.hpp"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// WaitableTimer class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The waitable timer object is a synchronization object which state is 
 *          set to signaled when the specified timeout is expired. There are two 
 *          types of waitable timers: manual- or auto-reset.
 **/
class WaitableTimerIX : public IEWaitableBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The POSIX timer routing function.
     * \param   si  The signal processing structure data passed to routine.
     **/
    static void _posixTimerRoutine(union sigval si);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes waitable timer, sets the states.
     * \param   isAutoReset     Indicates whether the waitable timer is manual- or aut-reset.
     * \param   isSignaled      Indicates the initial signaled state.
     * \param   name            The name of waitable timer. Plays no role for POSIX timers.
     **/
    WaitableTimerIX( bool isAutoReset = false, bool isSignaled = true, const char * name = NULL);
    /**
     * \brief   Destructor.
     */
    virtual ~WaitableTimerIX( void );

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Starts the timer that runs periodic or only once.
     * \param   msTimeout   The timer timeout in milliseconds to run.
     * \param   isPeriodic  If true, the timer is periodic and it runs until stopped.
     *                      Otherwise, it runs only once.
     * \return  Returns true if succeeded to start timer.
     **/
    bool setTimer( unsigned int msTimeout, bool isPeriodic );

    /**
     * \brief   Stops the running timer.
     * \return  Returns true if succeeded to stop timer.
     **/
    bool stopTimer( void );

    /**
     * \brief   Cancel and release running timer.
     * \return  Returns true if succeeded to stop timer.
     **/
    bool cancelTimer( void );

/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool isValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected calls
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     * param    contextThread   The thread ID where lock and check happened.
     **/
    virtual bool checkSignaled( pthread_t contextThread ) const;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable successfully has taken thread ownership.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread );

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void notifyReleasedThreads( int numThreads );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Waitable timer reset information. Either manual- or auto-reset.
     **/
    const NESynchTypesIX::eEventResetInfo mResetInfo;
    /**
     * \brief   POSIX timer ID.
     **/
    timer_t         mTimerId;
    /**
     * \brief   Timeout is milliseconds to run.
     **/
    unsigned int    mTimeout;
    /**
     * \brief   Flag, indicates whether timer is in signaled or non-signaled state.
     **/
    bool            mIsSignaled;
    /**
     * \brief   Counts how many times the timer was fired. Used by periodic timers.
     **/
    unsigned int    mFiredCount;
    /**
     * \brief   POSIX timer structure.
     **/
    struct timespec mDueTime;
    /**
     * \brief   The ID of thread that triggered the timer. Used to notify the asynchronous call when waitable timer expired.
     **/
    ITEM_ID         mThreadId;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Stops and deletes timer
     **/
    inline void _resetTimer( void );
    /**
     * \brief   Called when timer has expired.
     **/
    inline void _timerExpired( void );
    /**
     * \brief   Called to stop running timer.
     **/
    inline void _stopTimer( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    WaitableTimerIX( const WaitableTimerIX & /*src*/ );
    const WaitableTimerIX & operator = ( const WaitableTimerIX & /*src*/ );
};

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
