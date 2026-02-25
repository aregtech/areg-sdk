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
#include "areg/base/GEGlobal.h"
#include "areg/base/ResourceMap.hpp"

#include "areg/logging/LoggingDefs.hpp"

#include <string_view>

#if AREG_LOGS
/************************************************************************
 * Dependencies
 ************************************************************************/
class LogScope;
class Property;

//////////////////////////////////////////////////////////////////////////
// ScopeController::LogScopeMap class declaration
//////////////////////////////////////////////////////////////////////////
//!< Scope hash map
using MapLogScope   = areg::ScopeList;
//!< Scope resource map helper
using ImplLogScope  = areg::ResourceMapImpl<uint32_t, LogScope *>;
//!< The log scope key-value pair.
using LogScopePair  = std::pair<uint32_t, LogScope *>;
//!< The map of scopes to configure
using ListScopes    = StringToIntegerHashMap;
    
/**
 * \brief   Resource map, container of all logging scopes
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
 * \brief   The controller of scopes in the application.
 *          It contains all scopes in the application.
 *          It contains the list of initialized scopes in the configuration file.
 *          It activates and deactivates scopes, and changes the scope priority.
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
     * \brief   Returns the list of scopes registered in the application.
     **/
    inline const LogScopeMap & getScopeList() const;

    /**
     * \brief   By given scope ID it returns instance of Log Scope object.
     *          Returns null if no scope found.
     * \param   scopeId     The log scope ID to lookup in the system.
     * \return  If found, returns valid pointer of registered Log Scope object;
     *          otherwise, it returns null.
     **/
    inline const LogScope * getScope( uint32_t scopeId ) const;

    /**
     * \brief   By given unique scope name it returns instance of registered Log Scope object.
     *          Returns null if there is no log scope object with specified name registered
     *          in the system.
     * \param   scopeName   The unique log scope name to lookup in the system.
     * \return  If found, returns valid pointer of registered Log Scope object;
     *          otherwise, it returns null.
     **/
    inline const LogScope * getScope( const char * scopeName ) const;

    /**
     * \brief   Checks if there is a log scope object registered with the given ID.
     * \param   scopeId     The log scope ID to lookup.
     * \return  Returns true if there is a log scope with specified ID is registered in the system;
     *          otherwise, it returns false.
     **/
    bool isScopeRegistered( uint32_t scopeId ) const;

    /**
     * \brief   Checks whether there is a log scope object registered with the given name;
     * \param   scopeName   The log scope name to lookup.
     * \return  Returns true if there is a log scope with specified name is registered in the system;
     *          otherwise, it returns false.
     **/
    bool isScopeRegistered( const char * scopeName ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers instance of log scope object in Scope Controller.
     * \param   scope   The instance log scope object to register.
     **/
    void registerScope( LogScope & scope );

    /**
     * \brief   Unregisters instance of log scope object in Scope Controller.
     * \param   scope   The instance log scope object to unregister.
     **/
    void unregisterScope( LogScope & scope );

    /**
     * \brief   Searches the log scope object by given ID and sets specified priority.
     * \param   scopeId     The ID of log scope to search in the system.
     * \param   newPrio     The priority value to set to log scope.
     **/
    void setScopePriority( uint32_t scopeId, uint32_t newPrio );

    /**
     * \brief   By given ID searches log scope and sets the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   newPrio     The name of priority to set.
     **/
    inline void setScopePriority( uint32_t scopeId, const areg::String & newPrio );

    /**
     * \brief   By given name searches log scope and sets the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   newPrio     The priority value to set.
     **/
    inline void setScopePriority( const areg::String & scopeName, uint32_t newPrio );

    /**
     * \brief   By given name searches log scope and sets the specified priority.
     * \param   scopeName   The name of log scope to search.
     * \param   newPrio     The name of priority value to set.
     **/
    inline void setScopePriority( const areg::String & scopeName, const areg::String & newPrio );

    /**
     * \brief   By given ID searches log scope map adds the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   addPrio     The priority value to add.
     **/
    void addScopePriority( uint32_t scopeId, areg::LogPriority addPrio );

    /**
     * \brief   By given ID searches log scope and adds the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   addPrio     The name of priority value to add.
     **/
    inline void addScopePriority( uint32_t scopeId, const areg::String & addPrio );

    /**
     * \brief   By given name searches log scope and adds the specified scope priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   addPrio     The priority value to add.
     **/
    inline void addScopePriority( const areg::String & scopeName, areg::LogPriority addPrio );

    /**
     * \brief   By given name searches log scope and adds the specified scope priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   addPrio     The name of priority to add.
     **/
    inline void addScopePriority( const areg::String & scopeName, const areg::String & addPrio );

    /**
     * \brief   By given ID searches log scope and removes the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   remPrio     The priority value to remove.
     **/
    void removeScopePriority( uint32_t scopeId, areg::LogPriority remPrio );

    /**
     * \brief   By given ID searches log scope and removes the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   remPrio     The name of priority value to remove.
     **/
    inline void removeScopePriority( uint32_t scopeId, const areg::String & remPrio );

    /**
     * \brief   By given name searches log scope and removes the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   remPrio     The priority value to remove.
     **/
    inline void removeScopePriority( const areg::String & scopeName, areg::LogPriority remPrio );

    /**
     * \brief   By given name searches log scope and removes the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   remPrio     The name of priority to remove.
     **/
    inline void removeScopePriority( const areg::String & scopeName, const areg::String & remPrio );

    /**
     * \brief   By given name of scope group searches log scopes and sets 
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   newPrio         The priority value to set to each log scope.
     * \return  Returns number of log scope, which priority has been set.
     *          Returns zero, if no log scope found.
     **/
    int32_t setScopeGroupPriority( const areg::String & scopeGroupName, uint32_t newPrio );

     /**
      * \brief   By given name of scope group searches log scopes and sets
      *          the log message priority to each scope.
      * \param   scopeGroupName  The name of log scope group to search.
      * \param   newPrio         The name of priority to set to each log scope.
      * \return  Returns number of log scope, which priority has been set.
      *          Returns zero, if no log scope found.
      **/
    inline int32_t setScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & newPrio );

    /**
     * \brief   By given name of scope group searches log scopes and adds
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   addPrio         The priority value to add to each log scope.
     * \return  Returns number of log scope, which priority has been added.
     *          Returns zero, if no log scope found.
     **/
    int32_t addScopeGroupPriority( const areg::String & scopeGroupName, areg::LogPriority addPrio );

    /**
     * \brief   By given name of scope group searches log scopes and adds
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   addPrio         The name of priority to add to each log scope.
     * \return  Returns number of log scope, which priority has been added.
     *          Returns zero, if no log scope found.
     **/
    inline int32_t addScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & addPrio );

    /**
     * \brief   By given name of scope group searches log scopes and removes
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   remPrio         The priority value to remove from each log scope.
     * \return  Returns number of log scope, which priority has been removed.
     *          Returns zero, if no log scope found.
     **/
    int32_t removeScopeGroupPriority( const areg::String & scopeGroupName, areg::LogPriority remPrio );

    /**
     * \brief   By given name of scope group searches log scopes and removes
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   remPrio         The priority name to remove from each log scope.
     * \return  Returns number of log scope, which priority has been removed.
     *          Returns zero, if no log scope found.
     **/
    inline int32_t removeScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & remPrio );

    /**
     * \brief   Clears the list of configured scopes.
     *          Call this method if making new configuration.
     **/
    inline void clearConfigScopes();

    /**
     * \brief   Call to reset and disable any message priority of scopes.
     **/
    void resetScopes();

    /**
     * \brief   Activates default scopes listed in the internally.
     **/
    void activateDefaults();

    /**
     * \brief   Configures the scope or scope group defined in the property.
     * \param   prop    The property, which contains the scope name to search
     *                  and priority information to set.
     **/
    void configureScopes( const Property & prop );

    /**
     * \brief   Sets the log message priority to the log scope or scope group.
     * \param   scopeName   The name of the scope or scope group to change.
     * \param   scopePrio   The priority to set to scope or scope group.
     **/
    void configureScopes( const areg::String & scopeName, uint32_t scopePrio );

    /**
     * \brief   Picks up the list of configured scopes and log priorities and sets the scope priorities,
     *          so that they are activated or deactivated.
     **/
    void configureScopes();

    /**
     * \brief   Activates specified scope. The system cannot log if a scope is inactive.
     *          No property of specified scope is changed, the priority remains unchanged.
     * \param   logScope  The scope to activate.
     **/
    void activateScope( LogScope & logScope );

    /**
     * \brief   Activates specified scope and sets specified logging priority. 
     *          The system cannot log if a scope is inactive. The method as well changes logging priority.
     * \param   logScope      The scope to activate and update logging priority.
     * \param   defaultPrio     The default priority to set to the scope.
     **/
    void activateScope( LogScope & logScope, uint32_t defaultPrio );

    /**
     * \brief   Activates or deactivates the scope based on the log scope priority set in the configuration.
     * \param   makeActive  Flag, indicating whether scopes should be activated or deactivated.
     *                      If true, it lookups in the configuration settings and updates the priority.
     *                      Otherwise, it deactivates all scopes, so that no messages are logged.
     **/
    void changeScopeActivityStatus( bool makeActive );

    /**
     * \brief   Changes the logging priority of the scope.
     * \param   scopeName   The name of a scope or scope group.
     * \param   scopeId     The ID of the scope. Can be zero if it is a scope group.
     * \param   logPrio     The priority of the log to set.
     **/
    void changeScopeActivityStatus( const areg::String & scopeName, uint32_t scopeId, uint32_t logPrio );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Checks whether the specified scope name indicates the group of scopes or a single scope name.
     *          The group of scopes ends with symbol '*'.
     **/
    static inline bool _isScopeGroup( const areg::String & scopeName );

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

