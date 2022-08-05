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
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/WatchdogManager.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"
#include <signal.h>
#include <time.h>
#include <errno.h>

DEF_TRACE_SCOPE(areg_component_private_posix_WatchdogManager__systemTimerStart);
DEF_TRACE_SCOPE(areg_component_private_posix_WatchdogManager__defaultPosixTimerExpiredRoutine);

//////////////////////////////////////////////////////////////////////////
// Linux specific methods
//////////////////////////////////////////////////////////////////////////

void WatchdogManager::_systemTimerStop(const Watchdog& watchdog)
{
    TimerPosix* posixTimer = reinterpret_cast<TimerPosix*>(watchdog.getHandle());
    if (posixTimer != nullptr)
    {
        posixTimer->stopTimer();
    }
}

bool WatchdogManager::_systemTimerStart(Watchdog& watchdog)
{
    TRACE_SCOPE(areg_component_private_posix_WatchdogManager__systemTimerStart);

    bool result = false;
    TimerPosix* posixTimer = reinterpret_cast<TimerPosix*>(watchdog.getHandle());
    if (posixTimer != nullptr)
    {
        if (posixTimer->createTimer(&WatchdogManager::_defaultPosixWatchdogExpiredRoutine))
        {
            result = true;
            if (posixTimer->startTimer(watchdog.getTimeout(), 1))
            {
                TRACE_DBG("Started watchdog timer [ %s ] with timeout [ %u ] ms", watchdog.getName().getString(), watchdog.getTimeout());
                result = true;

            }
            else
            {
                TRACE_ERR("System failed to start watchdog timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                                , watchdog.getTimeout()
                                , watchdog.getName().getString()
                                , static_cast<id_type>(errno));
            }
        }
        else
        {
            TRACE_ERR("Failed to create instance of POSIX system timer [ %s ]", watchdog.getName().getString());
        }
    }
    else
    {
        TRACE_ERR("The POSIX timer is null");
    }

    return result;
}

void WatchdogManager::_defaultPosixWatchdogExpiredRoutine(union sigval argSig)
{
    TRACE_SCOPE(areg_component_private_posix_WatchdogManager__defaultPosixTimerExpiredRoutine);

    TimerPosix* posixTimer = reinterpret_cast<TimerPosix*>(argSig.sival_ptr);
    ASSERT(posixTimer != nullptr);
    ASSERT(posixTimer->getContextId() != 0);
    if (posixTimer->isValid())
    {
        TRACE_DBG("Watchdog Timer [ 0u ] expired at [ %u ] sec and [ %u ] ns, going to notify thread [ %u ]"
                        , posixTimer->mContextId
                        , posixTimer->getDueTime().tv_sec
                        , posixTimer->getDueTime().tv_nsec
                        , static_cast<unsigned int>(posixTimer->mThreadId));

        unsigned int highValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_sec);
        unsigned int lowValue = static_cast<unsigned int>(posixTimer->mDueTime.tv_nsec);
        posixTimer->stopTimer();
        WatchdogManager::getInstance()._timerExpired(posixTimer->mContextId, highValue, lowValue);
    }
    else
    {
        TRACE_WARN("Ignore handling timer [ %p ], it is [ %s ]", posixTimer, timer == nullptr ? "NOT REGISTERED ANYMORE" : "ALREADY INVALID");
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
