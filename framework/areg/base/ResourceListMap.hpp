#ifndef AREG_BASE_RESOURCELISTMAP_HPP
#define AREG_BASE_RESOURCELISTMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ResourceListMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Resource List Container Map class template.
 *              Use to store list of resources associated with the Key.
 *              Here are defined 2 types of resource map:
 *              -   Blocking (thread safe);
 *              -   Non-blocking (no thread safe).
 *
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/Containers.hpp"
#include "areg/base/SyncPrimitives.hpp"
namespace areg {

  /************************************************************************
   * Hierarchies. Following class are declared.
   ************************************************************************/
   // HashMap 
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class ResourceList, class MapContainer, class Tracker> class ResourceListMapBase;
/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>,
 *                  which is a generic list of resource map class template;
 *
 *              2.  ResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>,
 *                  which is a no thread safe list of resource map class template;
 *
 *              3.  ConcurrentResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>,
 *                  which is a thread safe list of resource map class template;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/


//////////////////////////////////////////////////////////////////////////
// ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   ResourceListMapBase is a class template of Hash Map container used to keep
 *          control over registered resources in the linked list (ResourceList).
 *          The resources are objects allocated in the memory and saved in the 
 *          ResourceList, which are accessed by unique resource key. This means that
 *          the ResourceList is a container of pointers to the resource objects. 
 *          The resource keys should have at least default constructor and should have
 *          implemented hasher std::hash<KEY> to compute hash value. The resource map
 *          receives a synchronization object to lock and unlock resources during
 *          runtime if needed. For additional of locking resources see 
 *          ConcurrentResourceListMap and ResourceListMap class templates.
 * 
 *          See MapContainer for details of computing hash value.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  LOCKABLE            The type of synchronization object to use for locking the resource map.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 * 
 * \see    ConcurrentResourceListMap, ResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , typename LOCKABLE  = NolockSyncObject
        , class ResourceList = LinkedList<RESOURCE_OBJECT>
        , class MapContainer = HashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
/**
 * \brief   Hash map container for managing registered resources with thread synchronization
 *          support. Resources are accessed by unique keys and remain valid until explicitly
 *          unregistered.
 **/
class ResourceListMapBase   : protected MapContainer
                            , protected Tracker
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    inline ResourceListMapBase();

    inline ~ResourceListMapBase();

//////////////////////////////////////////////////////////////////////////
// Operations and attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the number of resources in the map.
     **/
    [[nodiscard]]
    inline uint32_t size() const noexcept;

    /**
     * \brief   Returns true if the resource map is empty.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Checks whether a resource with the specified key is registered.
     *
     * \param   Key     The unique key of the resource to check.
     * \return  Returns true if a resource with the specified key is registered.
     **/
    [[nodiscard]]
    inline bool exist(const RESOURCE_KEY & Key) const noexcept;

    /**
     * \brief   Locks the resource map to block other threads from accessing it.
     **/
    inline void lock() const;

    /**
     * \brief   Unlocks the resource map to allow other threads to access it.
     **/
    inline void unlock() const;

    /**
     * \brief   Attempts to lock the resource map without blocking. Returns immediately whether the
     *          lock was successful.
     *
     * \return  Returns true if the lock was successfully acquired; false otherwise without
     *          blocking.
     **/
    inline bool try_lock() const;

    /**
     * \brief   Returns a reference to the synchronization object used for locking the resource map.
     **/
    [[nodiscard]]
    inline Lockable& lockable() const noexcept;

