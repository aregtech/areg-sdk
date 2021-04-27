/************************************************************************
 * \file        areg/base/private/win32/ESynchObjectsWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects. Windows specific methods
 *
 ************************************************************************/
#include "areg/base/ESynchObjects.hpp"

#ifdef  _WINDOWS
#include "areg/base/NEMemory.hpp"
#include "areg/base/CEThread.hpp"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// IESynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IESynchObject class methods
//////////////////////////////////////////////////////////////////////////

void IESynchObject::_destroySynchObject( void )
{
    if (mSynchObject != NULL)
        CloseHandle(static_cast<HANDLE>(mSynchObject));

    mSynchObject= NULL;
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
    mSynchObject    = static_cast<void *>(CreateMutex(NULL, lock ? TRUE : FALSE, NULL));
    if (lock && (mSynchObject != NULL))
    {
        _setOwnership();
    }
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
    InterlockedExchange((long *)&mOwnerThreadId, (long)CEThread::GetCurrentThreadId());
}

void CEMutex::_releaseOwnership( void )
{
    InterlockedExchange((long *)&mOwnerThreadId, (long)0);
}

bool CEMutex::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    bool result = false;

    if (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0)
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
    if (ReleaseMutex(static_cast<HANDLE>(mSynchObject)))
    {
        _releaseOwnership();
        result = true;
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
    : IESynchObject   (IESynchObject::SO_EVENT)

    , mAutoReset        (autoReset)
{
    mSynchObject= static_cast<void *>(CreateEvent(NULL, autoReset ? FALSE : TRUE, lock ? FALSE : TRUE, NULL));
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
    return ( WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0 );
}

bool CESynchEvent::Unlock( void )
{
    ASSERT(mSynchObject != NULL);
    return (::SetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool CESynchEvent::SetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    return (::SetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool CESynchEvent::ResetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    return (::ResetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

void CESynchEvent::PulseEvent( void )
{
    ASSERT(mSynchObject != NULL);
    ::PulseEvent(static_cast<HANDLE>(mSynchObject));
}

//////////////////////////////////////////////////////////////////////////
// CESemaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESemaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CESemaphore::CESemaphore(int maxCount, int initCount /* = 0 */)
    : IEBlockingSynchObject(IESynchObject::SO_SEMAPHORE)

    , mMaxCount             (maxCount <= 0 ? 1 : maxCount)
    , mCurrCount            (0)
{
    initCount   = initCount >= 0 && initCount <= mMaxCount ? initCount : 0;
    mSynchObject= static_cast<void *>(CreateSemaphore(NULL, initCount, mMaxCount, NULL));
    mCurrCount  = mSynchObject != NULL ? initCount : 0;
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
    bool result = false;
    if (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0)
    {
        InterlockedDecrement(&mCurrCount);
        result = true;
    }
    return result;
}

bool CESemaphore::Unlock( void )
{
    ASSERT(mSynchObject != NULL);
    bool result = false;
    if (ReleaseSemaphore(static_cast<HANDLE>(mSynchObject), 1, &mCurrCount) == TRUE)
    {
        InterlockedIncrement(&mCurrCount);
        // ASSERT(mCurrCount <= mMaxCount);
        result = true;
    }
    return result;
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
    mSynchObject= static_cast<HANDLE>(CreateWaitableTimer(NULL, autoReset ? FALSE : TRUE, NULL));
    if (initSignaled == false)
    {
        SetTimer();
    }
}

CESynchTimer::~CESynchTimer( void )
{
    if (mSynchObject != NULL)
    {
        CancelWaitableTimer(static_cast<HANDLE>(mSynchObject));
        CloseHandle(static_cast<HANDLE>(mSynchObject));
    }

    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CESynchTimer class, Methods
//////////////////////////////////////////////////////////////////////////
bool CESynchTimer::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    return (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0);
}

bool CESynchTimer::Unlock( void )
{
    return (CancelWaitableTimer(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool CESynchTimer::SetTimer( void )
{
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = static_cast<int64_t>(-1) * static_cast<int64_t>(mTimeMilliseconds) * static_cast<int64_t>(CESynchTimer::NANOSECONDS_KOEF_100);
    return (SetWaitableTimer(static_cast<HANDLE>(mSynchObject), &dueTime, mIsPeriodic ? mTimeMilliseconds : 0, NULL, NULL, FALSE) != FALSE);
}

bool CESynchTimer::CancelTimer( void )
{
    return (CancelWaitableTimer(static_cast<HANDLE>(mSynchObject)) != FALSE);
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
    mSynchObject = static_cast<void *>( DEBUG_NEW unsigned char [sizeof(CRITICAL_SECTION)] );
    if (mSynchObject != NULL)
    {
        NEMemory::ConstructElems<CRITICAL_SECTION>(mSynchObject, 1);
        InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    }
}

CECriticalSection::~CECriticalSection( void )
{
    if (mSynchObject != NULL)
    {
        DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
        delete [] reinterpret_cast<unsigned char *>(mSynchObject);
    }
    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CECriticalSection class, Methods
//////////////////////////////////////////////////////////////////////////
bool CECriticalSection::Lock(unsigned int  /*timeout = IESynchObject::WAIT_INFINITE */)
{
    if (mSynchObject != NULL)
        EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    return (mSynchObject != NULL);
}

bool CECriticalSection::Unlock( void )
{
    if (mSynchObject != NULL)
        LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    return (mSynchObject != NULL);
}

bool CECriticalSection::TryLock( void )
{
    return (mSynchObject != NULL && TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject)) ? true : false);
}

//////////////////////////////////////////////////////////////////////////
// CEResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

CEResourceLock::CEResourceLock( bool initLock /*= false*/ )
    : IEResourceLock(IESynchObject::SO_RES_LOCK)
{
    mSynchObject    = new CEMutex(initLock);
}

CEResourceLock::~CEResourceLock(void)
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->Unlock();
        delete reinterpret_cast<IEBlockingSynchObject *>(mSynchObject);
        mSynchObject = NULL;
    }
}

bool CEResourceLock::Lock(unsigned int timeout /*= IESynchObject::WAIT_INFINITE */)
{
    return( mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->Lock(timeout) : true);
}

bool CEResourceLock::Unlock(void)
{
    return ( mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->Unlock() : true);
}

bool CEResourceLock::TryLock(void)
{
    return (mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->TryLock() : true);
}

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class, Methods
//////////////////////////////////////////////////////////////////////////
int CEMultiLock::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    void * syncHandles[CEMultiLock::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i)
        syncHandles[i] = mSyncObjArray[i]->GetHandle();

    int index = CEMultiLock::LOCK_INDEX_INVALID;
    unsigned int result = mSizeCount > 0 ? WaitForMultipleObjectsEx(mSizeCount, static_cast<HANDLE *>(syncHandles), waitForAll ? TRUE : FALSE, timeout, isAlertable ? TRUE : FALSE) : WAIT_FAILED;
    if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + mSizeCount))
    {
        if (waitForAll == false)
        {
            index = result - WAIT_OBJECT_0;
            ASSERT(index >= 0 && index < mSizeCount);
            mLockedStates[index] = CEMultiLock::STATE_LOCKED;
        }
        else
        {
            for (int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = CEMultiLock::STATE_LOCKED;
            }

            index = CEMultiLock::LOCK_INDEX_ALL;
        }
    }
    else if (result == WAIT_IO_COMPLETION)
    {
        index = CEMultiLock::LOCK_INDEX_COMPLETION;
    }
    else if (result == WAIT_TIMEOUT)
    {
        index = CEMultiLock::LOCK_INDEX_TIMEOUT;
    }

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
    , mData     (reinterpret_cast<int *>(varPtr == NULL ? &mValue : varPtr))
    , mContext  (static_cast<void *>(NULL))
{
    ; // do nothing
}

CEInterlockedValue::CEInterlockedValue( unsigned int initValue )
    : mValue    (initValue)
    , mData     (reinterpret_cast<int *>(&mValue))
    , mContext  (static_cast<void *>(NULL))
{
    ; // do nothing
}

CEInterlockedValue::~CEInterlockedValue( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEInterlockedValue class, Methods
//////////////////////////////////////////////////////////////////////////
unsigned int CEInterlockedValue::Increment( void )
{
    return static_cast<unsigned int>(::InterlockedIncrement(reinterpret_cast<long *>(mData)));
}

unsigned int CEInterlockedValue::Decrement( void )
{
    return static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData)));
}

bool CEInterlockedValue::TestDecrement( void )
{
    return (static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData))) != 0);
}

