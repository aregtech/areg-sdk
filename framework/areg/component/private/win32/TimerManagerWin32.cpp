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
 *              Windows OS specific calls.
 *
 ************************************************************************/
#include "areg/component/private/TimerManager.hpp"

#ifdef  _WINDOWS

#include "areg/component/Timer.hpp"
#include "areg/base/NEUtilities.hpp"

#include <windows.h>


//////////////////////////////////////////////////////////////////////////
//  Windows OS specific methods
//////////////////////////////////////////////////////////////////////////

TIMERHANDLE TimerManager::_createWaitableTimer( const String & timerName )
{
    TIMERHANDLE result = nullptr;
    TCHAR* name = nullptr;
    TCHAR convertName[MAX_PATH];

    if ( timerName.isEmpty() == false)
    {
        NEString::copyString<TCHAR, char>(convertName, MAX_PATH, timerName, timerName.getLength());
        name = convertName;
    }

    result = static_cast<TIMERHANDLE>( ::CreateWaitableTimer( nullptr, FALSE, name ) );

#ifdef _DEBUG
    if ( result == nullptr )
    {
        OUTPUT_ERR( "Failed creating timer [ %s ], the system error code is [ 0xp ]", timerName.getString(), GetLastError());
    }
#endif // _DEBUG

    return result;
}

void TimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool cancelTimer )
{
    ASSERT( timerHandle != nullptr );
    if ( cancelTimer )
    {
        _systemTimerStop(timerHandle);
    }

    ::CloseHandle( static_cast<HANDLE>(timerHandle) );
}

void TimerManager::_systemTimerStop( TIMERHANDLE timerHandle )
{

    ASSERT( timerHandle != nullptr );
    ::CancelWaitableTimer( static_cast<HANDLE>(timerHandle) );
}

bool TimerManager::_systemTimerStart( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    bool result = false;

    FILETIME fileTime;
    ::GetSystemTimeAsFileTime( &fileTime );
    timerInfo.timerStarting( fileTime.dwHighDateTime, fileTime.dwLowDateTime );

    Timer* whichTimer = timerInfo.mTimer;
    ASSERT( whichTimer != nullptr );

    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    long period     = whichTimer->getEventCount( ) > 1 ? static_cast<long>(whichTimer->getTimeout()) : 0;
    int64_t dueTime = static_cast<int64_t>(whichTimer->getTimeout( )) * NEUtilities::MILLISEC_TO_100NS;  // timer from now
    dueTime *= -1;
    LARGE_INTEGER timeTrigger;

    timeTrigger.LowPart = static_cast<unsigned long>(MACRO_64_LO_BYTE32( dueTime ));
    timeTrigger.HighPart= static_cast<  signed long>(MACRO_64_HI_BYTE32( dueTime ));

    timerTable.registerObject( whichTimer, timerInfo );
    if ( ::SetWaitableTimer( timerInfo.mHandle, &timeTrigger, period, reinterpret_cast<PTIMERAPCROUTINE>(&TimerManager::_defaultWindowsTimerExpiredRoutine), static_cast<void *>(whichTimer), FALSE ) == FALSE )
    {
        OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                        , whichTimer->getTimeout( )
                        , whichTimer->getName( ).getString()
                        , static_cast<id_type>(GetLastError( )) );

        timerInfo.mTimerState = TimerInfo::eTimerState::TimerIdle;
        timerTable.updateObject( whichTimer, timerInfo );
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
void TimerManager::_defaultWindowsTimerExpiredRoutine( void * argPtr, unsigned long lowValue, unsigned long highValue )
{
    ASSERT(argPtr != nullptr);
    TimerManager::getInstance()._timerExpired(reinterpret_cast<Timer *>(argPtr), highValue, lowValue);
}

#endif // _WINDOWS
