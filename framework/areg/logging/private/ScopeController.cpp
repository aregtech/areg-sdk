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

#if AREG_LOGS
inline bool ScopeController::_isScopeGroup( const areg::String & scopeName )
{
    return (scopeName.findLast(areg::SYNTAX_SCOPE_GROUP ) >= areg::START_POS);
}

void ScopeController::registerScope( areg::LogScope & scope )
{
    ASSERT( mMapLogScope.findResourceObject( static_cast<uint32_t>(scope) ) == nullptr );
    mMapLogScope.registerResourceObject( static_cast<uint32_t>(scope), &scope );
}

void ScopeController::unregisterScope( areg::LogScope & scope )
{
    mMapLogScope.unregisterResourceObject( static_cast<uint32_t>(scope) );
}

void ScopeController::setScopePriority( uint32_t scopeId, uint32_t newPrio )
{
    mMapLogScope.lock( );

    areg::LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->setPriority( newPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::addScopePriority( uint32_t scopeId, areg::LogPriority addPrio )
{
    mMapLogScope.lock( );

    areg::LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->addPriority( addPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::removeScopePriority( uint32_t scopeId, areg::LogPriority remPrio )
{
    mMapLogScope.lock( );

    areg::LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->removePriority( remPrio );
    }

    mMapLogScope.unlock( );
}

int32_t ScopeController::setScopeGroupPriority( const areg::String & scopeGroupName, uint32_t newPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        areg::String scopeGroup{ scopeGroupName };
        if (scopeGroupName.endsWith(areg::SYNTAX_SCOPE_GROUP))
        {
            scopeGroup.resize(scopeGroup.getLength() - 1);
        }

        if (scopeGroup.isEmpty() || scopeGroup.endsWith(areg::SYNTAX_SCOPE_SEPARATOR))
        {
            for (auto pos = mMapLogScope.firstPosition(); mMapLogScope.isValidPosition(pos); pos = mMapLogScope.nextPosition(pos))
            {
                areg::LogScope* scope = mMapLogScope.valueAtPosition(pos);
                if (scopeGroup.isEmpty() || scope->getScopeName().startsWith(scopeGroup, true))
                {
                    scope->setPriority(newPrio);
                    ++result;
                }
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

int32_t ScopeController::addScopeGroupPriority( const areg::String & scopeGroupName, areg::LogPriority addPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            areg::LogScope * scope = mMapLogScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == areg::Ordering::Equal )
            {
                scope->addPriority( addPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

int32_t ScopeController::removeScopeGroupPriority( const areg::String & scopeGroupName, areg::LogPriority remPrio )
{
    int32_t result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            areg::LogScope * scope = mMapLogScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == areg::Ordering::Equal )
            {
                scope->removePriority( remPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

void ScopeController::resetScopes()
{
    mMapLogScope.lock();

    for (auto pos = mMapLogScope.firstPosition(); mMapLogScope.isValidPosition(pos); pos = mMapLogScope.nextPosition(pos))
    {
        areg::LogScope * scope = mMapLogScope.valueAtPosition(pos);
        ASSERT(scope != nullptr);
        scope->setPriority(static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    }

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();

    mMapLogScope.unlock();
}

void ScopeController::activateDefaults()
{
    for (const auto& entry : areg::DEFAULT_LOG_ENABLED_SCOPES)
    {
        mConfigScopeGroup.setAt(entry.first, entry.second);
    }
}

void ScopeController::configureScopes( const areg::Property & prop )
{
    const areg::PropertyKey & Key = prop.getKey( );
    const areg::PropertyValue & Value = prop.getValue( );
    ASSERT( Key.isValid( ) );
    ASSERT( Key.getKeyType() == areg::ConfigEntry::LogScope );

    uint32_t prio = Value.getIndetifier( areg::LogScopePriorityIndentifiers );
    configureScopes(Key.getPosition(), prio);
}

void ScopeController::configureScopes( const areg::String & scopeName, uint32_t scopePrio )
{
    if ( _isScopeGroup( scopeName ) )
    {
        mConfigScopeGroup.setAt( scopeName, scopePrio );
    }
    else
    {
        mConfigScopeList.setAt( scopeName, scopePrio );
    }
}

void ScopeController::configureScopes()
{
    std::vector<areg::Property> scopes;
    areg::Application::getConfigManager().getModuleLogScopes(scopes);
    for (const areg::Property& prop : scopes)
    {
        configureScopes(prop);
    }
}

void ScopeController::activateScope( areg::LogScope & logScope )
{
    uint32_t logPrio{ areg::DEFAULT_LOG_PRIORITY };
    mConfigScopeGroup.find(areg::LOG_SCOPES_GRPOUP, logPrio );
    activateScope( logScope, logPrio );
}

void ScopeController::activateScope( areg::LogScope & logScope, uint32_t defaultPrio )
{
    const areg::String & scopeName = logScope.getScopeName( );
    uint32_t scopePrio{ defaultPrio };

    if ( mConfigScopeList.find( scopeName, scopePrio ) )
    {
        logScope.setPriority( scopePrio ); // exact match. Set priority
    }
    else
    {
        logScope.setPriority( defaultPrio ); // set first default priority
        areg::String groupName( scopeName );
        areg::CharPos pos = areg::END_POS;
        do
        {
            pos = groupName.findLast( areg::SYNTAX_SCOPE_SEPARATOR, pos, true );
            if ( groupName.isValidPosition( pos ) )
            {
                // set group syntax
                groupName.setAt( areg::SYNTAX_SCOPE_GROUP, pos + 1 ).resize( pos + 2 );
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
                logScope.setPriority( scopePrio );
                break;
            }

        } while ( pos != areg::INVALID_POS );
    }
}

void ScopeController::changeScopeActivityStatus( bool makeActive )
{
    mMapLogScope.lock( );

    if ( makeActive )
    {
        uint32_t defaultPrio = areg::DEFAULT_LOG_PRIORITY;
        mConfigScopeGroup.find( areg::LOG_SCOPES_GRPOUP, defaultPrio );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            activateScope( *mMapLogScope.valueAtPosition( pos ), defaultPrio );
        }
    }
    else
    {
        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            mMapLogScope.valueAtPosition( pos )->setPriority( static_cast<uint32_t>(areg::LogPriority::PrioNotset) );
        }
    }

    mMapLogScope.unlock( );
}

void ScopeController::changeScopeActivityStatus( const areg::String & scopeName, uint32_t scopeId, uint32_t logPrio )
{
    if ( _isScopeGroup( scopeName ) )
    {
        setScopeGroupPriority( scopeName, logPrio );
        mConfigScopeGroup.setAt( scopeName, logPrio );
    }
    else
    {
        if (scopeId != areg::LOG_SCOPE_ID_NONE)
        {
            setScopePriority(scopeId, logPrio);
        }
        else
        {
            setScopePriority(scopeName, logPrio);
        }

        mConfigScopeList.setAt( scopeName, logPrio );
    }
}

#endif  // AREG_LOGS
