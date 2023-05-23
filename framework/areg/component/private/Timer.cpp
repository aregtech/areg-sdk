/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Timer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Timer class.
 *              Use to fire timer.
 *
 ************************************************************************/
#include "areg/component/Timer.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/TimerEventData.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_Timer_startTimer);


//////////////////////////////////////////////////////////////////////////
// Timer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Timer::Timer( IETimerConsumer& timerConsumer
            , const String & timerName  /*= String::getEmptyString()*/
            , uint32_t timeoutMs        /*= NECommon::INVALID_TIMEOUT*/
            , int maxQueued             /*= Timer::DEFAULT_MAXIMUM_QUEUE*/)
    : TimerBase         ( TimerBase::eTimerType::TimerTypeNormal, NEUtilities::generateName(timerName), timeoutMs )
    , mConsumer         (timerConsumer)

    , mCurrentQueued    (0)
    , mMaxQueued        (maxQueued > 0 ? maxQueued : Timer::IGNORE_TIMER_QUEUE)
    , mDispatchThread   (nullptr)
    , mStartedAt        ( 0u )
    , mExpiredAt        ( 0u )
    , mStarted          (false)
{
}

Timer::~Timer(void)
{
    _stopTimer();
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

bool Timer::startTimer( void )
{
    return startTimer(mTimeoutInMs, 1);
}

void Timer::stopTimer(void)
{
    _stopTimer();
}

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

    if (eventCount != 0)
    {
        TRACE_DBG("Starting [ %s ] with timeout [ %u ] ms and event count [ %d ]", mName.getString(), timeoutInMs, eventCount);

        mTimeoutInMs    = timeoutInMs;
        mEventsCount    = eventCount;
        mCurrentQueued  = 0;
        mActive         = true;

        if (TimerBase::createWaitableTimer())
        {
            mStarted = TimerManager::startTimer(self(), whichThread);
            mDispatchThread = mStarted ? &whichThread : nullptr;
        }

        return (mDispatchThread != nullptr);
    }
    else
    {
        TRACE_WARN("Ignoring to start a timer [ %s ] with either 0 timeout or 0 event count", mName.getString());
        mTimeoutInMs = timeoutInMs;
        mEventsCount = 0;

        return false;
    }
}

bool Timer::timerIsExpired(unsigned int highValue, unsigned int lowValue, ptr_type /*context*/ )
{
    Lock lock(mLock);

    if (mExpiredAt != 0)
    {
        mStartedAt = mExpiredAt;
    }

    mExpiredAt = MACRO_MAKE_64(highValue, lowValue);
    mEventsCount -= (mEventsCount != 0 && mEventsCount != TimerBase::CONTINUOUSLY ? 1 : 0);
    mActive = mEventsCount != 0;

    if (mTimeoutInMs != NECommon::INVALID_TIMEOUT)
    {
        TimerEvent::sendEvent(*this, *mDispatchThread);
    }
    else
    {
        mActive = false;
    }

    return mActive;
}


void Timer::timerStarting(unsigned int highValue, unsigned int lowValue, ptr_type /*context*/)
{
    mStartedAt = MACRO_MAKE_64(highValue, lowValue);
    mExpiredAt = 0;
}

void Timer::_queueTimer( void )
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

void Timer::_unqueueTimer( void )
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

inline void Timer::_stopTimer(void)
{
    Lock lock(mLock);

    TimerManager::stopTimer(self());

    mStarted        = false;
    mActive         = false;
    mDispatchThread = nullptr;
    mCurrentQueued  = 0;
    mTimeoutInMs    = NECommon::INVALID_TIMEOUT;
    mEventsCount    = 0;
    mStartedAt      = 0;
    mExpiredAt      = 0;
}
