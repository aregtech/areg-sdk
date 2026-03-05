#ifndef AREG_COMPONENT_PRIVATE_COMPONENTINFO_HPP
#define AREG_COMPONENT_PRIVATE_COMPONENTINFO_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentInfo.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Info class declaration.
 *              Contains information of threads binded with component.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/HashMap.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/ComponentAddress.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ComponentThread;
class WorkerThread;
class DispatcherThread;

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Encapsulates information about a component, including its master thread and registered
 *          worker threads. Provides communication and message routing between threads.
 **/
class AREG_API ComponentInfo
{
//////////////////////////////////////////////////////////////////////////
// Internal class and types
//////////////////////////////////////////////////////////////////////////
private:
    //////////////////////////////////////////////////////////////////////////
    // ComponentInfo::_WorkerThreadMap type declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The Hash Map object to save information of threads
     *          saved in specified Component Info object.
     **/
    using _WorkerThreadMap  = OrderedMap<ThreadAddress, WorkerThread *>;

    /**
     * \brief   Resource mapping object type. 
     *          As a Key, used thread address (Worker Thread address)
     *          As a value, it saves pointers of Worker Thread object
     *          As a Hash Map used ComponentInfo::_WorkerThreadMap object
     **/
    using MapWorkerThread   = ConcurrentResourceMap<ThreadAddress, WorkerThread *, _WorkerThreadMap, ResourceMapImpl<ThreadAddress, WorkerThread>>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates a component info object with the master thread and component role name.
     *
     * \param   ownerThread     The master thread of the component.
     * \param   roleName        The role name of the component (must not be empty).
     **/
    ComponentInfo( ComponentThread & ownerThread, const String & roleName );

    ~ComponentInfo() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the master component thread.
     **/
    inline ComponentThread & master_thread() const;

    /**
     * \brief   Returns the component address.
     **/
    inline const ComponentAddress & address() const;

    /**
     * \brief   Returns the role name of the component.
     **/
    inline const String & role_name() const;

    /**
     * \brief   Returns true if the specified thread address is a registered worker thread.
     *
     * \param   threadAddress       The thread address to check.
     * \return  Returns true if the thread address is a registered worker thread; false otherwise.
     **/
    [[nodiscard]]
    inline bool is_worker_thread( const ThreadAddress & threadAddress) const;

    /**
     * \brief   Returns true if the component has at least one registered worker thread.
     **/
    [[nodiscard]]
    inline bool has_worker_threads() const noexcept;

    /**
     * \brief   Returns true if the specified worker thread is registered in the component.
     *
     * \param   workerThread    The worker thread object to check.
     * \return  Returns true if the worker thread is registered; false otherwise.
     **/
    [[nodiscard]]
    bool is_worker_registered( WorkerThread & workerThread ) const;

