#ifndef AREG_COMPONENT_REGISTRY_THREADENTRY_HPP
#define AREG_COMPONENT_REGISTRY_THREADENTRY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/registry/ThreadEntry.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *              Declared the following objects:
 *              WorkerThreadEntry    - Registry entry for a worker thread.
 *              ComponentThreadEntry - Defines Master Thread of the registered Component.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/registry/Model.hpp"
#include "areg/component/registry/ComponentEntry.hpp"

#include <any>

/************************************************************************
 * Declared classes
 ************************************************************************/

namespace areg {
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
     * \param   maxQueue            Maximum message queue size; IGNORE_VALUE uses configured or
     *                              default value.
     **/
    WorkerThreadEntry( const String& masterThreadName
                     , const String& workerThreadName
                     , const String& compRoleName
                     , const String& compConsumerName
                     , const uint32_t watchdogTimeout   = areg::WATCHDOG_IGNORE
                     , const uint32_t stackSizeKb       = areg::DEFAULT_STACK_SIZE
                     , const uint32_t maxQueue          = areg::IGNORE_VALUE);

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
     **/
    explicit ComponentThreadEntry( const String & threadName
                                 , const uint32_t watchdogTimeout   = areg::WATCHDOG_IGNORE
                                 , const uint32_t stackSizeKb       = areg::DEFAULT_STACK_SIZE
                                 , const uint32_t maxQueue          = areg::IGNORE_VALUE);

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
     **/
    ComponentThreadEntry( const String & threadName
                        , const areg::ComponentList & componentList
                        , const uint32_t watchdogTimeout= areg::WATCHDOG_IGNORE
                        , const uint32_t stackSizeKb    = areg::DEFAULT_STACK_SIZE
                        , const uint32_t maxQueue       = areg::IGNORE_VALUE );

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
};


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
 * \brief   Returns predefined invalid Thread List containing single invalid thread entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentThreadList & invalid_thread_list() noexcept;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// ThreadEntry inline methods
//////////////////////////////////////////////////////////////////////////


inline const areg::WorkerThreadEntry& areg::WorkerThreadList::operator [] (uint32_t index) const noexcept
{
    return (mListWorkers.is_valid_index(index) ? mListWorkers[index] : areg::invalid_worker_entry());
}
inline const areg::ComponentThreadEntry& areg::ComponentThreadList::operator [] (uint32_t index) const noexcept
{
    return (mListThreads.is_valid_index(index) ? mListThreads[index] : areg::invalid_thread_entry());
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

#endif // AREG_COMPONENT_REGISTRY_THREADENTRY_HPP
