/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
namespace areg {
//////////////////////////////////////////////////////////////////////////
// Timer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Timer::Timer( TimerConsumer& timerConsumer
            , const String & timerName  /*= String::empty_string()*/
            , uint32_t timeoutMs        /*= areg::INVALID_TIMEOUT*/
            , int32_t maxQueued         /*= Timer::DEFAULT_MAXIMUM_QUEUE*/
            , areg::EventPriority prio  /*= areg::DefaultPriority*/)
    : TimerBase         (TimerBase::TimerType::PerThreadTimer, areg::generate_name(timerName), timeoutMs, TimerBase::CONTINUOUSLY, prio)
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
    bool active = false;
    {
        Lock lock(mLock);
        active = is_active();
    }

    if (active)
    {
        TimerManager::stop_timer(self());
    }

    if (eventCount == 0)
    {
        Lock lock(mLock);
        mTimeoutInMs.store(timeoutInMs, std::memory_order_relaxed);
        mEventsCount.store(0u, std::memory_order_relaxed);
        mStarted        = false;
        mActive.store(false, std::memory_order_relaxed);
        mDispatchThread = nullptr;
        mCurrentQueued  = 0;

        return false;
    }

    // Publish the dispatch thread before registering so an early expiry always sees a valid target.
    bool canStart = false;
    {
        Lock lock(mLock);

        mTimeoutInMs.store(timeoutInMs, std::memory_order_relaxed);
        mEventsCount.store(eventCount, std::memory_order_relaxed);
        mCurrentQueued  = 0;
        mStarted        = false;
        canStart        = whichThread.is_running() && TimerBase::create_waitable_timer();
        mActive.store(canStart, std::memory_order_relaxed);
        mDispatchThread = canStart ? &whichThread : nullptr;
    }

    if (canStart == false)
    {
        return false;
    }

    // Register and arm the timer with the manager OUTSIDE mLock (keeps resource-lock -> mLock order).
    // Only a failed start resets the active state.
    const bool started = TimerManager::start_timer(self(), whichThread);

    Lock lock(mLock);
    mStarted = started;
    if (started == false)
    {
        mActive.store(false, std::memory_order_relaxed);
        mDispatchThread = nullptr;
    }

    return (mDispatchThread != nullptr);
}

void Timer::stop_timer()
{
    _stop_timer();
}

bool Timer::timer_is_expired(uint32_t highValue, uint32_t lowValue, ptr_type /*context*/)
{
    Lock lock(mLock);
    uint32_t eventCount{ mEventsCount.load(std::memory_order_relaxed) };
    if (eventCount == 0u)
    {
        mActive.store(false, std::memory_order_relaxed);
        return false;
    }

    if (mExpiredAt != 0)
    {
        mStartedAt = mExpiredAt;
    }

    mExpiredAt = areg::make64(highValue, lowValue);

    const bool isContinuous = (eventCount == TimerBase::CONTINUOUSLY);
    if (!isContinuous)
    {
        --eventCount;
        mEventsCount.store(eventCount, std::memory_order_relaxed);
    }

    bool active{ false };
    if ((mTimeoutInMs.load(std::memory_order_relaxed) != areg::INVALID_TIMEOUT) && (mDispatchThread != nullptr))
    {
        // the state has to be up to date before the event is queued.
        active = isContinuous || (eventCount != 0u);
        mActive.store(active, std::memory_order_relaxed);
        if (TimerEvent::send_event(*this, *mDispatchThread, priority()) == false)
        {
            active = false;
            mActive.store(false, std::memory_order_relaxed);
        }
    }
    else
    {
        mActive.store(false, std::memory_order_relaxed);
    }

    return active;
}

void Timer::timer_starting(uint32_t highValue, uint32_t lowValue, ptr_type /*context*/)
{
    mStartedAt = areg::make64(highValue, lowValue);
    mExpiredAt = 0;
}

void Timer::_queue_timer()
{
    bool needStop = false;
    {
        Lock lock(mLock);

        if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && event_count() > static_cast<uint32_t>(mMaxQueued) )
        {
            if (mDispatchThread != nullptr)
            {
                if ((++ mCurrentQueued > mMaxQueued) && mStarted)
                {
                    mStarted = false;
                    needStop = true;
                }
            }
        }
    }

    if (needStop)
        TimerManager::stop_timer(self());
}

void Timer::_unqueue_timer()
{
    DispatcherThread * startThread = nullptr;
    {
        Lock lock(mLock);

        if ( mMaxQueued != Timer::IGNORE_TIMER_QUEUE && event_count() > static_cast<uint32_t>(mMaxQueued) )
        {
            if (mDispatchThread != nullptr)
            {
               if ((-- mCurrentQueued < mMaxQueued) && (mStarted == false))
               {
                    startThread = mDispatchThread;
               }
            }
        }
    }

    if (startThread != nullptr)
    {
        const bool started = TimerManager::start_timer(self(), *startThread);
        Lock lock(mLock);
        mStarted = started;
    }
}

void Timer::_stop_timer()
{
    TimerManager::stop_timer(self());

    Lock lock(mLock);

    mStarted        = false;
    mActive.store(false, std::memory_order_relaxed);
    mDispatchThread = nullptr;
    mCurrentQueued  = 0;
    mTimeoutInMs.store(areg::INVALID_TIMEOUT, std::memory_order_relaxed);
    mEventsCount.store(0u, std::memory_order_relaxed);
    mStartedAt      = 0;
    mExpiredAt      = 0;
}

} // namespace areg
