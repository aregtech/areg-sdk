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
#include "areg/base/GEGlobal.h"

#include "areg/base/Containers.hpp"
#include "areg/base/SyncPrimitives.hpp"

namespace areg
{
    /************************************************************************
     * Hierarchies. Following class are declared.
     ************************************************************************/
    // HashMap 
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class MapContainer, class Tracker> class ResourceListMapBase;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class MapContainer, class Tracker> class ConcurrentResourceListMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList, class MapContainer, class Tracker> class ResourceListMap;

    /************************************************************************
     * \brief   This file contains declarations of following class templates:
     *
     *              1.  ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>,
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
    // ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> class template declaration
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
     * \tparam  ResourceList        The name of class that contains list of resources.
     * \tparam  MapContainer        HashMap class to inherit to implement resource list map.
     * \tparam  Tracker             Helper class to track the resources when add, remove all clean resource object from the list.
     * 
     * \see    ConcurrentResourceListMap, ResourceListMap
     **/
    template <typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList = areg::LinkedList<RESOURCE_OBJECT>
            , class MapContainer = areg::HashMap<RESOURCE_KEY, ResourceList>
            , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
    class ResourceListMapBase   : protected MapContainer
                                , protected Tracker
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Initializes the locking object.
         **/
        explicit ResourceListMapBase( areg::Lockable & syncObject );

        /**
         * \brief   Destructor.
         **/
        ~ResourceListMapBase();

    //////////////////////////////////////////////////////////////////////////
    // Operations and attributes
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
        inline void lock() const;

        /**
         * \brief	Unlocks previously locked Resource List Map, so that other threads
         *          can have access to resource map data.
         **/
        inline void unlock() const;

        /**
         * \brief	Tries to lock Resource List Map. If succeeds, calling thread gets resource
         *          locking ownership and any other threads are blocked and cannot access
         *          Resource List Map data. Otherwise the calling thread is not blocked and 
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
         *          list if 'remEmptyList' parameter is true.
         * \param	Resource	    The resource object to lookup.
         * \param   remEmptyList    If true and the resource list is empty, removes the entry.
         * \return	Returns true if found and removed the resource from the list.
         **/
        inline bool removeResourceObject( RESOURCE_OBJECT Resource, bool remEmptyList );

        /**
         * \brief	Removes all registered resources one-by-one. For every removed 
         *          resource calls cleanResourceElement() function to perform additional
         *          cleanup job if needed.
         **/
        inline void removeAllResources();

