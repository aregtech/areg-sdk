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
#include "areg/base/areg_global.h"

#include "areg/base/CommonDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TimerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for timers that trigger events at specified intervals; supports one-shot and
 *          continuous modes with optional watchdog protection.
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
     * \brief   Returns the system uptime in milliseconds since startup, wrapping after
     *          approximately 49.7 days.
     **/
    static uint32_t tick_count();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes timer with type, name, timeout, and event count.
     *
     * \param   timerType       Timer type: Normal or Watchdog.
     * \param   timerName       name of timer; can be empty.
     * \param   timeoutMs       Timeout in milliseconds; defaults to INVALID_TIMEOUT.
     * \param   eventCount      Number of events to fire: CONTINUOUSLY (indefinite), ONE_TIME
     *                          (single), or a specific count; zero disables firing.
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
     * \brief   Returns the timer timeout in milliseconds.
     **/
    inline uint32_t timeout() const;

    /**
     * \brief   Sets the timer timeout in milliseconds.
     *
     * \param   timeoutMs       Timeout value in milliseconds.
     **/
    inline void set_timeout(uint32_t timeoutMs);

    /**
     * \brief   Returns the number of events remaining to fire; zero if stopped, CONTINUOUSLY for
     *          infinite.
     **/
    inline uint32_t event_count() const;

    /**
     * \brief   Sets the number of timeout events to fire.
     *
     * \param   eventCount      Event count: CONTINUOUSLY (indefinite), ONE_TIME (single), or a
     *                          specific count; zero disables firing.
     **/
    inline void set_event_count(uint32_t eventCount);

    /**
     * \brief   Returns the timer name.
     **/
    inline const String& name() const;

    /**
     * \brief   Returns the OS timer handle.
     **/
    inline TIMERHANDLE handle() const;

    /**
     * \brief   Returns true if the timer is currently active.
     **/
    inline bool is_active() const;

    /**
     * \brief   Returns true if the timer is valid (has a defined timeout).
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns the timer type.
     **/
    inline TimerBase::TimerType timer_type() const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Creates an OS-specific timer handle. Has platform-dependent implementation.
     *
     * \return  Returns true if creation succeeded or timer was already created.
     **/
    bool create_waitable_timer();

    /**
     * \brief   Destroys the OS timer. Timer cannot be used after this call.
     **/
    void destroy_waitable_timer();

//////////////////////////////////////////////////////////////////////////
// OS specific hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Platform-specific implementation to create and return an OS timer handle.
     *
     * \return  OS timer handle.
     **/
    TIMERHANDLE _os_create();

    /**
     * \brief   Platform-specific implementation to destroy an OS timer.
     *
     * \param   handle      OS timer handle to destroy.
     **/
    void _os_destroy( TIMERHANDLE handle );

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

inline bool TimerBase::is_valid() const
{
    return ((mTimeoutInMs != areg::INVALID_TIMEOUT) && (mHandle != nullptr));
}

inline void TimerBase::set_event_count(uint32_t eventCount)
{
    mEventsCount = eventCount;
}

inline const String & TimerBase::name() const
{
    return mName;
}

inline uint32_t TimerBase::timeout() const
{
    return mTimeoutInMs;
}

inline void TimerBase::set_timeout(uint32_t timeoutMs)
{
    mTimeoutInMs = timeoutMs;
}

inline uint32_t TimerBase::event_count() const
{
    return mEventsCount;
}

inline TIMERHANDLE TimerBase::handle() const
{
    return mHandle;
}

inline bool TimerBase::is_active() const
{
    return mActive;
}

inline TimerBase::TimerType TimerBase::timer_type() const
{
    return mTimerType;
}

} // namespace areg
#endif  // AREG_COMPONENT_TIMERBASE_HPP
