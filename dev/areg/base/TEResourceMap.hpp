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
    inline bool RegisterResourceObject( const RESOURCE_KEY & Key, RESOURCE_OBJECT * resource );

    /**
     * \brief	Unregisters Resource from map and if succeeded, returns pointer
     *          to unregistered resource object.
     * \param	Key	    Unique Key of resource that was previously registered.
     * \return	If succeeded, returns pointer to unregistered resource.
     *          Otherwise returns NULL
     **/
    inline RESOURCE_OBJECT * UnregisterResourceObject(const RESOURCE_KEY & Key);

    /**
     * \brief	Search Resource in Resource Map by give Key
     *          and if found, return pointer to registered resource
     * \param	Key	    The unique Key of resource
     * \return	If could find Resource in Resource Map by unique Key,
     *          returns pointer to resource. Otherwise returns NULL.
     **/
    inline RESOURCE_OBJECT * FindResourceObject( const RESOURCE_KEY & Key ) const;

    /**
     * \brief	Locks the Resource Map, grants ownership to locking thread
     *          and blocks any other thread to access resource map data.
     * \return	
     **/
    inline void ResourceLock( void ) const;

    /**
     * \brief	Unlocks previously locked Resource Map that other threads
     *          can have access to resource map data.
     * \return	
     **/
    inline void ResourceUnlock( void ) const;

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
    inline bool ResourceTryLock( void ) const;

    /**
     * \brief	Removes Resource from Resource map.
     *          The function will search resource by checking every entry
     *          and if finds any match, removes unique Key and resource from map
     * \param	resource	The resource object to lookup
     * \return	If found and removed any such resource, returns true.
     *          Otherwise returns false.
     **/
    inline bool RemoveResourceObject( RESOURCE_OBJECT * resource );

    /**
     * \brief	Removes all registered resources one-by-one.
     *          For every removed resource calls CleanResourceElement() function
     *          if any additional job should be performed (for example, delete resources)
     **/
    inline void RemoveAllResources( void );

    /**
     * \brief   Removes first element of Resource map and returns true if successfully removed.
     *          On output out_FirstElement contains Resource Key and Object pair.
     * \param   out_FirstElement    On output, this will contain Key and Object pair 
     *                              of first element in resource map.
     * \return  Returns true if successfully removed first element.
     **/
    inline bool RemoveResourceFirstElement( TEPair<RESOURCE_KEY, RESOURCE_OBJECT *> out_FirstElement );

    /**
     * \brief   Returns resource object of first element and its Key.
     * \param   out_FirstKey    Resource Key, no need to be valid, it will
     *                          validate on output and will contain the Key
     *                          value of first element in Resource Map.
     * \return  Returns pointer of stored Resource Object.
     **/
    inline RESOURCE_OBJECT* GetResourceFirstKey( RESOURCE_KEY & out_FirstKey ) const;

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
    inline RESOURCE_OBJECT * GetResourceNextKey( RESOURCE_KEY & in_out_NextKey ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Returns the size of Resource Map
     **/
    inline int GetSize( void ) const;

    /**
     * \brief   Returns true if resource map is empty.
     **/
    inline bool IsEmpty( void ) const;

    /**
     * \brief	Checks whether Resource Map contains entry
     *          with specified unique Key or not.
     * \param	Key	    The unique Key of Resource that was registered.
     * \return	If Resource with unique Key is registered, returns true.
     *          Otherwise returns false
     **/
    inline bool ResourceKeyExist( const RESOURCE_KEY & Key ) const;

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from RemoveAllResources() for every single
     *          resource being unregistered. Override this function if any additional
     *          work should be performed after unregistering resource.
     * \param	Key	        The Key value of resource
     * \param	resource	Pointer to resource object
     **/
    inline void CleanResourceElement( RESOURCE_KEY & Key, RESOURCE_OBJECT * resource );

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
 *              TEResourceMap and contains instance of CENoLockSynchObject, 
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
     * \brief   Instance of non-locking synchronization object.
     *          No thread locking will happen.
     **/
    CENolockSynchObject mNoLock;

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
    CEResourceLock   mLock;

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
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::RegisterResourceObject(const RESOURCE_KEY & Key, RESOURCE_OBJECT * resource)
{
    CELock lock(mSynchObj);

    RESOURCE_OBJECT* existing = NULL;
    return ( HashMap::Find(Key, existing) ? resource == existing : HashMap::SetKey(Key, resource, false) != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT * TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::UnregisterResourceObject(const RESOURCE_KEY & Key)
{
    CELock lock(mSynchObj);

    RESOURCE_OBJECT* result = NULL;
    HashMap::RemoveKey(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::FindResourceObject(const RESOURCE_KEY & Key) const
{
    CELock lock(mSynchObj);

    RESOURCE_OBJECT* result = NULL;
    HashMap::Find(Key, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::ResourceKeyExist(const RESOURCE_KEY & Key) const
{   CELock lock(mSynchObj); return (HashMap::Find(Key) != NULL);    }

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::ResourceLock( void ) const
{   mSynchObj.Lock(IESynchObject::WAIT_INFINITE);                 }

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::ResourceUnlock( void ) const
{   mSynchObj.Unlock( );                                            }

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::ResourceTryLock( void ) const
{
    return mSynchObj.TryLock( );
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline int TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::GetSize( void ) const
{   CELock lock(mSynchObj); return HashMap::GetSize();              }

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::IsEmpty( void ) const
{   CELock lock(mSynchObj); return HashMap::IsEmpty();              }

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::RemoveResourceObject( RESOURCE_OBJECT * resource )
{
    CELock lock(mSynchObj);

    bool removed = false;
    for ( MAPPOS pos = HashMap::GetStartPosition(); pos != NULL; pos = HashMap::GetNextPosition(pos))
    {
        if ( resource == HashMap::GetValueAt(pos) )
        {
            HashMap::RemoveAt(pos);
            removed = true;
            break;
        }
    }
    return removed;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::RemoveAllResources( void )
{
    CELock lock(mSynchObj);

    for ( MAPPOS pos = HashMap::GetStartPosition(); pos != NULL; pos = HashMap::GetNextPosition(pos))
    {
        typename HashMap::CEBlock * block = HashMap::GetBlockAt(pos);
        if (block != NULL)
        {
            CleanResourceElement(block->mKey, block->mValue);
        }
    }

    HashMap::RemoveAll();
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline bool TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::RemoveResourceFirstElement( TEPair<RESOURCE_KEY, RESOURCE_OBJECT *> out_FirstElement )
{
    CELock lock(mSynchObj);
    MAPPOS pos = HashMap::GetStartPosition();
    if (pos != NULL)
        HashMap::RemoveAt(pos, out_FirstElement.mKey, out_FirstElement.mValue);
    return (pos != NULL);
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::GetResourceFirstKey( RESOURCE_KEY & out_FirstKey ) const
{
    CELock lock(mSynchObj);

    RESOURCE_OBJECT * result = NULL;
    MAPPOS pos = HashMap::GetStartPosition();
    if (pos != NULL)
        HashMap::GetAt(pos, out_FirstKey, result);
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline RESOURCE_OBJECT* TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::GetResourceNextKey( RESOURCE_KEY & in_out_NextKey ) const
{
    CELock lock(mSynchObj);

    RESOURCE_OBJECT * result = NULL;
    MAPPOS pos = HashMap::Find(in_out_NextKey);
    if (pos != NULL && HashMap::GetNextElem(pos, in_out_NextKey, result) == false)
        result = NULL;
    return result;
}

template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class HashMap, class Implement>
inline void TEResourceMap<RESOURCE_KEY, RESOURCE_OBJECT, HashMap, Implement>::CleanResourceElement( RESOURCE_KEY & Key, RESOURCE_OBJECT * Resource )
{
    Implement::ImplCleanResourceElement(Key, Resource);
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
