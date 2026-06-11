#ifndef AREG_LOGGING_PRIVATE_SCOPECONTROLLER_HPP
#define AREG_LOGGING_PRIVATE_SCOPECONTROLLER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeController.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Scope Controller.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ResourceMap.hpp"

#include "areg/logging/LoggingDefs.hpp"

#include <string_view>

#if AREG_LOGGING
/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {

    class LogScope;
    class Property;
}

namespace areg {

//!< The map of scopes to configure
using ListScopes    = StringToIntegerHashMap;
    
//!< The hash map container of all logging scopes.
using LogScopeMap   = HashMap<uint32_t, LogScope *>;

//!< Initial reservation for the scope map to avoid rehashing during static initialization.
inline constexpr uint32_t SCOPE_MAP_INITIAL_RESERVE { 512u };

/**
 * \brief   Container and controller of all registered log scopes in the application. Manages scope
 *          registration, activation, and priority changes including individual scopes and scope groups.
 **/
class ScopeController
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ScopeController();
    ~ScopeController() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns a read-only list of registered scopes.
     **/
    [[nodiscard]]
    inline const LogScopeMap & scope_list() const noexcept;

    /**
     * \brief   Returns the log scope object with the specified ID, or nullptr if not found.
     *
     * \param   scopeId     The log scope ID to lookup.
     **/
    [[nodiscard]]
    inline LogScope * scope( uint32_t scopeId ) const noexcept;

    /**
     * \brief   Returns the log scope object with the specified name, or nullptr if not found.
     *
     * \param   scopeName       The unique log scope name to lookup.
     **/
    [[nodiscard]]
    inline LogScope * scope( const char * scopeName ) const noexcept;

    /**
     * \brief   Returns true if a log scope with the specified ID is registered.
     *
     * \param   scopeId     The log scope ID to lookup.
     **/
    [[nodiscard]]
    bool is_scope_registered( uint32_t scopeId ) const noexcept;

