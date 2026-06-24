#ifndef AREG_COMPONENT_MODEL_HPP
#define AREG_COMPONENT_MODEL_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/Model.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Version.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <functional>
#include <any>

/************************************************************************
 * Declared classes
 ************************************************************************/
namespace areg {
    class ServiceEntry;
    class ServiceList;
    class ComponentThreadEntry;
    class ComponentThreadList;
    class WorkerThreadEntry;
    class WorkerThreadList;
    class DependencyEntry;
    class DependencyList;
    class ComponentEntry;
    class ComponentList;
    class Model;
} // namespace areg

/************************************************************************
 * \brief   In this file following classes are declared:
 *              1. ServiceEntry;
 *              2. ServiceList;
 *              3. ComponentThreadEntry;
 *              4. ComponentThreadList;
 *              5. WorkerThreadEntry;
 *              6. WorkerThreadList;
 *              7. DependencyEntry;
 *              8. DependencyList;
 *              9. ComponentEntry;
 *             10. ComponentList;
 *             11. Model;
 *          These classes are declared as part of Registry and used when
 *          Model is defined and created, where object have descriptions
 *          of Service Interfaces, Components, Threads and dependencies.
 *          For more information, see descriptions bellow.
 ************************************************************************/

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Component;
    class WorkerThread;
    class ComponentThread;
    class WorkerThreadConsumer;
} // namespace areg

/************************************************************************
 * Global types
 ************************************************************************/

/**
 * \brief   Type of Component Load Function. Called to instantiate and start component
 *          ComponentEntry      Component registry entry passed to component constructor.
 *                              The component entry contains list of dependencies, services and component data.
 *          ComponentThread     The component owning thread object.
 * 
 * \example This function should create and return pointer to component object.
 *  [](const areg::ComponentEntry& entry, ComponentThread& ownerThread) -> Component *{
 *      return new MyComponent(entry, ownerThread);
 *  }
 **/
using  FuncCreateComponent  = std::function<areg::Component* (const areg::ComponentEntry& /*entry*/, areg::ComponentThread& /*ownerThread*/)>;

/**
 * \brief   Type of Component Unload Function. Called to stop and delete component
 *          Component           The component to stop and delete.
 *          ComponentEntry      Component registry entry passed to delete function.
 * 
 * \example This function should stop and delete component object.
 *  [] (Component& comp, const areg::ComponentEntry& entry) {
 *      delete &comp;
 *  }
 **/
using FuncDeleteComponent   = std::function<void (areg::Component& /*comp*/, const areg::ComponentEntry& /*entry*/)>;

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       This namespace contains collection of classes to register
 *              components, service interfaces, threads and point
 *              dependencies. These is used when defining Model object
 *              and used to start initialization process when Model
 *              object is loading.
 *
 **/
namespace areg {

//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Registry entry for a service interface implemented by a component; includes service
 *          name and version.
 **/
class AREG_API ServiceEntry
{
//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceEntry() = default;

    ServiceEntry(const ServiceEntry& src) = default;

    ServiceEntry(ServiceEntry&& src) noexcept = default;

    ~ServiceEntry() = default;

    /**
     * \brief   Initializes a service entry with a name and version numbers.
     *
     * \param   serviceName     name of the service interface; must not be empty.
     * \param   major           Major version number.
     * \param   minor           Minor version number.
     * \param   patch           Patch version number.
     **/
    ServiceEntry( const String & serviceName, uint32_t major, uint32_t minor, uint32_t patch );

    /**
     * \brief   Initializes a service entry with a name and version object.
     *
     * \param   serviceName     name of the service interface; must not be empty.
     * \param   version         Version object containing major, minor, and patch numbers.
     **/
    ServiceEntry( const String & serviceName, const Version & version );

//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::ServiceEntry & operator = ( const areg::ServiceEntry & src ) = default;

    areg::ServiceEntry & operator = ( areg::ServiceEntry && src ) noexcept = default;

    [[nodiscard]]
    bool operator == ( const areg::ServiceEntry & other ) const noexcept;

    [[nodiscard]]
    bool operator != ( const areg::ServiceEntry & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class, Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the service entry has a non-empty name and valid version.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class, Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The name of implemented Service Interface
     **/
    String     mName;

    /**
     * \brief   The Version of implemented Service.
     **/
    Version   mVersion;
};

//////////////////////////////////////////////////////////////////////////
// areg::ServiceList class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   List of service entries implemented by a component.
 **/
class AREG_API ServiceList
{
//////////////////////////////////////////////////////////////////////////
// areg::ServiceList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< The list of service entries.
    using ServiceListBase = ArrayList<areg::ServiceEntry>;

    ServiceList() = default;

    ServiceList( const areg::ServiceList & src) = default;

    ServiceList( areg::ServiceList && src ) noexcept = default;

    ~ServiceList() = default;

    /**
     * \brief   Creates a list with a single service entry.
     * \param   entry       Service entry to add as the first element.
     **/
    explicit ServiceList( const areg::ServiceEntry & entry );

//////////////////////////////////////////////////////////////////////////
// areg::ServiceList class, operator
//////////////////////////////////////////////////////////////////////////

    areg::ServiceList & operator = ( const areg::ServiceList & src ) = default;

    areg::ServiceList & operator = ( areg::ServiceList && src ) noexcept = default;

    /**
     * \brief   Accesses a service entry by index.
     **/
    [[nodiscard]]
    inline const areg::ServiceEntry& operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ServiceList class, Attributes and Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the list is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Finds a service entry by name.
     *
     * \param   serviceName     name of the service interface to find.
     * \return  Valid entry if found; invalid entry otherwise.
     **/
    [[nodiscard]]
    const areg::ServiceEntry & service( const String & serviceName ) const noexcept;

    /**
     * \brief   Finds the index of a service entry by name.
     *
     * \param   serviceName     name of the service interface to find.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_service( const String & serviceName ) const noexcept;

    /**
     * \brief   Finds the index of a specific service entry.
     *
     * \param   entry       Service entry to find.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_service(const areg::ServiceEntry & entry) const noexcept;

public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of services.
     */
    ServiceListBase mListServices;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadEntry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Registry entry for a worker thread; specifies master thread, worker thread name,
 *          consumer object, and thread configuration.
 **/
class AREG_API WorkerThreadEntry
{
//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadEntry class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    WorkerThreadEntry();

