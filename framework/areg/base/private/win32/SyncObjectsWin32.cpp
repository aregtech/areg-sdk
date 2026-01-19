/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SyncObjectsWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects. Windows specific methods
 *
 ************************************************************************/
#include "areg/base/SyncObjects.hpp"

#ifdef  _WIN32
#include "areg/base/NEMemory.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/win32/SpinLockWin32.hpp"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
// IESyncObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESyncObject class methods
//////////////////////////////////////////////////////////////////////////

void IESyncObject::_osDestroySyncObject( void )
{
    if ( mSyncObject != nullptr )
    {
        CloseHandle( static_cast<HANDLE>(mSyncObject) );
        mSyncObject = nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void Mutex::_osCreateMutex( bool initLock )
{
    SYNCHANDLE syncObj = static_cast<SYNCHANDLE>(CreateMutex( nullptr, initLock ? TRUE : FALSE, nullptr ));
    mSyncObject = syncObj;
    if ( initLock )
    {
        mOwnerThreadId.store( Thread::getCurrentThreadId( ) );
    }
}

bool Mutex::_osLockMutex( unsigned int timeout )
{
    bool result = false;

    if (WaitForSingleObject(static_cast<HANDLE>(mSyncObject), timeout) == WAIT_OBJECT_0)
    {
        mOwnerThreadId.store( Thread::getCurrentThreadId( ) );
        result = true;
    }

    return result;
}

bool Mutex::_osUnlockMutex( void )
{
    bool result = false;
    if (ReleaseMutex(static_cast<HANDLE>(mSyncObject)))
    {
        mOwnerThreadId.store( 0 );
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class implementation
//////////////////////////////////////////////////////////////////////////

void SyncEvent::_osCreateEvent( bool initLock )
{
    mSyncObject = static_cast<void *>(CreateEvent( nullptr, mAutoReset ? FALSE : TRUE, initLock ? FALSE : TRUE, nullptr ));
}

bool SyncEvent::_osUnlockEvent( void * /* eventHandle */ )
{
    return ( ::SetEvent( static_cast<HANDLE>(mSyncObject) ) != FALSE );
}

bool SyncEvent::_osLockEvent(unsigned int timeout)
{
    return ( WaitForSingleObject(static_cast<HANDLE>(mSyncObject), timeout) == WAIT_OBJECT_0 );
}

bool SyncEvent::_osSetEvent( void )
{
    return (::SetEvent(static_cast<HANDLE>(mSyncObject)) != FALSE);
}

bool SyncEvent::_osResetEvent( void )
{
    return (::ResetEvent(static_cast<HANDLE>(mSyncObject)) != FALSE);
}

void SyncEvent::_osPulseEvent( void )
{
    ::PulseEvent(static_cast<HANDLE>(mSyncObject));
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_osCreateSemaphore( void )
{
    mSyncObject= static_cast<void *>(CreateSemaphore(nullptr, mCurrCount.load(), mMaxCount, nullptr));
}

void Semaphore::_osReleaseSemaphore( void )
{
}

bool Semaphore::_osLock(unsigned int timeout)
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSyncObject), timeout ) == WAIT_OBJECT_0);
}

bool Semaphore::_osUnlock( void )
{
    return (ReleaseSemaphore( static_cast<HANDLE>(mSyncObject), 1, nullptr ) == TRUE);
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_osCreateCriticalSection( void )
{
    mSyncObject = static_cast<void *>( DEBUG_NEW unsigned char [sizeof(CRITICAL_SECTION)] );
    NEMemory::constructElems<CRITICAL_SECTION>( mSyncObject, 1 );
    InitializeCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
}

void CriticalSection::_osReleaseCriticalSection( void )
{
    LeaveCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    DeleteCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    delete[] reinterpret_cast<unsigned char *>(mSyncObject);
    mSyncObject = nullptr;
}

bool CriticalSection::_osLock( void )
{
    EnterCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    return true;
}

bool CriticalSection::_osUnlock( void )
{
    LeaveCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    return true;
}

bool CriticalSection::_osTryLock( void )
{
    return (TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject)) == TRUE);
}

#if 0 // TODO: Probably don't need anymore and should be removed

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock( void )
    : IEResourceLock( IESyncObject::eSyncObject::SoSpinlock )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    mSyncObject    = DEBUG_NEW SpinLockWin32( );
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    mSyncObject    = DEBUG_NEW CriticalSection( );
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

