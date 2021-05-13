/************************************************************************
 * \file        areg/base/private/posix/ESynchObjectsWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects. POSIX specific methods
 *
 ************************************************************************/
#include "areg/base/ESynchObjects.hpp"

#ifdef  _POSIX
#include "areg/base/Thread.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/private/posix/WaitableEventIX.hpp"
#include "areg/base/private/posix/WaitableMutexIX.hpp"
#include "areg/base/private/posix/WaitableSemaphoreIX.hpp"
#include "areg/base/private/posix/CriticalSectionIX.hpp"
#include "areg/base/private/posix/WaitableTimerIX.hpp"
#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

#include <string.h>

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
    if (mSynchObject != NULL)
    {
        IESynchObjectBaseIX * synchObject = reinterpret_cast<IESynchObjectBaseIX *>(mSynchObject);
        mSynchObject = NULL;
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
    : IEBlockingSynchObject ( IESynchObject::SO_MUTEX )

    , mOwnerThreadId        ( 0 )
{
    mSynchObject    = DEBUG_NEW WaitableMutexIX(lock, "POSIX_Mutex");
}

Mutex::~Mutex( void )
{
    unlock();
}

//////////////////////////////////////////////////////////////////////////
// Mutex class, Methods
//////////////////////////////////////////////////////////////////////////
void Mutex::_setOwnership( void )
{
    WaitableMutexIX * synchMutex = static_cast<WaitableMutexIX *>(mSynchObject);
    ASSERT(synchMutex != NULL);

    synchMutex->lock();
    mOwnerThreadId = reinterpret_cast<ITEM_ID>(synchMutex->getOwningThreadId());
    synchMutex->unlock();
}

void Mutex::_releaseOwnership( void )
{
    WaitableMutexIX * synchMutex = static_cast<WaitableMutexIX *>(mSynchObject);
    ASSERT(synchMutex != NULL);

    synchMutex->lock();
    mOwnerThreadId = reinterpret_cast<ITEM_ID>(synchMutex->getOwningThreadId());
    synchMutex->unlock();
}

bool Mutex::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    bool result = false;

    WaitableMutexIX * synchMutex = static_cast<WaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != NULL) && (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchMutex, timeout)) )
    {
        _setOwnership();
        result = true;
    }

    return result;
}

bool Mutex::unlock( void )
{
    ASSERT(mSynchObject != NULL);
    bool result = false;

    WaitableMutexIX * synchMutex = static_cast<WaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != NULL) && synchMutex->releaseMutex() )
    {
        _releaseOwnership();
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
    : IESynchObject (IESynchObject::SO_EVENT)
    , mAutoReset    (autoReset)
{
    mSynchObject    = static_cast<void *>( DEBUG_NEW WaitableEventIX(lock == false, autoReset, "POSIX_Event") );
}

SynchEvent::~SynchEvent( void )
{
    unlock();
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Methods
//////////////////////////////////////////////////////////////////////////
bool SynchEvent::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchEvent, timeout));
}

bool SynchEvent::unlock( void )
{
    ASSERT(mSynchObject != NULL);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return synchEvent->setEvent();
}

bool SynchEvent::setEvent( void )
{
    ASSERT(mSynchObject != NULL);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return synchEvent->setEvent();
}

bool SynchEvent::resetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    WaitableEventIX * synchEvent = reinterpret_cast<WaitableEventIX *>(mSynchObject);
    return synchEvent->resetEvent();
}

void SynchEvent::pulseEvent( void )
{
    ASSERT(mSynchObject != NULL);
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
    : IEBlockingSynchObject(IESynchObject::SO_SEMAPHORE)

    , mMaxCount             ( MACRO_MAX(maxCount, 1) )
    , mCurrCount            ( MACRO_IN_RANGE(initCount, 0, mMaxCount) ? initCount : 0 )
{
    mSynchObject = DEBUG_NEW WaitableSemaphoreIX(mMaxCount, initCount, "POSIX_Semaphore");
}

Semaphore::~Semaphore( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Methods
//////////////////////////////////////////////////////////////////////////
bool Semaphore::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    WaitableSemaphoreIX * synchSemaphore = static_cast<WaitableSemaphoreIX *>(mSynchObject);
    return ( (synchSemaphore != NULL) && (NESynchTypesIX::SynchObject0 == SynchLockAndWaitIX::waitForSingleObject(*synchSemaphore, timeout)) );
}

bool Semaphore::unlock( void )
{
    ASSERT(mSynchObject != NULL);
    WaitableSemaphoreIX * synchSemaphore = static_cast<WaitableSemaphoreIX *>(mSynchObject);
    return ((synchSemaphore != NULL) && synchSemaphore->releaseSemaphore());
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SynchTimer::SynchTimer(unsigned int timeMilliseconds, bool periodic /* = false */, bool autoReset /* = true */, bool initSignaled /* = true */)
    : IESynchObject   (IESynchObject::SO_TIMER)

    , mTimeMilliseconds (timeMilliseconds)
    , mIsPeriodic       (periodic)
    , mIsAutoReset      (autoReset)
{
    mSynchObject= static_cast<void *>(DEBUG_NEW WaitableTimerIX(mIsAutoReset, initSignaled, "POSIX_WaitableTimer"));
}

SynchTimer::~SynchTimer( void )
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer();
    }
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Methods
//////////////////////////////////////////////////////////////////////////
bool SynchTimer::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    return ( (mSynchObject != NULL) && SynchLockAndWaitIX::waitForSingleObject( *reinterpret_cast<IEWaitableBaseIX *>(mSynchObject), timeout) == NESynchTypesIX::SynchObject0);
}

