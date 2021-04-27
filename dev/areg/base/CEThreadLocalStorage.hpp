#ifndef AREG_BASE_CETHREADLOCALSTORAGE_HPP
#define AREG_BASE_CETHREADLOCALSTORAGE_HPP
/************************************************************************
 * \file        areg/base/CEThreadLocalStorage.hpp
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
#include "areg/base/CEString.hpp"
#include "areg/base/TEPair.hpp"
#include "areg/base/TELinkedList.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEThread;

/**
 * \brief   A local storage of a thread.
 *          When thread starts, it initializes local storage.
 *          The user of local storage can save initialized value in the storage
 *          by unique key. By default, every CEThread object contains
 *          thread consumer entry in the storage.
 *          The local storage items are access by name.
 * \see     CEThread
 **/

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEThreadLocalStorage
{
/************************************************************************/
// Friend classes
/************************************************************************/
    friend CEThread;

/************************************************************************/
// Internal class declaration
/************************************************************************/
private:
    class CEStorageItem;
    class CEStorageList;

//////////////////////////////////////////////////////////////////////////
//
// Internals of CEThreadLocalStorage class. Begin
// 
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // CEThreadLocalStorage::CEStorageItem class declaration
    //////////////////////////////////////////////////////////////////////////

    typedef TEPairImpl <const CEString &, NEMemory::uAlign>   ImplStorageItem;

    /**
     * \brief   Local Storage Item class.
     *          Local Storage Item is a pair of string and aligned NEMemory::uAlign
     *          entry, which can be converted to any simple value.
     **/
    class AREG_API CEStorageItem : public TEPair<CEString, NEMemory::uAlign, const CEString &, NEMemory::uAlign, ImplStorageItem>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. Creates empty item. Never used, required by linked list object
         **/
        CEStorageItem( void );
        /**
         * \brief   Creates Local Storage item and initializes key and value of an item.
         * \param   Key     The name of a storage item
         * \param   Value   The value of a storage item.
         **/
        CEStorageItem(const CEString & Key, NEMemory::uAlign Value);
        /**
         * \brief   Copy constructor.
         * \param   src     The source to copy data.
         **/
        CEStorageItem( const CEThreadLocalStorage::CEStorageItem & src );
        /**
         * \brief   Destructor
         **/
        ~CEStorageItem( void );

    //////////////////////////////////////////////////////////////////////////
    // Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigning operator. Copies Key and Value values from given source.
         **/
        const CEThreadLocalStorage::CEStorageItem & operator = ( const CEThreadLocalStorage::CEStorageItem & src );
    };

    //////////////////////////////////////////////////////////////////////////
    // CEThreadLocalStorage::CEStorageList class declaration
    //////////////////////////////////////////////////////////////////////////

    typedef TEListImpl<const CEThreadLocalStorage::CEStorageItem &>   ImplStorageList;

    /**
     * \brief   Local Storage Item List, which contains local storage items.
     *          The Local Storage Items are searched in this list by their name.
     *          The name should be unique, otherwise when item is searched by
     *          name, any first item entry will be returned.
     **/
    class AREG_API CEStorageList : public TELinkedList<CEThreadLocalStorage::CEStorageItem, const CEThreadLocalStorage::CEStorageItem &, ImplStorageList>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        CEStorageList( void );
        /**
         * \brief   Destructor.
         **/
        ~CEStorageList( void );

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods.
    //////////////////////////////////////////////////////////////////////////
    private:
        CEStorageList( const CEThreadLocalStorage::CEStorageList & /*src*/ );
        const CEThreadLocalStorage::CEStorageList & operator = ( const CEThreadLocalStorage::CEStorageList & /*src*/ );
    };

//////////////////////////////////////////////////////////////////////////
//
// Internals of CEThreadLocalStorage class. End
// 
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Start CETrhead class
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     *          Creates thread local storage object by passing owning thread object.
     *          The thread local storage class cannot be manually created. It is
     *          automatically instantiated when thread is created and deleted
     *          when thread exists.
     *          The local storage object is valid and accessible only 
     *          withing thread context where it was created
     * \param   owningThread    The local storage owning thread.
     **/
    CEThreadLocalStorage( CEThread & owningThread );
    /**
     * \brief   Destructor
     **/
    ~CEThreadLocalStorage( void );

