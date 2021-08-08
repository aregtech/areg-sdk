#ifndef AREG_BASE_TERUNTIMERESOURCEMAP_HPP
#define AREG_BASE_TERUNTIMERESOURCEMAP_HPP
/************************************************************************
 * \file        areg/base/TERuntimeResourceMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Blocking and Non-blockind
 *              Runtime Resource Object container.
 *
 *
 *              This class is a container for registered resources.
 *              As an example, class used to keep mapping of created Thread
 *              Objects, where the Key value can be unique Thread ID.
 *              Having Resource Mapping object, it is possible accessing
 *              to registered object by unique Key value. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/TEHashMap.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/ESynchObjects.hpp"

/************************************************************************
 * Hierarchies and list of declared classes
 ************************************************************************/
// TEHashMap<RuntimeClassID, RUNTIME_DELEGATE *, const RuntimeClassID &, RUNTIME_DELEGATE *>
    template <typename RUNTIME_DELEGATE> class TERuntimeHashMap;
        // TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>>
            template <class RUNTIME_DELEGATE, class Implement> class TERuntimeResourceMap;
                template <class RUNTIME_DELEGATE, class Implement> class TENolockRuntimeResourceMap;
                template <class RUNTIME_DELEGATE, class Implement> class TELockRuntimeResourceMap;

/************************************************************************
 * \brief   In this file there are defined Blocking and Non-Blocking resource maps.
 *          Both classes are instances of TERuntimeHashMap class template.
 *          For details, see descriptions bellow.
 * 
 *          TELockRuntimeResourceMap class template is thread safe template and
 *          data access is synchronized. It can be used in cases, when data is accessed
 *          and modified from different threads.
 * 
 *          TELockRuntimeResourceMap class template is not thread safe template and
 *          data access is not synchronized. Do not use in case, where data is accessed
 *          and modified from different threads.
 *
 *          As an example of using runtime resource map, see consumer registration map
 *          declared in EventConsumerMap.
 *
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// TERuntimeHashMapImpl<RUNTIME_DELEGATE> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TERuntimeHashMapImpl<RUNTIME_DELEGATE> class template is an
 *          implementation of hash key, key and value comparing methods
 *          required by hash map object. This is a specific and customized
 *          implementation methods used by TERuntimeHashMap<RUNTIME_DELEGATE>
 *          class template to hold runtime object information.
 **/
