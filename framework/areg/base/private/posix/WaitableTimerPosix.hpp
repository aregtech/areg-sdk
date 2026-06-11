#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableTimerPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/private/posix/MutexPosix.hpp"
#include <time.h>

#ifdef __APPLE__
    #include <dispatch/dispatch.h>
#else   // !__APPLE__
    using signal_value = union sigval;
#endif  // !__APPLE__

namespace areg::os {
//////////////////////////////////////////////////////////////////////////
// WaitableTimer class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Waitable timer synchronization object. Can be manual-reset or auto-reset. The timer
 *          state is set to signaled when the timeout expires. Used to synchronize operations based
 *          on time elapse.
 **/
class WaitableTimerPosix final  : public WaitablePosix
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
private:
#ifndef __APPLE__
    /**
     * \brief   Static callback routine invoked when the POSIX timer expires.
     *
     * \param   si      Signal information structure containing data for the timer handler.
     **/
    static void _posix_timer_routine(signal_value si);
#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the waitable timer as manual-reset or auto-reset.
     *
     * \param   isAutoReset     If true, the timer is auto-reset; otherwise, it is manual-reset.
     **/
    explicit WaitableTimerPosix( bool isAutoReset = false );
    virtual ~WaitableTimerPosix();

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Starts the timer with the specified timeout and firing mode.
     *
     * \param   msTimeout       The timer timeout in milliseconds.
     * \param   is_periodic     If true, the timer fires periodically; otherwise, it fires once.
     * \return  Returns true if successfully started.
     **/
    bool set_timer( uint32_t msTimeout, bool is_periodic );

    /**
     * \brief   Stops the running timer.
     *
     * \return  Returns true if successfully stopped.
     **/
    bool stop_timer() noexcept;

    /**
     * \brief   Cancels and releases the running timer.
     *
     * \return  Returns true if successfully cancelled.
     **/
    bool cancel_timer() noexcept;

/************************************************************************/
// SyncObjectPosix overrides.
/************************************************************************/
    /**
     * \brief   Returns true if the synchronization timer object is valid.
     **/
    bool is_valid() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Protected calls
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// WaitablePosix callback overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the timer has expired (signaled); false otherwise.
     *
     * \param   contextThread       The thread ID where the lock and check happened. Not used for
     *                              waitable timers.
     * \return  Returns true if the timer is in signaled state.
     **/
    bool check_signaled( pthread_t contextThread ) const final;

    /**
     * \brief   Callback invoked when a waiting thread is released due to timer expiration.
     *
     * \param   ownerThread     The POSIX thread ID that completed waiting.
     * \return  Returns true if the thread successfully took ownership.
     **/
    bool notify_request_ownership( pthread_t ownerThread ) final;

    /**
     * \brief   Returns false to indicate that the timer can signal only one thread at a time.
     **/
    bool can_signal_threads() const noexcept final;

    /**
     * \brief   Notifies the timer that threads were released when it was in signaled state.
     *
     * \param   numThreads      The number of threads released. Zero means no thread was released.
     **/
    void notify_released_threads( int32_t numThreads ) final;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //!< Internal mutex protecting timer state fields.
    MutexPosix                  mObjectLock;

    //!< Waitable timer reset information. Either manual- or auto-reset.
    const areg::os::ResetMode   mResetInfo;

#ifdef __APPLE__

    //!< GCD dispatch timer source for macOS.
    dispatch_source_t   mTimerSource;
    //!< GCD dispatch queue for timer.
    dispatch_queue_t    mTimerQueue;
#else   // !__APPLE__

    //!< POSIX timer ID.
    timer_t         mTimerId;
#endif  // __APPLE__

    //!< Timeout is milliseconds to run.
    uint32_t        mTimeout;
    //!< Flag, indicates whether timer is in signaled or non-signaled state.
    bool            mIsSignaled;
    //!< Counts how many times the timer was fired. Used by periodic timers.
    uint32_t        mFiredCount;
    //!< POSIX timer structure.
    struct timespec mDueTime;
    //!< The ID of thread that triggered the timer. Used to notify the asynchronous call when waitable timer expired.
    id_type         mThreadId;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Stops and deletes the timer.
     **/
    void _reset_timer() noexcept;
    /**
     * \brief   Internal callback invoked when the timer expires.
     **/
    void _timer_expired() noexcept;
    /**
     * \brief   Internal method to stop the running timer.
     **/
    void _stop_timer() noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( WaitableTimerPosix );
};

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