//////////////////////////////////////////////////////////////////////////
// Attributes / Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the name of thread local storage.
     *          The name of local storage is same as thread name.
     **/
    const char * GetName( void ) const;

    /**
     * \brief   Returns true, if there is an local storage item 
     *          exists in the list with specified name.
     * \param   Key     The name of a local storage item.
     * \return  Returns true if local storage has an item with specified name.
     **/
    bool KeyExist(const char* Key) const;

    /**
     * \brief   Returns the owning thread object.
     **/
    inline CEThread & GetThread( void ) const;

    /**
     * \brief   Returns the size of a thread local storage, 
     *          i.e. the number of items saved in storage.
     **/
    inline int GetSize( void ) const;

    /**
     * \brief   Returns the element value, saved in the 
     *          local storage by given name.
     *          The Key should be exist in the list when function is called.
     *          Otherwise it will return NEMemory::InvalidElement element,
     *          which is defining invalid element.
     *          If element was not found, it will raise assertion in Debug version.
     *          In release version either call KeyExist() before calling thie method,
     *          or check the address of returned element with NEMemory::InvalidElement
     * \param   Key     The name of a local storage item.
     * \return  Returns local storage element if Key exists. 
     *          Otherwise it returns NEMemory::InvalidElement, if there is no element
     *          with specified name.
     **/
    NEMemory::uAlign GetStorageItem( const char * Key ) const;

    /**
     * \brief   Saves specified item in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call RemoveStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void SetStorageItem(const char * Key, NEMemory::uAlign Value);

    /**
     * \brief   Saves pointer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call RemoveStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void SetStorageItem(const char * Key, const void * Value);

    /**
     * \brief   Saves integer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call RemoveStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void SetStorageItem(const char * Key, unsigned int Value);

    /**
     * \brief   Saves 64-bit integer value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call RemoveStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void SetStorageItem(const char * Key, uint64_t Value);

    /**
     * \brief   Saves number with floating point value in thread local storage object.
     *          The method is not checking whether the local storage already has
     *          item with specified name or not. If item with specified name
     *          exists, it will not be replaced. To make sure that every item
     *          is saved by unique name, first call RemoveStoragteItem() method
     * \param   Key     The name of local storage item
     * \param   Value   The value of local storage item
     **/
    void SetStorageItem(const char * Key, double Value);

    /**
     * \brief   If thread local storage has an item with specified name
     *          it will be removed. If thread local storage
     *          has more than one item with the same name, the first matching
     *          item will be removed.
     * \param   Key     The name of local storage item to be removed
     * \return  Returns the value of returned element. If element does not
     *          exist, it will return dummy zero value.
     **/
    NEMemory::uAlign RemoveStoragteItem(const char * Key);

    /**
     * \brief   Removes all items in thread local storage
     **/
    inline void RemoveAllItems( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The storage list object, which contains (key; value) pair entrier
     **/
    CEStorageList   mStorageList;
    /**
     * \brief   The thread object, which is a holder of thread storage.
     **/
    CEThread &      mOwningThread;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    CEThreadLocalStorage( void );
    CEThreadLocalStorage( const CEThreadLocalStorage & /*src*/ );
    const CEThreadLocalStorage & operator = ( const CEThreadLocalStorage & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEThreadLocalStorage class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEThread & CEThreadLocalStorage::GetThread( void ) const
{   return mOwningThread;               }

inline int CEThreadLocalStorage::GetSize( void ) const
{   return mStorageList.GetSize();      }

inline void CEThreadLocalStorage::RemoveAllItems( void )
{   return mStorageList.RemoveAll();    }

#endif  // AREG_BASE_CETHREADLOCALSTORAGE_HPP
