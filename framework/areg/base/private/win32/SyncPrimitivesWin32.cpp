/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SyncObjectsWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects. Windows specific methods
 *
 ************************************************************************/
#include "areg/base/SyncPrimitives.hpp"

#ifdef  _WIN32
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/win32/SpinLockWin32.hpp"
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

namespace
{
    inline double _getFrequencyNs()
    {
        LARGE_INTEGER frequency{ };
        QueryPerformanceFrequency(&frequency);
        return (static_cast<double>(static_cast<double>(frequency.QuadPart) / static_cast<double>(areg::Wait::ONE_SEC.count())));
    }

    const double _ticksPerNs{ _getFrequencyNs() };

} // namespace

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SyncObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncObject class methods
//////////////////////////////////////////////////////////////////////////

void SyncObject::_os_destroy()
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

void Mutex::_os_create_mutex( bool initLock )
{
    SYNCHANDLE syncObj = static_cast<SYNCHANDLE>(CreateMutex( nullptr, initLock ? TRUE : FALSE, nullptr ));
    mSyncObject = syncObj;
    if ( initLock )
    {
        mOwnerThreadId.store( Thread::current_thread_id( ) );
    }
}

bool Mutex::_os_lock_mutex( uint32_t timeout )
{
    bool result = false;

    if (WaitForSingleObject(static_cast<HANDLE>(mSyncObject), timeout) == WAIT_OBJECT_0)
    {
        mOwnerThreadId.store( Thread::current_thread_id( ) );
        result = true;
    }

    return result;
}

bool Mutex::_os_unlock_mutex()
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

void SyncEvent::_os_create_event( bool initLock )
{
    mSyncObject = static_cast<void *>(CreateEvent( nullptr, mAutoReset ? FALSE : TRUE, initLock ? FALSE : TRUE, nullptr ));
}

bool SyncEvent::_os_unlock_event( void * /* eventHandle */ ) noexcept
{
    return ( ::SetEvent( static_cast<HANDLE>(mSyncObject) ) != FALSE );
}

bool SyncEvent::_os_lock_event(uint32_t timeout)
{
    return ( WaitForSingleObject(static_cast<HANDLE>(mSyncObject), timeout) == WAIT_OBJECT_0 );
}

bool SyncEvent::_os_set_event() noexcept
{
    return (::SetEvent(static_cast<HANDLE>(mSyncObject)) != FALSE);
}

bool SyncEvent::_os_reset_event() noexcept
{
    return (::ResetEvent(static_cast<HANDLE>(mSyncObject)) != FALSE);
}

void SyncEvent::_os_pulse_event() noexcept
{
    ::PulseEvent(static_cast<HANDLE>(mSyncObject));
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_os_create_semaphore()
{
    mSyncObject= static_cast<void *>(CreateSemaphore(nullptr, mCurrCount.load(), mMaxCount, nullptr));
}

void Semaphore::_os_release_semaphore()
{
}

bool Semaphore::_os_lock(uint32_t timeout)
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSyncObject), timeout ) == WAIT_OBJECT_0);
}

bool Semaphore::_os_unlock()
{
    return (ReleaseSemaphore( static_cast<HANDLE>(mSyncObject), 1, nullptr ) == TRUE);
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_os_create_critical_section()
{
    mSyncObject = static_cast<void *>( DEBUG_NEW uint8_t [sizeof(CRITICAL_SECTION)] );
    areg::construct_elems<CRITICAL_SECTION>( mSyncObject, 1 );
    InitializeCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
}

void CriticalSection::_os_release_critical_section()
{
    LeaveCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    DeleteCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    delete[] reinterpret_cast<uint8_t *>(mSyncObject);
    mSyncObject = nullptr;
}

bool CriticalSection::_os_lock()
{
    EnterCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    return true;
}

bool CriticalSection::_os_unlock()
{
    LeaveCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject) );
    return true;
}

bool CriticalSection::_os_try_lock()
{
    return (TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSyncObject)) == TRUE);
}

#if 0 // TODO: Probably don't need anymore and should be removed

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock()
    : Lockable( SyncObject::SyncKind::SoSpinlock )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    mSyncObject    = DEBUG_NEW SpinLockWin32( );
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    mSyncObject    = DEBUG_NEW CriticalSection( );
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

SpinLock::~SpinLock()
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

bool SpinLock::lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ )
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->lock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->lock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::unlock()
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->unlock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->unlock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}

bool SpinLock::try_lock()
{
#if defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<SpinLockWin32 *>(mSyncObject)->try_lock( ) : false);
#else // defined (__cplusplus) && (__cplusplus > 201703L)
    return (mSyncObject != nullptr ? reinterpret_cast<CriticalSection *>(mSyncObject)->try_lock( ) : false);
#endif // defined (__cplusplus) && (__cplusplus > 201703L)
}
#endif

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_os_create_resource_lock( bool initLock )
{
#if 0
    mSyncObject = new Mutex( initLock );

#else

    mSyncObject = new CriticalSection( );
    if ( initLock )
    {
        reinterpret_cast<Lockable *>(mSyncObject)->lock( areg::WAIT_INFINITE );
    }

#endif

}

