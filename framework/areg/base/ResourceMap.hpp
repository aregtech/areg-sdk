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

namespace areg
{
    /************************************************************************
     * Hierarchies. Following class are declared.
     ************************************************************************/
    // HashMap 
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter> class ResourceMapBase;
            template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter> class ConcurrentResourceMap;
            template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter> class ResourceMap;

    /************************************************************************
     * \brief   This file contains declarations of following class templates:
     *
     *              1.  ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>,
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
    // ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template declaration
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
     * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
     * \tparam  Deleter             Helper class to track the resource cleanup procedure.
     *
     * \see    ConcurrentResourceMap, ResourceMap
     **/
    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class MapContainer   = areg::HashMap<RESOURCE_KEY, RESOURCE_OBJECT>
            , class Deleter        = ResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
    class ResourceMapBase   : protected MapContainer
                            , protected Deleter
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Default Constructor
         **/
        explicit ResourceMapBase( areg::Lockable & syncObject );

        /**
         * \brief   Destructor
         **/
        ~ResourceMapBase() = default;

    //////////////////////////////////////////////////////////////////////////
    // Attributed and operations
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief	Returns the size of Resource Map
         **/
        inline uint32_t getSize() const;

        /**
         * \brief   Returns true if resource map is empty.
         **/
        inline bool isEmpty() const;

        /**
         * \brief	Checks whether Resource Map contains entry with specified unique Key or not.
         * 
         * \param	Key	    The unique Key of Resource that was registered.
         * \return	Returns true if Resource map has registered and object with specified unique key.
         *          Otherwise, returns false
         **/
        inline bool existResource(const RESOURCE_KEY& Key) const;

        /**
         * \brief	Locks the Resource Map, grants ownership to locking thread
         *          and blocks any other thread to access resource map data.
         **/
        inline void lock() const;

        /**
         * \brief	Unlocks previously locked Resource Map, so that other threads
         *          can have access to resource map data.
         **/
        inline void unlock() const;

        /**
         * \brief	Tries to lock Resource Map. If succeeds, calling thread gets resource
         *          locking ownership and any other threads are blocked and cannot access
         *          Resource Map data. Otherwise the calling thread is not blocked and
         *          does not get the ownership. Check return value to figure out whether
         *          calling have got resource ownership or not.
         * \return	Returns true if calling thread locked resource and have got the ownership.
         *          Otherwise,  returns false, but does not block thread execution.
         **/
        inline bool tryLock() const;

        /**
         * \brief	Registers resource object in the resource list of the map.
         * \param	Key	        The unique Key of Resource object.
         * \param	Resource	The pointer to Resource object, which should remain
         *                      valid until it is unregistered.
         **/
        inline void registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

        /**
         * \brief	Unregisters resource from the resource list of the map.
         * \param	Key	    The unique Key of resource that was previously registered.
         * \return	If succeeded, returns pointer to unregistered resource.
         *          Otherwise, returns nullptr.
         **/
        inline RESOURCE_OBJECT unregisterResourceObject(const RESOURCE_KEY & Key);

        /**
         * \brief	Search Resource object in Resource Map by the given unique Key
         *          and if found, returns valid pointer to registered resource object.
         * \param	Key	    The unique Key of resource.
         * \return	Returns valid pointer to the resource object if found a resource
         *          registered with the unique key. Otherwise, returns nullptr.
         **/
        inline RESOURCE_OBJECT findResourceObject( const RESOURCE_KEY & Key ) const;

        /**
         * \brief	Removes given Resource object from Resource Map and returns true 
         *          if operation succeeded. The function searches every entry in the map
         *          to find and remove the object from the map. To remove resource
         *          faster, unregister the resource with the unique key.
         * \param	Resource	The resource object to lookup and remove.
         * \return	If found and removed any such resource, returns true.
         *          Otherwise returns false.
         **/
        inline bool removeResourceObject( RESOURCE_OBJECT Resource );

        /**
         * \brief	Removes all registered resources one-by-one.
         *          For every removed resource calls cleanResourceEntry() function
         *          if any additional job should be performed (for example, delete resources)
         **/
        inline void removeAllResources();