inline const LogScopeMap & ScopeController::getScopeList() const
{
    return mMapLogScope;
}

inline const LogScope * ScopeController::getScope( uint32_t scopeId ) const
{
    return mMapLogScope.findResourceObject( scopeId );
}

inline const LogScope * ScopeController::getScope( const char * scopeName ) const
{
    return getScope( areg::makeScopeId(scopeName) );
}

inline bool ScopeController::isScopeRegistered( uint32_t scopeId ) const
{
    return (getScope(scopeId) != nullptr);
}

inline bool ScopeController::isScopeRegistered( const char * scopeName ) const
{
    return (getScope( scopeName ) != nullptr);
}

inline void ScopeController::setScopePriority( uint32_t scopeId, const areg::String & newPrio )
{
    setScopePriority( scopeId, static_cast<uint32_t>(areg::stringToLogPrio( newPrio )) );
}

inline void ScopeController::setScopePriority( const areg::String & scopeName, uint32_t newPrio )
{
    setScopePriority( areg::makeScopeId( scopeName ), newPrio );
}

inline void ScopeController::setScopePriority( const areg::String & scopeName, const areg::String & newPrio )
{
    setScopePriority( areg::makeScopeId( scopeName ), static_cast<uint32_t>(areg::stringToLogPrio( newPrio )) );
}

