/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ThreadLocalStorage.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Local Storage class.
 *              Should be initialized when thread starts running.
 *
 ************************************************************************/
#include "areg/base/ThreadLocalStorage.hpp"
#include "areg/base/Thread.hpp"

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class constructor / destructor
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage::ThreadLocalStorage(Thread & owningThread)
    : mStorageList  ( )
    , mOwningThread (owningThread)
{
}

ThreadLocalStorage::~ThreadLocalStorage( void )
{
    mStorageList.clear();
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class methods
//////////////////////////////////////////////////////////////////////////
NEMemory::uAlign ThreadLocalStorage::getStorageItem( const String & Key ) const
{
    NEMemory::uAlign result = NEMemory::InvalidElement;

    StorageList::LISTPOS pos = mStorageList.firstPosition();
    for ( ; mStorageList.isValidPosition(pos); pos = mStorageList.nextPosition(pos))
    {
        const ThreadLocalStorage::StorageItem& value = mStorageList.valueAtPosition(pos);
        if (value.first == Key)
        {
            result = value.second;
            break;
        }
    }

    return result;
}

void ThreadLocalStorage::setStorageItem(const String & Key, NEMemory::uAlign Value)
{
    mStorageList.pushFirst(ThreadLocalStorage::StorageItem(Key, Value));
}

void ThreadLocalStorage::setStorageItem( const String & Key, const void* Value )
{
    NEMemory::uAlign aln;
    aln.alignPtr.mElement = (void *)(Value);
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const String & Key, unsigned int Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt.mElement = Value;
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const String & Key, uint64_t Value )
{
    NEMemory::uAlign aln;
    aln.alignUInt64.mElement = Value;
    setStorageItem(Key, aln);
}

void ThreadLocalStorage::setStorageItem( const String & Key, double Value )
{
    NEMemory::uAlign aln;
    aln.alignDouble.mElement = Value;
    setStorageItem(Key, aln);
}

NEMemory::uAlign ThreadLocalStorage::removeStoragteItem( const String & Key )
{
    NEMemory::uAlign result{ {0} };
    StorageList::LISTPOS pos = mStorageList.firstPosition();
    for ( ; mStorageList.isValidPosition(pos); pos = mStorageList.nextPosition(pos))
    {
        const ThreadLocalStorage::StorageItem & value = mStorageList.valueAtPosition(pos);
        if (value.first == Key)
        {
            result = value.second;
            mStorageList.removeAt(pos);
            break;
        }
    }

    return result;
}

bool ThreadLocalStorage::existKey( const String & Key ) const
{
    StorageList::LISTPOS pos = mStorageList.firstPosition();
    for ( ; mStorageList.isValidPosition(pos); pos = mStorageList.nextPosition(pos))
    {
        if (mStorageList.valueAtPosition(pos).first== Key)
            break;
    }

    return mStorageList.isValidPosition(pos);
}

const String & ThreadLocalStorage::getName( void ) const
{
    return mOwningThread.getName();
}