        /**
         * \brief   Returns the vector object where the data are stored.
         **/
        inline const std::unordered_map<RESOURCE_KEY, ResourceList>& getData() const;

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
        areg::Lockable &     mSyncObj;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        ResourceListMapBase() = delete;
        ResourceListMapBase( const ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> & /*src*/) = delete;
        ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> & operator = ( const ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> & /*src*/) = delete;
        ResourceListMapBase( ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> && /*src*/ ) noexcept = delete;
        ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> & operator = ( ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> && /*src*/ ) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // ResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, ResourceList> class template declaration
    //////////////////////////////////////////////////////////////////////////

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
            , class ResourceList = areg::LinkedList<RESOURCE_OBJECT>
            , class MapContainer = areg::HashMap<RESOURCE_KEY, ResourceList>
            , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
    class ResourceListMap  : public ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        ResourceListMap();
        /**
         * \brief   Destructor
         **/
        ~ResourceListMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Instance of non-locking synchronization object.
         *          No thread locking will happen.
         **/
        areg::NolockSyncObject mNoLock;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( ResourceListMap );
    };

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker> class template declaration
    //////////////////////////////////////////////////////////////////////////
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
            , class ResourceList = areg::LinkedList<RESOURCE_OBJECT>
            , class MapContainer = areg::HashMap<RESOURCE_KEY, ResourceList>
            , class Tracker      = ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>>
    class ConcurrentResourceListMap    : public ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        ConcurrentResourceListMap();
        /**
         * \brief   Destructor
         **/
        ~ConcurrentResourceListMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Instance of SpinLock to synchronize thread access.
         **/
        areg::ResourceLock    mLock;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( ConcurrentResourceListMap );
    };

    //////////////////////////////////////////////////////////////////////////
    // Function implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, MapContainer, ResourceList> class template implementation
    //////////////////////////////////////////////////////////////////////////

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::ResourceListMapBase( areg::Lockable & syncObject )
        : MapContainer  ( )
        , Tracker       ( )

        , mSyncObj  ( syncObject )
    {
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::~ResourceListMapBase()
    {
        removeAllResources();
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::lock() const
    {
        mSyncObj.lock( WAIT_INFINITE );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::unlock() const
    {
        mSyncObj.unlock( );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::tryLock() const
    {
        return mSyncObj.tryLock( );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource )
    {
        areg::Lock lock( mSyncObj );

        ResourceList & resourceList = MapContainer::operator[](Key);
        addResourceObject( resourceList, Resource );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::unregisterResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT Resource, bool removeEmpty /*= true*/ )
    {
        areg::Lock lock( mSyncObj );

        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(Key);
        if (MapContainer::isValidPosition(pos))
        {
            ResourceList & resList = MapContainer::valueAtPosition(pos);
            removeResourceObject( resList, Resource );
            if (removeEmpty && resList.isEmpty())
            {
                MapContainer::removePosition(pos);
            }
        }
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline ResourceList & ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::registerResource( const RESOURCE_KEY & Key )
    {
        areg::Lock lock( mSyncObj );

        return MapContainer::getAt(Key);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline ResourceList ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::unregisterResource( const RESOURCE_KEY & Key )
    {
        areg::Lock lock( mSyncObj );

        ResourceList result;
        MapContainer::removeAt( Key, result );
        return result;
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::findResource( const RESOURCE_KEY & Key )
    {
        areg::Lock lock( mSyncObj );

        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(Key);
        return (MapContainer::isValidPosition(pos) ? &MapContainer::valueAtPosition(pos) : nullptr);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline const ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::findResource( const RESOURCE_KEY & Key ) const
    {
        areg::Lock lock( mSyncObj );

        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(Key);
        return (MapContainer::isValidPosition(pos) ? &MapContainer::valueAtPosition(pos) : nullptr);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::getResource( const RESOURCE_KEY & Key )
    {
        areg::Lock lock( mSyncObj );

        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(Key);
        return (MapContainer::isValidPosition(pos) ? &MapContainer::valueAtPosition(pos) : nullptr);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline const ResourceList * ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::getResource( const RESOURCE_KEY & Key ) const
    {
        areg::Lock lock( mSyncObj );

        typename MapContainer::MAPPOS pos = MapContainer::isEmpty() ? MapContainer::invalidPosition() : MapContainer::find(Key);
        return (MapContainer::isValidPosition(pos) ? &MapContainer::valueAtPosition(pos) : nullptr);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::removeResourceObject( RESOURCE_OBJECT Resource, bool remEmptyList )
    {
        areg::Lock lock( mSyncObj );

        bool result = false;
        for (typename MapContainer::MAPPOS pos = MapContainer::firstPosition( ); MapContainer::isValidPosition(pos); )
        {
            ResourceList & list = MapContainer::valueAtPosition( pos );
            if ( removeResourceObject(list, Resource) )
            {
                result = true;
            }

            if (remEmptyList && list.isEmpty())
            {
                pos = MapContainer::removePosition(pos);
            }
            else
            {
                pos = MapContainer::nextPosition( pos );
            }
        }

        return result;
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::removeAllResources()
    {
        areg::Lock lock( mSyncObj );

        for (typename MapContainer::MAPPOS pos = MapContainer::firstPosition( ); MapContainer::isValidPosition(pos); pos = MapContainer::nextPosition( pos ) )
        {
            cleanResourceList(MapContainer::keyAtPosition(pos), MapContainer::valueAtPosition(pos));
        }

        MapContainer::clear( );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline const std::unordered_map<RESOURCE_KEY, ResourceList>& ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::getData() const
    {
        return MapContainer::getData();
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline uint32_t ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::getSize() const
    {
        areg::Lock lock( mSyncObj );
        return MapContainer::getSize( );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::isEmpty() const
    {
        areg::Lock lock( mSyncObj );
        return MapContainer::isEmpty( );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::existResource( const RESOURCE_KEY & Key ) const
    {
        areg::Lock lock( mSyncObj );
        return MapContainer::contains( Key );
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::cleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
    {
        Tracker::implCleanResourceList(Key, List);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline void ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::addResourceObject( ResourceList & List, RESOURCE_OBJECT Resource )
    {
        Tracker::implAddResource(List, Resource);
    }

    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    inline bool ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::removeResourceObject( ResourceList & List, RESOURCE_OBJECT Resource )
    {
        return Tracker::implRemoveResource(List, Resource);
    }

    //////////////////////////////////////////////////////////////////////////
    // ResourceListMap class template implementation
    //////////////////////////////////////////////////////////////////////////
    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    ResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::ResourceListMap()
        : ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>( static_cast<areg::Lockable &>(mNoLock) )

        , mNoLock   ( )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentResourceListMap class template implementation
    //////////////////////////////////////////////////////////////////////////
    template < typename RESOURCE_KEY
            , typename RESOURCE_OBJECT
            , class ResourceList   /*= LinkedList<RESOURCE_OBJECT>*/
            , class MapContainer   /*= HashMap<RESOURCE_KEY, ResourceList>*/
            , class Tracker        /*= ResourceListMapImpl<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList>*/>
    ConcurrentResourceListMap<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>::ConcurrentResourceListMap()
        : ResourceListMapBase<RESOURCE_KEY, RESOURCE_OBJECT, ResourceList, MapContainer, Tracker>( static_cast<areg::Lockable &>(mLock) )

        , mLock ( )
    {
    }

} // namespace areg
#endif  // AREG_BASE_RESOURCELISTMAP_HPP
