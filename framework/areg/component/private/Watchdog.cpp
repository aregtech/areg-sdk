/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Watchdog.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread watchdog object implementation
 *
 ************************************************************************/

#include "areg/component/private/Watchdog.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/private/WatchdogManager.hpp"

#include <atomic>

Watchdog::GUARD_ID Watchdog::_generateId(void)
{
    static std::atomic<Watchdog::GUARD_ID> _id{ 0 };
    return (++_id);
}

Watchdog::Watchdog(ComponentThread& thread, uint32_t msTimeout /*= NECommon::WATCHDOG_IGNORE*/)
    : TimerBase         (TimerBase::eTimerType::TimerTypeWatchdog, thread.getName(), msTimeout, TimerBase::ONE_TIME)
    , mGuardId          ( _generateId() )
    , mSequence         ( 0u )
    , mComponentThread  ( thread )
{
}

Watchdog::Watchdog(WorkerThread& thread, uint32_t msTimeout /*= NECommon::WATCHDOG_IGNORE*/)
    : TimerBase         ( TimerBase::eTimerType::TimerTypeWatchdog, thread.getName(), msTimeout, TimerBase::ONE_TIME)
    , mGuardId          ( _generateId() )
    , mSequence         ( 0u )
    , mComponentThread  ( thread.getBindingComponentThread() )
{
}

Watchdog::~Watchdog(void)
{
    WatchdogManager::stopTimer(*this);
}

void Watchdog::startGuard(void)
{
    if (mTimeoutInMs != NECommon::WATCHDOG_IGNORE)
    {
        Lock lock(mLock);
        ASSERT(mHandle != nullptr);
        ++mSequence;
        mActive = WatchdogManager::startTimer(*this);
    }
}

void Watchdog::stopGuard( void )
{
    if (mTimeoutInMs != NECommon::WATCHDOG_IGNORE)
    {
        Lock lock(mLock);
        ASSERT(mHandle != nullptr);
        mActive = false;
        WatchdogManager::stopTimer(*this);
    }
}
