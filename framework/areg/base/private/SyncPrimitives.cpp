/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SyncPrimitives.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/Thread.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// Lockable class implementation
//////////////////////////////////////////////////////////////////////////

Lockable::Lockable( SyncObject::SyncKind syncObjectType )
    : SyncObject   (syncObjectType)
{
    ASSERT( syncObjectType == SyncObject::SyncKind::SoMutex      ||
            syncObjectType == SyncObject::SyncKind::SoSemaphore  ||
            syncObjectType == SyncObject::SyncKind::SoCritical   ||
            syncObjectType == SyncObject::SyncKind::SoSpinlock   ||
            syncObjectType == SyncObject::SyncKind::SoReslock    ||
            syncObjectType == SyncObject::SyncKind::SoNolock     );
}

bool Lockable::tryLock()
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Mutex::Mutex( bool initLock /* = true */ )
    : Lockable( SyncObject::SyncKind::SoMutex )
    , mOwnerThreadId( 0 )
{
    _osCreateMutex( initLock );
}

Mutex::~Mutex()
{
    _osUnlockMutex( );
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SyncEvent::SyncEvent( bool initLock /* = true */, bool autoReset /* = true */ )
    : SyncObject( SyncObject::SyncKind::SoEvent )

    , mAutoReset( autoReset )
{
    _osCreateEvent( initLock );
}

SyncEvent::~SyncEvent()
{
    ASSERT( mSyncObject != nullptr );
    _osUnlockEvent( mSyncObject );
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore( int32_t maxCount, int32_t initCount /* = 0 */ )
    : Lockable( SyncObject::SyncKind::SoSemaphore )

    , mMaxCount( std::max( maxCount, 1 ) )
    , mCurrCount( NEMath::isInRange<int32_t>(initCount, 0, mMaxCount) ? initCount : 0 )
{
    _osCreateSemaphore( );
}

Semaphore::~Semaphore()
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseSemaphore( );
}

bool Semaphore::lock( uint32_t timeout /* = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    bool result = false;
    if ( _osLock( timeout ) )
    {
        mCurrCount.fetch_add( 1 );
        result = true;
    }

    return result;
}

bool Semaphore::unlock()
{
    ASSERT( mSyncObject != nullptr );
    bool result = false;
    if ( _osUnlock() )
    {
        mCurrCount.fetch_add( 1 );
        result = true;
    }

    return result;
}

bool Semaphore::tryLock()
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection()
    : Lockable( SyncObject::SyncKind::SoCritical )
{
    _osCreateCriticalSection( );
}

CriticalSection::~CriticalSection()
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseCriticalSection( );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock()
    : Lockable( SyncObject::SyncKind::SoSpinlock )
    , mLock         ( false )
{
}

bool SpinLock::lock( uint32_t /*timeout = NECommon::WAIT_INFINITE*/ )
{
    for ( ; ; )
    {
        if ( mLock.exchange( true, std::memory_order_acquire ) == false )
            break;

        while ( mLock.load( std::memory_order_relaxed ) )
            Thread::sleep( 0 );
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

ResourceLock::ResourceLock( bool initLock /*= false*/ )
    : Lockable( SyncObject::SyncKind::SoReslock )
{
    _osCreateResourceLock( initLock );
}

ResourceLock::~ResourceLock()
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseResourceLock( );
}

//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class implementation
//////////////////////////////////////////////////////////////////////////

NolockSyncObject::NolockSyncObject()
    : Lockable( SyncObject::SyncKind::SoNolock )
{
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SyncTimer::SyncTimer( uint32_t msTimeout, bool isPeriodic /* = false */, bool isAutoReset /* = true */, bool isSteady /* = true */ )
    : SyncObject  ( SyncObject::SyncKind::SoTimer )

    , mTimeout      ( msTimeout )
    , mIsPeriodic   ( isPeriodic )
    , mIsAutoReset  ( isAutoReset )
{
    _osCreateTimer( isSteady );
}

SyncTimer::~SyncTimer()
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseTime( );
    mSyncObject = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Lock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Lock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Lock::Lock(SyncObject &syncObj, bool autoLock /* = true */)
    : mSyncObject(syncObj)
    , mAutoLock  (autoLock)
{
    if (mAutoLock && mSyncObject.isValid() )
    {
        mSyncObject.lock();
    }
}

Lock::~Lock()
{
    if (mAutoLock && mSyncObject.isValid())
    {
        mSyncObject.unlock();
    }
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
MultiLock::MultiLock(SyncObject* pObjects[], int32_t count, bool autoLock /* = true */)
    : mSyncObjArray (pObjects)
    , mSizeCount    (std::min(count, NECommon::MAXIMUM_WAITING_OBJECTS))
    , mAutoLock     (autoLock)
{
    NEMemory::memZero(static_cast<void *>(mLockedStates), NECommon::MAXIMUM_WAITING_OBJECTS * sizeof(LockState)  );
    if (autoLock)
    {
        lock(NECommon::WAIT_INFINITE, true);
    }
}

MultiLock::~MultiLock()
{
    if (mAutoLock)
    {
        unlock();
    }
}

bool MultiLock::unlock()
{
    for (int i = 0; i < mSizeCount; ++ i)
    {
        if (mLockedStates[i] == MultiLock::LockState::Locked)
        {
            mSyncObjArray[i]->unlock( );
        }
    }

    return true;
}

bool MultiLock::unlock( int32_t index )
{
    bool result = false;
    if ( (index >= 0) && (index < mSizeCount) )
    {
        if (mLockedStates[index] == MultiLock::LockState::Locked)
        {
            result = mSyncObjArray[index]->unlock( );
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Wait class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Wait::Wait()
    : mTimer(nullptr)
{
    _osInitTimer();
}

Wait::~Wait()
{
    _osReleaseTimer();
}
