/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/ESynchObjectsWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects. POSIX specific methods
 *
 ************************************************************************/
#include "areg/base/SynchObjects.hpp"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/Thread.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/IESynchObject.hpp"
#include "areg/base/private/posix/WaitableEventIX.hpp"
#include "areg/base/private/posix/WaitableMutexIX.hpp"
#include "areg/base/private/posix/WaitableSemaphoreIX.hpp"
#include "areg/base/private/posix/CriticalSectionIX.hpp"
#include "areg/base/private/posix/WaitableTimerIX.hpp"
#include "areg/base/private/posix/SpinLockIX.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

#include <string.h>
#include <time.h>

//////////////////////////////////////////////////////////////////////////
// Internal static methods
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// IESynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class methods
//////////////////////////////////////////////////////////////////////////

void IESynchObject::_osDestroySynchObject( void )
{
    if (mSynchObject != nullptr)
    {
        IESynchObjectBaseIX * synchObject = reinterpret_cast<IESynchObjectBaseIX *>(mSynchObject);
        mSynchObject = nullptr;

        synchObject->freeResources();
        delete synchObject;
    }
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

void Mutex::_osCreateMutex( bool initLock )
{
    mSynchObject = DEBUG_NEW WaitableMutexIX(initLock, "POSIX_Mutex");
}

bool Mutex::_osLockMutex( unsigned int timeout )
{
    bool result{ false };
    WaitableMutexIX * synchMutex{ reinterpret_cast<WaitableMutexIX *>(mSynchObject) };
    
    if ( NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchMutex, timeout) )
    {
        mOwnerThreadId.store(reinterpret_cast<id_type>(synchMutex->getOwningThreadId()));
        result = true;
    }

    return result;
}

