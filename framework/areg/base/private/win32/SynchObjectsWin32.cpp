/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/ESynchObjectsWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects. Windows specific methods
 *
 ************************************************************************/
#include "areg/base/SynchObjects.hpp"

#ifdef  _WINDOWS
#include "areg/base/NEMemory.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/win32/SpinLockWin32.hpp"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// IESynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class methods
//////////////////////////////////////////////////////////////////////////

void IESynchObject::_osDestroySynchObject( void )
{
    CloseHandle( static_cast<HANDLE>(mSynchObject) );
    mSynchObject = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void Mutex::_osCreateMutex( bool initLock )
{
    SYNCHANDLE synchObj = static_cast<SYNCHANDLE>(CreateMutex( nullptr, initLock ? TRUE : FALSE, nullptr ));
    mSynchObject = synchObj;
    if ( initLock )
    {
        // _osLockMutex( NECommon::WAIT_INFINITE );
        mOwnerThreadId.store( Thread::getCurrentThreadId( ) );
    }
}

bool Mutex::_osLockMutex( unsigned int timeout )
{
    bool result = false;

    if (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0)
    {
        mOwnerThreadId.store( Thread::getCurrentThreadId( ) );
        result = true;
    }

    return result;
}

bool Mutex::_osUnlockMutex( void )
{
    bool result = false;
    if (ReleaseMutex(static_cast<HANDLE>(mSynchObject)))
    {
        mOwnerThreadId.store( 0 );
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class implementation
//////////////////////////////////////////////////////////////////////////

void SynchEvent::_osCreateEvent( bool initLock )
{
    mSynchObject = static_cast<void *>(CreateEvent( nullptr, mAutoReset ? FALSE : TRUE, initLock ? FALSE : TRUE, nullptr ));
}

bool SynchEvent::_osUnlockEvent( void * eventHandle )
{
    return ( ::SetEvent( static_cast<HANDLE>(mSynchObject) ) != FALSE );
}

bool SynchEvent::_osLockEvent(unsigned int timeout)
{
    return ( WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0 );
}

bool SynchEvent::_osSetEvent( void )
{
    return (::SetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool SynchEvent::_osResetEvent( void )
{
    return (::ResetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

void SynchEvent::_osPulseEvent( void )
{
    ::PulseEvent(static_cast<HANDLE>(mSynchObject));
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_osCreateSemaphore( void )
{
    mSynchObject= static_cast<void *>(CreateSemaphore(nullptr, mCurrCount.load(), mMaxCount, nullptr));
}

void Semaphore::_osReleaseSemaphore( void )
{
}

bool Semaphore::_osLock(unsigned int timeout)
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSynchObject), timeout ) == WAIT_OBJECT_0);
}

bool Semaphore::_osUnlock( void )
{
    return (ReleaseSemaphore( static_cast<HANDLE>(mSynchObject), 1, nullptr ) == TRUE);
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_osCreateCriticalSection( void )
{
    mSynchObject = static_cast<void *>( DEBUG_NEW unsigned char [sizeof(CRITICAL_SECTION)] );
    NEMemory::constructElems<CRITICAL_SECTION>( mSynchObject, 1 );
    InitializeCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject) );
}

void CriticalSection::_osReleaseCriticalSection( void )
{
    DeleteCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject) );
    delete[] reinterpret_cast<unsigned char *>(mSynchObject);
    mSynchObject = nullptr;
}

bool CriticalSection::_osLock( void )
{
    EnterCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject) );
    return true;
}

bool CriticalSection::_osUnlock( void )
{
    LeaveCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject) );
    return true;
}

bool CriticalSection::_osTryLock( void )
{
    return (TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject)) == TRUE);
}

#if 0 // TODO: Probably don't need anymore and should be removed

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock( void )
    : IEResourceLock( IESynchObject::eSyncObject::SoSpinlock )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    mSynchObject    = DEBUG_NEW SpinLockWin32( );
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    mSynchObject    = DEBUG_NEW CriticalSection( );
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

SpinLock::~SpinLock( void )
{
    if ( mSynchObject != nullptr )
    {
#if defined (__cplusplus) && (__cplusplus > 201703L)
        delete reinterpret_cast<SpinLockWin32 *>(mSynchObject);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
        delete reinterpret_cast<CriticalSection *>(mSynchObject);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
        mSynchObject = nullptr;
    }
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSynchObject)->lock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<CriticalSection *>(mSynchObject)->lock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::unlock( void )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSynchObject)->unlock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<CriticalSection *>(mSynchObject)->unlock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::tryLock( void )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSynchObject)->tryLock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSynchObject != nullptr ? reinterpret_cast<CriticalSection *>(mSynchObject)->tryLock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}
#endif

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_osCreateResourceLock( bool initLock )
{
#if 0
    mSynchObject = new Mutex( initLock );

#else

    mSynchObject = new CriticalSection( );
    if ( initLock )
    {
        reinterpret_cast<IEResourceLock *>(mSynchObject)->lock( NECommon::WAIT_INFINITE );
    }

#endif

}

void ResourceLock::_osReleaseResourceLock( void )
{
    reinterpret_cast<IEResourceLock *>(mSynchObject)->unlock( );
    delete reinterpret_cast<IEResourceLock *>(mSynchObject);
    mSynchObject = nullptr;
}

bool ResourceLock::_osLock(unsigned int timeout)
{
    return reinterpret_cast<IEResourceLock *>(mSynchObject)->lock(timeout);
}