inline void ScopeController::addScopePriority( uint32_t scopeId, const areg::String & addPrio )
{
    addScopePriority( scopeId, areg::stringToLogPrio( addPrio ) );
}

inline void ScopeController::addScopePriority( const areg::String & scopeName, areg::LogPriority addPrio )
{
    addScopePriority( areg::makeScopeId( scopeName ), addPrio );
}

inline void ScopeController::addScopePriority( const areg::String & scopeName, const areg::String & addPrio )
{
    addScopePriority( areg::makeScopeId( scopeName ), areg::stringToLogPrio( addPrio ) );
}

inline void ScopeController::removeScopePriority( uint32_t scopeId, const areg::String & remPrio )
{
    removeScopePriority( scopeId, areg::stringToLogPrio( remPrio ) );
}

inline void ScopeController::removeScopePriority( const areg::String & scopeName, areg::LogPriority remPrio )
{
    removeScopePriority( areg::makeScopeId( scopeName ), remPrio );
}

inline void ScopeController::removeScopePriority( const areg::String & scopeName, const areg::String & remPrio )
{
    removeScopePriority( areg::makeScopeId( scopeName ), areg::stringToLogPrio( remPrio ) );
}

inline int32_t ScopeController::setScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & newPrio )
{
    return setScopeGroupPriority( scopeGroupName, static_cast<uint32_t>(areg::stringToLogPrio( newPrio )) );
}

inline int32_t ScopeController::addScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & addPrio )
{
    return addScopeGroupPriority( scopeGroupName, areg::stringToLogPrio( addPrio ) );
}

inline int32_t ScopeController::removeScopeGroupPriority( const areg::String & scopeGroupName, const areg::String & remPrio )
{
    return removeScopeGroupPriority( scopeGroupName, areg::stringToLogPrio( remPrio ) );
}

inline void ScopeController::clearConfigScopes()
{
    mConfigScopeList.clear( );
    mConfigScopeGroup.clear( );
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_ScopeController_HPP