    /**
     * \brief   Registers a resource object in the resource list associated with the given key.
     *
     * \param   Key         The unique key for the resource.
     * \param   Resource    The resource object pointer, which must remain valid until unregistered.
     **/
    inline void register_resource_object( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

    /**
     * \brief   Unregisters a resource object from the resource list associated with the given key.
     *
     * \param   Key             The unique key of the registered resource.
     * \param   Resource        The resource object pointer to unregister.
     * \param   removeEmpty     If true, removes the key entry when the resource list becomes empty;
     *                          otherwise keeps the empty list.
     **/
    inline void unregister_resource_object(const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource, bool removeEmpty = true );

    /**
     * \brief   Registers or retrieves the resource list for the given key. Returns the existing
     *          list if the key exists; otherwise creates and returns a new empty list.
     *
     * \param   Key     The unique key of the resource to register.
     * \return  Returns a reference to the resource list associated with the key.
     * \note    This method may create a new entry if the key does not exist, so it should be used with caution.
     *          If you want to only retrieve without creating, use find_resource() instead.
     **/
    [[nodiscard]]
    inline ResourceList& resource_at( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Removes the resource list for the given key and returns a copy. Returns an empty
     *          list if the key does not exist.
     *
     * \param   Key     The unique key of the resource to unregister.
     * \return  Returns a copy of the resource list that was removed, or an empty list if not found.
     **/
    inline ResourceList unregister_resource( const RESOURCE_KEY & Key );

    /**
     * \brief   Searches for the resource list by the given key.
     *
     * \param   Key     The unique key that specifies the resource list.
     * \return  Returns a pointer to the resource list if found; nullptr otherwise.
     **/
    inline ResourceList * find_resource( const RESOURCE_KEY & Key );
    /**
     * \brief   Searches for the resource list by the given key (const overload).
     *
     * \param   Key     The unique key that specifies the resource list.
     * \return  Returns a const pointer to the resource list if found; nullptr otherwise.
     **/
    inline const ResourceList * find_resource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Returns a pointer to the resource list for the given key.
     *
     * \param   Key     The unique key that specifies the resource list.
     * \return  Returns a pointer to the resource list; nullptr if not found.
     **/
    inline ResourceList * resource( const RESOURCE_KEY & Key );
    /**
     * \brief   Returns a const pointer to the resource list for the given key.
     *
     * \param   Key     The unique key that specifies the resource list.
     * \return  Returns a const pointer to the resource list; nullptr if not found.
     **/
    inline const ResourceList * resource( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Removes a resource object from all resource lists by searching each entry.
     *          Optionally removes empty resource lists.
     *
     * \param   Resource        The resource object pointer to remove.
     * \param   remEmptyList    If true, removes the key entry when its resource list becomes empty.
     * \return  Returns true if the resource was found and removed.
     **/
    inline bool remove_resource_object( RESOURCE_OBJECT Resource, bool remEmptyList );

    /**
     * \brief   Removes all registered resources and calls clean_resource_list for each one.
     **/
    inline void remove_all_resources();

    /**
     * \brief   Returns a const reference to the underlying unordered map data structure.
     **/
    [[nodiscard]]
    inline const std::unordered_map<RESOURCE_KEY, ResourceList>& data() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Protected methods.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Called when removing all resources to perform additional cleanup for each resource
     *          list.
     *
     * \param   Key     The key value of the resource being removed.
     * \param   List    The resource list of objects being removed.
     **/
    inline void clean_resource_list( RESOURCE_KEY& Key, ResourceList& List );

    /**
     * \brief   Adds a resource object to a resource list.
     *
     * \param   List        The resource list to add the object to.
     * \param   Resource    The resource object to add.
     **/
    inline void add_resource_object( ResourceList & List, RESOURCE_OBJECT Resource );

    /**
     * \brief   Removes a resource object from a resource list.
     *
     * \param   List        The resource list to remove the object from.
     * \param   Resource    The resource object to remove.
     * \return  Returns true if the resource was found and removed.
     **/
    inline bool remove_resource_object( ResourceList & List, RESOURCE_OBJECT Resource );

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object to synchronize access to resource data
     **/
    mutable LOCKABLE    mSyncObj;

//////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ResourceListMapBase( const ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> & /*src*/) = delete;
    ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> & operator = ( const ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> & /*src*/) = delete;
    ResourceListMapBase( ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> && /*src*/ ) noexcept = delete;
    ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> & operator = ( ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker> && /*src*/ ) noexcept = delete;
};

/**
 * \brief   Non-blocking Resource List Map. It is not thread safe and 
 *          not synchronizing thread access. Use this class template
 *          if there is no need to control thread access. For thread safe
 *          resource map use ConcurrentResourceListMap class template.
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 *
 * \see    ResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class ResourceList = LinkedList<RESOURCE_OBJECT>
        , class MapContainer = HashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
using ResourceListMap  = ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, NolockSyncObject, ResourceList, MapContainer, Tracker>;

/**
 * \brief   Resource list map to use when there is no need to synchronize resource
 *          access between threads. 
 * 
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the list.
 * \tparam  ResourceList        The name of class that contains list of resources.
 * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
 * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
 *
 * \see    ConcurrentResourceListMap
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class ResourceList = LinkedList<RESOURCE_OBJECT>
        , class MapContainer = HashMap<RESOURCE_KEY, ResourceList>
        , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
using ConcurrentResourceListMap    = ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceLock, ResourceList, MapContainer, Tracker>;

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, ResourceList> class template implementation
//////////////////////////////////////////////////////////////////////////

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::ResourceListMapBase()
    : MapContainer  ( )
    , Tracker       ( )

    , mSyncObj      ( )
{
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::~ResourceListMapBase()
{
    remove_all_resources();
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::lock() const
{
    lockable().lock(areg::WAIT_INFINITE);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::unlock() const
{
    lockable().unlock( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::try_lock() const
{
    return lockable().try_lock( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline Lockable& ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::lockable() const noexcept
{
    return static_cast<Lockable &>(mSyncObj);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::register_resource_object( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource )
{
    Lock lock( lockable() );

    ResourceList & resourceList = MapContainer::operator[](Key);
    add_resource_object( resourceList, Resource );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::unregister_resource_object( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource, bool removeEmpty /*= true*/ )
{
    Lock lock( lockable() );

    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(Key);
    if (MapContainer::is_valid_position(pos))
    {
        ResourceList & resList = MapContainer::value_at(pos);
        remove_resource_object( resList, Resource );
        if (removeEmpty && resList.is_empty())
        {
            MapContainer::remove_at(pos);
        }
    }
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList& ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::resource_at( const RESOURCE_KEY & Key ) const
{
    Lock lock( lockable() );

    return MapContainer::value_at(Key);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::unregister_resource( const RESOURCE_KEY & Key )
{
    Lock lock( lockable() );

    ResourceList result;
    MapContainer::remove_at( Key, result );
    return result;
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::find_resource( const RESOURCE_KEY & Key )
{
    Lock lock( lockable() );

    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(Key);
    return (MapContainer::is_valid_position(pos) ? &MapContainer::value_at(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline const ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::find_resource( const RESOURCE_KEY & Key ) const
{
    Lock lock( lockable() );

    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(Key);
    return (MapContainer::is_valid_position(pos) ? &MapContainer::value_at(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::resource( const RESOURCE_KEY & Key )
{
    Lock lock( lockable() );

    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(Key);
    return (MapContainer::is_valid_position(pos) ? &MapContainer::value_at(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline const ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::resource( const RESOURCE_KEY & Key ) const
{
    Lock lock( lockable() );

    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(Key);
    return (MapContainer::is_valid_position(pos) ? &MapContainer::value_at(pos) : nullptr);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::remove_resource_object( RESOURCE_OBJECT Resource, bool remEmptyList )
{
    Lock lock( lockable() );

    bool result = false;
    for (typename MapContainer::MAPPOS pos = MapContainer::first_position( ); MapContainer::is_valid_position(pos); )
    {
        ResourceList & list = MapContainer::value_at( pos );
        if ( remove_resource_object(list, Resource) )
        {
            result = true;
        }

        if (remEmptyList && list.is_empty())
        {
            pos = MapContainer::remove_at(pos);
        }
        else
        {
            pos = MapContainer::next_position( pos );
        }
    }

    return result;
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::remove_all_resources()
{
    Lock lock( lockable() );

    for (typename MapContainer::MAPPOS pos = MapContainer::first_position( ); MapContainer::is_valid_position(pos); pos = MapContainer::next_position( pos ) )
    {
        clean_resource_list(MapContainer::key_at(pos), MapContainer::value_at(pos));
    }

    MapContainer::clear( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline const std::unordered_map<RESOURCE_KEY, ResourceList>& ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::data() const noexcept
{
    return MapContainer::data();
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline uint32_t ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::size() const noexcept
{
    Lock lock( lockable() );
    return MapContainer::size( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::is_empty() const noexcept
{
    Lock lock( lockable() );
    return MapContainer::is_empty( );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::exist( const RESOURCE_KEY & Key ) const noexcept
{
    Lock lock( lockable() );
    return MapContainer::contains( Key );
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::clean_resource_list( RESOURCE_KEY & Key, ResourceList & List )
{
    Tracker::impl_clean_list(Key, List);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::add_resource_object( ResourceList & List, RESOURCE_OBJECT Resource )
{
    Tracker::impl_add_resource(List, Resource);
}

template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    /*= NolockSyncObject */
         , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
         , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
         , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, ResourceList, MapContainer, Tracker>::remove_resource_object( ResourceList & List, RESOURCE_OBJECT Resource )
{
    return Tracker::impl_remove_resource(List, Resource);
}

} // namespace areg
#endif  // AREG_BASE_RESOURCELISTMAP_HPP
