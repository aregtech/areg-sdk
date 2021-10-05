#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/ThreadLocalStorage.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Local Storage class.
 *              Should be initialized when thread starts running.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NEMemory.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEPair.hpp"
#include "areg/base/TELinkedList.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class Thread;

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A local storage of a thread.
 *          When thread starts, it initializes local storage.
 *          Any object of local threadcan save an entry in thread local storage
 *          accessed by unique name. By default, every local storage ot a Thread
 *          contains the entry of the thread consumer. The entries of the 
 *          local storage are access by name.
 * \see     Thread
 **/
class AREG_API ThreadLocalStorage
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend Thread;

/************************************************************************/
// Internal class declaration
/************************************************************************/
private:
    //////////////////////////////////////////////////////////////////////////
    // ThreadLocalStorage::StorageItem class declaration
    //////////////////////////////////////////////////////////////////////////

    //!< Definition of storage helper object to compare keys and values.
    using ImplStorageItem   = TEPairImpl<const String &, NEMemory::uAlign>;

    //!< Definition of storage item to store.
    using StorageItem       = TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem>;

    //!< Definition of storage list helper object to compare values.
    using ImplStorageList   = TEListImpl<const ThreadLocalStorage::StorageItem &>;

    //!< Definition of storage list object to store items.
    using StorageList       = TELinkedList<ThreadLocalStorage::StorageItem, const ThreadLocalStorage::StorageItem &, ImplStorageList>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates thread local storage object by passing owning thread object.
     *          The thread local storage class cannot be manually created. It is
     *          automatically instantiated when thread is created and deleted
     *          when thread exists. The local storage object is valid and accessible only 
     *          withing thread context where it was created.
     * \param   owningThread    The local storage owning thread.
     **/
    explicit ThreadLocalStorage( Thread & owningThread );
    /**
     * \brief   Destructor
     **/
    ~ThreadLocalStorage( void );

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the name of thread local storage.
     *          The name of local storage is same as thread name.
     **/
    const String & getName( void ) const;

    /**
     * \brief   Returns true, if there is an local storage item 
     *          exists in the list with specified name.
     * \param   Key     The name of a local storage item.
     * \return  Returns true if local storage has an item with specified name.
     **/
    bool existKey(const char* Key) const;

    /**
     * \brief   Returns the owning thread object.
     **/
    inline Thread & getOwnerThread( void ) const;

    /**
     * \brief   Returns the size of a thread local storage, 
     *          i.e. the number of items saved in storage.
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Returns the element value, saved in the 
     *          local storage by given name.
     *          The Key should be exist in the list when function is called.
     *          Otherwise it will return NEMemory::InvalidElement element,
     *          which is defining invalid element.
     *          If element was not found, it will raise assertion in Debug version.
     *          In release version either call existKey() before calling thie method,
     *          or check the address of returned element with NEMemory::InvalidElement
     * \param   Key     The name of a local storage item.
     * \return  Returns local storage element if Key exists. 
     *          Otherwise it returns NEMemory::InvalidElement, if there is no element
     *          with specified name.
     **/
    NEMemory::uAlign getStorageItem( const char * Key ) const;

    /**
     * \brief   Saves specified item in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call removeStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void setStorageItem(const char * Key, NEMemory::uAlign Value);

    /**
     * \brief   Saves pointer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call removeStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void setStorageItem(const char * Key, const void * Value);

    /**
     * \brief   Saves integer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call removeStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void setStorageItem(const char * Key, unsigned int Value);

    /**
     * \brief   Saves 64-bit integer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call removeStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void setStorageItem(const char * Key, uint64_t Value);

    /**
     * \brief   Saves number with floating point value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call removeStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void setStorageItem(const char * Key, double Value);

    /**
     * \brief   If thread local storage has an item with specified name
     *          it will be removed. If thread local storage
     *          has more than one item with the same name, the first matching
     *          item will be removed.
     * \param   Key     The name of local storage item to be removed
     * \return  Returns the value of returned element. If element does not
     *          exist, it will return dummy zero value.
     **/
    NEMemory::uAlign removeStoragteItem(const char * Key);

    /**
     * \brief   Removes all items in thread local storage
     **/
    inline void removeAll( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The storage list object, which contains (key; value) pair entrier
     **/
    StorageList mStorageList;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The thread object, which is a holder of thread storage.
     **/
    Thread &    mOwningThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ThreadLocalStorage( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ThreadLocalStorage );
};

//////////////////////////////////////////////////////////////////////////
// ThreadLocalStorage class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Thread & ThreadLocalStorage::getOwnerThread( void ) const
{
    return mOwningThread;
}

inline int ThreadLocalStorage::getSize( void ) const
{
    return mStorageList.getSize();
}

inline void ThreadLocalStorage::removeAll( void )
{
    return mStorageList.removeAll();
}