SpinLock::~SpinLock( void )
{
    if ( mSyncObject != nullptr )
    {
#if defined (__cplusplus) && (__cplusplus > 201703L)
        delete reinterpret_cast<SpinLockWin32 *>(mSyncObject);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
        delete reinterpret_cast<CriticalSection *>(mSyncObject);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
        mSyncObject = nullptr;
    }
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->lock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->lock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::unlock( void )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->unlock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->unlock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::tryLock( void )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->tryLock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->tryLock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}
#endif

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_osCreateResourceLock( bool initLock )
{
#if 0
    mSyncObject = new Mutex( initLock );

#else

    mSyncObject = new CriticalSection( );
    if ( initLock )
    {
        reinterpret_cast<IEResourceLock *>(mSyncObject)->lock( NECommon::WAIT_INFINITE );
    }

#endif

}

void ResourceLock::_osReleaseResourceLock( void )
{
    reinterpret_cast<IEResourceLock *>(mSyncObject)->unlock( );
    delete reinterpret_cast<IEResourceLock *>(mSyncObject);
    mSyncObject = nullptr;
}

bool ResourceLock::_osLock(unsigned int timeout)
{
    return reinterpret_cast<IEResourceLock *>(mSyncObject)->lock(timeout);
}

bool ResourceLock::_osUnlock( void )
{
    return reinterpret_cast<IEResourceLock *>(mSyncObject)->unlock( );
}

bool ResourceLock::_osTryLock(void)
{
    return reinterpret_cast<IEResourceLock *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_osCreateTimer( bool isSteady )
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

    mSyncObject = static_cast<SYNCHANDLE>(::CreateWaitableTimerEx(nullptr, nullptr, flag, TIMER_ALL_ACCESS));
}

void SyncTimer::_osReleaseTime( void )
{
    CancelWaitableTimer( static_cast<HANDLE>(mSyncObject) );
    CloseHandle( static_cast<HANDLE>(mSyncObject) );
}

bool SyncTimer::_osLock( unsigned int timeout )
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSyncObject), timeout ) == WAIT_OBJECT_0);
}

bool SyncTimer::_osSetTimer( void )
{
    constexpr int NANOSECONDS_COEF_100  { 10'000 };

    LARGE_INTEGER dueTime{};
    dueTime.QuadPart = -(static_cast<LONGLONG>(mTimeout) * NANOSECONDS_COEF_100);
    LONG lPeriod = mIsPeriodic ? static_cast<LONG>(mTimeout) : 0;
    return (SetWaitableTimer( static_cast<HANDLE>(mSyncObject), &dueTime, lPeriod, nullptr, nullptr, FALSE ) != FALSE);
}

bool SyncTimer::_osCancelTimer( void )
{
    return (CancelWaitableTimer( static_cast<HANDLE>(mSyncObject) ) != FALSE);
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int MultiLock::_osLock( unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/ )
{
    void * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS] { };
    for ( int i = 0; i < mSizeCount; ++ i)
    {
        syncHandles[i] = mSyncObjArray[i]->getHandle( );
    }

    int index = MultiLock::LOCK_INDEX_INVALID;
    unsigned int maxEvent= static_cast<uint32_t>(WAIT_OBJECT_0) + static_cast<uint32_t>(mSizeCount);
    unsigned int result  = mSizeCount > 0 ? WaitForMultipleObjectsEx(static_cast<uint32_t>(mSizeCount), static_cast<HANDLE *>(syncHandles), waitForAll ? TRUE : FALSE, timeout, isAlertable ? TRUE : FALSE) : WAIT_FAILED;
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
        LARGE_INTEGER frequency{ };
        QueryPerformanceFrequency(&frequency);
        return ( static_cast<double>(static_cast<double>(frequency.QuadPart) / static_cast<double>(Wait::ONE_SEC.count())) );
    }

    const double _ticksPerNs{ _getFrequencyNs() };

}

void Wait::_osInitTimer(void)
{
    mTimer = ::CreateWaitableTimerEx( nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS );
}

void Wait::_osReleaseTimer(void)
{
    ASSERT(mTimer != nullptr);
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
        LARGE_INTEGER dueTime{};
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
        int64_t deadline = start.QuadPart + static_cast<int64_t>(static_cast<double>(timeout.count()) * _ticksPerNs);
        
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


#endif  // _WIN32
