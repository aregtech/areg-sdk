#ifndef AREG_COMPONENT_TIMERBASE_HPP
#define AREG_COMPONENT_TIMERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/TimerBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer base class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/CommonDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TimerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The timer is used to fire event in a certain period of time.
 *          This class is a base class for all types of timers like Timer
 *          and Watchdog used to control thread executions.
 **/
class AREG_API TimerBase
{
//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   The types of supported timers.
     *          At the moment it supports normal timer and watchdog.
     */
    enum class TimerType    : uint8_t
    {
          PerThreadTimer       //! Normal timer
        , WatchdogTimer     //! Watchdog timer.
    };

    /**
     * \brief   TimerBase::CONTINUOUSLY
     *          This value is used to set continues Timer, which will not
     *          stop, until it is not requested to be stopped manually.
     **/
    static constexpr uint32_t   CONTINUOUSLY{ static_cast<uint32_t>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   TimerBase::ONE_TIME
     *          Timer which is fired one time.
     */
    static constexpr uint32_t   ONE_TIME    { static_cast<uint32_t>(1u) };

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed
     *          since the system was started, up to 49.7 days.
     **/
    static uint32_t getTickCount();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The protected constructor that gets the timer name and timeout.
     *          By default, the timeout is invalid.
     * \param   timerType   The type of timer. Currently it is either Normal or Watchdog.
     * \param   timerName   The name of Timer. Can be empty if no name is needed.
     * \param   timeoutMs   The timer timeout in milliseconds. By default it is zero.
     * \param   eventCount  The amount of times that event should run:
     *                      - TimerBase::CONTINUOUSLY if runs continuously until manually stopped.
     *                      - TimerBase::ONE_TIME if runs one time, then automatically stopped.
     *                      If values is zero, does not run the timer.
     *                      Any other number defines the amount of timeout events to fire.
     **/
    TimerBase( const TimerType timerType
             , const String & timerName
             , uint32_t timeoutMs   = areg::INVALID_TIMEOUT
             , uint32_t eventCount  = TimerBase::CONTINUOUSLY );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~TimerBase();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the timeout of timer in milliseconds.
     **/
    inline uint32_t getTimeout() const;

    /**
     * \brief   Set the timeout of timer in milliseconds.
     * \param   timeoutMs   The timeout of timer in milliseconds.
     **/
    inline void setTimeout(uint32_t timeoutMs);

    /**
     * \brief   Returns the amount of events, which timer still needs to send.
     *          This function returns zero, if timer is stopped (automatically or manually),
     *          and returns Timer::CONTINUOUSLY for continues events.
     **/
    inline uint32_t getEventCount() const;

    /**
     * \brief   Set the number of timeout events to fire.
     * 
     * \param   eventCount  The amount of times that event should run:
     *                      - TimerBase::CONTINUOUSLY if runs continuously until manually stopped.
     *                      - TimerBase::ONE_TIME if runs one time, then automatically stopped.
     *                      If values is zero, does not run the timer.
     *                      Any other number defines the amount of timeout events to fire.
     **/
    inline void setEventCount(uint32_t eventCount);

    /**
     * \brief   Returns the name of timer.
     **/
    inline const String& getName() const;

    /**
     * \brief   Returns the handle of the system waitable timer.
     **/
    inline TIMERHANDLE getHandle() const;

    /**
     * \brief   Returns true if timer is active.
     **/
    inline bool isActive() const;

    /**
     * \brief   Returns true if timer is valid. The valid timer has timeout
     *          not equal to areg::INVALID_TIMEOUT;
     **/
    inline bool isValid() const;

    /**
     * \brief   Returns the type of the timer.
     **/
    inline TimerBase::TimerType getTimerType() const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Call to creates system waitable timer.
     *          The timer can be used if succeeded to create handle.
     *          It has OS specific implementation
     * \return  Returns true if succeeded to create system timer or the timer was already created.
     **/
    bool createWaitableTimer();

    /**
     * \brief   Call to destroy system waitable.
     *          After calling this method, the timer cannot be used anymore.
     **/
    void destroyWaitableTimer();

//////////////////////////////////////////////////////////////////////////
// OS specific hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS specific implementation of creating waitable timer and
     *          returns the handle of created timer.
     **/
    TIMERHANDLE _osCreateWaitableTimer();

    /**
     * \brief   OS specific implementation to destroy waitable timer.
     **/
    void _osDestroyWaitableTimer( TIMERHANDLE handle );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The type of the timer.
     */
    const TimerType     mTimerType;
    /**
     * \brief   The timer handle.
     */
    TIMERHANDLE         mHandle;
    /**
     * \brief   Timer name. If not empty, it is unique name
     **/
    const String        mName;
    /**
     * \brief   Timeout to fire timer.
     **/
    uint32_t        mTimeoutInMs;
    /**
     * \brief   The amount of events to fire
     **/
    uint32_t        mEventsCount;
    /**
     * \brief   Flag, indicating whether the timer is active or not.
     **/
    bool                mActive;
    /**
     * \brief   Synchronization object
     **/
    ResourceLock        mLock;

private:
    TimerBase() = delete;
    AREG_NOCOPY_NOMOVE(TimerBase);
};

//////////////////////////////////////////////////////////////////////////
// Timer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool TimerBase::isValid() const
{
    return ((mTimeoutInMs != areg::INVALID_TIMEOUT) && (mHandle != nullptr));
}

inline void TimerBase::setEventCount(uint32_t eventCount)
{
    mEventsCount = eventCount;
}

inline const String & TimerBase::getName() const
{
    return mName;
}

inline uint32_t TimerBase::getTimeout() const
{
    return mTimeoutInMs;
}

inline void TimerBase::setTimeout(uint32_t timeoutMs)
{
    mTimeoutInMs = timeoutMs;
}

inline uint32_t TimerBase::getEventCount() const
{
    return mEventsCount;
}

inline TIMERHANDLE TimerBase::getHandle() const
{
    return mHandle;
}

inline bool TimerBase::isActive() const
{
    return mActive;
}

inline TimerBase::TimerType TimerBase::getTimerType() const
{
    return mTimerType;
}

#endif  // AREG_COMPONENT_TIMERBASE_HPP
