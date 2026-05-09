#ifndef AREG_COMPONENT_REGISTRY_LISTS_HPP
#define AREG_COMPONENT_REGISTRY_LISTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/registry/Lists.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *              Declared the following objects:
 *              ServiceList         - List of service entries implemented by a component.
 *              DependencyList      - List of dependencies between components.
 *              WorkerThreadList    - Worker thread entries associated with a component.
 *              ComponentList       - List of component entries within a thread.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ArrayList.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <any>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
  class ServiceEntry;
  class DependencyEntry;
  class WorkerThreadEntry;
  class ComponentEntry;
}

namespace areg {
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
// Lists namespace Predefined Invalid Entries.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Returns predefined invalid Service List.
 **/
[[nodiscard]]
AREG_API const areg::ServiceList & invalid_service_list() noexcept;

/**
 * \brief   Returns predefined invalid service dependency list.
 **/
[[nodiscard]]
AREG_API const areg::DependencyList & invalid_depedency_list() noexcept;

/**
 * \brief   Returns predefined invalid Component List.
 **/
[[nodiscard]]
AREG_API const areg::ComponentList & invalid_component_list() noexcept;

} // namepsace areg

#endif // AREG_COMPONENT_REGISTRY_LISTS_HPP

