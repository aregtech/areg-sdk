/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitableTimerIX.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/base/private/posix/WaitableTimerIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/NEMemory.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/posix/NESyncTypesIX.hpp"
#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"
#include <signal.h>


void WaitableTimerIX::_posixTimerRoutine(union sigval si)
{
    WaitableTimerIX *timer = reinterpret_cast<WaitableTimerIX *>(si.sival_ptr);

    OUTPUT_DBG("Fired waitable timer [ %p ], processing in thread [ %p ]", timer, reinterpret_cast<id_type>(pthread_self()));
    if ( timer != nullptr )
    {
        timer->_timerExpired();
    }
}


WaitableTimerIX::WaitableTimerIX(bool isAutoReset /*= false*/, const char * name /*= nullptr*/)
    : IEWaitableBaseIX  ( NESyncTypesIX::eSyncObject::SoWaitTimer, false, name )

    , mResetInfo        ( isAutoReset ? NESyncTypesIX::eEventResetInfo::EventResetAutomatic : NESyncTypesIX::eEventResetInfo::EventResetManual )
    , mTimerId          ( static_cast<timer_t>(0) )
    , mTimeout          ( 0 )
    , mIsSignaled       ( false )
    , mFiredCount       ( 0 )
    , mDueTime          ( {0, 0} )
    , mThreadId         ( Thread::INVALID_THREAD_ID )
{
    struct sigevent sigEvent;
    NEMemory::memZero(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
    sigEvent.sigev_notify           = SIGEV_THREAD;
    sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
    sigEvent.sigev_notify_function  = &WaitableTimerIX::_posixTimerRoutine;
    sigEvent.sigev_notify_attributes= nullptr;

    if (RETURNED_OK != ::timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId))
    {
        mTimerId = static_cast<timer_t>(0);
    }
}

WaitableTimerIX::~WaitableTimerIX(void)
{
    _resetTimer();
}

bool WaitableTimerIX::setTimer(unsigned int msTimeout, bool isPeriodic)
{
    bool result = false;    
    ObjectLockIX lock(*this);

    _stopTimer();
    if ((mTimerId = static_cast<timer_t>(0)) && (msTimeout != 0))
    {
        struct itimerspec interval;
        NEMemory::memZero(static_cast<void *>(&interval), sizeof(struct itimerspec));
        NESyncTypesIX::convTimeout(interval.it_value, msTimeout);
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
        if ( RETURNED_OK != ::timer_settime(mTimerId, 0, &interval, nullptr) )
        {
            result = false;
            _resetTimer();
        }
    }

    return result;
}

bool WaitableTimerIX::stopTimer(void)
{
    bool sendSignal = false;
    do 
    {
        ObjectLockIX lock(*this);

        sendSignal = (mIsSignaled == false);
        _stopTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitIX::eventSignaled(*this);
    }

    return true;
}



bool WaitableTimerIX::cancelTimer(void)
{
    bool sendSignal = false;
    do 
    {
        ObjectLockIX lock(*this);

        sendSignal = (mIsSignaled == false);
        _resetTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitIX::eventSignaled(*this);
    }

    return true;
}

bool WaitableTimerIX::checkSignaled(pthread_t /*contextThread*/) const
{
    ObjectLockIX lock(*this);
    return mIsSignaled;
}

bool WaitableTimerIX::isValid( void ) const
{
    ObjectLockIX lock(*this);
    return (mTimerId != static_cast<timer_t>(0));
}

bool WaitableTimerIX::notifyRequestOwnership(pthread_t /* ownerThread */ )
{
    return true;
}

bool WaitableTimerIX::checkCanSignalMultipleThreads(void) const
{
    return true;
}

void WaitableTimerIX::notifyReleasedThreads(int /* numThreads */)
{
    ObjectLockIX lock(*this);
    if (mResetInfo == NESyncTypesIX::eEventResetInfo::EventResetAutomatic)
    {
        OUTPUT_DBG("Automatically resets waitable timer [ %s ] state to un-signaled.", getName().getString( ));
        mIsSignaled = false;
    }
}

inline void WaitableTimerIX::_resetTimer( void )
{
    _stopTimer();
    if (mTimerId != static_cast<timer_t>(0))
    {
        ::timer_delete(mTimerId);
    }

    mTimerId    = static_cast<timer_t>(0);
    mThreadId   = 0;
}

inline void WaitableTimerIX::_stopTimer( void )
{
    if ((mTimerId != static_cast<timer_t>(0)) && (mTimeout != 0))
    {
        mDueTime.tv_sec = 0;
        mDueTime.tv_nsec= 0;
        itimerspec cancelSpec;
        NEMemory::memZero(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        ::timer_settime(mTimerId, 0, &cancelSpec, nullptr);
    }

    mTimeout    = 0;
    mFiredCount = 0;
    mThreadId   = 0;
}

inline void WaitableTimerIX::_timerExpired(void)
{
    bool sendSignal = false;

    do 
    {
        ObjectLockIX lock(*this);
        
        if ( mTimerId != static_cast<timer_t>(0) )
        {
            ++ mFiredCount;
            NESyncTypesIX::convTimeout(mDueTime, mTimeout);
            
            mIsSignaled = true;
            sendSignal  = true;

            OUTPUT_DBG("Waitable timer [ %s ] has fired event [ %d ] times with timeout [ %d ] ms", getName().getString( ), mFiredCount, mTimeout);
        }
#ifdef DEBUG
        else
        {
            OUTPUT_WARN("The waitable timer was previously canceled, ignoring processing");
        }
#endif // DEBUG

    } while (false);

    if (sendSignal)
    {
        SyncLockAndWaitIX::eventSignaled(*this);
        SyncLockAndWaitIX::notifyAsyncSignal(mThreadId);
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
