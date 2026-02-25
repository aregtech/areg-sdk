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
namespace areg
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
namespace areg { class WorkerThreadConsumer; }

/************************************************************************
 * Global types
 ************************************************************************/

/**
 * \brief   Type of Component Load Function. Called to instantiate and start component
 * \type areg::ComponentEntry     Component registry entry passed to component constructor.
 *                                      The component entry contains list of dependencies, services and component data.
 * \type ComponentThread                The component owning thread object.
 * \example This function should create and return pointer to component object.
 *  [](const areg::ComponentEntry& entry, ComponentThread& ownerThread) -> Component *{
 *      return new MyComponent(entry, ownerThread);
 *  }
 **/
using  FuncCreateComponent  = std::function<Component* (const areg::ComponentEntry& /*entry*/, ComponentThread& /*ownerThread*/)>;

/**
 * \brief   Type of Component Unload Function. Called to stop and delete component
 * \type Component                    The component to stop and delete.
 * \type areg::ComponentEntry   Component registry entry passed to delete function.
 * \example This function should stop and delete component object.
 *  [] (Component& comp, const areg::ComponentEntry& entry) {
 *      delete &comp;
 *  }
 **/
using FuncDeleteComponent   = std::function<void (Component& /*comp*/, const areg::ComponentEntry& /*entry*/)>;

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
namespace areg
{

//////////////////////////////////////////////////////////////////////////
// areg::ServiceEntry class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::ServiceEnty, defines Server side of implemented
     *          Service Interfaces inComponent. It contains name of
     *          Service Interface and implemented Version. One component
     *          can contain more than one Service Entry, indicating all
     *          implemented Service Interfaces. The list of implemented
     *          Services are listed in the Service List.
     **/
    class AREG_API ServiceEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Default constructor. Should not be directly used.
         *          Creates invalid Service Entry and required by Array List object.
         *          Invalid Service Entry has an empty service name.
         **/
        ServiceEntry() = default;

        /**
         * \brief   Initialize service entry by given name and version numbers.
         *          The passed serviceName and the version number should be valid.
         *          The valid service name should not be empty.
         *          The version is valid, when either major or minor version numbers are not zero.
         * \param   serviceName     The name of implemented service interface.
         * \param   major           The major version number of implemented interface.
         * \param   minor           The minor version number of implemented interface
         * \param   patch           The patch version number of implemented interface
         **/
        ServiceEntry( const areg::String & serviceName, uint32_t major, uint32_t minor, uint32_t patch );

        /**
         * \brief   Initialize service interface entry by given name and version object.
         *          The passed serviceName and the Version should be valid.
         *          The valid Service name should not be empty.
         *          The Version is valid if major or minor version is not zero.
         * \param   serviceName The name of service interface
         * \param   version     The version object of implemented service interface,
         *                      containing major, minor and patch version numbers
         **/
        ServiceEntry( const areg::String & serviceName, const areg::Version & version );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        ServiceEntry( const ServiceEntry & src ) = default;

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ServiceEntry( ServiceEntry && src ) noexcept = default;

        /**
         * \brief   Destructor.
         **/
        ~ServiceEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entry data from given source.
         * \param   src     The source of data to copy
         **/
        areg::ServiceEntry & operator = ( const areg::ServiceEntry & src ) = default;

        /**
         * \brief   Moves Service Entry data from given source.
         * \param   src     The source of data to move.
         **/
        areg::ServiceEntry & operator = ( areg::ServiceEntry && src ) noexcept = default;

        /**
         * \brief   Checks the equality of two Service Entries and returns true if they are equal.
         *          It compares Service names and Versions.
         * \param   other   The Service Entry object to compare.
         **/
        bool operator == ( const areg::ServiceEntry & other ) const;
        bool operator != ( const areg::ServiceEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry is valid.
         *          The Service Entry is valid if name is not empty and version is valid.
         **/
        bool isValid() const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of implemented Service Interface
         **/
        areg::String     mName;

        /**
         * \brief   The Version of implemented Service.
         **/
        areg::Version   mVersion;
    };

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceList class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::ServiceList, defines list of Service Entries.
     *          It is a list of all implemented Service Interfaces in the Component.
     **/
    class AREG_API ServiceList
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The list of service entries.
        using ServiceListBase = areg::ArrayList<areg::ServiceEntry>;

        /**
         * \brief   Default constructor.
         **/
        ServiceList() = default;

