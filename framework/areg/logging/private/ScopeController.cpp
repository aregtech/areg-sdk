/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/persist/Property.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogOptions.hpp"

#if AREG_LOGGING
namespace areg {

inline bool ScopeController::_is_scope_group( const String & scopeName )
{
    return (scopeName.find_last(areg::SYNTAX_SCOPE_GROUP ) >= areg::START_POS);
}

void ScopeController::register_scope( LogScope & scope )
{
    ASSERT( mMapLogScope.find_resource_object( static_cast<uint32_t>(scope) ) == nullptr );
    mMapLogScope.register_resource_object( static_cast<uint32_t>(scope), &scope );
}

void ScopeController::unregister_scope( LogScope & scope )
{
    mMapLogScope.unregister_resource_object( static_cast<uint32_t>(scope) );
}

void ScopeController::set_scope_priority( uint32_t scopeId, uint32_t newPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.find_resource_object( scopeId );
    if ( scope != nullptr )
    {
        scope->set_priority( newPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::add_scope_priority( uint32_t scopeId, areg::LogPriority addPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.find_resource_object( scopeId );
    if ( scope != nullptr )
    {
        scope->add_priority( addPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::remove_scope_priority( uint32_t scopeId, areg::LogPriority remPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.find_resource_object( scopeId );
    if ( scope != nullptr )
    {
        scope->remove_priority( remPrio );
    }

    mMapLogScope.unlock( );
}

int32_t ScopeController::set_group_priority( const String & scopeGroupName, uint32_t newPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.is_empty( ) == false )
    {
        mMapLogScope.lock( );

        String scopeGroup{ scopeGroupName };
        if (scopeGroupName.ends_with(areg::SYNTAX_SCOPE_GROUP))
        {
            scopeGroup.resize(scopeGroup.length() - 1);
        }

        if (scopeGroup.is_empty() || scopeGroup.ends_with(areg::SYNTAX_SCOPE_SEPARATOR))
        {
            for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
            {
                LogScope* scope = mMapLogScope.value_at(pos);
                if (scopeGroup.is_empty() || scope->scope_name().starts_with(scopeGroup, true))
                {
                    scope->set_priority(newPrio);
                    ++result;
                }
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

int32_t ScopeController::add_group_priority( const String & scopeGroupName, areg::LogPriority addPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.is_empty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.first_position( ); mMapLogScope.is_valid_position( pos ); pos = mMapLogScope.next_position( pos ) )
        {
            LogScope * scope = mMapLogScope.value_at( pos );
            if ( scopeGroupName.compare( scope->scope_name( ), true ) == areg::Ordering::Equal )
            {
                scope->add_priority( addPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

int32_t ScopeController::remove_group_priority( const String & scopeGroupName, areg::LogPriority remPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.is_empty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.first_position( ); mMapLogScope.is_valid_position( pos ); pos = mMapLogScope.next_position( pos ) )
        {
            LogScope * scope = mMapLogScope.value_at( pos );
            if ( scopeGroupName.compare( scope->scope_name( ), true ) == areg::Ordering::Equal )
            {
                scope->remove_priority( remPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

void ScopeController::reset()
{
    mMapLogScope.lock();

    for (auto pos = mMapLogScope.first_position(); mMapLogScope.is_valid_position(pos); pos = mMapLogScope.next_position(pos))
    {
        LogScope * scope = mMapLogScope.value_at(pos);
        ASSERT(scope != nullptr);
        scope->set_priority(static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    }

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();

    mMapLogScope.unlock();
}

void ScopeController::activate_defaults()
{
    for (const auto& entry : areg::DEFAULT_LOG_ENABLED_SCOPES)
    {
        mConfigScopeGroup.set_value_at(entry.first, entry.second);
    }
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
    mConfigScopeGroup.find(areg::LOG_SCOPES_GRPOUP, logPrio );
    activate_scope( logScope, logPrio );
}

void ScopeController::activate_scope( LogScope & logScope, uint32_t defaultPrio )
{
    const String & scopeName = logScope.scope_name( );
    uint32_t scopePrio{ defaultPrio };

    if ( mConfigScopeList.find( scopeName, scopePrio ) )
    {
        logScope.set_priority( scopePrio ); // exact match. Set priority
    }
    else
    {
        logScope.set_priority( defaultPrio ); // set first default priority
        String groupName( scopeName );
        areg::CharPos pos = areg::END_POS;
        do
        {
            pos = groupName.find_last( areg::SYNTAX_SCOPE_SEPARATOR, pos, true );
            if ( groupName.is_valid_position( pos ) )
            {
                // set group syntax
                groupName.set_at( areg::SYNTAX_SCOPE_GROUP, pos + 1 ).resize( pos + 2 );
                pos -= 1;
            }
            else
            {
                pos = areg::INVALID_POS;
                groupName = areg::SYNTAX_SCOPE_GROUP;
            }

            if ( mConfigScopeGroup.find( groupName, scopePrio ) )
            {
                // Found group priority, set it and exit the loop
                logScope.set_priority( scopePrio );
                break;
            }

        } while ( pos != areg::INVALID_POS );
    }
}

void ScopeController::set_scope_activity( bool makeActive )
{
    mMapLogScope.lock( );

    if ( makeActive )
    {
        uint32_t defaultPrio = areg::DEFAULT_LOG_PRIORITY;
        mConfigScopeGroup.find( areg::LOG_SCOPES_GRPOUP, defaultPrio );

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

    mMapLogScope.unlock( );
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
