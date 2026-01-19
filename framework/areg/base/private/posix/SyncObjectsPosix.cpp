/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncObjectsPosix.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects. POSIX specific methods
 *
 ************************************************************************/
#include "areg/base/SyncObjects.hpp"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/Thread.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/IESyncObject.hpp"
#include "areg/base/private/posix/WaitableEventIX.hpp"
#include "areg/base/private/posix/WaitableMutexIX.hpp"
#include "areg/base/private/posix/WaitableSemaphoreIX.hpp"
#include "areg/base/private/posix/CriticalSectionIX.hpp"
#include "areg/base/private/posix/WaitableTimerIX.hpp"
#include "areg/base/private/posix/SpinLockIX.hpp"
#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"

#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// IESyncObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESyncObject class methods
//////////////////////////////////////////////////////////////////////////

void IESyncObject::_osDestroySyncObject( void )
{
    if (mSyncObject != nullptr)
    {
        IESyncObjectBaseIX * syncObject = reinterpret_cast<IESyncObjectBaseIX *>(mSyncObject);
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
    mSyncObject = DEBUG_NEW WaitableMutexIX(initLock, "POSIX_Mutex");
}

bool Mutex::_osLockMutex( unsigned int timeout )
{
    bool result{ false };
    WaitableMutexIX * syncMutex{ reinterpret_cast<WaitableMutexIX *>(mSyncObject) };

    if ( NESyncTypesIX::SyncObject0 == SyncLockAndWaitIX::waitForSingleObject(*syncMutex, timeout) )
    {
        mOwnerThreadId.store(NEUtilities::convToNum<id_type, pthread_t>(syncMutex->getOwningThreadId()));
        result = true;
    }

    return result;
}

bool Mutex::_osUnlockMutex( void )
{
    bool result{ false };
    WaitableMutexIX * syncMutex{ reinterpret_cast<WaitableMutexIX *>(mSyncObject) };

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
    mSyncObject    = static_cast<void *>( DEBUG_NEW WaitableEventIX(!initLock, mAutoReset, "POSIX_Event") );
}

bool SyncEvent::_osUnlockEvent( void * eventHandle )
{
    return reinterpret_cast<WaitableEventIX *>(eventHandle)->setEvent();
}

bool SyncEvent::_osLockEvent(unsigned int timeout)
{
    WaitableEventIX * syncEvent{ reinterpret_cast<WaitableEventIX *>(mSyncObject) };
    return (NESyncTypesIX::SyncObject0 == SyncLockAndWaitIX::waitForSingleObject(*syncEvent, timeout));
}

bool SyncEvent::_osSetEvent( void )
{
    return reinterpret_cast<WaitableEventIX *>(mSyncObject)->setEvent();
}

bool SyncEvent::_osResetEvent( void )
{
    return reinterpret_cast<WaitableEventIX *>(mSyncObject)->resetEvent();
}

void SyncEvent::_osPulseEvent( void )
{
    reinterpret_cast<WaitableEventIX *>(mSyncObject)->pulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_osCreateSemaphore( )
{
    mSyncObject = DEBUG_NEW WaitableSemaphoreIX(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

void Semaphore::_osReleaseSemaphore( void )
{
    static_cast<WaitableSemaphoreIX *>(mSyncObject)->releaseSemaphore( );
}

bool Semaphore::_osLock( unsigned int timeout )
{
    WaitableSemaphoreIX * syncSemaphore{ static_cast<WaitableSemaphoreIX *>(mSyncObject) };
    return (NESyncTypesIX::SyncObject0 == SyncLockAndWaitIX::waitForSingleObject( *syncSemaphore, timeout ));
}

bool Semaphore::_osUnlock( void )
{
    return static_cast<WaitableSemaphoreIX *>(mSyncObject)->releaseSemaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_osCreateCriticalSection( void )
{
    mSyncObject = static_cast<void *>( DEBUG_NEW CriticalSectionIX(false) );
}

void CriticalSection::_osReleaseCriticalSection( void )
{
    // do not unlock, it will automatically unlock in CriticalSectionIX::freeResource()
}

bool CriticalSection::_osLock( void )
{
    return reinterpret_cast<CriticalSectionIX *>(mSyncObject)->lock();
}

bool CriticalSection::_osUnlock( void )
{
    reinterpret_cast<CriticalSectionIX *>(mSyncObject)->unlock( );
    return true;
}

bool CriticalSection::_osTryLock( void )
{
    return reinterpret_cast<CriticalSectionIX *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////
#if 0 // TODO: Probably don't need anymore and should be removed

SpinLock::SpinLock( void )
    : IEResourceLock( IESyncObject::eSyncObject::SoSpinlock )
{
    mSyncObject    = DEBUG_NEW SpinLockIX( );
}

SpinLock::~SpinLock( void )
{
    delete reinterpret_cast<SpinLockIX *>(mSyncObject);
    mSyncObject = nullptr;
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return reinterpret_cast<SpinLockIX *>(mSyncObject)->lock();
}

bool SpinLock::unlock( void )
{
    return reinterpret_cast<SpinLockIX *>(mSyncObject)->unlock( );
}

bool SpinLock::tryLock( void )
{
    return reinterpret_cast<SpinLockIX *>(mSyncObject)->tryLock( );
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_osCreateResourceLock( bool initLock )
{
    mSyncObject  = DEBUG_NEW MutexIX(initLock, "ResourceLock");
}

void ResourceLock::_osReleaseResourceLock( void )
{
    // do not unlock, it will automatically unlock in MutexIX::freeResource()
}

bool ResourceLock::_osLock(unsigned int timeout)
{
    return reinterpret_cast<MutexIX *>(mSyncObject)->lock(timeout);
}

bool ResourceLock::_osUnlock(void)
{
    reinterpret_cast<MutexIX *>(mSyncObject)->unlock( );
    return true;
}

bool ResourceLock::_osTryLock(void)
{
    return reinterpret_cast<MutexIX *>(mSyncObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

void SyncTimer::_osCreateTimer( bool /* isSteady */ )
{
    mSyncObject= static_cast<void *>(DEBUG_NEW WaitableTimerIX( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void SyncTimer::_osReleaseTime( void )
{
    reinterpret_cast<WaitableTimerIX *>(mSyncObject)->cancelTimer( );
}

bool SyncTimer::_osLock( unsigned int timeout )
{
    return (SyncLockAndWaitIX::waitForSingleObject( *reinterpret_cast<IEWaitableBaseIX *>(mSyncObject), timeout ) == NESyncTypesIX::SyncObject0);
}

bool SyncTimer::_osSetTimer( void )
{
    return reinterpret_cast<WaitableTimerIX *>(mSyncObject)->setTimer( mTimeout, mIsPeriodic );
}

bool SyncTimer::_osCancelTimer( void )
{
    return reinterpret_cast<WaitableTimerIX *>(mSyncObject)->cancelTimer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int MultiLock::_osLock(unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    IEWaitableBaseIX * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        syncHandles[i] = reinterpret_cast<IEWaitableBaseIX *>(mSyncObjArray[i]->getHandle( ));
    }

    int index = MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int result = SyncLockAndWaitIX::waitForMultipleObjects(syncHandles, mSizeCount, waitForAll, timeout);

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

void Wait::_osInitTimer(void)
{
}

void Wait::_osReleaseTimer(void)
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
        if (::nanosleep(&dueTime, nullptr) == RETURNED_OK)
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
