/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableTimerPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/base/private/posix/WaitableTimerPosix.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

#ifndef __APPLE__
    #include <signal.h>
#endif

namespace areg::os {

#ifndef __APPLE__
void WaitableTimerPosix::_posix_timer_routine(signal_value si)
{
    WaitableTimerPosix *timer = reinterpret_cast<WaitableTimerPosix *>(si.sival_ptr);

    AREG_OUTPUT_DBG("Fired waitable timer [ %p ], processing in thread [ %p ]", timer, reinterpret_cast<id_type>(pthread_self()));
    if ( timer != nullptr )
    {
        timer->_timer_expired();
    }
}
#endif


WaitableTimerPosix::WaitableTimerPosix(bool is_auto_reset /*= false*/, const char * name /*= nullptr*/)
    : WaitablePosix  ( areg::os::SyncKind::SoWaitTimer, false, name )

    , mResetInfo        ( is_auto_reset ? areg::os::ResetMode::Automatic : areg::os::ResetMode::Manual )
#ifdef __APPLE__
    , mTimerSource      ( nullptr )
    , mTimerQueue       ( nullptr )
#else   // !__APPLE__
    , mTimerId          ( static_cast<timer_t>(0) )
#endif  // __APPLE__
    , mTimeout          ( 0 )
    , mIsSignaled       ( false )
    , mFiredCount       ( 0 )
    , mDueTime          ( {0, 0} )
    , mThreadId         ( Thread::INVALID_THREAD_ID )
{
#ifdef __APPLE__
    mTimerQueue = dispatch_queue_create("areg.waitable.timer", DISPATCH_QUEUE_SERIAL);
#else   // !__APPLE__
    struct sigevent sigEvent;
    areg::mem_zero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = &WaitableTimerPosix::_posix_timer_routine;
    sigEvent.sigev_notify_attributes= nullptr;

    if (areg::RETURNED_OK != ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId))
    {
        mTimerId = static_cast<timer_t>(0);
    }
#endif  // __APPLE__
}

WaitableTimerPosix::~WaitableTimerPosix()
{
    _reset_timer();
}

bool WaitableTimerPosix::set_timer(uint32_t msTimeout, bool is_periodic)
{
    bool result = false;
    ObjectLockPosix lock(*this);

    _stop_timer();

#ifdef __APPLE__
    if ((mTimerQueue != nullptr) && (msTimeout != 0))
    {
        mTimerSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, mTimerQueue);
        if (mTimerSource != nullptr)
        {
            uint64_t interval_ns = static_cast<uint64_t>(msTimeout) * NSEC_PER_MSEC;
            uint64_t leeway_ns   = NSEC_PER_MSEC; // 1ms leeway

            dispatch_source_set_timer(mTimerSource,
                                      dispatch_time(DISPATCH_TIME_NOW, interval_ns),
                                      is_periodic ? interval_ns : DISPATCH_TIME_FOREVER,
                                      leeway_ns);

            dispatch_source_set_event_handler(mTimerSource, ^{
                this->_timer_expired();
            });

            areg::os::conv_timeout(mDueTime, msTimeout);
            mTimeout    = msTimeout;
            mIsSignaled = false;
            mThreadId   = Thread::current_thread_id();
            result      = true;

            dispatch_resume(mTimerSource);
        }
    }
#else   // !__APPLE__
    if ((mTimerId != static_cast<timer_t>(0)) && (msTimeout != 0))
    {
        struct itimerspec interval;
        areg::mem_zero(static_cast<void *>(&interval), sizeof(struct itimerspec));
        areg::os::conv_timeout(interval.it_value, msTimeout);
        if ( is_periodic )
        {
            interval.it_interval.tv_sec = interval.it_value.tv_sec;
            interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
        }

        mDueTime.tv_sec = interval.it_value.tv_sec;
        mDueTime.tv_nsec= interval.it_value.tv_nsec;
        mTimeout        = msTimeout;
        mIsSignaled     = false;
        mThreadId       = Thread::current_thread_id();
        result          = true;
        if ( areg::RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr) )
        {
            result = false;
            _reset_timer();
        }
    }
