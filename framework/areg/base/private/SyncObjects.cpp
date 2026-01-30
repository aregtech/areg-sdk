/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SyncObjects.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/base/SyncObjects.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// IEResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

IEResourceLock::IEResourceLock( IESyncObject::eSyncObject syncObjectType )
    : IESyncObject   (syncObjectType)
{
    ASSERT( syncObjectType == IESyncObject::eSyncObject::SoMutex      ||
            syncObjectType == IESyncObject::eSyncObject::SoSemaphore  ||
            syncObjectType == IESyncObject::eSyncObject::SoCritical   ||
            syncObjectType == IESyncObject::eSyncObject::SoSpinlock   ||
            syncObjectType == IESyncObject::eSyncObject::SoReslock    ||
            syncObjectType == IESyncObject::eSyncObject::SoNolock     );
}

bool IEResourceLock::tryLock(void)
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
    : IEResourceLock( IESyncObject::eSyncObject::SoMutex )
    , mOwnerThreadId( 0 )
{
    _osCreateMutex( initLock );
}

Mutex::~Mutex( void )
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
    : IESyncObject( IESyncObject::eSyncObject::SoEvent )

    , mAutoReset( autoReset )
{
    _osCreateEvent( initLock );
}

SyncEvent::~SyncEvent( void )
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
Semaphore::Semaphore( int maxCount, int initCount /* = 0 */ )
    : IEResourceLock( IESyncObject::eSyncObject::SoSemaphore )

    , mMaxCount( MACRO_MAX( maxCount, 1 ) )
    , mCurrCount( MACRO_IN_RANGE( initCount, 0, mMaxCount ) ? initCount : 0 )
{
    _osCreateSemaphore( );
}

Semaphore::~Semaphore( void )
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseSemaphore( );
}

bool Semaphore::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
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

bool Semaphore::unlock( void )
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

bool Semaphore::tryLock( void )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection( void )
    : IEResourceLock( IESyncObject::eSyncObject::SoCritical )
{
    _osCreateCriticalSection( );
}

CriticalSection::~CriticalSection( void )
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseCriticalSection( );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock( void )
    : IEResourceLock( IESyncObject::eSyncObject::SoSpinlock )
    , mLock         ( false )
{
}

bool SpinLock::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
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
    : IEResourceLock( IESyncObject::eSyncObject::SoReslock )
{
    _osCreateResourceLock( initLock );
}

ResourceLock::~ResourceLock( void )
{
    ASSERT( mSyncObject != nullptr );
    _osReleaseResourceLock( );
}

//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class implementation
//////////////////////////////////////////////////////////////////////////

NolockSyncObject::NolockSyncObject( void )
    : IEResourceLock( IESyncObject::eSyncObject::SoNolock )
{
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SyncTimer::SyncTimer( unsigned int msTimeout, bool isPeriodic /* = false */, bool isAutoReset /* = true */, bool isSteady /* = true */ )
    : IESyncObject  ( IESyncObject::eSyncObject::SoTimer )

    , mTimeout      ( msTimeout )
    , mIsPeriodic   ( isPeriodic )
    , mIsAutoReset  ( isAutoReset )
{
    _osCreateTimer( isSteady );
}

SyncTimer::~SyncTimer( void )
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
Lock::Lock(IESyncObject &syncObj, bool autoLock /* = true */)
    : mSyncObject(syncObj)
    , mAutoLock  (autoLock)
{
    if (mAutoLock && mSyncObject.isValid() )
    {
        mSyncObject.lock();
    }
}

Lock::~Lock( void )
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
MultiLock::MultiLock(IESyncObject* pObjects[], int count, bool autoLock /* = true */)
    : mSyncObjArray (pObjects)
    , mSizeCount    (MACRO_MIN(count, NECommon::MAXIMUM_WAITING_OBJECTS))
    , mAutoLock     (autoLock)
{
    NEMemory::memZero(static_cast<void *>(mLockedStates), NECommon::MAXIMUM_WAITING_OBJECTS * sizeof(eLockedState)  );
    if (autoLock)
    {
        lock(NECommon::WAIT_INFINITE, true);
    }
}

MultiLock::~MultiLock( void )
{
    if (mAutoLock)
    {
        unlock();
    }
}

bool MultiLock::unlock( void )
{
    for (int i = 0; i < mSizeCount; ++ i)
    {
        if (mLockedStates[i] == MultiLock::eLockedState::StateLocked)
        {
            mSyncObjArray[i]->unlock( );
        }
    }

    return true;
}

bool MultiLock::unlock( int index )
{
    bool result = false;
    if ( (index >= 0) && (index < mSizeCount) )
    {
        if (mLockedStates[index] == MultiLock::eLockedState::StateLocked)
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
Wait::Wait(void)
    : mTimer(nullptr)
{
    _osInitTimer();
}

Wait::~Wait(void)
{
    _osReleaseTimer();
}