bool CEInterlockedValue::TestDecrementZero(void)
{
    unsigned int data = static_cast<unsigned int>(-1);
    static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(&data), *mData));
    return ( data != 0 ? static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData))) == 0 : false );
}

unsigned int CEInterlockedValue::Add( unsigned int increment )
{
    return static_cast<unsigned int>(::InterlockedExchangeAdd(reinterpret_cast<long *>(mData), static_cast<long>(increment)));
}

unsigned int CEInterlockedValue::Subtract( unsigned int decrement )
{
    return static_cast<unsigned int>(::InterlockedExchangeAdd(reinterpret_cast<long *>(mData), static_cast<long>(0 - decrement)));
}

void CEInterlockedValue::GetData( unsigned int& data ) const
{
    return static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(&data), *mData));
}

unsigned int CEInterlockedValue::GetData( void ) const
{
    unsigned int result = 0;
    GetData(result);
    return result;
}

void CEInterlockedValue::SetData( unsigned int data )
{
    return static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(mData), static_cast<long>(data)));
}

unsigned int CEInterlockedValue::SetBits( unsigned int mask )
{
    long oldValue = 0;
    long newValue = 0;
    do
    {
        oldValue = newValue = *mData;
        newValue |= static_cast<long>(mask);  
    } while(::InterlockedCompareExchange(reinterpret_cast<long *>(mData), newValue, oldValue) != oldValue);

    return static_cast<unsigned int>(oldValue); 
}

unsigned int CEInterlockedValue::RemoveBits( unsigned int mask )
{
    long oldValue = 0;
    long newValue = 0;
    do
    {
        oldValue = newValue = *mData;
        newValue &= static_cast<long>(~mask);  
    } while(::InterlockedCompareExchange(reinterpret_cast<long *>(mData), newValue, oldValue) != oldValue);

    return static_cast<unsigned int>(oldValue); 
}

unsigned int CEInterlockedValue::ToggleBits( unsigned int mask )
{
    long oldValue = 0;
    long newValue = 0;
    do
    {
        oldValue = newValue = *mData;
        newValue ^= static_cast<long>(mask);  
    }
    while(::InterlockedCompareExchange(reinterpret_cast<long *>(mData), newValue, oldValue) != oldValue);
    return static_cast<unsigned int>(oldValue); 
}

#endif  // _WINDOWS
