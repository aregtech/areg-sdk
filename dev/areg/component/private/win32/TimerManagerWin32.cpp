/************************************************************************
 * \file        areg/component/private/TimerManagerWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

TIMERHANDLE TimerManager::_createWaitableTimer( const char * timerName )
{
    TIMERHANDLE result = static_cast<TIMERHANDLE>(NULL);
    TCHAR* name = NULL;
    TCHAR convertName[MAX_PATH];

    if ( NEString::isEmpty<char>(timerName) == false )
    {
        NEString::copyString<TCHAR, char>(convertName, MAX_PATH, timerName);
        name = convertName;
    }
    result = static_cast<TIMERHANDLE>( ::CreateWaitableTimer( NULL, FALSE, name ) );

#ifdef _DEBUG
    if ( result == static_cast<TIMERHANDLE>(NULL) )
    {
        OUTPUT_ERR( "Failed creating timer [ %s ], the system error code is [ 0xp ]", timerName != NULL_STRING ? timerName : "NULL", GetLastError( ) );
    }
#endif // _DEBUG

    return result;
}

void TimerManager::_destroyWaitableTimer( TIMERHANDLE timerHandle, bool cancelTimer )
{
    ASSERT( timerHandle != static_cast<TIMERHANDLE>(NULL) );
    if ( cancelTimer )
    {
        _stopSystemTimer(timerHandle);
    }

    ::CloseHandle( static_cast<HANDLE>(timerHandle) );
}

void TimerManager::_stopSystemTimer( TIMERHANDLE timerHandle )
{
    ASSERT( timerHandle != static_cast<TIMERHANDLE>(NULL) );
    ::CancelWaitableTimer( static_cast<HANDLE>(timerHandle) );
}

bool TimerManager::_startSystemTimer( TimerInfo & timerInfo, MapTimerTable & timerTable )
{
    bool result = false;

    FILETIME fileTime;
    ::GetSystemTimeAsFileTime( &fileTime );
    timerInfo.isTimerStarting( fileTime.dwHighDateTime, fileTime.dwLowDateTime );

    Timer* whichTimer = timerInfo.mTimer;
    ASSERT( whichTimer != static_cast<Timer *>(NULL) );

    // the period of time. If should be fired several times, set the period value. Otherwise set zero to fire once.
    unsigned long period = whichTimer->getEventCount( ) > 1 ? whichTimer->getFireTime( ) : 0;
    int64_t dueTime = whichTimer->getFireTime( ) * NEUtilities::MILLISEC_TO_100NS;  // timer from now
    dueTime *= -1;
    LARGE_INTEGER timeTrigger;

    timeTrigger.LowPart = static_cast<unsigned long>(MACRO_64_LO_BYTE32( dueTime ));
    timeTrigger.HighPart= static_cast<  signed long>(MACRO_64_HI_BYTE32( dueTime ));

    timerTable.registerObject( whichTimer, timerInfo );
    if ( ::SetWaitableTimer( timerInfo.mHandle, &timeTrigger, period, reinterpret_cast<PTIMERAPCROUTINE>(&TimerManager::_defaultWindowsTimerExpiredRoutine), static_cast<void *>(whichTimer), FALSE ) == FALSE )
    {
        OUTPUT_ERR( "System Failed to start timer in period [ %d ] ms, timer name [ %s ]. System Error [ %p ]"
                        , whichTimer->getFireTime( )
                        , whichTimer->getName( ).getString()
                        , static_cast<id_type>(GetLastError( )) );

        timerInfo.mTimerState = TimerInfo::TimerIdle;
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
    ASSERT(argPtr != NULL);
    TimerManager::getInstance()._timerExpired(reinterpret_cast<Timer *>(argPtr), highValue, lowValue);
}

#endif // _WINDOWS
