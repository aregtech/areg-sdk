/************************************************************************
 * \file        areg/src/base/private/posix/ESynchObjectsWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects. POSIX specific methods
 *
 ************************************************************************/
#include "areg/src/base/ESynchObjects.hpp"

#ifdef  _POSIX
#include "areg/src/base/CEThread.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/private/posix/CEWaitableEventIX.hpp"
#include "areg/src/base/private/posix/CEWaitableMutexIX.hpp"
#include "areg/src/base/private/posix/CEWaitableSemaphoreIX.hpp"
#include "areg/src/base/private/posix/CECriticalSectionIX.hpp"
#include "areg/src/base/private/posix/CEWaitableTimerIX.hpp"
#include "areg/src/base/private/posix/CESynchLockAndWaitIX.hpp"

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
        synchObject->FreeResources();
        delete synchObject;
    }
}

//////////////////////////////////////////////////////////////////////////
// CEMutex class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMutex class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEMutex::CEMutex(bool lock /* = true */)
    : IEBlockingSynchObject ( IESynchObject::SO_MUTEX )

    , mOwnerThreadId        ( 0 )
{
    mSynchObject    = new CEWaitableMutexIX(lock, "POSIX_Mutex");
}

CEMutex::~CEMutex( void )
{
    Unlock();
}

//////////////////////////////////////////////////////////////////////////
// CEMutex class, Methods
//////////////////////////////////////////////////////////////////////////
void CEMutex::_setOwnership( void )
{
    CEWaitableMutexIX * synchMutex = static_cast<CEWaitableMutexIX *>(mSynchObject);
    ASSERT(synchMutex != NULL);

    synchMutex->LockObject();
    mOwnerThreadId = reinterpret_cast<ITEM_ID>(synchMutex->GetOwningThreadId());
    synchMutex->UnlockObject();
}

void CEMutex::_releaseOwnership( void )
{
    CEWaitableMutexIX * synchMutex = static_cast<CEWaitableMutexIX *>(mSynchObject);
    ASSERT(synchMutex != NULL);

    synchMutex->LockObject();
    mOwnerThreadId = reinterpret_cast<ITEM_ID>(synchMutex->GetOwningThreadId());
    synchMutex->UnlockObject();
}

bool CEMutex::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    bool result = false;

    CEWaitableMutexIX * synchMutex = static_cast<CEWaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != NULL) && (NESynchTypesIX::SynchObject0 == CESynchLockAndWaitIX::WaitForSingleObject(*synchMutex, timeout)) )
    {
        _setOwnership();
        result = true;
    }

    return result;
}

bool CEMutex::Unlock( void )
{
    ASSERT(mSynchObject != NULL);
    bool result = false;

    CEWaitableMutexIX * synchMutex = static_cast<CEWaitableMutexIX *>(mSynchObject);
    if ( (synchMutex != NULL) && synchMutex->ReleaseMutex() )
    {
        _releaseOwnership();
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// CESynchEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESynchEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CESynchEvent::CESynchEvent(bool lock /* = true */, bool autoReset /* = true */)
    : IESynchObject (IESynchObject::SO_EVENT)
    , mAutoReset    (autoReset)
{
    mSynchObject    = static_cast<void *>( new CEWaitableEventIX(lock == false, autoReset, "POSIX_Event") );
}

CESynchEvent::~CESynchEvent( void )
{
    Unlock();
}

//////////////////////////////////////////////////////////////////////////
// CESynchEvent class, Methods
//////////////////////////////////////////////////////////////////////////
bool CESynchEvent::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    CEWaitableEventIX * synchEvent = reinterpret_cast<CEWaitableEventIX *>(mSynchObject);
    return (NESynchTypesIX::SynchObject0 == CESynchLockAndWaitIX::WaitForSingleObject(*synchEvent, timeout));
}

bool CESynchEvent::Unlock( void )
{
    ASSERT(mSynchObject != NULL);
    CEWaitableEventIX * synchEvent = reinterpret_cast<CEWaitableEventIX *>(mSynchObject);
    return synchEvent->SetEvent();
}

bool CESynchEvent::SetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    CEWaitableEventIX * synchEvent = reinterpret_cast<CEWaitableEventIX *>(mSynchObject);
    return synchEvent->SetEvent();
}

bool CESynchEvent::ResetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    CEWaitableEventIX * synchEvent = reinterpret_cast<CEWaitableEventIX *>(mSynchObject);
    return synchEvent->ResetEvent();
}

void CESynchEvent::PulseEvent( void )
{
    ASSERT(mSynchObject != NULL);
    CEWaitableEventIX * synchEvent = reinterpret_cast<CEWaitableEventIX *>(mSynchObject);
    synchEvent->PulseEvent();
}

