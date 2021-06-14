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
#include "areg/base/Thread.hpp"
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
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Mutex::Mutex(bool lock /* = true */)
    : IEBlockingSynchObject ( IESynchObject::SO_MUTEX )

    , mOwnerThreadId        ( 0 )
{
    mSynchObject    = static_cast<void *>(CreateMutex(NULL, lock ? TRUE : FALSE, NULL));
    if (lock && (mSynchObject != NULL))
    {
        _setOwnership();
    }
}

Mutex::~Mutex( void )
{
    unlock();
}

//////////////////////////////////////////////////////////////////////////
// Mutex class, Methods
//////////////////////////////////////////////////////////////////////////

bool Mutex::_lockMutex( void * mutexHandle, unsigned int timeout );
{
    bool result = false;

    if ((mutexHandle != NULL) && (WaitForSingleObject(static_cast<HANDLE>(mutexHandle), timeout) == WAIT_OBJECT_0))
    {
        InterlockedExchange((long *)&mOwnerThreadId, (long)Thread::getCurrentThreadId());
        result = true;
    }

    return result;
}

bool Mutex::_unlockMutex( void * mutexHandle )
{
    bool result = false;
    if ((mutexHandle != NULL) && ReleaseMutex(static_cast<HANDLE>(mutexHandle)))
    {
        InterlockedExchange((long *)&mOwnerThreadId, (long)0);
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
    : IESynchObject   (IESynchObject::SO_EVENT)

    , mAutoReset        (autoReset)
{
    mSynchObject= static_cast<void *>(CreateEvent(NULL, autoReset ? FALSE : TRUE, lock ? FALSE : TRUE, NULL));
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
    return (mSynchObject != NULL ? ::SetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE : false);
}

bool SynchEvent::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    return ( WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0 );
}

bool SynchEvent::setEvent( void )
{
    ASSERT(mSynchObject != NULL);
    return (::SetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool SynchEvent::resetEvent( void )
{
    ASSERT(mSynchObject != NULL);
    return (::ResetEvent(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

void SynchEvent::pulseEvent( void )
{
    ASSERT(mSynchObject != NULL);
    ::PulseEvent(static_cast<HANDLE>(mSynchObject));
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore(int maxCount, int initCount /* = 0 */)
    : IEBlockingSynchObject(IESynchObject::SO_SEMAPHORE)

    , mMaxCount             (maxCount <= 0 ? 1 : maxCount)
    , mCurrCount            (0)
{
    initCount   = initCount >= 0 && initCount <= mMaxCount ? initCount : 0;
    mSynchObject= static_cast<void *>(CreateSemaphore(NULL, initCount, mMaxCount, NULL));
    mCurrCount  = mSynchObject != NULL ? initCount : 0;
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
    bool result = false;
    if (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0)
    {
        InterlockedDecrement(&mCurrCount);
        result = true;
    }
    return result;
}

bool Semaphore::unlock( void )
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
    mSynchObject= static_cast<HANDLE>(CreateWaitableTimer(NULL, autoReset ? FALSE : TRUE, NULL));
    if (initSignaled == false)
    {
        setTimer();
    }
}

SynchTimer::~SynchTimer( void )
{
    if (mSynchObject != NULL)
    {
        CancelWaitableTimer(static_cast<HANDLE>(mSynchObject));
        CloseHandle(static_cast<HANDLE>(mSynchObject));
    }

    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class, Methods
//////////////////////////////////////////////////////////////////////////
bool SynchTimer::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    return (WaitForSingleObject(static_cast<HANDLE>(mSynchObject), timeout) == WAIT_OBJECT_0);
}

bool SynchTimer::unlock( void )
{
    return (CancelWaitableTimer(static_cast<HANDLE>(mSynchObject)) != FALSE);
}

bool SynchTimer::setTimer( void )
{
    LARGE_INTEGER dueTime;
    dueTime.QuadPart = static_cast<int64_t>(-1) * static_cast<int64_t>(mTimeMilliseconds) * static_cast<int64_t>(SynchTimer::NANOSECONDS_KOEF_100);
    return (SetWaitableTimer(static_cast<HANDLE>(mSynchObject), &dueTime, mIsPeriodic ? mTimeMilliseconds : 0, NULL, NULL, FALSE) != FALSE);
}

bool SynchTimer::cancelTimer( void )
{
    return (CancelWaitableTimer(static_cast<HANDLE>(mSynchObject)) != FALSE);
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
    mSynchObject = static_cast<void *>( DEBUG_NEW unsigned char [sizeof(CRITICAL_SECTION)] );
    if (mSynchObject != NULL)
    {
        NEMemory::constructElems<CRITICAL_SECTION>(mSynchObject, 1);
        InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    }
}

CriticalSection::~CriticalSection( void )
{
    if (mSynchObject != NULL)
    {
        DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
        delete [] reinterpret_cast<unsigned char *>(mSynchObject);
    }
    mSynchObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Methods
//////////////////////////////////////////////////////////////////////////
bool CriticalSection::lock(unsigned int  /*timeout = IESynchObject::WAIT_INFINITE */)
{
    if (mSynchObject != NULL)
        EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    return (mSynchObject != NULL);
}

bool CriticalSection::unlock( void )
{
    if (mSynchObject != NULL)
        LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject));
    return (mSynchObject != NULL);
}

bool CriticalSection::tryLock( void )
{
    return (mSynchObject != NULL && TryEnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mSynchObject)) ? true : false);
}

//////////////////////////////////////////////////////////////////////////
// ResourceLock class implementation
//////////////////////////////////////////////////////////////////////////

ResourceLock::ResourceLock( bool initLock /*= false*/ )
    : IEResourceLock(IESynchObject::SO_RES_LOCK)
{

#if 0
    mSynchObject    = new Mutex(initLock);

#else

    mSynchObject    = new CriticalSection();
    if ( initLock && (mSynchObject != NULL) )
    {
        reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->lock(IESynchObject::WAIT_INFINITE);
    }

#endif

}

ResourceLock::~ResourceLock(void)
{
    if (mSynchObject != NULL)
    {
        reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->unlock();
        delete reinterpret_cast<IEBlockingSynchObject *>(mSynchObject);
        mSynchObject = NULL;
    }
}

bool ResourceLock::lock(unsigned int timeout /*= IESynchObject::WAIT_INFINITE */)
{
    return( mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->lock(timeout) : true);
}

bool ResourceLock::unlock(void)
{
    return ( mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->unlock() : true);
}

bool ResourceLock::tryLock(void)
{
    return (mSynchObject != NULL ? reinterpret_cast<IEBlockingSynchObject *>(mSynchObject)->tryLock() : true);
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Methods
//////////////////////////////////////////////////////////////////////////
int MultiLock::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/)
{
    void * syncHandles[NECommon::MAXIMUM_WAITING_OBJECTS];
    for ( int i = 0; i < mSizeCount; ++ i)
        syncHandles[i] = mSyncObjArray[i]->getHandle();

    int index = MultiLock::LOCK_INDEX_INVALID;
    unsigned int result = mSizeCount > 0 ? WaitForMultipleObjectsEx(mSizeCount, static_cast<HANDLE *>(syncHandles), waitForAll ? TRUE : FALSE, timeout, isAlertable ? TRUE : FALSE) : WAIT_FAILED;
    if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + mSizeCount))
    {
        if (waitForAll == false)
        {
            index = result - WAIT_OBJECT_0;
            ASSERT(index >= 0 && index < mSizeCount);
            mLockedStates[index] = MultiLock::STATE_LOCKED;
        }
        else
        {
            for (int i = 0; i < mSizeCount; ++ i)
            {
                mLockedStates[i] = MultiLock::STATE_LOCKED;
            }

            index = MultiLock::LOCK_INDEX_ALL;
        }
    }
    else if (result == WAIT_IO_COMPLETION)
    {
        index = MultiLock::LOCK_INDEX_COMPLETION;
    }
    else if (result == WAIT_TIMEOUT)
    {
        index = MultiLock::LOCK_INDEX_TIMEOUT;
    }

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
    , mData     (reinterpret_cast<int *>(varPtr == NULL ? &mValue : varPtr))
    , mContext  (static_cast<void *>(NULL))
{
    ; // do nothing
}

InterlockedValue::InterlockedValue( unsigned int initValue )
    : mValue    (initValue)
    , mData     (reinterpret_cast<int *>(&mValue))
    , mContext  (static_cast<void *>(NULL))
{
    ; // do nothing
}

InterlockedValue::~InterlockedValue( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// InterlockedValue class, Methods
//////////////////////////////////////////////////////////////////////////
unsigned int InterlockedValue::increment( void )
{
    return static_cast<unsigned int>(::InterlockedIncrement(reinterpret_cast<long *>(mData)));
}

unsigned int InterlockedValue::decrement( void )
{
    return static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData)));
}