    WorkerThreadEntry(const areg::WorkerThreadEntry& src) = default;

    WorkerThreadEntry(areg::WorkerThreadEntry&& src) noexcept = default;

    ~WorkerThreadEntry() = default;

    /**
     * \brief   Initializes a worker thread entry with thread names, consumer identification,
     *          and optional watchdog/stack configuration.
     *
     * \param   masterThreadName    name of the master thread where the component runs.
     * \param   workerThreadName    name of the worker thread; must be unique.
     * \param   compRoleName        Role name of the component containing the consumer.
     * \param   compConsumerName    name of the consumer object; must differ from component role
     *                              name.
     * \param   watchdogTimeout     Watchdog timeout in milliseconds; 0 (WATCHDOG_IGNORE)
     *                              disables watchdog.
     * \param   stackSizeKb         Stack size in kilobytes; 0 (DEFAULT_STACK_SIZE) uses system
     *                              default.
     * \param   maxQueue            Event-queue ring capacity; areg::IGNORE_VALUE (0) reads the value
     *                              from configuration, falling back to the built-in default.
     * \param   queueDropEvent      Indicates whether a new event should be dropped if the message queue
     *                              is full or the producer should wait until a free slot appears. If
     *                              `True`, drops the event when the message queue is full. If `False`
     *                              and the message queue is full, the producer waits until a free slot
     *                              is opened. If `Undefined`, reads the value from configuration or uses
     *                              the default value.
     * \param   queueTimeout        The waiting time for the queue free slot when the message queue is full.
     *                              Meaningful only if `queueDropEvent` is `False`. areg::WAIT_INFINITE reads
     *                              the value from configuration.
     **/
    WorkerThreadEntry( const String& masterThreadName
                     , const String& workerThreadName
                     , const String& compRoleName
                     , const String& compConsumerName
                     , const uint32_t watchdogTimeout   = areg::WATCHDOG_IGNORE
                     , const uint32_t stackSizeKb       = areg::DEFAULT_STACK_SIZE
                     , const uint32_t maxQueue          = areg::IGNORE_VALUE
                     , const areg::Bool queueDropEvent  = areg::Bool::Undefined
                     , const uint32_t queueTimeout      = areg::WAIT_INFINITE );

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadEntry class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::WorkerThreadEntry& operator = (const areg::WorkerThreadEntry& src) = default;

    areg::WorkerThreadEntry& operator = (areg::WorkerThreadEntry&& src) noexcept = default;

    [[nodiscard]]
    bool operator == (const areg::WorkerThreadEntry& other) const noexcept;

    [[nodiscard]]
    bool operator != (const areg::WorkerThreadEntry& other) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadEntry class, Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if worker thread and consumer names are non-empty.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadEntry class, Member variables.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The name of Worker Thread
     **/
    String      mThreadName;
    /**
     * \brief   The name of Worker Thread Consumer.
     **/
    String      mConsumerName;
    /**
     * \brief   The watchdog timeout in milliseconds.
     **/
    uint32_t    mWatchdogTimeout;
    /**
     * \brief   The stack size of the worker thread in kilobytes.
     **/
    uint32_t    mStackSizeKb;
    /**
     * \brief   The maximum size of message queue for the thread
     **/
    uint32_t    mMaxQueue;
    /**
     * \brief   The flag indicating whether a new event message should be dropped or the producer
     *          should wait for a free slot if the message queue is full.
     **/
    areg::Bool  mDropOnFull;
    /**
     * \brief   The waiting time in milliseconds for the free slot if the queue is full.
     *          Meaningful only if `mDropOnFull` is `False`. Otherwise, ignored.
     **/
    uint32_t    mQueueTimeout;
};

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadList class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   List of worker thread entries associated with a component.
 **/
class AREG_API WorkerThreadList
{
//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< The list of worker threads.
    using WorkerThreadListBase = ArrayList<areg::WorkerThreadEntry>;

    WorkerThreadList() = default;

    WorkerThreadList(const areg::WorkerThreadList& src) = default;

    WorkerThreadList(areg::WorkerThreadList&& src) noexcept = default;

    ~WorkerThreadList() = default;

    /**
     * \brief   Creates a list with a single worker thread entry.
     *
     * \param   entry       Worker thread entry to add as the first element.
     **/
    explicit WorkerThreadList( const areg::WorkerThreadEntry & entry );

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadList class, Operator
//////////////////////////////////////////////////////////////////////////

    areg::WorkerThreadList & operator = ( const areg::WorkerThreadList & src ) = default;

    areg::WorkerThreadList & operator = ( areg::WorkerThreadList && src ) noexcept = default;

    /**
     * \brief   Accesses a worker thread entry by index.
     **/
    [[nodiscard]]
    inline const areg::WorkerThreadEntry& operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::WorkerThreadList class, Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the list is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Finds a worker thread entry by name.
     *
     * \param   threadName      name of the worker thread entry to find.
     * \return  Valid entry if found; invalid entry otherwise.
     **/
    [[nodiscard]]
    const areg::WorkerThreadEntry & worker_thread( const String & threadName ) const noexcept;

    /**
     * \brief   Finds the index of a worker thread entry by name.
     *
     * \param   threadName      name of the worker thread entry to find.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_thread( const String & threadName ) const noexcept;

    /**
     * \brief   Finds the index of a specific worker thread entry.
     *
     * \param   entry       Worker thread entry to find.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_thread( const areg::WorkerThreadEntry & entry ) const noexcept;

public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of worker threads.
     */
    WorkerThreadListBase    mListWorkers;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::DependencyEntry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Defines dependency of client Component from Server Component. Contains Role name of
 *          dependent Server Component.
 **/
class AREG_API DependencyEntry
{
//////////////////////////////////////////////////////////////////////////
// areg::DependencyEntry class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    DependencyEntry() = default;

