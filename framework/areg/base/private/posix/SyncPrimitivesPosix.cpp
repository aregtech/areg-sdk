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

void areg::SyncObject::_osDestroySyncObject()
{
    if (mSyncObject != nullptr)
    {
        areg::os::SyncObjectPosix * syncObject = reinterpret_cast<areg::os::SyncObjectPosix *>(mSyncObject);
        mSyncObject = nullptr;

        syncObject->freeResources();
        delete syncObject;
    }
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void areg::Mutex::_osCreateMutex( bool initLock )
{
    mSyncObject = DEBUG_NEW areg::os::WaitableMutexPosix(initLock, "POSIX_Mutex");
}

bool areg::Mutex::_osLockMutex( uint32_t timeout )
{
    bool result{ false };
    areg::os::WaitableMutexPosix * syncMutex{ reinterpret_cast<areg::os::WaitableMutexPosix *>(mSyncObject) };

    if ( static_cast<int32_t>(areg::os::SyncSignal::First) == SyncLockAndWaitPosix::waitForSingleObject(*syncMutex, timeout) )
    {
        mOwnerThreadId.store(areg::convToNum<id_type, pthread_t>(syncMutex->getOwningThreadId()));
        result = true;
    }

    return result;
}

bool areg::Mutex::_osUnlockMutex()
{
    bool result{ false };
    areg::os::WaitableMutexPosix * syncMutex{ reinterpret_cast<areg::os::WaitableMutexPosix *>(mSyncObject) };

    if ( syncMutex->releaseMutex() )
    {
        mOwnerThreadId.store(0);
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class implementation
//////////////////////////////////////////////////////////////////////////

void areg::SyncEvent::_osCreateEvent( bool initLock )
{
    mSyncObject    = static_cast<void *>( DEBUG_NEW WaitableEventPosix(!initLock, mAutoReset, "POSIX_Event") );
}

bool areg::SyncEvent::_osUnlockEvent( void * eventHandle )
{
    return reinterpret_cast<WaitableEventPosix *>(eventHandle)->setEvent();
}

bool areg::SyncEvent::_osLockEvent(uint32_t timeout)
{
    WaitableEventPosix * syncEvent{ reinterpret_cast<WaitableEventPosix *>(mSyncObject) };
    return (static_cast<int32_t>(areg::os::SyncSignal::First) == SyncLockAndWaitPosix::waitForSingleObject(*syncEvent, timeout));
}

bool areg::SyncEvent::_osSetEvent()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->setEvent();
}

bool areg::SyncEvent::_osResetEvent()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->resetEvent();
}

void areg::SyncEvent::_osPulseEvent()
{
    reinterpret_cast<WaitableEventPosix *>(mSyncObject)->pulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void areg::Semaphore::_osCreateSemaphore( )
{
    mSyncObject = DEBUG_NEW areg::os::WaitableSemaphorePosix(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

void areg::Semaphore::_osReleaseSemaphore()
{
    static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject)->releaseSemaphore( );
}

bool areg::Semaphore::_osLock( uint32_t timeout )
{
    areg::os::WaitableSemaphorePosix * syncSemaphore{ static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject) };
    return (static_cast<int32_t>(areg::os::SyncSignal::First)  == SyncLockAndWaitPosix::waitForSingleObject( *syncSemaphore, timeout ));
}

bool areg::Semaphore::_osUnlock()
{
    return static_cast<areg::os::WaitableSemaphorePosix *>(mSyncObject)->releaseSemaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void areg::CriticalSection::_osCreateCriticalSection()
{
    mSyncObject = static_cast<void *>( DEBUG_NEW CriticalSectionPosix(false) );
}

void areg::CriticalSection::_osReleaseCriticalSection()
{
    // do not unlock, it will automatically unlock in CriticalSectionPosix::freeResource()
}

bool areg::CriticalSection::_osLock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->lock();
}

bool areg::CriticalSection::_osUnlock()
{
    reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->unlock( );
    return true;
}

bool areg::CriticalSection::_osTryLock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////
#if 0 // TODO: Probably don't need anymore and should be removed

areg::SpinLock::SpinLock()
    : areg::Lockable( areg::SyncObject::SyncKind::SoSpinlock )
{
    mSyncObject    = DEBUG_NEW SpinLockPosix( );
}

areg::SpinLock::~areg::SpinLock()
{
    delete reinterpret_cast<SpinLockPosix *>(mSyncObject);
    mSyncObject = nullptr;
}

bool areg::SpinLock::lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ )
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->lock();
}

