#ifndef AREG_COMPONENT_COMPONENT_HPP
#define AREG_COMPONENT_COMPONENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/Component.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component class declaration.
 *              To receive messages, all service interface objects
 *              should be grouped in component. Use this class as a base
 *              class for all defined components in application.
 *          
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/RuntimeObject.hpp"

#include "areg/component/ComponentAddress.hpp"
#include "areg/component/private/ComponentInfo.hpp"
#include "areg/component/Model.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class WorkerThreadConsumer;
    class ComponentThread;
    class WorkerThread;
    class StubBase;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Component class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Container for service objects and client proxies. Each component has an owning thread
 *          and may have worker threads. Components provide and consume services identified by role
 *          name, which is unique within the component's visibility scope.
 **/
class AREG_API Component   : public    RuntimeObject
{
//////////////////////////////////////////////////////////////////////////
// Predefined types. Fol local use
//////////////////////////////////////////////////////////////////////////
    //!< The basic operations of resource-map.
    using ImplComponentResource = ResourceMapImpl<uint32_t, Component *>;
    /**
     * \brief   The integer hash-map to store components where the keys are the calculated number of the component.
     *          Component           The saved values are Component objects.
     **/
    using MapComponentContainer  = IntegerHashMap<Component *>;
    /**
     * \brief   Component::MapComponentResource
     *          The Resource Map of instantiated components.
     *          uint32_t                The calculated number of component as a key.
     *          Component               The type of container values, it contains Components
     *          MapComponentContainer   The hash-map object to store containers.
     *          ImplComponentResource   The implementation of basic resource+map operations.
     **/
    using MapComponentResource  = ConcurrentResourceMap<uint32_t, Component *, MapComponentContainer, ImplComponentResource>;
//////////////////////////////////////////////////////////////////////////
// Declare as runtime object
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(Component)

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Component::ListServers
     *          The list of addresses of Servers.
     *          StubBase  The pointer to base class of Stub objects.
     **/
    using ListServers           = LinkedList<StubBase*>;

/************************************************************************/
// static functions to load / unload component
/************************************************************************/

    /**
     * \brief   Loads and instantiates a component from registry entry.
     *
     * \param   entry               The registry entry with component loading information.
     * \param   componentThread     The thread that will own and dispatch the component.
     * \return  Pointer to the instantiated component.
     **/
    static Component * load_component( const areg::ComponentEntry & entry, ComponentThread & componentThread);

    /**
     * \brief   Unloads and destroys a component.
     *
     * \param   comItem     The component to unload.
     * \param   entry       The registry entry with component information.
     **/
    static void unload_component( Component & comItem, const areg::ComponentEntry & entry) noexcept;

/************************************************************************/
// static utility functions to search component and check existence
/************************************************************************/

    /**
     * \brief   Finds a component by its role name.
     *
     * \param   roleName    The role name to search for.
     * \return  Pointer to the component if found; null otherwise.
     **/
    [[nodiscard]]
    static inline Component * find_by_name(const String & roleName) noexcept;

    /**
     * \brief   Finds a component by its hash value.
     *
     * \param   magicNum    The hash value to search for.
     * \return  Pointer to the component if found; null otherwise.
     **/
    [[nodiscard]]
    static inline Component * find_by_number(uint32_t magicNum) noexcept;

    /**
     * \brief   Finds a component by its address.
     *
     * \param   comAddress      The component address to search for.
     * \return  Pointer to the component if found; null otherwise.
     **/
    [[nodiscard]]
    static inline Component * find_by_address(const ComponentAddress & comAddress) noexcept;

    /**
     * \brief   Returns true if a component with the specified role name exists.
     *
     * \param   roleName    The role name to check.
     * \return  True if the component exists; false otherwise.
     **/
    [[nodiscard]]
    static inline bool exist(const String & roleName) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a component with specified role name and owning thread.
     *
     * \param   roleName        Unique role name for the component.
     * \param   ownerThread     The thread that owns the component.
     **/
    Component( const String & roleName, ComponentThread & ownerThread );

