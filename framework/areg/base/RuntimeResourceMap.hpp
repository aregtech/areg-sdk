#ifndef AREG_BASE_RUNTIMERESOURCEMAP_HPP
#define AREG_BASE_RUNTIMERESOURCEMAP_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeResourceMap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Blocking and Non-blocking
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
#include "areg/base/HashMap.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/SyncPrimitives.hpp"

namespace areg
{
    /************************************************************************
     * Hierarchies and list of declared classes
     ************************************************************************/
    // HashMap<RuntimeClassID, RUNTIME_DELEGATE>
        template <typename RUNTIME_DELEGATE> class RuntimeHashMap;
            // ResourceMapBase<RuntimeClassID, RUNTIME_DELEGATE, RuntimeHashMap<RUNTIME_DELEGATE>>
                template <class RUNTIME_DELEGATE, class Deleter> class RuntimeResourceMapBase;
                    template <class RUNTIME_DELEGATE, class Deleter> class RuntimeResourceMap;
                    template <class RUNTIME_DELEGATE, class Deleter> class ConcurrentRuntimeResourceMap;

    //////////////////////////////////////////////////////////////////////////
    // RuntimeHashMap<RUNTIME_DELEGATE, Deleter> class template declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   A hash map class template to track run-time objects accessed by 
     *          key, which are Runtime Class ID, and the stored values are 
     *          run-time objects.
     * 
     * \tparam  RUNTIME_DELEGATE    The type of runtime object to store in runtime resource map.
     **/
    template <typename RUNTIME_DELEGATE>
    class RuntimeHashMap : public areg::HashMap<areg::RuntimeClassID, RUNTIME_DELEGATE>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        RuntimeHashMap() = default;
        /**
         * \brief   Destructor.
         **/
        ~RuntimeHashMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden method calls
    //////////////////////////////////////////////////////////////////////////
    private:
        RuntimeHashMap(const RuntimeHashMap<RUNTIME_DELEGATE> & /*src*/) = delete;
        RuntimeHashMap<RUNTIME_DELEGATE> & operator = (const RuntimeHashMap<RUNTIME_DELEGATE> & /*src*/) = delete;
        RuntimeHashMap( RuntimeHashMap<RUNTIME_DELEGATE> && /*src*/ ) noexcept = delete;
        RuntimeHashMap<RUNTIME_DELEGATE> & operator = ( RuntimeHashMap<RUNTIME_DELEGATE> && /*src*/ ) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> class template declaration
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
    class RuntimeResourceMapBase : public areg::ResourceMapBase<areg::RuntimeClassID, RUNTIME_DELEGATE, RuntimeHashMap<RUNTIME_DELEGATE>, Deleter>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Initialization constructor. The only public available.
         *          Requires instance of synchronization object.
         * \param   syncObject Reference to synchronization object.
         **/
        RuntimeResourceMapBase( Lockable & syncObject );

        /**
         * \brief   Destructor.
         **/
        ~RuntimeResourceMapBase() = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden method calls
    //////////////////////////////////////////////////////////////////////////
    private:
        RuntimeResourceMapBase() = delete;
        RuntimeResourceMapBase(const RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> & operator = (const RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        RuntimeResourceMapBase( RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
        RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> & operator = ( RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template declaration
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
    class RuntimeResourceMap   : public RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor
         **/
        RuntimeResourceMap();
        /**
         * \brief   Destructor.
         **/
        ~RuntimeResourceMap() = default;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Non-locking synchronization object.
         *          It will not lock thread on access.
         **/
        NolockSyncObject mNoLock;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden method calls
    //////////////////////////////////////////////////////////////////////////
    private:
        RuntimeResourceMap(const RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = (const RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        RuntimeResourceMap( RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
        RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = ( RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, class Deleter> class template declaration
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
    class ConcurrentRuntimeResourceMap   : public RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter>
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor.
         **/
        ConcurrentRuntimeResourceMap();
        /**
         * \brief   Destructor.
         **/
        ~ConcurrentRuntimeResourceMap() = default;

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
        ConcurrentRuntimeResourceMap(const ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = (const ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & /*src*/) = delete;
        ConcurrentRuntimeResourceMap( ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
        ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> & operator = ( ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> && /*src*/ ) noexcept = delete;
    };

    //////////////////////////////////////////////////////////////////////////
    // RuntimeHashMap<RUNTIME_DELEGATE> class template implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter> class template implementation
    //////////////////////////////////////////////////////////////////////////
    template <class RUNTIME_DELEGATE, class Deleter>
    RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter>::RuntimeResourceMapBase( Lockable& syncObject )
        : areg::ResourceMapBase<areg::RuntimeClassID, RUNTIME_DELEGATE, RuntimeHashMap<RUNTIME_DELEGATE>, Deleter> (syncObject)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // RuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template implementation
    //////////////////////////////////////////////////////////////////////////
    template <class RUNTIME_DELEGATE, class Deleter>
    RuntimeResourceMap<RUNTIME_DELEGATE, Deleter>::RuntimeResourceMap()
        : RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter>   (static_cast<Lockable &>(mNoLock))
        
        , mNoLock   ( )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter> class template implementation
    //////////////////////////////////////////////////////////////////////////
    template <class RUNTIME_DELEGATE, class Deleter>
    ConcurrentRuntimeResourceMap<RUNTIME_DELEGATE, Deleter>::ConcurrentRuntimeResourceMap()
        : RuntimeResourceMapBase<RUNTIME_DELEGATE, Deleter>   (static_cast<Lockable &>(mLock))
        
        , mLock ( )
    {
    }

} // namespace areg
#endif  // AREG_BASE_RUNTIMERESOURCEMAP_HPP
