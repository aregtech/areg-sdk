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

#if AREG_LOGS

//////////////////////////////////////////////////////////////////////////
// ScopeLeaf class implementation
//////////////////////////////////////////////////////////////////////////

ScopeLeaf::ScopeLeaf()
    : ScopeNodeBase ( ScopeNodeBase::NodeType::Leaf )
{
}

ScopeLeaf::ScopeLeaf( const ScopeNodeBase & base )
    : ScopeNodeBase( ScopeNodeBase::NodeType::Leaf, base.getNodeName(), base.getPriority() )
{
}

ScopeLeaf::ScopeLeaf( const ScopeLeaf & src )
    : ScopeNodeBase ( static_cast<const ScopeNodeBase &>(src) )
{
}

ScopeLeaf::ScopeLeaf( ScopeLeaf && src ) noexcept
    : ScopeNodeBase( std::move(static_cast<ScopeNodeBase &>(src)) )
{
}

String ScopeLeaf::makeScopePath( const String & prefix ) const
{
    String result{ prefix };
    result += mNodeName;
    return result;
}

uint32_t ScopeLeaf::updateConfigNode(ConfigManager& config, const String & parentPath ) const
{
    config.addModuleLogScope(makeConfigString(parentPath), mPrioStates);
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
    : ScopeNodeBase ( ScopeNodeBase::NodeType::Node, base.getNodeName( ), base.getPriority() )
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
    : ScopeNodeBase ( static_cast<const ScopeNodeBase &>(src) )
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

const ScopeNodeBase & ScopeNode::makeChildNode( String & scopePath, uint32_t prioStates ) const
{
    if ( scopePath.isEmpty( ) )
    {
        return ScopeNodeBase::makeChildNode( scopePath, prioStates );
    }
    else
    {
        String nodeName = ScopeNodeBase::extractNodeName( scopePath );
        if ( scopePath.isEmpty( ) )
        {
            static ScopeLeaf _leaf;
            _leaf.setNodeName(nodeName);
            _leaf.setPriority(prioStates);
            return _leaf;
        }
        else
        {
            static ScopeNode _node;
            _node.setNodeName(nodeName);
            _node.setPriority(prioStates);
            return _node;
        }
    }
}

std::pair<ScopeNodeBase &, bool>  ScopeNode::addChildNode( const ScopeNodeBase & child )
{
    ScopeNodeBase * scope = &(ScopeNodeBase::invalidNode());
    bool newEntry{ false };

    if ( child.isLeaf( ) )
    {
        auto atPos = mChildLeafs.addIfUnique( ScopeLeaf( child ), false );
        const ScopeNodeBase& leaf{ mChildLeafs.valueAtPosition(atPos.first) };
        scope = const_cast<ScopeNodeBase *>(&leaf);
        newEntry = atPos.second;
    }
    else if (child.isNode( ))
    {
        auto atPos = mChildNodes.addIfUnique( ScopeNode( child ), false );
        const ScopeNodeBase& node{ mChildNodes.valueAtPosition(atPos.first) };
        scope = const_cast<ScopeNodeBase*>(&node);
        scope->addPriority(child.getPriority());
        newEntry = atPos.second;
    }

    if ( scope->isValid() )
    {
        mPrioStates |= child.getPriority( );
    }

    return std::pair<ScopeNodeBase &, bool>{*scope, newEntry};
}

std::pair<ScopeNodeBase &, bool>  ScopeNode::addChildNode( String & scopePath, uint32_t prioStates )
{
    const ScopeNodeBase & node = makeChildNode( scopePath, prioStates );
    return addChildNode( node );
}

String ScopeNode::makeScopePath( const String & prefix ) const
{
    ASSERT(mNodeName.isEmpty() == false);
    ASSERT(isValid());
    char scope[NELogging::LOG_MESSAGE_IZE];
    uint32_t len = static_cast<uint32_t>(String::formatString(scope, NELogging::LOG_MESSAGE_IZE, "%s%s%c", prefix.getString(), mNodeName.getString(), NELogOptions::SYNTAX_SCOPE_SEPARATOR));

    return String(scope, len);
}

uint32_t ScopeNode::groupChildNodes() 
{
    uint32_t result{ 0 };
    uint32_t sameLeafs{ mChildLeafs.getSize( ) };
    uint32_t sameNodes{ mChildNodes.getSize() };
    uint32_t prioNode{ mPrioStates };

    mGrouping = static_cast<uint32_t>(ScopeNodeBase::Grouping::None);

    if ( sameNodes > 0 )
    {
        for ( NodeList::LISTPOS pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
        {
            const ScopeNode & node{ mChildNodes.valueAtPosition( pos ) };
            if ( (node.getPriority( ) != prioNode) || (node.childNodeCount( ) != 0) )
            {
                -- sameNodes;
                break;
            }
        }

        if ( sameNodes == mChildNodes.getSize( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Nodes);
            result += mChildNodes.getSize( );
            mChildNodes.clear( );

            result += removePriorityNodesRecursive( prioNode );
        }
        else
        {
            prioNode = 0;
        }
    }

    if ( sameLeafs > 0 )
    {
        for ( LeafList::LISTPOS pos = mChildLeafs.firstPosition( ); mChildLeafs.isValidPosition( pos ); pos = mChildLeafs.nextPosition( pos ) )
        {
            const ScopeLeaf & leaf{ mChildLeafs.valueAtPosition( pos ) };
            uint32_t prio = leaf.getPriority( );
            if (prio != prioNode )
            {
                -- sameLeafs;
                break;
            }
        }

        if (sameLeafs == mChildLeafs.getSize( ))
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Leaves);
            result += mChildLeafs.getSize( );
            mChildLeafs.clear( );
            result += removePriorityNodesRecursive( prioNode );
        }
    }

    return result;
}

