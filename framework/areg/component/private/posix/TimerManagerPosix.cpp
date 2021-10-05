/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *              POSIX specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"
#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"
#include <signal.h>
#include <time.h>
#include <errno.h>

DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__createSystemTimer);
DEF_TRACE_SCOPE(areg_component_private_posix_TimerManager__defaultPosixTimerExpiredRoutine);

//////////////////////////////////////////////////////////////////////////
// Linux specific methods
//////////////////////////////////////////////////////////////////////////

TIMERHANDLE TimerManager::_createWaitableTimer( const char * /* timerName */ )
{
    return static_cast<TIMERHANDLE>(DEBUG_NEW TimerPosix());
}

void TimerManager::_stopSystemTimer( TIMERHANDLE timerHandle )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != nullptr )
        posixTimer->stopTimer();
}

void TimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool /* cancelTimer */ )
{
    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(timerHandle);
    if ( posixTimer != nullptr )
    {
        posixTimer->destroyTimer();
        delete posixTimer;
    }
}

bool TimerManager::_createSystemTimer( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    TRACE_SCOPE(areg_component_private_posix_TimerManager__createSystemTimer);

    bool result = false;
    TimerPosix * posixTimer   = reinterpret_cast<TimerPosix *>(timerInfo.getHandle());
    Timer * whichTimer        = timerInfo.getTimer();
    if ((posixTimer != nullptr) && (whichTimer != nullptr))
    {
    	ASSERT(posixTimer->mContext == nullptr);
        if (posixTimer->createTimer(&TimerManager::_defaultPosixTimerExpiredRoutine))
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            timerInfo.timerStarting( static_cast<unsigned int>(ts.tv_sec), static_cast<unsigned int>(ts.tv_nsec));
            timerTable.registerObject( whichTimer, timerInfo );

            result = true;
            if (posixTimer->startTimer(*whichTimer) == false)
            {
                TRACE_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                                , whichTimer->getFireTime( )
                                , whichTimer->getName( ).getString()
                                , static_cast<id_type>(errno) );

                result = false;
                timerInfo.mTimerState   = TimerInfo::eTimerState::TimerIdle;
                timerTable.updateObject( whichTimer, timerInfo );
            }
            else
            {
                TRACE_DBG("Started timer [ %s ], starting time at [ %u ]:[ %u ], expire time at [ %u ]:[ %u ], difference: [ %u ] sec and [ %u ] ns "
                            , whichTimer->getName().getString()
                            , ts.tv_sec
                            , ts.tv_nsec
                            , posixTimer->getDueTime().tv_sec
                            , posixTimer->getDueTime().tv_nsec
                            , posixTimer->getDueTime().tv_sec - ts.tv_sec
                            , posixTimer->getDueTime().tv_nsec- ts.tv_nsec);
            }
        }
        else
        {
        	TRACE_ERR("Failed to create instance of POSIX system timer [ %s ]", whichTimer->getName().getString());
        }
    }
    else
    {
    	TRACE_ERR("Either POSIX system [ %p ], or the timer [ %p ] object are null", posixTimer, whichTimer);
    }

    return result;
}

void TimerManager::_defaultPosixTimerExpiredRoutine( union sigval argSig )
{
    TRACE_SCOPE(areg_component_private_posix_TimerManager__defaultPosixTimerExpiredRoutine);

    TimerPosix * posixTimer = reinterpret_cast<TimerPosix *>(argSig.sival_ptr);
    Timer *timer = TimerManager::getInstance().mTimerResource.findResourceObject(reinterpret_cast<TIMERHANDLE>(posixTimer));

    if ((timer != nullptr) && (posixTimer->isValid()))
    {
        TRACE_DBG("Timer [ %s ] expired at [ %u ] sec and [ %u ] ns, going to notify thread [ %u ]"
                        , posixTimer->mContext->getName().getString()
                        , posixTimer->getDueTime().tv_sec
                        , posixTimer->getDueTime().tv_nsec
                        , static_cast<unsigned int>(posixTimer->mThreadId));

        unsigned int highValue  = static_cast<unsigned int>(posixTimer->mDueTime.tv_sec );
        unsigned int lowValue   = static_cast<unsigned int>(posixTimer->mDueTime.tv_nsec);
        posixTimer->timerExpired();
        TimerManager::getInstance()._timerExpired(posixTimer->mContext, highValue, lowValue);
    }
    else
    {
        TRACE_WARN("Ignore handling timer [ %p ], it is [ %s ]", posixTimer, timer == nullptr ? "NOT REGISTERED ANYMORE" : "ALREADY INVALID");
    }
}

#endif  // defined(_POSIX) || defined(POSIX)
