/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/ScopeController.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace controller.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/ScopeController.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/private/TraceProperty.hpp"


inline bool ScopeController::_isScopeGroup( const String & scopeName )
{
    return (scopeName.findLast( NELogConfig::SYNTAX_SCOPE_GROUP ) >= NEString::START_POS);
}

void ScopeController::registerScope( TraceScope & scope )
{
    ASSERT( mMapTraceScope.findResourceObject( static_cast<unsigned int>(scope) ) == nullptr );
    mMapTraceScope.registerResourceObject( static_cast<unsigned int>(scope), &scope );
}

void ScopeController::unregisterScope( TraceScope & scope )
{
    mMapTraceScope.unregisterResourceObject( static_cast<unsigned int>(scope) );
}

void ScopeController::setScopePriority( unsigned int scopeId, unsigned int newPrio )
{
    mMapTraceScope.lock( );

    TraceScope * scope = mMapTraceScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->setPriority( newPrio );
    }

    mMapTraceScope.unlock( );
}

void ScopeController::addScopePriority( unsigned int scopeId, NETrace::eLogPriority addPrio )
{
    mMapTraceScope.lock( );

    TraceScope * scope = mMapTraceScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->addPriority( addPrio );
    }

    mMapTraceScope.unlock( );
}

void ScopeController::removeScopePriority( unsigned int scopeId, NETrace::eLogPriority remPrio )
{
    mMapTraceScope.lock( );

    TraceScope * scope = mMapTraceScope.findResourceObject( scopeId );
    if ( scope != nullptr )
    {
        scope->removePriority( remPrio );
    }

    mMapTraceScope.unlock( );
}

int ScopeController::setScopeGroupPriority( const String & scopeGroupName, unsigned int newPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapTraceScope.lock( );

        for ( TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition( ); mMapTraceScope.isValidPosition( pos ); pos = mMapTraceScope.nextPosition( pos ) )
        {
            TraceScope * scope = mMapTraceScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == NEMath::eCompare::Equal )
            {
                scope->setPriority( newPrio );
                ++ result;
            }
        }

        mMapTraceScope.unlock( );
    }

    return result;
}

int ScopeController::addScopeGroupPriority( const String & scopeGroupName, NETrace::eLogPriority addPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapTraceScope.lock( );

        for ( TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition( ); mMapTraceScope.isValidPosition( pos ); pos = mMapTraceScope.nextPosition( pos ) )
        {
            TraceScope * scope = mMapTraceScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == NEMath::eCompare::Equal )
            {
                scope->addPriority( addPrio );
                ++ result;
            }
        }

        mMapTraceScope.unlock( );
    }

    return result;
}

int ScopeController::removeScopeGroupPriority( const String & scopeGroupName, NETrace::eLogPriority remPrio )
{
    int result{ 0 };
    if ( scopeGroupName.isEmpty( ) == false )
    {
        mMapTraceScope.lock( );

        for ( TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition( ); mMapTraceScope.isValidPosition( pos ); pos = mMapTraceScope.nextPosition( pos ) )
        {
            TraceScope * scope = mMapTraceScope.valueAtPosition( pos );
            if ( scopeGroupName.compare( scope->getScopeName( ), true ) == NEMath::eCompare::Equal )
            {
                scope->removePriority( remPrio );
                ++ result;
            }
        }

        mMapTraceScope.unlock( );
    }

    return result;
}

void ScopeController::resetScopes(void)
{
    mMapTraceScope.lock();

    for (TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition(); mMapTraceScope.isValidPosition(pos); pos = mMapTraceScope.nextPosition(pos))
    {
        TraceScope * scope = mMapTraceScope.valueAtPosition(pos);
        ASSERT(scope != nullptr);
        scope->setPriority(static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset));
    }

    mConfigScopeList.clear();
    mConfigScopeGroup.clear();

    mMapTraceScope.unlock();
}

void ScopeController::activateDefaults( void )
{
    int i = 0;
    do
    {
        const sLogEnabling & scopes = ScopeController::DEFAULT_LOG_ENABLED_SCOPES[ i ++ ];
        if ( scopes.logScope == nullptr )
            break;

        mConfigScopeGroup.setAt( scopes.logScope, static_cast<int>(scopes.logPrio) );
    } while ( true );
}

void ScopeController::configureScopes( const TraceProperty & prop )
{
    const TracePropertyKey & Key = prop.getKey( );
    const TracePropertyValue & Value = prop.getValue( );
    ASSERT( Key.getLogConfig( ) == NELogConfig::eLogConfig::ConfigScope );
    ASSERT( Key.isValidKey( ) );

    unsigned int prio = Value.getPriority( );
    if ( Key.isGlobalKey( ) && Key.isModuleGlobal() )
    {
        mConfigScopeGroup.setAt( NELogConfig::LOG_SCOPES_GRPOUP, prio );
    }
    else
    {
        configureScopes( Key.getModuleData( ), prio );
    }
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

void ScopeController::activateScope( TraceScope & traceScope )
{
    unsigned int logPrio{ NELogConfig::DEFAULT_LOG_PRIORITY };
    mConfigScopeGroup.find( NELogConfig::LOG_SCOPES_GRPOUP, logPrio );
    activateScope( traceScope, logPrio );
}

void ScopeController::activateScope( TraceScope & traceScope, unsigned int defaultPriority )
{
    const String & scopeName = traceScope.getScopeName( );
    unsigned int scopePrio{ defaultPriority };

    if ( mConfigScopeList.find( scopeName, scopePrio ) )
    {
        traceScope.setPriority( scopePrio ); // exact match. Set priority
    }
    else
    {
        traceScope.setPriority( defaultPriority ); // set first default priority
        String groupName( scopeName );
        NEString::CharPos pos = NEString::END_POS;
        do
        {
            pos = groupName.findLast( NELogConfig::SYNTAX_SCOPE_SEPARATOR, pos, true );
            if ( groupName.isValidPosition( pos ) )
            {
                // set group syntax
                groupName.setAt( NELogConfig::SYNTAX_SCOPE_GROUP, pos + 1 ).resize( pos + 2 );
                pos -= 1;
            }
            else
            {
                pos = NEString::INVALID_POS;
                groupName = NELogConfig::SYNTAX_SCOPE_GROUP;
            }

            if ( mConfigScopeGroup.find( groupName, scopePrio ) )
            {
                // Found group priority, set it and exit the loop
                traceScope.setPriority( scopePrio );
                break;
            }

        } while ( pos != NEString::INVALID_POS );
    }
}

void ScopeController::changeScopeActivityStatus( bool makeActive )
{
    mMapTraceScope.lock( );

    if ( makeActive )
    {
        unsigned int defaultPrio = NELogConfig::DEFAULT_LOG_PRIORITY;
        mConfigScopeGroup.find( NELogConfig::LOG_SCOPES_GRPOUP, defaultPrio );

        for ( TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition( ); mMapTraceScope.isValidPosition( pos ); pos = mMapTraceScope.nextPosition( pos ) )
        {
            activateScope( *mMapTraceScope.valueAtPosition( pos ), defaultPrio );
        }
    }
    else
    {
        for ( TraceScopeMap::MAPPOS pos = mMapTraceScope.firstPosition( ); mMapTraceScope.isValidPosition( pos ); pos = mMapTraceScope.nextPosition( pos ) )
        {
            mMapTraceScope.valueAtPosition( pos )->setPriority( NETrace::eLogPriority::PrioNotset );
        }
    }

    mMapTraceScope.unlock( );
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
        setScopePriority( scopeName, logPrio );
        mConfigScopeList.setAt( scopeName, logPrio );
    }
}