uint32_t ScopeNode::updateConfigNode( ConfigManager & config, const String & parentPath ) const
{
    uint32_t result{ 0 };
    String thisScope = makeScopePath( parentPath );
    if ( (mGrouping & static_cast<uint32_t>(ScopeNodeBase::Grouping::All)) != 0 )
    {
        config.addModuleLogScope(makeConfigString(parentPath), mPrioStates);
        result = 1;
    }

    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        const ScopeNode & node = mChildNodes.valueAtPosition( pos );
        result += node.updateConfigNode( config, thisScope );
    }

    for ( auto pos = mChildLeafs.firstPosition( ); mChildLeafs.isValidPosition( pos ); pos = mChildLeafs.nextPosition( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.valueAtPosition( pos );
        result += leaf.updateConfigNode( config, thisScope );
    }

    return result;
}

uint32_t ScopeNode::groupRecursive()
{
    uint32_t result{ 0 };
    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        const ScopeNode& node{ mChildNodes.valueAtPosition(pos) };
        result += const_cast<ScopeNode &>(node).groupRecursive( );
    }

    result += groupChildNodes( );
    return result;
}

String ScopeNode::makeConfigString( const String & parent ) const
{
    char scope[NELogging::LOG_MESSAGE_IZE];
    uint32_t len = static_cast<uint32_t>(String::formatString(    scope, NELogging::LOG_MESSAGE_IZE
                                                                , "%s%s%c%c"
                                                                , parent.getString()
                                                                , mNodeName.getString()
                                                                , NELogOptions::SYNTAX_SCOPE_SEPARATOR
                                                                , NELogOptions::SYNTAX_SCOPE_GROUP));
    return String(scope, len);
}

uint32_t ScopeNode::removePriorityNodesRecursive( uint32_t prioRemove )
{
    uint32_t result{ 0 };

    if ( mChildLeafs.getSize( ) != 0 )
    {
        LeafList::LISTPOS pos = mChildLeafs.firstPosition( );
        while ( mChildLeafs.isValidPosition( pos ) )
        {
            const ScopeNodeBase& leaf{ mChildLeafs.valueAtPosition(pos) };
            if ( leaf.getPriority( ) == prioRemove )
            {
                pos = mChildLeafs.removeAt( pos );
                ++ result;
            }
            else
            {
                pos = mChildLeafs.nextPosition( pos );
            }
        }

        if ( mChildLeafs.isEmpty( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Leaves);
        }
    }

    if ( mChildNodes.getSize( ) != 0 )
    {
        NodeList::LISTPOS pos = mChildNodes.firstPosition( );
        while ( mChildNodes.isValidPosition( pos ) )
        {
            const ScopeNodeBase& node{ mChildNodes.valueAtPosition(pos) };
            if ( mChildNodes.valueAtPosition( pos ).getPriority( ) == prioRemove )
            {
                result += const_cast<ScopeNodeBase &>(node).removePriorityNodesRecursive( prioRemove );
                if (node.isEmpty() )
                {
                    pos = mChildNodes.removeAt( pos );
                    ++ result;
                }
            }
            else
            {
                const_cast<ScopeNodeBase&>(node).removePriorityNodesRecursive( prioRemove );
                pos = mChildNodes.nextPosition( pos );
            }
        }

        if ( mChildNodes.isEmpty( ) )
        {
            mGrouping |= static_cast<uint32_t>(ScopeNodeBase::Grouping::Nodes);
        }
    }

    return result;
}

bool ScopeNode::isEmpty() const
{
    return mChildLeafs.isEmpty() && mChildNodes.isEmpty();
}

//////////////////////////////////////////////////////////////////////////
// ScopeRoot class implementation
//////////////////////////////////////////////////////////////////////////

ScopeRoot::ScopeRoot()
    : ScopeNode     ( ScopeNodeBase::NodeType::Root, Process::getInstance().getAppName(), static_cast<uint32_t>(NELogging::LogPriority::PrioNotset) )
{
}

String ScopeRoot::makeScopePath( const String & /*prefix*/ ) const
{
    return String::EmptyString;
}

uint32_t ScopeRoot::updateConfigNode( ConfigManager & config, const String & /*parentPath*/ ) const
{
    uint32_t result{ 0 };

    String thisScope( makeScopePath(String::EmptyString) );
    for ( auto pos = mChildLeafs.firstPosition( ); mChildLeafs.isValidPosition( pos ); pos = mChildLeafs.nextPosition( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.valueAtPosition( pos );
        result += leaf.updateConfigNode(config, thisScope);
    }

    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        const ScopeNode & node = mChildNodes.valueAtPosition( pos );
        result += node.updateConfigNode(config, thisScope);
    }

    if ( result == 0 )
    {
        config.addModuleLogScope(makeConfigString(thisScope), mPrioStates);
        result = 1;
    }

    return result;
}

String ScopeRoot::makeConfigString(const String& parent) const
{
    return (parent + NELogOptions::SYNTAX_SCOPE_GROUP);
}

#endif // AREG_LOGS
