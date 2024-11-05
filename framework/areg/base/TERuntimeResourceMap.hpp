#ifndef AREG_BASE_TERUNTIMERESOURCEMAP_HPP
#define AREG_BASE_TERUNTIMERESOURCEMAP_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TERuntimeResourceMap.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Blocking and Non-blocking
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
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/SynchObjects.hpp"

/************************************************************************
 * Hierarchies and list of declared classes
 ************************************************************************/
// TEHashMap<RuntimeClassID, RUNTIME_DELEGATE>
    template <typename RUNTIME_DELEGATE> class TERuntimeHashMap;
        // TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>>
            template <class RUNTIME_DELEGATE, class Deleter> class TERuntimeResourceMap;
                template <class RUNTIME_DELEGATE, class Deleter> class TENolockRuntimeResourceMap;
                template <class RUNTIME_DELEGATE, class Deleter> class TELockRuntimeResourceMap;

//////////////////////////////////////////////////////////////////////////
// TERuntimeHashMap<RUNTIME_DELEGATE, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A hash map class template to track run-time objects accessed by 
 *          key, which are Runtime Class ID, and the stored values are 
 *          run-time objects.
 * 
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 **/
template <typename RUNTIME_DELEGATE>
class TERuntimeHashMap : public TEHashMap<RuntimeClassID, RUNTIME_DELEGATE>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    TERuntimeHashMap( void ) = default;
    /**
     * \brief   Destructor.
     **/
    ~TERuntimeHashMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TERuntimeHashMap(const TERuntimeHashMap<RUNTIME_DELEGATE> & /*src*/) = delete;
    TERuntimeHashMap<RUNTIME_DELEGATE> & operator = (const TERuntimeHashMap<RUNTIME_DELEGATE> & /*src*/) = delete;
    TERuntimeHashMap( TERuntimeHashMap<RUNTIME_DELEGATE> && /*src*/ ) noexcept = delete;
    TERuntimeHashMap<RUNTIME_DELEGATE> & operator = ( TERuntimeHashMap<RUNTIME_DELEGATE> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A run-time object resource map base class template to track run-time objects
 *          accessed by key, which are Runtime Class ID, and the stored values are 
 *          run-time object. Whether the resource map is thread safe or not, depends
 *          on instance of resource lock object passed in constructor. If a resource map
 *          is used only within one thread context, pass no locking synchronization object,
 *          which is faster. Otherwise, pass one of instances of resource lock.
 *
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 * \tparam  Deleter             The implementation of resource clean call.
 **/
template <class RUNTIME_DELEGATE, class Deleter>
class TERuntimeResourceMap : public TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>, Deleter>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor. The only public available.
     *          Requires instance of synchronization object.
     * \param   synchObject Reference to synchronization object.
     **/
    TERuntimeResourceMap( IEResourceLock & synchObject );

    /**
     * \brief   Destructor.
     **/
    ~TERuntimeResourceMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    TERuntimeResourceMap( void ) = delete;
    TERuntimeResourceMap(const TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = (const TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TERuntimeResourceMap( TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
    TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = ( TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Non thread-safe resource map class template to track run-time objects
 *          accessed by their key, which are Runtime Class ID, and the stored
 *          elements are run-time object. Use this object if resources are modified
 *          and accessed only within one thread context.
 *
 * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 * \tparam  Deleter           The implementation of resource clean call.
 **/
template <class RUNTIME_DELEGATE, class Deleter>
class TENolockRuntimeResourceMap   : public TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter>
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
    ~TENolockRuntimeResourceMap( void ) = default;

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
    TENolockRuntimeResourceMap(const TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = (const TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TENolockRuntimeResourceMap( TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
    TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = ( TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TELockRuntimeResourceMap<RUNTIME_DELEGATE, class Deleter> class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Thread-safe resource map class template to track run-time objects
 *          accessed by their key, which are Runtime Class ID, and the stored
 *          elements are run-time object. Use this object if resources are modified
 *          and accessed within multiple thread context.
 *
 * \tparam      RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
 **/
template <class RUNTIME_DELEGATE, class Deleter>
class TELockRuntimeResourceMap   : public TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter>
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    TELockRuntimeResourceMap( void );
    /**
     * \brief   Destructor.
     **/
    ~TELockRuntimeResourceMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Resource lock object to synchronize data access.
     **/
    ResourceLock    mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    TELockRuntimeResourceMap(const TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = (const TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
    TELockRuntimeResourceMap( TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
    TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = ( TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
};

//////////////////////////////////////////////////////////////////////////
// TERuntimeHashMap<RUNTIME_DELEGATE> class template implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Deleter>
TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter>::TERuntimeResourceMap( IEResourceLock& synchObject )
    : TEResourceMap<RuntimeClassID, RUNTIME_DELEGATE, TERuntimeHashMap<RUNTIME_DELEGATE>, Deleter> (synchObject)
{
}

//////////////////////////////////////////////////////////////////////////
// TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Deleter>
TENolockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter>::TENolockRuntimeResourceMap( void )
    : TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter>   (static_cast<IEResourceLock &>(mNoLock))
    
    , mNoLock   ( )
{
}

//////////////////////////////////////////////////////////////////////////
// TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template implementation
//////////////////////////////////////////////////////////////////////////
template <class RUNTIME_DELEGATE, class Deleter>
TELockRuntimeResourceMap<RUNTIME_DELEGATE, Deleter>::TELockRuntimeResourceMap( void )
    : TERuntimeResourceMap<RUNTIME_DELEGATE, Deleter>   (static_cast<IEResourceLock &>(mLock))
    
    , mLock ( )
{
}

#endif  // AREG_BASE_TERUNTIMERESOURCEMAP_HPP
