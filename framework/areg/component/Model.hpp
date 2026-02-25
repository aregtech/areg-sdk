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
#include "areg/base/GEGlobal.h"
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
namespace NERegistry
{
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
}

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
class Component;
class WorkerThread;
class ComponentThread;
class WorkerThreadConsumer;

/************************************************************************
 * Global types
 ************************************************************************/

/**
 * \brief   Type of Component Load Function. Called to instantiate and start component
 * \type NERegistry::ComponentEntry     Component registry entry passed to component constructor.
 *                                      The component entry contains list of dependencies, services and component data.
 * \type ComponentThread                The component owning thread object.
 * \example This function should create and return pointer to component object.
 *  [](const NERegistry::ComponentEntry& entry, ComponentThread& ownerThread) -> Component *{
 *      return new MyComponent(entry, ownerThread);
 *  }
 **/
using  FuncCreateComponent  = std::function<Component* (const NERegistry::ComponentEntry& /*entry*/, ComponentThread& /*ownerThread*/)>;

/**
 * \brief   Type of Component Unload Function. Called to stop and delete component
 * \type Component                    The component to stop and delete.
 * \type NERegistry::ComponentEntry   Component registry entry passed to delete function.
 * \example This function should stop and delete component object.
 *  [] (Component& comp, const NERegistry::ComponentEntry& entry) {
 *      delete &comp;
 *  }
 **/
using FuncDeleteComponent   = std::function<void (Component& /*comp*/, const NERegistry::ComponentEntry& /*entry*/)>;

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
namespace NERegistry
{

//////////////////////////////////////////////////////////////////////////
// NERegistry::ServiceEntry class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Registry entry for a service interface implemented by a component; includes service
     *          name and version.
     **/
    class AREG_API ServiceEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an invalid service entry with an empty name.
         **/
        ServiceEntry() = default;

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

        /**
         * \brief   Copies the service entry.
         *
         * \param   src     Source to copy.
         **/
        ServiceEntry( const ServiceEntry & src ) = default;

        /**
         * \brief   Moves the service entry.
         *
         * \param   src     Source to move.
         **/
        ServiceEntry( ServiceEntry && src ) noexcept = default;

        /**
         * \brief   Destructor.
         **/
        ~ServiceEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies the service entry.
         *
         * \param   src     Source to copy.
         **/
        NERegistry::ServiceEntry & operator = ( const NERegistry::ServiceEntry & src ) = default;

        /**
         * \brief   Moves the service entry.
         *
         * \param   src     Source to move.
         **/
        NERegistry::ServiceEntry & operator = ( NERegistry::ServiceEntry && src ) noexcept = default;

