/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/SynchObjects.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/base/SynchObjects.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// IEResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

IEResourceLock::IEResourceLock( IESynchObject::eSyncObject synchObjectType )
    : IESynchObject   (synchObjectType)
{
    ASSERT( synchObjectType == IESynchObject::eSyncObject::SoMutex      ||
            synchObjectType == IESynchObject::eSyncObject::SoSemaphore  ||
            synchObjectType == IESynchObject::eSyncObject::SoCritical   ||
            synchObjectType == IESynchObject::eSyncObject::SoSpinlock   ||
            synchObjectType == IESynchObject::eSyncObject::SoReslock    ||
            synchObjectType == IESynchObject::eSyncObject::SoNolock     );
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
    : IEResourceLock( IESynchObject::eSyncObject::SoMutex )
    , mOwnerThreadId( 0 )
{
    _osCreateMutex( initLock );
}

Mutex::~Mutex( void )
{
    _osUnlockMutex( );
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SynchEvent::SynchEvent( bool initLock /* = true */, bool autoReset /* = true */ )
    : IESynchObject( IESynchObject::eSyncObject::SoEvent )

    , mAutoReset( autoReset )
{
    _osCreateEvent( initLock );
}

SynchEvent::~SynchEvent( void )
{
    ASSERT( mSynchObject != nullptr );
    _osUnlockEvent( mSynchObject );
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore( int maxCount, int initCount /* = 0 */ )
    : IEResourceLock( IESynchObject::eSyncObject::SoSemaphore )

    , mMaxCount( MACRO_MAX( maxCount, 1 ) )
    , mCurrCount( MACRO_IN_RANGE( initCount, 0, mMaxCount ) ? initCount : 0 )
{
    _osCreateSemaphore( );
}

Semaphore::~Semaphore( void )
{
    ASSERT( mSynchObject != nullptr );
    _osReleaseSemaphore( );
}

bool Semaphore::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
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
    ASSERT( mSynchObject != nullptr );
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
    : IEResourceLock( IESynchObject::eSyncObject::SoCritical )
{
    _osCreateCriticalSection( );
}

CriticalSection::~CriticalSection( void )
{
    ASSERT( mSynchObject != nullptr );
    _osReleaseCriticalSection( );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock( void )
    : IEResourceLock( IESynchObject::eSyncObject::SoSpinlock )
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
    : IEResourceLock( IESynchObject::eSyncObject::SoReslock )
{
    _osCreateResourceLock( initLock );
}

ResourceLock::~ResourceLock( void )
{
    ASSERT( mSynchObject != nullptr );
    _osReleaseResourceLock( );
}

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class implementation
//////////////////////////////////////////////////////////////////////////

NolockSynchObject::NolockSynchObject( void )
    : IEResourceLock( IESynchObject::eSyncObject::SoNolock )
{
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SynchTimer::SynchTimer( unsigned int msTimeout, bool isPeriodic /* = false */, bool isAutoReset /* = true */, bool isSteady /* = true */ )
    : IESynchObject ( IESynchObject::eSyncObject::SoTimer )

    , mTimeout      ( msTimeout )
    , mIsPeriodic   ( isPeriodic )
    , mIsAutoReset  ( isAutoReset )
{
    _osCreateTimer( isSteady );
}

SynchTimer::~SynchTimer( void )
{
    ASSERT( mSynchObject != nullptr );
    _osReleaseTime( );
    mSynchObject = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Lock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Lock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Lock::Lock(IESynchObject &syncObj, bool autoLock /* = true */)
    : mSynchObject  (syncObj)
    , mAutoLock     (autoLock)
{
    if (mAutoLock && mSynchObject.isValid() )
    {
        mSynchObject.lock();
    }
}

Lock::~Lock( void )
{
    if (mAutoLock && mSynchObject.isValid())
    {
        mSynchObject.unlock();
    }
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
MultiLock::MultiLock(IESynchObject* pObjects[], int count, bool autoLock /* = true */)
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
    ASSERT( mTimer != nullptr );
    _osReleaseTimer();
}
