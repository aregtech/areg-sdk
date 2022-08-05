/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Watchdog.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
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

Watchdog::Watchdog(ComponentThread& thread, uint32_t msTimeout /*= Watchdog::NO_WATCHDOG*/)
    : mGuardId          ( _generateId() )
    , mTimeoutMs        ( msTimeout )
    , mSequence         ( 0u )
    , mComponentThread  ( thread )
    , mWorkerThread     ( nullptr )
    , mTimer            ( nullptr )
    , mIsActive         ( false )
    , mLock             ( false )
{
    mTimer   = Watchdog::_createWaitableTimer(*this);
}

Watchdog::Watchdog(WorkerThread& thread, uint32_t msTimeout /*= Watchdog::NO_WATCHDOG*/)
    : mGuardId          ( _generateId() )
    , mTimeoutMs        ( msTimeout )
    , mSequence         ( 0u )
    , mComponentThread  ( thread.getBindingComponentThread() )
    , mWorkerThread     ( &thread )
    , mTimer            ( nullptr )
    , mIsActive         ( false )
    , mLock             ( false )
{
    mTimer = Watchdog::_createWaitableTimer(*this);
}

Watchdog::~Watchdog(void)
{
    Watchdog::_destroyWaitableTimer(*this);
}

void Watchdog::startGuard(void)
{
    if (mTimeoutMs != Watchdog::NO_WATCHDOG)
    {
        Lock lock(mLock);
        ASSERT(mTimer != nullptr);
        ++mSequence;
        mIsActive = WatchdogManager::startTimer(*this);
    }
}

void Watchdog::stopGuard( void )
{
    if (mTimeoutMs != Watchdog::NO_WATCHDOG)
    {
        Lock lock(mLock);
        ASSERT(mTimer != nullptr);
        mIsActive = false;
        WatchdogManager::stopTimer(*this);
    }
}

const Watchdog::GUARD_ID Watchdog::getId(void) const
{
    return mGuardId;
}

const Watchdog::SEQUENCE_ID Watchdog::getSequence(void) const
{
    return mSequence;
}

const String& Watchdog::getName(void) const
{
    return (mWorkerThread != nullptr ? mWorkerThread->getName() : mComponentThread.getName());
}

const uint32_t Watchdog::getTimeout(void) const
{
    return mTimeoutMs;
}

const DispatcherThread& Watchdog::getThread(void) const
{
    return (mWorkerThread != nullptr ? static_cast<const DispatcherThread&>(*mWorkerThread) : static_cast<const DispatcherThread&>(mComponentThread));
}

const ComponentThread& Watchdog::getComponentThread(void) const
{
    return mComponentThread;
}

const WorkerThread* Watchdog::getWorkerThread(void) const
{
    return mWorkerThread;
}

TIMERHANDLE Watchdog::getHandle(void) const
{
    return mTimer;
}

bool Watchdog::isActive(void) const
{
    return mIsActive;
}

Watchdog::WATCHDOG_ID Watchdog::makeWatchdogId(void)
{
    return Watchdog::makeWatchdogId(mGuardId, mSequence);
}

Watchdog::WATCHDOG_ID Watchdog::makeWatchdogId(GUARD_ID guardId, SEQUENCE_ID sequence)
{
#if defined(BIT64)
    return static_cast<id_type>(MACRO_MAKE_64(guardId, sequence));
#else   // !defined(BIT64)
    return static_cast<id_type>(MACRO_MAKE_32(guardId, sequence));
#endif  // defined(BIT64)
}

Watchdog::GUARD_ID Watchdog::makeGuardId(Watchdog::WATCHDOG_ID watchdogId)
{
#if defined(BIT64)
    return static_cast<GUARD_ID>(MACRO_64_HI_BYTE32(watchdogId));
#else   // !defined(BIT64)
    return static_cast<GUARD_ID>(MACRO_32_HI_BYTE16(watchdogId));
#endif  // defined(BIT64)
}

Watchdog::SEQUENCE_ID Watchdog::makeSequenceId(Watchdog::WATCHDOG_ID watchdogId)
{
#if defined(BIT64)
    return static_cast<GUARD_ID>(MACRO_64_LO_BYTE32(watchdogId));
#else   // !defined(BIT64)
    return static_cast<GUARD_ID>(MACRO_32_LO_BYTE16(watchdogId));
#endif  // defined(BIT64)
}

bool Watchdog::isValid(void) const
{
    return (mTimer != nullptr);
}

void Watchdog::timeoutExpired(Watchdog::WATCHDOG_ID watchdogId)
{
    ASSERT(makeGuardId(watchdogId) == mGuardId);
    ASSERT(mTimeoutMs != Watchdog::NO_WATCHDOG);
    do
    {
        Lock lock(mLock);
        if (makeSequenceId(watchdogId) == mSequence)
        {
            mComponentThread.destroyThread(NECommon::DO_NOT_WAIT);
        }
    } while (false);
}