        /**
         * \brief   Returns true if service names and versions are equal.
         *
         * \param   other       Service entry to compare.
         **/
        bool operator == ( const NERegistry::ServiceEntry & other ) const;
        /**
         * \brief   Returns true if service entries are not equal.
         *
         * \param   other       Service entry to compare.
         **/
        bool operator != ( const NERegistry::ServiceEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the service entry has a non-empty name and valid version.
         **/
        bool is_valid() const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceEntry class, Member variables
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
    // NERegistry::ServiceList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   List of service entries implemented by a component.
     **/
    class AREG_API ServiceList
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The list of service entries.
        using ServiceListBase = ArrayList<NERegistry::ServiceEntry>;

        /**
         * \brief   Creates an empty service list.
         **/
        ServiceList() = default;

        /**
         * \brief   Creates a list with a single service entry.
         *
         * \param   entry       Service entry to add as the first element.
         **/
        explicit ServiceList( const NERegistry::ServiceEntry & entry );

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        ServiceList( const NERegistry::ServiceList & src) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        ServiceList( NERegistry::ServiceList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ServiceList() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        NERegistry::ServiceList & operator = ( const NERegistry::ServiceList & src ) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        NERegistry::ServiceList & operator = ( NERegistry::ServiceList && src ) noexcept = default;

        /**
         * \brief   Accesses a service entry by index.
         *
         * \param   index       Valid index of the entry.
         * \return  Entry at index; invalid entry if index is out of bounds.
         **/
        inline const NERegistry::ServiceEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ServiceList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the list is valid.
         **/
        bool is_valid() const;

        /**
         * \brief   Finds a service entry by name.
         *
         * \param   serviceName     name of the service interface to find.
         * \return  Valid entry if found; invalid entry otherwise.
         **/
        const NERegistry::ServiceEntry & service( const String & serviceName ) const;

        /**
         * \brief   Finds the index of a service entry by name.
         *
         * \param   serviceName     name of the service interface to find.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_service( const String & serviceName ) const;

        /**
         * \brief   Finds the index of a specific service entry.
         *
         * \param   entry       Service entry to find.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_service(const NERegistry::ServiceEntry & entry) const;

    public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
        /**
         * \brief   The list of services.
         */
        ServiceListBase mListServices;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Registry entry for a worker thread; specifies master thread, worker thread name,
     *          consumer object, and thread configuration.
     **/
    class AREG_API WorkerThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an invalid worker thread entry.
         **/
        WorkerThreadEntry();

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
         * \param   stackSizeKb         Stack size in kilobytes; 0 (STACK_SIZE_DEFAULT) uses system
         *                              default.
         * \param   maxQueue            Maximum message queue size; IGNORE_VALUE uses configured or
         *                              default value.
         **/
        WorkerThreadEntry( const String & masterThreadName
                         , const String & workerThreadName
                         , const String & compRoleName
                         , const String & compConsumerName
                         , const uint32_t watchdogTimeout = NECommon::WATCHDOG_IGNORE
                         , const uint32_t stackSizeKb     = NECommon::STACK_SIZE_DEFAULT
                         , const uint32_t maxQueue        = NECommon::IGNORE_VALUE );

        /**
         * \brief   Copies the worker thread entry.
         *
         * \param   src     Source to copy.
         **/
        WorkerThreadEntry( const NERegistry::WorkerThreadEntry & src ) = default;
        /**
         * \brief   Moves the worker thread entry.
         *
         * \param   src     Source to move.
         **/
        WorkerThreadEntry( NERegistry::WorkerThreadEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~WorkerThreadEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies the worker thread entry.
         *
         * \param   src     Source to copy.
         **/
        NERegistry::WorkerThreadEntry & operator = ( const NERegistry::WorkerThreadEntry & src ) = default;
        /**
         * \brief   Moves the worker thread entry.
         *
         * \param   src     Source to move.
         **/
        NERegistry::WorkerThreadEntry & operator = ( NERegistry::WorkerThreadEntry && src ) noexcept = default;

        /**
         * \brief   Returns true if worker thread names and consumer names are equal.
         *
         * \param   other       Worker thread entry to compare.
         **/
        bool operator == ( const NERegistry::WorkerThreadEntry & other ) const;
        /**
         * \brief   Returns true if worker thread entries are not equal.
         *
         * \param   other       Worker thread entry to compare.
         **/
        bool operator != ( const NERegistry::WorkerThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if worker thread and consumer names are non-empty.
         **/
        bool is_valid() const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadEntry class, Member variables.
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
   };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   List of worker thread entries associated with a component.
     **/
    class AREG_API WorkerThreadList
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The list of worker threads.
        using WorkerThreadListBase = ArrayList<NERegistry::WorkerThreadEntry>;

        /**
         * \brief   Creates an empty worker thread list.
         **/
        WorkerThreadList() = default;

        /**
         * \brief   Creates a list with a single worker thread entry.
         *
         * \param   entry       Worker thread entry to add as the first element.
         **/
        explicit WorkerThreadList( const NERegistry::WorkerThreadEntry & entry );

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        WorkerThreadList( const NERegistry::WorkerThreadList & src ) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        WorkerThreadList( NERegistry::WorkerThreadList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~WorkerThreadList() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        NERegistry::WorkerThreadList & operator = ( const NERegistry::WorkerThreadList & src ) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        NERegistry::WorkerThreadList & operator = ( NERegistry::WorkerThreadList && src ) noexcept = default;

        /**
         * \brief   Accesses a worker thread entry by index.
         *
         * \param   index       Valid index of the entry.
         * \return  Entry at index; invalid entry if index is out of bounds.
         **/
        inline const NERegistry::WorkerThreadEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::WorkerThreadList class, Attributes and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the list is valid.
         **/
        bool is_valid() const;

        /**
         * \brief   Finds a worker thread entry by name.
         *
         * \param   threadName      name of the worker thread entry to find.
         * \return  Valid entry if found; invalid entry otherwise.
         **/
        const NERegistry::WorkerThreadEntry & worker_thread( const String & threadName ) const;

        /**
         * \brief   Finds the index of a worker thread entry by name.
         *
         * \param   threadName      name of the worker thread entry to find.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_thread( const String & threadName ) const;

        /**
         * \brief   Finds the index of a specific worker thread entry.
         *
         * \param   entry       Worker thread entry to find.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_thread( const NERegistry::WorkerThreadEntry & entry ) const;

    public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
        /**
         * \brief   The list of worker threads.
         */
        WorkerThreadListBase    mListWorkers;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Defines dependency of client Component from Server Component. Contains Role name of
     *          dependent Server Component.
     **/
    class AREG_API DependencyEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty Dependency Entry required by Array List object.
         **/
        DependencyEntry() = default;

        /**
         * \brief   Sets the role name of dependent Component.
         *
         * \param   roleName    The Role name of dependent Component to set.
         **/
        explicit DependencyEntry( const String & roleName );

        /**
         * \brief   Copies data from given source.
         *
         * \param   src     The source of data to copy.
         **/
        DependencyEntry( const NERegistry::DependencyEntry & src ) = default;
        /**
         * \brief   Moves data from given source.
         *
         * \param   src     The source of data to move.
         **/
        DependencyEntry( NERegistry::DependencyEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~DependencyEntry()= default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies Dependency Entry data from given source.
         *
         * \param   src     The source of data to copy.
         **/
        NERegistry::DependencyEntry & operator = ( const DependencyEntry & src ) = default;
        /**
         * \brief   Moves Dependency Entry data from given source.
         *
         * \param   src     The source of data to move.
         **/
        NERegistry::DependencyEntry & operator = ( DependencyEntry && src ) noexcept = default;

        /**
         * \brief   Returns true if dependency entries are equal. Compares dependency Role names.
         *
         * \param   other       The Dependency Entry object to compare.
         **/
        bool operator == ( const NERegistry::DependencyEntry & other ) const;
        /**
         * \brief   Returns true if dependency entries are not equal. Compares dependency Role
         *          names.
         *
         * \param   other       The Dependency Entry object to compare.
         **/
        bool operator != ( const NERegistry::DependencyEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns true if entry is valid. The entry is valid if role name is not empty.
         **/
        bool is_valid() const;

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
        const String & depdendent_service() const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role name of dependent Component
         **/
        String     mRoleName;
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   List of dependency entries defining client-server relationships between components.
     **/
    class AREG_API DependencyList
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The base class of dependency list
        using DependencyListBase = ArrayList<NERegistry::DependencyEntry>;

        /**
         * \brief   Creates an empty dependency list.
         **/
        DependencyList() = default;

        /**
         * \brief   Creates a list with a single entry if the entry is valid; empty otherwise.
         *
         * \param   entry       Dependency entry; ignored if invalid.
         **/
        explicit DependencyList( const NERegistry::DependencyEntry & entry );

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        DependencyList( const NERegistry::DependencyList & src ) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        DependencyList( NERegistry::DependencyList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~DependencyList() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::DependencyList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies entries from another list.
         *
         * \param   src     Source list to copy.
         **/
        NERegistry::DependencyList & operator = ( const NERegistry::DependencyList & src ) = default;

        /**
         * \brief   Moves entries from another list.
         *
         * \param   src     Source list to move.
         **/
        NERegistry::DependencyList & operator = ( NERegistry::DependencyList && src ) noexcept = default;

        /**
         * \brief   Accesses a dependency entry by index.
         *
         * \param   index       Valid index of the entry.
         * \return  Entry at index; invalid entry if index is out of bounds.
         **/
        inline const NERegistry::DependencyEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Finds a dependency entry by component role name.
         *
         * \param   roleName    Role name of the dependent component.
         * \return  Valid entry if found; invalid entry otherwise.
         **/
        const NERegistry::DependencyEntry & dependency( const String & roleName ) const;

        /**
         * \brief   Finds the index of a dependency entry by role name.
         *
         * \param   roleName    Role name of the dependent component.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_dependency( const String & roleName ) const;

        /**
         * \brief   Finds the index of a specific dependency entry.
         *
         * \param   entry       Dependency entry to find.
         * \return  Zero-based index if found; -1 otherwise.
         **/
        int32_t find_dependency( const NERegistry::DependencyEntry & entry ) const;

        /**
         * \brief   Returns true if the list is valid.
         **/
        bool is_valid() const;

    public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
        /**
         * \brief   The list of dependency services.
         */
        DependencyListBase  mListDependencies;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Defines a component registry entry containing the role name, component
     *          creation/deletion methods, and lists of implemented services, worker threads, and
     *          service dependencies.
     **/
    class AREG_API ComponentEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an invalid ComponentEntry.
         **/
        ComponentEntry();

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
        ComponentEntry(   const String & masterThreadName
                        , const String & roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::ServiceList & serviceList
                        , const NERegistry::DependencyList & dependencyList
                        , const NERegistry::WorkerThreadList & workerList);

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
        ComponentEntry(   const String & masterThreadName
                        , const String & roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const NERegistry::ServiceEntry & service
                        , const NERegistry::DependencyEntry & dependency
                        , const NERegistry::WorkerThreadEntry & worker);

        /**
         * \brief   Copy constructor. Copies data from the source ComponentEntry.
         *
         * \param   src     The source ComponentEntry to copy.
         **/
        ComponentEntry( const NERegistry::ComponentEntry & src );

        /**
         * \brief   Move constructor. Takes ownership of data from the source ComponentEntry.
         *
         * \param   src     The source ComponentEntry to move.
         **/
        ComponentEntry( NERegistry::ComponentEntry && src ) noexcept;

        /**
         * \brief   Destructor
         **/
        ~ComponentEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copy assignment operator. Copies data from the source ComponentEntry.
         *
         * \param   src     The source ComponentEntry to copy.
         **/
        NERegistry::ComponentEntry & operator = ( const NERegistry::ComponentEntry & src );

        /**
         * \brief   Move assignment operator. Takes ownership of data from the source
         *          ComponentEntry.
         *
         * \param   src     The source ComponentEntry to move.
         **/
        NERegistry::ComponentEntry & operator = ( NERegistry::ComponentEntry && src ) noexcept;

        /**
         * \brief   Equality operator. Compares master thread name, role name, and component
         *          function pointers.
         *
         * \param   other       The ComponentEntry to compare.
         **/
        bool operator == ( const NERegistry::ComponentEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the ComponentEntry is valid (role name is not empty).
         **/
        bool is_valid() const;

        /**
         * \brief   Adds a supported service entry to the component.
         *
         * \param   entry       The service entry to add.
         **/
        void add_supported_service( const NERegistry::ServiceEntry & entry );

        /**
         * \brief   Adds a list of supported service entries to the component.
         *
         * \param   serviceList     The list of service entries to add.
         **/
        void add_supported_service( const NERegistry::ServiceList & serviceList );

        /**
         * \brief   Adds a supported service interface by name and version, ensuring uniqueness
         *          within the component.
         *
         * \param   serviceName     The name of the service interface. Must be unique within the
         *                          component.
         * \param   version         The version of the service interface.
         * \return  Returns a reference to the newly added or existing service entry.
         **/
        NERegistry::ServiceEntry & add_supported_service( const String & serviceName, const Version & version );

        /**
         * \brief   Removes a supported service from the component by name.
         *
         * \param   serviceName     The name of the service to remove.
         * \return  Returns true if the service was found and removed; false otherwise.
         **/
        bool remove_supported_service( const String & serviceName );

        /**
         * \brief   Searches for a supported service entry and returns its zero-based index if
         *          found.
         *
         * \param   entry       The service entry to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_supported_service( const NERegistry::ServiceEntry & entry ) const;

        /**
         * \brief   Searches for a supported service by name and returns its zero-based index if
         *          found.
         *
         * \param   serviceName     The name of the service to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_supported_service( const String & serviceName ) const;

        /**
         * \brief   Adds a worker thread entry to the component.
         *
         * \param   entry       The worker thread entry to add.
         **/
        void add_worker_thread( const NERegistry::WorkerThreadEntry & entry );

        /**
         * \brief   Adds a list of worker thread entries to the component.
         *
         * \param   workerList      The list of worker thread entries to add.
         **/
        void add_worker_thread( const NERegistry::WorkerThreadList & workerList );

        /**
         * \brief   Searches for a worker thread entry and returns its zero-based index if found.
         *
         * \param   entry       The worker thread entry to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_worker_thread( const NERegistry::WorkerThreadEntry & entry ) const;

        /**
         * \brief   Searches for a worker thread by name and returns its zero-based index if found.
         *
         * \param   workerName      The name of the worker thread to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_worker_thread( const String & workerName ) const;

        /**
         * \brief   Removes a worker thread from the component by name.
         *
         * \param   workerName      The name of the worker thread to remove.
         * \return  Returns true if the worker thread was found and removed; false otherwise.
         **/
        bool remove_worker_thread( const String & workerName );

        /**
         * \brief   Adds a service dependency entry to the component.
         *
         * \param   entry       The dependency entry to add.
         **/
        void add_dependency_service( const NERegistry::DependencyEntry & entry );

        /**
         * \brief   Adds a list of service dependency entries to the component.
         *
         * \param   dependencyList      The list of dependency entries to add.
         **/
        void add_dependency_service( const NERegistry::DependencyList & dependencyList );

        /**
         * \brief   Adds a service dependency by role name of the server component.
         *
         * \param   roleName    The role name of the dependent service.
         * \return  Returns a reference to the newly added or existing dependency entry.
         **/
        NERegistry::DependencyEntry & add_dependency_service( const String & roleName );

        /**
         * \brief   Searches for a dependency entry and returns its zero-based index if found.
         *
         * \param   entry       The dependency entry to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_dependency_service( const NERegistry::DependencyEntry & entry ) const;

        /**
         * \brief   Removes a service dependency from the component by role name.
         *
         * \param   roleName    The role name of the dependency to remove.
         * \return  Returns true if the dependency was found and removed; false otherwise.
         **/
        bool remove_dependency_service( const String & roleName );

        /**
         * \brief   Searches for a dependency by role name and returns its zero-based index if
         *          found.
         *
         * \param   roleName    The role name of the dependency to search for.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_dependency_service(const String & roleName) const;

        /**
         * \brief   Returns the list of supported services in the component.
         **/
        const NERegistry::ServiceList & supported_services() const;

        /**
         * \brief   Returns the list of worker threads in the component.
         **/
        const NERegistry::WorkerThreadList & worker_threads() const;

        /**
         * \brief   Returns the list of service dependencies in the component.
         **/
        const NERegistry::DependencyList & dependency_services() const;

        /**
         * \brief   Sets the component creation and deletion function pointers.
         *
         * \param   fnCreate    Pointer to the component creation function; nullptr to clear.
         * \param   fnDelete    Pointer to the component deletion function; nullptr to clear.
         **/
        void set_instance_methods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete );

        /**
         * \brief   Sets component data to be passed to the creation function.
         *
         * \param   compData    The data to associate with the component.
         * \note    Manually free memory if the data was manually allocated.
         **/
        void set_data( std::any compData );

        /**
         * \brief   Resets all component entry fields to their default values.
         **/
        void reset();

        /**
         * \brief   Returns the component data; verify it holds a value before use.
         **/
        std::any data() const;

        /**
         * \brief   Returns true if component data is set; false otherwise.
         **/
        bool has_data() const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntry class, Member variables
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
#if defined(_MSC_VER) && (_MSC_VER > 1200)
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
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentEntryList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   List of component entries within a thread; all components must have unique role
     *          names and service interfaces.
     **/
    class AREG_API ComponentList
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The base class of component list.
        using ComponentListBase = ArrayList<NERegistry::ComponentEntry>;

        /**
         * \brief   Creates an empty component list.
         **/
        ComponentList() = default;

        /**
         * \brief   Creates a component list with the given component entry as the first element.
         *
         * \param   entry       The component entry to add as the first element.
         **/
        explicit ComponentList(  const NERegistry::ComponentEntry & entry );

        /**
         * \brief   Copies values from the given source.
         *
         * \param   src     The source of data to copy.
         **/
        ComponentList( const NERegistry::ComponentList & src ) = default;

        /**
         * \brief   Moves values from the given source.
         *
         * \param   src     The source of data to move.
         **/
        ComponentList( NERegistry::ComponentList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentList() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies component entries from the given source.
         *
         * \param   src     The source component list.
         **/
        NERegistry::ComponentList & operator = ( const NERegistry::ComponentList & src ) = default;

        /**
         * \brief   Moves component entries from the given source.
         *
         * \param   src     The source component list.
         **/
        NERegistry::ComponentList & operator = ( NERegistry::ComponentList && src ) noexcept = default;

        /**
         * \brief   Returns the component entry at the given index for read-only access.
         *
         * \param   index       The zero-based index of the entry.
         * \return  Returns the component entry; invalid entry if index is out of bounds.
         **/
        inline const NERegistry::ComponentEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the component list is valid.
         **/
        bool is_valid() const;

        /**
         * \brief   Returns the component entry with the given role name.
         *
         * \param   roleName    The role name to search for.
         * \return  Returns the component entry if found; otherwise returns an invalid component
         *          entry.
         **/
        const NERegistry::ComponentEntry & component( const String & roleName ) const;

        /**
         * \brief   Sets component data for the component with the given role name.
         *
         * \param   roleName    The role name of the component.
         * \param   compData    The data to set (passed to component creation method).
         * \return  Returns true if component was found and data was set; false otherwise.
         * \note    Caller must manually free allocated memory.
         **/
        bool set_component_data( const String & roleName, std::any compData );

        /**
         * \brief   Resets component data for the component with the given role name.
         *
         * \param   roleName    The role name of the component.
         * \return  Returns true if component was found and data was reset; false otherwise.
         **/
        bool reset( const String& roleName );

        /**
         * \brief   Returns the zero-based index of the component with the given role name.
         *
         * \param   roleName    The role name to search for.
         * \return  Returns the index if found; -1 if not found.
         **/
        int32_t find_component( const String & roleName ) const;

        /**
         * \brief   Returns the zero-based index of the given component entry.
         *
         * \param   entry       The component entry to search for.
         * \return  Returns the index if found; -1 if not found.
         **/
        int32_t find_component(const NERegistry::ComponentEntry& entry) const;

    public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

        /**
         * \brief   The list of components.
         */
        ComponentListBase   mListComponents;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class declaration
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
    // NERegistry::ComponentThreadEntry class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief
         **/
        ComponentThreadEntry();

        /**
         * \brief   Initializes Thread Entry with given Thread name and optional thread parameters.
         *
         * \param   threadName          The Thread name to assign.
         * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component
         *                              thread. The value 0 (NECommon::WATCHDOG_IGNORE) ignores
         *                              watchdog.
         * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024
         *                              Bytes). Pass `NECommon::STACK_SIZE_DEFAULT` (0) to ignore
         *                              changing stack size and use system default stack size.
         * \param   maxQueue            The maximum size of message queue for the thread. Pass
         *                              NECommon::IGNORE_VALUE to use default value set in
         *                              configuration or ignore the parameter if not configured.
         **/
        explicit ComponentThreadEntry( const String & threadName
                                     , const uint32_t watchdogTimeout   = NECommon::WATCHDOG_IGNORE
                                     , const uint32_t stackSizeKb       = NECommon::STACK_SIZE_DEFAULT
                                     , const uint32_t maxQueue          = NECommon::IGNORE_VALUE);

        /**
         * \brief   Initializes Thread Entry with given Thread name and given Component List.
         *
         * \param   threadName          The Thread name to assign.
         * \param   componentList       The List of Component Entries to assign.
         * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component
         *                              thread. The value 0 (NECommon::WATCHDOG_IGNORE) ignores
         *                              watchdog.
         * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024
         *                              Bytes). Pass `NECommon::STACK_SIZE_DEFAULT` (0) to ignore
         *                              changing stack size and use system default stack size.
         * \param   maxQueue            The maximum size of message queue for the thread.
         **/
        ComponentThreadEntry( const String & threadName
                            , const NERegistry::ComponentList & componentList
                            , const uint32_t watchdogTimeout= NECommon::WATCHDOG_IGNORE
                            , const uint32_t stackSizeKb    = NECommon::STACK_SIZE_DEFAULT
                            , const uint32_t maxQueue       = NECommon::IGNORE_VALUE );

        /**
         * \brief
         *
         * \param   src     The source of data to copy.
         **/
        ComponentThreadEntry( const NERegistry::ComponentThreadEntry & src ) = default;

        /**
         * \brief
         *
         * \param   src     The source of data to move.
         **/
        ComponentThreadEntry( NERegistry::ComponentThreadEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentThreadEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief
         *
         * \param   src     The source of data to copy
         **/
        NERegistry::ComponentThreadEntry & operator = ( const NERegistry::ComponentThreadEntry & src ) = default;

        /**
         * \brief
         *
         * \param   src     The source of data to move.
         **/
        NERegistry::ComponentThreadEntry & operator = ( NERegistry::ComponentThreadEntry && src ) noexcept = default;

        /**
         * \brief   Returns true if Thread Entry names are equal.
         *
         * \param   other       The Thread Entry to compare.
         **/
        bool operator == ( const NERegistry::ComponentThreadEntry & other ) const;
        /**
         * \brief   Returns true if Thread Entry names are not equal.
         *
         * \param   other       The Thread Entry to compare.
         **/
        bool operator != ( const NERegistry::ComponentThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Attribute and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread Entry is valid. The Entry is valid if name of Thread
         *          Entry is not empty.
         **/
        bool is_valid() const;

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
        void add_component( const NERegistry::ComponentEntry & entry );

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
        void add_component( const NERegistry::ComponentList & componentList );

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
        NERegistry::ComponentEntry & add_component( const String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

        template<typename ComponentType>
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
        inline NERegistry::ComponentEntry& add_component(const String& roleName);

        /**
         * \brief   Searches the component entry by given role name. If found, remove the component
         *          entry from the list.
         *
         * \param   roleName    The roleName of service component to search and remove.
         * \return  Returns true if found and could remove the component entry.
         **/
        bool remove_component( const String & roleName );

        /**
         * \brief   Searches Component Entry in the existing list of Thread Entry and returns the
         *          zero-based valid position if found. Otherwise, returns -1, if cannot find
         *          Component Entry in the list.
         *
         * \param   entry       The Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. Otherwise, returns
         *          -1.
         **/
        int32_t find_component_entry( const NERegistry::ComponentEntry & entry ) const;

        /**
         * \brief   By given name, searches Component Entry in the existing list of Thread Entry and
         *          returns the zero-based valid position if found. Returns -1, if cannot find
         *          Component Entry in the list.
         *
         * \param   roleName    The name of Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element. Otherwise, returns
         *          -1.
         **/
        int32_t find_component_entry( const String & roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         *
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    You should manually free memory if the data was manually allocated in the memory
         **/
        bool set_component_data( const String & roleName, std::any compData );

        /**
         * \brief   Searches in the list the component by given name. If found, resets component
         *          data. Returns true if found component and the data was successfully reset.
         *
         * \param   roleName    The name of component to search in the list.
         **/
        bool reset( const String & roleName );

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadEntry class, Member variables
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
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   List of thread entries in a model. Each thread name must be unique.
     **/
    class AREG_API ComponentThreadList
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Constructors / destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The base class of component thread list.
        using ComponentThreadListBase = ArrayList< NERegistry::ComponentThreadEntry>;

        /**
         * \brief
         **/
        ComponentThreadList() = default;

        /**
         * \brief   Creates Component Thread List and adds specified Thread Entry to the list as
         *          first element.
         *
         * \param   entry       The Thread Entry to set as first element in the list.
         **/
        explicit ComponentThreadList( const NERegistry::ComponentThreadEntry & entry );

        /**
         * \brief   Copies entries from given source.
         *
         * \param   src     The source of data to copy.
         **/
        ComponentThreadList( const NERegistry::ComponentThreadList & src ) = default;

        /**
         * \brief   Moves entries from given source.
         *
         * \param   src     The source of data to move.
         **/
        ComponentThreadList( NERegistry::ComponentThreadList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentThreadList() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entries from given source of Thread List.
         *
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        NERegistry::ComponentThreadList & operator = ( const NERegistry::ComponentThreadList & src ) = default;

        /**
         * \brief   Moves Thread Entries from given source of Thread List.
         *
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        NERegistry::ComponentThreadList & operator = ( NERegistry::ComponentThreadList && src ) noexcept = default;

        /**
         * \brief   Accesses element at given index by position.
         *
         * \param   index       The zero-based index. Must be valid.
         * \return  The thread entry at the specified index. Returns invalid thread entry if index
         *          is out of bounds.
         **/
        inline const NERegistry::ComponentThreadEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::ComponentThreadList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread List is valid.
         **/
        bool is_valid() const;

        /**
         * \brief   Searches Thread Entry having given name. Returns valid Thread Entry if found,
         *          otherwise returns invalid thread entry. There should not be several Thread
         *          Entries with same name.
         *
         * \param   threadName      The name of Thread Entry to search.
         * \return  Returns valid Thread Entry if found. Otherwise returns invalid thread entry.
         **/
        const NERegistry::ComponentThreadEntry & thread( const String & threadName ) const;

        /**
         * \brief   Searches Thread Entry by given Thread name and returns zero-based valid index if
         *          found. The valid index is not negative.
         *
         * \param   threadName      The Thread name of Thread Entry to search.
         * \return  Returns zero-based valid index if found entry. Otherwise, returns -1.
         **/
        int32_t find_thread( const String & threadName ) const;

        /**
         * \brief   Searches specified Thread Entry and returns zero-based valid index if found. The
         *          valid index is not negative.
         *
         * \param   entry       The Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry. Otherwise, returns -1.
         **/
        int32_t find_thread( const NERegistry::ComponentThreadEntry & entry ) const;

    public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

        /**
         * \brief   The list of component threads.
         */
        ComponentThreadListBase mListThreads;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER
    };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Defines the model of a project/binary, specifying threads, components, and their
     *          dependencies. Each application can have only one model; multiple models must be
     *          defined in separate projects/binaries.
     **/
    class AREG_API Model
    {
    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Friend classes
    //////////////////////////////////////////////////////////////////////////
        friend class ComponentLoader;

        enum class ModelState : uint8_t
        {
              Initialized
            , Loaded
            , Unloaded
        };

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates an empty model.
         **/
        Model();

        /**
         * \brief   Creates an empty model and sets the name.
         **/
        explicit Model( const String & modelName );

        /**
         * \brief   Creates a model, sets the name, and registers the thread list.
         **/
        Model( const String & modelName, const ComponentThreadList & threadList );

        /**
         * \brief
         **/
        Model( const Model & src ) = default;
        /**
         * \brief
         **/
        Model( Model && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~Model() = default;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief
         **/
        NERegistry::Model & operator = ( const NERegistry::Model & src ) = default;
        /**
         * \brief
         **/
        NERegistry::Model & operator = ( NERegistry::Model && src ) noexcept = default;

        /**
         * \brief   Returns true if the models are equal by comparing names and registered thread
         *          entries.
         **/
        bool operator == ( const NERegistry::Model & other ) const;
        /**
         * \brief   Returns true if the models are not equal.
         **/
        bool operator != ( const NERegistry::Model & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if the model is valid (non-empty name and at least one thread
         *          entry).
         **/
        bool is_valid() const;

        /**
         * \brief   Returns the name of the model.
         **/
        const String & model_name() const;

        /**
         * \brief   Returns true if the specified component entry is registered in the model.
         *
         * \param   entry       The component entry to search in the model.
         * \return  Returns true if the component entry is found; false otherwise.
         **/
        bool has_registered_component( const NERegistry::ComponentEntry & entry ) const;

        /**
         * \brief   Returns true if a component with the specified role name is registered in the
         *          model.
         *
         * \param   roleName    The role name of the component entry to search in the model.
         * \return  Returns true if a component with the specified role name is found; false
         *          otherwise.
         **/
        bool has_registered_component( const String & roleName ) const;

        /**
         * \brief   Returns true if the model is loaded; false otherwise.
         **/
        bool is_model_loaded() const;

        /**
         * \brief   Returns the list of thread entries registered in the model.
         **/
        const NERegistry::ComponentThreadList & thread_list() const;

        /**
         * \brief   Marks the model as loaded or unloaded.
         *
         * \param   isLoaded    If true, marks the model as loaded; if false, marks it as unloaded.
         **/
        void mark_model_loaded( bool isLoaded = true );

        /**
         * \brief   Marks the model as alive and starts the timer, or stops the timer when no longer
         *          alive to calculate the duration.
         *
         * \param   is_alive     If true, marks the model as alive and starts the timer; if false,
         *                      stops the timer.
         **/
        void mark_model_alive( bool is_alive);

        /**
         * \brief   Adds a thread entry to the model. The entry must have a globally unique name.
         *
         * \param   entry       The thread entry to add.
         **/
        void add_thread( const NERegistry::ComponentThreadEntry & entry );

        /**
         * \brief   Adds multiple thread entries to the model. Each entry must have a globally
         *          unique name.
         *
         * \param   threadList      The list of thread entries to add.
         **/
        void add_thread( const NERegistry::ComponentThreadList & threadList );

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
        NERegistry::ComponentThreadEntry & add_thread(const String & threadName );

        /**
         * \brief   Removes the thread entry with the specified name from the model.
         *
         * \param   threadName      The name of the thread entry to search and remove.
         * \return  Returns true if the thread was found and removed; false otherwise.
         **/
        bool remove_thread( const String & threadName );

        /**
         * \brief   Searches for a thread entry in the model and returns its zero-based index.
         *
         * \param   entry       The thread entry to search for in the model.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_thread( const NERegistry::ComponentThreadEntry & entry ) const;

        /**
         * \brief   Searches for a thread entry by name in the model and returns its zero-based
         *          index.
         *
         * \param   threadName      The name of the thread entry to search for in the model.
         * \return  Returns the zero-based index if found; -1 otherwise.
         **/
        int32_t find_thread(const String & threadName) const;

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
        bool reset(const String& roleName);

        /**
         * \brief   Returns the duration in nanoseconds the model was alive (loaded). Returns zero
         *          if the model was never loaded; returns the current duration if still alive;
         *          returns the last duration if no longer alive.
         **/
        inline TIME64 alive_duration() const;

    //////////////////////////////////////////////////////////////////////////
    // NERegistry::Model class, Member variables
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
        NEUtilities::Duration   mAliveDuration;
    };

//////////////////////////////////////////////////////////////////////////
// NERegistry namespace Predefined Invalid Entries.
//////////////////////////////////////////////////////////////////////////

     /**
      * \brief   Returns predefined invalid Service Entry.
      **/
     AREG_API const NERegistry::ServiceEntry & invalid_service_entry();;

    /**
     * \brief   Returns predefined invalid Service List.
     **/
    AREG_API const NERegistry::ServiceList & invalid_service_list();

    /**
     * \brief   Returns predefined invalid Worker Thread Entry.
     **/
    AREG_API const NERegistry::WorkerThreadEntry & invalid_worker_entry();

    /**
     * \brief   Returns predefined invalid Worker Thread List.
     **/
    AREG_API const NERegistry::WorkerThreadList & invalid_worker_list();

    /**
     * \brief   Returns predefined invalid service dependency entry.
     **/
    AREG_API const NERegistry::DependencyEntry & invalid_depedency_entry();

    /**
     * \brief   Returns predefined invalid service dependency list.
     **/
    AREG_API const NERegistry::DependencyList & invalid_depedency_list();

    /**
     * \brief   Returns predefined invalid Component Entry.
     **/
    AREG_API const NERegistry::ComponentEntry & invalid_component_entry();

    /**
     * \brief   Returns predefined invalid Component List.
     **/
    AREG_API const NERegistry::ComponentList & invalid_component_list();

    /**
     * \brief   Returns predefined invalid Thread Entry.
     **/
    AREG_API const NERegistry::ComponentThreadEntry & invalid_thread_entry();

    /**
     * \brief   Returns predefined invalid Thread List containing single invalid thread entry.
     **/
    AREG_API const NERegistry::ComponentThreadList & invalid_thread_list();

    /**
     * \brief   Returns predefined invalid Model.
     **/
    AREG_API const NERegistry::Model & invalid_model();
}

//////////////////////////////////////////////////////////////////////////
// NERegistry inline methods
//////////////////////////////////////////////////////////////////////////

inline const NERegistry::ServiceEntry& NERegistry::ServiceList::operator [] (uint32_t index) const
{
    return (mListServices.is_valid_index(index) ? mListServices[index] : NERegistry::invalid_service_entry());
}

inline const NERegistry::WorkerThreadEntry& NERegistry::WorkerThreadList::operator [] (uint32_t index) const
{
    return (mListWorkers.is_valid_index(index) ? mListWorkers[index] : NERegistry::invalid_worker_entry());
}

inline const NERegistry::DependencyEntry& NERegistry::DependencyList::operator [] (uint32_t index) const
{
    return (mListDependencies.is_valid_index(index) ? mListDependencies[index] : NERegistry::invalid_depedency_entry());
}

inline const NERegistry::ComponentEntry& NERegistry::ComponentList::operator [] (uint32_t index) const
{
    return (mListComponents.is_valid_index(index) ? mListComponents[index] : NERegistry::invalid_component_entry());
}

inline const NERegistry::ComponentThreadEntry& NERegistry::ComponentThreadList::operator [] (uint32_t index) const
{
    return (mListThreads.is_valid_index(index) ? mListThreads[index] : NERegistry::invalid_thread_entry());
}

inline TIME64 NERegistry::Model::alive_duration() const
{
    return (mLoadState == ModelState::Initialized ? 0 : mAliveDuration.duration_since_start());
}

template<typename ComponentType>
inline NERegistry::ComponentEntry& NERegistry::ComponentThreadEntry::add_component(const String& roleName)
{
    return add_component(roleName
        , [](const NERegistry::ComponentEntry& entry, ComponentThread& owner) -> Component* {
            return new ComponentType(entry, owner);
        }
        , [](Component& comp, const NERegistry::ComponentEntry& /*entry*/) -> void {
            ComponentType* tmp = static_cast<ComponentType *>(&comp);
            delete tmp;
        });
}

#endif  // AREG_COMPONENT_MODEL_HPP