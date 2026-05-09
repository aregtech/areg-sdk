#ifndef AREG_COMPONENT_REGISTRY_MODEL_HPP
#define AREG_COMPONENT_REGISTRY_MODEL_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/registry/Model.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, collection of Service Registry
 *              classes to initialize and load application objects.
 *              Declared the following objects:
 *              ComponentThreadList   - List of thread entries in a model.
 *              Model                 - Defines the model of a project/binary.
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
class ComponentEntry;
    class Component;
    class ComponentThreadEntry;
} // namespace areg

namespace areg {

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

/**
 * \brief   Returns predefined invalid Thread List containing single invalid thread entry.
 **/
[[nodiscard]]
AREG_API const areg::ComponentThreadList & invalid_thread_list() noexcept;

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
/**
 * \brief   Returns predefined invalid Model.
 **/
AREG_API const areg::Model & invalid_model() noexcept;


} // namespace areg

//////////////////////////////////////////////////////////////////////////
// NERegistry inline methods
//////////////////////////////////////////////////////////////////////////

inline TIME64 areg::Model::alive_duration() const noexcept
{
    return (mLoadState == ModelState::Initialized ? 0 : mAliveDuration.duration_since_start());
}

#endif
