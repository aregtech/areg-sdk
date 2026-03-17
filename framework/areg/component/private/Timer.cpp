/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Timer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer class.
 *              Use to fire timer.
 *
 ************************************************************************/
#include "areg/component/Timer.hpp"
#include "areg/component/private/TimerManager.hpp"
#include "areg/component/private/TimerEventData.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/logging/areg_log.h"
namespace areg {

DEF_LOG_SCOPE(areg_component_Timer_startTimer);


//////////////////////////////////////////////////////////////////////////
// Timer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Timer::Timer( TimerConsumer& timerConsumer
            , const String & timerName  /*= String::empty_string()*/
            , uint32_t timeoutMs        /*= areg::INVALID_TIMEOUT*/
            , int32_t maxQueued             /*= Timer::DEFAULT_MAXIMUM_QUEUE*/)
    : TimerBase         (TimerBase::TimerType::PerThreadTimer, areg::generate_name(timerName), timeoutMs)
    , mConsumer         (timerConsumer)

    , mCurrentQueued    (0)
    , mMaxQueued        (maxQueued > 0 ? maxQueued : Timer::IGNORE_TIMER_QUEUE)
    , mDispatchThread   (nullptr)
    , mStartedAt        ( 0u )
    , mExpiredAt        ( 0u )
    , mStarted          (false)
{
}

Timer::~Timer()
{
    _stop_timer();
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

bool Timer::start_timer( uint32_t timeoutInMs, uint32_t eventCount /*= Timer::CONTINUOUSLY*/ )
{
    return start_timer(timeoutInMs, DispatcherThread::current_dispatcher_thread(), eventCount);
}

bool Timer::start_timer(uint32_t timeoutInMs, DispatcherThread & whichThread, uint32_t eventCount /*= Timer::CONTINUOUSLY*/)
{
    LOG_SCOPE(areg_component_Timer_startTimer);

    Lock lock(mLock);

    if (is_active())
    {
        LOG_WARN("The timer [ %s ] is still active, going to stop first. Current timeout [ %u ] ms and event count [ %d]"
                    , mName.as_string()
                    , mTimeoutInMs
                    , mEventsCount);

        TimerManager::stop_timer(self());
    }

    if (eventCount != 0)
    {
        LOG_DBG("Starting [ %s ] with timeout [ %u ] ms and event count [ %d ]", mName.as_string(), timeoutInMs, eventCount);

        mTimeoutInMs    = timeoutInMs;
        mEventsCount    = eventCount;
        mCurrentQueued  = 0;

        if (TimerBase::create_waitable_timer())
        {
            mActive         = true;
            mStarted        = TimerManager::start_timer(self(), whichThread);
            mDispatchThread = mStarted ? &whichThread : nullptr;
        }

        return (mDispatchThread != nullptr);
    }
    else
    {
        LOG_WARN("Ignoring to start a timer [ %s ] with either 0 timeout or 0 event count", mName.as_string());
        mTimeoutInMs = timeoutInMs;
        mEventsCount = 0;

        return false;
    }
}

void Timer::stop_timer()
{
    _stop_timer();
}

bool Timer::timer_is_expired(uint32_t highValue, uint32_t lowValue, ptr_type /*context*/ )
{
    Lock lock(mLock);

    if (mExpiredAt != 0)
    {
        mStartedAt = mExpiredAt;
    }

    mExpiredAt = areg::make64(highValue, lowValue);
    mEventsCount -= (mEventsCount != 0 && mEventsCount != TimerBase::CONTINUOUSLY ? 1 : 0);
    mActive = mEventsCount != 0;

    if (mTimeoutInMs != areg::INVALID_TIMEOUT)
    {
        TimerEvent::send_event(*this, *mDispatchThread);
    }
    else
    {
        mActive = false;
    }

    return mActive;
}

void Timer::timer_starting(uint32_t highValue, uint32_t lowValue, ptr_type /*context*/)
{
    mStartedAt = areg::make64(highValue, lowValue);
    mExpiredAt = 0;
}

void Timer::_queue_timer()
{
    Lock lock(mLock);

    if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<uint32_t>(mMaxQueued) )
    {
        if (mDispatchThread != nullptr)
        {
            if ((++ mCurrentQueued > mMaxQueued) && mStarted)
            {
                mStarted = false;
                TimerManager::stop_timer(self());
            }
        }
    }
}

void Timer::_unqueue_timer()
{
    Lock lock(mLock);

    if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && mEventsCount > static_cast<uint32_t>(mMaxQueued) )
    {
        if (mDispatchThread != nullptr)
        {
           if ((-- mCurrentQueued < mMaxQueued) && (mStarted == false))
           {
                mStarted = TimerManager::start_timer(self(), *mDispatchThread);
           }
        }
    }
}

void Timer::_stop_timer()
{
    Lock lock(mLock);

    TimerManager::stop_timer(self());

    mStarted        = false;
    mActive         = false;
    mDispatchThread = nullptr;
    mCurrentQueued  = 0;
    mTimeoutInMs    = areg::INVALID_TIMEOUT;
    mEventsCount    = 0;
    mStartedAt      = 0;
    mExpiredAt      = 0;
}

} // namespace areg
