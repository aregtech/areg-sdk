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
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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
    typedef enum class E_TimerType : uint8_t
    {
          TimerTypeNormal       //! Normal timer
        , TimerTypeWatchdog     //! Watchdog timer.
    } eTimerType;

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
     **/
    explicit TimerBase( const eTimerType timerType, const String & timerName, unsigned int timeoutMs = NECommon::INVALID_TIMEOUT );

public:
    /**
     * \brief   Destructor.
     **/
    virtual ~TimerBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
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

protected:

    /**
     * \brief   Called by timer manager when timer is expired.
     *          The function should returns false to stop the timer.
     *          Otherwise, should return true.
     *          The passed parameters are low and high 32-bits of
     *          64-bit time in Coordinated Universal Time (UTC) format.
     *
     * \param   highValue   Th high 32-bit value to set.
     * \param   lowValue    The low 32-bit value to set.
     * \return  Returns true, if timer should still remain active.
     *          Otherwise, should return false to stop the timer.
     **/
    virtual bool timerIsExpired(unsigned int highValue, unsigned int lowValue) = 0;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the name of timer. 
     **/
    inline const String & getName( void ) const;

    /**
     * \brief   Returns the timeout of timer in milliseconds.
     **/
    inline unsigned int getTimeout( void ) const;

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
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The type of the timer.
     */
    eTimerType      mTimerType;
    /**
     * \brief   Timer name. If not empty, it is unique name
     **/
    String          mName;
    /**
     * \brief   Timeout to fire timer.
     **/
    unsigned int    mTimeoutInMs;

    /**
     * \brief   Flag, indicating whether the timer is already started by timer manager or not.
     **/
    bool            mStarted;
    /**
     * \brief   Flag, indicating whether the timer is active or not.
     **/
    bool            mActive;
    /**
     * \brief   Synchronization object
     **/
    ResourceLock    mLock;

private:
    TimerBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE(TimerBase);
};

//////////////////////////////////////////////////////////////////////////
// Timer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool TimerBase::isValid( void ) const
{
    return (mTimeoutInMs != NECommon::INVALID_TIMEOUT);
}

inline const String & TimerBase::getName( void ) const
{
    return mName;
}

inline unsigned int TimerBase::getTimeout( void ) const
{
    return mTimeoutInMs;
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