        /**
         * \brief   Removes first element of Resource map and returns true if successfully removed.
         *          On output firstElement contains Resource Key and Object pair.
         * \param[out]  firstElement    On output, this will contain Key and Object pair 
         *                              of first element in resource map.
         * \return  Returns true if successfully removed first element.
         **/
        inline bool removeResourceFirstElement( std::pair<RESOURCE_KEY, RESOURCE_OBJECT> & firstElement );

        /**
         * \brief   Returns resource object of first object and the associated unique key in the map.
         * \param[out]  firstKey    On output, this parameter contains a resource valid key
         *                          if the resource is not empty.
         * \return  Returns pointer of stored Resource Object.
         **/
        inline RESOURCE_OBJECT resourceFirstKey( RESOURCE_KEY & firstKey ) const;

        /**
         * \brief	Returns resource object of next element stored after specified unique Key.
         *          On input, the parameter 'nextKey' should be valid resource key.
         *          On output, this parameter points to the next element in the resource map.
         *          To get first element key, call resourceFirstKey() method. Check return value
         *          to know whether there are more elements in the map or not.
         * 
         * \param[in,out]   nextKey     On input, this Key should be valid and point to the valid resource. 
         *                              On output, this parameter contain the key of the next element in the map.
         * \return	Returns valid pointer to the next registered resource object. Returns nullptr if
         *          reached end of the resource map.
         **/
        inline RESOURCE_OBJECT resourceNextKey( RESOURCE_KEY & nextKey ) const;

        /**
         * \brief   Returns the vector object where the data are stored.
         **/
        inline const std::unordered_map<RESOURCE_KEY, RESOURCE_OBJECT>& getData() const;


    //////////////////////////////////////////////////////////////////////////
    // Override operations
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief	Called when all resources are removed.
         *          This function is called from removeAllResources() for every single
         *          resource being unregistered. Override this function if any additional
         *          work should be performed after unregistering resource.
         * \param[in,out]   Key         The Key value of resource
         * \param[in,out]   Resource    Pointer to resource object
         **/
        inline void cleanResourceEntry( RESOURCE_KEY & Key, RESOURCE_OBJECT Resource );

