#ifndef AREG_BASE_RESOURCEMAP_HPP
#define AREG_BASE_RESOURCEMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ResourceMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Resource Container Map class template.
 *              Use to store resources associated with the Key. 
 *              Here are defined 2 types of resource map: locking and
 *              non-locking.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/TemplateBase.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/SyncPrimitives.hpp"

namespace areg {

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
// HashMap 
    template <typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , typename LOCKABLE
            , class MapContainer 
            , class Deleter      > class ResourceMapBase;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>,
 *                  which is a generic resource mapping class template;
 *
 *              2.  ConcurrentResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>,
 *                  which is a resource mapping class template, but thread safe.
 *
 *              3.  ResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>,
 *                  which is a resource mapping class template, but no thread safe;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/


//////////////////////////////////////////////////////////////////////////
// ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   ResourceListMapBase is a class template of Hash Map container used to keep
 *          control over registered resources. The resources are objects allocated
 *          in the memory, which should remain valid until they are unregistered.
 *          The registered resource objects are accessed by unique resource key.
 *          The resource keys should have at least default constructor and should have
 *          implemented hasher std::hash<KEY> to compute hash value. The resource map
 *          receives a synchronization object to lock and unlock resources during
 *          runtime if needed. For additional of locking resources see
 *          ConcurrentResourceMap and ResourceMap class templates.
 *
 *          See HashMap for details of computing hash value.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
 * \tparam  LOCKABLE          The type of synchronization object to use for locking the resource map.
 * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
 * \tparam  Deleter             Helper class to track the resource cleanup procedure.
 *
 * \see    ConcurrentResourceMap, ResourceMap
 **/
template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , typename LOCKABLE    = NolockSyncObject
         , class MapContainer   = HashMap<RESOURCE_KEY, RESOURCE_OBJECT>
         , class Deleter        = ResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
/**
 * \brief   Hash map container for managing registered resources with optional synchronization.
 *          Resources are accessed by unique keys and should remain valid until unregistered.
 **/
class ResourceMapBase   : protected MapContainer
                        , protected Deleter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    inline ResourceMapBase();