    DependencyEntry(const areg::DependencyEntry& src) = default;

    DependencyEntry(areg::DependencyEntry&& src) noexcept = default;

    ~DependencyEntry() = default;

    /**
     * \brief   Sets the role name of dependent Component.
     * \param   roleName    The Role name of dependent Component to set.
     **/
    explicit DependencyEntry( const String & roleName );

//////////////////////////////////////////////////////////////////////////
// areg::DependencyEntry class, Operators
//////////////////////////////////////////////////////////////////////////
public:

    areg::DependencyEntry & operator = ( const DependencyEntry & src ) = default;

    areg::DependencyEntry & operator = ( DependencyEntry && src ) noexcept = default;

    [[nodiscard]]
    bool operator == ( const areg::DependencyEntry & other ) const noexcept;

    [[nodiscard]]
    bool operator != ( const areg::DependencyEntry & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::DependencyEntry class, Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if entry is valid. The entry is valid if role name is not empty.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Sets the role name of dependent service.
     *
     * \param   roleName    The role name of dependent service. If empty, invalidates
     *                      dependency.
     **/
    void set_dependent_service( const String & roleName );

    /**
     * \brief   Returns the role name of dependent service.
     **/
    [[nodiscard]]
    const String & depdendent_service() const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::DependencyEntry class, Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The Role name of dependent Component
     **/
    String     mRoleName;
};

//////////////////////////////////////////////////////////////////////////
// areg::DependencyList class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   List of dependency entries defining client-server relationships between components.
 **/
class AREG_API DependencyList
{
//////////////////////////////////////////////////////////////////////////
// areg::DependencyList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< The base class of dependency list
    using DependencyListBase = ArrayList<areg::DependencyEntry>;

    DependencyList() = default;

    DependencyList(const areg::DependencyList& src) = default;

    DependencyList(areg::DependencyList&& src) noexcept = default;

    ~DependencyList() = default;

    /**
     * \brief   Creates a list with a single entry if the entry is valid; empty otherwise.
     * \param   entry       Dependency entry; ignored if invalid.
     **/
    explicit DependencyList( const areg::DependencyEntry & entry );

//////////////////////////////////////////////////////////////////////////
// areg::DependencyList class, Operator
//////////////////////////////////////////////////////////////////////////

    areg::DependencyList & operator = ( const areg::DependencyList & src ) = default;

    areg::DependencyList & operator = ( areg::DependencyList && src ) noexcept = default;

    /**
     * \brief   Accesses a dependency entry by index.
     **/
    [[nodiscard]]
    inline const areg::DependencyEntry& operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Finds a dependency entry by component role name.
     *
     * \param   roleName    Role name of the dependent component.
     * \return  Valid entry if found; invalid entry otherwise.
     **/
    [[nodiscard]]
    const areg::DependencyEntry & dependency( const String & roleName ) const noexcept;

    /**
     * \brief   Finds the index of a dependency entry by role name.
     *
     * \param   roleName    Role name of the dependent component.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_dependency( const String & roleName ) const noexcept;

    /**
     * \brief   Finds the index of a specific dependency entry.
     *
     * \param   entry       Dependency entry to find.
     * \return  Zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_dependency( const areg::DependencyEntry & entry ) const noexcept;

    /**
     * \brief   Returns true if the list is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of dependency services.
     */
    DependencyListBase  mListDependencies;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Defines a component registry entry containing the role name, component
 *          creation/deletion methods, and lists of implemented services, worker threads, and
 *          service dependencies.
 **/
class AREG_API ComponentEntry
{
//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class, Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ComponentLoader;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ComponentEntry();

    ComponentEntry(const areg::ComponentEntry& src);

    ComponentEntry(areg::ComponentEntry&& src) noexcept;

    ~ComponentEntry() = default;

