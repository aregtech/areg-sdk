#ifndef AREG_COMPONENT_REGISTRY_COMPONENTENTRY_HPP
#define AREG_COMPONENT_REGISTRY_COMPONENTENTRY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/registry/ComponentEntry.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *              Declared the following objects:
 *              ComponentEntry    - Defines a component registry entry.
 *              ServiceEntry      - Registry entry for a service interface 
 *              DependencyEntry   - Dependency of client Component from Server Component.
 *              SyncTimer         - Timer synchronization object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Version.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/registry/Lists.hpp"

#include <functional>
#include <any>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class Component;
    class ComponentList;
    class ComponentThread;
    class ComponentThreadEntry;
    class WorkerThreadEntry;
} // namespace areg


namespace areg {
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

/**
* \brief   Returns predefined invalid Service Entry.
**/
[[nodiscard]]
AREG_API const areg::ServiceEntry & invalid_service_entry() noexcept;

/**
 * \brief   Returns predefined invalid service dependency entry.
 **/
[[nodiscard]]
AREG_API const areg::DependencyEntry & invalid_depedency_entry() noexcept;


/**
 * \brief   Returns predefined invalid Component Entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentEntry & invalid_component_entry() noexcept;

/**
 * \brief   Returns predefined invalid Thread Entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentThreadEntry & invalid_thread_entry() noexcept;


} // namespace areg

//////////////////////////////////////////////////////////////////////////
// ComponentEntry inline methods
//////////////////////////////////////////////////////////////////////////

inline const areg::ServiceEntry& areg::ServiceList::operator [] (uint32_t index) const noexcept
{
    return (mListServices.is_valid_index(index) ? mListServices[index] : areg::invalid_service_entry());
}

inline const areg::DependencyEntry& areg::DependencyList::operator [] (uint32_t index) const noexcept
{
    return (mListDependencies.is_valid_index(index) ? mListDependencies[index] : areg::invalid_depedency_entry());
}

inline const areg::ComponentEntry& areg::ComponentList::operator [] (uint32_t index) const noexcept
{
    return (mListComponents.is_valid_index(index) ? mListComponents[index] : areg::invalid_component_entry());
}




#endif // AREG_COMPONENT_REGISTRY_COMPONENTENTRY_HPP
