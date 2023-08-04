/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/ScopeNodeBase.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, The base class of scope nodes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/trace/private/ScopeNodeBase.hpp"

#include "areg/base/FileBase.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/NELogConfig.hpp"

ScopeNodeBase & ScopeNodeBase::invalidNode( void )
{
    static ScopeNodeBase _invalid;
    return _invalid;
}

ScopeNodeBase::ScopeNodeBase( void )
    : mNodeType     ( ScopeNodeBase::eNode::Invalid )
    , mPrioStates   ( static_cast<uint32_t>(NETrace::eLogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGroupping    ( static_cast<unsigned int>(ScopeNodeBase::eGroupping::NoGroupping) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::eNode nodeType )
    : mNodeType     ( nodeType )
    , mPrioStates   ( static_cast<uint32_t>(NETrace::eLogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGroupping    ( static_cast<unsigned int>(ScopeNodeBase::eGroupping::NoGroupping) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::eNode nodeType, const String & nodeName, unsigned int prio /*= static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset)*/ )
    : mNodeType     ( nodeType )
    , mPrioStates   ( prio )
    , mNodeName     ( nodeName )
    , mGroupping    ( static_cast<unsigned int>(ScopeNodeBase::eGroupping::NoGroupping) )
{
}

ScopeNodeBase::ScopeNodeBase( const ScopeNodeBase & src )
    : mNodeType     ( src.mNodeType )
    , mPrioStates   ( src.mPrioStates )
    , mNodeName     ( src.mNodeName )
    , mGroupping    ( src.mGroupping )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase && src ) noexcept
    : mNodeType     ( src.mNodeType )
    , mPrioStates   ( src.mPrioStates )
    , mNodeName     ( std::move(src.mNodeName) )
    , mGroupping    ( src.mGroupping )
{
}

String ScopeNodeBase::extractNodeName( String & scopeName )
{
    String result(scopeName);
    NEString::CharPos startPos = NEString::START_POS;
    const char * str = scopeName.getString( );

    // move position forward if a node starts with '_', which should
    // be included in the node name.
    while ( *(str + startPos) == NELogConfig::SYNTAX_SCOPE_SEPARATOR )
    {
        ++ startPos;
    }

    NEString::CharPos pos = scopeName.findFirst( NELogConfig::SYNTAX_SCOPE_SEPARATOR, startPos );
    if ( NEString::isPositionValid(pos) )
    {
        result.substring( 0, pos );
        scopeName.substring( pos + 1 );
    }
    else
    {
        scopeName = String::EmptyString;
    }

    return result;
}

ScopeNodeBase & ScopeNodeBase::operator = ( const ScopeNodeBase & src )
{
    ASSERT( mNodeType == src.mNodeType );
    if ( this != &src )
    {
        mPrioStates = src.mPrioStates;
        mNodeName   = src.mNodeName;
        mGroupping  = src.mGroupping;
    }

    return (*this);
}

ScopeNodeBase & ScopeNodeBase::operator = ( ScopeNodeBase && src ) noexcept
{
    ASSERT( mNodeType == src.mNodeType );
    if ( this != &src )
    {
        mPrioStates = src.mPrioStates;
        mNodeName   = std::move(src.mNodeName);
        mGroupping  = src.mGroupping;
    }

    return (*this);
}

bool ScopeNodeBase::operator == ( const ScopeNodeBase & other ) const
{
    return (mNodeType == other.mNodeType) && (mNodeName == other.mNodeName);
}

bool ScopeNodeBase::operator != ( const ScopeNodeBase & other ) const
{
    return (mNodeType != other.mNodeType) || (mNodeName != other.mNodeName);
}

bool ScopeNodeBase::operator > ( const ScopeNodeBase & other ) const
{
    return (mNodeType == other.mNodeType ? (mNodeName > other.mNodeName) : (mNodeType > other.mNodeType));
}

bool ScopeNodeBase::operator < ( const ScopeNodeBase & other ) const
{
    return (mNodeType == other.mNodeType ? (mNodeName < other.mNodeName) : (mNodeType < other.mNodeType));
}

const ScopeNodeBase & ScopeNodeBase::makeChildNode( String & IN OUT scopePath, unsigned int prioStates ) const
{
    static ScopeNodeBase _invalidNode;
    scopePath = String::EmptyString;
    return _invalidNode;
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( const ScopeNodeBase & child )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( String & scopePath, unsigned int prioStates )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

String ScopeNodeBase::makeScopePath( const String & prefix ) const
{
    return prefix;
}

unsigned int ScopeNodeBase::groupChildNodes( void )
{
    return 0;
}

unsigned int ScopeNodeBase::saveNodeConfig( FileBase & file, const String & parentPath ) const
{
    return 0;
}

unsigned int ScopeNodeBase::addChildRecursive( String & scopePath, unsigned int prioStates )
{
    std::pair<ScopeNodeBase &, bool> node = addChildNode( scopePath, prioStates );
    return (node.first.isValid() ? (1 + node.first.addChildRecursive(scopePath, prioStates)) : 0);
}

unsigned int ScopeNodeBase::addChildRecursive( const TraceScope & traceScope )
{
    return addChildRecursive( traceScope.getScopeName( ), traceScope.getPriority( ) );
}

unsigned int ScopeNodeBase::groupRecursive( void )
{
    return 0;
}

String ScopeNodeBase::makeConfigString( const String & parent ) const
{
    String result(parent);
    return (isValid() ? result.format("%s%s = %s", parent.getString( ), mNodeName.getString( ), makePrioString( ).getString( )) : result);
}

unsigned int ScopeNodeBase::removePriorityNodesRecursive( unsigned int prioRemove )
{
    return 0;
}

bool ScopeNodeBase::isEmpty( void ) const
{
    return true;
}

String ScopeNodeBase::makePrioString( void ) const
{
    String result( NETrace::PRIO_NOTSET_STR );
    if ( hasLogsEneabled( ) )
    {
        if ( hasPrioDebug( ) )
        {
            result = NETrace::PRIO_DEBUG_STR;
        }
        else if ( hasPrioInfo( ) )
        {
            result = NETrace::PRIO_INFO_STR;
        }
        else if ( hasPrioWarning( ) )
        {
            result = NETrace::PRIO_WARNING_STR;
        }
        else if ( hasPrioError( ) )
        {
            result = NETrace::PRIO_ERROR_STR;
        }
        else if ( hasPrioFatal( ) )
        {
            result = NETrace::PRIO_FATAL_STR;
        }
        else
        {
            ASSERT( false );
        }

        if ( hasLogScopes( ) )
        {
            result.format("%s | %s", result.getString() : NETrace::PRIO_SCOPE_STR.data());
        }
    }

    return result;
}
