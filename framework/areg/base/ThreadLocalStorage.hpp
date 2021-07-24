#ifndef AREG_BASE_THREADLOCALSTORAGE_HPP
#define AREG_BASE_THREADLOCALSTORAGE_HPP
/************************************************************************
 * \file        areg/base/ThreadLocalStorage.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
 *          The user of local storage can save initialized value in the storage
 *          by unique key. By default, every Thread object contains
 *          thread consumer entry in the storage.
 *          The local storage items are access by name.
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
    class StorageItem;
    class StorageList;

//////////////////////////////////////////////////////////////////////////
//
// Internals of ThreadLocalStorage class. Begin
// 
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // ThreadLocalStorage::StorageItem class declaration
    //////////////////////////////////////////////////////////////////////////

    typedef TEPairImpl <const String &, NEMemory::uAlign>   ImplStorageItem;

    /**
     * \brief   Local Storage Item class.
     *          Local Storage Item is a pair of string and aligned NEMemory::uAlign
     *          entry, which can be converted to any simple value.
     **/
    class AREG_API StorageItem : public TEPair<String, NEMemory::uAlign, const String &, NEMemory::uAlign, ImplStorageItem>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. Creates empty item. Never used, required by linked list object
         **/
        StorageItem( void );
        /**
         * \brief   Creates Local Storage item and initializes key and value of an item.
         * \param   Key     The name of a storage item
         * \param   Value   The value of a storage item.
         **/
        StorageItem(const String & Key, NEMemory::uAlign Value);
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        StorageItem( const ThreadLocalStorage::StorageItem & src );
        /**
         * \brief   Destructor
         **/
        ~StorageItem( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigning operator. Copies Key and Value values from given source.
         **/
        const ThreadLocalStorage::StorageItem & operator = ( const ThreadLocalStorage::StorageItem & src );
    };

    //////////////////////////////////////////////////////////////////////////
    // ThreadLocalStorage::StorageList class declaration
    //////////////////////////////////////////////////////////////////////////

    typedef TEListImpl<const ThreadLocalStorage::StorageItem &>   ImplStorageList;

    /**
     * \brief   Local Storage Item List, which contains local storage items.
     *          The Local Storage Items are searched in this list by their name.
     *          The name should be unique, otherwise when item is searched by
     *          name, any first item entry will be returned.
     **/
    class AREG_API StorageList : public TELinkedList<ThreadLocalStorage::StorageItem, const ThreadLocalStorage::StorageItem &, ImplStorageList>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        StorageList( void );
        /**
         * \brief   Destructor.
         **/
        ~StorageList( void );

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        StorageList( const ThreadLocalStorage::StorageList & /*src*/ );
        const ThreadLocalStorage::StorageList & operator = ( const ThreadLocalStorage::StorageList & /*src*/ );
    };

//////////////////////////////////////////////////////////////////////////
//
// Internals of ThreadLocalStorage class. End
// 
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Start Trhead class
/************************************************************************/

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
    ThreadLocalStorage( Thread & owningThread );
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
    const char * getName( void ) const;

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
    /**
     * \brief   The storage list object, which contains (key; value) pair entrier
     **/
    StorageList mStorageList;
    /**
     * \brief   The thread object, which is a holder of thread storage.
     **/
    Thread &    mOwningThread;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ThreadLocalStorage( void );
    ThreadLocalStorage( const ThreadLocalStorage & /*src*/ );
    const ThreadLocalStorage & operator = ( const ThreadLocalStorage & /*src*/ );
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

#endif  // AREG_BASE_THREADLOCALSTORAGE_HPP
