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
 * \brief       Areg Platform, Timer base class.
 *
 ************************************************************************/
#include "areg/component/TimerBase.hpp"

#include "areg/base/DateTime.hpp"

uint32_t TimerBase::getTickCount()
{
    return static_cast<uint32_t>(DateTime::getSystemTickCount());
}

TimerBase::TimerBase( const TimerType timerType
                    , const areg::String& timerName
                    , uint32_t timeoutMs    /*= areg::INVALID_TIMEOUT*/
                    , uint32_t eventCount   /*= TimerBase::CONTINUOUSLY*/)
    : mTimerType    ( timerType )
    , mHandle       ( nullptr   )
    , mName         ( timerName )
    , mTimeoutInMs  ( timeoutMs )
    , mEventsCount  ( eventCount)
    , mActive       ( false     )
    , mLock         ( false     )
{
    createWaitableTimer();
}

TimerBase::~TimerBase()
{
    destroyWaitableTimer();
}

bool TimerBase::createWaitableTimer()
{
    Lock lock( mLock );

    if ( (mHandle == nullptr) && (mTimeoutInMs != areg::INVALID_TIMEOUT) )
    {
        mHandle = _osCreateWaitableTimer( );
    }

    return (mHandle != nullptr);
}


void TimerBase::destroyWaitableTimer()
{
    Lock lock( mLock );

    TIMERHANDLE handle = mHandle;
    mHandle = nullptr;
    if ( handle != nullptr )
    {
        _osDestroyWaitableTimer( handle );
    }
}
