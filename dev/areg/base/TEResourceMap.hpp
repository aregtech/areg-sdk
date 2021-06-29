#ifndef AREG_BASE_TERESOURCEMAP_HPP
#define AREG_BASE_TERESOURCEMAP_HPP
/************************************************************************
 * \file        areg/base/TEResourceMap.hpp
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
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement> class TEResourceMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement> class TELockResourceMap;
        template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement> class TENolockResourceMap;

/************************************************************************
 * \brief   This file contains declarations of following class templates:
 *
 *              1.  TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>,
 *                  which is a generic resource mapping class template;
 *
 *              2.  TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>,
 *                  which is a resource mapping class template, but no thread safe;
 *
 *              3.  TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>,
 *                  which is a resource mapping class template, but thread safe.
 *
 *          For more information, see descriptions bellow
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>
 *              This class is Hash Map container, used to keep control over 
 *              registered resources. The resource can be any type of object.
 *              Every registered resource should have unique Key to ensure
 *              accessing resource by referring to unique Key.
 *              The resource objects are stored by their pointer values,
 *              which means that Resource should be allocated in memory and
 *              be valid as long, as they are store in Resource Map.
 * 
 * \details     To instantiate Resource Map, the Hash Map class 
 *              should be specified, as well as the synchronization object. 
 *              As a HashMap class type one of predefined Hash Map containers 
 *              can be used, or define own. The reason of passing HashMap as 
 *              class type, to have Hash Key value calculation 
 *              for every type of resource map to store unique Key.
 *              This class is a container for registered resources.
 *              Use blocking and non-blocking hash map instead of this class.
 *              See description bellow.
 *
 * \tparam  RESOURCE_KEY    The type of Key to access resource element
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types will be used and resource map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement = TEResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
class TEResourceMap     : protected HashMap
                        , protected Implement
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default Constructor
     **/
    TEResourceMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor
     **/
    ~TEResourceMap( void );

//////////////////////////////////////////////////////////////////////////
// Operations
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
    inline bool registerResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * resource );

    /**
     * \brief	Unregisters Resource from map and if succeeded, returns pointer
     *          to unregistered resource object.
     * \param	Key	    Unique Key of resource that was previously registered.
     * \return	If succeeded, returns pointer to unregistered resource.
     *          Otherwise returns NULL
     **/
    inline RESOURCE_OBJECT * unregisterResourceObject(const RESOURCE_KEY & Key);

    /**
     * \brief	Search Resource in Resource Map by give Key
     *          and if found, return pointer to registered resource
     * \param	Key	    The unique Key of resource
     * \return	If could find Resource in Resource Map by unique Key,
     *          returns pointer to resource. Otherwise returns NULL.
     **/
    inline RESOURCE_OBJECT * findResourceObject( const RESOURCE_KEY & Key ) const;

    /**
     * \brief	Removes Resource from Resource map.
     *          The function will search resource by checking every entry
     *          and if finds any match, removes unique Key and resource from map
     * \param	resource	The resource object to lookup
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
    inline bool removeResourceFirstElement( TEPair<RESOURCE_KEY, RESOURCE_OBJECT *> out_FirstElement );

    /**
     * \brief   Returns resource object of first element and its Key.
     * \param   out_FirstKey    Resource Key, no need to be valid, it will
     *                          validate on output and will contain the Key
     *                          value of first element in Resource Map.
     * \return  Returns pointer of stored Resource Object.
     **/
    inline RESOURCE_OBJECT* resourceFirstKey( RESOURCE_KEY & out_FirstKey ) const;

    /**
     * \brief	Returns resource object of next element and its Key.
     *          Before use of this function, call GetResourceFirstKey()
     *          to get at least Key of first element. This function
     *          can be called repeatedly and on output Key will contain
     *          value of next resource object.
     * \param	in_out_NextKey  On input this Key should be valid. Before
     *                          first call, use function GetResourceFirstKey()
     *                          to get valid Key of first element. 
     *                          On output this value will be updated and will
     *                          contain Key value of next element. Use output
     *                          value to access next resource object.
     * \return	Returns pointer to next Resource Object.
     **/
    inline RESOURCE_OBJECT * resourceNextKey( RESOURCE_KEY & in_out_NextKey ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

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
    TEResourceMap( void );
    TEResourceMap( const TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> & );
    const TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> & operator = ( const TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> & );
};