    /**
     * \brief   Creates a component from a registry entry and owning thread.
     *
     * \param   regEntry        The registry entry with component role name.
     * \param   ownerThread     The thread that owns the component.
     **/
    Component( const areg::ComponentEntry & regEntry, ComponentThread & ownerThread );

    /**
     * \brief   Creates a component with specified role name in the current thread.
     *
     * \param   roleName    Unique role name for the component.
     **/
    explicit Component( const String & roleName );

    virtual ~Component();

//////////////////////////////////////////////////////////////////////////
// Operations, overrides.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief   Called when the component thread starts. Override to perform initialization and
     *          register listeners.
     *
     * \param   comThread       The component thread triggering startup.
     **/
    virtual void startup_component( ComponentThread & comThread );

    /**
     * \brief   Called when the component thread shuts down. Override to perform cleanup and
     *          unregister listeners.
     *
     * \param   comThread       The component thread triggering shutdown.
     **/
    virtual void shutdown_component( ComponentThread & comThread );

    /**
     * \brief   Called when the master thread of the component is notified to shut down. Not
     *          thread-safe. Called primarily by the Service Manager.
     *
     * \param   comThread       The component thread triggering shutdown notification.
     **/
    virtual void notify_component_shutdown( ComponentThread & comThread );

    /**
     * \brief   Waits for all worker threads to complete their jobs with the specified timeout per thread.
     *
     * \param   waitTimeout     The timeout in milliseconds for each worker thread.
     **/
    virtual void wait_component_completion( uint32_t waitTimeout );

    /**
     * \brief   Returns the worker thread consumer for creating worker threads. Called during
     *          component initialization.
     *
     * \param   consumerName        The name of the consumer to identify.
     * \param   workerThreadName    The name of the worker thread.
     * \return  Valid pointer if the worker thread has an assigned consumer; null otherwise.
     **/
    virtual WorkerThreadConsumer * worker_thread_consumer( const String & consumerName, const String & workerThreadName );

    /**
     * \brief   Called when a worker thread starts. Override to perform additional operations.
     *
     * \param   consumer        The worker thread consumer.
     * \param   workerThread    The worker thread that started.
     **/
    virtual void notify_thread_started(WorkerThreadConsumer& consumer, WorkerThread & workerThread);

/************************************************************************/
// Component operations
/************************************************************************/

    /**
     * \brief   Creates and starts a worker thread with the specified configuration.
     *
     * \param   threadName          Unique thread name within the system.
     * \param   consumer            The consumer whose start and stop methods will be called.
     * \param   ownerThread         The component thread that owns this worker thread.
     * \param   watchdogTimeout     Watchdog timeout in milliseconds (0 to disable).
     * \param   stackSizeKb         Stack size in kilobytes (0 to use system default).
     * \param   maxQeueue           Event-queue ring capacity; areg::IGNORE_VALUE (0) reads from configuration.
     * \param   dropOnFull          Full-ring policy; areg::Bool::Undefined reads from configuration.
     * \param   waitMs              Lossless full-ring block timeout; areg::WAIT_INFINITE reads from configuration.
     * \return  Pointer to the created worker thread.
     **/
    WorkerThread * create_worker_thread( const String & threadName
                                       , WorkerThreadConsumer & consumer
                                       , ComponentThread & ownerThread
                                       , uint32_t watchdogTimeout = areg::WATCHDOG_IGNORE
                                       , uint32_t stackSizeKb     = areg::DEFAULT_STACK_SIZE
                                       , uint32_t maxQeueue       = areg::IGNORE_VALUE
                                       , areg::Bool dropOnFull     = areg::Bool::Undefined
                                       , uint32_t waitMs           = areg::WAIT_INFINITE );

    /**
     * \brief   Stops and deletes a worker thread by name.
     *
     * \param   threadName      The name of the worker thread to delete.
     **/
    void delete_worker_thread( const String & threadName );

    /**
     * \brief   Terminates the component, deletes all worker threads, and cleans up resources.
     **/
    void terminate_self();

    /**
     * \brief   Registers a stub (service provider) object with this component.
     *
     * \param   server      The stub object to register.
     **/
    inline void register_service_provider( StubBase & server );