//////////////////////////////////////////////////////////////////////////
// CESemaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESemaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CESemaphore::CESemaphore(int maxCount, int initCount /* = 0 */)
    : IEBlockingSynchObject(IESynchObject::SO_SEMAPHORE)

    , mMaxCount             ( MACRO_MAX(maxCount, 1) )
    , mCurrCount            ( MACRO_IN_RANGE(initCount, 0, mMaxCount) ? initCount : 0 )
{
    mSynchObject = new CEWaitableSemaphoreIX(mMaxCount, initCount, "POSIX_Semaphore");
}

CESemaphore::~CESemaphore( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESemaphore class, Methods
//////////////////////////////////////////////////////////////////////////
bool CESemaphore::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    CEWaitableSemaphoreIX * synchSemaphore = static_cast<CEWaitableSemaphoreIX *>(mSynchObject);
    return ( (synchSemaphore != NULL) && (NESynchTypesIX::SynchObject0 == CESynchLockAndWaitIX::WaitForSingleObject(*synchSemaphore, timeout)) );
}

bool CESemaphore::Unlock( void )
{
    ASSERT(mSynchObject != NULL);
    CEWaitableSemaphoreIX * synchSemaphore = static_cast<CEWaitableSemaphoreIX *>(mSynchObject);
    return ((synchSemaphore != NULL) && synchSemaphore->ReleaseSemaphore());
}

//////////////////////////////////////////////////////////////////////////
// CESynchTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESynchTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CESynchTimer::CESynchTimer(unsigned int timeMilliseconds, bool periodic /* = false */, bool autoReset /* = true */, bool initSignaled /* = true */)
    : IESynchObject   (IESynchObject::SO_TIMER)

    , mTimeMilliseconds (timeMilliseconds)
    , mIsPeriodic       (periodic)
    , mIsAutoReset      (autoReset)
{
    mSynchObject= static_cast<void *>(DEBUG_NEW CEWaitableTimerIX(mIsAutoReset, initSignaled, "POSIX_WaitableTimer"));
}

CESynchTimer::~CESynchTimer( void )
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<CEWaitableTimerIX *>(mSynchObject)->ReleaseTimer();
    }
}

//////////////////////////////////////////////////////////////////////////
// CESynchTimer class, Methods
//////////////////////////////////////////////////////////////////////////
bool CESynchTimer::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    return ( (mSynchObject != NULL) && CESynchLockAndWaitIX::WaitForSingleObject( *reinterpret_cast<IEWaitableBaseIX *>(mSynchObject), timeout) == NESynchTypesIX::SynchObject0);
}

bool CESynchTimer::Unlock( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<CEWaitableTimerIX *>(mSynchObject)->ReleaseTimer());
}

bool CESynchTimer::SetTimer( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<CEWaitableTimerIX *>(mSynchObject)->SetTimer(mTimeMilliseconds, mIsPeriodic));
}

bool CESynchTimer::CancelTimer( void )
{
    return ((mSynchObject != NULL) && reinterpret_cast<CEWaitableTimerIX *>(mSynchObject)->ReleaseTimer());
}

//////////////////////////////////////////////////////////////////////////
// CECriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CECriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CECriticalSection::CECriticalSection( void )
    : IEBlockingSynchObject(IESynchObject::SO_CRITICAL)
{
    ASSERT(mSynchObject == NULL);
    mSynchObject = static_cast<void *>( DEBUG_NEW CECriticalSectionIX(false) );
}

CECriticalSection::~CECriticalSection( void )
{
    if (mSynchObject != NULL)
    {
        delete reinterpret_cast<CECriticalSectionIX *>(mSynchObject);
    }

    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CECriticalSection class, Methods
//////////////////////////////////////////////////////////////////////////

bool CECriticalSection::Lock(unsigned int  /*timeout = IESynchObject::WAIT_INFINITE */)
{
    return (mSynchObject != NULL ? reinterpret_cast<CECriticalSectionIX *>(mSynchObject)->Lock() : false);
}

bool CECriticalSection::Unlock( void )
{
    if (mSynchObject != NULL)
        reinterpret_cast<CECriticalSectionIX *>(mSynchObject)->Unlock();

    return (mSynchObject != NULL);
}

bool CECriticalSection::TryLock( void )
{
    return (mSynchObject != NULL ? reinterpret_cast<CECriticalSectionIX *>(mSynchObject)->TryLock() : false);
}

//////////////////////////////////////////////////////////////////////////
// CEResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

CEResourceLock::CEResourceLock( bool initLock /*= false*/ )
    : IEResourceLock(IESynchObject::SO_RES_LOCK)
{
    mSynchObject    = new CEMutexIX(initLock, "ResourceLock");
}

CEResourceLock::~CEResourceLock(void)
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<CEMutexIX *>(mSynchObject)->UnlockObject();
        delete reinterpret_cast<CEMutexIX *>(mSynchObject);
        mSynchObject = NULL;
    }
}