    /**
     * \brief   Returns true if the specified thread address is the component's master thread.
     *
     * \param   threadAddress       The thread address to check.
     * \return  Returns true if the thread address is the master thread; false otherwise.
     **/
    [[nodiscard]]
    bool is_master_thread( const ThreadAddress & threadAddress ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Registers a worker thread in the component.
     *
     * \param   workerThread    The worker thread to register.
     **/
    void register_worker_thread( WorkerThread & workerThread );

    /**
     * \brief   Unregisters a worker thread from the component.
     *
     * \param   workerThread    The worker thread to unregister.
     * \return  Returns true if the worker thread was unregistered; false if it was not registered.
     **/
    bool unregister_worker_thread( WorkerThread & workerThread );

    /**
     * \brief   Searches for a worker thread by address and returns a pointer if found.
     *
     * \param   threadAddress       The unique thread address to search.
     * \return  Returns a pointer to the worker thread if found; nullptr otherwise.
     **/
    inline WorkerThread * find_worker_thread( const ThreadAddress & threadAddress ) const;

    /**
     * \brief   Searches for a worker thread by name and returns a pointer if found.
     *
     * \param   threadName      The unique thread name to search.
     * \return  Returns a pointer to the worker thread if found; nullptr otherwise.
     **/
    inline WorkerThread * find_worker_thread( const String & threadName ) const;

    /**
     * \brief   Searches for a worker thread by extracting the thread address from a component path.
     *
     * \param   componentPath       The component path containing the worker thread address.
     * \return  Returns a pointer to the worker thread if found; nullptr otherwise.
     **/
    inline WorkerThread * find_thread( const String & componentPath ) const;

    /**
     * \brief   Searches for a registered event consumer by class ID in the master and worker
     *          threads.
     *
     * \param   whichClass      The runtime class ID of the event consumer.
     * \return  Returns a pointer to the dispatcher thread where the event consumer is registered;
     *          nullptr if not found.
     **/
    DispatcherThread * find_event_consumer( const RuntimeClassID & whichClass ) const;

    /**
     * \brief   Returns a pointer to the first worker thread and sets the thread address on output.
     *
     * \param[out] threadAddress       On output, contains the address of the first worker thread,
     *                                 or an invalid address if none exist.
     * \return  Returns a pointer to the first worker thread; nullptr if no worker threads are
     *          registered.
     **/
    inline WorkerThread * first_worker_thread( ThreadAddress & threadAddress );

    /**
     * \brief   Returns a pointer to the next worker thread and updates the thread address on
     *          output.
     *
     * \param[in,out] threadAddress       On input, contains the address of the current worker
     *                                    thread. On output, contains the address of the next
     *                                    thread, or an invalid address if at the end.
     * \return  Returns a pointer to the next worker thread; nullptr if no more threads exist.
     **/
    inline WorkerThread * next_worker_thread( ThreadAddress & threadAddress );

    /**
     * \brief   Removes and returns the first worker thread from the list.
     *
     * \param[out] threadAddress       On output, contains the address of the removed worker thread.
     * \return  Returns a pointer to the removed worker thread; nullptr if no threads are
     *          registered.
     **/
    inline WorkerThread * remove_worker_thread( ThreadAddress & threadAddress );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The address of Component associated with Component Info object
     **/
    ComponentAddress  mComponentAddress;

    /**
     * \brief   The Master Thread of Component (Component Thread)
     **/
    ComponentThread & mMasterThread;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The map of Worker Threads binded with Component
     **/
    MapWorkerThread     mWorkerThreadMap;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ComponentInfo() = delete;
    AREG_NOCOPY_NOMOVE( ComponentInfo );
};

//////////////////////////////////////////////////////////////////////////
// ComponentInfo class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline ComponentThread& ComponentInfo::master_thread() const
{
    return mMasterThread;
}

inline const ComponentAddress& ComponentInfo::address() const
{
    return mComponentAddress;
}

inline const String& ComponentInfo::role_name() const
{
    return mComponentAddress.role_name();
}

inline bool ComponentInfo::is_worker_thread( const ThreadAddress& threadAddress ) const
{
    return mWorkerThreadMap.exist(threadAddress);
}

inline WorkerThread* ComponentInfo::find_worker_thread( const ThreadAddress& threadAddress ) const
{
    return mWorkerThreadMap.find_resource_object(threadAddress);
}

inline WorkerThread* ComponentInfo::find_worker_thread( const String & threadName ) const
{
    Thread* targetThread = Thread::find_by_name(threadName);
    return (targetThread != nullptr ? find_worker_thread(targetThread->address()) : nullptr);
}

inline WorkerThread* ComponentInfo::find_thread( const String & componentPath ) const
{
    ComponentAddress componentAddress = ComponentAddress::from_path(componentPath);
    return find_worker_thread(componentAddress.thread_address());
}

inline WorkerThread* ComponentInfo::first_worker_thread( ThreadAddress & threadAddress )
{
    return mWorkerThreadMap.resource_first_key(threadAddress);
}

inline WorkerThread* ComponentInfo::next_worker_thread( ThreadAddress & threadAddress )
{
    return mWorkerThreadMap.resource_next_key(threadAddress);
}

inline WorkerThread* ComponentInfo::remove_worker_thread(ThreadAddress& threadAddress)
{
    std::pair<ThreadAddress, WorkerThread*> elem{ThreadAddress::invalid_thread_address(), nullptr};
    if (mWorkerThreadMap.is_empty() == false)
    {
        mWorkerThreadMap.remove_first_element(elem);
    }

    threadAddress = elem.first;
    return elem.second;
}

inline bool ComponentInfo::has_worker_threads() const noexcept
{
    return (mWorkerThreadMap.is_empty() == false);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_COMPONENTINFO_HPP
