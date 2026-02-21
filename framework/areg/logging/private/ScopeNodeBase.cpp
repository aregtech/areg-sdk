/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeNodeBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The base class of scope nodes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "areg/logging/private/ScopeNodeBase.hpp"

#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogOptions.hpp"

#if AREG_LOGS

ScopeNodeBase & ScopeNodeBase::invalidNode()
{
    static ScopeNodeBase _invalid;
    return _invalid;
}

ScopeNodeBase::ScopeNodeBase()
    : mNodeType     ( ScopeNodeBase::NodeType::Invalid )
    , mPrioStates   ( static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGrouping     ( static_cast<uint32_t>(ScopeNodeBase::Grouping::None) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::NodeType nodeType )
    : mNodeType     ( nodeType )
    , mPrioStates   ( static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGrouping    ( static_cast<uint32_t>(ScopeNodeBase::Grouping::None) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::NodeType nodeType, const String & nodeName, uint32_t prio /*= static_cast<uint32_t>(NELogging::LogPriority::PrioNotset)*/ )
    : mNodeType     ( nodeType )
    , mPrioStates   ( prio )
    , mNodeName     ( nodeName )
    , mGrouping    ( static_cast<uint32_t>(ScopeNodeBase::Grouping::None) )
{
}

ScopeNodeBase::ScopeNodeBase( const ScopeNodeBase & src )
    : mNodeType     ( src.mNodeType )
    , mPrioStates   ( src.mPrioStates )
    , mNodeName     ( src.mNodeName )
    , mGrouping    ( src.mGrouping )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase && src ) noexcept
    : mNodeType     ( src.mNodeType )
    , mPrioStates   ( src.mPrioStates )
    , mNodeName     ( std::move(src.mNodeName) )
    , mGrouping    ( src.mGrouping )
{
}

String ScopeNodeBase::extractNodeName( String & scopeName )
{
    String result(scopeName);
    NEString::CharPos startPos = NEString::START_POS;
    const char * str = scopeName.getString( );

    // move position forward if a node starts with '_', which should
    // be included in the node name.
    while ( *(str + startPos) == NELogOptions::SYNTAX_SCOPE_SEPARATOR )
    {
        ++ startPos;
    }

    NEString::CharPos pos = scopeName.findFirst(NELogOptions::SYNTAX_SCOPE_SEPARATOR, startPos );
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
        mGrouping  = src.mGrouping;
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
        mGrouping  = src.mGrouping;
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

const ScopeNodeBase & ScopeNodeBase::makeChildNode( String & scopePath, uint32_t /* prioStates */ ) const
{
    static ScopeNodeBase _invalidNode;
    scopePath = String::EmptyString;
    return _invalidNode;
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( const ScopeNodeBase & /* child */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( String & /* scopePath */, uint32_t /* prioStates */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

String ScopeNodeBase::makeScopePath( const String & prefix ) const
{
    return prefix;
}

uint32_t ScopeNodeBase::groupChildNodes()
{
    return 0;
}

uint32_t ScopeNodeBase::updateConfigNode(ConfigManager& /*config*/, const String& /*parentPath*/) const
{
    return 0;
}

uint32_t ScopeNodeBase::addChildRecursive( String & scopePath, uint32_t prioStates )
{
    std::pair<ScopeNodeBase &, bool> node = addChildNode( scopePath, prioStates );
    return (node.first.isValid() ? (1 + node.first.addChildRecursive(scopePath, prioStates)) : 0);
}

uint32_t ScopeNodeBase::addChildRecursive( const LogScope & logScope )
{
    String scopeName( logScope.getScopeName( ) );
    return addChildRecursive( scopeName, logScope.getPriority( ) );
}

uint32_t ScopeNodeBase::groupRecursive()
{
    return 0;
}

String ScopeNodeBase::makeConfigString( const String & parent ) const
{
    if (isValid())
    {
        char scope[NELogging::LOG_MESSAGE_IZE];
        uint32_t len = static_cast<uint32_t>(String::formatString(scope, NELogging::LOG_MESSAGE_IZE, "%s%s", parent.getString(), mNodeName.getString()));
        return String(scope, len);
    }
    else
    {
        return parent;
    }
}

uint32_t ScopeNodeBase::removePriorityNodesRecursive( uint32_t /* prioRemove */ )
{
    return 0;
}

bool ScopeNodeBase::isEmpty() const
{
    return true;
}

#endif  // AREG_LOGS
