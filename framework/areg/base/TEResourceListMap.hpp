#ifndef AREG_BASE_TERESOURCELISTMAP_HPP
#define AREG_BASE_TERESOURCELISTMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TEResourceListMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Resource List Container Map class template.
 *              Use to store list of resources associated with the Key.
 *              Here are defined 2 types of resource map:
 *              -   Blocking (thread safe);
 *              -   Non-blocking (no thread safe).
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/Containers.hpp"
#include "areg/base/SynchObjects.hpp"

  /************************************************************************
   * Hierarchies. Following class are declared.
   ************************************************************************/
   // HashMap 
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class HashMap, class Tracker> class TEResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class HashMap, class Tracker> class TELockResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class HashMap, class Tracker> class TENolockResourceListMap;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>,
 *                  which is a generic list of resource map class template;
 *
 *              2.  TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>,
 *                  which is a no thread safe list of resource map class template;
 *
 *              3.  TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>,
 *                  which is a thread safe list of resource map class template;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TEResourceListMap is a class template of Hash Map container used to keep
 *          control over registered resources in the linked list (ResourceList).
 *          The resources are objects allocated in the memory and saved in the 
 *          ResourceList, which are accessed by unique resource key. This means that
 *          the ResourceList is a container of pointers to the resource objects. 
 *          The resource keys should have at least default constructor and should have
 *          implemented hasher std::hash<KEY> to compute hash value. The resource map
 *          receives a synchronization object to lock and unlock resources during
 *          runtime if needed. For additional of locking resources see 
 *          TELockResourceListMap and TENolockResourceListMap class templates.
 * 
 *          See TEHashMap for details of computing hash value.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 * 
 * \see    TELockResourceListMap, TENolockResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class ResourceList = TELinkedList<RESOURCE_OBJECT>
        , class HashMap      = TEHashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
class TEResourceListMap : protected HashMap
                        , protected Tracker
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes the locking object.
     **/
    explicit TEResourceListMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor.
     **/
    ~TEResourceListMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Returns the size of Resource Map
     **/
    inline uint32_t getSize(void) const;

    /**
     * \brief   Returns true if resource map is empty.
     **/
    inline bool isEmpty(void) const;

    /**
     * \brief	Checks whether Resource List Map contains entry
     *          with specified unique Key or not.
     * \param	Key	    The unique Key of Resource that was registered.
     * \return	If Resource with unique Key is registered, returns true.
     *          Otherwise returns false
     **/
    inline bool existResource(const RESOURCE_KEY & Key) const;

    /**
     * \brief	Locks the Resource List Map, grants ownership to locking thread
     *          and blocks any other thread to access resource map data.
     **/
    inline void lock( void ) const;

    /**
     * \brief	Unlocks previously locked Resource List Map, so that other threads
     *          can have access to resource map data.
     **/
    inline void unlock( void ) const;

    /**
     * \brief	Tries to lock Resource List Map. If succeeds, calling thread gets resource
     *          locking ownership and any other threads are blocked and cannot access
     *          Resource List Map data. Otherwise the calling thread is not blocked and 
     *          does not get the ownership. Check return value to figure out whether
     *          calling have got resource ownership or not.
     * \return	Returns true if calling thread locked resource and have got the ownership.
     *          Otherwise,  returns false, but does not block thread execution.
     **/
    inline bool tryLock( void ) const;

    /**
     * \brief	Registers resource object in the resource list of the map. 
     * \param	Key	        The unique Key of Resource object.
     * \param	Resource	The pointer to Resource object, which should remain
     *                      valid until it is unregistered.
     **/
    inline void registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

    /**
     * \brief	Unregisters resource from the resource list of the map.
     * \param	Key	        Unique Key identifying previously registered resource.
     * \param   Resource    The valid pointer to the resource to unregister.
     * \param   removeEmpty If true, will remove the empty list and the entry specified
     *                      specified by Key. Otherwise, keeps the Key entry with
     *                      empty resource list.
     **/
    inline void unregisterResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource, bool removeEmpty = true );

    /**
     * \brief   If needed, creates new resource entry in the map. If the resource
     *          exists, returns the existing resource list. Otherwise, creates new
     *          entry and returns empty resource list.
     * \param   Key     The unique key of resource to register.
     * \return  If resource exists, returns existing resource list. Otherwise,
     *          creates new empty list of resource objects.
     **/
    inline ResourceList & registerResource( const RESOURCE_KEY & Key );

    /**
     * \brief   Unregister the resource in the map specified by the given Key and
     *          returns the copy of existing resource. Otherwise, returns empty list.
     * 
     * \param   Key     The unique Key of resource to unregister.
     * \return  Returns the list of registered resource objects lists.
     **/
    inline ResourceList unregisterResource( const RESOURCE_KEY & Key );

    /**
     * \brief	Search Resource in Resource Map by give Key.
     * 
     * \param	Key	    The unique Key that specifies resource list.
     * \return	If found, returns valid pointer to the existing resource list.
     *          Otherwise, returns nullptr.
     **/
    inline ResourceList * findResource( const RESOURCE_KEY & Key );
    inline const ResourceList * findResource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Returns pointer to the resource list in the map.
     * \param	Key	    The unique Key that specifies resource list.
     **/
    inline ResourceList * getResource( const RESOURCE_KEY & Key );
    inline const ResourceList * getResource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief	Removes Resource object from the resource list of the map.
     *          The function searches resource by checking every entry in the list.
     *          If found, removes the entry from the list. It remove empty resource
     *          list if 'removeEmpty' parameter is true.
     * \param	Resource	The resource object to lookup
     * \param   removeEmpty If true and the resource list is empty, removes the entry.
     * \return	Returns true if found and removed the resource from the list.
     **/
    inline bool removeResourceObject( RESOURCE_OBJECT Resource, bool remEmptyList );

    /**
     * \brief	Removes all registered resources one-by-one. For every removed 
     *          resource calls cleanResourceElement() function to perform additional
     *          cleanup job if needed.
     **/
    inline void removeAllResources( void );

