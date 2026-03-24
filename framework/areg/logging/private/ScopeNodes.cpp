/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeNodes.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The scope nodes classes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/ScopeNodes.hpp"
#include "areg/logging/private/LogOptions.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/base/FileBase.hpp"
#include "areg/base/Process.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/persist/PersistenceDefs.hpp"

#if AREG_LOGGING

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ScopeLeaf class implementation
//////////////////////////////////////////////////////////////////////////

ScopeLeaf::ScopeLeaf()
    : ScopeNodeBase     ( ScopeNodeBase::NodeType::Leaf )
{
}

ScopeLeaf::ScopeLeaf( const ScopeNodeBase & base )
    : ScopeNodeBase ( ScopeNodeBase::NodeType::Leaf, base.node_name(), base.priority() )
{
}

ScopeLeaf::ScopeLeaf( const ScopeLeaf & src )
    : ScopeNodeBase ( static_cast<const ScopeNodeBase &>(src) )
{
}

ScopeLeaf::ScopeLeaf( ScopeLeaf && src ) noexcept
    : ScopeNodeBase ( std::move(static_cast<ScopeNodeBase &>(src)) )
{
}

String ScopeLeaf::make_scope_path( const String & prefix ) const
{
    String result{ prefix };
    result += mNodeName;
    return result;
}

uint32_t ScopeLeaf::update_config_node(ConfigManager& config, const String & parentPath ) const
{
    config.add_log_scope(make_config_string(parentPath), mPrioStates);
    return 1;
}

//////////////////////////////////////////////////////////////////////////
// ScopeNode class implementation
//////////////////////////////////////////////////////////////////////////

ScopeNode::ScopeNode()
    : ScopeNodeBase ( ScopeNodeBase::NodeType::Node )
    , mChildNodes   ( true )
    , mChildLeafs   ( true )
{
}

ScopeNode::ScopeNode( const ScopeNodeBase & base )
    : ScopeNodeBase ( ScopeNodeBase::NodeType::Node, base.node_name( ), base.priority() )
    , mChildNodes   ( true )
    , mChildLeafs   ( true )
{
}

ScopeNode::ScopeNode( const ScopeNode & src )
    : ScopeNodeBase ( static_cast<const ScopeNodeBase &>(src) )
    , mChildNodes   ( src.mChildNodes )
    , mChildLeafs   ( src.mChildLeafs )
{
}

ScopeNode::ScopeNode( ScopeNode && src ) noexcept
    : ScopeNodeBase ( std::move(static_cast<ScopeNodeBase &&>(src)) )
    , mChildNodes   ( std::move(src.mChildNodes) )
    , mChildLeafs   ( std::move(src.mChildLeafs) )
{
}

ScopeNode::ScopeNode( ScopeNodeBase::NodeType nodeType, const String & name, uint32_t prio )
    : ScopeNodeBase( nodeType, name, prio )
    , mChildNodes( true )
    , mChildLeafs( true )
{
}

ScopeNode & ScopeNode::operator = ( const ScopeNode & src )
{
    ScopeNodeBase::operator = ( static_cast<const ScopeNodeBase &>(src) );
    if ( this != &src )
    {
        mChildNodes = src.mChildNodes;
        mChildLeafs = src.mChildLeafs;
    }

    return (*this);
}

ScopeNode & ScopeNode::operator=( ScopeNode && src ) noexcept
{
    ScopeNodeBase::operator = ( std::move(static_cast<ScopeNodeBase &&>(src)) );
    if ( this != &src )
    {
        mChildNodes = std::move(src.mChildNodes);
        mChildLeafs = std::move(src.mChildLeafs);
    }

    return (*this);
}

const ScopeNodeBase & ScopeNode::make_child_node( String & scopePath, uint32_t prioStates ) const
{
    if ( scopePath.is_empty( ) )
    {
        return ScopeNodeBase::make_child_node( scopePath, prioStates );
    }

    // A '.' prefix means the remaining path is a leaf name verbatim (may contain '_').
    // Strip the dot and create a leaf without further splitting.
    if ( scopePath.as_string()[0] == areg::SYNTAX_SCOPE_LEAF_SEPARATOR )
    {
        scopePath.substring( 1 );   // remove leading '.'
        static ScopeLeaf _dotLeaf;
        _dotLeaf.set_node_name( scopePath );
        _dotLeaf.set_priority( prioStates );
        scopePath = String::EmptyString;
        return _dotLeaf;
    }

    String nodeName = ScopeNodeBase::extract_node_name( scopePath );
    if ( scopePath.is_empty( ) )
    {
        static ScopeLeaf _leaf;
        _leaf.set_node_name( nodeName );
        _leaf.set_priority( prioStates );
        return _leaf;
    }
    else
    {
        static ScopeNode _node;
        _node.set_node_name( nodeName );
        _node.set_priority( prioStates );
        return _node;
    }
}

