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
namespace areg { class TimerConsumer; }
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Timer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The timer is used to fire event in a certain period of time.
 *          When timer is expired, it will trigger Timer Event and forward 
 *          to specified Timer Consumer object to process. There are 2 types
 *          of available timers:
 *              - Periodic timer to set the number of timer events to fire.
 *              - Continues timers run in cycle until are not manually stopped.
 **/
class AREG_API Timer : public areg::TimerBase
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
     * \brief   The constructor sets Timer object consumer and optional the name.
     *          If timer name is not nullptr and to provide uniqueness of names in the system, 
     *          the system will use passed name as a prefix.
     * \param   timerConsumer   The Timer Consumer object.
     * \param   timerName       The name of Timer. If this is not nullptr, the system will 
     *                          generate unique name using passed name as a prefix.
     *                          The timer objects must have unique names and should not
     *                          contain back slashes.
     * \param   timeoutMs       The timeout in milliseconds of the timer. By default it is zero.
     *                          The timeout can be as well set when starting the timer.
     * \param   maxQueued       The maximum number of timer events queued in dispatcher thread.
     *                          This parameter controls number of fired timer events in dispatcher thread.
     *                          If currently queued timer events are more or equal than this number,
     *                          the timer temporary will be stopped and fired again, when number timer
     *                          timer events are less than allowed maximum number of timer events in queue.
     *                          This parameter makes sense only for continues timer. For timers,
     *                          fired once or less than maxQueued timer, this parameter will be ignored
     *                          and play no role.
     **/
    explicit Timer( areg::TimerConsumer & timerConsumer
                  , const areg::String & timerName = areg::String::getEmptyString()
                  , uint32_t timeoutMs       = areg::INVALID_TIMEOUT
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
     * \brief   Call to start timer. The system will send and the
     *          the Timer consumer object will receive timer event to process
     *          every time when specified timeout in milliseconds expires.
     *          The events will be send specified event count times and will
     *          be dispatched in the current thread context. If eventCount is,
     *          the events will be sent continuously until timer is not stopped.
     *          If the timer was already started and requested new start,
     *          the ongoing timer will be stopped and canceled, and reactivated
     *          with new parameters.
     * \param   timeoutInMs Timeout to fire timer event
     * \param   eventCount  The number of events to fire. 
     *                      If this value is zero, no time event will be fired.
     *                      If this value is CONTINUOUSLY, the event fill be triggered
     *                      until it will not be stopped manually.
     *                      Otherwise, timer will be triggered until event count 
     *                      reached specified number.
     * \return  Returns true if Timer was successfully started.
     **/
    bool startTimer(uint32_t timeoutInMs, uint32_t eventCount = areg::TimerBase::CONTINUOUSLY);

    /**
     * \brief   Call to start timer. The system will send and the
     *          the Timer consumer object will receive timer event to process
     *          every time when specified timeout in milliseconds expires.
     *          The events will be send specified event count times and will
     *          be dispatched in the specified thread context. If eventCount is,
     *          the events will be sent continuously until timer is not stopped.
     *          If the timer was already started and requested new start,
     *          the ongoing timer will be stopped and canceled, and reactivated
     *          with new parameters.
     * \param   timeoutInMs Timeout to fire timer event.
     * \param   whichThread The dispatcher thread, which should process the fired
     *                      timer event. Should be valid dispatcher thread.
     * \param   eventCount  The number of events to fire. 
     *                      If this value is zero, no time event will be fired.
     *                      If this value is CONTINUOUSLY, the event fill be triggered
     *                      until it will not be stopped manually.
     *                      Otherwise, timer will be triggered until event count is not
     *                      reaching specified number.
     * \return  Returns true if Timer was successfully started.
     **/
    bool startTimer(uint32_t timeoutInMs, DispatcherThread & whichThread, uint32_t eventCount = areg::TimerBase::CONTINUOUSLY);

    /**
     * \brief   Call to stop previously started timer.
     **/
    void stopTimer();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns Timer Consumer object.
     **/
    inline areg::TimerConsumer & getConsumer() const;

    /**
     * \brief   Returns true if timer is stopped. The timer is stopped if timeout value is zero.
     **/
    inline bool isStopped() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Called by timer manager when timer is expired.
     *          The function should returns false to stop the timer.
     *          Otherwise, should return true.
     *          The passed parameters are low and high 32-bits of
     *          64-bit time in Coordinated Universal Time (UTC) format.
     *
     * \param   highValue   Th high 32-bit value to set.
     * \param   lowValue    The low 32-bit value to set.
     * \param   context     The optional timer context. It is OS and timer specific, can be an ID value.
     * \return  Returns true, if timer should still remain active.
     *          Otherwise, should return false to stop the timer.
     **/
    bool timerIsExpired(uint32_t highValue, uint32_t lowValue, ptr_type context);

    /**
     * \brief   Called by timer manager when timer is starting.
     *          The passed parameters are low and high 32-bits of
     *          64-bit time in Coordinated Universal Time (UTC) format.
     *
     * \param   highValue   Th high 32-bit value to set.
     * \param   lowValue    The low 32-bit value to set.
     * \param   context     The optional timer context. It is OS and timer specific, can be an ID value.
     **/
    void timerStarting(uint32_t highValue, uint32_t lowValue, ptr_type context);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Timer consumer object
     **/
    areg::TimerConsumer &   mConsumer;
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
     *          This flag is true, only when startTimer of timer manager is called. 
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
     * \brief   Triggered in Timer Event object constructor, indicating that timer is queued.
     *          The function will increase the timer queued count and if it reaches
     *          its maximum, it will stop timer on Timer Manager side, 
     *          but will not reset timer data.
     *          The function will be ignored if maximum queue count in zero and/or is less
     *          than the event count.
     **/
    void _queueTimer();

    /**
     * \brief   Triggered in Timer Event object destructor, indicating that timer is removed from queue.
     *          The function will decrease the number of queue count and it is less than the maximum
     *          queue count, it will start timer using internal saved data.
     *          The function will be ignored if timer was stopped manually or it is completed to be fired.
     *          The function will be ignored if the maximum queue count is zero.
     **/
    void _unqueueTimer();

    /**
     * \brief   Called to stop and release the timer.
     */
    inline void _stopTimer();

    /**
     * \brief   Returns reference to Timer object.
     **/
    inline Timer & self();

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
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

inline areg::TimerConsumer& Timer::getConsumer() const
{
    return mConsumer;
}

inline bool Timer::isStopped() const
{
    return (mTimeoutInMs == areg::INVALID_TIMEOUT);
}

#endif  // AREG_COMPONENT_TIMER_HPP
