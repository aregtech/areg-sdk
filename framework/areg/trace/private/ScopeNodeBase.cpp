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

#include "areg/persist/ConfigManager.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/NELogging.hpp"

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
    while ( *(str + startPos) == NELogging::SYNTAX_SCOPE_SEPARATOR )
    {
        ++ startPos;
    }

    NEString::CharPos pos = scopeName.findFirst(NELogging::SYNTAX_SCOPE_SEPARATOR, startPos );
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

unsigned int ScopeNodeBase::updateConfigNode(ConfigManager& /*config*/, const String& /*parentPath*/) const
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
    String scopeName( traceScope.getScopeName( ) );
    return addChildRecursive( scopeName, traceScope.getPriority( ) );
}

unsigned int ScopeNodeBase::groupRecursive( void )
{
    return 0;
}

String ScopeNodeBase::makeConfigString( const String & parent ) const
{
    if (isValid())
    {
        char scope[NETrace::LOG_MESSAGE_BUFFER_SIZE];
        int len = String::formatString(scope, NETrace::LOG_MESSAGE_BUFFER_SIZE, "%s%s", parent.getString(), mNodeName.getString());
        return String(scope, len > 0 ? len : 0);
    }
    else
    {
        return parent;
    }
}

unsigned int ScopeNodeBase::removePriorityNodesRecursive( unsigned int prioRemove )
{
    return 0;
}

bool ScopeNodeBase::isEmpty( void ) const
{
    return true;
}
