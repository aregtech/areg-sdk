/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/logging/private/ScopeController.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Scope Controller.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/ScopeController.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogOptions.hpp"

#if AREG_LOGGING
namespace areg {

areg::ScopeController::ScopeController()
{
    mMapLogScope.reserve( SCOPE_MAP_INITIAL_RESERVE );
}

inline bool ScopeController::_is_scope_group( const String & scopeName ) noexcept
{
    return (scopeName.find_last(areg::LOG_SCOPES_GROUP) >= areg::START_POS);
}

void ScopeController::register_scope( LogScope & scope )
{
    ASSERT( !mMapLogScope.contains(static_cast<uint32_t>(scope)) );
    mMapLogScope.set_value_at( static_cast<uint32_t>(scope), &scope );
}

void ScopeController::unregister_scope( LogScope & scope )
{
    mMapLogScope.remove_at( static_cast<uint32_t>(scope) );
}

void ScopeController::set_scope_priority( uint32_t scopeId, uint32_t newPrio ) noexcept
{
    LogScope * scope = _find( scopeId );
    if ( scope != nullptr )
    {
        scope->set_priority( newPrio );
    }
}

void ScopeController::add_scope_priority( uint32_t scopeId, areg::LogPriority addPrio )
{
    LogScope * scope = _find( scopeId );
    if ( scope != nullptr )
    {
        scope->add_priority( addPrio );
    }
}

void ScopeController::remove_scope_priority( uint32_t scopeId, areg::LogPriority remPrio )
{
    LogScope * scope = _find( scopeId );
    if ( scope != nullptr )
    {
        scope->remove_priority( remPrio );
    }
}

int32_t ScopeController::set_group_priority( const String & scopeGroupName, uint32_t newPrio )
{
    if (scopeGroupName.is_empty())
        return 0;

    int32_t result{ 0 };
    String scopeGroup{ scopeGroupName };
    if (scopeGroupName.ends_with(areg::LOG_SCOPES_GROUP))
    {
        scopeGroup.resize(scopeGroup.length() - 1);
    }

    if ( scopeGroup.is_empty()
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_SEPARATOR)
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_LEAF_SEPARATOR) )
    {
        for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
        {
            LogScope* scope = mMapLogScope.value_at(pos);
            if (scopeGroup.is_empty() || areg::string_starts_with(scope->name().data(), scopeGroup.as_string(), true))
            {
                scope->set_priority(newPrio);
                ++result;
            }
        }
    }

    return result;
}

int32_t ScopeController::add_group_priority( const String & scopeGroupName, areg::LogPriority addPrio )
{
    if (scopeGroupName.is_empty())
        return 0;

    int32_t result{ 0 };
    String scopeGroup{ scopeGroupName };
    if (scopeGroupName.ends_with(areg::LOG_SCOPES_GROUP))
    {
        scopeGroup.resize(scopeGroup.length() - 1);
    }

    if ( scopeGroup.is_empty()
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_SEPARATOR)
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_LEAF_SEPARATOR) )
    {
        for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
        {
            LogScope* scope = mMapLogScope.value_at(pos);
            if (scopeGroup.is_empty() || areg::string_starts_with(scope->name().data(), scopeGroup.as_string(), true))
            {
                scope->add_priority(addPrio);
                ++result;
            }
        }
    }

    return result;
}

int32_t ScopeController::remove_group_priority( const String & scopeGroupName, areg::LogPriority remPrio )
{
    if (scopeGroupName.is_empty())
        return 0;

    int32_t result{ 0 };
    String scopeGroup{ scopeGroupName };
    if (scopeGroupName.ends_with(areg::LOG_SCOPES_GROUP))
    {
        scopeGroup.resize(scopeGroup.length() - 1);
    }

    if ( scopeGroup.is_empty()
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_SEPARATOR)
         || scopeGroup.ends_with(areg::SYNTAX_SCOPE_LEAF_SEPARATOR) )
    {
        for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
        {
            LogScope* scope = mMapLogScope.value_at(pos);
            if (scopeGroup.is_empty() || areg::string_starts_with(scope->name().data(), scopeGroup.as_string(), true))
            {
                scope->remove_priority(remPrio);
                ++result;
            }
        }
    }

    return result;
}

