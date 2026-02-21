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

#ifndef __APPLE__
void WaitableTimerPosix::_posixTimerRoutine(union sigval si)
{
    WaitableTimerPosix *timer = reinterpret_cast<WaitableTimerPosix *>(si.sival_ptr);

    AREG_OUTPUT_DBG("Fired waitable timer [ %p ], processing in thread [ %p ]", timer, reinterpret_cast<id_type>(pthread_self()));
    if ( timer != nullptr )
    {
        timer->_timerExpired();
    }
}
#endif


WaitableTimerPosix::WaitableTimerPosix(bool isAutoReset /*= false*/, const char * name /*= nullptr*/)
    : WaitablePosix  ( areg::os::eSyncObject::SoWaitTimer, false, name )

    , mResetInfo        ( isAutoReset ? areg::os::eEventResetInfo::EventResetAutomatic : areg::os::eEventResetInfo::EventResetManual )
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
    areg::memZero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = &WaitableTimerPosix::_posixTimerRoutine;
    sigEvent.sigev_notify_attributes= nullptr;

    if (areg::RETURNED_OK != ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId))
    {
        mTimerId = static_cast<timer_t>(0);
    }
#endif  // __APPLE__
}

WaitableTimerPosix::~WaitableTimerPosix()
{
    _resetTimer();
}

bool WaitableTimerPosix::setTimer(unsigned int msTimeout, bool isPeriodic)
{
    bool result = false;
    ObjectLockPosix lock(*this);

    _stopTimer();

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
                                      isPeriodic ? interval_ns : DISPATCH_TIME_FOREVER,
                                      leeway_ns);

            dispatch_source_set_event_handler(mTimerSource, ^{
                this->_timerExpired();
            });

            areg::os::convTimeout(mDueTime, msTimeout);
            mTimeout    = msTimeout;
            mIsSignaled = false;
            mThreadId   = Thread::getCurrentThreadId();
            result      = true;

            dispatch_resume(mTimerSource);
        }
    }
#else   // !__APPLE__
    if ((mTimerId != static_cast<timer_t>(0)) && (msTimeout != 0))
    {
        struct itimerspec interval;
        areg::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));
        areg::os::convTimeout(interval.it_value, msTimeout);
        if ( isPeriodic )
        {
            interval.it_interval.tv_sec = interval.it_value.tv_sec;
            interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
        }

        mDueTime.tv_sec = interval.it_value.tv_sec;
        mDueTime.tv_nsec= interval.it_value.tv_nsec;
        mTimeout        = msTimeout;
        mIsSignaled     = false;
        mThreadId       = Thread::getCurrentThreadId();
        result          = true;
        if ( areg::RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr) )
        {
            result = false;
            _resetTimer();
        }
    }
#endif  // __APPLE__

    return result;
}

bool WaitableTimerPosix::stopTimer()
{
    bool sendSignal = false;
    do 
    {
        ObjectLockPosix lock(*this);

        sendSignal = (mIsSignaled == false);
        _stopTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::eventSignaled(*this);
    }

    return true;
}



bool WaitableTimerPosix::cancelTimer()
{
    bool sendSignal = false;
    do 
    {
        ObjectLockPosix lock(*this);

        sendSignal = (mIsSignaled == false);
        _resetTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitPosix::eventSignaled(*this);
    }

    return true;
}

bool WaitableTimerPosix::checkSignaled(pthread_t /*contextThread*/) const
{
    ObjectLockPosix lock(*this);
    return mIsSignaled;
}

bool WaitableTimerPosix::isValid() const
{
    ObjectLockPosix lock(*this);
#ifdef __APPLE__
    return (mTimerQueue != nullptr);
#else   // !__APPLE__
    return (mTimerId != static_cast<timer_t>(0));
#endif  // __APPLE__
}

bool WaitableTimerPosix::notifyRequestOwnership(pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableTimerPosix::checkCanSignalMultipleThreads() const
{
    return true;
}

void WaitableTimerPosix::notifyReleasedThreads(int /* numThreads */)
{
    ObjectLockPosix lock(*this);
    if (mResetInfo == areg::os::eEventResetInfo::EventResetAutomatic)
    {
        AREG_OUTPUT_DBG("Automatically resets waitable timer [ %s ] state to un-signaled.", getName().getString( ));
        mIsSignaled = false;
    }
}

inline void WaitableTimerPosix::_resetTimer()
{
    _stopTimer();
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

inline void WaitableTimerPosix::_stopTimer()
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
        areg::memZero(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        ::timer_settime(mTimerId, 0, &cancelSpec, nullptr);
    }
#endif  // __APPLE__

    mTimeout    = 0;
    mFiredCount = 0;
    mThreadId   = 0;
}

inline void WaitableTimerPosix::_timerExpired()
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
            areg::os::convTimeout(mDueTime, mTimeout);

            mIsSignaled = true;
            sendSignal  = true;

            AREG_OUTPUT_DBG("Waitable timer [ %s ] has fired event [ %d ] times with timeout [ %d ] ms", getName().getString( ), mFiredCount, mTimeout);
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
        SyncLockAndWaitPosix::eventSignaled(*this);
        SyncLockAndWaitPosix::notifyAsyncSignal(mThreadId);
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