    /**
     * \brief   Initializes a ComponentEntry with thread name, role name, and component
     *          lifecycle functions.
     *
     * \param   masterThreadName    The name of the master thread where the component runs.
     * \param   roleName            The unique role name of the component.
     * \param   funcCreate          Pointer to the component creation function.
     * \param   funcDelete          Pointer to the component deletion function.
     **/
    ComponentEntry( const String & masterThreadName, const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

    /**
     * \brief   Initializes a ComponentEntry with thread name, role name, lifecycle functions,
     *          and service/dependency/worker lists.
     *
     * \param   masterThreadName    The name of the master thread where the component runs.
     * \param   roleName            The unique role name of the component.
     * \param   funcCreate          Pointer to the component creation function.
     * \param   funcDelete          Pointer to the component deletion function.
     * \param   serviceList         The list of implemented service interfaces.
     * \param   dependencyList      The list of dependent service interfaces.
     * \param   workerList          The list of worker threads.
     **/
    ComponentEntry( const String & masterThreadName
                  , const String & roleName
                  , FuncCreateComponent funcCreate
                  , FuncDeleteComponent funcDelete
                  , const areg::ServiceList & serviceList
                  , const areg::DependencyList & dependencyList
                  , const areg::WorkerThreadList & workerList);

    /**
     * \brief   Initializes a ComponentEntry with thread name, role name, lifecycle functions,
     *          and individual service/dependency/worker entries.
     *
     * \param   masterThreadName    The name of the master thread where the component runs.
     * \param   roleName            The unique role name of the component.
     * \param   funcCreate          Pointer to the component creation function.
     * \param   funcDelete          Pointer to the component deletion function.
     * \param   service             An entry of an implemented service interface.
     * \param   dependency          An entry of a dependent service interface.
     * \param   worker              An entry of a worker thread.
     **/
    ComponentEntry( const String & masterThreadName
                  , const String & roleName
                  , FuncCreateComponent funcCreate
                  , FuncDeleteComponent funcDelete
                  , const areg::ServiceEntry & service
                  , const areg::DependencyEntry & dependency
                  , const areg::WorkerThreadEntry & worker);

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::ComponentEntry & operator = ( const areg::ComponentEntry & src );

    areg::ComponentEntry & operator = ( areg::ComponentEntry && src ) noexcept;

    [[nodiscard]]
    bool operator == ( const areg::ComponentEntry & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class, Attributes and Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the ComponentEntry is valid (role name is not empty).
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Adds a supported service entry to the component.
     *
     * \param   entry       The service entry to add.
     **/
    void add_supported_service( const areg::ServiceEntry & entry );

    /**
     * \brief   Adds a list of supported service entries to the component.
     *
     * \param   serviceList     The list of service entries to add.
     **/
    void add_supported_service( const areg::ServiceList & serviceList );

    /**
     * \brief   Adds a supported service interface by name and version, ensuring uniqueness
     *          within the component.
     *
     * \param   serviceName     The name of the service interface. Must be unique within the
     *                          component.
     * \param   version         The version of the service interface.
     * \return  Returns a reference to the newly added or existing service entry.
     **/
    areg::ServiceEntry & add_supported_service( const String & serviceName, const Version & version );

    /**
     * \brief   Removes a supported service from the component by name.
     *
     * \param   serviceName     The name of the service to remove.
     * \return  Returns true if the service was found and removed; false otherwise.
     **/
    bool remove_supported_service( const String & serviceName ) noexcept;

    /**
     * \brief   Searches for a supported service entry and returns its zero-based index if
     *          found.
     *
     * \param   entry       The service entry to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_supported_service( const areg::ServiceEntry & entry ) const noexcept;

    /**
     * \brief   Searches for a supported service by name and returns its zero-based index if
     *          found.
     *
     * \param   serviceName     The name of the service to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_supported_service( const String & serviceName ) const noexcept;

    /**
     * \brief   Adds a worker thread entry to the component.
     *
     * \param   entry       The worker thread entry to add.
     **/
    void add_worker_thread( const areg::WorkerThreadEntry & entry );

    /**
     * \brief   Adds a list of worker thread entries to the component.
     *
     * \param   workerList      The list of worker thread entries to add.
     **/
    void add_worker_thread( const areg::WorkerThreadList & workerList );

    /**
     * \brief   Searches for a worker thread entry and returns its zero-based index if found.
     *
     * \param   entry       The worker thread entry to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_worker_thread( const areg::WorkerThreadEntry & entry ) const noexcept;

    /**
     * \brief   Searches for a worker thread by name and returns its zero-based index if found.
     *
     * \param   workerName      The name of the worker thread to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_worker_thread( const String & workerName ) const noexcept;

    /**
     * \brief   Removes a worker thread from the component by name.
     *
     * \param   workerName      The name of the worker thread to remove.
     * \return  Returns true if the worker thread was found and removed; false otherwise.
     **/
    bool remove_worker_thread( const String & workerName ) noexcept;

    /**
     * \brief   Adds a service dependency entry to the component.
     *
     * \param   entry       The dependency entry to add.
     **/
    void add_dependency_service( const areg::DependencyEntry & entry );

    /**
     * \brief   Adds a list of service dependency entries to the component.
     *
     * \param   dependencyList      The list of dependency entries to add.
     **/
    void add_dependency_service( const areg::DependencyList & dependencyList );

    /**
     * \brief   Adds a service dependency by role name of the server component.
     *
     * \param   roleName    The role name of the dependent service.
     * \return  Returns a reference to the newly added or existing dependency entry.
     **/
    areg::DependencyEntry & add_dependency_service( const String & roleName );

    /**
     * \brief   Searches for a dependency entry and returns its zero-based index if found.
     *
     * \param   entry       The dependency entry to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_dependency_service( const areg::DependencyEntry & entry ) const noexcept;

    /**
     * \brief   Removes a service dependency from the component by role name.
     *
     * \param   roleName    The role name of the dependency to remove.
     * \return  Returns true if the dependency was found and removed; false otherwise.
     **/
    bool remove_dependency_service( const String & roleName ) noexcept;

    /**
     * \brief   Searches for a dependency by role name and returns its zero-based index if
     *          found.
     *
     * \param   roleName    The role name of the dependency to search for.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_dependency_service(const String & roleName) const noexcept;

    /**
     * \brief   Returns the list of supported services in the component.
     **/
    [[nodiscard]]
    const areg::ServiceList & supported_services() const noexcept;

    /**
     * \brief   Returns the list of worker threads in the component.
     **/
    [[nodiscard]]
    const areg::WorkerThreadList & worker_threads() const noexcept;

    /**
     * \brief   Returns the list of service dependencies in the component.
     **/
    [[nodiscard]]
    const areg::DependencyList & dependency_services() const noexcept;

    /**
     * \brief   Sets the component creation and deletion function pointers.
     *
     * \param   fnCreate    Pointer to the component creation function; nullptr to clear.
     * \param   fnDelete    Pointer to the component deletion function; nullptr to clear.
     **/
    void set_instance_methods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete ) noexcept;

    /**
     * \brief   Sets component data to be passed to the creation function.
     *
     * \param   compData    The data to associate with the component.
     * \note    Manually free memory if the data was manually allocated.
     **/
    void set_data( std::any compData ) noexcept;

    /**
     * \brief   Resets all component entry fields to their default values.
     **/
    void reset() noexcept;

    /**
     * \brief   Returns the component data; verify it holds a value before use.
     **/
    [[nodiscard]]
    std::any data() const noexcept;

    /**
     * \brief   Returns true if component data is set; false otherwise.
     **/
    [[nodiscard]]
    bool has_data() const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntry class, Member variables
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The Role name of Component
     **/
    String              mRoleName;
    /**
     * \brief   The Master Thread Entry of Component.
     **/
    String              mThreadName;
    /**
     * \brief   List of supported services
     **/
    ServiceList         mSupportedServices;
    /**
     * \brief   List of worker threads
     **/
    WorkerThreadList    mWorkerThreads;
    /**
     * \brief   List of dependencies
     **/
    DependencyList      mDependencyServices;
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Pointer of function to create component
     **/
    FuncCreateComponent mFuncCreate;
    /**
     * \brief   Pointer of function to delete component
     **/
    FuncDeleteComponent mFuncDelete;
    /**
     * \brief   The data to pass to component create method.
     * \note    You should manually free memory if the data was manually allocated in the memory
     **/
    mutable std::any    mComponentData;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::ComponentEntryList class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   List of component entries within a thread; all components must have unique role
 *          names and service interfaces.
 **/
class AREG_API ComponentList
{
//////////////////////////////////////////////////////////////////////////
// areg::ComponentList class, Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ComponentLoader;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< The base class of component list.
    using ComponentListBase = ArrayList<areg::ComponentEntry>;

    ComponentList() = default;

    ComponentList(const areg::ComponentList& src) = default;

    ComponentList(areg::ComponentList&& src) noexcept = default;

    ~ComponentList() = default;

    /**
     * \brief   Creates a component list with the given component entry as the first element.
     * \param   entry       The component entry to add as the first element.
     **/
    explicit ComponentList(  const areg::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// areg::ComponentList class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::ComponentList & operator = ( const areg::ComponentList & src ) = default;

    areg::ComponentList & operator = ( areg::ComponentList && src ) noexcept = default;

    /**
     * \brief   Returns the component entry at the given index for read-only access.
     **/
    [[nodiscard]]
    inline const areg::ComponentEntry& operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentList class, Attributes and Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the component list is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Returns the component entry with the given role name.
     *
     * \param   roleName    The role name to search for.
     * \return  Returns the component entry if found; otherwise returns an invalid component
     *          entry.
     **/
    [[nodiscard]]
    const areg::ComponentEntry & component( const String & roleName ) const noexcept;

    /**
     * \brief   Sets component data for the component with the given role name.
     *
     * \param   roleName    The role name of the component.
     * \param   compData    The data to set (passed to component creation method).
     * \return  Returns true if component was found and data was set; false otherwise.
     * \note    Caller must manually free allocated memory.
     **/
    bool set_component_data( const String & roleName, std::any compData ) noexcept;

    /**
     * \brief   Resets component data for the component with the given role name.
     *
     * \param   roleName    The role name of the component.
     * \return  Returns true if component was found and data was reset; false otherwise.
     **/
    bool reset( const String& roleName ) noexcept;

    /**
     * \brief   Returns the zero-based index of the component with the given role name.
     *
     * \param   roleName    The role name to search for.
     * \return  Returns the index if found; -1 if not found.
     **/
    [[nodiscard]]
    int32_t find_component( const String & roleName ) const noexcept;

    /**
     * \brief   Returns the zero-based index of the given component entry.
     *
     * \param   entry       The component entry to search for.
     * \return  Returns the index if found; -1 if not found.
     **/
    [[nodiscard]]
    int32_t find_component(const areg::ComponentEntry& entry) const noexcept;

public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of components.
     */
    ComponentListBase   mListComponents;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Defines Master Thread of one or more registered Component. All registered Components
 *          are created in specified thread. One Thread should have at least one registered
 *          Component. All registered Components should have unique Role Names and different
 *          Service Names.
 **/
class AREG_API ComponentThreadEntry
{
//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class, Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ComponentLoader;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ComponentThreadEntry();

    ComponentThreadEntry(const areg::ComponentThreadEntry& src) = default;

    ComponentThreadEntry(areg::ComponentThreadEntry&& src) noexcept = default;

    ~ComponentThreadEntry() = default;

    /**
     * \brief   Initializes Thread Entry with given Thread name and optional thread parameters.
     *
     * \param   threadName          The Thread name to assign.
     * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component
     *                              thread. The value 0 (areg::WATCHDOG_IGNORE) ignores
     *                              watchdog.
     * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024
     *                              Bytes). Pass `areg::DEFAULT_STACK_SIZE` (0) to ignore
     *                              changing stack size and use system default stack size.
     * \param   maxQueue            The maximum size of message queue for the thread. Pass
     *                              areg::IGNORE_VALUE to use default value set in
     *                              configuration or ignore the parameter if not configured.
     * \param   queueDropEvent          Indicates wether new event should be dropped if message queue
     *                              is full or wait until free slot appears. If `True`, drops event
     *                              if message queue is full. If `False` and the message queue is full
     *                              producer waits for until free slot is opened. If `Undefined`,
     *                              read data from configuration file or use the default value.
     * \param   queueTimeout    The waiting time for the queue free slot when the message queue is full.
     *                              Meaningful only if `queueDropEvent` is `True`. Otherwise, it is ignored.
     **/
    explicit ComponentThreadEntry( const String & threadName
                                 , const uint32_t watchdogTimeout   = areg::WATCHDOG_IGNORE
                                 , const uint32_t stackSizeKb       = areg::DEFAULT_STACK_SIZE
                                 , const uint32_t maxQueue          = areg::IGNORE_VALUE
                                 , const areg::Bool queueDropEvent  = areg::Bool::Undefined
                                 , const uint32_t queueTimeout      = areg::WAIT_INFINITE );

    /**
     * \brief   Initializes Thread Entry with given Thread name and given Component List.
     *
     * \param   threadName          The Thread name to assign.
     * \param   componentList       The List of Component Entries to assign.
     * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component
     *                              thread. The value 0 (areg::WATCHDOG_IGNORE) ignores
     *                              watchdog.
     * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024
     *                              Bytes). Pass `areg::DEFAULT_STACK_SIZE` (0) to ignore
     *                              changing stack size and use system default stack size.
     * \param   maxQueue            The maximum size of message queue for the thread.
     * \param   queueDropEvent          Indicates wether new event should be dropped if message queue
     *                              is full or wait until free slot appears. If `True`, drops event
     *                              if message queue is full. If `False` and the message queue is full
     *                              producer waits for until free slot is opened. If `Undefined`,
     *                              read data from configuration file or use the default value.
     * \param   queueTimeout        The waiting time for the queue free slot when the message queue is full.
     *                              Meaningful only if `queueDropEvent` is `True`. Otherwise, it is ignored.
     **/
    ComponentThreadEntry( const String & threadName
                        , const areg::ComponentList & componentList
                        , const uint32_t watchdogTimeout    = areg::WATCHDOG_IGNORE
                        , const uint32_t stackSizeKb        = areg::DEFAULT_STACK_SIZE
                        , const uint32_t maxQueue           = areg::IGNORE_VALUE
                        , const areg::Bool queueDropEvent   = areg::Bool::Undefined
                        , const uint32_t queueTimeout       = areg::WAIT_INFINITE );

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::ComponentThreadEntry & operator = ( const areg::ComponentThreadEntry & src ) = default;

    areg::ComponentThreadEntry & operator = ( areg::ComponentThreadEntry && src ) noexcept = default;

    [[nodiscard]]
    bool operator == ( const areg::ComponentThreadEntry & other ) const noexcept;

    [[nodiscard]]
    bool operator != ( const areg::ComponentThreadEntry & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class, Attribute and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if Thread Entry is valid. The Entry is valid if name of Thread
     *          Entry is not empty.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Adds supported Component Entry in Thread Entry object. A Thread may have more
     *          than one Component with unique Role name, and every Component within one Thread
     *          should have implementation of different Service Interfaces. If there is a
     *          Service Interface with different implementation, they should be defined in
     *          different threads. For example, if Service Interface "QWERT" is implemented in
     *          Component ABC and XYZ, they should not be in the same Thread Entry, but can be
     *          in different Thread. Otherwise, same Event will be processed twice in every Stub
     *          of same Service Interface.
     *
     * \param   entry       The Component Entry, defining Component in Thread.
     **/
    void add_component( const areg::ComponentEntry & entry );

    /**
     * \brief   Adds List of supported Component Entries in Thread Entry object. A Thread may
     *          have more than one Component with unique Role name, and every Component within
     *          one Thread should have implementation of different Service Interfaces. If there
     *          is a Service Interface with different implementation, they should be defined in
     *          different threads. For example, if Service Interface "QWERT" is implemented in
     *          Component ABC and XYZ, they should not be in the same Thread Entry, but can be
     *          in different Thread. Otherwise, same Event will be processed twice in every Stub
     *          of same Service Interface.
     *
     * \param   componentList       The List of Component Entries. Every Component Entry is
     *                              defining Component in Thread.
     **/
    void add_component( const areg::ComponentList & componentList );

    /**
     * \brief   Adds a new component entry with the given role.
     *
     * \param   roleName        The name of component to add. The name must be unique to add new
     *                          entry.
     * \param   funcCreate      The pointer to the method that instantiates the component.
     * \param   funcDelete      The pointer to the method that deletes the component.
     * \return  Returns instance of new added or the instance of the existing component entry
     *          with the given role name. The checkup happens only within the current thread
     *          list.
     * \note    NOTE: The method does not check the uniqueness of role name within the entire
     *          system or entire model. It checks only within the current component thread entry
     *          context.
     **/
    areg::ComponentEntry & add_component( const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

    /**
     * \brief   Adds a new component entry with the given role.
     *
     * \param   roleName    The name of component to add. The name must be unique to add new
     *                      entry.
     * \return  Returns instance of new added or the instance of the existing component entry
     *          with the given role name.
     * \note    NOTE: The method does not check the uniqueness of role name within the entire
     *          system or entire model. It checks only within the current component thread entry
     *          context.
     **/
    template<typename ComponentType>
    inline areg::ComponentEntry& add_component(const String& roleName);

    /**
     * \brief   Searches the component entry by given role name. If found, remove the component
     *          entry from the list.
     *
     * \param   roleName    The roleName of service component to search and remove.
     * \return  Returns true if found and could remove the component entry.
     **/
    bool remove_component( const String & roleName ) noexcept;

    /**
     * \brief   Searches Component Entry in the existing list of Thread Entry and returns the
     *          zero-based valid position if found. Otherwise, returns -1, if cannot find
     *          Component Entry in the list.
     *
     * \param   entry       The Component Entry to search in the list.
     * \return  If Entry found, returns valid zero-based index of element. Otherwise, returns -1.
     **/
    [[nodiscard]]
    int32_t find_component_entry( const areg::ComponentEntry & entry ) const noexcept;

    /**
     * \brief   By given name, searches Component Entry in the existing list of Thread Entry and
     *          returns the zero-based valid position if found. Returns -1, if cannot find
     *          Component Entry in the list.
     *
     * \param   roleName    The name of Component Entry to search in the list.
     * \return  If Entry found, returns valid zero-based index of element. Otherwise, returns -1.
     **/
    [[nodiscard]]
    int32_t find_component_entry( const String & roleName ) const noexcept;

    /**
     * \brief   Searches in the list the component by given name. If found, sets component data.
     *          Returns true if found component and the data was successfully set.
     *
     * \param   roleName    The name of component to search in the list.
     * \param   compData    The data to set in component which is passed to create method.
     * \note    You should manually free memory if the data was manually allocated in the memory
     **/
    bool set_component_data( const String & roleName, std::any compData ) noexcept;

    /**
     * \brief   Searches in the list the component by given name. If found, resets component
     *          data. Returns true if found component and the data was successfully reset.
     *
     * \param   roleName    The name of component to search in the list.
     **/
    bool reset( const String & roleName ) noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadEntry class, Member variables
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   The name of thread component (thread name)
     **/
    String          mThreadName;

    /**
     * \brief   List of component entries
     **/
    ComponentList   mComponents;

    /**
     * \brief   The watchdog timeout in milliseconds.
     **/
    uint32_t        mWatchdogTimeout;

    /**
     * \brief   The size of the thread stack in kilobytes.
     **/
    uint32_t        mStackSizeKB;

    /**
     * \brief   The maximum size of message queue for the thread.
     **/
    uint32_t        mMaxQueue;

    /**
     * \brief   The flag indicating whether new event message should be dropped or the producer
     *          should wait for the free slot if message queue is full.
     */
    areg::Bool      mDropOnFull;

    /**
     * \brief   The waiting time in milliseconds for the free slot if queue is full.
     *          Meaningful only if `mDropOnFull` is `True`. Otherwise, ignored.
     **/
    uint32_t        mQueueTimeout;
};

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadList class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   List of thread entries in a model. Each thread name must be unique.
 **/
class AREG_API ComponentThreadList
{
//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadList class, Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ComponentLoader;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadList class, Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< The base class of component thread list.
    using ComponentThreadListBase = ArrayList< areg::ComponentThreadEntry>;

    ComponentThreadList() = default;

    ComponentThreadList( const areg::ComponentThreadList & src ) = default;

    ComponentThreadList( areg::ComponentThreadList && src ) noexcept = default;

    ~ComponentThreadList() = default;

    /**
     * \brief   Creates Component Thread List and adds specified Thread Entry to the list as first element.
     * \param   entry       The Thread Entry to set as first element in the list.
     **/
    explicit ComponentThreadList( const areg::ComponentThreadEntry & entry );

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadList class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::ComponentThreadList & operator = ( const areg::ComponentThreadList & src ) = default;

    areg::ComponentThreadList & operator = ( areg::ComponentThreadList && src ) noexcept = default;

    /**
     * \brief   Accesses element at given index by position.
     **/
    [[nodiscard]]
    inline const areg::ComponentThreadEntry& operator [] (uint32_t index) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::ComponentThreadList class, Attributes and Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if Thread List is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Searches Thread Entry having given name. Returns valid Thread Entry if found,
     *          otherwise returns invalid thread entry. There should not be several Thread
     *          Entries with same name.
     *
     * \param   threadName      The name of Thread Entry to search.
     * \return  Returns valid Thread Entry if found. Otherwise returns invalid thread entry.
     **/
    [[nodiscard]]
    const areg::ComponentThreadEntry & thread( const String & threadName ) const noexcept;

    /**
     * \brief   Searches Thread Entry by given Thread name and returns zero-based valid index if
     *          found. The valid index is not negative.
     *
     * \param   threadName      The Thread name of Thread Entry to search.
     * \return  Returns zero-based valid index if found entry. Otherwise, returns -1.
     **/
    [[nodiscard]]
    int32_t find_thread( const String & threadName ) const noexcept;

    /**
     * \brief   Searches specified Thread Entry and returns zero-based valid index if found. The
     *          valid index is not negative.
     *
     * \param   entry       The Thread Entry to search in the list.
     * \return  Returns zero-based valid index if found entry. Otherwise, returns -1.
     **/
    [[nodiscard]]
    int32_t find_thread( const areg::ComponentThreadEntry & entry ) const noexcept;

public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The list of component threads.
     */
    ComponentThreadListBase mListThreads;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// areg::Model class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Defines the model of a project/binary, specifying threads, components, and their
 *          dependencies. Each application can have only one model; multiple models must be
 *          defined in separate projects/binaries.
 **/
class AREG_API Model
{
//////////////////////////////////////////////////////////////////////////
// areg::Model class, Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class ComponentLoader;

    enum class ModelState : uint8_t
    {
          Initialized
        , Loaded
        , Unloaded
    };

//////////////////////////////////////////////////////////////////////////
// areg::Model class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Model();

    Model( const Model & src ) = default;

    Model( Model && src ) noexcept = default;

    ~Model() = default;

    /**
     * \brief   Creates an empty model and sets the name.
     **/
    explicit Model( const String & modelName );

    /**
     * \brief   Creates a model, sets the name, and registers the thread list.
     **/
    Model( const String & modelName, const ComponentThreadList & threadList );

//////////////////////////////////////////////////////////////////////////
// areg::Model class, Operators
//////////////////////////////////////////////////////////////////////////

    areg::Model & operator = ( const areg::Model & src ) = default;

    areg::Model & operator = ( areg::Model && src ) noexcept = default;

    [[nodiscard]]
    bool operator == ( const areg::Model & other ) const noexcept;

    [[nodiscard]]
    bool operator != ( const areg::Model & other ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::Model class, Attributes and Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the model is valid (non-empty name and at least one thread
     *          entry).
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Returns the name of the model.
     **/
    [[nodiscard]]
    const String & model_name() const noexcept;

    /**
     * \brief   Returns true if the specified component entry is registered in the model.
     *
     * \param   entry       The component entry to search in the model.
     * \return  Returns true if the component entry is found; false otherwise.
     **/
    [[nodiscard]]
    bool has_registered_component( const areg::ComponentEntry & entry ) const noexcept;

    /**
     * \brief   Returns true if a component with the specified role name is registered in the
     *          model.
     *
     * \param   roleName    The role name of the component entry to search in the model.
     * \return  Returns true if a component with the specified role name is found; false
     *          otherwise.
     **/
    [[nodiscard]]
    bool has_registered_component( const String & roleName ) const noexcept;

    /**
     * \brief   Returns true if the model is loaded; false otherwise.
     **/
    [[nodiscard]]
    bool is_model_loaded() const noexcept;

    /**
     * \brief   Returns the list of thread entries registered in the model.
     **/
     [[nodiscard]]
   const areg::ComponentThreadList & thread_list() const noexcept;

    /**
     * \brief   Marks the model as loaded or unloaded.
     *
     * \param   isLoaded    If true, marks the model as loaded; if false, marks it as unloaded.
     **/
    void mark_model_loaded( bool isLoaded = true ) noexcept;

    /**
     * \brief   Marks the model as alive and starts the timer, or stops the timer when no longer
     *          alive to calculate the duration.
     *
     * \param   is_alive     If true, marks the model as alive and starts the timer; if false,
     *                      stops the timer.
     **/
    void mark_model_alive( bool is_alive) noexcept;

    /**
     * \brief   Adds a thread entry to the model. The entry must have a globally unique name.
     *
     * \param   entry       The thread entry to add.
     **/
    void add_thread( const areg::ComponentThreadEntry & entry );

    /**
     * \brief   Adds multiple thread entries to the model. Each entry must have a globally
     *          unique name.
     *
     * \param   threadList      The list of thread entries to add.
     **/
    void add_thread( const areg::ComponentThreadList & threadList );

    /**
     * \brief   Adds a new thread entry with the specified name, or returns the existing one if
     *          already registered.
     *
     * \param   threadName      The name of the thread to add. The name must be unique.
     * \return  Returns a reference to the component thread entry (new or existing).
     * \note    The method only checks uniqueness within the current model context and does not
     *          verify uniqueness across the entire system or worker thread lists. Callers must
     *          ensure global uniqueness.
     **/
    areg::ComponentThreadEntry & add_thread(const String & threadName );

    /**
     * \brief   Removes the thread entry with the specified name from the model.
     *
     * \param   threadName      The name of the thread entry to search and remove.
     * \return  Returns true if the thread was found and removed; false otherwise.
     **/
    bool remove_thread( const String & threadName ) noexcept;

    /**
     * \brief   Searches for a thread entry in the model and returns its zero-based index.
     *
     * \param   entry       The thread entry to search for in the model.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_thread( const areg::ComponentThreadEntry & entry ) const noexcept;

    /**
     * \brief   Searches for a thread entry by name in the model and returns its zero-based
     *          index.
     *
     * \param   threadName      The name of the thread entry to search for in the model.
     * \return  Returns the zero-based index if found; -1 otherwise.
     **/
    [[nodiscard]]
    int32_t find_thread(const String & threadName) const noexcept;

    /**
     * \brief   Searches for a component by name and sets its data.
     *
     * \param   roleName    The name of the component to search.
     * \param   compData    The data to set in the component, passed to its create method.
     * \return  Returns true if the component was found and data was set; false otherwise.
     * \note    Manually free memory if the data was manually allocated.
     **/
    bool set_component_data( const String & roleName, std::any compData );

    /**
     * \brief   Searches for a component by name and resets its data.
     *
     * \param   roleName    The name of the component to search.
     * \return  Returns true if the component was found and data was reset; false otherwise.
     **/
    bool reset(const String& roleName) noexcept;

    /**
     * \brief   Returns the duration in nanoseconds the model was alive (loaded). Returns zero
     *          if the model was never loaded; returns the current duration if still alive;
     *          returns the last duration if no longer alive.
     **/
    [[nodiscard]]
    inline TIME64 alive_duration() const noexcept;

//////////////////////////////////////////////////////////////////////////
// areg::Model class, Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of Model
     **/
    String                  mModelName;

    /**
     * \brief   The List of Thread Entries in Model
     **/
    ComponentThreadList     mModelThreads;

    /**
     * \brief   The Flag, indicating whether model is loaded or not.
     **/
    ModelState             mLoadState;

    /**
     * \brief   The duration of time where model was loaded and alive.
     **/
    areg::Duration   mAliveDuration;
};

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace Predefined Invalid Entries.
//////////////////////////////////////////////////////////////////////////

 /**
  * \brief   Returns predefined invalid Service Entry.
  **/
[[nodiscard]]
AREG_API const areg::ServiceEntry & invalid_service_entry() noexcept;

/**
 * \brief   Returns predefined invalid Service List.
 **/
[[nodiscard]]
AREG_API const areg::ServiceList & invalid_service_list() noexcept;

/**
 * \brief   Returns predefined invalid Worker Thread Entry.
 **/
[[nodiscard]]
AREG_API const areg::WorkerThreadEntry & invalid_worker_entry() noexcept;

/**
 * \brief   Returns predefined invalid Worker Thread List.
 **/
[[nodiscard]]
AREG_API const areg::WorkerThreadList & invalid_worker_list() noexcept;

/**
 * \brief   Returns predefined invalid service dependency entry.
 **/
[[nodiscard]]
AREG_API const areg::DependencyEntry & invalid_depedency_entry() noexcept;

/**
 * \brief   Returns predefined invalid service dependency list.
 **/
[[nodiscard]]
AREG_API const areg::DependencyList & invalid_depedency_list() noexcept;

/**
 * \brief   Returns predefined invalid Component Entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentEntry & invalid_component_entry() noexcept;

/**
 * \brief   Returns predefined invalid Component List.
 **/
[[nodiscard]]
AREG_API const areg::ComponentList & invalid_component_list() noexcept;

/**
 * \brief   Returns predefined invalid Thread Entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentThreadEntry & invalid_thread_entry() noexcept;

/**
 * \brief   Returns predefined invalid Thread List containing single invalid thread entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentThreadList & invalid_thread_list() noexcept;

/**
 * \brief   Returns predefined invalid Model.
 **/
AREG_API const areg::Model & invalid_model() noexcept;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// NERegistry inline methods
//////////////////////////////////////////////////////////////////////////

inline const areg::ServiceEntry& areg::ServiceList::operator [] (uint32_t index) const noexcept
{
    return (mListServices.is_valid_index(index) ? mListServices[index] : areg::invalid_service_entry());
}

inline const areg::WorkerThreadEntry& areg::WorkerThreadList::operator [] (uint32_t index) const noexcept
{
    return (mListWorkers.is_valid_index(index) ? mListWorkers[index] : areg::invalid_worker_entry());
}

inline const areg::DependencyEntry& areg::DependencyList::operator [] (uint32_t index) const noexcept
{
    return (mListDependencies.is_valid_index(index) ? mListDependencies[index] : areg::invalid_depedency_entry());
}

inline const areg::ComponentEntry& areg::ComponentList::operator [] (uint32_t index) const noexcept
{
    return (mListComponents.is_valid_index(index) ? mListComponents[index] : areg::invalid_component_entry());
}

inline const areg::ComponentThreadEntry& areg::ComponentThreadList::operator [] (uint32_t index) const noexcept
{
    return (mListThreads.is_valid_index(index) ? mListThreads[index] : areg::invalid_thread_entry());
}

inline TIME64 areg::Model::alive_duration() const noexcept
{
    return (mLoadState == ModelState::Initialized ? 0 : mAliveDuration.duration_since_start());
}

template<typename ComponentType>
inline areg::ComponentEntry& areg::ComponentThreadEntry::add_component(const String& roleName)
{
    return add_component(roleName
        , [](const areg::ComponentEntry& entry, ComponentThread& owner) -> Component* {
            return new ComponentType(entry, owner);
        }
        , [](Component& comp, const areg::ComponentEntry& /*entry*/) -> void {
            ComponentType* tmp = static_cast<ComponentType *>(&comp);
            delete tmp;
        });
}

#endif  // AREG_COMPONENT_MODEL_HPP