void ScopeController::reset()
{
    for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
    {
        LogScope* scope = mMapLogScope.value_at(pos);
        ASSERT(scope != nullptr);
        scope->set_priority(static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    }

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();
}

void ScopeController::activate_defaults()
{
    for (const auto& entry : areg::DEFAULT_LOG_ENABLED_SCOPES)
    {
        mConfigScopeGroup.set_value_at(entry.first, entry.second);
    }
}

void ScopeController::force_activate_scopes(bool activate /*= true*/)
{
    mConfigScopeGroup.clear();
    mConfigScopeGroup.set_value_at(areg::SYNTAX_GROUP, activate
            ? static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug)
            : static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

void ScopeController::configure_scopes( const Property & prop )
{
    const PropertyKey & Key = prop.key( );
    const PropertyValue & Value = prop.value( );
    ASSERT( Key.is_valid( ) );
    ASSERT( Key.key_type() == areg::ConfigEntry::LogScope );

    uint32_t prio = Value.identifier( areg::LogScopePriorityIndentifiers );
    configure_scopes(Key.position(), prio);
}

void ScopeController::configure_scopes( const String & scopeName, uint32_t scopePrio )
{
    if ( _is_scope_group( scopeName ) )
    {
        mConfigScopeGroup.set_value_at( scopeName, scopePrio );
    }
    else
    {
        mConfigScopeList.set_value_at( scopeName, scopePrio );
    }
}

void ScopeController::configure_scopes()
{
    std::vector<Property> scopes;
    Application::config_manager().module_log_scopes(scopes);
    for (const Property& prop : scopes)
    {
        configure_scopes(prop);
    }
}

void ScopeController::activate_scope( LogScope & logScope )
{
    uint32_t logPrio{ areg::DEFAULT_LOG_PRIORITY };
    mConfigScopeGroup.find(areg::LOG_SCOPES_GROUP, logPrio );
    activate_scope( logScope, logPrio );
}

void ScopeController::activate_scope( LogScope & logScope, uint32_t defaultPrio )
{
    const String & scopeName = logScope.name( );
    uint32_t scopePrio{ defaultPrio };

    if ( mConfigScopeList.find( scopeName, scopePrio ) )
    {
        logScope.set_priority( scopePrio );     // exact match
        return;
    }

    logScope.set_priority( defaultPrio );

    String groupName( scopeName );
    areg::CharPos pos = areg::END_POS;

    // For new-format scope names ("path.method"), check the method-level group "path.*" first,
    // then fall through to path-node traversal using '_'.
    areg::CharPos dotPos = scopeName.find_last( areg::SYNTAX_SCOPE_LEAF_SEPARATOR );
    if ( areg::is_position_valid( dotPos ) )
    {
        // Build "path.*" and check it.
        groupName.set_at( areg::LOG_SCOPES_GROUP[0], dotPos + 1 ).resize( dotPos + 2 );
        if ( mConfigScopeGroup.find( groupName, scopePrio ) )
        {
            logScope.set_priority( scopePrio );
            return;
        }

        // Reduce groupName to the path portion only (before the dot) for '_' traversal.
        groupName.substring( 0, dotPos );
    }

    // Traverse path nodes from right to left using '_' separator.
    do
    {
        pos = groupName.find_last( areg::SYNTAX_SCOPE_SEPARATOR, pos, true );
        if ( groupName.is_valid_position( pos ) )
        {
            groupName.set_at( areg::LOG_SCOPES_GROUP[0], pos + 1 ).resize( pos + 2 );
            pos -= 1;
        }
        else
        {
            pos = areg::INVALID_POS;
            groupName = areg::LOG_SCOPES_GROUP;
        }

        if ( mConfigScopeGroup.find( groupName, scopePrio ) )
        {
            logScope.set_priority( scopePrio );
            break;
        }

    } while ( pos != areg::INVALID_POS );
}

void ScopeController::set_scope_activity( bool makeActive )
{
    if ( makeActive )
    {
        uint32_t defaultPrio = areg::DEFAULT_LOG_PRIORITY;
        mConfigScopeGroup.find( areg::LOG_SCOPES_GROUP, defaultPrio );

        for ( auto pos = mMapLogScope.first_position( ); mMapLogScope.is_valid_position( pos ); pos = mMapLogScope.next_position( pos ) )
        {
            activate_scope( *mMapLogScope.value_at( pos ), defaultPrio );
        }
    }
    else
    {
        for ( auto pos = mMapLogScope.first_position( ); mMapLogScope.is_valid_position( pos ); pos = mMapLogScope.next_position( pos ) )
        {
            mMapLogScope.value_at( pos )->set_priority( static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
        }
    }
}

void ScopeController::set_scope_activity( const String & scopeName, uint32_t scopeId, uint32_t logPrio )
{
    if ( _is_scope_group( scopeName ) )
    {
        set_group_priority( scopeName, logPrio );
        mConfigScopeGroup.set_value_at( scopeName, logPrio );
    }
    else
    {
        if (scopeId != areg::LOG_SCOPE_ID_NONE)
        {
            set_scope_priority(scopeId, logPrio);
        }
        else
        {
            set_scope_priority(scopeName, logPrio);
        }

        mConfigScopeList.set_value_at( scopeName, logPrio );
    }
}

} // namespace areg

#endif  // AREG_LOGGING
