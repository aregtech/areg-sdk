#ifndef AREG_LOGGING_PRIVATE_SCOPECONTROLLER_HPP
#define AREG_LOGGING_PRIVATE_SCOPECONTROLLER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeController.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Scope Controller.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEResourceMap.hpp"

#include "areg/logging/NELogging.hpp"

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
using MapLogScope   = NELogging::ScopeList;
//!< Scope resource map helper
using ImplLogScope  = TEResourceMapImpl<unsigned int, LogScope *>;
//!< The log scope key-value pair.
using LogScopePair  = std::pair<unsigned int, LogScope *>;
//!< The map of scopes to configure
using ListScopes    = StringToIntegerHashMap;
    
/**
 * \brief   Resource map, container of all logging scopes
 **/
class LogScopeMap   : public TELockResourceMap<unsigned int, LogScope *, MapLogScope, ImplLogScope>
{
    friend class NetTcpLogger;
    friend class ScopeController;
    friend class LogManager;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LogScopeMap( void ) = default;
    ~LogScopeMap( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LogScopeMap );
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
    ScopeController( void ) = default;
    ~ScopeController( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the list of scopes registered in the application.
     **/
    inline const LogScopeMap & getScopeList( void ) const;

    /**
     * \brief   By given scope ID it returns instance of Log Scope object.
     *          Returns null if no scope found.
     * \param   scopeId     The log scope ID to lookup in the system.
     * \return  If found, returns valid pointer of registered Log Scope object;
     *          otherwise, it returns null.
     **/
    inline const LogScope * getScope( unsigned int scopeId ) const;

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
    bool isScopeRegistered( unsigned int scopeId ) const;

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
    void setScopePriority( unsigned int scopeId, unsigned int newPrio );

    /**
     * \brief   By given ID searches log scope and sets the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   newPrio     The name of priority to set.
     **/
    inline void setScopePriority( unsigned int scopeId, const String & newPrio );

    /**
     * \brief   By given name searches log scope and sets the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   newPrio     The priority value to set.
     **/
    inline void setScopePriority( const String & scopeName, unsigned int newPrio );

    /**
     * \brief   By given name searches log scope and sets the specified priority.
     * \param   scopeName   The name of log scope to search.
     * \param   newPrio     The name of priority value to set.
     **/
    inline void setScopePriority( const String & scopeName, const String & newPrio );

    /**
     * \brief   By given ID searches log scope map adds the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   addPrio     The priority value to add.
     **/
    void addScopePriority( unsigned int scopeId, NELogging::eLogPriority addPrio );

    /**
     * \brief   By given ID searches log scope and adds the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   addPrio     The name of priority value to add.
     **/
    inline void addScopePriority( unsigned int scopeId, const String & addPrio );

    /**
     * \brief   By given name searches log scope and adds the specified scope priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   addPrio     The priority value to add.
     **/
    inline void addScopePriority( const String & scopeName, NELogging::eLogPriority addPrio );

    /**
     * \brief   By given name searches log scope and adds the specified scope priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   addPrio     The name of priority to add.
     **/
    inline void addScopePriority( const String & scopeName, const String & addPrio );

    /**
     * \brief   By given ID searches log scope and removes the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   remPrio     The priority value to remove.
     **/
    void removeScopePriority( unsigned int scopeId, NELogging::eLogPriority remPrio );

    /**
     * \brief   By given ID searches log scope and removes the specified priority.
     * \param   scopeId     The ID of the log scope to search.
     * \param   remPrio     The name of priority value to remove.
     **/
    inline void removeScopePriority( unsigned int scopeId, const String & remPrio );

    /**
     * \brief   By given name searches log scope and removes the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   remPrio     The priority value to remove.
     **/
    inline void removeScopePriority( const String & scopeName, NELogging::eLogPriority remPrio );

    /**
     * \brief   By given name searches log scope and removes the specified priority.
     * \param   scopeName   The name of the log scope to search.
     * \param   remPrio     The name of priority to remove.
     **/
    inline void removeScopePriority( const String & scopeName, const String & remPrio );

    /**
     * \brief   By given name of scope group searches log scopes and sets 
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   newPrio         The priority value to set to each log scope.
     * \return  Returns number of log scope, which priority has been set.
     *          Returns zero, if no log scope found.
     **/
    int setScopeGroupPriority( const String & scopeGroupName, unsigned int newPrio );

     /**
      * \brief   By given name of scope group searches log scopes and sets
      *          the log message priority to each scope.
      * \param   scopeGroupName  The name of log scope group to search.
      * \param   newPrio         The name of priority to set to each log scope.
      * \return  Returns number of log scope, which priority has been set.
      *          Returns zero, if no log scope found.
      **/
    inline int setScopeGroupPriority( const String & scopeGroupName, const String & newPrio );

    /**
     * \brief   By given name of scope group searches log scopes and adds
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   addPrio         The priority value to add to each log scope.
     * \return  Returns number of log scope, which priority has been added.
     *          Returns zero, if no log scope found.
     **/
    int addScopeGroupPriority( const String & scopeGroupName, NELogging::eLogPriority addPrio );

    /**
     * \brief   By given name of scope group searches log scopes and adds
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   addPrio         The name of priority to add to each log scope.
     * \return  Returns number of log scope, which priority has been added.
     *          Returns zero, if no log scope found.
     **/
    inline int addScopeGroupPriority( const String & scopeGroupName, const String & addPrio );

    /**
     * \brief   By given name of scope group searches log scopes and removes
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   remPrio         The priority value to remove from each log scope.
     * \return  Returns number of log scope, which priority has been removed.
     *          Returns zero, if no log scope found.
     **/
    int removeScopeGroupPriority( const String & scopeGroupName, NELogging::eLogPriority remPrio );

    /**
     * \brief   By given name of scope group searches log scopes and removes
     *          the log message priority to each scope.
     * \param   scopeGroupName  The name of log scope group to search.
     * \param   remPrio         The priority name to remove from each log scope.
     * \return  Returns number of log scope, which priority has been removed.
     *          Returns zero, if no log scope found.
     **/
    inline int removeScopeGroupPriority( const String & scopeGroupName, const String & remPrio );

    /**
     * \brief   Clears the list of configured scopes.
     *          Call this method if making new configuration.
     **/
    inline void clearConfigScopes( void );

    /**
     * \brief   Call to reset and disable any message priority of scopes.
     **/
    void resetScopes(void);

    /**
     * \brief   Activates default scopes listed in the internally.
     **/
    void activateDefaults( void );

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
    void configureScopes( const String & scopeName, unsigned int scopePrio );

    /**
     * \brief   Picks up the list of configured scopes and log priorities and sets the scope priorities,
     *          so that they are activated or deactivated.
     **/
    void configureScopes(void);

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
    void activateScope( LogScope & logScope, unsigned int defaultPrio );

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
    void changeScopeActivityStatus( const String & scopeName, unsigned int scopeId, unsigned int logPrio );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Checks whether the specified scope name indicates the group of scopes or a single scope name.
     *          The group of scopes ends with symbol '*'.
     **/
    static inline bool _isScopeGroup( const String & scopeName );

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
    DECLARE_NOCOPY_NOMOVE( ScopeController );
};

//////////////////////////////////////////////////////////////////////////
// ScopeController inline methods
//////////////////////////////////////////////////////////////////////////

inline const LogScopeMap & ScopeController::getScopeList( void ) const
{
    return mMapLogScope;
}

inline const LogScope * ScopeController::getScope( unsigned int scopeId ) const
{
    return mMapLogScope.findResourceObject( scopeId );
}

inline const LogScope * ScopeController::getScope( const char * scopeName ) const
{
    return getScope( NELogging::makeScopeId(scopeName) );
}

inline bool ScopeController::isScopeRegistered( unsigned int scopeId ) const
{
    return (getScope(scopeId) != nullptr);
}

inline bool ScopeController::isScopeRegistered( const char * scopeName ) const
{
    return (getScope( scopeName ) != nullptr);
}

inline void ScopeController::setScopePriority( unsigned int scopeId, const String & newPrio )
{
    setScopePriority( scopeId, NELogging::stringToLogPrio( newPrio ) );
}

inline void ScopeController::setScopePriority( const String & scopeName, unsigned int newPrio )
{
    setScopePriority( NELogging::makeScopeId( scopeName ), newPrio );
}

inline void ScopeController::setScopePriority( const String & scopeName, const String & newPrio )
{
    setScopePriority( NELogging::makeScopeId( scopeName ), NELogging::stringToLogPrio( newPrio ) );
}

inline void ScopeController::addScopePriority( unsigned int scopeId, const String & addPrio )
{
    addScopePriority( scopeId, NELogging::stringToLogPrio( addPrio ) );
}

inline void ScopeController::addScopePriority( const String & scopeName, NELogging::eLogPriority addPrio )
{
    addScopePriority( NELogging::makeScopeId( scopeName ), addPrio );
}

inline void ScopeController::addScopePriority( const String & scopeName, const String & addPrio )
{
    addScopePriority( NELogging::makeScopeId( scopeName ), NELogging::stringToLogPrio( addPrio ) );
}

inline void ScopeController::removeScopePriority( unsigned int scopeId, const String & remPrio )
{
    removeScopePriority( scopeId, NELogging::stringToLogPrio( remPrio ) );
}

inline void ScopeController::removeScopePriority( const String & scopeName, NELogging::eLogPriority remPrio )
{
    removeScopePriority( NELogging::makeScopeId( scopeName ), remPrio );
}

inline void ScopeController::removeScopePriority( const String & scopeName, const String & remPrio )
{
    removeScopePriority( NELogging::makeScopeId( scopeName ), NELogging::stringToLogPrio( remPrio ) );
}

inline int ScopeController::setScopeGroupPriority( const String & scopeGroupName, const String & newPrio )
{
    return setScopeGroupPriority( scopeGroupName, NELogging::stringToLogPrio( newPrio ) );
}

inline int ScopeController::addScopeGroupPriority( const String & scopeGroupName, const String & addPrio )
{
    return addScopeGroupPriority( scopeGroupName, NELogging::stringToLogPrio( addPrio ) );
}

inline int ScopeController::removeScopeGroupPriority( const String & scopeGroupName, const String & remPrio )
{
    return removeScopeGroupPriority( scopeGroupName, NELogging::stringToLogPrio( remPrio ) );
}

inline void ScopeController::clearConfigScopes( void )
{
    mConfigScopeList.clear( );
    mConfigScopeGroup.clear( );
}

#endif  // AREG_LOGS
#endif  // AREG_LOGGING_PRIVATE_ScopeController_HPP
