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

namespace areg
{

    #if AREG_LOGS
    inline bool ScopeController::_isScopeGroup( const String & scopeName )
    {
        return (scopeName.findLast(SYNTAX_SCOPE_GROUP ) >= START_POS);
    }

    void ScopeController::registerScope( LogScope & scope )
    {
        ASSERT( mMapLogScope.findResourceObject( static_cast<uint32_t>(scope) ) == nullptr );
        mMapLogScope.registerResourceObject( static_cast<uint32_t>(scope), &scope );
    }

    void ScopeController::unregisterScope( LogScope & scope )
    {
        mMapLogScope.unregisterResourceObject( static_cast<uint32_t>(scope) );
    }

    void ScopeController::setScopePriority( uint32_t scopeId, uint32_t newPrio )
    {
        mMapLogScope.lock( );

        LogScope * scope = mMapLogScope.findResourceObject( scopeId );
        if ( scope != nullptr )
        {
            scope->setPriority( newPrio );
        }

        mMapLogScope.unlock( );
    }

    void ScopeController::addScopePriority( uint32_t scopeId, LogPriority addPrio )
    {
        mMapLogScope.lock( );

        LogScope * scope = mMapLogScope.findResourceObject( scopeId );
        if ( scope != nullptr )
        {
            scope->addPriority( addPrio );
        }

        mMapLogScope.unlock( );
    }

    void ScopeController::removeScopePriority( uint32_t scopeId, LogPriority remPrio )
    {
        mMapLogScope.lock( );

        LogScope * scope = mMapLogScope.findResourceObject( scopeId );
        if ( scope != nullptr )
        {
            scope->removePriority( remPrio );
        }

        mMapLogScope.unlock( );
    }

    int32_t ScopeController::setScopeGroupPriority( const String & scopeGroupName, uint32_t newPrio )
    {
        int32_t result{ 0 };
        if ( scopeGroupName.isEmpty( ) == false )
        {
            mMapLogScope.lock( );

            String scopeGroup{ scopeGroupName };
            if (scopeGroupName.endsWith(SYNTAX_SCOPE_GROUP))
            {
                scopeGroup.resize(scopeGroup.getLength() - 1);
            }

            if (scopeGroup.isEmpty() || scopeGroup.endsWith(SYNTAX_SCOPE_SEPARATOR))
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

    int32_t ScopeController::addScopeGroupPriority( const String & scopeGroupName, LogPriority addPrio )
    {
        int32_t result{ 0 };
        if ( scopeGroupName.isEmpty( ) == false )
        {
            mMapLogScope.lock( );

            for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
            {
                LogScope * scope = mMapLogScope.valueAtPosition( pos );
                if ( scopeGroupName.compare( scope->getScopeName( ), true ) == Ordering::Equal )
                {
                    scope->addPriority( addPrio );
                    ++ result;
                }
            }

            mMapLogScope.unlock( );
        }

        return result;
    }

    int32_t ScopeController::removeScopeGroupPriority( const String & scopeGroupName, LogPriority remPrio )
    {
        int32_t result{ 0 };
        if ( scopeGroupName.isEmpty( ) == false )
        {
            mMapLogScope.lock( );

            for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
            {
                LogScope * scope = mMapLogScope.valueAtPosition( pos );
                if ( scopeGroupName.compare( scope->getScopeName( ), true ) == Ordering::Equal )
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
            LogScope * scope = mMapLogScope.valueAtPosition(pos);
            ASSERT(scope != nullptr);
            scope->setPriority(static_cast<uint32_t>(LogPriority::PrioNotset));
        }

        mConfigScopeList.clear();
        mConfigScopeGroup.clear();

        mMapLogScope.unlock();
    }

    void ScopeController::activateDefaults()
    {
        for (const auto& entry : DEFAULT_LOG_ENABLED_SCOPES)
        {
            mConfigScopeGroup.setAt(entry.first, entry.second);
        }
    }

    void ScopeController::configureScopes( const Property & prop )
    {
        const PropertyKey & Key = prop.getKey( );
        const PropertyValue & Value = prop.getValue( );
        ASSERT( Key.isValid( ) );
        ASSERT( Key.getKeyType() == ConfigEntry::LogScope );

        uint32_t prio = Value.getIndetifier( LogScopePriorityIndentifiers );
        configureScopes(Key.getPosition(), prio);
    }

    void ScopeController::configureScopes( const String & scopeName, uint32_t scopePrio )
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
        std::vector<Property> scopes;
        Application::getConfigManager().getModuleLogScopes(scopes);
        for (const Property& prop : scopes)
        {
            configureScopes(prop);
        }
    }

    void ScopeController::activateScope( LogScope & logScope )
    {
        uint32_t logPrio{ DEFAULT_LOG_PRIORITY };
        mConfigScopeGroup.find(LOG_SCOPES_GRPOUP, logPrio );
        activateScope( logScope, logPrio );
    }

    void ScopeController::activateScope( LogScope & logScope, uint32_t defaultPrio )
    {
        const String & scopeName = logScope.getScopeName( );
        uint32_t scopePrio{ defaultPrio };

        if ( mConfigScopeList.find( scopeName, scopePrio ) )
        {
            logScope.setPriority( scopePrio ); // exact match. Set priority
        }
        else
        {
            logScope.setPriority( defaultPrio ); // set first default priority
            String groupName( scopeName );
            CharPos pos = END_POS;
            do
            {
                pos = groupName.findLast( SYNTAX_SCOPE_SEPARATOR, pos, true );
                if ( groupName.isValidPosition( pos ) )
                {
                    // set group syntax
                    groupName.setAt( SYNTAX_SCOPE_GROUP, pos + 1 ).resize( pos + 2 );
                    pos -= 1;
                }
                else
                {
                    pos = INVALID_POS;
                    groupName = SYNTAX_SCOPE_GROUP;
                }

                if ( mConfigScopeGroup.find( groupName, scopePrio ) )
                {
                    // Found group priority, set it and exit the loop
                    logScope.setPriority( scopePrio );
                    break;
                }

            } while ( pos != INVALID_POS );
        }
    }

    void ScopeController::changeScopeActivityStatus( bool makeActive )
    {
        mMapLogScope.lock( );

        if ( makeActive )
        {
            uint32_t defaultPrio = DEFAULT_LOG_PRIORITY;
            mConfigScopeGroup.find( LOG_SCOPES_GRPOUP, defaultPrio );

            for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
            {
                activateScope( *mMapLogScope.valueAtPosition( pos ), defaultPrio );
            }
        }
        else
        {
            for ( auto pos = mMapLogScope.firstPosition( ); mMapLogScope.isValidPosition( pos ); pos = mMapLogScope.nextPosition( pos ) )
            {
                mMapLogScope.valueAtPosition( pos )->setPriority( static_cast<uint32_t>(LogPriority::PrioNotset) );
            }
        }

        mMapLogScope.unlock( );
    }

    void ScopeController::changeScopeActivityStatus( const String & scopeName, uint32_t scopeId, uint32_t logPrio )
    {
        if ( _isScopeGroup( scopeName ) )
        {
            setScopeGroupPriority( scopeName, logPrio );
            mConfigScopeGroup.setAt( scopeName, logPrio );
        }
        else
        {
            if (scopeId != LOG_SCOPE_ID_NONE)
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

} // namespace areg