void ResourceLock::_os_release_resource_lock()
{
    reinterpret_cast<Lockable *>(mSyncObject)->unlock( );
    delete reinterpret_cast<Lockable *>(mSyncObject);
    mSyncObject = nullptr;
}

bool ResourceLock::_os_lock(uint32_t timeout)
{
    return reinterpret_cast<Lockable *>(mSyncObject)->lock(timeout);
}

bool ResourceLock::_os_unlock()
{
    return reinterpret_cast<Lockable *>(mSyncObject)->unlock( );
}

bool ResourceLock::_os_try_lock()
{
    return reinterpret_cast<Lockable *>(mSyncObject)->try_lock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_os_create_timer( bool isSteady )
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

void SyncTimer::_os_release_time()
{
    CancelWaitableTimer( static_cast<HANDLE>(mSyncObject) );
    CloseHandle( static_cast<HANDLE>(mSyncObject) );
}

bool SyncTimer::_os_lock( uint32_t timeout )
{
    return (WaitForSingleObject( static_cast<HANDLE>(mSyncObject), timeout ) == WAIT_OBJECT_0);
}

bool SyncTimer::_os_set_timer()
{
    constexpr int32_t NANOSECONDS_COEF_100  { 10'000 };

    LARGE_INTEGER due_time{};
    due_time.QuadPart = -(static_cast<LONGLONG>(mTimeout) * NANOSECONDS_COEF_100);
    LONG lPeriod = mIsPeriodic ? static_cast<LONG>(mTimeout) : 0;
    return (SetWaitableTimer( static_cast<HANDLE>(mSyncObject), &due_time, lPeriod, nullptr, nullptr, FALSE ) != FALSE);
}

bool SyncTimer::_os_cancel_timer()
{
    return (CancelWaitableTimer( static_cast<HANDLE>(mSyncObject) ) != FALSE);
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int32_t MultiLock::_os_lock( uint32_t timeout /* = areg::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/ )
{
    void * syncHandles[areg::MAXIMUM_WAITING_OBJECTS] { };
    for ( int i = 0; i < mSizeCount; ++ i)
    {
        syncHandles[i] = mSyncObjArray[i]->handle( );
    }

    int32_t index = MultiLock::LOCK_INDEX_INVALID;
    uint32_t maxEvent= static_cast<uint32_t>(WAIT_OBJECT_0) + static_cast<uint32_t>(mSizeCount);
    uint32_t result  = mSizeCount > 0 ? WaitForMultipleObjectsEx(static_cast<uint32_t>(mSizeCount), static_cast<HANDLE *>(syncHandles), waitForAll ? TRUE : FALSE, timeout, isAlertable ? TRUE : FALSE) : WAIT_FAILED;
    if (result < maxEvent)
    {
        if (waitForAll == false)
        {
            index = static_cast<int32_t>(result - WAIT_OBJECT_0);
            ASSERT((index >= 0) && index < mSizeCount);
            mLockedStates[index] = MultiLock::LockState::Locked;
        }
        else
        {
            for (int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::LockState::Locked;
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

void Wait::_os_init_timer()
{
    mTimer = ::CreateWaitableTimerEx( nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS );
}

void Wait::_os_release_timer()
{
    ASSERT(mTimer != nullptr);
    ::CancelWaitableTimer( mTimer );
    ::CloseHandle( mTimer );
    mTimer = nullptr;
}

Wait::WaitResolution Wait::_os_wait_for(const Wait::Duration& timeout) const
{
    static constexpr int64_t _COEF{ -10'000 };

    Wait::WaitResolution result {Wait::WaitResolution::Invalid};
    if (timeout >= Wait::MIN_WAIT)
    {
        LARGE_INTEGER due_time{};
        due_time.QuadPart = static_cast<int64_t>(timeout.count() / ONE_MS.count()) * _COEF;
        ::SetWaitableTimer(mTimer, &due_time, 0, nullptr, nullptr, FALSE);
        if (::WaitForSingleObject(mTimer, INFINITE) == WAIT_OBJECT_0)
        {
            result = Wait::WaitResolution::Millisecond;
        }
    }
    else if (timeout >= Wait::ONE_MUS)
    {
        LARGE_INTEGER start, due_time;
        QueryPerformanceCounter(&start);
        // due time is current time in ticks + expected ticks
        int64_t deadline = start.QuadPart + static_cast<int64_t>(static_cast<double>(timeout.count()) * _ticksPerNs);
        
        do
        {
            ::Sleep(areg::DO_NOT_WAIT);
            ::QueryPerformanceCounter(&due_time);
        } while (due_time.QuadPart < deadline);

        result = Wait::WaitResolution::Microsecond;
    }
    else if (timeout.count() > 0)
    {
        result = Wait::WaitResolution::Ignored;
    }

    return result;
}


} // namespace areg
#endif  // _WIN32
