/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/ScopeNodes.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, The scope nodes classes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/ScopeNodes.hpp"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/base/FileBase.hpp"
#include "areg/base/Process.hpp"

//////////////////////////////////////////////////////////////////////////
// ScopeLeaf class implementation
//////////////////////////////////////////////////////////////////////////

ScopeLeaf::ScopeLeaf( void )
    : ScopeNodeBase ( ScopeNodeBase::eNode::Leaf )
{
}

ScopeLeaf::ScopeLeaf( const ScopeNodeBase & base )
    : ScopeNodeBase( ScopeNodeBase::eNode::Leaf, base.getNodeName(), base.getPriority() )
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

unsigned int ScopeLeaf::saveNodeConfig( FileBase & file, const String & parentPath ) const
{
    file.writeLine( makeConfigString( parentPath ) );
    return 1;
}

//////////////////////////////////////////////////////////////////////////
// ScopeNode class implementation
//////////////////////////////////////////////////////////////////////////

ScopeNode::ScopeNode( void )
    : ScopeNodeBase ( ScopeNodeBase::eNode::Node )
    , mChildNodes   ( true )
    , mChildLeafs   ( true )
{
}

ScopeNode::ScopeNode( const ScopeNodeBase & base )
    : ScopeNodeBase ( ScopeNodeBase::eNode::Node, base.getNodeName( ), base.getPriority() )
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

ScopeNode::ScopeNode( ScopeNodeBase::eNode nodeType, const String & name, unsigned int prio )
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

const ScopeNodeBase & ScopeNode::makeChildNode( String & IN OUT scopePath, unsigned int prioStates ) const
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
        scope = &(mChildLeafs.valueAtPosition( atPos.first ));
        newEntry = atPos.second;
    }
    else if (child.isNode( ))
    {
        auto atPos = mChildNodes.addIfUnique( ScopeNode( child ), false );
        scope = &(mChildNodes.valueAtPosition( atPos.first ));
        scope->addPriority(child.getPriority());
        newEntry = atPos.second;
    }

    if ( scope->isValid() )
    {
        mPrioStates |= child.getPriority( );
    }

    return std::pair<ScopeNodeBase &, bool>{*scope, newEntry};
}

std::pair<ScopeNodeBase &, bool>  ScopeNode::addChildNode( String & scopePath, unsigned int prioStates )
{
    const ScopeNodeBase & node = makeChildNode( scopePath, prioStates );
    return addChildNode( node );
}

String ScopeNode::makeScopePath( const String & prefix ) const
{
    String result{ prefix };
    ASSERT( mNodeName.isEmpty( ) == false );
    ASSERT( isValid( ) );

    result += mNodeName;
    result += NELogConfig::SYNTAX_SCOPE_SEPARATOR;
    return result;
}

unsigned int ScopeNode::groupChildNodes( void )
{
    unsigned int result{ 0 };
    unsigned int sameLeafs{ mChildLeafs.getSize( ) };
    unsigned int sameNodes{ mChildNodes.getSize() };
    unsigned int prioNode{ mPrioStates };

    mGroupping = static_cast<unsigned int>(ScopeNodeBase::eGroupping::NoGroupping);

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
            mGroupping |= static_cast<unsigned int>(ScopeNodeBase::eGroupping::GrouppingNodes);
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
            unsigned int prio = leaf.getPriority( );
            if (prio != prioNode )
            {
                -- sameLeafs;
                break;
            }
        }

        if (sameLeafs == mChildLeafs.getSize( ))
        {
            mGroupping |= static_cast<unsigned int>(ScopeNodeBase::eGroupping::GrouppingLeafes);
            result += mChildLeafs.getSize( );
            mChildLeafs.clear( );
            result += removePriorityNodesRecursive( prioNode );
        }
    }

    return result;
}

unsigned int ScopeNode::saveNodeConfig( FileBase & file, const String & parentPath ) const
{
    unsigned int result{ 0 };
    if ( hasLogsEneabled( ) == false )
    {
        return result;
    }

    String thisScope = makeScopePath( parentPath );
    if ( (mGroupping & static_cast<unsigned int>(ScopeNodeBase::eGroupping::GrouppingAll)) != 0 )
    {
        file.writeLine( makeConfigString(parentPath) );
        result = 1;
    }

    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        const ScopeNode & node = mChildNodes.valueAtPosition( pos );
        result += node.saveNodeConfig( file, thisScope );
    }

    for ( auto pos = mChildLeafs.firstPosition( ); mChildLeafs.isValidPosition( pos ); pos = mChildLeafs.nextPosition( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.valueAtPosition( pos );
        result += leaf.saveNodeConfig( file, thisScope );
    }

    return result;
}

