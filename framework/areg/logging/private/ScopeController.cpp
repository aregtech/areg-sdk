/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeController.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Scope Controller.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/ScopeController.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/persist/Property.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/NELogOptions.hpp"

#if AREG_LOGS
inline bool ScopeController::_isScopeGroup( const String & scopeName )
{
    return (scopeName.findLast(NELogOptions::SYNTAX_SCOPE_GROUP ) >= NEString::START_POS);
}

void ScopeController::registerScope( LogScope & scope )
{
    ASSERT( mMapLogScope.findResourceObject( static_cast<unsigned int>(scope) ) == nullptr );
    mMapLogScope.registerResourceObject( static_cast<unsigned int>(scope), &scope );
}

void ScopeController::unregisterScope( LogScope & scope )
{
    mMapLogScope.unregisterResourceObject( static_cast<unsigned int>(scope) );
}

void ScopeController::setScopePriority( unsigned int scopeId, unsigned int newPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->setPriority( newPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::addScopePriority( unsigned int scopeId, NELogging::eLogPriority addPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->addPriority( addPrio );
    }

    mMapLogScope.unlock( );
}

void ScopeController::removeScopePriority( unsigned int scopeId, NELogging::eLogPriority remPrio )
{
    mMapLogScope.lock( );

    LogScope * scope = mMapLogScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->removePriority( remPrio );
    }

    mMapLogScope.unlock( );
}

int ScopeController::setScopeGroupPriority( const String & scopeGroupName, unsigned int newPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        String scopeGroup{ scopeGroupName };
        if (scopeGroupName.endsWith(NELogOptions::SYNTAX_SCOPE_GROUP))
        {
            scopeGroup.resize(scopeGroup.getLength() - 1);
        }

        if (scopeGroup.isEmpty() || scopeGroup.endsWith(NELogOptions::SYNTAX_SCOPE_SEPARATOR))
        {
            for (auto pos = mMapLogScope.firstPosition(); mMapLogScope.isValidPosition(pos); pos = mMapLogScope.nextPosition(pos))
            {
                LogScope* scope = mMapLogScope.valueAtPosition(pos);
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

int ScopeController::addScopeGroupPriority( const String & scopeGroupName, NELogging::eLogPriority addPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            LogScope * scope = mMapLogScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == NEMath::eCompare::Equal )
            {
                scope->addPriority( addPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

int ScopeController::removeScopeGroupPriority( const String & scopeGroupName, NELogging::eLogPriority remPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapLogScope.lock( );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            LogScope * scope = mMapLogScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == NEMath::eCompare::Equal )
            {
                scope->removePriority( remPrio );
                ++ result;
            }
        }

        mMapLogScope.unlock( );
    }

    return result;
}

void ScopeController::resetScopes(void)
{
    mMapLogScope.lock();

    for (auto pos = mMapLogScope.firstPosition(); mMapLogScope.isValidPosition(pos); pos = mMapLogScope.nextPosition(pos))
    {
        LogScope * scope = mMapLogScope.valueAtPosition(pos);
        ASSERT(scope != nullptr);
        scope->setPriority(static_cast<unsigned int>(NELogging::eLogPriority::PrioNotset));
    }

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();

    mMapLogScope.unlock();
}

void ScopeController::activateDefaults( void )
{
    for (const auto& entry : NELogOptions::DEFAULT_LOG_ENABLED_SCOPES)
    {
        mConfigScopeGroup.setAt(entry.first, entry.second);
    }
}

void ScopeController::configureScopes( const Property & prop )
{
    const PropertyKey & Key = prop.getKey( );
    const PropertyValue & Value = prop.getValue( );
    ASSERT( Key.isValid( ) );
    ASSERT( Key.getKeyType() == NEPersistence::eConfigKeys::EntryLogScope );

    unsigned int prio = Value.getIndetifier( NEApplication::LogScopePriorityIndentifiers );
    configureScopes(Key.getPosition(), prio);
}

void ScopeController::configureScopes( const String & scopeName, unsigned int scopePrio )
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

void ScopeController::configureScopes(void)
{
    std::vector<Property> scopes;
    Application::getConfigManager().getModuleLogScopes(scopes);
    for (const Property& prop : scopes)
    {
        configureScopes(prop);
    }
}

void ScopeController::activateScope( LogScope & logScope )
{
    unsigned int logPrio{ NELogOptions::DEFAULT_LOG_PRIORITY };
    mConfigScopeGroup.find(NELogOptions::LOG_SCOPES_GRPOUP, logPrio );
    activateScope( logScope, logPrio );
}

void ScopeController::activateScope( LogScope & logScope, unsigned int defaultPrio )
{
    const String & scopeName = logScope.getScopeName( );
    unsigned int scopePrio{ defaultPrio };

    if ( mConfigScopeList.find( scopeName, scopePrio ) )
    {
        logScope.setPriority( scopePrio ); // exact match. Set priority
    }
    else
    {
        logScope.setPriority( defaultPrio ); // set first default priority
        String groupName( scopeName );
        NEString::CharPos pos = NEString::END_POS;
        do
        {
            pos = groupName.findLast( NELogOptions::SYNTAX_SCOPE_SEPARATOR, pos, true );
            if ( groupName.isValidPosition( pos ) )
            {
                // set group syntax
                groupName.setAt( NELogOptions::SYNTAX_SCOPE_GROUP, pos + 1 ).resize( pos + 2 );
                pos -= 1;
            }
            else
            {
                pos = NEString::INVALID_POS;
                groupName = NELogOptions::SYNTAX_SCOPE_GROUP;
            }

            if ( mConfigScopeGroup.find( groupName, scopePrio ) )
            {
                // Found group priority, set it and exit the loop
                logScope.setPriority( scopePrio );
                break;
            }

        } while ( pos != NEString::INVALID_POS );
    }
}

void ScopeController::changeScopeActivityStatus( bool makeActive )
{
    mMapLogScope.lock( );

    if ( makeActive )
    {
        unsigned int defaultPrio = NELogOptions::DEFAULT_LOG_PRIORITY;
        mConfigScopeGroup.find( NELogOptions::LOG_SCOPES_GRPOUP, defaultPrio );

        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            activateScope( *mMapLogScope.valueAtPosition( pos ), defaultPrio );
        }
    }
    else
    {
        for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
        {
            mMapLogScope.valueAtPosition( pos )->setPriority( NELogging::eLogPriority::PrioNotset );
        }
    }

    mMapLogScope.unlock( );
}

void ScopeController::changeScopeActivityStatus( const String & scopeName, unsigned int scopeId, unsigned int logPrio )
{
    if ( _isScopeGroup( scopeName ) )
    {
        setScopeGroupPriority( scopeName, logPrio );
        mConfigScopeGroup.setAt( scopeName, logPrio );
    }
    else
    {
        if (scopeId != NELogging::LOG_SCOPE_ID_NONE)
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
