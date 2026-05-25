/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncObjectsPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects. POSIX specific methods
 *
 ************************************************************************/

#if  defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/SyncObject.hpp"
#include "areg/base/private/posix/WaitableEventPosix.hpp"
#include "areg/base/private/posix/WaitableMutexPosix.hpp"
#include "areg/base/private/posix/WaitableSemaphorePosix.hpp"
#include "areg/base/private/posix/CriticalSectionPosix.hpp"
#include "areg/base/private/posix/WaitableTimerPosix.hpp"
#include "areg/base/private/posix/SpinLockPosix.hpp"
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"

#include <cstdint>
#include <string.h>
#include <time.h>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// SyncObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncObject class methods
//////////////////////////////////////////////////////////////////////////

void SyncObject::_os_destroy()
{
    if (mSyncObject != nullptr)
    {
        areg::os::SyncObjectPosix * syncObject = reinterpret_cast<areg::os::SyncObjectPosix *>(mSyncObject);
        mSyncObject = nullptr;

        syncObject->free_resources();
        delete syncObject;
    }
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void Mutex::_os_create_mutex( bool initLock )
{
    mSyncObject = DEBUG_NEW areg::os::WaitableMutexPosix(initLock, "POSIX_Mutex");
}

bool Mutex::_os_lock_mutex( uint32_t timeout )
{
    bool result{ false };
    areg::os::WaitableMutexPosix * syncMutex{ reinterpret_cast<areg::os::WaitableMutexPosix *>(mSyncObject) };

    if ( static_cast<int32_t>(areg::os::SyncSignal::First) == areg::os::SyncLockAndWaitPosix::wait_single(*syncMutex, timeout) )
    {
        mOwnerThreadId.store(areg::to_num<id_type, pthread_t>(syncMutex->owning_thread_id()));
        result = true;
    }

    return result;
}

bool Mutex::_os_unlock_mutex()
{
    bool result{ false };
    areg::os::WaitableMutexPosix * syncMutex{ reinterpret_cast<areg::os::WaitableMutexPosix *>(mSyncObject) };

    if ( syncMutex->release_mutex() )
    {
        mOwnerThreadId.store(0);
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class implementation
//////////////////////////////////////////////////////////////////////////

void SyncEvent::_os_create_event( bool initLock )
{
    mSyncObject    = static_cast<void *>( DEBUG_NEW areg::os::WaitableEventPosix(!initLock, mAutoReset, "POSIX_Event") );
}

bool SyncEvent::_os_unlock_event( void * eventHandle ) noexcept
{
    return reinterpret_cast<areg::os::WaitableEventPosix *>(eventHandle)->set_signaled();
}

bool SyncEvent::_os_lock_event(uint32_t timeout)
{
    areg::os::WaitableEventPosix * syncEvent{ reinterpret_cast<areg::os::WaitableEventPosix *>(mSyncObject) };
    return (static_cast<int32_t>(areg::os::SyncSignal::First) == areg::os::SyncLockAndWaitPosix::wait_single(*syncEvent, timeout));
}

bool SyncEvent::_os_set_event() noexcept
{
    return reinterpret_cast<areg::os::WaitableEventPosix *>(mSyncObject)->set_signaled();
}

bool SyncEvent::_os_reset_event() noexcept
{
    return reinterpret_cast<areg::os::WaitableEventPosix *>(mSyncObject)->reset();
}

void SyncEvent::_os_pulse_event() noexcept
{
    reinterpret_cast<areg::os::WaitableEventPosix *>(mSyncObject)->pulse_event();
}

int32_t SyncEvent::_os_wait_any( SyncEvent** events, int32_t count, uint32_t timeout ) noexcept
{
    areg::os::WaitablePosix* handles[areg::MAXIMUM_WAITING_OBJECTS];
    for ( int32_t i = 0; i < count; ++i )
    {
        handles[i] = reinterpret_cast<areg::os::WaitablePosix*>(events[i]->handle());
    }

    int32_t result = areg::os::SyncLockAndWaitPosix::wait_multiple(handles, count, false, timeout);
    return ((result >= 0) && (result < count)) ? result : SyncEvent::WAIT_ANY_TIMEOUT;
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_os_create_semaphore( )
{
    mSyncObject = DEBUG_NEW areg::os::WaitableSemaphorePosix(static_cast<int32_t>(mMaxCount), static_cast<int32_t>(mCurrCount.load()), "POSIX_Semaphore");
}

void Semaphore::_os_release_semaphore()
{
    static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject)->release_semaphore( );
}

bool Semaphore::_os_lock( uint32_t timeout )
{
    areg::os::WaitableSemaphorePosix * syncSemaphore{ static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject) };
    return (static_cast<int32_t>(areg::os::SyncSignal::First)  == areg::os::SyncLockAndWaitPosix::wait_single( *syncSemaphore, timeout ));
}

bool Semaphore::_os_unlock()
{
    return static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject)->release_semaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_os_create_critical_section()
{
    mSyncObject = static_cast<void *>( DEBUG_NEW areg::os::CriticalSectionPosix(false) );
}

void CriticalSection::_os_release_critical_section()
{
}

bool CriticalSection::_os_lock()
{
    return reinterpret_cast<areg::os::CriticalSectionPosix *>(mSyncObject)->lock();
}

bool CriticalSection::_os_unlock()
{
    reinterpret_cast<areg::os::CriticalSectionPosix *>(mSyncObject)->unlock( );
    return true;
}

bool CriticalSection::_os_try_lock()
{
    return reinterpret_cast<areg::os::CriticalSectionPosix *>(mSyncObject)->try_lock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_os_create_timer( bool /* isSteady */ )
{
    mSyncObject= static_cast<void *>(DEBUG_NEW areg::os::WaitableTimerPosix( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void SyncTimer::_os_release_time()
{
    reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->cancel_timer( );
}

bool SyncTimer::_os_lock( uint32_t timeout )
{
    return (static_cast<int32_t>(areg::os::SyncSignal::First) == areg::os::SyncLockAndWaitPosix::wait_single( *reinterpret_cast<areg::os::WaitablePosix *>(mSyncObject), timeout ));
}

bool SyncTimer::_os_set_timer()
{
    return reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->set_timer( mTimeout, mIsPeriodic );
}

bool SyncTimer::_os_cancel_timer()
{
    return reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->cancel_timer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int32_t MultiLock::_os_lock(uint32_t timeout /* = areg::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    areg::os::WaitablePosix * syncHandles[areg::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        syncHandles[i] = reinterpret_cast<areg::os::WaitablePosix *>(mSyncObjArray[i]->handle( ));
    }

    int32_t index = MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int32_t result = areg::os::SyncLockAndWaitPosix::wait_multiple(syncHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int32_t>(areg::os::SyncSignal::All):
            index = MultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::LockState::Locked;
            }
            break;

        case static_cast<int32_t>(areg::os::SyncSignal::AsyncSignal):
                if (isAlertable)
                {
                    index = MultiLock::LOCK_INDEX_COMPLETION;
                }
                else
                {
                    index = MultiLock::LOCK_INDEX_INVALID;
                    continue;
                }
            break;

        case static_cast<int32_t>(areg::os::SyncSignal::Timeout):
            index = MultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int32_t>(areg::os::SyncSignal::First)) && (result < mSizeCount) )
            {
                index = result;
                mLockedStates[result] = MultiLock::LockState::Locked;
            }
            break;
        }

    } while(false);

    return index;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

void Wait::_os_init_timer()
{
}

void Wait::_os_release_timer()
{
}

Wait::WaitResolution Wait::_os_wait_for(const Wait::Duration& timeout) const
{
    Wait::WaitResolution result{ timeout.count() >= 0 ? Wait::WaitResolution::Ignored : Wait::WaitResolution::Invalid };
    if (timeout >= Wait::ONE_MUS)
    {
        struct timespec due_time;
        Wait::Duration mus{ timeout - (timeout % Wait::ONE_MUS) };
        due_time.tv_sec  = mus >= Wait::ONE_SEC ? (mus.count() / Wait::ONE_SEC.count()) : 0;
        due_time.tv_nsec = mus.count() % Wait::ONE_SEC.count();
        if (::nanosleep(&due_time, nullptr) == areg::RETURNED_OK)
        {
            result = timeout >= Wait::MIN_WAIT ? Wait::WaitResolution::Millisecond : Wait::WaitResolution::Microsecond;
        }
        else
        {
            result = Wait::WaitResolution::Invalid;
        }
    }

    return result;
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
