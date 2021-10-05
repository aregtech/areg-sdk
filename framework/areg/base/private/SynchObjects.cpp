/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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
// Mutex class, Methods
//////////////////////////////////////////////////////////////////////////

Mutex::~Mutex( void )
{
    _unlockMutex( );
}

bool Mutex::lock(unsigned int timeout /* = NECommon::WAIT_INFINITE */)
{
    ASSERT(mSynchObject != nullptr);
    return _lockMutex(timeout);
}

bool Mutex::tryLock( void )
{
    return lock(NECommon::DO_NOT_WAIT);
}

bool Mutex::unlock( void )
{
    ASSERT(mSynchObject != nullptr);
    return _unlockMutex( );
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class, Methods
//////////////////////////////////////////////////////////////////////////

bool SynchEvent::unlock( void )
{
    ASSERT(mSynchObject != nullptr);
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

NolockSynchObject::NolockSynchObject( void )
    : IEResourceLock( IESynchObject::eSyncObject::SoNolock )
{
}

NolockSynchObject::~NolockSynchObject( void )
{
}

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class, Methods
//////////////////////////////////////////////////////////////////////////
bool NolockSynchObject::lock(unsigned int /*timeout = NECommon::WAIT_INFINITE*/)
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