bool Mutex::_osUnlockMutex( void )
{
    bool result{ false };
    WaitableMutexIX * synchMutex{ reinterpret_cast<WaitableMutexIX *>(mSynchObject) };

    if ( synchMutex->releaseMutex() )
    {
        mOwnerThreadId.store(0);
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class implementation
//////////////////////////////////////////////////////////////////////////

void SynchEvent::_osCreateEvent( bool initLock )
{
    mSynchObject    = static_cast<void *>( DEBUG_NEW WaitableEventIX(initLock == false, mAutoReset, "POSIX_Event") );
}

bool SynchEvent::_osUnlockEvent( void * eventHandle )
{
    return reinterpret_cast<WaitableEventIX *>(eventHandle)->setEvent();
}

bool SynchEvent::_osLockEvent(unsigned int timeout)
{
    WaitableEventIX * synchEvent{ reinterpret_cast<WaitableEventIX *>(mSynchObject) };
    return (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchEvent, timeout));
}

bool SynchEvent::_osSetEvent( void )
{
    return reinterpret_cast<WaitableEventIX *>(mSynchObject)->setEvent();
}

bool SynchEvent::_osResetEvent( void )
{
    return reinterpret_cast<WaitableEventIX *>(mSynchObject)->resetEvent();
}

void SynchEvent::_osPulseEvent( void )
{
    reinterpret_cast<WaitableEventIX *>(mSynchObject)->pulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

void Semaphore::_osCreateSemaphore( )
{
    mSynchObject = DEBUG_NEW WaitableSemaphoreIX(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

void Semaphore::_osReleaseSemaphore( void )
{
    static_cast<WaitableSemaphoreIX *>(mSynchObject)->releaseSemaphore( );
}

bool Semaphore::_osLock( unsigned int timeout )
{
    WaitableSemaphoreIX * synchSemaphore{ static_cast<WaitableSemaphoreIX *>(mSynchObject) };
    return (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject( *synchSemaphore, timeout ));
}

bool Semaphore::_osUnlock( void )
{
    return static_cast<WaitableSemaphoreIX *>(mSynchObject)->releaseSemaphore( );
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

void CriticalSection::_osCreateCriticalSection( void )
{
    mSynchObject = static_cast<void *>( DEBUG_NEW CriticalSectionIX(false) );
}

void CriticalSection::_osReleaseCriticalSection( void )
{
    // do not unlock, it will automatically unlock in CriticalSectionIX::freeResource()
}

bool CriticalSection::_osLock( void )
{
    return reinterpret_cast<CriticalSectionIX *>(mSynchObject)->lock();
}

bool CriticalSection::_osUnlock( void )
{
    reinterpret_cast<CriticalSectionIX *>(mSynchObject)->unlock( );
    return true;
}

bool CriticalSection::_osTryLock( void )
{
    return reinterpret_cast<CriticalSectionIX *>(mSynchObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////
#if 0 // TODO: Probably don't need anymore and should be removed

SpinLock::SpinLock( void )
    : IEResourceLock( IESynchObject::eSyncObject::SoSpinlock )
{
    mSynchObject    = DEBUG_NEW SpinLockIX( );
}

SpinLock::~SpinLock( void )
{
    delete reinterpret_cast<SpinLockIX *>(mSynchObject);
    mSynchObject = nullptr;
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return reinterpret_cast<SpinLockIX *>(mSynchObject)->lock();
}

bool SpinLock::unlock( void )
{
    return reinterpret_cast<SpinLockIX *>(mSynchObject)->unlock( );
}

bool SpinLock::tryLock( void )
{
    return reinterpret_cast<SpinLockIX *>(mSynchObject)->tryLock( );
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

void ResourceLock::_osCreateResourceLock( bool initLock )
{
    mSynchObject  = DEBUG_NEW MutexIX(initLock, "ResourceLock");
}

void ResourceLock::_osReleaseResourceLock( void )
{
    // do not unlock, it will automatically unlock in MutexIX::freeResource()
}

bool ResourceLock::_osLock(unsigned int timeout)
{
    return reinterpret_cast<MutexIX *>(mSynchObject)->lock(timeout);
}

bool ResourceLock::_osUnlock(void)
{
    reinterpret_cast<MutexIX *>(mSynchObject)->unlock( );
    return true;
}

bool ResourceLock::_osTryLock(void)
{
    return reinterpret_cast<MutexIX *>(mSynchObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer implementation
//////////////////////////////////////////////////////////////////////////

void SynchTimer::_osCreateTimer( bool /* isSteady */ )
{
    mSynchObject= static_cast<void *>(DEBUG_NEW WaitableTimerIX( mIsAutoReset, "POSIX_WaitableTimer" ));
}

void SynchTimer::_osReleaseTime( void )
{
    reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer( );
}

bool SynchTimer::_osLock( unsigned int timeout )
{
    return (SynchLockAndWaitIX::waitForSingleObject( *reinterpret_cast<IEWaitableBaseIX *>(mSynchObject), timeout ) == NESynchTypesIX::SynchObject0);
}

bool SynchTimer::_osSetTimer( void )
{
    return reinterpret_cast<WaitableTimerIX *>(mSynchObject)->setTimer( mTimeout, mIsPeriodic );
}

bool SynchTimer::_osCancelTimer( void )
{
    return reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer( );
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

int MultiLock::_osLock(unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    IEWaitableBaseIX * synchHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
    {
        synchHandles[i] = reinterpret_cast<IEWaitableBaseIX *>(mSyncObjArray[i]->getHandle( ));
    }

    int index = MultiLock::LOCK_INDEX_INVALID;
    do
    {
        int result = SynchLockAndWaitIX::waitForMultipleObjects(synchHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int>(NESynchTypesIX::SynchObjectAll):
            index = MultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::eLockedState::StateLocked;
            }
            break;

        case static_cast<int>(NESynchTypesIX::SynchAsynchSignal):
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

        case static_cast<int>(NESynchTypesIX::SynchObjectTimeout):
            index = MultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int>(NESynchTypesIX::SynchObject0)) && (result < mSizeCount) )
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
