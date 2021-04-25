/************************************************************************
 * \file        areg/src/base/private/ESynchObjects.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/src/base/ESynchObjects.hpp"
#include "areg/src/base/NEMemory.hpp"

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class, constants
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

bool IEBlockingSynchObject::TryLock(void)
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
// CEMutex class, Methods
//////////////////////////////////////////////////////////////////////////

bool CEMutex::TryLock( void )
{
    return Lock(IESynchObject::DO_NOT_WAIT);
}

//////////////////////////////////////////////////////////////////////////
// CESemaphore class, Methods
//////////////////////////////////////////////////////////////////////////

bool CESemaphore::TryLock( void )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// CENolockSynchObject class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CENolockSynchObject class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CENolockSynchObject::CENolockSynchObject( bool /*lock = true*/, bool /*autoReset = true*/ )
    : IEResourceLock(IESynchObject::SO_NOLOCK)
{
    ; // do nothing
}

CENolockSynchObject::CENolockSynchObject( int /*maxCount*/, int /*initCount = 0*/ )
    : IEResourceLock(IESynchObject::SO_NOLOCK)
{
    ; // do nothing
}

CENolockSynchObject::~CENolockSynchObject( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CENolockSynchObject class, Methods
//////////////////////////////////////////////////////////////////////////
bool CENolockSynchObject::Lock(unsigned int /*timeout = IESynchObject::WAIT_INFINITE*/)
{
    return true;
}

bool CENolockSynchObject::Unlock( void )
{
    return true;
}

bool CENolockSynchObject::TryLock( void )
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// CELock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CELock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CELock::CELock(IESynchObject &syncObj, bool autoLock /* = true */)
    : mSynchObject  (syncObj)
    , mAutoLock     (autoLock)
{
    if (mAutoLock && mSynchObject.IsValid() )
        mSynchObject.Lock();
}

CELock::~CELock( void )
{
    if (mAutoLock && mSynchObject.IsValid())
        mSynchObject.Unlock();
}

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMultiLock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEMultiLock::CEMultiLock(IESynchObject* pObjects[], int count, bool autoLock /* = true */)
    : mSyncObjArray (pObjects)
    , mSizeCount    (MACRO_MIN(count, CEMultiLock::MAXIMUM_WAITING_OBJECTS))
    , mAutoLock     (autoLock)
{
    // memset(mSyncHandles,  0, MAXIMUM_WAITING_OBJECTS * sizeof(HANDLE));
    // memset(mLockedStates, 0, MAXIMUM_WAITING_OBJECTS * sizeof(eLockedState)  );
    NEMemory::ZeroBuffer(static_cast<void *>(mLockedStates), MAXIMUM_WAITING_OBJECTS * sizeof(eLockedState)  );
    if (autoLock)
    {
        Lock(IESynchObject::WAIT_INFINITE, true);
    }
}

CEMultiLock::~CEMultiLock( void )
{
    if (mAutoLock)
    {
        Unlock();
    }
}

bool CEMultiLock::Unlock( void )
{
    for (int i = 0; i < mSizeCount; ++ i)
    {
        if (mLockedStates[i] == static_cast<int>(CEMultiLock::STATE_LOCKED))
            mSyncObjArray[i]->Unlock();
    }

    return true;
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
