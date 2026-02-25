/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ThreadLocalStorage.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Local Storage class.
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

ThreadLocalStorage::~ThreadLocalStorage()
{
    mStorageList.clear();
}

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class methods
//////////////////////////////////////////////////////////////////////////
NEMemory::Primitive ThreadLocalStorage::storage_item( const String & Key ) const
{
    NEMemory::Primitive result{ NEMemory::InvalidElement };

    StorageList::LISTPOS pos = mStorageList.first_position();
    for ( ; mStorageList.is_valid_position(pos); pos = mStorageList.next_position(pos))
    {
        const ThreadLocalStorage::StorageItem& value = mStorageList.value_at_position(pos);
        if (value.first == Key)
        {
            result = value.second;
            break;
        }
    }

    return result;
}

void ThreadLocalStorage::set_storage_item(const String & Key, NEMemory::Primitive Value)
{
    mStorageList.push_first(ThreadLocalStorage::StorageItem(Key, Value));
}

void ThreadLocalStorage::set_storage_item( const String & Key, const void* Value )
{
    NEMemory::Primitive aln;
    aln.valPtr.mElement = const_cast<void *>(Value);
    set_storage_item(Key, aln);
}

void ThreadLocalStorage::set_storage_item( const String & Key, uint32_t Value )
{
    NEMemory::Primitive aln;
    aln.valUInt.mElement = Value;
    set_storage_item(Key, aln);
}

void ThreadLocalStorage::set_storage_item( const String & Key, uint64_t Value )
{
    NEMemory::Primitive aln;
    aln.valUInt64.mElement = Value;
    set_storage_item(Key, aln);
}

void ThreadLocalStorage::set_storage_item( const String & Key, double Value )
{
    NEMemory::Primitive aln;
    aln.valDouble.mElement = Value;
    set_storage_item(Key, aln);
}

NEMemory::Primitive ThreadLocalStorage::remove_storagte_item( const String & Key )
{
    NEMemory::Primitive result{ {0} };
    StorageList::LISTPOS pos = mStorageList.first_position();
    for ( ; mStorageList.is_valid_position(pos); pos = mStorageList.next_position(pos))
    {
        const ThreadLocalStorage::StorageItem & value = mStorageList.value_at_position(pos);
        if (value.first == Key)
        {
            result = value.second;
            mStorageList.remove_at(pos);
            break;
        }
    }

    return result;
}

bool ThreadLocalStorage::exist( const String & Key ) const
{
    StorageList::LISTPOS pos = mStorageList.first_position();
    for ( ; mStorageList.is_valid_position(pos); pos = mStorageList.next_position(pos))
    {
        if (mStorageList.value_at_position(pos).first== Key)
            break;
    }

    return mStorageList.is_valid_position(pos);
}

const String & ThreadLocalStorage::name() const
{
    return mOwningThread.name();
}