//////////////////////////////////////////////////////////////////////////
// TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief       TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>
 *              Non-blocking Resource Map. It is not synchronizing
 *              thread access. Use instance of this class template
 *              if there is no need to control thread access. It inherits from 
 *              TEResourceMap and contains instance of NoLockSynchObject, 
 *              which is not synchronizing thread access.
 * \tparam  RESOURCE_KEY    The type of Key to access resource element
 * \tparam  RESOURCE_OBJECT The type of resource objects. Pointer of
 *                          this defined types will be used and resource map.
 * \tparam  HashMap         HashMap class to inherit, which has possibility
 *                          to calculate Hash value from RESOURCE_KEY type.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
class TENolockResourceMap  : public TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>
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
    ~TENolockResourceMap( void );

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
    TENolockResourceMap(const TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> &);
    const TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>& operator = (const TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> &);
};

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> class template declaration
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
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement = TEResourceMapImpl<RESOURCE_KEY, RESOURCE_OBJECT>>
class TELockResourceMap    : public TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>
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
    ~TELockResourceMap( void );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Instance of Critical Section to synchronize thread access.
     **/
    ResourceLock   mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    TELockResourceMap(const TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> &);
    const TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>& operator = (const TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> &);
};

//////////////////////////////////////////////////////////////////////////
// Function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::TEResourceMap( IEResourceLock & synchObject )
    : HashMap   ( )
    , Implement ( )
    , mSynchObj (synchObject)
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::~TEResourceMap( void )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::registerResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT * resource)
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT* existing = NULL;
    return ( HashMap::find(Key, existing) ? resource == existing : HashMap::setAt(Key, resource, false) != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT * TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::unregisterResourceObject(const RESOURCE_KEY & Key)
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT* result = NULL;
    HashMap::removeAt(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::findResourceObject(const RESOURCE_KEY & Key) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT* result = NULL;
    HashMap::find(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::existResource(const RESOURCE_KEY & Key) const
{
    Lock lock(mSynchObj);
    return (HashMap::find(Key) != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::lock( void ) const
{
    mSynchObj.lock(IESynchObject::WAIT_INFINITE);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::unlock( void ) const
{
    mSynchObj.unlock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::tryLock( void ) const
{
    return mSynchObj.tryLock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline int TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::getSize( void ) const
{
    Lock lock(mSynchObj);
    return HashMap::getSize();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::isEmpty( void ) const
{
    Lock lock(mSynchObj);
    return HashMap::isEmpty();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::removeResourceObject( RESOURCE_OBJECT * resource )
{
    Lock lock(mSynchObj);

    bool removed = false;
    for ( MAPPOS pos = HashMap::firstPosition(); pos != NULL; pos = HashMap::nextPosition(pos))
    {
        if ( resource == HashMap::valueAtPosition(pos) )
        {
            HashMap::removePosition(pos);
            removed = true;
            break;
        }
    }

    return removed;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::removeAllResources( void )
{
    Lock lock(mSynchObj);

    for ( MAPPOS pos = HashMap::firstPosition(); pos != NULL; pos = HashMap::nextPosition(pos))
    {
        typename HashMap::Block * block = HashMap::blockAt(pos);
        if (block != NULL)
        {
            cleanResourceEntry(block->mKey, block->mValue);
        }
    }

    HashMap::removeAll();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::removeResourceFirstElement( TEPair<RESOURCE_KEY, RESOURCE_OBJECT *> out_FirstElement )
{
    Lock lock(mSynchObj);
    MAPPOS pos = HashMap::firstPosition();
    if (pos != NULL)
    {
        HashMap::removePosition(pos, out_FirstElement.mKey, out_FirstElement.mValue);
    }

    return (pos != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::resourceFirstKey( RESOURCE_KEY & out_FirstKey ) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT * result = NULL;
    MAPPOS pos = HashMap::firstPosition();
    if (pos != NULL)
    {
        HashMap::getAtPosition(pos, out_FirstKey, result);
    }

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::resourceNextKey( RESOURCE_KEY & in_out_NextKey ) const
{
    Lock lock(mSynchObj);

    RESOURCE_OBJECT * result = NULL;
    MAPPOS pos = HashMap::find(in_out_NextKey);
    if (pos != NULL && HashMap::nextEntry(pos, in_out_NextKey, result) == false)
        result = NULL;

    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::cleanResourceEntry( RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource )
{
    Implement::implCleanResource(Key, Resource);
}

//////////////////////////////////////////////////////////////////////////
// TENolockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::TENolockResourceMap( void )
    : TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>    (static_cast<IEResourceLock &>(mNoLock))

    , mNoLock   ( false )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TENolockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::~TENolockResourceMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TELockResourceMap class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::TELockResourceMap( void )
    : TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>    (static_cast<IEResourceLock &>(mLock))

    , mLock ( false )
{
    ; // do nothing
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
TELockResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::~TELockResourceMap( void )
{
    ; // do nothing
}

#endif  // AREG_BASE_TERESOURCEMAP_HPP
