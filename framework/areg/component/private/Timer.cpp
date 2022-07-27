/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/Timer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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

unsigned int Timer::getTickCount( void )
{
    return static_cast<unsigned int>(DateTime::getSystemTickCount());
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Timer::Timer(IETimerConsumer& timerConsumer, const String & timerName /*= String::EmptyString*/, int maxQueued /*= Timer::DEFAULT_MAXIMUM_QUEUE*/)
    : mConsumer         (timerConsumer)
    , mName             (NEUtilities::generateName(timerName))
    , mTimeoutInMs      (Timer::INVALID_TIMEOUT)
    , mEventsCount      (0)
    , mNextFire         (0)
    , mLastFired        (0)

    , mCurrentQueued    (0)
    , mMaxQueued        (maxQueued > 0 ? maxQueued : Timer::IGNORE_TIMER_QUEUE)
    , mDispatchThread   (nullptr)
    , mStarted          (false)
    , mLock             ( )
{
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
        TRACE_WARN("The timer [ %s ] is still active, going to stop first. Current timeout [ %u ] ms and event count [ %d]"
                    , mName.getString()
                    , mTimeoutInMs
                    , mEventsCount);

        TimerManager::stopTimer(self());
    }

    TRACE_DBG("Starting [ %s ] with timeout [ %u ] ms and event count [ %d ]", mName.getString(), timeoutInMs, eventCount);

    mTimeoutInMs    = timeoutInMs;
    mEventsCount    = eventCount;
    mNextFire       = Timer::getTickCount() + timeoutInMs;
    mLastFired      = 0;
    mCurrentQueued  = 0;

    mStarted        = TimerManager::startTimer(self(), whichThread);
    mDispatchThread = mStarted && whichThread.isValid() ? &whichThread : nullptr;

    return (mDispatchThread != nullptr);
}

void Timer::stopTimer( void )
{
    Lock lock(mLock);

    TimerManager::stopTimer(self());

    mStarted        = false;
    mDispatchThread = nullptr;
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
        if (mDispatchThread != nullptr)
        {
            if ((++ mCurrentQueued > mMaxQueued) && mStarted)
            {
                mStarted = false;
                TimerManager::stopTimer(self());
                OUTPUT_WARN("Timer Object: Temporary stopped timer [ %s ]! current queued = [ %d ], max queue = [ %d ]"
                                , mName.getString()
                                , mCurrentQueued
                                , mMaxQueued);
            }
        }
    }
}

void Timer::unqueueTimer( void )
{
    Lock lock(mLock);

    if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<unsigned int>(mMaxQueued) )
    {
        if (mDispatchThread != nullptr)
        {
           if ((-- mCurrentQueued < mMaxQueued) && (mStarted == false))
           {
                mStarted = TimerManager::startTimer(self(), *mDispatchThread);
                OUTPUT_WARN("Timer Object: Restarting timer [ %s ]! started = [ %s ], current queued = [ %d ], max queue = [ %d ]"
                                , mStarted ? "YES" : "NO"
                                , mName.getString()
                                , mCurrentQueued
                                , mMaxQueued);
           }
#ifdef _DEBUG

           else if (mStarted == false)
           {
               OUTPUT_WARN("Timer Object: Ignore restart timer. Timer [ %s ] is not started, current queued = [ %d ], max queue = [ %d ]"
                                , mName.getString()
                                , mCurrentQueued
                                , mMaxQueued);
           }
#endif  // _DEBUG
        }
    }
}