bool InterlockedValue::testDecrement( void )
{
    return (static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData))) != 0);
}

bool InterlockedValue::testDecrementZero(void)
{
    unsigned int data = static_cast<unsigned int>(-1);
    static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(&data), *mData));
    return ( data != 0 ? static_cast<unsigned int>(::InterlockedDecrement(reinterpret_cast<long *>(mData))) == 0 : false );
}

unsigned int InterlockedValue::add( unsigned int increment )
{
    return static_cast<unsigned int>(::InterlockedExchangeAdd(reinterpret_cast<long *>(mData), static_cast<long>(increment)));
}

unsigned int InterlockedValue::subtract( unsigned int decrement )
{
    return static_cast<unsigned int>(::InterlockedExchangeAdd(reinterpret_cast<long *>(mData), static_cast<long>(0 - decrement)));
}

void InterlockedValue::getData( unsigned int& data ) const
{
    return static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(&data), *mData));
}

unsigned int InterlockedValue::getData( void ) const
{
    unsigned int result = 0;
    getData(result);
    return result;
}

void InterlockedValue::setData( unsigned int data )
{
    return static_cast<void>(::InterlockedExchange(reinterpret_cast<long *>(mData), static_cast<long>(data)));
}

unsigned int InterlockedValue::setBits( unsigned int mask )
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

unsigned int InterlockedValue::removeBits( unsigned int mask )
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

unsigned int InterlockedValue::toggleBits( unsigned int mask )
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
