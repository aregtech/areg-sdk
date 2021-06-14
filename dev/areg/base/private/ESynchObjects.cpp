/************************************************************************
 * \file        areg/base/private/ESynchObjects.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/NEMemory.hpp"

//////////////////////////////////////////////////////////////////////////
// MultiLock class, constants
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEBlockingSynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEBlockingSynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
IEBlockingSynchObject::IEBlockingSynchObject( IESynchObject::eSyncObject synchObjectType )
    : IESynchObject   (synchObjectType)
{
    ASSERT( synchObjectType == IESynchObject::SO_MUTEX      ||
            synchObjectType == IESynchObject::SO_SEMAPHORE  ||
            synchObjectType == IESynchObject::SO_CRITICAL   ||
            synchObjectType == IESynchObject::SO_RES_LOCK   ||
            synchObjectType == IESynchObject::SO_NOLOCK     );
}

IEBlockingSynchObject::~IEBlockingSynchObject( void )
{
    ; // do nothing
}

bool IEBlockingSynchObject::tryLock(void)
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// IEBlockingSynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEBlockingSynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class, Methods
//////////////////////////////////////////////////////////////////////////

bool Mutex::lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != NULL);
    return _lockMutex(mSynchObject, timeout);
}

bool Mutex::tryLock( void )
{
    return lock(IESynchObject::DO_NOT_WAIT);
}

bool Mutex::unlock( void )
{
    ASSERT(mSynchObject != NULL);
    return _unlockMutex(mSynchObject);
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Methods
//////////////////////////////////////////////////////////////////////////

bool SynchEvent::unlock( void )
{
    ASSERT(mSynchObject != NULL);
    return _unlockEvent(mSynchObject);
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Methods
//////////////////////////////////////////////////////////////////////////

bool Semaphore::tryLock( void )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NolockSynchObject::NolockSynchObject( void )
    : IEResourceLock(IESynchObject::SO_NOLOCK)
{
    ; // do nothing
}

NolockSynchObject::~NolockSynchObject( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class, Methods
//////////////////////////////////////////////////////////////////////////
bool NolockSynchObject::lock(unsigned int /*timeout = IESynchObject::WAIT_INFINITE*/)
{
    return true;
}

bool NolockSynchObject::unlock( void )
{
    return true;
}

bool NolockSynchObject::tryLock( void )
{
    return true;
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
        mSynchObject.lock();
}

Lock::~Lock( void )
{
    if (mAutoLock && mSynchObject.isValid())
        mSynchObject.unlock();
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
    // memset(mSyncHandles,  0, NECommon::MAXIMUM_WAITING_OBJECTS * sizeof(HANDLE));
    // memset(mLockedStates, 0, NECommon::MAXIMUM_WAITING_OBJECTS * sizeof(eLockedState)  );
    NEMemory::zeroBuffer(static_cast<void *>(mLockedStates), NECommon::MAXIMUM_WAITING_OBJECTS * sizeof(eLockedState)  );
    if (autoLock)
    {
        lock(IESynchObject::WAIT_INFINITE, true);
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
        if (mLockedStates[i] == static_cast<int>(MultiLock::STATE_LOCKED))
            mSyncObjArray[i]->unlock();
    }

    return true;
}

bool MultiLock::unlock( int index )
{
    bool result = false;
    if ( (index >= 0) && (index < mSizeCount) )
    {
        if (mLockedStates[index] == static_cast<int>(MultiLock::STATE_LOCKED))
            result = mSyncObjArray[index]->unlock();
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// IEResourceLock class implementation
//////////////////////////////////////////////////////////////////////////
IEResourceLock::IEResourceLock(IESynchObject::eSyncObject synchObjectType)
    : IEBlockingSynchObject ( synchObjectType)
{
    ASSERT((synchObjectType == IESynchObject::SO_RES_LOCK) || (synchObjectType == IESynchObject::SO_NOLOCK));
}

IEResourceLock::~IEResourceLock()
{

}
