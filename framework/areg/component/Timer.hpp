#ifndef AREG_COMPONENT_TIMER_HPP
#define AREG_COMPONENT_TIMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/Timer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer class.
 *              Use to fire timer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/TimerBase.hpp"

#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class TimerConsumer;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Timer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Fires events at specified intervals. Supports periodic (single or multiple events) and
 *          continuous timers. Events are dispatched to the timer consumer on expiration.
 **/
class AREG_API Timer : public TimerBase
{
/************************************************************************/
// Friend classes to access protected members.
/************************************************************************/

    friend class TimerEvent;
    friend class TimerManager;

//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Timer::DEFAULT_MAXIMUM_QUEUE
     *          Default number of maximum queued number of timer events in dispatcher thread.
     **/
    static constexpr int32_t            DEFAULT_MAXIMUM_QUEUE= static_cast<int32_t>(5);            /*0x00000005*/

    /**
     * \brief   Timer::IGNORE_TIMER_QUEUE
     *          Defined to ignore number of maximum queued timer events in dispatcher thread.
     **/
    static constexpr int32_t            IGNORE_TIMER_QUEUE    = static_cast<int32_t>(0);            /*0x00000000*/

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes timer with consumer and optional configuration.
     *
     * \param   timerConsumer       The timer consumer to receive events.
     * \param   timerName           Optional timer name (must be unique, no backslashes).
     * \param   timeoutMs           Initial timeout in milliseconds (default: zero).
     * \param   maxQueued           Maximum queued events before throttling (for continuous timers
     *                              only).
     **/
    explicit Timer( TimerConsumer & timerConsumer
                  , const String & timerName = String::empty_string()
                  , uint32_t timeoutMs       = NECommon::INVALID_TIMEOUT
                  , int32_t maxQueued            = Timer::IGNORE_TIMER_QUEUE );
    /**
     * \brief   Destructor
     **/
    virtual ~Timer();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Starts timer with timeout and event count. Restarting an active timer cancels the
     *          previous one.
     *
     * \param   timeoutInMs     Timeout in milliseconds.
     * \param   eventCount      Number of events (CONTINUOUSLY for infinite).
     * \return  Returns true if timer was successfully started.
     **/
    bool start_timer(uint32_t timeoutInMs, uint32_t eventCount = TimerBase::CONTINUOUSLY);

    /**
     * \brief   Starts timer with timeout, thread, and event count. Restarting an active timer
     *          cancels the previous one.
     *
     * \param   timeoutInMs     Timeout in milliseconds.
     * \param   whichThread     The dispatcher thread to process events.
     * \param   eventCount      Number of events (CONTINUOUSLY for infinite).
     * \return  Returns true if timer was successfully started.
     * \note    Overload with explicit thread.
     **/
    bool start_timer(uint32_t timeoutInMs, DispatcherThread & whichThread, uint32_t eventCount = TimerBase::CONTINUOUSLY);

    /**
     * \brief   Stops the timer.
     **/
    void stop_timer();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the timer consumer.
     **/
    inline TimerConsumer & consumer() const;

    /**
     * \brief   Returns true if timer is stopped (timeout is zero).
     **/
    inline bool is_stopped() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Called by timer manager when timer expires. Returns false to stop timer, true to
     *          continue.
     *
     * \param   highValue       High 32-bit value of 64-bit UTC time.
     * \param   lowValue        Low 32-bit value of 64-bit UTC time.
     * \param   context         Optional OS-specific timer context (e.g., ID).
     * \return  Returns true if timer should remain active; false to stop.
     **/
    bool timer_is_expired(uint32_t highValue, uint32_t lowValue, ptr_type context);

    /**
     * \brief   Called by timer manager when timer starts.
     *
     * \param   highValue       High 32-bit value of 64-bit UTC time.
     * \param   lowValue        Low 32-bit value of 64-bit UTC time.
     * \param   context         Optional OS-specific timer context.
     **/
    void timer_starting(uint32_t highValue, uint32_t lowValue, ptr_type context);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Timer consumer object
     **/
    TimerConsumer &   mConsumer;
private:
    /**
     * \brief   Number of timer events, currently queued dispatcher.
     **/
    int32_t                 mCurrentQueued;
    /**
     * \brief   Maximum number of events, which should be queued in dispatcher.
     *          If currently queued events are more or equal than the maximum number
     *          of events currently queued in dispatcher thread, 
     **/
    const int32_t           mMaxQueued;
    /**
     * \brief   The Dispatcher thread where currently the timer is dispatched.
     **/
    DispatcherThread *  mDispatchThread;
    /**
     * \brief   First starting time. The value is system dependent.
     **/
    uint64_t            mStartedAt;
    /**
     * \brief   Last fired time. The value is system dependent.
     **/
    uint64_t            mExpiredAt;
    /**
     * \brief   Flag, indicating whether the timer is already started by timer manager or not.
     *          This flag is true, only when start_timer of timer manager is called. 
     *          Otherwise it is false.
     *          The timer can be active, but stopped. This might happen when timer event
     *          reached its maximum queue number in the dispatcher thread (mMaxQueued).
     *          In this case the timer might be stopped on timer manager side, 
     *          but it will remain active.
     **/
    bool                mStarted;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Increments queued count and stops timer if maximum reached. Called by TimerEvent
     *          constructor.
     **/
    void _queue_timer();

    /**
     * \brief   Decrements queued count and restarts timer if below maximum. Called by TimerEvent
     *          destructor.
     **/
    void _unqueue_timer();

    /**
     * \brief   Stops and releases the timer.
     **/
    inline void _stop_timer();

    /**
     * \brief   Returns reference to this Timer object.
     **/
    inline Timer & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Deleted default constructor.
     **/
    Timer() = delete;
    AREG_NOCOPY_NOMOVE(Timer);
};

//////////////////////////////////////////////////////////////////////////
// Timer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Timer & Timer::self()
{
    return (*this);
}

inline TimerConsumer& Timer::consumer() const
{
    return mConsumer;
}

inline bool Timer::is_stopped() const
{
    return (mTimeoutInMs == NECommon::INVALID_TIMEOUT);
}

#endif  // AREG_COMPONENT_TIMER_HPP
