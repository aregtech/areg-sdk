/************************************************************************
 * \file        areg/src/component/CETimer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer class.
 *              Use to fire timer.
 *
 ************************************************************************/
#include "areg/src/component/CETimer.hpp"
#include "areg/src/component/private/CETimerManager.hpp"

#include "areg/src/component/CEDispatcherThread.hpp"
#include "areg/src/base/CEDateTime.hpp"

//////////////////////////////////////////////////////////////////////////
// CETimer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This value is used to set continues Timer, which will not stop, until it is not requested to be stopped manually.
 **/
const unsigned int  CETimer::CONTINUOUSLY            = static_cast<unsigned int>(-1);   /*0xFFFFFFFF*/
/**
 * \brief   Default number of maximum queued number of timer events in dispatcher thread.
 **/
const int           CETimer::DEFAULT_MAXIMUM_QUEUE   = static_cast<int>(5);             /*0x00000005*/
/**
 * \brief   Defined to ignore number of maximum queued timer events in dispatcher thread.
 **/
const int           CETimer::IGNORE_TIMER_QUEUE      = static_cast<int>(0);             /*0x00000000*/

const unsigned int  CETimer::TIMEOUT_1_MS            = 1;

const unsigned int  CETimer::TIMEOUT_100_MS          = (CETimer::TIMEOUT_1_MS * 100);

const unsigned int  CETimer::TIMEOUT_500_MS          = (CETimer::TIMEOUT_100_MS * 5);

const unsigned int  CETimer::TIMEOUT_1_SEC           = (CETimer::TIMEOUT_1_MS * 1000);

const unsigned int  CETimer::TIMEOUT_1_MIN           = (CETimer::TIMEOUT_1_SEC * 60);


unsigned int CETimer::GetTickCount( void )
{
    return static_cast<unsigned int>(CEDateTime::GetTickCount());
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CETimer::CETimer(IETimerConsumer& timerConsumer, const char* timerName /*= NULL*/, int maxQueued /*= CETimer::DEFAULT_MAXIMUM_QUEUE*/)
    : mConsumer         (timerConsumer)
    , mName             (NEUtilities::GenerateName(timerName))
    , mTimeoutInMs      (0)
    , mEventsCount      (0)
    , mNextFire         (0)
    , mLastFired        (0)

    , mCurrentQueued    (0)
    , mMaxQueued        (maxQueued > 0 ? maxQueued : CETimer::IGNORE_TIMER_QUEUE)
    , mDispatchThread   (NULL)
    , mStarted          (false)
    , mLock             ( )
{
    ; // do nothing
}

CETimer::~CETimer( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
bool CETimer::StartTimer( unsigned int timeoutInMs, unsigned int eventCount /*= CETimer::CONTINUOUSLY*/ )
{
    CELock lock(mLock);

    if (mTimeoutInMs != 0)
        CETimerManager::StopTimer(self());

    mTimeoutInMs    = timeoutInMs;
    mEventsCount    = eventCount;
    mNextFire       = CETimer::GetTickCount() + timeoutInMs;
    mLastFired      = 0;
    mCurrentQueued  = 0;

    mStarted        = CETimerManager::StartTimer(self());
    mDispatchThread = mStarted ? RUNTIME_CAST(CEThread::GetCurrentThread(), CEDispatcherThread) : NULL;

    return (mDispatchThread != NULL);
}

bool CETimer::StartTimer(unsigned int timeoutInMs, CEDispatcherThread & whichThread, unsigned int eventCount /*= CETimer::CONTINUOUSLY*/)
{
    CELock lock(mLock);

    if (mTimeoutInMs != 0)
        CETimerManager::StopTimer(self());

    mTimeoutInMs    = timeoutInMs;
    mEventsCount    = eventCount;
    mNextFire       = CETimer::GetTickCount() + timeoutInMs;
    mLastFired      = 0;
    mCurrentQueued  = 0;

    mStarted        = CETimerManager::StartTimer(self(), whichThread);
    mDispatchThread = mStarted ? &whichThread : NULL;

    return (mDispatchThread != NULL);
}

void CETimer::StopTimer( void )
{
    CELock lock(mLock);

    CETimerManager::StopTimer(self());

    mStarted        = false;
    mDispatchThread = NULL;
    mCurrentQueued  = 0;
    mTimeoutInMs    = 0;
    mEventsCount    = 0;
    mNextFire       = CETimer::GetTickCount();
    mLastFired      = 0;
}

bool CETimer::TimerIsStarting( void )
{
    bool result = false;
    if (mTimeoutInMs != 0 && mEventsCount != 0)
    {
        // mEventsCount-= (mEventsCount != CONTINUOUSLY ? 1 : 0);
        mNextFire   = CETimer::GetTickCount() + mTimeoutInMs;
        result      = true;
    }
    return result;
}

bool CETimer::TimerIsExpired( unsigned int highValue, unsigned int lowValue )
{
    mNextFire  = CETimer::GetTickCount() + mTimeoutInMs;
    mLastFired = MACRO_MAKE_64(highValue, lowValue);
    mEventsCount -= (mEventsCount != 0 && mEventsCount != CETimer::CONTINUOUSLY ? 1 : 0);
    return (mEventsCount != 0 ? true : false);
}

void CETimer::TimerIsQueued( void )
{
    CELock lock(mLock);

    if ( mMaxQueued != CETimer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<unsigned int>(mMaxQueued) )
    {
        if (mDispatchThread != NULL)
        {
            if ((++ mCurrentQueued > mMaxQueued) && mStarted)
            {
                mStarted = false;
                CETimerManager::StopTimer(self());
                OUTPUT_WARN("Timer Object: Temporary stopped timer [ %s ]! current queued = [ %d ], max queue = [ %d ]", mName.GetBuffer(), mCurrentQueued, mMaxQueued);
            }
        }
    }
}

void CETimer::TimerIsUnqueued( void )
{
    CELock lock(mLock);

    if ( mMaxQueued != CETimer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<unsigned int>(mMaxQueued) )
    {
        if (mDispatchThread != NULL)
        {
           if ((-- mCurrentQueued < mMaxQueued) && (mStarted == false))
           {
                mStarted = CETimerManager::StartTimer(self(), *mDispatchThread);
                OUTPUT_WARN("Timer Object: Restarting timer [ %s ]! started = [ %s ], current queued = [ %d ], max queue = [ %d ]", mStarted ? "YES" : "NO", mName.GetBuffer(), mCurrentQueued, mMaxQueued);
           }
#ifdef _DEBUG

           else if (mStarted == false)
           {
               OUTPUT_WARN("Timer Object: Ignore restart timer. Timer [ %s ] is not started, current queued = [ %d ], max queue = [ %d ]", mName.GetBuffer(), mCurrentQueued, mMaxQueued);
           }
#endif  // _DEBUG
        }
    }
}
