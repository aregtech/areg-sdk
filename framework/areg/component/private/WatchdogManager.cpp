/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The Thread Watchdog Manager.
 *              Controlling and stopping stuck threads.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#include "areg/component/private/ExitEvent.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_component_private_WatchdogManager_startTimer);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager_processEvent);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager__registerWatchdog);
DEF_TRACE_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

//////////////////////////////////////////////////////////////////////////
// WatchdogManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

WatchdogManager& WatchdogManager::getInstance(void)
{
    static WatchdogManager	_theWatchdogManager;
    return _theWatchdogManager;
}

bool WatchdogManager::startWatchdogManager(void)
{
    return getInstance().startTimerManagerThread();
}

void WatchdogManager::stopWatchdogManager(void)
{
    return getInstance().stopTimerManagerThread();
}

bool WatchdogManager::isWatchdogManagerStarted(void)
{
    return getInstance().isReady();
}

bool WatchdogManager::startTimer(Watchdog& watchdog)
{
    TRACE_SCOPE(areg_component_private_WatchdogManager_startTimer);

    bool result = false;
    ASSERT(watchdog.getHandle() != nullptr);
    WatchdogManager& watchdogManager = getInstance();

    if (watchdogManager.isWatchdogManagerStarted())
    {
        if (watchdogManager._registerWatchdog(watchdog))
        {
            TRACE_DBG("Registered timer [ %s ], sending event to start timer", watchdog.getName().getString());
            result = TimerManagerEvent::sendEvent( TimerManagerEventData(&watchdog)
                                                 , static_cast<IETimerManagerEventConsumer&>(watchdogManager)
                                                 , static_cast<DispatcherThread&>(watchdogManager));
        }
    }
    else
    {
        TRACE_ERR("The Timer manager service is not running, cannot start and process the timer.");
    }

    return result;
}

void WatchdogManager::stopTimer(Watchdog& watchdog)
{
    getInstance()._unregisterWatchdog(watchdog);
    WatchdogManager::_systemTimerStop(watchdog.getHandle());
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

WatchdogManager::WatchdogManager(void)
    : TimerManagerBase(WatchdogManager::WATCHDOG_THREAD_NAME)

    , mWatchdogResource()
{
}

WatchdogManager::~WatchdogManager(void)
{
    _removeAllWatchdogs();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool WatchdogManager::_registerWatchdog(Watchdog & watchdog)
{
    bool result{ false };
    if (watchdog.getHandle() != nullptr)
    {
        TRACE_SCOPE(areg_component_private_WatchdogManager__registerWatchdog);
        mWatchdogResource.registerResourceObject(watchdog.getId(), &watchdog);
        TRACE_DBG("Registered watchdog [ %s ]", watchdog.getName().getString());
        result = true;
    }

    return result;
}

void WatchdogManager::_unregisterWatchdog(Watchdog& watchdog)
{
    mWatchdogResource.unregisterResourceObject(watchdog.getId());
    WatchdogManager::_systemTimerStop(watchdog.getHandle());
}

void WatchdogManager::_removeAllWatchdogs(void)
{
    mWatchdogResource.lock();

    std::pair< Watchdog::GUARD_ID, Watchdog*> elem;
    while (mWatchdogResource.isEmpty() == false)
    {
        mWatchdogResource.removeResourceFirstElement(elem);
        ASSERT(elem.second != nullptr);
        WatchdogManager::_systemTimerStop(elem.second->getHandle());
    }

    mWatchdogResource.unlock();
}

void WatchdogManager::processEvent(const TimerManagerEventData & data)
{
    TRACE_SCOPE(areg_component_private_WatchdogManager_processEvent);
    Watchdog* watchdog = static_cast<Watchdog*>(data.getTimer());
    ASSERT((watchdog != nullptr) && mWatchdogResource.existResource(watchdog->getId()));
    TRACE_DBG("Starting timer [ %s ] with timeout [ %u ] ms.", watchdog->getName().getString(), watchdog->getTimeout());
    WatchdogManager::_systemTimerStart(*watchdog);
}

void WatchdogManager::_processExpiredTimer(Watchdog* whatchdog, Watchdog::WATCHDOG_ID watchdogId, uint32_t hiBytes, uint32_t loBytes)
{
    TRACE_SCOPE(areg_component_private_WatchdogManager__processExpiredTimers);

    mWatchdogResource.lock();

    Watchdog::GUARD_ID guardId      = Watchdog::makeGuardId(watchdogId);
    Watchdog::SEQUENCE_ID sequence  = Watchdog::makeSequenceId(watchdogId);
    Watchdog* watchdog = mWatchdogResource.findResourceObject(guardId);
    if ((watchdog != nullptr) && (watchdog->getSequence() == sequence))
    {
        TRACE_WARN("The watchdog [ %s ] has expired, terminating component thread [ %s ]"
                        , watchdog->getName().getString()
                        , watchdog->getComponentThread().getName().getString());

        ServiceManager::requestRecreateThread(watchdog->getComponentThread());
    }

    mWatchdogResource.unlock();
}

void WatchdogManager::readyForEvents(bool isReady)
{
    if (isReady == false)
    {
        _removeAllWatchdogs();
    }

    TimerManagerBase::readyForEvents(isReady);
}