#endif  // __APPLE__

    return result;
}

bool WaitableTimerPosix::stop_timer()
{
    bool sendSignal = false;
    do 
    {
        ObjectLockPosix lock(*this);

        sendSignal = (mIsSignaled == false);
        _stop_timer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return true;
}



bool WaitableTimerPosix::cancel_timer()
{
    bool sendSignal = false;
    do 
    {
        ObjectLockPosix lock(*this);

        sendSignal = (mIsSignaled == false);
        _reset_timer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
    }

    return true;
}

bool WaitableTimerPosix::check_signaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(*this);
    return mIsSignaled;
}

bool WaitableTimerPosix::is_valid() const
{
    ObjectLockPosix lock(*this);
#ifdef __APPLE__
    return (mTimerQueue != nullptr);
#else   // !__APPLE__
    return (mTimerId != static_cast<timer_t>(0));
#endif  // __APPLE__
}

bool WaitableTimerPosix::notify_request_ownership(pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableTimerPosix::can_signal_threads() const
{
    return true;
}

void WaitableTimerPosix::notify_released_threads(int32_t /* numThreads */)
{
    ObjectLockPosix lock(*this);
    if (mResetInfo == areg::os::ResetMode::Automatic)
    {
        AREG_OUTPUT_DBG("Automatically resets waitable timer [ %s ] state to un-signaled.", name().as_string( ));
        mIsSignaled = false;
    }
}

inline void WaitableTimerPosix::_reset_timer()
{
    _stop_timer();
#ifdef __APPLE__
    if (mTimerQueue != nullptr)
    {
        dispatch_release(mTimerQueue);
        mTimerQueue = nullptr;
    }
#else   // !__APPLE__
    if (mTimerId != static_cast<timer_t>(0))
    {
        ::timer_delete(mTimerId);
    }
    mTimerId    = static_cast<timer_t>(0);
#endif  // __APPLE__
    mThreadId   = 0;
}

inline void WaitableTimerPosix::_stop_timer()
{
#ifdef __APPLE__
    if (mTimerSource != nullptr)
    {
        dispatch_source_cancel(mTimerSource);
        dispatch_release(mTimerSource);
        mTimerSource = nullptr;
    }
    mDueTime.tv_sec = 0;
    mDueTime.tv_nsec= 0;
#else   // !__APPLE__
    if ((mTimerId != static_cast<timer_t>(0)) && (mTimeout != 0))
    {
        mDueTime.tv_sec = 0;
        mDueTime.tv_nsec= 0;
        itimerspec cancelSpec;
        areg::mem_zero(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        ::timer_settime(mTimerId, 0, &cancelSpec, nullptr);
    }
#endif  // __APPLE__

    mTimeout    = 0;
    mFiredCount = 0;
    mThreadId   = 0;
}

inline void WaitableTimerPosix::_timer_expired()
{
    bool sendSignal = false;

    do
    {
        ObjectLockPosix lock(*this);

#ifdef __APPLE__
        if ( mTimerSource != nullptr )
#else   // !__APPLE__
        if ( mTimerId != static_cast<timer_t>(0) )
#endif  // __APPLE__
        {
            ++ mFiredCount;
            areg::os::conv_timeout(mDueTime, mTimeout);

            mIsSignaled = true;
            sendSignal  = true;

            AREG_OUTPUT_DBG("Waitable timer [ %s ] has fired event [ %d ] times with timeout [ %d ] ms", name().as_string( ), mFiredCount, mTimeout);
        }
#ifdef DEBUG
        else
        {
            AREG_OUTPUT_WARN("The waitable timer was previously canceled, ignoring processing");
        }
#endif // DEBUG

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::event_signaled(*this);
        SyncLockAndWaitPosix::notify_async_signal(mThreadId);
    }
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)