template <typename RUNTIME_DELEGATE>
class TERuntimeHashMapImpl : public TEHashMapImpl<const RuntimeClassID &, RUNTIME_DELEGATE *>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int implHashKey( const RuntimeClassID & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

//////////////////////////////////////////////////////////////////////////
// TERuntimeHashMap<RUNTIME_DELEGATE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       TERuntimeHashMap<RUNTIME_DELEGATE> class template is a 
 *              container of Runtime objects. As a Key it uses 
 *              Runtime Class ID, which means that it will store 2 
 *              Runtime Objects with same Class ID values.
 * 
 * \details     This class is used as a base class for blocking
 *              and non-blocking Runtime Resource Maps. Do not instantiate
 *              This class directly, instead use either 
 *              TENolockRuntimeResourceMap or TELockRuntimeResourceMap
 *              class templates.
 *              
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 **/
template <typename RUNTIME_DELEGATE>
class TERuntimeHashMap : public TEHashMap<RuntimeClassID, RUNTIME_DELEGATE *, const RuntimeClassID &, RUNTIME_DELEGATE *, TERuntimeHashMapImpl<RUNTIME_DELEGATE>>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    TERuntimeHashMap( void );
    /**
     * \brief   Destructor.
     **/
    ~TERuntimeHashMap( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TERuntimeHashMap(const TERuntimeHashMap<RUNTIME_DELEGATE> & /*src*/);
    const TERuntimeHashMap<RUNTIME_DELEGATE> & operator = (const TERuntimeHashMap<RUNTIME_DELEGATE> & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// TERuntimeResourceMap<RUNTIME_DELEGATE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Runtime Resource Map is a Base class of blocking and 
 *              non-blocking resource maps. To instantiate class,
 *              pass defined Locking Synchronization object
 * 
 * \details     This class inherits from TEResourceMap, which is getting
 *              defined above TERuntimeHashMap as a type in type list.
 *              By this, it will get inheritance from TERuntimeHashMap.
 *              The only public constructor requires instantiated
 *              synchronization object. Bellow are defined locking and
 *              non-locking defined objects using critical section object
 *              and non-locking synchronization object.
 *
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 * \tparam  Implement           The implementation of resource clean call.
 **/
template <class RUNTIME_DELEGATE, class Implement>
class TERuntimeResourceMap : public TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor. The only public available.
     *          Requires instance of synchronization object.
     * \param   synchObject Reference to synchronization object.
     **/
    TERuntimeResourceMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor.
     **/
    ~TERuntimeResourceMap( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TERuntimeResourceMap( void );
    TERuntimeResourceMap(const TERuntimeResourceMap<RUNTIME_DELEGATE, Implement> & /*src*/);
    const TERuntimeResourceMap<RUNTIME_DELEGATE, Implement> & operator = (const TERuntimeResourceMap<RUNTIME_DELEGATE, Implement> & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Non-blocking class template. It inherits 
 *              TERuntimeResourceMap class template and contains instance
 *              of NolockSynchObject to pass to TERuntimeResourceMap.
 *              By this, any access to resource will not be blocked.
 *              Avoid using it if resources are accessed by more than one
 *              thread.
 * 
 * \details     Use instance of non-locking class if there is no need to
 *              synchronize resource access. The instance of
 *              NolockSynchObject does not block thread
 *
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 * \tparam  Implement           The implementation of resource clean call.
 **/
template <class RUNTIME_DELEGATE, class Implement>
class TENolockRuntimeResourceMap   : public TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    TENolockRuntimeResourceMap( void );
    /**
     * \brief   Destructor.
     **/
    ~TENolockRuntimeResourceMap( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Non-locking synchronization object.
     *          It will not lock thread on access.
     **/
    NolockSynchObject mNoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TENolockRuntimeResourceMap(const TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> & /*src*/);
    const TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> & operator = (const TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> &);
};

//////////////////////////////////////////////////////////////////////////
// TELockRuntimeResourceMap<RUNTIME_DELEGATE, class Implement> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Blocking class template. It inherits TERuntimeResourceMap 
 *              class template and contains instance of IEResourceLock
 *              to pass to TERuntimeResourceMap. By this, any access to 
 *              resource will be synchronized to ensure that only one thread
 *              has access permission within one unit-time.
 *              Use it if resources are accessed by more than one thread.
 * 
 * \details     Use instance of locking class if there is a need to
 *              synchronize resource access. The instance of
 *              IEResourceLock excludes concurrent thread access to resources.
 *
 * \tparam      RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 **/
template <class RUNTIME_DELEGATE, class Implement>
class TELockRuntimeResourceMap   : public TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>
{
public:
    TELockRuntimeResourceMap( void );
    ~TELockRuntimeResourceMap( void );

private:
    ResourceLock   mLock;

private:
    TELockRuntimeResourceMap(const TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> & /*src*/);
    const TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> & operator = (const TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> & /*src*/);
};

//////////////////////////////////////////////////////////////////////////
// TERuntimeHashMap<RUNTIME_DELEGATE> class template implementation
//////////////////////////////////////////////////////////////////////////
template <typename RUNTIME_DELEGATE>
TERuntimeHashMap<RUNTIME_DELEGATE>::TERuntimeHashMap( void )
    : TEHashMap<RuntimeClassID, RUNTIME_DELEGATE *, const RuntimeClassID &, RUNTIME_DELEGATE *, TERuntimeHashMapImpl<RUNTIME_DELEGATE>>  ( )
{

}

template <typename RUNTIME_DELEGATE>
TERuntimeHashMap<RUNTIME_DELEGATE>::~TERuntimeHashMap( void )
{

}

//////////////////////////////////////////////////////////////////////////
// TERuntimeResourceMap<RUNTIME_DELEGATE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Implement>
TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>::TERuntimeResourceMap( IEResourceLock& synchObject )
    : TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>, Implement> (synchObject)
{
    ; // do nothing
}

template <class RUNTIME_DELEGATE, class Implement>
TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>::~TERuntimeResourceMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Implement>
TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement>::TENolockRuntimeResourceMap( void )
    : TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>   (static_cast<IEResourceLock &>(mNoLock))
    
    , mNoLock   ( )
{
    ; // do nothing
}

template <class RUNTIME_DELEGATE, class Implement>
TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Implement>::~TENolockRuntimeResourceMap( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Implement>
TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement>::TELockRuntimeResourceMap( void )
    : TERuntimeResourceMap<RUNTIME_DELEGATE, Implement>   (static_cast<IEResourceLock &>(mLock))
    
    , mLock ( false )
{
    ; // do nothing
}

template <class RUNTIME_DELEGATE, class Implement>
TELockRuntimeResourceMap<RUNTIME_DELEGATE, Implement>::~TELockRuntimeResourceMap( void )
{
    ; // do nothing
}

#endif  // AREG_BASE_TERUNTIMERESOURCEMAP_HPP
