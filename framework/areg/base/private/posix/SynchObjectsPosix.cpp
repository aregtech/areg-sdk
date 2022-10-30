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

void IESynchObject::_destroySynchObject( void )
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

//////////////////////////////////////////////////////////////////////////
// Mutex class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Mutex::Mutex(bool lock /* = true */)
    : IEResourceLock( IESynchObject::eSyncObject::SoMutex )

    , mOwnerThreadId        ( 0 )
{
    mSynchObject    = DEBUG_NEW WaitableMutexIX(lock, "POSIX_Mutex");
}

//////////////////////////////////////////////////////////////////////////
// Mutex class, Methods
//////////////////////////////////////////////////////////////////////////

bool Mutex::_lockMutex( unsigned int timeout )
{
    bool result = false;

    WaitableMutexIX * synchMutex = reinterpret_cast<WaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != nullptr) && (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchMutex, timeout)) )
    {
        mOwnerThreadId.store(reinterpret_cast<id_type>(synchMutex->getOwningThreadId()));
        result = true;
    }

    return result;
}

bool Mutex::_unlockMutex( void )
{
    bool result = false;
    WaitableMutexIX * synchMutex = reinterpret_cast<WaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != nullptr) && synchMutex->releaseMutex() )
    {
        mOwnerThreadId.store(0);
        result = true;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SynchEvent::SynchEvent(bool lock /* = true */, bool autoReset /* = true */)
    : IESynchObject (IESynchObject::eSyncObject::SoEvent )
    , mAutoReset    (autoReset)
{
    mSynchObject    = static_cast<void *>( DEBUG_NEW WaitableEventIX(lock == false, autoReset, "POSIX_Event") );
}

SynchEvent::~SynchEvent( void )
{
    _unlockEvent(mSynchObject);
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Methods
//////////////////////////////////////////////////////////////////////////

bool SynchEvent::_unlockEvent( void * eventHandle )
{
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(eventHandle);
    return synchEvent->setEvent();
}

bool SynchEvent::lock(unsigned int timeout /* = NECommon::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != nullptr);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchEvent, timeout));
}

bool SynchEvent::setEvent( void )
{
    ASSERT(mSynchObject != nullptr);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return synchEvent->setEvent();
}

bool SynchEvent::resetEvent( void )
{
    ASSERT(mSynchObject != nullptr);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return synchEvent->resetEvent();
}

void SynchEvent::pulseEvent( void )
{
    ASSERT(mSynchObject != nullptr);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    synchEvent->pulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore(int maxCount, int initCount /* = 0 */)
    : IEResourceLock(IESynchObject::eSyncObject::SoSemaphore)

    , mMaxCount     ( MACRO_MAX(maxCount, 1) )
    , mCurrCount    ( MACRO_IN_RANGE(initCount, 0, mMaxCount) ? initCount : 0 )
{
    mSynchObject = DEBUG_NEW WaitableSemaphoreIX(mMaxCount, mCurrCount.load(), "POSIX_Semaphore");
}

Semaphore::~Semaphore( void )
{
    if ( mSynchObject != nullptr )
    {
        static_cast<WaitableSemaphoreIX *>(mSynchObject)->releaseSemaphore();
    }
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Methods
//////////////////////////////////////////////////////////////////////////
bool Semaphore::lock(unsigned int timeout /* = NECommon::WAIT_INFINITE */)
{
    bool result = false;
    WaitableSemaphoreIX * synchSemaphore = static_cast<WaitableSemaphoreIX *>(mSynchObject);
    if ( (synchSemaphore != nullptr) && (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchSemaphore, timeout)) )
    {
        mCurrCount.fetch_sub( 1 );
        result = true;
    }

    return result;
}

bool Semaphore::unlock( void )
{
    bool result = false;
    WaitableSemaphoreIX * synchSemaphore = static_cast<WaitableSemaphoreIX *>(mSynchObject);
    if ((synchSemaphore != nullptr) && synchSemaphore->releaseSemaphore())
    {
        mCurrCount.fetch_add( 1 );
        result = true;
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection( void )
    : IEResourceLock(IESynchObject::eSyncObject::SoCritical)
{
    ASSERT(mSynchObject == nullptr );
    mSynchObject = static_cast<void *>( DEBUG_NEW CriticalSectionIX(false) );
}

CriticalSection::~CriticalSection( void )
{
    // do not unlock, it will automatically unlock in CriticalSectionIX::freeResource()
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Methods
//////////////////////////////////////////////////////////////////////////

bool CriticalSection::lock(unsigned int  /*timeout = NECommon::WAIT_INFINITE */)
{
    return (mSynchObject != nullptr) && reinterpret_cast<CriticalSectionIX *>(mSynchObject)->lock();
}

bool CriticalSection::unlock( void )
{
    bool result = false;
    if (mSynchObject != nullptr)
    {
        reinterpret_cast<CriticalSectionIX *>(mSynchObject)->unlock( );
        result = true;
    }

    return result;
}

bool CriticalSection::tryLock( void )
{
    return (mSynchObject != nullptr) && reinterpret_cast<CriticalSectionIX *>(mSynchObject)->tryLock();
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
    if ( mSynchObject != nullptr )
    {
        delete reinterpret_cast<SpinLockIX *>(mSynchObject);
        mSynchObject = nullptr;
    }
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockIX *>(mSynchObject)->lock() : false);
}

bool SpinLock::unlock( void )
{
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockIX *>(mSynchObject)->unlock( ) : false);
}

bool SpinLock::tryLock( void )
{
    return (mSynchObject != nullptr ? reinterpret_cast<SpinLockIX *>(mSynchObject)->tryLock( ) : false);
}
#endif //0

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

ResourceLock::ResourceLock( bool initLock /*= false*/ )
    : IEResourceLock(IESynchObject::eSyncObject::SoReslock)
{
    mSynchObject    = DEBUG_NEW MutexIX(initLock, "ResourceLock");
}

ResourceLock::~ResourceLock(void)
{
    // do not unlock, it will automatically unlock in MutexIX::freeResource()
}

bool ResourceLock::lock(unsigned int timeout /*= NECommon::WAIT_INFINITE */)
{
    return( (mSynchObject == nullptr) || reinterpret_cast<MutexIX *>(mSynchObject)->lock(timeout) );
}

bool ResourceLock::unlock(void)
{
    if ( mSynchObject != nullptr )
    {
        reinterpret_cast<MutexIX *>(mSynchObject)->unlock();
    }

    return true;
}

bool ResourceLock::tryLock(void)
{
    return (mSynchObject == nullptr) || reinterpret_cast<MutexIX *>(mSynchObject)->tryLock();
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SynchTimer::SynchTimer( unsigned int msTimeout, bool isPeriodic /* = false */, bool isAutoReset /* = true */, bool /*isSteady*/ /* = true */ )
    : IESynchObject ( IESynchObject::eSyncObject::SoTimer )

    , mTimeout 		( msTimeout )
    , mIsPeriodic   ( isPeriodic )
    , mIsAutoReset  ( isAutoReset )
{
    mSynchObject= static_cast<void *>(DEBUG_NEW WaitableTimerIX( mIsAutoReset, "POSIX_WaitableTimer" ));
}

SynchTimer::~SynchTimer( void )
{
    if ( mSynchObject != nullptr )
    {
        reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer( );
    }
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Methods
//////////////////////////////////////////////////////////////////////////
bool SynchTimer::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
{
    return ((mSynchObject != nullptr) && SynchLockAndWaitIX::waitForSingleObject( *reinterpret_cast<IEWaitableBaseIX *>(mSynchObject), timeout ) == NESynchTypesIX::SynchObject0);
}

bool SynchTimer::unlock( void )
{
    return ((mSynchObject != nullptr) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer( ));
}

bool SynchTimer::setTimer( void )
{
    return ((mSynchObject != nullptr) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->setTimer( mTimeout, mIsPeriodic ));
}

bool SynchTimer::cancelTimer( void )
{
    return ((mSynchObject != nullptr) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer( ));
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Methods
//////////////////////////////////////////////////////////////////////////
int MultiLock::lock(unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
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

//////////////////////////////////////////////////////////////////////////
// Wait class, Methods
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
