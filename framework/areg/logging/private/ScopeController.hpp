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

#if AREG_LOGS
/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {

    class LogScope;
    class Property;
}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ScopeController::LogScopeMap class declaration
//////////////////////////////////////////////////////////////////////////
//!< Scope hash map
using MapLogScope   = areg::ScopeList;
//!< Scope resource map helper
using ImplLogScope  = ResourceMapImpl<uint32_t, LogScope *>;
//!< The log scope key-value pair.
using LogScopePair  = std::pair<uint32_t, LogScope *>;
//!< The map of scopes to configure
using ListScopes    = StringToIntegerHashMap;
    
/**
 * \brief   Resource map, container of all logging scopes.
 **/
class LogScopeMap   : public ConcurrentResourceMap<uint32_t, LogScope *, MapLogScope, ImplLogScope>
{
    friend class NetTcpLogger;
    friend class ScopeController;
    friend class LogManager;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LogScopeMap() = default;
    ~LogScopeMap() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( LogScopeMap );
};

/**
 * \brief   Container and controller of all registered log scopes in the application. Manages scope
 *          registration, activation, and priority changes including individual scopes and scope
 *          groups.
 **/
class ScopeController
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ScopeController() = default;
    ~ScopeController() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns a read-only list of registered scopes.
     **/
    inline const LogScopeMap & scope_list() const;

    /**
     * \brief   Returns the log scope object with the specified ID, or nullptr if not found.
     *
     * \param   scopeId     The log scope ID to lookup.
     * \return  Valid pointer to the log scope if found; nullptr otherwise.
     **/
    inline const LogScope * scope( uint32_t scopeId ) const;

    /**
     * \brief   Returns the log scope object with the specified name, or nullptr if not found.
     *
     * \param   scopeName       The unique log scope name to lookup.
     * \return  Valid pointer to the log scope if found; nullptr otherwise.
     **/
    inline const LogScope * scope( const char * scopeName ) const;

    /**
     * \brief   Returns true if a log scope with the specified ID is registered.
     *
     * \param   scopeId     The log scope ID to lookup.
     * \return  True if a scope with the specified ID is registered; false otherwise.
     **/
    bool is_scope_registered( uint32_t scopeId ) const;

    /**
     * \brief   Returns true if a log scope with the specified name is registered.
     *
     * \param   scopeName       The log scope name to lookup.
     * \return  True if a scope with the specified name is registered; false otherwise.
     **/
    bool is_scope_registered( const char * scopeName ) const;

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
    void set_scope_priority( uint32_t scopeId, uint32_t newPrio );

    /**
     * \brief   Sets the logging priority for a scope by ID using a priority name.
     *
     * \param   scopeId     The ID of the log scope.
     * \param   newPrio     The name of the priority to set.
     **/
    inline void set_scope_priority( uint32_t scopeId, const String & newPrio );

    /**
     * \brief   Sets the logging priority for a scope by name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   newPrio         The priority value to set.
     **/
    inline void set_scope_priority( const String & scopeName, uint32_t newPrio );

    /**
     * \brief   Sets the logging priority for a scope by name using a priority name.
     *
     * \param   scopeName       The name of the log scope.
     * \param   newPrio         The name of the priority to set.
     **/
    inline void set_scope_priority( const String & scopeName, const String & newPrio );

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
    inline int32_t set_group_priority( const String & scopeGroupName, const String & newPrio );

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
    static inline bool _is_scope_group( const String & scopeName );

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

inline const LogScopeMap & ScopeController::scope_list() const
{
    return mMapLogScope;
}

inline const LogScope * ScopeController::scope( uint32_t scopeId ) const
{
    return mMapLogScope.find_resource_object( scopeId );
}

inline const LogScope * ScopeController::scope( const char * scopeName ) const
{
    return scope( areg::make_scope_id(scopeName) );
}

inline bool ScopeController::is_scope_registered( uint32_t scopeId ) const
{
    return (scope(scopeId) != nullptr);
}

inline bool ScopeController::is_scope_registered( const char * scopeName ) const
{
    return (scope( scopeName ) != nullptr);
}

inline void ScopeController::set_scope_priority( uint32_t scopeId, const String & newPrio )
{
    set_scope_priority( scopeId, static_cast<uint32_t>(areg::string_to_priority( newPrio )) );
}

inline void ScopeController::set_scope_priority( const String & scopeName, uint32_t newPrio )
{
    set_scope_priority( areg::make_scope_id( scopeName ), newPrio );
}

inline void ScopeController::set_scope_priority( const String & scopeName, const String & newPrio )
{
    set_scope_priority( areg::make_scope_id( scopeName ), static_cast<uint32_t>(areg::string_to_priority( newPrio )) );
}

inline void ScopeController::add_scope_priority( uint32_t scopeId, const String & addPrio )
{
    add_scope_priority( scopeId, areg::string_to_priority( addPrio ) );
}

inline void ScopeController::add_scope_priority( const String & scopeName, areg::LogPriority addPrio )
{
    add_scope_priority( areg::make_scope_id( scopeName ), addPrio );
}

inline void ScopeController::add_scope_priority( const String & scopeName, const String & addPrio )
{
    add_scope_priority( areg::make_scope_id( scopeName ), areg::string_to_priority( addPrio ) );
}

inline void ScopeController::remove_scope_priority( uint32_t scopeId, const String & remPrio )
{
    remove_scope_priority( scopeId, areg::string_to_priority( remPrio ) );
}

inline void ScopeController::remove_scope_priority( const String & scopeName, areg::LogPriority remPrio )
{
    remove_scope_priority( areg::make_scope_id( scopeName ), remPrio );
}

inline void ScopeController::remove_scope_priority( const String & scopeName, const String & remPrio )
{
    remove_scope_priority( areg::make_scope_id( scopeName ), areg::string_to_priority( remPrio ) );
}

inline int32_t ScopeController::set_group_priority( const String & scopeGroupName, const String & newPrio )
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

} // namespace areg

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_ScopeController_HPP