bool SynchTimer::unlock( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer());
}

bool SynchTimer::setTimer( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->setTimer(mTimeMilliseconds, mIsPeriodic));
}

bool SynchTimer::cancelTimer( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<WaitableTimerIX *>(mSynchObject)->cancelTimer());
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection( void )
    : IEBlockingSynchObject(IESynchObject::SO_CRITICAL)
{
    ASSERT(mSynchObject == NULL);
    mSynchObject = static_cast<void *>( DEBUG_NEW CriticalSectionIX(false) );
}

CriticalSection::~CriticalSection( void )
{
    if (mSynchObject != NULL)
    {
        delete reinterpret_cast<CriticalSectionIX *>(mSynchObject);
    }

    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Methods
//////////////////////////////////////////////////////////////////////////

bool CriticalSection::lock(unsigned int  /*timeout = IESynchObject::WAIT_INFINITE */)
{
    return (mSynchObject != NULL ? reinterpret_cast<CriticalSectionIX *>(mSynchObject)->lock() : false);
}

bool CriticalSection::unlock( void )
{
    if (mSynchObject != NULL)
        reinterpret_cast<CriticalSectionIX *>(mSynchObject)->unlock();

    return (mSynchObject != NULL);
}

bool CriticalSection::tryLock( void )
{
    return (mSynchObject != NULL ? reinterpret_cast<CriticalSectionIX *>(mSynchObject)->tryLock() : false);
}

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

ResourceLock::ResourceLock( bool initLock /*= false*/ )
    : IEResourceLock(IESynchObject::SO_RES_LOCK)
{
    mSynchObject    = DEBUG_NEW MutexIX(initLock, "ResourceLock");
}

ResourceLock::~ResourceLock(void)
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<MutexIX *>(mSynchObject)->unlock();
        delete reinterpret_cast<MutexIX *>(mSynchObject);
        mSynchObject = NULL;
    }
}

bool ResourceLock::lock(unsigned int timeout /*= IESynchObject::WAIT_INFINITE */)
{
    return( mSynchObject != NULL ? reinterpret_cast<MutexIX *>(mSynchObject)->lock(timeout) : true);
}

bool ResourceLock::unlock(void)
{
    if ( mSynchObject != NULL )
    {
        reinterpret_cast<MutexIX *>(mSynchObject)->unlock();
    }

    return true;
}

bool ResourceLock::tryLock(void)
{
    return (mSynchObject != NULL ? reinterpret_cast<MutexIX *>(mSynchObject)->tryLock() : true);
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Methods
//////////////////////////////////////////////////////////////////////////
int MultiLock::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    IEWaitableBaseIX * synchHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
        synchHandles[i] = reinterpret_cast<IEWaitableBaseIX *>( mSyncObjArray[i]->getHandle() );

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
                mLockedStates[i] = MultiLock::STATE_LOCKED;
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
                mLockedStates[result] = MultiLock::STATE_LOCKED;
            }
            break;
        }

    } while(false);

    return index;
}

//////////////////////////////////////////////////////////////////////////
// InterlockedValue class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// InterlockedValue class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
InterlockedValue::InterlockedValue( unsigned int *varPtr /*= NULL*/ )
    : mValue    (0)
    , mData     ( reinterpret_cast<int *>(varPtr == NULL ? &mValue : varPtr)   )
    , mContext  ( reinterpret_cast<void *>(DEBUG_NEW CriticalSectionIX(false)))
{
    ; // do nothing
}

InterlockedValue::InterlockedValue( unsigned int initValue )
    : mValue    ( initValue )
    , mData     ( reinterpret_cast<int *>(&mValue) )
    , mContext  ( reinterpret_cast<void *>(DEBUG_NEW CriticalSectionIX(false)))
{
    ; // do nothing
}

InterlockedValue::~InterlockedValue( void )
{
    if ( mContext != NULL )
    {
        delete reinterpret_cast<CriticalSectionIX *>(mContext);
    }

    mContext = static_cast<void *>(NULL);
}

//////////////////////////////////////////////////////////////////////////
// InterlockedValue class, Methods
//////////////////////////////////////////////////////////////////////////
unsigned int InterlockedValue::increment( void )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    return ++ (*mData);
}

unsigned int InterlockedValue::decrement( void )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    return -- (*mData);
}

bool InterlockedValue::testDecrement( void )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    return (-- (*mData) != 0);
}

bool InterlockedValue::testDecrementZero(void)
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    return ((*mData) != 0 ? -- (*mData) == 0 : false);
}

unsigned int InterlockedValue::add( unsigned int increment )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    (*mData) += increment;
    return (*mData);
}

unsigned int InterlockedValue::subtract( unsigned int decrement )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    (*mData) -= decrement;
    return (*mData);
}

void InterlockedValue::getData( unsigned int& data ) const
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    data = *mData;
}

unsigned int InterlockedValue::getData( void ) const
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    return (*mData);
}

void InterlockedValue::setData( unsigned int data )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));
    (*mData) = data;
}

unsigned int InterlockedValue::setBits( unsigned int mask )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) |= mask;
    
    return result;
}

unsigned int InterlockedValue::removeBits( unsigned int mask )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) &= ~mask;

    return result;
}

unsigned int InterlockedValue::toggleBits( unsigned int mask )
{
    SpinLockIX lock(*reinterpret_cast<CriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) ^= mask;

    return result;
}

#endif  // _POSIX