bool CEResourceLock::Lock(unsigned int timeout /*= IESynchObject::WAIT_INFINITE */)
{
    return( mSynchObject != NULL ? reinterpret_cast<CEMutexIX *>(mSynchObject)->LockObject(timeout) : true);
}

bool CEResourceLock::Unlock(void)
{
    if ( mSynchObject != NULL )
    {
        reinterpret_cast<CEMutexIX *>(mSynchObject)->UnlockObject();
    }

    return true;
}

bool CEResourceLock::TryLock(void)
{
    return (mSynchObject != NULL ? reinterpret_cast<CEMutexIX *>(mSynchObject)->TryLockObject() : true);
}

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class, Methods
//////////////////////////////////////////////////////////////////////////
int CEMultiLock::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    IEWaitableBaseIX * synchHandles[CEMultiLock::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i )
        synchHandles[i] = reinterpret_cast<IEWaitableBaseIX *>( mSyncObjArray[i]->GetHandle() );

    int index = CEMultiLock::LOCK_INDEX_INVALID;
    do
    {
        int result = CESynchLockAndWaitIX::WaitForMultipleObjects(synchHandles, mSizeCount, waitForAll, timeout);

        switch (result)
        {
        case static_cast<int>(NESynchTypesIX::SynchObjectAll):
            index = CEMultiLock::LOCK_INDEX_ALL;
            for ( int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = CEMultiLock::STATE_LOCKED;
            }
            break;

        case static_cast<int>(NESynchTypesIX::SynchAsynchSignal):
                if (isAlertable)
                {
                    index = CEMultiLock::LOCK_INDEX_COMPLETION;
                }
                else
                {
                    index = CEMultiLock::LOCK_INDEX_INVALID;
                    continue;
                }
            break;

        case static_cast<int>(NESynchTypesIX::SynchObjectTimeout):
            index = CEMultiLock::LOCK_INDEX_TIMEOUT;
            break;

        default:
            if ( (result >= static_cast<int>(NESynchTypesIX::SynchObject0)) && (result < mSizeCount) )
            {
                index = result;
                mLockedStates[result] = CEMultiLock::STATE_LOCKED;
            }
            break;
        }

    } while(false);

    return index;
}

//////////////////////////////////////////////////////////////////////////
// CEInterlockedValue class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CEInterlockedValue class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEInterlockedValue::CEInterlockedValue( unsigned int *varPtr /*= NULL*/ )
    : mValue    (0)
    , mData     ( reinterpret_cast<int *>(varPtr == NULL ? &mValue : varPtr)   )
    , mContext  ( reinterpret_cast<void *>(DEBUG_NEW CECriticalSectionIX(false)))
{
    ; // do nothing
}

CEInterlockedValue::CEInterlockedValue( unsigned int initValue )
    : mValue    ( initValue )
    , mData     ( reinterpret_cast<int *>(&mValue) )
    , mContext  ( reinterpret_cast<void *>(DEBUG_NEW CECriticalSectionIX(false)))
{
    ; // do nothing
}

CEInterlockedValue::~CEInterlockedValue( void )
{
    if ( mContext != NULL )
    {
        delete reinterpret_cast<CECriticalSectionIX *>(mContext);
    }

    mContext = static_cast<void *>(NULL);
}

//////////////////////////////////////////////////////////////////////////
// CEInterlockedValue class, Methods
//////////////////////////////////////////////////////////////////////////
unsigned int CEInterlockedValue::Increment( void )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    return ++ (*mData);
}

unsigned int CEInterlockedValue::Decrement( void )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    return -- (*mData);
}

bool CEInterlockedValue::TestDecrement( void )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    return (-- (*mData) != 0);
}

bool CEInterlockedValue::TestDecrementZero(void)
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    return ((*mData) != 0 ? -- (*mData) == 0 : false);
}

unsigned int CEInterlockedValue::Add( unsigned int increment )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    (*mData) += increment;
    return (*mData);
}

unsigned int CEInterlockedValue::Subtract( unsigned int decrement )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    (*mData) -= decrement;
    return (*mData);
}

void CEInterlockedValue::GetData( unsigned int& data ) const
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    data = *mData;
}

unsigned int CEInterlockedValue::GetData( void ) const
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    return (*mData);
}

void CEInterlockedValue::SetData( unsigned int data )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));
    (*mData) = data;
}

unsigned int CEInterlockedValue::SetBits( unsigned int mask )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) |= mask;
    
    return result;
}

unsigned int CEInterlockedValue::RemoveBits( unsigned int mask )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) &= ~mask;

    return result;
}

unsigned int CEInterlockedValue::ToggleBits( unsigned int mask )
{
    CESpinLockIX lock(*reinterpret_cast<CECriticalSectionIX *>(mContext));

    unsigned int result = (*mData);
    (*mData) ^= mask;

    return result;
}

#endif  // _POSIX
