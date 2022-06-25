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
 * \file        areg/base/TEResourceListMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Resource Container Map class template.
 *              Use to store resources associated by their Key.
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
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker> class TEResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker> class TELockResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker> class TENolockResourceListMap;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>,
 *                  which is a generic list of resource map class template;
 *
 *              2.  TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>,
 *                  which is a no thread safe list of resource map class template;
 *
 *              3.  TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>,
 *                  which is a thread safe list of resource map class template;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/

 //////////////////////////////////////////////////////////////////////////
 // TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap> class template declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief  This class is Hash Map container, used to keep control over registered 
  *         resources in the linked list (ResourceList). The resource can be 
  *         any type of object that are saved in the ResourceList and accessed by 
  *         unique resource key. The resource keys should have at least default 
  *         constructor and should be possible to convert to unsigned int.
  *         If converting to unsigned int requires special operation, the implementation
  *         should be done in a class that passed in HashMap of list.
  *         The resource map is thread safe if synchronization object provides
  *         locking mechanism. For additional information see TELockResourceListMap and
  *         TENolockResourceListMap class templates.
  *
  * \tparam  RESOURCE_KEY       The type of Key to access resource element
  * \tparam  RESOURCE_OBJECT    The type of resource objects. Pointer of  this defined types 
  *                             will be used and resource map.
  * \tparam  HashMap            HashMap class to inherit, which has possibility to calculate Hash 
  *                             value from RESOURCE_KEY type.
  * \tparam ResourceList        The name of class that contains list of resources.
  * \tparam Tracker             The name of class that tracks resources and contains implementation 
  *                             of methods called when add, remove and clean resource objects from the resource list.
  * \see    TELockResourceListMap, TENolockResourceListMap
  **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker = TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
class TEResourceListMap : protected HashMap
                        , protected Tracker
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor
     **/
    explicit TEResourceListMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor
     **/
    ~TEResourceListMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Locks the Resource Map, grants ownership to locking thread
     *          and blocks any other thread to access resource map data.
     * \return
     **/
    inline void lock( void ) const;

    /**
     * \brief	Unlocks previously locked Resource Map that other threads
     *          can have access to resource map data.
     * \return
     **/
    inline void unlock( void ) const;

    /**
     * \brief	Tries to lock Resource Map. If succeeds, calling thread
     *          gets resource locking ownership and any other threads will
     *          be blocked that do not access Resource Map data.
     *          The calling thread will not be blocked.
     *          Check return value to figure out whether calling thread
     *          locked resource or not.
     * \return	Returns true if calling thread locked resource.
     *          Otherwise returns false, but does not block thread execution.
     **/
    inline bool tryLock( void ) const;

    /**
     * \brief	Registers Resource in the map and returns true, 
     *          either is such resource already is registered (i.e. exists),
     *          or if successfully registered. If the unique Key of resource
     *          is registered, but the pointer of resources are different, returns false
     * \param	Key	        The unique Key of Resource object.
     * \param	resource	The pointer to Resource object. 
     *                      It should remain valid until is unregistered
     * \return	Returns true if such register resource already exists, or registered
     *          new resource. If the resource Key is already registered, but 
     *          resource objects are different, returns false.
     **/
    inline bool registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource );

    /**
     * \brief	Unregisters Resource from map and if succeeded, returns pointer
     *          to unregistered resource object.
     * \param	Key	    Unique Key of resource that was previously registered.
     * \return	If succeeded, returns pointer to unregistered resource.
     *          Otherwise returns nullptr.
     **/
    inline bool unregisterResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource, bool remEmptyList = true );

    /**
     * \brief   Registers resource in the map by given key. If the resources exists,
     *          it returns instance of existing resource object list.
     *          Otherwise, it creates new empty list.
     * \param   Key     The unique key of resource to register.
     * \return  If resource exists, returns instance of existing resource object list.
     *          Otherwise, creates new empty list of resource objects.
     **/
    inline ResourceList & registerResource( const RESOURCE_KEY & Key );

    /**
     * \brief   Unregister the resource in the map and returns the instance
     *          of registered resource objects list.
     * \param   Key     The unique Key of resource to unregister.
     * \return  Returns the list of registered resource objects lists.
     **/
    inline ResourceList unregisterResource( const RESOURCE_KEY & Key );

    /**
     * \brief	Search Resource in Resource Map by give Key
     *          and if found, return pointer to registered resource
     * \param	Key	    The unique Key of resource
     * \return	If could find Resource in Resource Map by unique Key,
     *          returns pointer to resource. Otherwise returns nullptr.
     **/
    inline ResourceList * findResource( const RESOURCE_KEY & Key );
    inline const ResourceList * findResource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Returns pointer to the resource list in the map.
     * \param   Key     The unique key to search resource list.
     * \return  If there is resource list registered by give key,
     *          it returns valid pointer to the resource object list.
     *          Otherwise, returns nullptr.
     **/
    inline ResourceList * getResource( const RESOURCE_KEY & Key );
    inline const ResourceList * getResource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief	Removes Resource from Resource map.
     *          The function will search resource by checking every entry
     *          and if finds any match, removes unique Key and resource from map
     * \param	Resource	The resource object to lookup
     * \return	If found and removed any such resource, returns true.
     *          Otherwise returns false.
     **/
    inline bool removeResourceObject( RESOURCE_OBJECT * Resource, bool remEmptyList );

    /**
     * \brief	Removes all registered resources one-by-one.
     *          For every removed resource calls CleanResourceElement() function
     *          if any additional job should be performed (for example, delete resources)
     **/
    inline void removeAllResources( void );

    /**
     * \brief	Returns the size of Resource Map
     **/
    inline int getSize( void ) const;

    /**
     * \brief   Returns true if resource map is empty.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief	Checks whether Resource Map contains entry
     *          with specified unique Key or not.
     * \param	Key	    The unique Key of Resource that was registered.
     * \return	If Resource with unique Key is registered, returns true.
     *          Otherwise returns false
     **/
    inline bool existResource( const RESOURCE_KEY & Key ) const;

