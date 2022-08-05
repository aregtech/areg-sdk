/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/WatchdogManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The Thread Watchdog Manager.
 *              Controlling, triggering and stopping timer to check threads.
 *              Windows OS specific calls.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#ifdef  _WINDOWS

#include "areg/component/private/Watchdog.hpp"
#include "areg/base/NEUtilities.hpp"

#include <windows.h>


 //////////////////////////////////////////////////////////////////////////
 //  Windows OS specific methods
 //////////////////////////////////////////////////////////////////////////

void WatchdogManager::_systemTimerStop(const Watchdog& watchdog)
{

    ASSERT(watchdog.getHandle() != nullptr);
    ::CancelWaitableTimer(static_cast<HANDLE>(watchdog.getHandle()));
}

bool WatchdogManager::_systemTimerStart(Watchdog& watchdog)
{
    bool result = false;

    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    long period = 0;
    int64_t dueTime = static_cast<int64_t>(watchdog.getTimeout()) * NEUtilities::MILLISEC_TO_100NS;  // timer from now
    dueTime *= -1;
    LARGE_INTEGER timeTrigger;
    timeTrigger.LowPart  = static_cast<unsigned long>(MACRO_64_LO_BYTE32(dueTime));
    timeTrigger.HighPart = static_cast<signed   long>(MACRO_64_HI_BYTE32(dueTime));

    if (::SetWaitableTimer( watchdog.getHandle()
                          , &timeTrigger
                          , period
                          , reinterpret_cast<PTIMERAPCROUTINE>(&WatchdogManager::_defaultWindowsWatchdogExpiredRoutine)
                          , reinterpret_cast<void*>(watchdog.makeWatchdogId(watchdog.getId(), watchdog.getSequence())), FALSE) == FALSE)
    {
        OUTPUT_ERR("System Failed to start watchdog timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                    , watchdog.getTimeout()
                    , watchdog.getName().getString()
                    , static_cast<id_type>(::GetLastError()));
    }
    else
    {
        result = true; // succeeded
    }

    return result;
}

/**
 * \brief   Windows OS timer routine function. Triggered, when one of timer is expired.
 * \param   argPtr      The pointer of argument passed to timer expired callback function
 * \param   lowValue    The low value of timer expiration
 * \param   highValue   The high value of timer expiration.
 **/
void WatchdogManager::_defaultWindowsWatchdogExpiredRoutine(void* argPtr, unsigned long lowValue, unsigned long highValue)
{
    ASSERT(argPtr != nullptr);
    Watchdog::WATCHDOG_ID watchdogId = reinterpret_cast<Watchdog::WATCHDOG_ID>(argPtr);
    WatchdogManager::getInstance()._timerExpired(watchdogId, highValue, lowValue);
}

#endif // _WINDOWS