unsigned int ScopeNode::groupRecursive( void )
{
    unsigned int result{ 0 };
    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        result += mChildNodes.valueAtPosition( pos ).groupRecursive( );
    }

    result += groupChildNodes( );
    return result;
}

String ScopeNode::makeConfigString( const String & parent ) const
{
    String result;
    result.format( "%s%s_* = %s", parent.getString( ), mNodeName.getString( ), makePrioString( ).getString( ) );
    return result;
}

unsigned int ScopeNode::removePriorityNodesRecursive( unsigned int prioRemove )
{
    unsigned int result{ 0 };

    if ( mChildLeafs.getSize( ) != 0 )
    {
        LeafList::LISTPOS pos = mChildLeafs.firstPosition( );
        while ( mChildLeafs.isValidPosition( pos ) )
        {
            ScopeNodeBase & leaf = mChildLeafs.valueAtPosition( pos );
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
            mGroupping |= static_cast<unsigned int>(ScopeNodeBase::eGroupping::GrouppingLeafes);
        }
    }

    if ( mChildNodes.getSize( ) != 0 )
    {
        NodeList::LISTPOS pos = mChildNodes.firstPosition( );
        while ( mChildNodes.isValidPosition( pos ) )
        {
            ScopeNodeBase & node = mChildNodes.valueAtPosition( pos );
            if ( mChildNodes.valueAtPosition( pos ).getPriority( ) == prioRemove )
            {
                result += node.removePriorityNodesRecursive( prioRemove );
                if (node.isEmpty() )
                {
                    pos = mChildNodes.removeAt( pos );
                    ++ result;
                }
            }
            else
            {
                node.removePriorityNodesRecursive( prioRemove );
                pos = mChildNodes.nextPosition( pos );
            }
        }

        if ( mChildNodes.isEmpty( ) )
        {
            mGroupping |= static_cast<unsigned int>(ScopeNodeBase::eGroupping::GrouppingNodes);
        }
    }

    return result;
}

bool ScopeNode::isEmpty( void ) const
{
    return mChildLeafs.isEmpty() && mChildNodes.isEmpty();
}

//////////////////////////////////////////////////////////////////////////
// ScopeRoot class implementation
//////////////////////////////////////////////////////////////////////////

ScopeRoot::ScopeRoot( void )
    : ScopeNode     ( ScopeNodeBase::eNode::Root, Process::getInstance().getAppName(), static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset) )
{
}

String ScopeRoot::makeScopePath( const String & /*prefix*/ ) const
{
    String result;
    return result.format( "%s.%s.", NELogConfig::SYNTAX_CMD_LOG_SCOPE.data( ), Process::getInstance( ).getAppName( ).getString( ) );
}

unsigned int ScopeRoot::saveNodeConfig( FileBase & file, const String & /*parentPath*/ ) const
{
    unsigned int result{ 0 };

    String thisScope( makeScopePath(String::EmptyString) );

    for ( auto pos = mChildLeafs.firstPosition( ); mChildLeafs.isValidPosition( pos ); pos = mChildLeafs.nextPosition( pos ) )
    {
        const ScopeLeaf & leaf = mChildLeafs.valueAtPosition( pos );
        if ( leaf.hasLogsEneabled( ) )
        {
            result += leaf.saveNodeConfig( file, thisScope );
        }
    }

    for ( auto pos = mChildNodes.firstPosition( ); mChildNodes.isValidPosition( pos ); pos = mChildNodes.nextPosition( pos ) )
    {
        const ScopeNode & node = mChildNodes.valueAtPosition( pos );
        if ( node.hasLogsEneabled( ) )
        {
            result += node.saveNodeConfig( file, thisScope );
        }
    }

    if ( result == 0 )
    {
        file.writeLine( makeConfigString( thisScope) );
        result = 1;
    }

    return result;
}

String ScopeRoot::makeConfigString( const String & parent ) const
{
    String result;
    return result.format( "%s* = %s", parent.getString(), makePrioString( ).getString() );
}
