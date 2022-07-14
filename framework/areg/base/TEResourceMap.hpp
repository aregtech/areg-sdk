#ifndef AREG_BASE_TERESOURCEMAP_HPP
#define AREG_BASE_TERESOURCEMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/TEResourceMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Resource Container Map class template.
 *              Use to store resources associated with the Key. 
 *              Here are defined 2 types of resource map: locking and
 *              non-locking.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/TETemplateBase.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/SynchObjects.hpp"

/************************************************************************
 * Hierarchies. Following class are declared.
 ************************************************************************/
// HashMap 
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter> class TEResourceMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter> class TELockResourceMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter> class TENolockResourceMap;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>,
 *                  which is a generic resource mapping class template;
 *
 *              2.  TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>,
 *                  which is a resource mapping class template, but thread safe.
 *
 *              3.  TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>,
 *                  which is a resource mapping class template, but no thread safe;
 *
 *          For more information, see descriptions bellow
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TEResourceListMap is a class template of Hash Map container used to keep
 *          control over registered resources. The resources are objects allocated
 *          in the memory, which should remain valid until they are unregistered.
 *          The registered resource objects are accessed by unique resource key.
 *          The resource keys should have at least default constructor and should have
 *          implemented hasher std::hash<KEY> to compute hash value. The resource map
 *          receives a synchronization object to lock and unlock resources during
 *          runtime if needed. For additional of locking resources see
 *          TELockResourceMap and TENolockResourceMap class templates.
 *
 *          See TEHashMap for details of computing hash value.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Deleter             Helper class to track the resource cleanup procedure.
 *
 * \see    TELockResourceMap, TENolockResourceMap
 **/
template < typename RESOURCE_KEY
         , typename RESOURCE_OBJECT
         , class HashMap = TEHashMap<RESOURCE_KEY, RESOURCE_OBJECT *>
         , class Deleter = TEResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
class TEResourceMap     : protected HashMap
                        , protected Deleter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default Constructor
     **/
    explicit TEResourceMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor
     **/
    ~TEResourceMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributed and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Returns the size of Resource Map
     **/
    inline int getSize(void) const;

    /**
     * \brief   Returns true if resource map is empty.
     **/
    inline bool isEmpty(void) const;

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
    inline void lock( void ) const;

    /**
     * \brief	Unlocks previously locked Resource Map, so that other threads
     *          can have access to resource map data.
     **/
    inline void unlock( void ) const;

    /**
     * \brief	Tries to lock Resource Map. If succeeds, calling thread gets resource
     *          locking ownership and any other threads are blocked and cannot access
     *          Resource Map data. Otherwise the calling thread is not blocked and
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
    inline void registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource );

    /**
     * \brief	Unregisters resource from the resource list of the map.
     * \param	Key	    The unique Key of resource that was previously registered.
     * \return	If succeeded, returns pointer to unregistered resource.
     *          Otherwise, returns nullptr.
     **/
    inline RESOURCE_OBJECT * unregisterResourceObject(const RESOURCE_KEY & Key);

    /**
     * \brief	Search Resource object in Resource Map by the given unique Key
     *          and if found, returns valid pointer to registered resource object.
     * \param	Key	    The unique Key of resource.
     * \return	Returns valid pointer to the resource object if found a resource
     *          registered with the unique key. Otherwise, returns nullptr.
     **/
    inline RESOURCE_OBJECT * findResourceObject( const RESOURCE_KEY & Key ) const;

    /**
     * \brief	Removes given Resource object from Resource Map and returns true 
     *          if operation succeeded. The function searches every entry in the map
     *          to find and remove the object from the map. To remove resource
     *          faster, unregister the resource with the unique key.
     * \param	resource	The resource object to lookup and remove.
     * \return	If found and removed any such resource, returns true.
     *          Otherwise returns false.
     **/
    inline bool removeResourceObject( RESOURCE_OBJECT * resource );

    /**
     * \brief	Removes all registered resources one-by-one.
     *          For every removed resource calls cleanResourceEntry() function
     *          if any additional job should be performed (for example, delete resources)
     **/
    inline void removeAllResources( void );

    /**
     * \brief   Removes first element of Resource map and returns true if successfully removed.
     *          On output out_FirstElement contains Resource Key and Object pair.
     * \param   out_FirstElement    On output, this will contain Key and Object pair 
     *                              of first element in resource map.
     * \return  Returns true if successfully removed first element.
     **/
    inline bool removeResourceFirstElement( std::pair<RESOURCE_KEY, RESOURCE_OBJECT *> OUT out_FirstElement );

    /**
     * \brief   Returns resource object of first object and the associated unique key in the map.
     * \param   out_FirstKey    On output, this parameter contains a resource valid key
     *                          if the resource is not empty.
     * \return  Returns pointer of stored Resource Object.
     **/
    inline RESOURCE_OBJECT* resourceFirstKey( RESOURCE_KEY & OUT out_FirstKey ) const;

    /**
     * \brief	Returns resource object of next element stored after specified unique Key.
     *          On input, the parameter 'in_out_NextKey' should be valid resource key.
     *          On output, this parameter points to the next element in the resource map.
     *          To get first element key, call resourceFirstKey() method. Check return value
     *          to know whether there are more elements in the map or not.
     * 
     * \param	in_out_NextKey  On input, this Key should be valid and point to the valid resource. 
     *                          On output, this parameter contain the key of the next element in the map.
     * \return	Returns valid pointer to the next registered resource object. Returns nullptr if
     *          reached end of the resource map.
     **/
    inline RESOURCE_OBJECT * resourceNextKey( RESOURCE_KEY & in_out_NextKey ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from removeAllResources() for every single
     *          resource being unregistered. Override this function if any additional
     *          work should be performed after unregistering resource.
     * \param	Key	        The Key value of resource
     * \param	resource	Pointer to resource object
     **/
    inline void cleanResourceEntry( RESOURCE_KEY & Key, RESOURCE_OBJECT * resource );

//////////////////////////////////////////////////////////////////////////
// Member Variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Synchronization object to synchronize access to resource data
     **/
    IEResourceLock &    mSynchObj;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TEResourceMap( void ) = delete;
    TEResourceMap( const TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> & /*src*/) = delete;
    TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> & operator = ( const TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> & /*src*/ ) = delete;
    TEResourceMap( TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> && /*src*/ ) noexcept = delete;
    TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> & operator = ( TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief       Blocking Resource Map. It is thread safe and synchronizes thread access. 
 *              Use instance of this class template if resources are accessed
 *              by more than one thread.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Deleter             Helper class to track the resource cleanup procedure.
 *
 * \see    TELockResourceMap, TENolockResourceMap
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter = TEResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
class TELockResourceMap    : public TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TELockResourceMap( void );
    /**
     * \brief   Destructor
     **/
    ~TELockResourceMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of ResourceLock to synchronize thread access.
     **/
    ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TELockResourceMap );
};

