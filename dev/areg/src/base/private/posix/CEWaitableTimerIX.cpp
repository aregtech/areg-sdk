/************************************************************************
 * \file        areg/src/base/private/posix/CEWaitableTimerIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, POSIX Waitable Timer class.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/

#include "areg/src/base/private/posix/CEWaitableTimerIX.hpp"

#ifdef  _POSIX
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/base/private/posix/NESynchTypesIX.hpp"
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"
#include <signal.h>


void CEWaitableTimerIX::_posixTimerRoutine(union sigval si)
{
    CEWaitableTimerIX *timer = reinterpret_cast<CEWaitableTimerIX *>(si.sival_ptr);

    OUTPUT_DBG("Fired waitable timer [ %p ], processing in thread [ %p ]", timer, pthread_self());
    if ( timer != NULL )
    {
        timer->timerExpired();
    }
}


CEWaitableTimerIX::CEWaitableTimerIX(bool isAutoReset /*= false*/, bool isSignaled /*= true*/, const char * name /*= NULL*/)
    : IEWaitableBaseIX  ( NESynchTypesIX::SoWaitTimer, false, name )

    , mResetInfo        ( isAutoReset ? NESynchTypesIX::EventResetAutomatic : NESynchTypesIX::EventResetManual )
    , mTimerId          ( static_cast<timer_t>(0) )
    , mTimeout          ( 0 )
    , mIsSignaled       ( isSignaled )
    , mFiredCount       ( 0 )
    , mDueTime          ( )
    , mThreadId     ( CEThread::INVALID_THREAD_ID )
{

}

CEWaitableTimerIX::~CEWaitableTimerIX(void)
{
    resetTimer();
}

bool CEWaitableTimerIX::SetTimer(unsigned int msTimeout, bool isPeriodic)
{
    bool result = false;    
    CEObjectLockIX lock(*this);

    resetTimer();
    if (msTimeout != 0)
    {
        struct sigevent sigEvent;
        NEMemory::ZeroBuffer(static_cast<void *>(&sigEvent), sizeof(struct sigevent));
        sigEvent.sigev_notify           = SIGEV_THREAD;
        sigEvent.sigev_value.sival_ptr  = static_cast<void *>(this);
        sigEvent.sigev_notify_function  = &CEWaitableTimerIX::_posixTimerRoutine;
        sigEvent.sigev_notify_attributes= NULL;

        if (RETURNED_OK == timer_create(CLOCK_REALTIME, &sigEvent, &mTimerId))
        {
            struct itimerspec interval;
            NEMemory::ZeroBuffer(static_cast<void *>(&interval), sizeof(struct itimerspec));
            NESynchTypesIX::ConvertTimeout(interval.it_value, msTimeout);
            if ( isPeriodic )
            {
                interval.it_interval.tv_sec = interval.it_value.tv_sec;
                interval.it_interval.tv_nsec= interval.it_value.tv_nsec;
            }

            mDueTime.tv_sec = interval.it_value.tv_sec;
            mDueTime.tv_nsec= interval.it_value.tv_nsec;
            mTimeout        = msTimeout;
            mIsSignaled     = false;
            mThreadId       = CEThread::GetCurrentThreadId();
            result          = true;
            if ( RETURNED_OK != timer_settime(mTimerId, 0, &interval, NULL) )
            {
                result = false;
                resetTimer();
            }
        }
    }

    return result;
}

bool CEWaitableTimerIX::StopTimer(void)
{
    bool sendSignal = false;
    do 
    {
        CEObjectLockIX lock(*this);

        sendSignal = (mIsSignaled == false);
        stopTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        CESynchLockAndWaitIX::EventSignaled(*this);
    }

    return true;
}



bool CEWaitableTimerIX::ReleaseTimer(void)
{
    bool sendSignal = false;
    do 
    {
        CEObjectLockIX lock(*this);

        sendSignal = (mIsSignaled == false);
        resetTimer();
        mIsSignaled = true;

    } while (false);

    if (sendSignal)
    {
        CESynchLockAndWaitIX::EventSignaled(*this);
    }

    return true;
}

bool CEWaitableTimerIX::IsSignaled(void) const
{
    CEObjectLockIX lock(*this);
    return mIsSignaled;
}

bool CEWaitableTimerIX::IsValid( void ) const
{
    CEObjectLockIX lock(*this);
    return (mTimerId != static_cast<timer_t>(0));
}

bool CEWaitableTimerIX::RequestsOwnership(pthread_t /* ownerThread */ )
{
    return true;
}

bool CEWaitableTimerIX::CanSignalMultipleThreads(void) const
{
    return true;
}

void CEWaitableTimerIX::ThreadsReleased(int /* numThreads */)
{
    CEObjectLockIX lock(*this);
    if (mResetInfo == NESynchTypesIX::EventResetAutomatic)
    {
        OUTPUT_DBG("Automatically resets waitable timer [ %s ] state to unsignaled.", GetName());
        mIsSignaled = false;
    }
}

inline void CEWaitableTimerIX::resetTimer( void )
{
    stopTimer();
    if (mTimerId != static_cast<timer_t>(NULL))
    {
        timer_delete(mTimerId);
    }

    mTimerId    = static_cast<timer_t>(NULL);
    mThreadId   = 0;
}

inline void CEWaitableTimerIX::stopTimer( void )
{
    if ((mTimerId != static_cast<timer_t>(NULL)) && (mTimeout != 0))
    {
        mDueTime.tv_sec = 0;
        mDueTime.tv_nsec= 0;
        itimerspec cancelSpec;
        NEMemory::ZeroBuffer(static_cast<void *>(&cancelSpec), sizeof(itimerspec));
        timer_settime(mTimerId, 0, &cancelSpec, NULL);
    }

    mTimeout    = 0;
    mFiredCount = 0;
    mThreadId   = 0;
}

inline void CEWaitableTimerIX::timerExpired(void)
{
    bool sendSignal = false;

    do 
    {
        CEObjectLockIX lock(*this);
        
        if ( mTimerId != static_cast<timer_t>(0) )
        {
            ++ mFiredCount;
            NESynchTypesIX::ConvertTimeout(mDueTime, mTimeout);
            
            mIsSignaled = true;
            sendSignal  = true;

            OUTPUT_DBG("Waitable timer [ %s ] has fired event [ %d ] times with timeout [ %d ] ms", GetName(), mFiredCount, mTimeout);
        }
#ifdef DEBUG
        else
        {
            OUTPUT_WARN("The waitaible timer was previousely canceled, ignoring processing");
        }
#endif // DEBUG

    } while (false);

    if (sendSignal)
    {
        CESynchLockAndWaitIX::EventSignaled(*this);

        CESynchLockAndWaitIX * lockAndWait = CESynchLockAndWaitIX::_mapWaitIdResource.FindResourceObject(mThreadId);
        if ( lockAndWait != NULL )
        {
            lockAndWait->NotifyAsynchSignal( );
        }
    }
}

#endif  // !_POSIX
