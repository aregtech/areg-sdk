#ifndef AREG_COMPONENT_TIMERBASE_HPP
#define AREG_COMPONENT_TIMERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/TimerBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Timer base class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SynchObjects.hpp"

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
    enum class eTimerType : uint8_t
    {
          TimerTypeNormal       //! Normal timer
        , TimerTypeWatchdog     //! Watchdog timer.
    };

    /**
     * \brief   TimerBase::CONTINUOUSLY
     *          This value is used to set continues Timer, which will not
     *          stop, until it is not requested to be stopped manually.
     **/
    static constexpr unsigned int   CONTINUOUSLY{ static_cast<unsigned int>(~0) };    /*0xFFFFFFFF*/

    /**
     * \brief   TimerBase::ONE_TIME
     *          Timer which is fired one time.
     */
    static constexpr unsigned int   ONE_TIME    { static_cast<unsigned int>(1u) };

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed
     *          since the system was started, up to 49.7 days.
     **/
    static unsigned int getTickCount( void );

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
    TimerBase( const eTimerType timerType
             , const String & timerName
             , unsigned int timeoutMs   = NECommon::INVALID_TIMEOUT
             , unsigned int eventCount  = TimerBase::CONTINUOUSLY );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~TimerBase( void );

/************************************************************************/
// TimerBase class overrides
/************************************************************************/
public:
    /**
     * \brief   Call to start timer. The timer should have valid timeout.
     * 
     * \return  Returns true if succeeded to start timer.
     */
    virtual bool startTimer( void ) = 0;

    /**
     * \brief   Call to stop previously started timer.
     **/
    virtual void stopTimer( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the timeout of timer in milliseconds.
     **/
    inline unsigned int getTimeout( void ) const;

    /**
     * \brief   Set the timeout of timer in milliseconds.
     * \param   timeoutMs   The timeout of timer in milliseconds.
     **/
    inline void setTimeout(unsigned int timeoutMs);

    /**
     * \brief   Returns the amount of events, which timer still needs to send.
     *          This function returns zero, if timer is stopped (automatically or manually),
     *          and returns Timer::CONTINUOUSLY for continues events.
     **/
    inline unsigned int getEventCount(void) const;

    /**
     * \brief   Set the number of timeout events to fire.
     * 
     * \param   eventCount  The amount of times that event should run:
     *                      - TimerBase::CONTINUOUSLY if runs continuously until manually stopped.
     *                      - TimerBase::ONE_TIME if runs one time, then automatically stopped.
     *                      If values is zero, does not run the timer.
     *                      Any other number defines the amount of timeout events to fire.
     **/
    inline void setEventCount(unsigned int eventCount);

    /**
     * \brief   Returns the name of timer.
     **/
    inline const String& getName(void) const;

    /**
     * \brief   Returns the handle of the system waitable timer.
     **/
    inline TIMERHANDLE getHandle( void ) const;

    /**
     * \brief   Returns true if timer is active.
     **/
    inline bool isActive( void ) const;

    /**
     * \brief   Returns true if timer is valid. The valid timer has timeout
     *          not equal to NECommon::INVALID_TIMEOUT;
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns the type of the timer.
     **/
    inline TimerBase::eTimerType getTimerType( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected methods
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Call to creates system waitable timer.
     *          The timer can be used if succeeded to create handle.
     *          It has OS specific implementation
     * 
     * \param   timer   The instance of the timer to create system waitable timer.
     * \return  Returns true if succeeded to create system timer or the timer was already created.
     **/
    bool createWaitableTimer( void );

    /**
     * \brief   Call to destroy system waitable.
     *          After calling this method, the timer cannot be used anymore.
     * 
     * \param   timer   The instance of the timer to destroy system waitable timer.
     **/
    void destroyWaitableTimer( void );

//////////////////////////////////////////////////////////////////////////
// OS specific hidden members
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS cpecific implementation of creating waitable timer and
     *          returns the handle of created timer.
     **/
    TIMERHANDLE _osCreateWaitableTimer( void );

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
    const eTimerType    mTimerType;
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
    unsigned int        mTimeoutInMs;
    /**
     * \brief   The amount of events to fire
     **/
    unsigned int        mEventsCount;
    /**
     * \brief   Flag, indicating whether the timer is active or not.
     **/
    bool                mActive;
    /**
     * \brief   Synchronization object
     **/
    ResourceLock        mLock;

private:
    TimerBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE(TimerBase);
};

//////////////////////////////////////////////////////////////////////////
// Timer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool TimerBase::isValid( void ) const
{
    return ((mTimeoutInMs != NECommon::INVALID_TIMEOUT) && (mHandle != nullptr));
}

inline void TimerBase::setEventCount(unsigned int eventCount)
{
    mEventsCount = eventCount;
}

inline const String & TimerBase::getName( void ) const
{
    return mName;
}

inline unsigned int TimerBase::getTimeout( void ) const
{
    return mTimeoutInMs;
}

inline void TimerBase::setTimeout(unsigned int timeoutMs)
{
    mTimeoutInMs = timeoutMs;
}

inline unsigned int TimerBase::getEventCount(void) const
{
    return mEventsCount;
}

inline TIMERHANDLE TimerBase::getHandle(void) const
{
    return mHandle;
}

inline bool TimerBase::isActive(void) const
{
    return mActive;
}

inline TimerBase::eTimerType TimerBase::getTimerType(void) const
{
    return mTimerType;
}

#endif  // AREG_COMPONENT_TIMERBASE_HPP