        /**
         * \brief   Creates Service List and adds specified Service Entry to the list as first element.
         * \param   entry   The Service Entry to set as first element in the list.
         **/
        explicit ServiceList( const areg::ServiceEntry & entry );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        ServiceList( const areg::ServiceList & src) = default;

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ServiceList( areg::ServiceList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ServiceList() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceList class, operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Service Entries from given source of Service List.
         * \param   src     The source of Service List, containing Service Entries.
         **/
        areg::ServiceList & operator = ( const areg::ServiceList & src ) = default;

        /**
         * \brief   Copies Service Entries from given source of Service List.
         * \param   src     The source of Service List, containing Service Entries.
         **/
        areg::ServiceList & operator = ( areg::ServiceList && src ) noexcept = default;

        /**
         * \brief   Operator to access elements of the service list by index.
         * \param   index   Must be valid index of the entry.
         * \return  The instance of service element entry for real-only operations.
         *          Returns instance of invalid service entry if index is invalid.
         */
        inline const areg::ServiceEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ServiceList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if service entry list is valid
         **/
        bool isValid() const;

        /**
         * \brief   Searches Service Entry having given name. Returns valid
         *          Service Entry if found, otherwise returns invalid entry.
         *          There should not be several Service Entries with same
         *          Service name. If a Service Interface should be implemented
         *          more than one time, they should be registered in
         *          different Components having different Role Names and
         *          running in different threads.
         * \param   serviceName The name of Service Entry to search.
         * \return  Returns valid Service Entry if found. Otherwise returns
         *          invalid service entry.
         **/
        const areg::ServiceEntry & getService( const areg::String & serviceName ) const;

        /**
         * \brief   Searches Service Entry by given Service Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   serviceName The Service Name of Service Entry to search.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findService( const areg::String & serviceName ) const;

        /**
         * \brief   Searches specified Service Entry and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Service Entry to search in the list.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findService(const areg::ServiceEntry & entry) const;

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
    // areg::WorkerThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::WorkerThreadEntry, defines Worker Thread.
     *          It should be part of binding Component. The Worker Threads
     *          are created when Component is crated.
     **/
    class AREG_API WorkerThreadEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Creates invalid Worker Thread Entry.
         **/
        WorkerThreadEntry();

        /**
         * \brief   Initialize Worker Thread Entry by given name and specifying the name of Master Thread.
         *          The Master Thread is the thread where Component is registered and created.
         *          Since one component may have different role names and
         *          Component may have more than one worker thread, it should be
         *          possible to identify consumer object by consumer name and thread name
         *          to configure worker thread properly.
         * \param   masterThreadName    The name of Master Thread where Component is created and running.
         * \param   workerThreadName    The name of Worker Thread of Component. The name should be unique.
         * \param   compRoleName        The name of Component (Role Name) where consumer is registered.
         * \param   compConsumerName    The name of Consumer object to configure, it should not be same as Component name.
         * \param   watchdogTimeout     The timeout in milliseconds to set for watchdog. The value 0 ignores watchdog.
         * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024 Bytes).
         *                              Pass `areg::STACK_SIZE_DEFAULT` (0) to ignore changing stack size and use system default stack size.
         * \param   maxQueue            The maximum size of message queue for the thread.
         *                              Pass areg::IGNORE_VALUE to use default value set in configuration or ignore the parameter if not configured.
         **/
        WorkerThreadEntry( const areg::String & masterThreadName
                         , const areg::String & workerThreadName
                         , const areg::String & compRoleName
                         , const areg::String & compConsumerName
                         , const uint32_t watchdogTimeout = areg::WATCHDOG_IGNORE
                         , const uint32_t stackSizeKb     = areg::STACK_SIZE_DEFAULT
                         , const uint32_t maxQueue        = areg::IGNORE_VALUE );

        /**
         * \brief   Copies /move entries from source.
         **/
        WorkerThreadEntry( const areg::WorkerThreadEntry & src ) = default;
        WorkerThreadEntry( areg::WorkerThreadEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~WorkerThreadEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies / moves Worker Thread Entry data from given source.
         **/
        areg::WorkerThreadEntry & operator = ( const areg::WorkerThreadEntry & src ) = default;
        areg::WorkerThreadEntry & operator = ( areg::WorkerThreadEntry && src ) noexcept = default;

        /**
         * \brief   Checks equality of two Worker Thread Entries and returns true if they are equal.
         *          It compares Worker Thread name and Consumer Name.
         * \param   other   The Worker Thread Entry to compare.
         **/
        bool operator == ( const areg::WorkerThreadEntry & other ) const;
        bool operator != ( const areg::WorkerThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadEntry class, Attributes
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Worker Thread Entry is valid.
         *          The Entry is valid if neither Worker Thread nor Consumer names are empty.
         **/
        bool isValid() const;

    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadEntry class, Member variables.
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The name of Worker Thread
         **/
        areg::String      mThreadName;
        /**
         * \brief   The name of Worker Thread Consumer.
         **/
        areg::String      mConsumerName;
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
    // areg::WorkerThreadList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   areg::WorkerThreadList. Defines list of Worker Thread Entries.
     *          It is a list of all Worker Threads bound with one Component.
     **/
    class AREG_API WorkerThreadList
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The list of worker threads.
        using WorkerThreadListBase = areg::ArrayList<areg::WorkerThreadEntry>;

        /**
         * \brief   Default constructor.
         **/
        WorkerThreadList() = default;

        /**
         * \brief   Creates Worker Thread List and adds specified Worker Thread Entry to the list as first element.
         * \param   entry   The Worker Thread Entry to set as first element in the list.
         **/
        explicit WorkerThreadList( const areg::WorkerThreadEntry & entry );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        WorkerThreadList( const areg::WorkerThreadList & src ) = default;

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        WorkerThreadList( areg::WorkerThreadList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~WorkerThreadList() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Worker Thread Entries from given source of Worker Thread List.
         * \param   src     The source of Worker Thread List, containing Worker Thread Entries.
         **/
        areg::WorkerThreadList & operator = ( const areg::WorkerThreadList & src ) = default;

        /**
         * \brief   Copies Worker Thread Entries from given source of Worker Thread List.
         * \param   src     The source of Worker Thread List, containing Worker Thread Entries.
         **/
        areg::WorkerThreadList & operator = ( areg::WorkerThreadList && src ) noexcept = default;

        /**
         * \brief   Operator to access elements of the worker thread list by index.
         * \param   index   Must be valid index of the entry.
         * \return  The instance of service element entry for real-only operations.
         *          Returns instance of invalid worker thread etnry if index is invalid.
         **/
        inline const areg::WorkerThreadEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::WorkerThreadList class, Attributes and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if list is valid
         **/
        bool isValid() const;

        /**
         * \brief   Searches Worker Thread Entry having given name. Returns valid
         *          Worker Thread Entry if found, otherwise returns invalid worker thread entry.
         *          There should not be several Worker Thread Entries with same name.
         * \param   threadName  The name of Worker Thread Entry to search.
         * \return  Returns valid Worker Thread Entry if found. Otherwise returns
         *          invalid worker thread entry.
         **/
        const areg::WorkerThreadEntry & getWorkerThread( const areg::String & threadName ) const;

        /**
         * \brief   Searches Worker Thread Entry by given thread name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Worker Thread name to search entry
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findThread( const areg::String & threadName ) const;

        /**
         * \brief   Searches specified Worker Thread Entry and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findThread( const areg::WorkerThreadEntry & entry ) const;

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
    // areg::DependencyEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::DependencyEntry. Defines dependency of client Component
     *          from Server Component. It is a part of Client Component, but
     *          contains Role Name of dependent Server Component.
     *
     *          For example, if a Component ABC contains Dependency Entry
     *          with Role Name XYZ, means that the Component ABC is a
     *          Client of Component XYZThe Service Interface server
     *          implementation is on Component XYZ side, and the Client of
     *          the same Service Interface is on Component ABC side
     **/
    class AREG_API DependencyEntry
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyEntry class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Creates empty Dependency Entry and required by Array List object
         **/
        DependencyEntry() = default;

        /**
         * \brief   Sets the role name of dependent Component
         * \param   roleName    The Role Name of dependent Component to set.
         **/
        explicit DependencyEntry( const areg::String & roleName );

        /**
         * \brief   Copies / moves data from given source.
         **/
        DependencyEntry( const areg::DependencyEntry & src ) = default;
        DependencyEntry( areg::DependencyEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~DependencyEntry()= default;

    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Copies / moves Dependency Entry  data from given source.
         **/
        areg::DependencyEntry & operator = ( const DependencyEntry & src ) = default;
        areg::DependencyEntry & operator = ( DependencyEntry && src ) noexcept = default;

        /**
         * \brief   Checks equality of two Dependency Entries and returns true if they are equal.
         *          It compares dependency Role names.
         * \param   other   The Dependency Entry object to compare.
         **/
        bool operator == ( const areg::DependencyEntry & other ) const;
        bool operator != ( const areg::DependencyEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyEntry class, Operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns true if entry is valid. The entry is valid if role name is not empty
         **/
        bool isValid() const;

        /**
         * \brief   Sets the role name of dependent service.
         * \param   roleName    The role name of dependent service. If empty, invalidates dependency.
         **/
        void setDependentService( const areg::String & roleName );

        /**
         * \brief   Returns the role name of dependent service.
         **/
        const areg::String & getDepdendentService() const;

    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role Name of dependent Component
         **/
        areg::String     mRoleName;
    };

    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   areg::DependencyList, Defines list of Dependency Entries.
     *          It is a list of all dependent Components. Can be empty,
     *          if Component does not have dependency.
     *          The dependency is needed to specify client-server relationship
     *          between Components. Every Dependency Entry is specifying
     *          client of specified Component.
     **/
    class AREG_API DependencyList
    {
    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyList class, Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The base class of dependency list
        using DependencyListBase = areg::ArrayList<areg::DependencyEntry>;

        /**
         * \brief   Creates empty list of dependent services.
         **/
        DependencyList() = default;

        /**
         * \brief   If passed entry is valid, creates a dependency list with one entry.
         * \param   entry   The entry of dependent service. If entry is empty (invalid),
         *                  the parameter is ignored and empty list is created.
         **/
        explicit DependencyList( const areg::DependencyEntry & entry );

        /**
         * \brief   Copies entries from given source.
         * \param   src     The source of data to copy.
         **/
        DependencyList( const areg::DependencyList & src ) = default;

        /**
         * \brief   Moves entries from given source.
         * \param   src     The source of data to move.
         **/
        DependencyList( areg::DependencyList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~DependencyList() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::DependencyList class, Operator
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Dependency Entries from given source of Dependency List.
         * \param   src     The source of Dependency List, containing Dependency Entries.
         **/
        areg::DependencyList & operator = ( const areg::DependencyList & src ) = default;

        /**
         * \brief   Moves Dependency Entries from given source of Dependency List.
         * \param   src     The source of Dependency List, containing Dependency Entries.
         **/
        areg::DependencyList & operator = ( areg::DependencyList && src ) noexcept = default;

        /**
         * \brief   Operator to access elements of the dependency service list by index.
         * \param   index   Must be valid index of the entry.
         * \return  The instance of service element entry for real-only operations.
         *          Returns instance of invalid dependency entry if index is invalid.
         **/
        inline const areg::DependencyEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Searches Dependency Entry having given role name. Returns valid
         *          Worker Thread Entry if found, otherwise returns empty Dependency Entry.
         *          There should not be several Dependency Entries with same name.
         * \param   roleName    The name of dependent Component Entry to search.
         * \return  Returns valid Dependency Entry if found. Otherwise returns
         *          empty entry.
         **/
        const areg::DependencyEntry & getDependency( const areg::String & roleName ) const;

        /**
         * \brief   Searches Dependency Entry by given role name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of dependent Component
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findDependency( const areg::String & roleName ) const;

        /**
         * \brief   Searches specified Dependency Entry and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findDependency( const areg::DependencyEntry & entry ) const;

        /**
         * \brief   Returns true if dependency list is valid.
         **/
        bool isValid() const;

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
    // areg::ComponentEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::ComponentEntry, defines Component Entry.
     *          The Component Entry may contain list of implemented Services,
     *          list of Worker Threads and list of Dependencies, if they exist.
     *          The mandatory for component is the Role Name. All others are listed
     *          up to the Component design.
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
        /**
         * \brief   Creates invalid Component Entry.
         **/
        ComponentEntry();

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          list of implemented service interfaces, list of dependent services and list of worker threads.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         **/
        ComponentEntry( const areg::String & masterThreadName, const areg::String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          list of implemented service interfaces, list of dependent services and list of worker threads.
         *          The list parameters can be empty.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         * \param   serviceList         The list of implemented service interfaces
         * \param   dependencyList      The list of dependent service interfaces
         * \param   workerList          The list of worker threads
         **/
        ComponentEntry(   const areg::String & masterThreadName
                        , const areg::String & roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const areg::ServiceList & serviceList
                        , const areg::DependencyList & dependencyList
                        , const areg::WorkerThreadList & workerList);

        /**
         * \brief   Initialize Component Entry by given Role Name, component thread, component create and delete methods,
         *          single entry of implemented service interface, single entry of dependent service and single entry of worker threads.
         *          The entries can be empty.
         * \param   masterThreadName    The name of Master Thread where the component is created and run
         * \param   roleName            The Role Name of Component
         * \param   funcCreate          Pointer of component create function
         * \param   funcDelete          Pointer of component delete function
         * \param   service             The entry of implemented service interface, if there is any
         * \param   dependency          The entry of dependent service interface, if there is any
         * \param   worker              The entry of worker thread, if there is any
         **/
        ComponentEntry(   const areg::String & masterThreadName
                        , const areg::String & roleName
                        , FuncCreateComponent funcCreate
                        , FuncDeleteComponent funcDelete
                        , const areg::ServiceEntry & service
                        , const areg::DependencyEntry & dependency
                        , const areg::WorkerThreadEntry & worker);

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        ComponentEntry( const areg::ComponentEntry & src );

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ComponentEntry( areg::ComponentEntry && src ) noexcept;

        /**
         * \brief   Destructor
         **/
        ~ComponentEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entry data from given source.
         * \param   src     The source of data to copy
         **/
        areg::ComponentEntry & operator = ( const areg::ComponentEntry & src );

        /**
         * \brief   Moves Component Entry data from given source.
         * \param   src     The source of data to move.
         **/
        areg::ComponentEntry & operator = ( areg::ComponentEntry && src ) noexcept;

        /**
         * \brief   Checks equality of two Component Entries and returns true if they are equal.
         *          It compares Master Thread name, Role Name, and pointers of Create and Delete Component functions.
         * \param   other   The Component Entry to compare.
         **/
        bool operator == ( const areg::ComponentEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentEntry class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component Entry is valid.
         *          The Entry is valid if Role Name of Component Entry is not empty.
         **/
        bool isValid() const;

        /**
         * \brief   Adds supported Service Entry in Component Entry object.
         *          The Service Entry is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different
         *          Service Interfaces implementation.
         * \param   entry   The Service Entry, defining implemented
         *                  Service Interface in Component.
         **/
        void addSupportedService( const areg::ServiceEntry & entry );

        /**
         * \brief   Adds List of supported Service Entries in Component Entry object.
         *          Every Service Entry in the List is defining implemented Service Interface,
         *          supported by Component. A Component may have more than one different
         *          Service Interfaces implementation.
         * \param   serviceList The list of Service Entries. Every Service Entry is defining
         *                      implemented Service Interfaces in Component.
         **/
        void addSupportedService( const areg::ServiceList & serviceList );

        /**
         * \brief   Adds supported service interface entry in the component entry object.
         *          The serviceName should be unique within component entry context.
         * \param   serviceName The name of supported service interface.
         *                      If not unique, no new entry added.
         * \param   version     The supported interface version.
         * \return  Returns either new added instance or the existing instance of supported service interface.
         *          The uniqueness is checked within component entry context and ignores the version number.
         **/
        areg::ServiceEntry & addSupportedService( const areg::String & serviceName, const areg::Version & version );

        /**
         * \brief   Searches given supported service. If found, removes from list.
         * \param   serviceName The name of supported service
         * \return  Returns true if found and remove supported service entry in the list.
         **/
        bool removeSupportedService( const areg::String & serviceName );

        /**
         * \brief   Searches Service Entry in the existing list of Component Entry
         *          and returns the zero-based valid position if found.
         *          Otherwise, returns -1, if cannot find Service Entry in the list.
         * \param   entry   The Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findSupportedService( const areg::ServiceEntry & entry ) const;

        /**
         * \brief   By given name, searches Service Entry in the existing list of
         *          Component Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Service Entry in the list.
         * \param   serviceName The Name of Service Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findSupportedService( const areg::String & serviceName ) const;

        /**
         * \brief   Adds Worker Thread Entry in Component Entry object.
         *          The Worker Thread Entry is defining Worker Thread object,
         *          bound with Component. A Component may have zero or more
         *          bound Worker Threads.
         * \param   entry   The Worker Thread Entry, defining Worker Thread
         *                  object bound with Component.
         **/
        void addWorkerThread( const areg::WorkerThreadEntry & entry );

        /**
         * \brief   Adds List of Worker Thread Entries in Component Entry object.
         *          Every Worker Thread Entry in the List is defining Worker Thread object,
         *          bound with Component. A Component may have zero or more
         *          bound Worker Threads.
         * \param   workerList  The List of Worker Thread Entries, defining list of
         *                      Worker Thread objects bound with Component.
         **/
        void addWorkerThread( const areg::WorkerThreadList & workerList );

        /**
         * \brief   Searches Worker Thread Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Worker Thread Entry in the list.
         * \param   entry   The Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findWorkerThread( const areg::WorkerThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Worker Thread Entry in the existing list of
         *          Component Entry and returns the valid position in the list.
         *          Returns -1, if cannot find Worker Thread Entry in the list.
         * \param   workerName  The Name of Worker Thread Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findWorkerThread( const areg::String & workerName ) const;

        /**
         * \brief   Searches the worker thread entry in the list.
         *          If found, removes it from the list.
         * \param   workerName  The name of worker thread to remove
         * \return  Returns true if worker thread entry found and could remove.
         **/
        bool removeWorkerThread( const areg::String & workerName );

        /**
         * \brief   Adds Dependency Entry in Component Entry object.
         *          The Dependency Entry is defining Client part of Server Component,
         *          specified by Role Name. A Component may have more than one same
         *          Service Interfaces dependency, but every dependency should
         *          differ by unique Role Name of Server Component.
         *          For example, if Service Interface "QWERT" is implemented in
         *          Components ABC and XYZ (the Role Names are "ABC" and "XYZ"),
         *          then dependency should be defined by Role Names "ABC" and "XYZ",
         *          indicating dependency of same Service Interface "QWERT", but
         *          different implementation in ABC and XYZ Components.
         * \param   entry   The Dependency Entry, defining Role Name of Server Component.
         **/
        void addDependencyService( const areg::DependencyEntry & entry );

        /**
         * \brief   Adds List of Dependency Entries in Component Entry object.
         *          Every Dependency Entry in the List is defining Client part of Server Component,
         *          specified by Role Name. A Component may have more than one same
         *          Service Interfaces dependency, but every dependency should
         *          differ by unique Role Name of Server Component.
         *          For example, if Service Interface "QWERT" is implemented in
         *          Components ABC and XYZ (the Role Names are "ABC" and "XYZ"),
         *          then dependency should be defined by Role Names "ABC" and "XYZ",
         *          indicating dependency of same Service Interface "QWERT", but
         *          different implementation in ABC and XYZ Components.
         * \param   dependencyList  The Dependency Entry, defining Role Name of Server Component.
         **/
        void addDependencyService( const areg::DependencyList & dependencyList );

        /**
         * \brief   Adds a dependency entry in the component. The passed parameter should be
         *          the name of local or public service.
         * \param   roleName    The name dependent service.
         * \return  Returns the instance of new added or existing dependency of specified service.
         **/
        areg::DependencyEntry & addDependencyService( const areg::String & roleName );

        /**
         * \brief   Searches Dependency Entry in the existing list of Component Entry
         *          and returns the valid position in the list. Returns -1, if cannot
         *          find Dependency Entry in the list.
         * \param   entry   The Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findDependencyService( const areg::DependencyEntry & entry ) const;

        /**
         * \brief   Searches Service in dependency service list by given Role Name.
         *          If found, removes entry from the list.
         * \param   roleName    The name of dependency service role name to search.
         * \return  Returns true if found entry and could remove it.
         **/
        bool removeDependencyService( const areg::String & roleName );

        /**
         * \brief   By given name, searches Dependency Entry in the existing list of
         *          Component Entry and returns the valid position in the list.
         *          Returns -1, if cannot find Dependency Entry in the list.
         * \param   roleName    The Name of Dependency Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findDependencyService(const areg::String & roleName) const;

        /**
         * \brief   Returns list of supported Services of Component Entry object
         **/
        const areg::ServiceList & getSupportedServices() const;

        /**
         * \brief   Returns list of Worker Threads of Component Entry object
         **/
        const areg::WorkerThreadList & getWorkerThreads() const;

        /**
         * \brief   Returns list of Dependencies of Component Entry object
         **/
        const areg::DependencyList & getDependencyServices() const;

        /**
         * \brief   Sets pointers to create and delete component methods to be called when instantiating component object.
         *          If nullptr, it clears existing pointers and no component is instantiated.
         * \param   fnCreate    The pointer to create component method.
         * \param   fnDelete    The pointer to delete component method.
         **/
        void setInstanceMethods( FuncCreateComponent fnCreate, FuncDeleteComponent fnDelete );

        /**
         * \brief   Sets component data to pass to component create method.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    You should manually free memory if the data was manually allocated in the memory
         **/
        void setData( std::any compData );

        /**
         * \brief   Resets component data.
         **/
        void resetData();

        /**
         * \brief   Returns component data object. Check if data holds a value before use.
         **/
        std::any getData() const;

        /**
         * \brief   Returns true if component data is set. Otherwise, returns false.
         **/
        bool hasData() const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   The Role Name of Component
         **/
        areg::String              mRoleName;
        /**
         * \brief   The Master Thread Entry of Component.
         **/
        areg::String              mThreadName;
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
    // areg::ComponentEntryList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   areg::ComponentList. Defines list of Component Entries.
     *          It is a list of all Components within one Thread.
     *          All Components should be valid and have unique Role Name.
     *          None of Components should have same implemented Service Interface.
     *          If more than one Components are implementing same Service Interface,
     *          they should be listed in different list of different thread.
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
        using ComponentListBase = areg::ArrayList<areg::ComponentEntry>;

        /**
         * \brief   Creates empty list.
         **/
        ComponentList() = default;

        /**
         * \brief   Creates Component List and adds specified Component Entry to the list as first element.
         * \param   entry   The Component Entry to set as first element in the list.
         **/
        explicit ComponentList(  const areg::ComponentEntry & entry );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        ComponentList( const areg::ComponentList & src ) = default;

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ComponentList( areg::ComponentList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentList() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Component Entries from given source of Component List.
         * \param   src     The source of Component List, containing Component Entries.
         **/
        areg::ComponentList & operator = ( const areg::ComponentList & src ) = default;

        /**
         * \brief   Moves Component Entries from given source of Component List.
         * \param   src     The source of Component List, containing Component Entries.
         **/
        areg::ComponentList & operator = ( areg::ComponentList && src ) noexcept = default;

        /**
         * \brief   Operator to access elements of the service component list by index.
         * \param   index   Must be valid index of the entry.
         * \return  The instance of service element entry for real-only operations.
         *          Returns instance of invalid component entry if index is invalid.
         **/
        inline const areg::ComponentEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Component List is valid.
         **/
        bool isValid() const;

        /**
         * \brief   Searches Component Entry having given name. Returns valid
         *          Component Entry if found, otherwise returns invalid component entry.
         *          There should not be several Component Entries with same name.
         * \param   roleName    The name of Component Entry to search.
         * \return  Returns valid Component Entry if found. Otherwise returns
         *          invalid component entry.
         **/
        const areg::ComponentEntry & getComponent( const areg::String & roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    You should manually free memory if the data was manually allocated in the memory
         **/
        bool setComponentData( const areg::String & roleName, std::any compData );

        /**
         * \brief   Searches in the list the component by given name. If found, resets component data.
         *          Returns true if found component and the data was successfully reset.
         * \param   roleName    The name of component to search in the list.
         **/
        bool resetComponentData( const areg::String& roleName );

        /**
         * \brief   Searches Component Entry by given Role Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   roleName    The Role Name of Component Entry to search.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findComponent( const areg::String & roleName ) const;

        /**
         * \brief   Searches specified Component Entry and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Component Entry to search in the list.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findComponent(const areg::ComponentEntry& entry) const;

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
    // areg::ComponentThreadEntry class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::ComponentThreadEntry. Defines Master Thread of one or
     *          mode registered Component. All registered Components are
     *          created in specified thread. One Thread should have at least
     *          one registered Component. All registered Components should have
     *          unique Role Names and different Service Names.
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
        /**
         * \brief   Creates invalid Thread Entry.
         **/
        ComponentThreadEntry();

        /**
         * \brief   Initialize Thread Entry with given Thread Name.
         * \param   threadName  The Thread Name to assign.
         * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component thread.
         *                              The value 0 (areg::WATCHDOG_IGNORE) ignores watchdog.
         * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024 Bytes).
         *                              Pass `areg::STACK_SIZE_DEFAULT` (0) to ignore changing stack size and use system default stack size.
         * \param   maxQueue            The maximum size of message queue for the thread.
         *                              Pass areg::IGNORE_VALUE to use default value set in configuration or ignore the parameter if not configured.
         **/
        explicit ComponentThreadEntry( const areg::String & threadName
                                     , const uint32_t watchdogTimeout   = areg::WATCHDOG_IGNORE
                                     , const uint32_t stackSizeKb       = areg::STACK_SIZE_DEFAULT
                                     , const uint32_t maxQueue          = areg::IGNORE_VALUE);

        /**
         * \brief   Initialize Thread Entry with given Thread Name and given Component List.
         * \param   threadName      The Thread Name to assign.
         * \param   componentList   The List of Component Entries to assign.
         * \param   watchdogTimeout     The watchdog timeout in milliseconds to guard component thread.
         *                              The value 0 (areg::WATCHDOG_IGNORE) ignores watchdog.
         * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024 Bytes).
         *                              Pass `areg::STACK_SIZE_DEFAULT` (0) to ignore changing stack size and use system default stack size.
         * \param   maxQueue            The maximum size of message queue for the thread.
         **/
        ComponentThreadEntry( const areg::String & threadName
                            , const areg::ComponentList & componentList
                            , const uint32_t watchdogTimeout= areg::WATCHDOG_IGNORE
                            , const uint32_t stackSizeKb    = areg::STACK_SIZE_DEFAULT
                            , const uint32_t maxQueue       = areg::IGNORE_VALUE );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        ComponentThreadEntry( const areg::ComponentThreadEntry & src ) = default;

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        ComponentThreadEntry( areg::ComponentThreadEntry && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentThreadEntry() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentThreadEntry class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entry data from given source.
         * \param   src     The source of data to copy
         **/
        areg::ComponentThreadEntry & operator = ( const areg::ComponentThreadEntry & src ) = default;

        /**
         * \brief   Moves Thread Entry data from given source.
         * \param   src     The source of data to move.
         **/
        areg::ComponentThreadEntry & operator = ( areg::ComponentThreadEntry && src ) noexcept = default;

        /**
         * \brief   Checks equality of two Thread Entries and returns true if they are equal.
         *          It compares Thread names.
         * \param   other   The Thread Entry to compare.
         **/
        bool operator == ( const areg::ComponentThreadEntry & other ) const;
        bool operator != ( const areg::ComponentThreadEntry & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentThreadEntry class, Attribute and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread Entry is valid.
         *          The Entry is valid if Name of Thread Entry is not empty.
         **/
        bool isValid() const;

        /**
         * \brief   Adds supported Component Entry in Thread Entry object.
         *          A Thread may have more than one Component with unique
         *          Role Name, and every Component within one Thread should
         *          have implementation of different Service Interfaces.
         *          If there is a Service Interface with different implementation,
         *          they should be defined in different threads.
         *          For example, if Service Interface "QWERT" is implemented
         *          in Component ABC and XYZ, they should not be in the
         *          same Thread Entry, but can be in different Thread.
         *          Otherwise, same Event will be processed twice in
         *          every Stub of same Service Interface.
         * \param   entry   The Component Entry, defining Component in Thread.
         **/
        void addComponent( const areg::ComponentEntry & entry );

        /**
         * \brief   Adds List of supported Component Entries in Thread Entry object.
         *          A Thread may have more than one Component with unique
         *          Role Name, and every Component within one Thread should
         *          have implementation of different Service Interfaces.
         *          If there is a Service Interface with different implementation,
         *          they should be defined in different threads.
         *          For example, if Service Interface "QWERT" is implemented
         *          in Component ABC and XYZ, they should not be in the
         *          same Thread Entry, but can be in different Thread.
         *          Otherwise, same Event will be processed twice in
         *          every Stub of same Service Interface.
         * \param   componentList   The List of Component Entries. Every Component Entry is defining
         *                          Component in Thread.
         **/
        void addComponent( const areg::ComponentList & componentList );

        /**
         * \brief   Adds a new component entry with the given role.
         * \param   roleName    The name of component to add. The name must be unique to add new entry.
         * \param   funcCreate  The pointer to the method that instantiates the component.
         * \param   funcDelete  The pointer to the method that deletes the component.
         * \return  Returns instance of new added or the instance of the existing component entry with
         *          the given role name. The checkup happens only within the current thread list.
         *
         * \note    NOTE:   The method does not check the uniqueness of role name within the entire system or entire
         *                  model. It checks only within the current component thread entry context.
         **/
        areg::ComponentEntry & addComponent( const areg::String & roleName, FuncCreateComponent funcCreate, FuncDeleteComponent funcDelete );

        template<typename ComponentType>
        inline areg::ComponentEntry& addComponent(const areg::String& roleName);

        /**
         * \brief   Searches the component entry by given role name.
         *          If found, remove the component entry from the list.
         * \param   roleName    The roleName of service component to search and remove.
         * \return  Returns true if found and could remove the component entry.
         **/
        bool removeComponent( const areg::String & roleName );

        /**
         * \brief   Searches Component Entry in the existing list of Thread Entry
         *          and returns the zero-based valid position if found.
         *          Otherwise, returns -1, if cannot find Component Entry in the list.
         * \param   entry   The Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findComponentEntry( const areg::ComponentEntry & entry ) const;

        /**
         * \brief   By given name, searches Component Entry in the existing list of
         *          Thread Entry and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Component Entry in the list.
         * \param   roleName    The Name of Component Entry to search in the list.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findComponentEntry( const areg::String & roleName ) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    You should manually free memory if the data was manually allocated in the memory
         **/
        bool setComponentData( const areg::String & roleName, std::any compData );

        /**
         * \brief   Searches in the list the component by given name. If found, resets component data.
         *          Returns true if found component and the data was successfully reset.
         * \param   roleName    The name of component to search in the list.
         **/
        bool resetComponentData( const areg::String & roleName );

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentThreadEntry class, Member variables
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   The name of thread component (thread name)
         **/
        areg::String          mThreadName;

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
    // areg::ComponentThreadList class declaration
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   areg::ComponentThreadList. Defines list of Thread Entries in Model.
     *          All names of Thread should be unique. A Model has more than one
     *          Thread Entry.
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
        using ComponentThreadListBase = areg::ArrayList< areg::ComponentThreadEntry>;

        /**
         * \brief   Default constructor.
         **/
        ComponentThreadList() = default;

        /**
         * \brief   Creates Component Thread List and adds specified Thread Entry to the list as first element.
         * \param   entry   The Thread Entry to set as first element in the list.
         **/
        explicit ComponentThreadList( const areg::ComponentThreadEntry & entry );

        /**
         * \brief   Copies entries from given source.
         * \param   src     The source of data to copy.
         **/
        ComponentThreadList( const areg::ComponentThreadList & src ) = default;

        /**
         * \brief   Moves entries from given source.
         * \param   src     The source of data to move.
         **/
        ComponentThreadList( areg::ComponentThreadList && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~ComponentThreadList() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentThreadList class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies Thread Entries from given source of Thread List.
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        areg::ComponentThreadList & operator = ( const areg::ComponentThreadList & src ) = default;

        /**
         * \brief   Moves Thread Entries from given source of Thread List.
         * \param   src     The source of Thread List, containing Thread Entries.
         **/
        areg::ComponentThreadList & operator = ( areg::ComponentThreadList && src ) noexcept = default;

        /**
         * \brief   Operator to access elements of the service component thread list by index.
         * \param   index   Must be valid index of the entry.
         * \return  The instance of service element entry for real-only operations.
         *          Returns instance of invalid thread entry if index is invalid.
         **/
        inline const areg::ComponentThreadEntry& operator [] (uint32_t index) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::ComponentThreadList class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true if Thread List is valid.
         **/
        bool isValid() const;

        /**
         * \brief   Searches Thread Entry having given name. Returns valid
         *          Thread Entry if found, otherwise returns invalid thread entry.
         *          There should not be several Thread Entries with same name.
         * \param   threadName  The name of Thread Entry to search.
         * \return  Returns valid Thread Entry if found. Otherwise returns
         *          invalid thread entry.
         **/
        const areg::ComponentThreadEntry & getThread( const areg::String & threadName ) const;

        /**
         * \brief   Searches Thread Entry by given Thread Name and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   threadName  The Thread Name of Thread Entry to search.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findThread( const areg::String & threadName ) const;

        /**
         * \brief   Searches specified Thread Entry and returns
         *          zero-based valid index if found. The valid index is not negative.
         * \param   entry   The Thread Entry to search in the list.
         * \return  Returns zero-based valid index if found entry.
         *          Otherwise, returns -1.
         **/
        int32_t findThread( const areg::ComponentThreadEntry & entry ) const;

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
    // areg::Model class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   areg::Model. Defines Model of project / binary.
     *          Every project / binary can have only one Model, specifying model of
     *          Threads, Components and dependencies.
     *          If several Models should be defined in application, they should
     *          be specified separately in separate projects / binaries.
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
        /**
         * \brief   Creates empty Model.
         **/
        Model();

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        explicit Model( const areg::String & modelName );

        /**
         * \brief   Initialize empty Model and sets given Name.
         **/
        Model( const areg::String & modelName, const ComponentThreadList & threadList );

        /**
         * \brief   Copies / moves model entries from given source.
         **/
        Model( const Model & src ) = default;
        Model( Model && src ) noexcept = default;

        /**
         * \brief   Destructor
         **/
        ~Model() = default;

    //////////////////////////////////////////////////////////////////////////
    // areg::Model class, Operators
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Copies /Moves Model data from given source,
         **/
        areg::Model & operator = ( const areg::Model & src ) = default;
        areg::Model & operator = ( areg::Model && src ) noexcept = default;

        /**
         * \brief   Checks equality of two Models and returns true if they are equal.
         *          It will compare Model names and registered Thread Entries.
         **/
        bool operator == ( const areg::Model & other ) const;
        bool operator != ( const areg::Model & other ) const;

    //////////////////////////////////////////////////////////////////////////
    // areg::Model class, Attributes and Operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Returns true, if Mode is valid.
         *          The Model is valid if Name is not empty and it contains at least one thread entry.
         **/
        bool isValid() const;

        /**
         * \brief   Returns the name of model.
         **/
        const areg::String & getModelName() const;

        /**
         * \brief   Searches specified Component Entry  and returns true if
         *          found specified Component Entry. Otherwise returns false.
         * \param   entry   The Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool hasRegisteredComponent( const areg::ComponentEntry & entry ) const;

        /**
         * \brief   Searches specified Component Entry  and returns true if
         *          found specified Component Entry by Role Name. Otherwise returns false.
         * \param   roleName    The Role Name of Component Entry to search in entire Model.
         * \return  Returns true if found specified Component Entry.
         *          Otherwise returns false.
         **/
        bool hasRegisteredComponent( const areg::String & roleName ) const;

        /**
         * \brief   Returns true if Model is loaded. Otherwise, returns false.
         **/
        bool isModelLoaded() const;

        /**
         * \brief   Returns the List of Thread Entries.
         **/
        const areg::ComponentThreadList & getThreadList() const;

        /**
         * \brief   Sets flag and marks Model as loaded or unloaded.
         * \param   isLoaded    The flag to set.
         *                      If true, the Model is marked as loaded.
         *                      If false, the Model is marked as unloaded.
         **/
        void markModelLoaded( bool isLoaded = true );

        /**
         * \brief   Call to mark model as alive. It will start the timer and
         *          stop the timer when it is not alive anymore, so that the
         *          duration in nanoseconds can be calculated.
         * \param   isAlive     If true, the model is alive and it starts the timer.
         *                      If false, the model is not alive anymore and stop the timer.
         **/
        void markModelAlive( bool isAlive);

        /**
         * \brief   Adds supported Thread Entry to the list of Model.
         *          Every Thread Entry should have global unique Name.
         * \param   entry   The Thread Entry to add.
         **/
        void addThread( const areg::ComponentThreadEntry & entry );

        /**
         * \brief   Adds List of supported Thread Entries to the list of Model.
         *          Every Thread Entry in the List should have global unique Name.
         * \param   threadList  The list of Thread Entries to add.
         **/
        void addThread( const areg::ComponentThreadList & threadList );

        /**
         * \brief   Adds new component thread entry in the list if the given thread does not exit.
         * \param   threadName  The name of the thread to add. The name must be unique.
         * \return  Returns the instance of the component thread or returns the instance of the
         *          existing component thread if the thread with the given name is already registered.
         *
         * \note    NOTE:   The method does not check the uniqueness of thread entry in entire system
         *                  and does not check the lists of worker threads. It checks only within current
         *                  model context escaping worker thread names. The caller must take care of uniqueness
         *                  by its self.
         **/
        areg::ComponentThreadEntry & addThread(const areg::String & threadName );

        /**
         * \brief   Searches component thread entry in the list.
         *          If found, removes it.
         * \param   threadName  The name of component thread to search.
         * \return  Returns true if found and could remove the thread entry from the list.
         **/
        bool removeThread( const areg::String & threadName );

        /**
         * \brief   Searches Thread Entry in the existing list of Model
         *          and returns the zero-based valid position if found.
         *          Otherwise, returns -1, if cannot find Thread Entry in Model.
         * \param   entry   The Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findThread( const areg::ComponentThreadEntry & entry ) const;

        /**
         * \brief   By given name, searches Thread Entry in the existing list of
         *          Model and returns the zero-based valid position if found.
         *          Returns -1, if cannot find Thread Entry in Model.
         * \param   threadName  The Name of Thread Entry to search in Model.
         * \return  If Entry found, returns valid zero-based index of element.
         *          Otherwise, returns -1.
         **/
        int32_t findThread(const areg::String & threadName) const;

        /**
         * \brief   Searches in the list the component by given name. If found, sets component data.
         *          Returns true if found component and the data was successfully set.
         * \param   roleName    The name of component to search in the list.
         * \param   compData    The data to set in component which is passed to create method.
         * \note    You should manually free memory if the data was manually allocated in the memory
         **/
        bool setComponentData( const areg::String & roleName, std::any compData );

        /**
         * \brief   Searches in the list the component by given name. If found, resets component data.
         *          Returns true if found component and the data was successfully reset.
         * \param   roleName    The name of component to search in the list.
         **/
        bool resetComponentData(const areg::String& roleName);

        /**
         * \brief   Returns duration in nanoseconds when the model was loaded and alive.
         *          Returns zero if model was not loaded at all.
         *          Returns valid duration until the current time if model is still loaded.
         *          Returns last alive duration if it was alive and currently is stopped.
         **/
        inline TIME64 getAliveDuration() const;

    //////////////////////////////////////////////////////////////////////////
    // areg::Model class, Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   The Name of Model
         **/
        areg::String                  mModelName;

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
      * \brief   Predefined invalid Service Entry.
      **/
     AREG_API const areg::ServiceEntry & invalidServiceEntry();;

     /**
      * \brief   Predefined invalid Service List.
      **/
    AREG_API const areg::ServiceList & invalidServiceList();

    /**
     * \brief   Predefined invalid Worker Thread Entry.
     **/
    AREG_API const areg::WorkerThreadEntry & invalidWorkerThreadEntry();

    /**
     * \brief   Predefined invalid Worker Thread List.
     **/
    AREG_API const areg::WorkerThreadList & invalidWorkerThreadList();

    /**
     * \brief   Predefined invalid service dependency entry.
     **/
    AREG_API const areg::DependencyEntry & invalidDepedencyEntry();

    /**
     * \brief   Predefined invalid service dependency list.
     **/
    AREG_API const areg::DependencyList & invalidDepedencyList();

    /**
     * \brief   Predefined Invalid Component Entry.
     **/
    AREG_API const areg::ComponentEntry & invalidComponentEntry();

    /**
     * \brief   Predefined invalid Component List.
     **/
    AREG_API const areg::ComponentList & invalidComponentList();

    /**
     * \brief   Predefined invalid Thread Entry.
     **/
    AREG_API const areg::ComponentThreadEntry & invalidThreadEntry();

    /**
     * \brief   Predefined invalid Thread List. Contains single invalid thread entry.
     **/
    AREG_API const areg::ComponentThreadList & invalidThreadList();

    /**
     * \brief   Predefined invalid Model.
     **/
    AREG_API const areg::Model & invalidModel();
}

//////////////////////////////////////////////////////////////////////////
// NERegistry inline methods
//////////////////////////////////////////////////////////////////////////

inline const areg::ServiceEntry& areg::ServiceList::operator [] (uint32_t index) const
{
    return (mListServices.isValidIndex(index) ? mListServices[index] : areg::invalidServiceEntry());
}

inline const areg::WorkerThreadEntry& areg::WorkerThreadList::operator [] (uint32_t index) const
{
    return (mListWorkers.isValidIndex(index) ? mListWorkers[index] : areg::invalidWorkerThreadEntry());
}

inline const areg::DependencyEntry& areg::DependencyList::operator [] (uint32_t index) const
{
    return (mListDependencies.isValidIndex(index) ? mListDependencies[index] : areg::invalidDepedencyEntry());
}

inline const areg::ComponentEntry& areg::ComponentList::operator [] (uint32_t index) const
{
    return (mListComponents.isValidIndex(index) ? mListComponents[index] : areg::invalidComponentEntry());
}

inline const areg::ComponentThreadEntry& areg::ComponentThreadList::operator [] (uint32_t index) const
{
    return (mListThreads.isValidIndex(index) ? mListThreads[index] : areg::invalidThreadEntry());
}

inline TIME64 areg::Model::getAliveDuration() const
{
    return (mLoadState == ModelState::Initialized ? 0 : mAliveDuration.durationSinceStart());
}

template<typename ComponentType>
inline areg::ComponentEntry& areg::ComponentThreadEntry::addComponent(const areg::String& roleName)
{
    return addComponent(roleName
        , [](const areg::ComponentEntry& entry, ComponentThread& owner) -> Component* {
            return new ComponentType(entry, owner);
        }
        , [](Component& comp, const areg::ComponentEntry& /*entry*/) -> void {
            ComponentType* tmp = static_cast<ComponentType *>(&comp);
            delete tmp;
        });
}

#endif  // AREG_COMPONENT_MODEL_HPP
