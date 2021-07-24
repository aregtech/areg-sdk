/************************************************************************
 * \file        areg/component/Timer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Timer class.
 *              Use to fire timer.
 *
 ************************************************************************/
#include "areg/component/Timer.hpp"
#include "areg/component/private/TimerManager.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_Timer_startTimer);


//////////////////////////////////////////////////////////////////////////
// Timer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////

const unsigned int  Timer::INVALID_TIMEOUT          = static_cast<unsigned int>(0);

/**
 * \brief   This value is used to set continues Timer, which will not stop, until it is not requested to be stopped manually.
 **/
const unsigned int  Timer::CONTINUOUSLY             = static_cast<unsigned int>(-1);   /*0xFFFFFFFF*/
/**
 * \brief   Default number of maximum queued number of timer events in dispatcher thread.
 **/
const int           Timer::DEFAULT_MAXIMUM_QUEUE    = static_cast<int>(5);             /*0x00000005*/
/**
 * \brief   Defined to ignore number of maximum queued timer events in dispatcher thread.
 **/
const int           Timer::IGNORE_TIMER_QUEUE      = static_cast<int>(0);             /*0x00000000*/

const unsigned int  Timer::TIMEOUT_1_MS            = 1;

const unsigned int  Timer::TIMEOUT_100_MS          = (Timer::TIMEOUT_1_MS * 100);

const unsigned int  Timer::TIMEOUT_500_MS          = (Timer::TIMEOUT_100_MS * 5);

const unsigned int  Timer::TIMEOUT_1_SEC           = (Timer::TIMEOUT_1_MS * 1000);

const unsigned int  Timer::TIMEOUT_1_MIN           = (Timer::TIMEOUT_1_SEC * 60);


unsigned int Timer::getTickCount( void )
{
    return static_cast<unsigned int>(DateTime::getSystemTickCount());
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Timer::Timer(IETimerConsumer& timerConsumer, const char* timerName /*= NULL*/, int maxQueued /*= Timer::DEFAULT_MAXIMUM_QUEUE*/)
    : mConsumer         (timerConsumer)
    , mName             (NEUtilities::generateName(timerName))
    , mTimeoutInMs      (Timer::INVALID_TIMEOUT)
    , mEventsCount      (0)
    , mNextFire         (0)
    , mLastFired        (0)

    , mCurrentQueued    (0)
    , mMaxQueued        (maxQueued > 0 ? maxQueued : Timer::IGNORE_TIMER_QUEUE)
    , mDispatchThread   (NULL)
    , mStarted          (false)
    , mLock             ( )
{
    ; // do nothing
}

Timer::~Timer( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
bool Timer::startTimer( unsigned int timeoutInMs, unsigned int eventCount /*= Timer::CONTINUOUSLY*/ )
{
    return startTimer(timeoutInMs, DispatcherThread::getCurrentDispatcherThread(), eventCount);
}

bool Timer::startTimer(unsigned int timeoutInMs, DispatcherThread & whichThread, unsigned int eventCount /*= Timer::CONTINUOUSLY*/)
{
    TRACE_SCOPE(areg_component_Timer_startTimer);

    Lock lock(mLock);

    if (isActive())
    {
        TRACE_WARN("The timer [ %s ] is still active, going to stop first. Current timeout [ %u ] ms and event count [ %d]", getName(), mTimeoutInMs, mEventsCount);
        TimerManager::stopTimer(self());
    }

    TRACE_DBG("Starting [ %s ] with timeout [ %u ] ms and event count [ %d ]", getName().getString(), timeoutInMs, eventCount);

    mTimeoutInMs    = timeoutInMs;
    mEventsCount    = eventCount;
    mNextFire       = Timer::getTickCount() + timeoutInMs;
    mLastFired      = 0;
    mCurrentQueued  = 0;

    mStarted        = TimerManager::startTimer(self(), whichThread);
    mDispatchThread = mStarted && whichThread.isValid() ? &whichThread : NULL;

    return (mDispatchThread != NULL);
}

void Timer::stopTimer( void )
{
    Lock lock(mLock);

    TimerManager::stopTimer(self());

    mStarted        = false;
    mDispatchThread = NULL;
    mCurrentQueued  = 0;
    mTimeoutInMs    = Timer::INVALID_TIMEOUT;
    mEventsCount    = 0;
    mNextFire       = Timer::getTickCount();
    mLastFired      = 0;
}

bool Timer::timerIsStarting( void )
{
    bool result = false;
    if (isValid() && mEventsCount != 0)
    {
        // mEventsCount-= (mEventsCount != CONTINUOUSLY ? 1 : 0);
        mNextFire   = Timer::getTickCount() + mTimeoutInMs;
        result      = true;
    }
    return result;
}

bool Timer::timerIsExpired( unsigned int highValue, unsigned int lowValue )
{
    mNextFire  = Timer::getTickCount() + mTimeoutInMs;
    mLastFired = MACRO_MAKE_64(highValue, lowValue);
    mEventsCount -= (mEventsCount != 0 && mEventsCount != Timer::CONTINUOUSLY ? 1 : 0);
    return (mEventsCount != 0);
}

void Timer::queueTimer( void )
{
    Lock lock(mLock);

    if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<unsigned int>(mMaxQueued) )
    {
        if (mDispatchThread != NULL)
        {
            if ((++ mCurrentQueued > mMaxQueued) && mStarted)
            {
                mStarted = false;
                TimerManager::stopTimer(self());
                OUTPUT_WARN("Timer Object: Temporary stopped timer [ %s ]! current queued = [ %d ], max queue = [ %d ]", mName.getString(), mCurrentQueued, mMaxQueued);
            }
        }
    }
}

void Timer::unqueueTimer( void )
{
    Lock lock(mLock);

    if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<unsigned int>(mMaxQueued) )
    {
        if (mDispatchThread != NULL)
        {
           if ((-- mCurrentQueued < mMaxQueued) && (mStarted == false))
           {
                mStarted = TimerManager::startTimer(self(), *mDispatchThread);
                OUTPUT_WARN("Timer Object: Restarting timer [ %s ]! started = [ %s ], current queued = [ %d ], max queue = [ %d ]", mStarted ? "YES" : "NO", mName.getString(), mCurrentQueued, mMaxQueued);
           }
#ifdef _DEBUG

           else if (mStarted == false)
           {
               OUTPUT_WARN("Timer Object: Ignore restart timer. Timer [ %s ] is not started, current queued = [ %d ], max queue = [ %d ]", mName.getString(), mCurrentQueued, mMaxQueued);
           }
#endif  // _DEBUG
        }
    }
}