std::pair<ScopeNodeBase &, bool>  ScopeNode::add_child_node( const ScopeNodeBase & child )
{
    ScopeNodeBase * scope = &(ScopeNodeBase::invalid_node());
    bool newEntry{ false };

    if ( child.is_leaf( ) )
    {
        // Cast to ScopeLeaf so the copy constructor preserves mDotSeparator.
        const ScopeLeaf & leafChild = static_cast<const ScopeLeaf &>(child);
        auto atPos = mChildLeafs.add_if_unique( leafChild, false );
        const ScopeNodeBase& leaf{ mChildLeafs.value_at(atPos.first) };
        scope = const_cast<ScopeNodeBase *>(&leaf);
        newEntry = atPos.second;
    }
    else if (child.is_node( ))
    {
        auto atPos = mChildNodes.add_if_unique( ScopeNode( child ), false );
        const ScopeNodeBase& node{ mChildNodes.value_at(atPos.first) };
        scope = const_cast<ScopeNodeBase*>(&node);
        scope->add_priority(child.priority());
        newEntry = atPos.second;
    }

    if ( scope->is_valid() )
    {
        mPrioStates |= child.priority( );
    }

    return std::pair<ScopeNodeBase &, bool>{*scope, newEntry};
}

std::pair<ScopeNodeBase &, bool>  ScopeNode::add_child_node( String & scopePath, uint32_t prioStates )
{
    const ScopeNodeBase & node = make_child_node( scopePath, prioStates );
    return add_child_node( node );
}

String ScopeNode::make_scope_path( const String & prefix ) const
{
    ASSERT(!mNodeName.is_empty());
    ASSERT(is_valid());
    return prefix + mNodeName + areg::SYNTAX_SCOPE_SEPARATOR;
}

uint32_t ScopeNode::group_child_nodes() 
{
    uint32_t result{ 0 };
    uint32_t sameLeafs{ mChildLeafs.size( ) };
    uint32_t sameNodes{ mChildNodes.size() };
    uint32_t prioNode{ mPrioStates };

    mGrouping = static_cast<uint32_t>(ScopeNodeBase::Grouping::None);

    if ( sameNodes > 0 )
    {
        for ( NodeList::LISTPOS pos = mChildNodes.first_position( ); mChildNodes.is_valid_position( pos ); pos = mChildNodes.next_position( pos ) )
        {
            const ScopeNode & node{ mChildNodes.value_at( pos ) };
            if ( (node.priority( ) != prioNode) || (node.child_node_count( ) != 0) )
            {
                -- sameNodes;
                break;
            }
        }

        if ( sameNodes == mChildNodes.size( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Nodes);
            result += mChildNodes.size( );
            mChildNodes.clear( );

            result += remove_priority_nodes( prioNode );
        }
        else
        {
            prioNode = 0;
        }
    }

    if ( sameLeafs > 0 )
    {
        for ( LeafList::LISTPOS pos = mChildLeafs.first_position( ); mChildLeafs.is_valid_position( pos ); pos = mChildLeafs.next_position( pos ) )
        {
            const ScopeLeaf & leaf{ mChildLeafs.value_at( pos ) };
            uint32_t prio = leaf.priority( );
            if (prio != prioNode )
            {
                -- sameLeafs;
                break;
            }
        }

        if (sameLeafs == mChildLeafs.size( ))
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Leaves);
            result += mChildLeafs.size( );
            mChildLeafs.clear( );
            result += remove_priority_nodes( prioNode );
        }
    }

    return result;
}

