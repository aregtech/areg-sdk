#ifndef AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITABLETIMERIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include <stdint.h>

namespace areg::os {

class TimerServicePosix;

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
    friend class TimerServicePosix;

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
     * \brief   Stops the timer and signals it once, as if it expired now. An automatic reset
     *          timer releases one waiting thread, a manual reset timer releases all of them and
     *          stays signaled until it is set again. The timer can be set again.
     *
     * \return  Returns true.
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
     * \brief   Called by a waiting thread that found the timer signaled. An automatic reset
     *          timer is reset here, so only one waiter takes one expiration.
     *
     * \param   ownerThread     The POSIX thread ID that completed waiting.
     * \return  Returns true if the thread took the expiration, false if another one took it.
     **/
    bool notify_request_ownership( pthread_t ownerThread ) final;

    /**
     * \brief   Returns true: an expiration wakes every waiter, and an automatic reset timer is
     *          taken by one of them.
     **/
    bool can_signal_threads() const noexcept final;

    /**
     * \brief   Notifies the timer that threads were released when it was in signaled state.
     *          Does nothing: the waiter resets the timer when it takes the expiration.
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

    //!< Next armed timer in deadline order. Guarded by the timer service lock.
    WaitableTimerPosix *    mNextArmed;
    //!< Previous armed timer in deadline order. Guarded by the timer service lock.
    WaitableTimerPosix *    mPrevArmed;
    //!< Monotonic expiration time in nanoseconds. Guarded by the timer service lock.
    uint64_t                mDeadlineNs;
    //!< Period in nanoseconds, 0 for a one-shot timer. Guarded by the timer service lock.
    uint64_t                mPeriodNs;
    //!< Flag, indicates whether the timer is in the armed list. Guarded by the timer service lock.
    bool                    mIsArmed;

    //!< Timeout is milliseconds to run.
    uint32_t        mTimeout;
    //!< Flag, indicates whether timer is in signaled or non-signaled state.
    bool            mIsSignaled;
    //!< Counts how many times the timer was fired. Used by periodic timers.
    uint32_t        mFiredCount;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Internal callback invoked when the timer expires.
     *
     * \param   expirations     The number of periods elapsed since the previous call.
     **/
    void _timer_expired( uint32_t expirations ) noexcept;
    /**
     * \brief   Internal method to reset the timeout and the expiration count.
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
