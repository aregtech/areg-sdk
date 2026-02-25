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

namespace areg
{

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
    areg::Primitive ThreadLocalStorage::getStorageItem( const areg::String & Key ) const
    {
        areg::Primitive result{ areg::InvalidElement };

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

    void ThreadLocalStorage::setStorageItem(const areg::String & Key, areg::Primitive Value)
    {
        mStorageList.pushFirst(ThreadLocalStorage::StorageItem(Key, Value));
    }

    void ThreadLocalStorage::setStorageItem( const areg::String & Key, const void* Value )
    {
        areg::Primitive aln;
        aln.valPtr.mElement = const_cast<void *>(Value);
        setStorageItem(Key, aln);
    }

    void ThreadLocalStorage::setStorageItem( const areg::String & Key, uint32_t Value )
    {
        areg::Primitive aln;
        aln.valUInt.mElement = Value;
        setStorageItem(Key, aln);
    }

    void ThreadLocalStorage::setStorageItem( const areg::String & Key, uint64_t Value )
    {
        areg::Primitive aln;
        aln.valUInt64.mElement = Value;
        setStorageItem(Key, aln);
    }

    void ThreadLocalStorage::setStorageItem( const areg::String & Key, double Value )
    {
        areg::Primitive aln;
        aln.valDouble.mElement = Value;
        setStorageItem(Key, aln);
    }

    areg::Primitive ThreadLocalStorage::removeStoragteItem( const areg::String & Key )
    {
        areg::Primitive result{ {0} };
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

    bool ThreadLocalStorage::existKey( const areg::String & Key ) const
    {
        StorageList::LISTPOS pos = mStorageList.firstPosition();
        for ( ; mStorageList.isValidPosition(pos); pos = mStorageList.nextPosition(pos))
        {
            if (mStorageList.valueAtPosition(pos).first== Key)
                break;
        }

        return mStorageList.isValidPosition(pos);
    }

    const areg::String & ThreadLocalStorage::getName() const
    {
        return mOwningThread.getName();
    }

} // namespace areg