uint32_t ScopeNode::update_config_node( ConfigManager & config, const String & parentPath ) const
{
    uint32_t result{ 0 };
    String thisScope = make_scope_path( parentPath );
    if ( (mGrouping & static_cast<uint32_t>(ScopeNodeBase::Grouping::All)) != 0 )
    {
        config.add_log_scope(make_config_string(parentPath), mPrioStates);
        result = 1;
    }

    for ( auto pos = mChildNodes.first_position( ); mChildNodes.is_valid_position( pos ); pos = mChildNodes.next_position( pos ) )
    {
        const ScopeNode & node = mChildNodes.value_at( pos );
        result += node.update_config_node( config, thisScope );
    }

    for ( auto pos = mChildLeafs.first_position( ); mChildLeafs.is_valid_position( pos ); pos = mChildLeafs.next_position( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.value_at( pos );
        String leafScope = parentPath + mNodeName + areg::SYNTAX_SCOPE_LEAF_SEPARATOR;
        result += leaf.update_config_node( config, leafScope );
    }

    return result;
}

uint32_t ScopeNode::group_recursive()
{
    uint32_t result{ 0 };
    for ( auto pos = mChildNodes.first_position( ); mChildNodes.is_valid_position( pos ); pos = mChildNodes.next_position( pos ) )
    {
        const ScopeNode& node{ mChildNodes.value_at(pos) };
        result += const_cast<ScopeNode &>(node).group_recursive( );
    }

    result += group_child_nodes( );
    return result;
}

String ScopeNode::make_config_string( const String & parent ) const
{
    return parent + mNodeName + areg::SYNTAX_SCOPE_SEPARATOR + areg::SYNTAX_LOG_GROUP;
}

uint32_t ScopeNode::remove_priority_nodes( uint32_t prioRemove ) noexcept
{
    uint32_t result{ 0 };

    if ( mChildLeafs.size( ) != 0 )
    {
        LeafList::LISTPOS pos = mChildLeafs.first_position( );
        while ( mChildLeafs.is_valid_position( pos ) )
        {
            const ScopeNodeBase& leaf{ mChildLeafs.value_at(pos) };
            if ( leaf.priority( ) == prioRemove )
            {
                pos = mChildLeafs.remove_at( pos );
                ++ result;
            }
            else
            {
                pos = mChildLeafs.next_position( pos );
            }
        }

        if ( mChildLeafs.is_empty( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Leaves);
        }
    }

    if ( mChildNodes.size( ) != 0 )
    {
        NodeList::LISTPOS pos = mChildNodes.first_position( );
        while ( mChildNodes.is_valid_position( pos ) )
        {
            const ScopeNodeBase& node{ mChildNodes.value_at(pos) };
            if ( mChildNodes.value_at( pos ).priority( ) == prioRemove )
            {
                result += const_cast<ScopeNodeBase &>(node).remove_priority_nodes( prioRemove );
                if (node.is_empty() )
                {
                    pos = mChildNodes.remove_at( pos );
                    ++ result;
                }
            }
            else
            {
                const_cast<ScopeNodeBase&>(node).remove_priority_nodes( prioRemove );
                pos = mChildNodes.next_position( pos );
            }
        }

        if ( mChildNodes.is_empty( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Nodes);
        }
    }

    return result;
}

bool ScopeNode::is_empty() const noexcept
{
    return mChildLeafs.is_empty() && mChildNodes.is_empty();
}

//////////////////////////////////////////////////////////////////////////
// ScopeRoot class implementation
//////////////////////////////////////////////////////////////////////////

ScopeRoot::ScopeRoot()
    : ScopeNode     ( ScopeNodeBase::NodeType::Root, Process::instance().app_name(), static_cast<uint32_t>(areg::LogPriority::PrioNotset) )
{
}

String ScopeRoot::make_scope_path( const String & /*prefix*/ ) const
{
    return String::EmptyString;
}

uint32_t ScopeRoot::update_config_node( ConfigManager & config, const String & /*parentPath*/ ) const
{
    uint32_t result{ 0 };

    String thisScope( make_scope_path(String::EmptyString) );
    for ( auto pos = mChildLeafs.first_position( ); mChildLeafs.is_valid_position( pos ); pos = mChildLeafs.next_position( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.value_at( pos );
        result += leaf.update_config_node(config, thisScope);
    }

    for ( auto pos = mChildNodes.first_position( ); mChildNodes.is_valid_position( pos ); pos = mChildNodes.next_position( pos ) )
    {
        const ScopeNode & node = mChildNodes.value_at( pos );
        result += node.update_config_node(config, thisScope);
    }

    if ( result == 0 )
    {
        config.add_log_scope(make_config_string(thisScope), mPrioStates);
        result = 1;
    }

    return result;
}

String ScopeRoot::make_config_string(const String& parent) const
{
    return (parent + areg::SYNTAX_LOG_GROUP);
}

} // namespace areg

#endif // AREG_LOGGING