//////////////////////////////////////////////////////////////////////////
// Protected methods.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from RemoveAllResources() for every single
     *          resource being unregistered.
     * \param	Key	    The Key value of resource.
     * \param	List    The list of resource objects.
     **/
    inline void cleanResourceList( RESOURCE_KEY & Key, ResourceList & List );

    /**
     * \brief	Called when need to add resource object to the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void addResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource );

    /**
     * \brief	Called when need to remove resource object from the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool removeResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource );

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
    TEResourceListMap( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> & /*src*/) = delete;
    TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> & operator = ( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> & /*src*/) = delete;
    TEResourceListMap( TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> && /*src*/ ) noexcept = delete;
    TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> & operator = ( TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Non-blocking Resource List Map. It is not thread safe and 
 *          not synchronizing thread access. Use this class template
 *          if there is no need to control thread access. For thread safe
 *          resource map use TELockResourceListMap class template.
 * \tparam  RESOURCE_KEY    The type of Key to access resource element.
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types are saved in the resource list of the map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 * \tparam  ResourceList    The name of class which is a resource list.
 * \tparam  Tracker         The name of class that tracks resources and contains implementation
 *                          of methods called when add, remove and clean resource objects from the resource list.
 * \see     TELockResourceListMap
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
class TENolockResourceListMap  : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>
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
// TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread safe Resource Map. It is synchronizing thread access. 
 *          Use this class template if resources are accessed by more than 
 *          one thread that requires synchronization.
 *          Use TENolockResourceListMap if you don't need synchronization and
 *          thread safety is not an issue. The NELockResourceListMap also works
 *          faster than the thread safe analogue.
 * \tparam  RESOURCE_KEY    The type of Key to access resource element.
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types are saved in the resource list of the map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 * \tparam  ResourceList    The name of class which is a resource list.
 * \tparam  Tracker         The name of class that tracks resources and contains implementation
 *                          of methods called when add, remove and clean resource objects from the resource list.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
class TELockResourceListMap    : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>
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

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::TEResourceListMap( IEResourceLock & synchObject )
    : HashMap   ( )
    , Tracker   ( )

    , mSynchObj ( synchObject )
{
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::lock( void ) const
{
    mSynchObj.lock( NECommon::WAIT_INFINITE );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::unlock( void ) const
{
    mSynchObj.unlock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::tryLock( void ) const
{
    return mSynchObj.tryLock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource )
{
    Lock lock( mSynchObj );

    ResourceList & resourceList = HashMap::operator[](Key);
    bool result = resourceList.isEmpty();
    addResourceObject( resourceList, Resource );
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::unregisterResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource, bool remEmptyList /*= true*/ )
{
    Lock lock( mSynchObj );

    bool result = false;
    HashMap::MAPPOS pos = HashMap::find(Key);
    if (HashMap::isValidPosition(pos))
    {
        ResourceList & resList = HashMap::valueAtPosition(pos);
        result = removeResourceObject( resList, Resource );
        if ( remEmptyList && resList.isEmpty())
        {
            HashMap::removePosition(pos);
        }
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline ResourceList & TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::registerResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    return HashMap::getAt(Key);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline ResourceList TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::unregisterResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    ResourceList result;
    HashMap::removeAt( Key, result );
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::findResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::findResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::getResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::getResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    HashMap::MAPPOS pos = HashMap::find( Key );
    return (HashMap::isValidPosition(pos) ? &HashMap::valueAtPosition(pos) : nullptr);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::removeResourceObject( RESOURCE_OBJECT * Resource, bool remEmptyList )
{
    Lock lock( mSynchObj );

    bool result = false;
    for (HashMap::MAPPOS pos = HashMap::firstPosition( ); HashMap::isValidPosition(pos); )
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

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::removeAllResources( void )
{
    Lock lock( mSynchObj );

    for (HashMap::MAPPOS pos = HashMap::firstPosition( ); HashMap::isValidPosition(pos); pos = HashMap::nextPosition( pos ) )
    {
        cleanResourceList(pos->first, pos->second);
    }

    HashMap::clear( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline int TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::getSize( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::getSize( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::isEmpty( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::isEmpty( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::existResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );
    return HashMap::contains( Key );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::cleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
{
    Tracker::implCleanResourceList(Key, List);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::addResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource )
{
    Tracker::implAddResource(List, Resource);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::removeResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource )
{
    return Tracker::implRemoveResource(List, Resource);
}

//////////////////////////////////////////////////////////////////////////
// TENoLockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::TENolockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>( static_cast<IEResourceLock &>(mNoLock) )

    , mNoLock   ( )
{
}

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Tracker>
TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>::TELockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Tracker>( static_cast<IEResourceLock &>(mLock) )

    , mLock ( )
{
}
