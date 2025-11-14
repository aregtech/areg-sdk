#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableTimerIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

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
     * \param   isAutoReset 	Indicates whether the waitable timer is manual- or auto-reset.
     * \param   name        	The name of waitable timer. Plays no role for POSIX timers.
     **/
    explicit WaitableTimerIX( bool isAutoReset = false, const char * name = nullptr);
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
    virtual bool isValid( void ) const override;

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
    virtual bool checkSignaled( pthread_t contextThread ) const override;

    /**
     * \brief   This callback is triggered when a waiting thread is released to continue to run.
     * \param   ownerThread     Indicates the POSIX thread ID that completed to wait.
     * \return  Returns true if waitable successfully has taken thread ownership.
     **/
    virtual bool notifyRequestOwnership( pthread_t ownerThread ) override;

    /**
     * \brief   This callback is triggered to when a system needs to know whether waitable
     *          can signal multiple threads. Returned 'true' value indicates that there can be
     *          multiple threads can get waitable signaled state. For example, waitable Mutex 
     *          signals only one thread, when waitable Event can signal multiple threads.
     **/
    virtual bool checkCanSignalMultipleThreads( void ) const override;

    /**
     * \brief   This callback is called to notify the object the amount of
     *          threads that were leased when the object is in signaled state.
     * \param   numThreads  The number of threads that where released when the
     *                      object is in signaled state. 0 means that no thread
     *                      was released by the object.
     **/
    virtual void notifyReleasedThreads( int numThreads ) override;

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
    id_type         mThreadId;

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
    DECLARE_NOCOPY_NOMOVE( WaitableTimerIX );
};

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