//////////////////////////////////////////////////////////////////////////
// TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief       Non-blocking Resource Map. It is not thread safe. Use instance 
 *              of this class if there is no need to control thread access.
 *
 * \tparam  RESOURCE_KEY        The type of Key to access resource element. Should be possible to compute hash.
 * \tparam  RESOURCE_OBJECT     The type of resource objects, which pointers are saved in the map.
 * \tparam  HashMap             HashMap class to inherit to implement resource list map.
 * \tparam  Deleter             Helper class to track the resource cleanup procedure.
 *
 * \see    TELockResourceMap
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
class TENolockResourceMap  : public TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TENolockResourceMap( void );
    /**
     * \brief   Destructor
     **/
    ~TENolockResourceMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of non-locking synchronization object. No thread locking will happen.
     **/
    NolockSynchObject mNoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( TENolockResourceMap );
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::TEResourceMap( IEResourceLock & synchObject )
    : HashMap   ( )
    , Deleter   ( )
    , mSynchObj (synchObject)
{
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::registerResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT * resource)
{
    Lock lock(mSynchObj);
    HashMap::setAt(Key, resource);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline RESOURCE_OBJECT * TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::unregisterResourceObject(const RESOURCE_KEY & Key)
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT* result = nullptr;
    HashMap::removeAt(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::findResourceObject(const RESOURCE_KEY & Key) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT* result = nullptr;
    HashMap::find(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::existResource(const RESOURCE_KEY & Key) const
{
    Lock lock(mSynchObj);
    return HashMap::contains(Key);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::lock( void ) const
{
    mSynchObj.lock(NECommon::WAIT_INFINITE);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::unlock( void ) const
{
    mSynchObj.unlock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::tryLock( void ) const
{
    return mSynchObj.tryLock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline int TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::getSize( void ) const
{
    Lock lock(mSynchObj);
    return HashMap::getSize();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::isEmpty( void ) const
{
    Lock lock(mSynchObj);
    return HashMap::isEmpty();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::removeResourceObject( RESOURCE_OBJECT * resource )
{
    Lock lock(mSynchObj);

    bool result = false;
    for ( HashMap::MAPPOS pos = HashMap::firstPosition(); pos != nullptr; pos = HashMap::nextPosition(pos))
    {
        if ( resource == HashMap::valueAtPosition(pos) )
        {
            HashMap::removePosition(pos);
            result = true;
            break;
        }
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::removeAllResources( void )
{
    Lock lock(mSynchObj);

    for ( HashMap::MAPPOS pos = HashMap::firstPosition(); HashMap::isValidPosition(pos); pos = HashMap::nextPosition(pos))
    {
        RESOURCE_KEY Key;
        RESOURCE_OBJECT* Value;
        HashMap::getAtPosition(pos, Key, Value);
        cleanResourceEntry(Key, Value);
    }

    HashMap::clear();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::removeResourceFirstElement(std::pair<RESOURCE_KEY, RESOURCE_OBJECT *> OUT out_FirstElement )
{
    Lock lock(mSynchObj);
    bool result = false;
    HashMap::MAPPOS pos  = HashMap::firstPosition();
    if (HashMap::isValidPosition(pos))
    {
        result = true;
        HashMap::removePosition(pos, out_FirstElement.first, out_FirstElement.second);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::resourceFirstKey( RESOURCE_KEY & OUT out_FirstKey ) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT * result = nullptr;
    MAPPOS pos = HashMap::firstPosition();
    if (HashMap::isValidPosition(pos))
    {
        HashMap::getAtPosition(pos, out_FirstKey, result);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::resourceNextKey( RESOURCE_KEY & in_out_NextKey ) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT * result = nullptr;
    MAPPOS pos = HashMap::find(in_out_NextKey);
    if (HashMap::isValidPosition(pos))
    {
        HashMap::nextEntry(pos, in_out_NextKey, result);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::cleanResourceEntry( RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource )
{
    Deleter::implCleanResource(Key, Resource);
}

//////////////////////////////////////////////////////////////////////////
// TENolockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::TENolockResourceMap( void )
    : TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>    (static_cast<IEResourceLock &>(mNoLock))

    , mNoLock   ()
{
}

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Deleter>
TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>::TELockResourceMap( void )
    : TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Deleter>    (static_cast<IEResourceLock &>(mLock))

    , mLock ( )
{
}

#endif  // AREG_BASE_TERESOURCEMAP_HPP
