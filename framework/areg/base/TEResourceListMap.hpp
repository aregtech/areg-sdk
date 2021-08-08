#ifndef AREG_BASE_TERESOURCELISTMAP_HPP
#define AREG_BASE_TERESOURCELISTMAP_HPP
/************************************************************************
 * \file        areg/base/TEResourceListMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Resource Container Map class template.
 *              Use to store resources associated by their Key.
 *              Here are defined 2 types of resource map: locking and
 *              non-locking.
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/ETemplateBase.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/ESynchObjects.hpp"

  /************************************************************************
   * Hierarchies. Following class are declared.
   ************************************************************************/
   // HashMap 
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement> class TEResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement> class TELockResourceListMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement> class TENolockResourceListMap;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>,
 *                  which is a generic list of resource map class template;
 *
 *              2.  TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>,
 *                  which is a no thread safe list of resource map class template;
 *
 *              3.  TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>,
 *                  which is a thread safe list of resource map class template;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/

 //////////////////////////////////////////////////////////////////////////
 // TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap> class template declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief      TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>
  *             This class is Hash Map container, used to keep control over
  *             registered resources. The resource can be any type of object
  *             that are saved in the ResourceList and accessed by unique resource key.
  *             Every registered resource should have unique Key to ensure
  *             accessing resource by referring to unique Key.
  *             The resource objects are stored in the resource list.
  *
  * \tparam  RESOURCE_KEY       The type of Key to access resource element
  * \tparam  RESOURCE_OBJECT    The type of resource objects. Pointer of
  *                             this defined types will be used and resource map.
  * \tparam  HashMap            HashMap class to inherit, which has possibility
  *                             to calculate Hash value from RESOURCE_KEY type.
  * \tparam ResourceList        The name of class that contains list of resources.
  * \tparam Implement           The name of class that contains implementation of methods
  *                             when need to clean resources, add and remove resource objects
  *                             to the resource list.
  **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement = TEResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
class TEResourceListMap : protected HashMap
                        , protected Implement
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor
     **/
    TEResourceListMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor
     **/
    ~TEResourceListMap( void );

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
     *          Otherwise returns NULL
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
     *          returns pointer to resource. Otherwise returns NULL.
     **/
    inline ResourceList * findResource( const RESOURCE_KEY & Key );
    inline const ResourceList * findResource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Returns pointer to the resource list in the map.
     * \param   Key     The unique key to search resource list.
     * \return  If there is resource list registered by give key,
     *          it returns valid pointer to the resource object list.
     *          Otherwise, returns NULL.
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
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TEResourceListMap( void );
    TEResourceListMap( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> & );
    const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> & operator = ( const TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> & );
};

//////////////////////////////////////////////////////////////////////////
// TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief       TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>
 *              Non-blocking Resource List Map. It is not synchronizing
 *              thread access and not thread safe. Use instance of this class template
 *              if there is no need to control thread access.
 * \tparam  RESOURCE_KEY    The type of Key to access resource element.
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types are saved in the resource list of the map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 * \tparam  ResourceList    The name of class which is a resource list.
 * \tparam  Implement       The name of class that contains implementation of 
 *                          customized methods to add, remove and clean resource 
 *                          specific objects.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
class TENolockResourceListMap  : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>
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
    ~TENolockResourceListMap( void );

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
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TENolockResourceListMap(const TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> &);
    const TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> & operator = (const TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> &);
};