    ~ResourceMapBase() = default;

//////////////////////////////////////////////////////////////////////////
// Attributed and operations
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
     * \return  Returns true if the resource is registered with the specified key.
     **/
    [[nodiscard]]
    inline bool exist(const RESOURCE_KEY& Key) const;

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
     * \brief   Registers a resource object under the given key.
     *
     * \param   Key         The unique key for the resource.
     * \param   Resource    The resource object pointer, which must remain valid until unregistered.
     **/
    inline void register_resource_object( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

    /**
     * \brief   Unregisters the resource for the given key and returns the resource object pointer.
     *
     * \param   Key     The unique key of the resource to unregister.
     * \return  Returns the resource object pointer if found; nullptr otherwise.
     **/
    inline RESOURCE_OBJECT unregister_resource_object(const RESOURCE_KEY & Key);

    /**
     * \brief   Searches for a resource object by the given key.
     *
     * \param   Key     The unique key of the resource to find.
     * \return  Returns a pointer to the resource object if found; nullptr otherwise.
     **/
    [[nodiscard]]
    inline RESOURCE_OBJECT find_resource_object( const RESOURCE_KEY & Key ) const;

    /**
     * \brief   Removes the given resource object from the map by searching all entries.
     *
     * \param   Resource    The resource object to remove.
     * \return  Returns true if the resource was found and removed.
     **/
    inline bool remove_resource_object( RESOURCE_OBJECT Resource );

    /**
     * \brief   Removes all registered resources and calls clean_resource_entry for each one.
     **/
    inline void remove_all_resources();

    /**
     * \brief   Removes the first element from the resource map and returns it.
     *
     * \param[out] firstElement    On output, contains the key and object pair of the removed first
     *                             element.
     * \return  Returns true if an element was successfully removed.
     **/
    inline bool remove_first_element( std::pair<RESOURCE_KEY, RESOURCE_OBJECT> & firstElement );

    /**
     * \brief   Returns the resource object of the first entry and retrieves its associated key.
     *
     * \param[out] firstKey    On output, contains the key of the first resource if the map is not
     *                         empty.
     * \return  Returns a pointer to the first resource object.
     **/
    inline RESOURCE_OBJECT resource_first_key( RESOURCE_KEY & firstKey ) const;

    /**
     * \brief   Returns the resource object of the next entry after the specified key and updates
     *          the key parameter.
     *
     * \param[in,out] nextKey     On input, must be a valid key in the map. On output, contains the
     *                            key of the next element.
     * \return  Returns a pointer to the next resource object; nullptr if at the end.
     **/
    inline RESOURCE_OBJECT resource_next_key( RESOURCE_KEY & nextKey ) const;

    /**
     * \brief   Returns a const reference to the underlying unordered map data structure.
     **/
    [[nodiscard]]
    inline const std::unordered_map<RESOURCE_KEY, RESOURCE_OBJECT>& data() const noexcept;


//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Called when removing all resources. Override to perform cleanup for each resource.
     *
     * \param[in,out] Key         The key of the resource being removed.
     * \param[in,out] Resource    The resource object being removed.
     **/
    inline void clean_resource_entry( RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
private:
    mutable LOCKABLE mSyncObj;   //!< Synchronization object to synchronize access to resource data

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ResourceMapBase( const ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> & /*src*/) = delete;
    ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> & operator = ( const ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> & /*src*/ ) = delete;
    ResourceMapBase( ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> && /*src*/ ) noexcept = delete;
    ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> & operator = ( ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// ConcurrentResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Thread-safe resource map with blocking synchronization for multi-threaded access.
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class MapContainer = HashMap<RESOURCE_KEY, RESOURCE_OBJECT>
        , class Deleter = ResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
using ConcurrentResourceMap = ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceLock, MapContainer, Deleter>;

//////////////////////////////////////////////////////////////////////////
// ResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Non-thread-safe resource map for single-threaded use.
 **/
template <typename RESOURCE_KEY
        , typename RESOURCE_OBJECT
        , class MapContainer = HashMap<RESOURCE_KEY, RESOURCE_OBJECT>
        , class Deleter = ResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
using ResourceMap = ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, NolockSyncObject, MapContainer, Deleter>;

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::ResourceMapBase( )
    : MapContainer  ( )
    , Deleter       ( )
    , mSyncObj      ( )
{
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::register_resource_object(const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource)
{
    Lock lock(lockable());
    MapContainer::set_value_at(Key, Resource);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::unregister_resource_object(const RESOURCE_KEY & Key)
{
    Lock lock(lockable());
    RESOURCE_OBJECT result{ nullptr };
    MapContainer::remove_at(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::find_resource_object(const RESOURCE_KEY & Key) const
{
    Lock lock(lockable());
    if (MapContainer::is_empty())
        return nullptr;

    RESOURCE_OBJECT result{ nullptr };
    MapContainer::find(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::exist(const RESOURCE_KEY & Key) const
{
    Lock lock(lockable());
    return MapContainer::contains(Key);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::lock() const
{
    lockable().lock(areg::WAIT_INFINITE);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::unlock() const
{
    lockable().unlock();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::try_lock() const
{
    return lockable().try_lock();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline Lockable& ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::lockable() const noexcept
{
    return static_cast<Lockable &>(mSyncObj);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline uint32_t ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::size() const noexcept
{
    Lock lock(lockable());
    return MapContainer::size();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::is_empty() const noexcept
{
    Lock lock(lockable());
    return MapContainer::is_empty();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::remove_resource_object( RESOURCE_OBJECT Resource )
{
    Lock lock(lockable());

    bool result{ false };
    for ( auto pos = MapContainer::first_position(); pos != nullptr; pos = MapContainer::next_position(pos))
    {
        if ( Resource == MapContainer::value_at(pos) )
        {
            MapContainer::remove_at(pos);
            result = true;
            break;
        }
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::remove_all_resources()
{
    Lock lock(lockable());

    for ( auto pos = MapContainer::first_position(); MapContainer::is_valid_position(pos); pos = MapContainer::next_position(pos))
    {
        RESOURCE_KEY Key;
        RESOURCE_OBJECT Value{ nullptr };
        MapContainer::at_position(pos, Key, Value);
        clean_resource_entry(Key, Value);
    }

    MapContainer::clear();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::remove_first_element(std::pair<RESOURCE_KEY, RESOURCE_OBJECT> & firstElement )
{
    Lock lock(lockable());
    bool result{ false };
    typename MapContainer::MAPPOS pos  = MapContainer::first_position();
    if (MapContainer::is_valid_position(pos))
    {
        result = true;
        MapContainer::remove_at(pos, firstElement.first, firstElement.second);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::resource_first_key( RESOURCE_KEY & firstKey ) const
{
    Lock lock(lockable());

    RESOURCE_OBJECT result{ nullptr };
    typename MapContainer::MAPPOS pos = MapContainer::first_position();
    if (MapContainer::is_valid_position(pos))
    {
        MapContainer::at_position(pos, firstKey, result);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::resource_next_key( RESOURCE_KEY & nextKey ) const
{
    Lock lock(lockable());

    RESOURCE_OBJECT result{ nullptr };
    typename MapContainer::MAPPOS pos = MapContainer::is_empty() ? MapContainer::invalid_position() : MapContainer::find(nextKey);
    if (MapContainer::is_valid_position(pos))
    {
        MapContainer::next_entry(pos, nextKey, result);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline const std::unordered_map<RESOURCE_KEY, RESOURCE_OBJECT>& ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::data() const noexcept
{
    return MapContainer::data();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename LOCKABLE, class MapContainer, class Deleter>
inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, LOCKABLE, MapContainer, Deleter>::clean_resource_entry( RESOURCE_KEY & Key, RESOURCE_OBJECT Resource )
{
    if (MapContainer::is_empty())
        return;

    Deleter::impl_clean_resource(Key, Resource);
}

} // namespace areg
#endif  // AREG_BASE_RESOURCEMAP_HPP