    /**
     * \brief   Returns true if a log scope with the specified name is registered.
     *
     * \param   scopeName       The log scope name to lookup.
     **/
    [[nodiscard]]
    bool is_scope_registered( const char * scopeName ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers a log scope in the scope controller.
     *
     * \param   scope       The log scope object to register.
     **/
    void register_scope( LogScope & scope );

    /**
     * \brief   Unregisters a log scope from the scope controller.
     *
     * \param   scope       The log scope object to unregister.
     **/
    void unregister_scope( LogScope & scope );

    /**
     * \brief   Sets the logging priority for a scope by ID.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   newPrio     The priority value to set.
     **/
    void set_scope_priority( uint32_t scopeId, uint32_t newPrio ) noexcept;

    /**
     * \brief   Sets the logging priority for a scope by ID using a priority name.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   newPrio     The name of the priority to set.
     **/
    inline void set_scope_priority( uint32_t scopeId, const String & newPrio ) noexcept;

    /**
     * \brief   Sets the logging priority for a scope by name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   newPrio         The priority value to set.
     **/
    inline void set_scope_priority( const String & scopeName, uint32_t newPrio ) noexcept;

    /**
     * \brief   Sets the logging priority for a scope by name using a priority name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   newPrio         The name of the priority to set.
     **/
    inline void set_scope_priority( const String & scopeName, const String & newPrio ) noexcept;

    /**
     * \brief   Adds a priority flag to a scope by ID.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   addPrio     The priority value to add.
     **/
    void add_scope_priority( uint32_t scopeId, areg::LogPriority addPrio );

    /**
     * \brief   Adds a priority flag to a scope by ID using a priority name.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   addPrio     The name of the priority to add.
     **/
    inline void add_scope_priority( uint32_t scopeId, const String & addPrio );

    /**
     * \brief   Adds a priority flag to a scope by name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   addPrio         The priority value to add.
     **/
    inline void add_scope_priority( const String & scopeName, areg::LogPriority addPrio );

    /**
     * \brief   Adds a priority flag to a scope by name using a priority name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   addPrio         The name of the priority to add.
     **/
    inline void add_scope_priority( const String & scopeName, const String & addPrio );

    /**
     * \brief   Removes a priority flag from a scope by ID.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   remPrio     The priority value to remove.
     **/
    void remove_scope_priority( uint32_t scopeId, areg::LogPriority remPrio );

    /**
     * \brief   Removes a priority flag from a scope by ID using a priority name.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   remPrio     The name of the priority to remove.
     **/
    inline void remove_scope_priority( uint32_t scopeId, const String & remPrio );

    /**
     * \brief   Removes a priority flag from a scope by name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   remPrio         The priority value to remove.
     **/
    inline void remove_scope_priority( const String & scopeName, areg::LogPriority remPrio );

    /**
     * \brief   Removes a priority flag from a scope by name using a priority name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   remPrio         The name of the priority to remove.
     **/
    inline void remove_scope_priority( const String & scopeName, const String & remPrio );

    /**
     * \brief   Sets the logging priority for all scopes in a group.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   newPrio             The priority value to set to each scope.
     * \return  The number of scopes whose priority was set; 0 if no scopes matched.
     **/
    int32_t set_group_priority( const String & scopeGroupName, uint32_t newPrio );

    /**
     * \brief   Sets the logging priority for all scopes in a group using a priority name.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   newPrio             The name of the priority to set.
     * \return  The number of scopes whose priority was set; 0 if no scopes matched.
     **/
    inline int32_t set_group_priority( const String & scopeGroupName, const String & newPrio ) noexcept;

    /**
     * \brief   Adds a priority flag to all scopes in a group.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   addPrio             The priority value to add to each scope.
     * \return  The number of scopes whose priority was modified; 0 if no scopes matched.
     **/
    int32_t add_group_priority( const String & scopeGroupName, areg::LogPriority addPrio );

    /**
     * \brief   Adds a priority flag to all scopes in a group using a priority name.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   addPrio             The name of the priority to add.
     * \return  The number of scopes whose priority was modified; 0 if no scopes matched.
     **/
    inline int32_t add_group_priority( const String & scopeGroupName, const String & addPrio );

    /**
     * \brief   Removes a priority flag from all scopes in a group.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   remPrio             The priority value to remove from each scope.
     * \return  The number of scopes whose priority was modified; 0 if no scopes matched.
     **/
    int32_t remove_group_priority( const String & scopeGroupName, areg::LogPriority remPrio );

    /**
     * \brief   Removes a priority flag from all scopes in a group using a priority name.
     *
     * \param   scopeGroupName      The name of the scope group (typically ending with '*').
     * \param   remPrio             The name of the priority to remove.
     * \return  The number of scopes whose priority was modified; 0 if no scopes matched.
     **/
    inline int32_t remove_group_priority( const String & scopeGroupName, const String & remPrio );

    /**
     * \brief   Clears the list of configured scopes.
     **/
    inline void clear_config_scopes();

    /**
     * \brief   Resets and disables all scope priorities.
     **/
    void reset();

    /**
     * \brief   Activates the default scopes configured internally.
     **/
    void activate_defaults();

    /**
     * \brief   Forces to activate all scopes of the application regardless of their configured priorities.
     *          This is used for the case when logging needs to be enabled temporarily for debugging or 
     *          troubleshooting purposes.
     * \param   activate    If true, activates all scopes; if false, deactivates all scopes.
     **/
    void force_activate_scopes(bool activate = true);

    /**
     * \brief   Configures a scope or scope group priority from a property.
     *
     * \param   prop    The property containing the scope name and priority information.
     **/
    void configure_scopes( const Property & prop );

    /**
     * \brief   Sets the logging priority for a scope or scope group.
     *
     * \param   scopeName       The name of a single scope or scope group ending with '*'.
     * \param   scopePrio       The priority to set.
     **/
    void configure_scopes( const String & scopeName, uint32_t scopePrio );

    /**
     * \brief   Applies configured priorities from the internal configuration to all scopes.
     **/
    void configure_scopes();

    /**
     * \brief   Activates a scope without changing its priority.
     *
     * \param   logScope    The scope to activate.
     **/
    void activate_scope( LogScope & logScope );

    /**
     * \brief   Activates a scope and sets its logging priority.
     *
     * \param   logScope        The scope to activate and set priority for.
     * \param   defaultPrio     The default priority to set.
     **/
    void activate_scope( LogScope & logScope, uint32_t defaultPrio );

    /**
     * \brief   Activates or deactivates all scopes based on their configured priorities.
     *
     * \param   makeActive      If true, activates scopes with priorities from the configuration. If
     *                          false, deactivates all scopes.
     **/
    void set_scope_activity( bool makeActive );

    /**
     * \brief   Changes the logging priority of a scope or scope group.
     *
     * \param   scopeName       The name of a single scope or scope group.
     * \param   scopeId         The ID of the scope; can be 0 for scope groups.
     * \param   logPrio         The priority to set.
     **/
    void set_scope_activity( const String & scopeName, uint32_t scopeId, uint32_t logPrio );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Returns true if the scope name indicates a scope group (ends with '*').
     *
     * \param   scopeName       The scope name to check.
     * \return  True if the name ends with '*'; false otherwise.
     **/
    [[nodiscard]]
    static inline bool _is_scope_group( const String & scopeName ) noexcept;

    /**
     * \brief   Finds and returns the log scope object with the specified ID, or nullptr if not found.
     *
     * \param   scopeId     The log scope ID to lookup.
     **/
    [[nodiscard]]
    inline LogScope* _find(uint32_t scopeId) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    //!< The list of scopes read out from configuration file.
    ListScopes          mConfigScopeList;
    //!< The list of scope groups read out from configuration file
    ListScopes          mConfigScopeGroup;
    //!< The map of registered log scope objects.
    LogScopeMap         mMapLogScope;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( ScopeController );
};

//////////////////////////////////////////////////////////////////////////
// ScopeController inline methods
//////////////////////////////////////////////////////////////////////////

inline const LogScopeMap & ScopeController::scope_list() const noexcept
{
    return mMapLogScope;
}

inline LogScope * ScopeController::scope( uint32_t scopeId ) const noexcept
{
    return _find(scopeId);
}

inline LogScope * ScopeController::scope( const char * scopeName ) const noexcept
{
    return scope( areg::make_id(scopeName) );
}

inline bool ScopeController::is_scope_registered( uint32_t scopeId ) const noexcept
{
    return (scope(scopeId) != nullptr);
}

inline bool ScopeController::is_scope_registered( const char * scopeName ) const noexcept
{
    return (scope( scopeName ) != nullptr);
}

inline void ScopeController::set_scope_priority( uint32_t scopeId, const String & newPrio ) noexcept
{
    set_scope_priority( scopeId, static_cast<uint32_t>(areg::string_to_priority( newPrio )) );
}

inline void ScopeController::set_scope_priority( const String & scopeName, uint32_t newPrio ) noexcept
{
    set_scope_priority( areg::make_id( scopeName ), newPrio );
}

inline void ScopeController::set_scope_priority( const String & scopeName, const String & newPrio ) noexcept
{
    set_scope_priority( areg::make_id( scopeName ), static_cast<uint32_t>(areg::string_to_priority( newPrio )) );
}

inline void ScopeController::add_scope_priority( uint32_t scopeId, const String & addPrio )
{
    add_scope_priority( scopeId, areg::string_to_priority( addPrio ) );
}

inline void ScopeController::add_scope_priority( const String & scopeName, areg::LogPriority addPrio )
{
    add_scope_priority( areg::make_id( scopeName ), addPrio );
}

inline void ScopeController::add_scope_priority( const String & scopeName, const String & addPrio )
{
    add_scope_priority( areg::make_id( scopeName ), areg::string_to_priority( addPrio ) );
}

inline void ScopeController::remove_scope_priority( uint32_t scopeId, const String & remPrio )
{
    remove_scope_priority( scopeId, areg::string_to_priority( remPrio ) );
}

inline void ScopeController::remove_scope_priority( const String & scopeName, areg::LogPriority remPrio )
{
    remove_scope_priority( areg::make_id( scopeName ), remPrio );
}

inline void ScopeController::remove_scope_priority( const String & scopeName, const String & remPrio )
{
    remove_scope_priority( areg::make_id( scopeName ), areg::string_to_priority( remPrio ) );
}

inline int32_t ScopeController::set_group_priority( const String & scopeGroupName, const String & newPrio ) noexcept
{
    return set_group_priority( scopeGroupName, static_cast<uint32_t>(areg::string_to_priority( newPrio )) );
}

inline int32_t ScopeController::add_group_priority( const String & scopeGroupName, const String & addPrio )
{
    return add_group_priority( scopeGroupName, areg::string_to_priority( addPrio ) );
}

inline int32_t ScopeController::remove_group_priority( const String & scopeGroupName, const String & remPrio )
{
    return remove_group_priority( scopeGroupName, areg::string_to_priority( remPrio ) );
}

inline void ScopeController::clear_config_scopes()
{
    mConfigScopeList.clear( );
    mConfigScopeGroup.clear( );
}

inline LogScope* ScopeController::_find(uint32_t scopeId) const noexcept
{
    LogScopeMap::MAPPOS pos = mMapLogScope.find(scopeId);
    return (pos != mMapLogScope.invalid_position()) ? mMapLogScope.value_at(pos) : nullptr;
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_SCOPECONTROLLER_HPP
