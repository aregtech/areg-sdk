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

#if AREG_LOGGING

namespace areg {

ScopeNodeBase & ScopeNodeBase::invalid_node()
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

ScopeNodeBase::ScopeNodeBase( ScopeNodeBase::NodeType nodeType, const String & nodeName, uint32_t prio /*= static_cast<uint32_t>(areg::LogPriority::PrioNotset)*/ )
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

String ScopeNodeBase::extract_node_name( String & scopeName )
{
    String result(scopeName);
    areg::CharPos startPos = areg::START_POS;
    const char * str = scopeName.as_string( );

    // move position forward if a node starts with '_', which should
    // be included in the node name.
    while ( *(str + startPos) == areg::SYNTAX_SCOPE_SEPARATOR )
    {
        ++ startPos;
    }

    areg::CharPos pos = scopeName.find_first(areg::SYNTAX_SCOPE_SEPARATOR, startPos );
    if ( areg::is_position_valid(pos) )
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

const ScopeNodeBase & ScopeNodeBase::make_child_node( String & scopePath, uint32_t /* prioStates */ ) const
{
    static ScopeNodeBase _invalidNode;
    scopePath = String::EmptyString;
    return _invalidNode;
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::add_child_node( const ScopeNodeBase & /* child */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalid_node( ), false};
}

std::pair<ScopeNodeBase &, bool> ScopeNodeBase::add_child_node( String & /* scopePath */, uint32_t /* prioStates */ )
{
    return std::pair<ScopeNodeBase &, bool>{ScopeNodeBase::invalid_node( ), false};
}

String ScopeNodeBase::make_scope_path( const String & prefix ) const
{
    return prefix;
}

uint32_t ScopeNodeBase::group_child_nodes()
{
    return 0;
}

uint32_t ScopeNodeBase::update_config_node(ConfigManager& /*config*/, const String& /*parentPath*/) const
{
    return 0;
}

uint32_t ScopeNodeBase::add_child_recursive( String & scopePath, uint32_t prioStates )
{
    std::pair<ScopeNodeBase &, bool> node = add_child_node( scopePath, prioStates );
    return (node.first.is_valid() ? (1 + node.first.add_child_recursive(scopePath, prioStates)) : 0);
}

uint32_t ScopeNodeBase::add_child_recursive( const LogScope & logScope )
{
    String scopeName( logScope.scope_name( ) );
    return add_child_recursive( scopeName, logScope.priority( ) );
}

uint32_t ScopeNodeBase::group_recursive()
{
    return 0;
}

String ScopeNodeBase::make_config_string( const String & parent ) const
{
    if (is_valid())
    {
        char scope[areg::LOG_MESSAGE_IZE];
        uint32_t len = static_cast<uint32_t>(String::format_string(scope, areg::LOG_MESSAGE_IZE, "%s%s", parent.as_string(), mNodeName.as_string()));
        return String(scope, len);
    }
    else
    {
        return parent;
    }
}

uint32_t ScopeNodeBase::remove_priority_nodes( uint32_t /* prioRemove */ ) noexcept
{
    return 0;
}

bool ScopeNodeBase::is_empty() const noexcept
{
    return true;
}

} // namespace areg

#endif  // AREG_LOGGING
