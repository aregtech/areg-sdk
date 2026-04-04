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
namespace areg {

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
areg::Primitive ThreadLocalStorage::item( const String & Key ) const
{
    areg::Primitive result{ areg::InvalidElement };
    int32_t pos = _find_item(Key);
    if (pos != areg::INVALID_INDEX)
    {
        result = mStorageList[static_cast<uint32_t>(pos)].second;
    }

    return result;
}

void ThreadLocalStorage::set_item(const String & Key, areg::Primitive Value)
{
    int32_t pos = _find_item(Key);
    if (pos != areg::INVALID_INDEX)
    {
        mStorageList[static_cast<uint32_t>(pos)].second = Value;
    }
    else
    {
        mStorageList.push_first(ThreadLocalStorage::StorageItem(Key, Value));
    }
}

void ThreadLocalStorage::set_item( const String & Key, const void* Value )
{
    areg::Primitive aln;
    aln.valPtr.mElement = const_cast<void *>(Value);
    set_item(Key, aln);
}

void ThreadLocalStorage::set_item( const String & Key, uint32_t Value )
{
    areg::Primitive aln;
    aln.valUInt.mElement = Value;
    set_item(Key, aln);
}

void ThreadLocalStorage::set_item( const String & Key, uint64_t Value )
{
    areg::Primitive aln;
    aln.valUInt64.mElement = Value;
    set_item(Key, aln);
}

void ThreadLocalStorage::set_item( const String & Key, double Value )
{
    areg::Primitive aln;
    aln.valDouble.mElement = Value;
    set_item(Key, aln);
}

areg::Primitive ThreadLocalStorage::remove_item( const String & Key )
{
    areg::Primitive result{ {0} };
    StorageList::LISTPOS pos = mStorageList.first_position();
    for ( ; mStorageList.is_valid_position(pos); pos = mStorageList.next_position(pos))
    {
        const ThreadLocalStorage::StorageItem & value = mStorageList.value_at(pos);
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
        if (mStorageList.value_at(pos).first== Key)
            break;
    }

    return mStorageList.is_valid_position(pos);
}

const String & ThreadLocalStorage::name() const
{
    return mOwningThread.name();
}

} // namespace areg
