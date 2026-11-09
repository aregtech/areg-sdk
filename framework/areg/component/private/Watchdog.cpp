/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
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
    , mDisarmed         (false)
    , mSequence         (0u)
    , mComponentThread  (&thread)

    , mArmEvent         ( )
    , mNextRegistered   ( nullptr )
    , mPrevRegistered   ( nullptr )
    , mRegistered       ( false )
{
    WatchdogManager::prepare_arm_event(*this);
}

Watchdog::Watchdog(WorkerThread& thread, uint32_t msTimeout /*= areg::WATCHDOG_IGNORE*/)
    : TimerBase         (TimerBase::TimerType::WatchdogTimer, thread.name(), msTimeout, TimerBase::ONE_TIME)
    , mGuardId          (_generate_id())
    , mDisarmed         (false)
    , mSequence         (0u)
    , mComponentThread  (&thread.binding_component_thread())

    , mArmEvent         ( )
    , mNextRegistered   ( nullptr )
    , mPrevRegistered   ( nullptr )
    , mRegistered       ( false )
{
    WatchdogManager::prepare_arm_event(*this);
}

Watchdog::Watchdog(const String & name)
    : TimerBase         (TimerBase::TimerType::WatchdogTimer, name, areg::LIFECYCLE_TIMEOUT_DEFAULT, TimerBase::ONE_TIME)
    , mGuardId          (_generate_id())
    , mDisarmed         (false)
    , mSequence         (0u)
    , mComponentThread  (nullptr)

    , mArmEvent         ( )
    , mNextRegistered   ( nullptr )
    , mPrevRegistered   ( nullptr )
    , mRegistered       ( false )
{
    WatchdogManager::prepare_arm_event(*this);
}

Watchdog::~Watchdog()
{
    WatchdogManager::stop_timer(*this);
}

void Watchdog::start_guard()
{
    if ((timeout() != areg::WATCHDOG_IGNORE) && (mDisarmed.load(std::memory_order_acquire) == false))
    {
        Lock lock(mLock);
        ASSERT(handle() != nullptr);
        ++mSequence;
        mActive.store(WatchdogManager::start_timer(*this), std::memory_order_relaxed);
    }
}

void Watchdog::stop_guard()
{
    if (timeout() != areg::WATCHDOG_IGNORE)
    {
        Lock lock(mLock);
        ASSERT(handle() != nullptr);
        mActive.store(false, std::memory_order_relaxed);
        WatchdogManager::stop_timer(*this);
    }
}

bool Watchdog::arm(uint32_t msTimeout)
{
    Lock lock(mLock);
    if (handle() == nullptr)
        return false;

    const bool active{ WatchdogManager::rearm_timer(*this, (msTimeout != 0u ? msTimeout : 1u)) };
    mActive.store(active, std::memory_order_relaxed);
    return active;
}

void Watchdog::disarm()
{
    if (mDisarmed.exchange(true, std::memory_order_acq_rel))
        return;

    if (timeout() != areg::WATCHDOG_IGNORE)
    {
        WatchdogManager::stop_timer(*this);
    }
}

} // namespace areg