//////////////////////////////////////////////////////////////////////////
// TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Blocking Resource Map. It is synchronizing thread access. 
 *              Use instance of this class template if resources are accessed
 *              by more than one thread. It inherits from TEResourceMap and contains
 *              instance of IEResourceLock object to synchronize thread
 *              access
 * \tparam  RESOURCE_KEY    The type of Key to access resource element
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types will be used and resource map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 **/
/**
 * \brief       TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>
 *              Blocking Resource List Map. It is synchronizing
 *              thread access and it is thread safe. Use instance of this class template
 *              if there is a need to control thread access.
 * \tparam  RESOURCE_KEY    The type of Key to access resource element.
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types are saved in the resource list of the map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 * \tparam  ResourceList    The name of class which is a resource list.
 * \tparam  Implement       The name of class that contains implementation of 
 *                          customized methods to add, remove and clean resource 
 *                          specific objects.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
class TELockResourceListMap    : public TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>
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
    ~TELockResourceListMap( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of Critical Section to synchronize thread access.
     **/
    ResourceLock  mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TELockResourceListMap(const TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> &);
    const TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>& operator = (const TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement> &);
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::TEResourceListMap( IEResourceLock & synchObject )
    : HashMap       ( )
    , Implement     ( )

    , mSynchObj     ( synchObject )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::~TEResourceListMap( void )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::lock( void ) const
{
    mSynchObj.lock( IESynchObject::WAIT_INFINITE );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::unlock( void ) const
{
    mSynchObj.unlock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::tryLock( void ) const
{
    return mSynchObj.tryLock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource )
{
    Lock lock( mSynchObj );

    ResourceList & resourceList = HashMap::getAt(Key);
    bool result = resourceList.isEmpty();
    addResourceObject( resourceList, Resource );
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::unregisterResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource, bool remEmptyList /*= true*/ )
{
    Lock lock( mSynchObj );

    bool result = false;
    MAPPOS pos = HashMap::find(Key);
    if ( pos != NULL )
    {
        ResourceList & resList = HashMap::getPosition(pos);
        result = removeResourceObject( resList, Resource );
        if ( remEmptyList && resList.isEmpty())
        {
            HashMap::removePosition(pos);
        }
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline ResourceList & TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::registerResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    return HashMap::getAt(Key);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline ResourceList TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::unregisterResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    ResourceList result;
    HashMap::removeAt( Key, result );
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::findResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    MAPPOS pos = HashMap::find( Key );
    return (pos != NULL ? &HashMap::getPosition(pos) : NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::findResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    MAPPOS pos = HashMap::find( Key );
    return (pos != NULL ? &HashMap::getPosition(pos) : NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::getResource( const RESOURCE_KEY & Key )
{
    Lock lock( mSynchObj );

    MAPPOS pos = HashMap::find( Key );
    return (pos != NULL ? &HashMap::getPosition(pos) : NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline const ResourceList * TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::getResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );

    MAPPOS pos = HashMap::find( Key );
    return (pos != NULL ? &HashMap::getPosition(pos) : NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::removeResourceObject( RESOURCE_OBJECT * Resource, bool remEmptyList )
{
    Lock lock( mSynchObj );

    bool removed = false;
    for ( MAPPOS pos = HashMap::firstPosition( ); pos != NULL; )
    {
        ResourceList & list = HashMap::getPosition( pos );
        if ( removeResourceObject(list, Resource) )
        {
            removed = true;
        }

        if (remEmptyList && list.isEmpty())
        {
            RESOURCE_KEY key;
            pos = HashMap::removePosition(pos, key, list);
        }
        else
        {
            pos = HashMap::nextPosition( pos );
        }
    }

    return removed;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::removeAllResources( void )
{
    Lock lock( mSynchObj );

    for ( MAPPOS pos = HashMap::firstPosition( ); pos != NULL; pos = HashMap::nextPosition( pos ) )
    {
        typename HashMap::Block * block = HashMap::blockAt( pos );
        if (block != NULL)
        {
            cleanResourceList( block->mKey, block->mValue );
        }
    }

    HashMap::removeAll( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline int TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::getSize( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::getSize( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::isEmpty( void ) const
{
    Lock lock( mSynchObj );
    return HashMap::isEmpty( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::existResource( const RESOURCE_KEY & Key ) const
{
    Lock lock( mSynchObj );
    return (HashMap::find( Key ) != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::cleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
{
    Implement::implCleanResourceList(Key, List);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline void TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::addResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource )
{
    Implement::implAddResource(List, Resource);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
inline bool TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::removeResourceObject( ResourceList & List, RESOURCE_OBJECT * Resource )
{
    return Implement::implRemoveResource(List, Resource);
}

//////////////////////////////////////////////////////////////////////////
// TENoLockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::TENolockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>( static_cast<IEResourceLock &>(mNoLock) )

    , mNoLock   ( )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TENolockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::~TENolockResourceListMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::TELockResourceListMap( void )
    : TEResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>( static_cast<IEResourceLock &>(mLock) )

    , mLock ( false )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class ResourceList, class Implement>
TELockResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, ResourceList, Implement>::~TELockResourceListMap( void )
{
    ; // do nothing
}

#endif  // AREG_BASE_TERESOURCELISTMAP_HPP