bool areg::SpinLock::unlock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->unlock( );
}

bool areg::SpinLock::tryLock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->tryLock( );
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void areg::ResourceLock::_osCreateResourceLock( bool initLock )
{
    mSyncObject  = DEBUG_NEW MutexPosix(initLock, "ResourceLock");
}

void areg::ResourceLock::_osReleaseResourceLock()
{
    // do not unlock, it will automatically unlock in MutexPosix::freeResource()
}

bool areg::ResourceLock::_osLock(uint32_t timeout)
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->lock(timeout);
}

bool areg::ResourceLock::_osUnlock()
{
    reinterpret_cast<MutexPosix *>(mSyncObject)->unlock( );
    return true;
}

bool areg::ResourceLock::_osTryLock()
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void areg::SyncTimer::_osCreateTimer( bool /* isSteady */ )
{
    mSyncObject= static_cast<void *>(DEBUG_NEW areg::os::WaitableTimerPosix( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void areg::SyncTimer::_osReleaseTime()
{
    reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->cancelTimer( );
}

bool areg::SyncTimer::_osLock( uint32_t timeout )
{
    return (static_cast<int32_t>(areg::os::SyncSignal::First) == SyncLockAndWaitPosix::waitForSingleObject( *reinterpret_cast<areg::os::WaitablePosix *>(mSyncObject), timeout ));
}

bool areg::SyncTimer::_osSetTimer()
{
    return reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->setTimer( mTimeout, mIsPeriodic );
}

bool areg::SyncTimer::_osCancelTimer()
{
    return reinterpret_cast<areg::os::WaitableTimerPosix *>(mSyncObject)->cancelTimer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int32_t areg::MultiLock::_osLock(uint32_t timeout /* = areg::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    areg::os::WaitablePosix * syncHandles[areg::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        syncHandles[i] = reinterpret_cast<areg::os::WaitablePosix *>(mSyncObjArray[i]->getHandle( ));
    }

    int32_t index = areg::MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int32_t result = SyncLockAndWaitPosix::waitForMultipleObjects(syncHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int32_t>(areg::os::SyncSignal::All):
            index = areg::MultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = areg::MultiLock::LockState::Locked;
            }
            break;

        case static_cast<int32_t>(areg::os::SyncSignal::AsyncSignal):
                if (isAlertable)
                {
                    index = areg::MultiLock::LOCK_INDEX_COMPLETION;
                }
                else
                {
                    index = areg::MultiLock::LOCK_INDEX_INVALID;
                    continue;
                }
            break;

        case static_cast<int32_t>(areg::os::SyncSignal::Timeout):
            index = areg::MultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int32_t>(areg::os::SyncSignal::First)) && (result < mSizeCount) )
            {
                index = result;
                mLockedStates[result] = areg::MultiLock::LockState::Locked;
            }
            break;
        }

    } while(false);

    return index;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

void areg::Wait::_osInitTimer()
{
}

void areg::Wait::_osReleaseTimer()
{
}

areg::Wait::WaitResolution areg::Wait::_osWaitFor(const areg::Wait::Duration& timeout) const
{
    areg::Wait::WaitResolution result{ timeout.count() >= 0 ? areg::Wait::WaitResolution::Ignored : areg::Wait::WaitResolution::Invalid };
    if (timeout >= areg::Wait::ONE_MUS)
    {
        struct timespec dueTime;
        areg::Wait::Duration mus{ timeout - (timeout % areg::Wait::ONE_MUS) };
        dueTime.tv_sec  = mus >= areg::Wait::ONE_SEC ? (mus.count() / areg::Wait::ONE_SEC.count()) : 0;
        dueTime.tv_nsec = mus.count() % areg::Wait::ONE_SEC.count();
        if (::nanosleep(&dueTime, nullptr) == areg::RETURNED_OK)
        {
            result = timeout >= areg::Wait::MIN_WAIT ? areg::Wait::WaitResolution::Millisecond : areg::Wait::WaitResolution::Microsecond;
        }
        else
        {
            result = areg::Wait::WaitResolution::Invalid;
        }
    }

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
