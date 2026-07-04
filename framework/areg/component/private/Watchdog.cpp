/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/Watchdog.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread watchdog object implementation
 *
 ************************************************************************/

#include "areg/component/private/Watchdog.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/private/WatchdogManager.hpp"

#include <atomic>
namespace areg {

Watchdog::GUARD_ID Watchdog::_generate_id()
{
    static std::atomic<Watchdog::GUARD_ID> _id{ 0 };
    return (++_id);
}

Watchdog::Watchdog(ComponentThread& thread, uint32_t msTimeout /*= areg::WATCHDOG_IGNORE*/)
    : TimerBase         (TimerBase::TimerType::WatchdogTimer, thread.name(), msTimeout, TimerBase::ONE_TIME)
    , mGuardId          (_generate_id())
    , mSequence         (0u)
    , mComponentThread  (thread)
{
}

Watchdog::Watchdog(WorkerThread& thread, uint32_t msTimeout /*= areg::WATCHDOG_IGNORE*/)
    : TimerBase         (TimerBase::TimerType::WatchdogTimer, thread.name(), msTimeout, TimerBase::ONE_TIME)
    , mGuardId          (_generate_id())
    , mSequence         (0u)
    , mComponentThread  (thread.binding_component_thread())
{
}

Watchdog::~Watchdog()
{
    WatchdogManager::stop_timer(*this);
}

void Watchdog::start_guard()
{
    if (mTimeoutInMs != areg::WATCHDOG_IGNORE)
    {
        Lock lock(mLock);
        ASSERT(mHandle != nullptr);
        ++mSequence;
        mActive = WatchdogManager::start_timer(*this);
    }
}

void Watchdog::stop_guard()
{
    if (mTimeoutInMs != areg::WATCHDOG_IGNORE)
    {
        Lock lock(mLock);
        ASSERT(mHandle != nullptr);
        mActive = false;
        WatchdogManager::stop_timer(*this);
    }
}

} // namespace areg
