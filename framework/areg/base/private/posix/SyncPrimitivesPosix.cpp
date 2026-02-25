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
#include "areg/base/SyncPrimitives.hpp"
#include <cstdint>

#if  defined(_POSIX) || defined(POSIX)

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

#include <string.h>
#include <time.h>

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
        SyncObjectPosix * syncObject = reinterpret_cast<SyncObjectPosix *>(mSyncObject);
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
    mSyncObject = DEBUG_NEW WaitableMutexPosix(initLock, "POSIX_Mutex");
}

bool Mutex::_os_lock_mutex( uint32_t timeout )
{
    bool result{ false };
    WaitableMutexPosix * syncMutex{ reinterpret_cast<WaitableMutexPosix *>(mSyncObject) };

    if ( static_cast<int32_t>(NESyncTypesIX::SyncSignal::First) == SyncLockAndWaitPosix::wait_single(*syncMutex, timeout) )
    {
        mOwnerThreadId.store(NEUtilities::to_num<id_type, pthread_t>(syncMutex->owning_thread_id()));
        result = true;
    }

    return result;
}

bool Mutex::_os_unlock_mutex()
{
    bool result{ false };
    WaitableMutexPosix * syncMutex{ reinterpret_cast<WaitableMutexPosix *>(mSyncObject) };

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
    mSyncObject    = static_cast<void *>( DEBUG_NEW WaitableEventPosix(!initLock, mAutoReset, "POSIX_Event") );
}

bool SyncEvent::_os_unlock_event( void * eventHandle )
{
    return reinterpret_cast<WaitableEventPosix *>(eventHandle)->set_event();
}

bool SyncEvent::_os_lock_event(uint32_t timeout)
{
    WaitableEventPosix * syncEvent{ reinterpret_cast<WaitableEventPosix *>(mSyncObject) };
    return (static_cast<int32_t>(NESyncTypesIX::SyncSignal::First) == SyncLockAndWaitPosix::wait_single(*syncEvent, timeout));
}

bool SyncEvent::_os_set_event()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->set_event();
}

bool SyncEvent::_os_reset_event()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->reset();
}

void SyncEvent::_os_pulse_event()
{
    reinterpret_cast<WaitableEventPosix *>(mSyncObject)->pulse_event();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_os_create_semaphore( )
{
    mSyncObject = DEBUG_NEW WaitableSemaphorePosix(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

void Semaphore::_os_release_semaphore()
{
    static_cast<WaitableSemaphorePosix *>(mSyncObject)->release_semaphore( );
}

bool Semaphore::_os_lock( uint32_t timeout )
{
    WaitableSemaphorePosix * syncSemaphore{ static_cast<WaitableSemaphorePosix *>(mSyncObject) };
    return (static_cast<int32_t>(NESyncTypesIX::SyncSignal::First)  == SyncLockAndWaitPosix::wait_single( *syncSemaphore, timeout ));
}

bool Semaphore::_os_unlock()
{
    return static_cast<WaitableSemaphorePosix *>(mSyncObject)->release_semaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_os_create_critical_section()
{
    mSyncObject = static_cast<void *>( DEBUG_NEW CriticalSectionPosix(false) );
}

void CriticalSection::_os_release_critical_section()
{
    // do not unlock, it will automatically unlock in CriticalSectionPosix::freeResource()
}

bool CriticalSection::_os_lock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->lock();
}

bool CriticalSection::_os_unlock()
{
    reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->unlock( );
    return true;
}

bool CriticalSection::_os_try_lock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->try_lock();
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////
#if 0 // TODO: Probably don't need anymore and should be removed

SpinLock::SpinLock()
    : Lockable( SyncObject::SyncKind::SoSpinlock )
{
    mSyncObject    = DEBUG_NEW SpinLockPosix( );
}

SpinLock::~SpinLock()
{
    delete reinterpret_cast<SpinLockPosix *>(mSyncObject);
    mSyncObject = nullptr;
}

bool SpinLock::lock( uint32_t /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->lock();
}

bool SpinLock::unlock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->unlock( );
}

bool SpinLock::try_lock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->try_lock( );
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_os_create_resource_lock( bool initLock )
{
    mSyncObject  = DEBUG_NEW MutexPosix(initLock, "ResourceLock");
}

void ResourceLock::_os_release_resource_lock()
{
    // do not unlock, it will automatically unlock in MutexPosix::freeResource()
}

bool ResourceLock::_os_lock(uint32_t timeout)
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->lock(timeout);
}

bool ResourceLock::_os_unlock()
{
    reinterpret_cast<MutexPosix *>(mSyncObject)->unlock( );
    return true;
}

bool ResourceLock::_os_try_lock()
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->try_lock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_os_create_timer( bool /* isSteady */ )
{
    mSyncObject= static_cast<void *>(DEBUG_NEW WaitableTimerPosix( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void SyncTimer::_os_release_time()
{
    reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->cancel_timer( );
}

bool SyncTimer::_os_lock( uint32_t timeout )
{
    return (static_cast<int32_t>(NESyncTypesIX::SyncSignal::First) == SyncLockAndWaitPosix::wait_single( *reinterpret_cast<WaitablePosix *>(mSyncObject), timeout ));
}

bool SyncTimer::_os_set_timer()
{
    return reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->set_timer( mTimeout, mIsPeriodic );
}

bool SyncTimer::_os_cancel_timer()
{
    return reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->cancel_timer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int32_t MultiLock::_os_lock(uint32_t timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    WaitablePosix * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        syncHandles[i] = reinterpret_cast<WaitablePosix *>(mSyncObjArray[i]->handle( ));
    }

    int32_t index = MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int32_t result = SyncLockAndWaitPosix::wait_multiple(syncHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int32_t>(NESyncTypesIX::SyncSignal::All):
            index = MultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::LockState::Locked;
            }
            break;

        case static_cast<int32_t>(NESyncTypesIX::SyncSignal::AsyncSignal):
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

        case static_cast<int32_t>(NESyncTypesIX::SyncSignal::Timeout):
            index = MultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int32_t>(NESyncTypesIX::SyncSignal::First)) && (result < mSizeCount) )
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
        if (::nanosleep(&due_time, nullptr) == NECommon::RETURNED_OK)
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

#endif  // defined(_POSIX) || defined(POSIX)
