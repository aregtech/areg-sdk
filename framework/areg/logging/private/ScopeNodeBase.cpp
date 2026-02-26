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
    , mPrioStates   ( static_cast<uint32_t>(areg::LogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGrouping     ( static_cast<uint32_t>(ScopeNodeBase::Grouping::None) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::NodeType nodeType )
    : mNodeType     ( nodeType )
    , mPrioStates   ( static_cast<uint32_t>(areg::LogPriority::PrioInvalid) )
    , mNodeName     ( )
    , mGrouping    ( static_cast<uint32_t>(ScopeNodeBase::Grouping::None) )
{
}

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::NodeType nodeType, const areg::String & nodeName, uint32_t prio /*= static_cast<uint32_t>(areg::LogPriority::PrioNotset)*/ )
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

areg::String ScopeNodeBase::extractNodeName( areg::String & scopeName )
{
    areg::String result(scopeName);
    areg::CharPos startPos = areg::START_POS;
    const char * str = scopeName.getString( );

    // move position forward if a node starts with '_', which should
    // be included in the node name.
    while ( *(str + startPos) == areg::SYNTAX_SCOPE_SEPARATOR )
    {
        ++ startPos;
    }

    areg::CharPos pos = scopeName.findFirst(areg::SYNTAX_SCOPE_SEPARATOR, startPos );
    if ( areg::isPositionValid(pos) )
    {
        result.substring( 0, pos );
        scopeName.substring( pos + 1 );
    }
    else
    {
        scopeName = areg::String::EmptyString;
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

const ScopeNodeBase & ScopeNodeBase::makeChildNode( areg::String & scopePath, uint32_t /* prioStates */ ) const
{
    static ScopeNodeBase _invalidNode;
    scopePath = areg::String::EmptyString;
    return _invalidNode;
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( const ScopeNodeBase & /* child */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::addChildNode( areg::String & /* scopePath */, uint32_t /* prioStates */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalidNode( ), false};
}

areg::String ScopeNodeBase::makeScopePath( const areg::String & prefix ) const
{
    return prefix;
}

uint32_t ScopeNodeBase::groupChildNodes()
{
    return 0;
}

uint32_t ScopeNodeBase::updateConfigNode(ConfigManager& /*config*/, const areg::String& /*parentPath*/) const
{
    return 0;
}

uint32_t ScopeNodeBase::addChildRecursive( areg::String & scopePath, uint32_t prioStates )
{
    std::pair<ScopeNodeBase &, bool> node = addChildNode( scopePath, prioStates );
    return (node.first.isValid() ? (1 + node.first.addChildRecursive(scopePath, prioStates)) : 0);
}

uint32_t ScopeNodeBase::addChildRecursive( const areg::LogScope & logScope )
{
    areg::String scopeName( logScope.getScopeName( ) );
    return addChildRecursive( scopeName, logScope.getPriority( ) );
}

uint32_t ScopeNodeBase::groupRecursive()
{
    return 0;
}

areg::String ScopeNodeBase::makeConfigString( const areg::String & parent ) const
{
    if (isValid())
    {
        char scope[areg::LOG_MESSAGE_IZE];
        uint32_t len = static_cast<uint32_t>(areg::String::formatString(scope, areg::LOG_MESSAGE_IZE, "%s%s", parent.getString(), mNodeName.getString()));
        return areg::String(scope, len);
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
