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
 * \brief       AREG Platform, Timer base class.
 *
 ************************************************************************/
#include "areg/component/TimerBase.hpp"

#include "areg/base/DateTime.hpp"

unsigned int TimerBase::getTickCount(void)
{
    return static_cast<unsigned int>(DateTime::getSystemTickCount());
}

TimerBase::TimerBase( const eTimerType timerType
                    , const String& timerName
                    , unsigned int timeoutMs    /*= NECommon::INVALID_TIMEOUT*/
                    , unsigned int eventCount   /*= TimerBase::CONTINUOUSLY*/)
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

TimerBase::~TimerBase(void)
{
    destroyWaitableTimer();
}

bool TimerBase::createWaitableTimer( void )
{
    Lock lock( mLock );

    if ( (mHandle == nullptr) && (mTimeoutInMs != NECommon::INVALID_TIMEOUT) )
    {
        mHandle = _osCreateWaitableTimer( );
    }

    return (mHandle != nullptr);
}


void TimerBase::destroyWaitableTimer( void )
{
    Lock lock( mLock );

    TIMERHANDLE handle = mHandle;
    mHandle = nullptr;
    if ( handle != nullptr )
    {
        _osDestroyWaitableTimer( handle );
    }
}