bool ResourceLock::_osUnlock( void )
{
    return reinterpret_cast<IEResourceLock *>(mSynchObject)->unlock( );
}

bool ResourceLock::_osTryLock(void)
{
    return reinterpret_cast<IEResourceLock *>(mSynchObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer implementation
//////////////////////////////////////////////////////////////////////////

void SynchTimer::_osCreateTimer( bool isSteady )
{
    DWORD flag = 0;
    if (isSteady)
    {
        flag |= CREATE_WAITABLE_TIMER_HIGH_RESOLUTION;
    }

    if ( mIsAutoReset == false)
    {
        flag |= CREATE_WAITABLE_TIMER_MANUAL_RESET;
    }

    mSynchObject = static_cast<SYNCHANDLE>(::CreateWaitableTimerEx(nullptr, nullptr, flag, TIMER_ALL_ACCESS));
}

void SynchTimer::_osReleaseTime( void )
{
    CancelWaitableTimer( static_cast<HANDLE>(mSynchObject) );
    CloseHandle( static_cast<HANDLE>(mSynchObject) );
}

bool SynchTimer::_osLock( unsigned int timeout )
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSynchObject), timeout ) == WAIT_OBJECT_0);
}

bool SynchTimer::_osSetTimer( void )
{
    constexpr int NANOSECONDS_COEF_100  { 10'000 };

    LARGE_INTEGER dueTime;
    dueTime.QuadPart = -(static_cast<int64_t>(mTimeout) * NANOSECONDS_COEF_100);
    LONG lPeriod = mIsPeriodic ? static_cast<LONG>(mTimeout) : 0;
    return (SetWaitableTimer( static_cast<HANDLE>(mSynchObject), &dueTime, lPeriod, nullptr, nullptr, FALSE ) != FALSE);
}

bool SynchTimer::_osCancelTimer( void )
{
    return (CancelWaitableTimer( static_cast<HANDLE>(mSynchObject) ) != FALSE);
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int MultiLock::_osLock( unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/ )
{
    void * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i)
    {
        syncHandles[i] = mSyncObjArray[i]->getHandle( );
    }

    int index = MultiLock::LOCK_INDEX_INVALID;
    unsigned int maxEvent= static_cast<unsigned int>(WAIT_OBJECT_0 + mSizeCount);
    unsigned int result  = mSizeCount > 0 ? WaitForMultipleObjectsEx(static_cast<unsigned int>(mSizeCount), static_cast<HANDLE *>(syncHandles), waitForAll ? TRUE : FALSE, timeout, isAlertable ? TRUE : FALSE) : WAIT_FAILED;
    if (result < maxEvent)
    {
        if (waitForAll == false)
        {
            index = static_cast<int>(result - WAIT_OBJECT_0);
            ASSERT((index >= 0) && index < mSizeCount);
            mLockedStates[index] = MultiLock::eLockedState::StateLocked;
        }
        else
        {
            for (int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::eLockedState::StateLocked;
            }

            index = MultiLock::LOCK_INDEX_ALL;
        }
    }
    else if (result == WAIT_IO_COMPLETION)
    {
        index = MultiLock::LOCK_INDEX_COMPLETION;
    }
    else if (result == WAIT_TIMEOUT)
    {
        index = MultiLock::LOCK_INDEX_TIMEOUT;
    }

    return index;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
    inline double _getFrequencyNs(void)
    {
        LARGE_INTEGER frequency{ 0 };
        QueryPerformanceFrequency(&frequency);
        return ( static_cast<double>(frequency.QuadPart / static_cast<double>(Wait::ONE_SEC.count())) );
    }

    const double _ticksPerNs{ _getFrequencyNs() };

}

void Wait::_osInitTimer(void)
{
    mTimer = ::CreateWaitableTimerEx( NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS );
}

void Wait::_osReleaseTimer(void)
{
    ::CancelWaitableTimer( mTimer );
    ::CloseHandle( mTimer );
    mTimer = nullptr;
}

Wait::eWaitResult Wait::_osWaitFor(const Wait::Duration& timeout) const
{
    static constexpr int64_t _COEF{ -10'000 };

    Wait::eWaitResult result {Wait::eWaitResult::WaitInvalid};
    if (timeout >= Wait::MIN_WAIT)
    {
        LARGE_INTEGER dueTime;
        dueTime.QuadPart = static_cast<int64_t>(timeout.count() / ONE_MS.count()) * _COEF;
        ::SetWaitableTimer(mTimer, &dueTime, 0, nullptr, nullptr, FALSE);
        if (::WaitForSingleObject(mTimer, INFINITE) == WAIT_OBJECT_0)
        {
            result = Wait::eWaitResult::WaitInMilli;
        }
    }
    else if (timeout >= Wait::ONE_MUS)
    {
        LARGE_INTEGER start, dueTime;
        QueryPerformanceCounter(&start);
        // due time is current time in ticks + expected ticks
        int64_t deadline = start.QuadPart + static_cast<int64_t>(timeout.count() * _ticksPerNs);
        
        do
        {
            ::Sleep(NECommon::DO_NOT_WAIT);
            ::QueryPerformanceCounter(&dueTime);
        } while (dueTime.QuadPart < deadline);

        result = Wait::eWaitResult::WaitInMicro;
    }
    else if (timeout.count() > 0)
    {
        result = Wait::eWaitResult::WaitIgnored;
    }

    return result;
}


#endif  // _WINDOWS