    //////////////////////////////////////////////////////////////////////////
    // Member Variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Synchronization object to synchronize access to resource data
         **/
        areg::Lockable &    mSyncObj;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        ResourceMapBase() = delete;
        ResourceMapBase( const ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> & /*src*/) = delete;
        ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> & operator = ( const ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> & /*src*/ ) = delete;
        ResourceMapBase( ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> && /*src*/ ) noexcept = delete;
        ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> & operator = ( ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> && /*src*/ ) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief       Blocking Resource Map. It is thread safe and synchronizes thread access. 
     *              Use instance of this class template if resources are accessed
     *              by more than one thread.
     *
     * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
     * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
     * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
     * \tparam  Deleter             Helper class to track the resource cleanup procedure.
     *
     * \see    ConcurrentResourceMap, ResourceMap
     **/
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter = ResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
    class ConcurrentResourceMap    : public ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        ConcurrentResourceMap();
        /**
         * \brief   Destructor
         **/
        ~ConcurrentResourceMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Instance of ResourceLock to synchronize thread access.
         **/
        areg::ResourceLock    mLock;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( ConcurrentResourceMap );
    };

    //////////////////////////////////////////////////////////////////////////
    // ResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief       Non-blocking Resource Map. It is not thread safe. Use instance 
     *              of this class if there is no need to control thread access.
     *
     * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
     * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
     * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
     * \tparam  Deleter             Helper class to track the resource cleanup procedure.
     *
     * \see    ConcurrentResourceMap
     **/
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    class ResourceMap  : public ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        ResourceMap();
        /**
         * \brief   Destructor
         **/
        ~ResourceMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Instance of non-locking synchronization object. No thread locking will happen.
         **/
        areg::NolockSyncObject mNoLock;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( ResourceMap );
    };

    //////////////////////////////////////////////////////////////////////////
    // Function implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter> class template implementation
    //////////////////////////////////////////////////////////////////////////

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::ResourceMapBase( areg::Lockable & syncObject )
        : MapContainer  ( )
        , Deleter       ( )
        , mSyncObj  (syncObject)
    {
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::registerResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource)
    {
        areg::Lock lock(mSyncObj);
        MapContainer::setAt(Key, Resource);
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::unregisterResourceObject(const RESOURCE_KEY & Key)
    {
        areg::Lock lock(mSyncObj);

        RESOURCE_OBJECT result{ nullptr };
        MapContainer::removeAt(Key, result);
        return result;
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::findResourceObject(const RESOURCE_KEY & Key) const
    {
        areg::Lock lock(mSyncObj);

        RESOURCE_OBJECT result{ nullptr };
        if (MapContainer::isEmpty())
            return result;

        MapContainer::find(Key, result);
        return result;
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::existResource(const RESOURCE_KEY & Key) const
    {
        areg::Lock lock(mSyncObj);
        return MapContainer::contains(Key);
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::lock() const
    {
        mSyncObj.lock(WAIT_INFINITE);
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::unlock() const
    {
        mSyncObj.unlock( );
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::tryLock() const
    {
        return mSyncObj.tryLock( );
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline uint32_t ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::getSize() const
    {
        areg::Lock lock(mSyncObj);
        return MapContainer::getSize();
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::isEmpty() const
    {
        areg::Lock lock(mSyncObj);
        return MapContainer::isEmpty();
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::removeResourceObject( RESOURCE_OBJECT Resource )
    {
        areg::Lock lock(mSyncObj);

        bool result{ false };
        for ( auto pos = MapContainer::firstPosition(); pos != nullptr; pos = MapContainer::nextPosition(pos))
        {
            if ( Resource == MapContainer::valueAtPosition(pos) )
            {
                MapContainer::removePosition(pos);
                result = true;
                break;
            }
        }

        return result;
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::removeAllResources()
    {
        areg::Lock lock(mSyncObj);

        for ( auto pos = MapContainer::firstPosition(); MapContainer::isValidPosition(pos); pos = MapContainer::nextPosition(pos))
        {
            RESOURCE_KEY Key;
            RESOURCE_OBJECT Value{ nullptr };
            MapContainer::getAtPosition(pos, Key, Value);
            cleanResourceEntry(Key, Value);
        }

        MapContainer::clear();
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline bool ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::removeResourceFirstElement(std::pair<RESOURCE_KEY, RESOURCE_OBJECT> & firstElement )
    {
        areg::Lock lock(mSyncObj);
        bool result{ false };
        typename MapContainer::MAPPOS pos  = MapContainer::firstPosition();
        if (MapContainer::isValidPosition(pos))
        {
            result = true;
            MapContainer::removePosition(pos, firstElement.first, firstElement.second);
        }

        return result;
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::resourceFirstKey( RESOURCE_KEY & firstKey ) const
    {
        areg::Lock lock(mSyncObj);

        RESOURCE_OBJECT result{ nullptr };
        typename MapContainer::MAPPOS pos = MapContainer::firstPosition();
        if (MapContainer::isValidPosition(pos))
        {
            MapContainer::getAtPosition(pos, firstKey, result);
        }

        return result;
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline RESOURCE_OBJECT ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::resourceNextKey( RESOURCE_KEY & nextKey ) const
    {
        areg::Lock lock(mSyncObj);

        RESOURCE_OBJECT result{ nullptr };
        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(nextKey);
        if (MapContainer::isValidPosition(pos))
        {
            MapContainer::nextEntry(pos, nextKey, result);
        }

        return result;
    }

    template<typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline const std::unordered_map<RESOURCE_KEY, RESOURCE_OBJECT>& ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::getData() const
    {
        return MapContainer::getData();
    }

    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    inline void ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::cleanResourceEntry( RESOURCE_KEY & Key, RESOURCE_OBJECT Resource )
    {
        Deleter::implCleanResource(Key, Resource);
    }

    //////////////////////////////////////////////////////////////////////////
    // ResourceMap class template implementation
    //////////////////////////////////////////////////////////////////////////
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    ResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::ResourceMap()
        : ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>(static_cast<areg::Lockable &>(mNoLock))
        , mNoLock   ()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentResourceMap class template implementation
    //////////////////////////////////////////////////////////////////////////
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class MapContainer, class Deleter>
    ConcurrentResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>::ConcurrentResourceMap()
        : ResourceMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, Deleter>(static_cast<areg::Lockable &>(mLock))
        , mLock ( )
    {
    }

} // namespace areg
#endif  // AREG_BASE_RESOURCEMAP_HPP