//////////////////////////////////////////////////////////////////////////
// Protected methods.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Called when removing all resources to perform additional cleanups
     *          for every single resource object stored in the resource list.
     * \param	Key	    The Key value of resource.
     * \param	List    The resource list of objects that are going to be removed.
     **/
    inline void cleanResourceList( RESOURCE_KEY& Key, ResourceList& List );

    /**
     * \brief	Called when add a resource object into the resource list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void addResourceObject( ResourceList & List, RESOURCE_OBJECT Resource );

    /**
     * \brief	Called when removes a single resource object from the resource list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool removeResourceObject( ResourceList & List, RESOURCE_OBJECT Resource );

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object to synchronize access to resource data
     **/
    IEResourceLock &     mSynchObj;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TEResourceListMap( void ) = delete;
    TEResourceListMap( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> & /*src*/) = delete;
    TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> & operator = ( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> & /*src*/) = delete;
    TEResourceListMap( TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> && /*src*/ ) noexcept = delete;
    TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> & operator = ( TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Non-blocking Resource List Map. It is not thread safe and 
 *          not synchronizing thread access. Use this class template
 *          if there is no need to control thread access. For thread safe
 *          resource map use TELockResourceListMap class template.
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 *
 * \see    TENolockResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class ResourceList = TELinkedList<RESOURCE_OBJECT>
        , class HashMap      = TEHashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
class TENolockResourceListMap  : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TENolockResourceListMap( void );
    /**
     * \brief   Destructor
     **/
    ~TENolockResourceListMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of non-locking synchronization object.
     *          No thread locking will happen.
     **/
    NolockSynchObject mNoLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TENolockResourceListMap );
};

//////////////////////////////////////////////////////////////////////////
// TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Resource list map to use when there is no need to synchronize resource
 *          access between threads. 
 * 
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 *
 * \see    TELockResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class ResourceList = TELinkedList<RESOURCE_OBJECT>
        , class HashMap      = TEHashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
class TELockResourceListMap    : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TELockResourceListMap( void );
    /**
     * \brief   Destructor
     **/
    ~TELockResourceListMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of SpinLock to synchronize thread access.
     **/
    ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TELockResourceListMap );
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList> class template implementation
//////////////////////////////////////////////////////////////////////////

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::TEResourceListMap( IEResourceLock & synchObject )
    : HashMap   ( )
    , Tracker   ( )

    , mSynchObj ( synchObject )
{
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::lock( void ) const
{
    mSynchObj.lock( NECommon::WAIT_INFINITE );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::unlock( void ) const
{
    mSynchObj.unlock( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::tryLock( void ) const
{
    return mSynchObj.tryLock( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource )
{
    Lock lock( mSynchObj );

    ResourceList & resourceList = HashMap::operator[](Key);
    addResourceObject( resourceList, Resource );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::unregisterResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource, bool removeEmpty /*= true*/ )
{
    Lock lock( mSynchObj );

    typename HashMap::MAPPOS pos = HashMap::find(Key);
    if (HashMap::isValidPosition(pos))
    {
        ResourceList & resList = HashMap::valueAtPosition(pos);
        removeResourceObject( resList, Resource );
        if (removeEmpty && resList.isEmpty())
        {
            HashMap::removePosition(pos);
        }
    }
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList & TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::registerResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    return HashMap::getAt(Key);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::unregisterResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    ResourceList result;
    HashMap::removeAt( Key, result );
    return result;
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::findResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    typename HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::findResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    typename HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::getResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    typename HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::getResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    typename HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::removeResourceObject( RESOURCE_OBJECT Resource, bool remEmptyList )
{
    Lock lock( mSynchObj );

    bool result = false;
    for (typename HashMap::MAPPOS pos = HashMap::firstPosition( ); HashMap::isValidPosition(pos); )
    {
        ResourceList & list = HashMap::valueAtPosition( pos );
        if ( removeResourceObject(list, Resource) )
        {
            result = true;
        }

        if (remEmptyList && list.isEmpty())
        {
            pos = HashMap::removePosition(pos);
        }
        else
        {
            pos = HashMap::nextPosition( pos );
        }
    }

    return result;
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::removeAllResources( void )
{
    Lock lock( mSynchObj );

    for (typename HashMap::MAPPOS pos = HashMap::firstPosition( ); HashMap::isValidPosition(pos); pos = HashMap::nextPosition( pos ) )
    {
        cleanResourceList(pos->first, pos->second);
    }

    HashMap::clear( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline uint32_t TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::getSize( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::getSize( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::isEmpty( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::isEmpty( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::existResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );
    return HashMap::contains( Key );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::cleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
{
    Tracker::implCleanResourceList(Key, List);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::addResourceObject( ResourceList & List, RESOURCE_OBJECT Resource )
{
    Tracker::implAddResource(List, Resource);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::removeResourceObject( ResourceList & List, RESOURCE_OBJECT Resource )
{
    return Tracker::implRemoveResource(List, Resource);
}

//////////////////////////////////////////////////////////////////////////
// TENoLockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::TENolockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>( static_cast<IEResourceLock &>(mNoLock) )

    , mNoLock   ( )
{
}

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class ResourceList   /*= TELinkedList<RESOURCE_OBJECT>*/
         , class HashMap        /*= TEHashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>::TELockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, HashMap, Tracker>( static_cast<IEResourceLock &>(mLock) )

    , mLock ( )
{
}

#endif  // AREG_BASE_TERESOURCELISTMAP_HPP