    /**
     * \brief   Finds a registered stub by service name.
     *
     * \param   serviceName     The service name to search for.
     * \return  Pointer to the stub if found; null otherwise.
     **/
    [[nodiscard]]
    StubBase * find_provider( const String & serviceName ) noexcept;

    /**
     * \brief   Finds the dispatcher thread that has a consumer registered for the specified event
     *          class.
     *
     * \param   whichClass      The runtime class ID of the event.
     * \return  Pointer to the dispatcher thread if found; null otherwise.
     **/
    [[nodiscard]]
    inline DispatcherThread * find_event_consumer( uint32_t whichClass ) const noexcept;

    /**
     * \brief   Returns the master thread that owns this component.
     **/
    [[nodiscard]]
    inline ComponentThread & master_thread() noexcept;

    /**
     * \brief   Returns the role name of this component.
     **/
    [[nodiscard]]
    inline const String & role_name() const noexcept;

    /**
     * \brief   Returns the address of this component.
     **/
    [[nodiscard]]
    inline const ComponentAddress & address() const noexcept;

    /**
     * \brief   Returns the list of registered service provider addresses.
     **/
    [[nodiscard]]
    inline const ListServers & extract_service_addresses() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    [[nodiscard]]
    inline Component & self() noexcept;

    /**
     * \brief   Shuts down all registered services of this component.
     **/
    inline void _shutdown_services();
    /**
     * \brief   Returns the component thread of the current component.
     **/
    [[nodiscard]]
    static ComponentThread & _current_component_thread() noexcept;

    /**
     * \brief   Computes a hash value for a component.
     *
     * \param   comp    The component to hash.
     * \return  Hash value of the component.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( Component & comp ) noexcept;

    /**
     * \brief   Returns the static resource map of all created components.
     **/
    [[nodiscard]]
    static Component::MapComponentResource& resource_map() noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Hidden member variables
/************************************************************************/

    /**
     * \brief   Component information object, which contains
     *          component address and registered worker thread list.
     **/
    ComponentInfo   mComponentInfo;

    /**
     * \brief   The calculated number of component.
     **/
    uint32_t        mMagicNum;

private:
/************************************************************************/
// Private member variables
/************************************************************************/
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    Component::ListServers  mServerList;    //!< List of registered server services

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Component() = delete;
    AREG_NOCOPY_NOMOVE( Component );
};

//////////////////////////////////////////////////////////////////////////
// Component class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Component* Component::find_by_name(const String& roleName) noexcept
{
    return (roleName.is_empty() ? nullptr : Component::find_by_number(areg::crc32_calculate(roleName.as_string())));
}

inline Component* Component::find_by_number(uint32_t magicNum) noexcept
{
    ASSERT(magicNum != areg::CHECKSUM_IGNORE);
    return Component::resource_map().find_resource_object(magicNum);
}

inline Component* Component::find_by_address(const ComponentAddress& comAddress) noexcept
{
    Component* result = Component::resource_map().find_resource_object(static_cast<uint32_t>(comAddress.role_name()));
    return (result != nullptr && result->address() == comAddress ? result : nullptr);
}

inline bool Component::exist(const String& roleName) noexcept
{
    ASSERT(roleName.is_empty() == false);
    return Component::resource_map().exist(areg::crc32_calculate(roleName.as_string()));
}

inline void Component::register_service_provider(StubBase& server)
{
    mServerList.push_last(&server);
}

inline ComponentThread & Component::master_thread() noexcept
{
    return mComponentInfo.master_thread();
}

inline DispatcherThread * Component::find_event_consumer( uint32_t whichClass ) const noexcept
{
    return mComponentInfo.find_event_consumer(whichClass);
}

inline const String & Component::role_name() const noexcept
{
    return mComponentInfo.role_name();
}

inline const ComponentAddress& Component::address() const noexcept
{
    return mComponentInfo.address();
}

inline const Component::ListServers & Component::extract_service_addresses() const noexcept
{
    return mServerList;
}

inline Component& Component::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_COMPONENT_HPP
