#ifndef AREG_BASE_PRIVATE_POSIX_CEWAITABLETIMERIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CEWAITABLETIMERIX_HPP
/************************************************************************
 * \file        areg/base/private/posix/CEWaitableTimerIX.hpp
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
// CEWaitableTimer class declaration.
//////////////////////////////////////////////////////////////////////////

class CEWaitableTimerIX : public IEWaitableBaseIX
{
private:
    static void _posixTimerRoutine(union sigval si);

public:
    CEWaitableTimerIX( bool isAutoReset = false, bool isSignaled = true, const char * name = NULL);
    virtual ~CEWaitableTimerIX( void );

public:

    bool SetTimer( unsigned int msTimeout, bool isPeriodic );

    bool StopTimer( void );

    bool ReleaseTimer( void );

/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool IsValid( void ) const;

protected:
/************************************************************************/
// IEWaitableBaseIX callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the object is signaled. Otherwise, returns false.
     **/
    virtual bool IsSignaled( void ) const;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable successfully has taken thread ownership.
     **/
    virtual bool RequestsOwnership( pthread_t ownerThread );

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool CanSignalMultipleThreads( void ) const;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void ThreadsReleased( int numThreads );

private:

    inline void resetTimer( void );

    inline void timerExpired( void );

    inline void stopTimer( void );

protected:
    const NESynchTypesIX::eEventResetInfo mResetInfo;

    timer_t         mTimerId;

    unsigned int    mTimeout;

    bool            mIsSignaled;

    unsigned int    mFiredCount;

    struct timespec mDueTime;

    ITEM_ID         mThreadId;

private:
    CEWaitableTimerIX( const CEWaitableTimerIX & /*src*/ );
    const CEWaitableTimerIX & operator = ( const CEWaitableTimerIX & /*src*/ );
};

#endif  // _POSIX

#endif  // AREG_BASE_PRIVATE_POSIX_CEWAITABLETIMERIX_HPP
