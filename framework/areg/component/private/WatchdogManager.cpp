/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The Thread Watchdog Manager.
 *              Controlling and stopping stuck threads.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/logging/GELog.h"

namespace areg
{
    DEF_LOG_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

    //////////////////////////////////////////////////////////////////////////
    // WatchdogManager class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Static functions
    //////////////////////////////////////////////////////////////////////////

    WatchdogManager& WatchdogManager::getInstance()
    {
        static WatchdogManager	_theWatchdogManager;
        return _theWatchdogManager;
    }

    bool WatchdogManager::startWatchdogManager()
    {
        return getInstance().startTimerManagerThread();
    }

    void WatchdogManager::stopWatchdogManager(bool waitComplete)
    {
        return getInstance().stopTimerManagerThread(waitComplete);
    }

    void WatchdogManager::waitWatchdogManager()
    {
        return getInstance().waitCompletion();
    }

    bool WatchdogManager::isWatchdogManagerStarted()
    {
        return getInstance().isReady();
    }

    bool WatchdogManager::startTimer(areg::Watchdog& watchdog)
    {
        bool result = false;
        ASSERT(watchdog.getHandle() != nullptr);
        WatchdogManager& watchdogManager = getInstance();

        if (watchdogManager.isWatchdogManagerStarted())
        {
            watchdogManager._registerWatchdog(watchdog);
            result = areg::TimerManagerEvent::sendEvent( areg::TimerManagerEventData(&watchdog)
                                                , static_cast<areg::TimerManagerEventConsumer&>(watchdogManager)
                                                , static_cast<areg::DispatcherThread&>(watchdogManager));
        }

        return result;
    }

    void WatchdogManager::stopTimer(areg::Watchdog& watchdog)
    {
        getInstance()._unregisterWatchdog(watchdog);
        WatchdogManager::_osSystemTimerStop(watchdog.getHandle());
    }

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////

    WatchdogManager::WatchdogManager()
        : areg::TimerManagerBase(WatchdogManager::WATCHDOG_THREAD_NAME)

        , mWatchdogResource()
    {
    }

    WatchdogManager::~WatchdogManager()
    {
        _removeAllWatchdogs();
    }

    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////

    inline void WatchdogManager::_registerWatchdog(areg::Watchdog & watchdog)
    {
        mWatchdogResource.registerResourceObject(watchdog.getId(), &watchdog);
    }

    inline void WatchdogManager::_unregisterWatchdog(areg::Watchdog& watchdog)
    {
        mWatchdogResource.unregisterResourceObject(watchdog.getId());
        WatchdogManager::_osSystemTimerStop(watchdog.getHandle());
    }

    void WatchdogManager::_removeAllWatchdogs()
    {
        mWatchdogResource.lock();

        std::pair< areg::Watchdog::GUARD_ID, areg::Watchdog*> elem;
        while (mWatchdogResource.isEmpty() == false)
        {
            mWatchdogResource.removeResourceFirstElement(elem);
            ASSERT(elem.second != nullptr);
            WatchdogManager::_osSystemTimerStop(elem.second->getHandle());
        }

        mWatchdogResource.unlock();
    }

    void WatchdogManager::processEvent(const areg::TimerManagerEventData & data)
    {
        areg::Watchdog* watchdog = static_cast<areg::Watchdog*>(data.getTimer());
        ASSERT(watchdog != nullptr);
        if (mWatchdogResource.existResource(watchdog->getId()))
        {
            WatchdogManager::_osSystemTimerStart(*watchdog);
        }
    }

    void WatchdogManager::_processExpiredTimer(areg::Watchdog* watchdog, areg::Watchdog::WATCHDOG_ID watchdogId, uint32_t /* hiBytes */, uint32_t /* loBytes */)
    {
        LOG_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

        mWatchdogResource.lock();

        areg::Watchdog::SEQUENCE_ID sequence  = areg::Watchdog::makeSequenceId(watchdogId);
        if ((watchdog != nullptr) && (watchdog->getSequence() == sequence))
        {
            LOG_WARN("The watchdog [ %s ] has expired, terminating component thread [ %s ]"
                            , watchdog->getName().getString()
                            , watchdog->getComponentThread().getName().getString());

            areg::ServiceManager::requestRecreateThread(watchdog->getComponentThread());
        }

        mWatchdogResource.unlock();
    }

    void WatchdogManager::readyForEvents(bool isReady)
    {
        if (isReady == false)
        {
            _removeAllWatchdogs();
        }

        areg::TimerManagerBase::readyForEvents(isReady);
    }

} // namespace areg