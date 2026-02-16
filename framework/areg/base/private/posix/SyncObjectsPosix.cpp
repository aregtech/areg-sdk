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
#include "areg/base/SyncObjects.hpp"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/Thread.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEUtilities.hpp"
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

void SyncObject::_osDestroySyncObject()
{
    if (mSyncObject != nullptr)
    {
        SyncObjectPosix * syncObject = reinterpret_cast<SyncObjectPosix *>(mSyncObject);
        mSyncObject = nullptr;

        syncObject->freeResources();
        delete syncObject;
    }
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void Mutex::_osCreateMutex( bool initLock )
{
    mSyncObject = DEBUG_NEW WaitableMutexPosix(initLock, "POSIX_Mutex");
}

bool Mutex::_osLockMutex( unsigned int timeout )
{
    bool result{ false };
    WaitableMutexPosix * syncMutex{ reinterpret_cast<WaitableMutexPosix *>(mSyncObject) };

    if ( NESyncTypesIX::SyncObject0 == SyncLockAndWaitPosix::waitForSingleObject(*syncMutex, timeout) )
    {
        mOwnerThreadId.store(NEUtilities::convToNum<id_type, pthread_t>(syncMutex->getOwningThreadId()));
        result = true;
    }

    return result;
}

bool Mutex::_osUnlockMutex()
{
    bool result{ false };
    WaitableMutexPosix * syncMutex{ reinterpret_cast<WaitableMutexPosix *>(mSyncObject) };

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

void SyncEvent::_osCreateEvent( bool initLock )
{
    mSyncObject    = static_cast<void *>( DEBUG_NEW WaitableEventPosix(!initLock, mAutoReset, "POSIX_Event") );
}

bool SyncEvent::_osUnlockEvent( void * eventHandle )
{
    return reinterpret_cast<WaitableEventPosix *>(eventHandle)->setEvent();
}

bool SyncEvent::_osLockEvent(unsigned int timeout)
{
    WaitableEventPosix * syncEvent{ reinterpret_cast<WaitableEventPosix *>(mSyncObject) };
    return (NESyncTypesIX::SyncObject0 == SyncLockAndWaitPosix::waitForSingleObject(*syncEvent, timeout));
}

bool SyncEvent::_osSetEvent()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->setEvent();
}

bool SyncEvent::_osResetEvent()
{
    return reinterpret_cast<WaitableEventPosix *>(mSyncObject)->resetEvent();
}

void SyncEvent::_osPulseEvent()
{
    reinterpret_cast<WaitableEventPosix *>(mSyncObject)->pulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_osCreateSemaphore( )
{
    mSyncObject = DEBUG_NEW WaitableSemaphorePosix(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

void Semaphore::_osReleaseSemaphore()
{
    static_cast<WaitableSemaphorePosix *>(mSyncObject)->releaseSemaphore( );
}

bool Semaphore::_osLock( unsigned int timeout )
{
    WaitableSemaphorePosix * syncSemaphore{ static_cast<WaitableSemaphorePosix *>(mSyncObject) };
    return (NESyncTypesIX::SyncObject0 == SyncLockAndWaitPosix::waitForSingleObject( *syncSemaphore, timeout ));
}

bool Semaphore::_osUnlock()
{
    return static_cast<WaitableSemaphorePosix *>(mSyncObject)->releaseSemaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_osCreateCriticalSection()
{
    mSyncObject = static_cast<void *>( DEBUG_NEW CriticalSectionPosix(false) );
}

void CriticalSection::_osReleaseCriticalSection()
{
    // do not unlock, it will automatically unlock in CriticalSectionPosix::freeResource()
}

bool CriticalSection::_osLock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->lock();
}

bool CriticalSection::_osUnlock()
{
    reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->unlock( );
    return true;
}

bool CriticalSection::_osTryLock()
{
    return reinterpret_cast<CriticalSectionPosix *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////
#if 0 // TODO: Probably don't need anymore and should be removed

SpinLock::SpinLock()
    : Lockable( SyncObject::eSyncObject::SoSpinlock )
{
    mSyncObject    = DEBUG_NEW SpinLockPosix( );
}

SpinLock::~SpinLock()
{
    delete reinterpret_cast<SpinLockPosix *>(mSyncObject);
    mSyncObject = nullptr;
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->lock();
}

bool SpinLock::unlock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->unlock( );
}

bool SpinLock::tryLock()
{
    return reinterpret_cast<SpinLockPosix *>(mSyncObject)->tryLock( );
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_osCreateResourceLock( bool initLock )
{
    mSyncObject  = DEBUG_NEW MutexPosix(initLock, "ResourceLock");
}

void ResourceLock::_osReleaseResourceLock()
{
    // do not unlock, it will automatically unlock in MutexPosix::freeResource()
}

bool ResourceLock::_osLock(unsigned int timeout)
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->lock(timeout);
}

bool ResourceLock::_osUnlock()
{
    reinterpret_cast<MutexPosix *>(mSyncObject)->unlock( );
    return true;
}

bool ResourceLock::_osTryLock()
{
    return reinterpret_cast<MutexPosix *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_osCreateTimer( bool /* isSteady */ )
{
    mSyncObject= static_cast<void *>(DEBUG_NEW WaitableTimerPosix( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void SyncTimer::_osReleaseTime()
{
    reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->cancelTimer( );
}

bool SyncTimer::_osLock( unsigned int timeout )
{
    return (SyncLockAndWaitPosix::waitForSingleObject( *reinterpret_cast<WaitablePosix *>(mSyncObject), timeout ) == NESyncTypesIX::SyncObject0);
}

bool SyncTimer::_osSetTimer()
{
    return reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->setTimer( mTimeout, mIsPeriodic );
}

bool SyncTimer::_osCancelTimer()
{
    return reinterpret_cast<WaitableTimerPosix *>(mSyncObject)->cancelTimer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int MultiLock::_osLock(unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    WaitablePosix * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        syncHandles[i] = reinterpret_cast<WaitablePosix *>(mSyncObjArray[i]->getHandle( ));
    }

    int index = MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int result = SyncLockAndWaitPosix::waitForMultipleObjects(syncHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int>(NESyncTypesIX::SyncObjectAll):
            index = MultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::eLockedState::StateLocked;
            }
            break;

        case static_cast<int>(NESyncTypesIX::SyncAsyncSignal):
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

        case static_cast<int>(NESyncTypesIX::SyncObjectTimeout):
            index = MultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int>(NESyncTypesIX::SyncObject0)) && (result < mSizeCount) )
            {
                index = result;
                mLockedStates[result] = MultiLock::eLockedState::StateLocked;
            }
            break;
        }

    } while(false);

    return index;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

void Wait::_osInitTimer()
{
}

void Wait::_osReleaseTimer()
{
}

Wait::eWaitResult Wait::_osWaitFor(const Wait::Duration& timeout) const
{
    Wait::eWaitResult result{ timeout.count() >= 0 ? Wait::eWaitResult::WaitIgnored : Wait::eWaitResult::WaitInvalid };
    if (timeout >= Wait::ONE_MUS)
    {
        struct timespec dueTime;
        Wait::Duration mus{ timeout - (timeout % Wait::ONE_MUS) };
        dueTime.tv_sec  = mus >= Wait::ONE_SEC ? (mus.count() / Wait::ONE_SEC.count()) : 0;
        dueTime.tv_nsec = mus.count() % Wait::ONE_SEC.count();
        if (::nanosleep(&dueTime, nullptr) == NECommon::RETURNED_OK)
        {
            result = timeout >= Wait::MIN_WAIT ? Wait::eWaitResult::WaitInMilli : Wait::eWaitResult::WaitInMicro;
        }
        else
        {
            result = Wait::eWaitResult::WaitInvalid;
        }
    }

    return result;
}

#endif  // defined(_POSIX) || defined(POSIX)
