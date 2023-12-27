#ifndef AREG_TRACE_PRIVATE_SCOPENODEBASE_HPP
#define AREG_TRACE_PRIVATE_SCOPENODEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/ScopeNodeBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, The base class of scope nodes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/String.hpp"
#include "areg/trace/NETrace.hpp"

 /************************************************************************
  * Dependencies.
  ************************************************************************/
class ConfigManager;
class TraceScope;

//////////////////////////////////////////////////////////////////////////
// ScopeNodeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This is a base class to create logging scope nodes.
 *          It is used not only to generate the nodes, but also group them by priorities.
 *          For example, when save current configuration state in the configuration file
 *          to read and initialize on next start, without grouping we should write the state
 *          of each scope and list could be huge. To avoid huge listing, the grouping helps
 *          to write the state of the scope groups. To achieve this, the scopes are split
 *          into the nodes (nodes and leafs), which are then grouped by having same priorities.
 * 
 *          NOTE:   This class is not thread safe and supposed to be used only by one thread at a time.
 *                  This and derived object are for internal used.
 **/
class ScopeNodeBase
{
//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The types of the node. Set when object is created and cannot be changed.
     **/
    enum class eNode    : unsigned char
    {
          Invalid   = 0 // The node is invalid,             bits: 0000 0000
        , Leaf      = 2 // The node is root, has no parent, bits: 0000 0010
        , Node      = 4 // The node that has children,      bits: 0000 0100
        , Root      = 12// The node does not have children, bits: 0000 1100
    };

    /**
     * \brief   The grouping flags to set bitwise.
     **/
    enum class eGroupping   : unsigned char
    {
          NoGroupping           = 0 // Bits: 0000 0000
        , GrouppingPartially    = 4 // Bits: 0000 0100
        , GrouppingLeafes       = 5 // Bits: 0000 0101
        , GrouppingNodes        = 6 // Bits: 0000 0110
        , GrouppingAll          = 7 // Bits: 0000 0111
    };

//////////////////////////////////////////////////////////////////////////
// Protected constructors and destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates and empty node with no priority.
     *          The type depends on the passed parameter and cannot be changed later.
     * \param   nodeType    The type of node to set when initialized. Cannot be changed.
     **/
    explicit ScopeNodeBase( ScopeNodeBase::eNode nodeType );

    /**
     * \brief   Creates a node and sets the node type, node name and priority flags.
     *          The type of the node cannot be changed anymore.
     * \param   nodeType    The type of the node. Cannot be changed anymore.
     * \param   nodeName    The name of the node.
     * \param   prio        The logging priority flags set bitwise.
     **/
    ScopeNodeBase( ScopeNodeBase::eNode nodeType, const String & nodeName, unsigned int prio = static_cast<unsigned int>(NETrace::eLogPriority::PrioNotset) );

    //!< Copies, moves and destroys the node object.
    ScopeNodeBase( const ScopeNodeBase & src );
    ScopeNodeBase( ScopeNodeBase && src ) noexcept;
    virtual ~ScopeNodeBase( void ) = default;

/************************************************************************
 * ScopeNodeBase statics
 ************************************************************************/

    /**
     * \brief   Extracts the node name from the passed name of the scope.
     *          Each node should be separated by '_' symbol. If a scope
     *          contains "__", then the node is create with the name
     *          containing symbol '_'. For example, the scope with name
     *          "areg_unittest__someTest_testFunction" is split into 4
     *          node names: "areg", "unittest", "_someTest" and "testFunction".
     *          On output, the 'scopeName' does not contain the name of
     *          extracted node name. In the first cycle of the example, the
     *          node name would be "unittest__someTest_testFunction".
     * \param   The name of the scope to extract node name.
     **/
    static String extractNodeName( String & scopeName );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Assignment operators
     **/
    ScopeNodeBase & operator = ( const ScopeNodeBase & src );
    ScopeNodeBase & operator = ( ScopeNodeBase && src ) noexcept;

    /**
     * \brief   Compare operators.
     **/
    bool operator == ( const ScopeNodeBase & other ) const;
    bool operator != ( const ScopeNodeBase & other ) const;
    bool operator > ( const ScopeNodeBase & other ) const;
    bool operator < ( const ScopeNodeBase & other ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * ScopeNodeBase override
 ************************************************************************/

    /**
     * \brief   Creates a child node out of scope name and sets the scope log priority flags.
     *          Whether it is a node or a leaf, depends on scope name. The last name in 
     *          the scope is a leaf. All others are nodes.
     *          On output, the 'scopePath' with contain the remaining path or an empty string
     *          if a leaf was created.
     * \param   stopePath   The path of the scope to create a node. On output, this contains
     *                      either the remaining path or empty string if a leaf was created.
     * \param   prioStates  Bitwise logging priority set to scope.
     * \return  Returns created node. It is either a node or a leaf.
     * \note    Only the root and nodes can create nodes or leafs. The leafs return invalid node.
     **/
    virtual const ScopeNodeBase & makeChildNode( String & IN OUT scopePath, unsigned int prioStates ) const;

    /**
     * \brief   Adds a child node to the node if it does not contain a child of the same type
     *          and same name. If it contains, only updates the log priority flag.
     *          Only root or nodes can contain child. This method is ignored in case of leaf.
     * \param   child   The child node to add or update.
     * \return  Returns a pair of new node entry in the child list and a boolean flag indicating whether
     *          it created new entry or updated the existing.
     **/
    virtual std::pair<ScopeNodeBase &, bool> addChildNode( const ScopeNodeBase & child );

    /**
     * \brief   Creates a new node out of passed scope name and specified log priority.
     *          If there is node of the same type and same name, it updates the log priority.
     *          Otherwise, creates and inserts new entry. On output, the parameter 'scopePath'
     *          contains the remaining part of the scope after creating child node. It is empty
     *          when creates and adds a leaf.
     *          Only root or nodes can contain child. This method is ignored in case of leaf.
     * \param   scopePath       The name of the scope to generate a node. On output, this contains
     *                          the remaining part of the scope name.
     * \param   priorityState   The logging priority flags of the scope.
     * \return  Returns a pair of new node entry in the child list and a boolean flag indicating whether
     *          it created new entry or updated the existing.
     **/
    virtual std::pair<ScopeNodeBase &, bool> addChildNode( String & scopePath, unsigned int prioStates );

    /**
     * \brief   Creates a scope name (scope path) by adding the prefix to the node name.
     *          If the node is a leaf, it just adds the name. If it is a node, it adds
     *          the name and '_' at the end to indicate the node.
     * \param   prefix  The prefix to add to the name. In case of root, it is an empty
     *                  sting. In case of nodes, it is a path generated by parent nodes.
     **/
    virtual String makeScopePath( const String & prefix ) const;

    /**
     * \brief   Generates the scope configuration string with the logging
     *          priority setting to save in the file.
     * \param   parent  The parent scope name (or prefix) to add to generated name.
     * \return  Returns generated string, which contains the logging priority setting.
     **/
    virtual String makeConfigString( const String & parent ) const;

    /**
     * \brief   Groups and removes child nodes if they have same log priority settings.
     *          It is grouping separately leafs and nodes. If leafs are grouped it as well
     *          removes all nodes with the same logging priority. If nodes are grouped, it
     *          removes all leafs with the same logging priority. When grouping, adds
     *          appropriate grouping flag, so that it generates the 'scope configuration setting'.
     *          Only the root and nodes can group child entries.
     * \return  Returns number of nodes that where removed.
     **/
    virtual unsigned int groupChildNodes( void );

    /**
     * \brief   Generates the scope setting and updates in the configuration.
     *          -   If the entry is a leaf, it generates the full scope name to save in configuration.
     *          -   If the entry is a node and it has a grouping flag, it generates the scope group to save in configuration.
     *          -   If the entry is the root and has no child entries, it generates application scope group entry to save in configuration.
     * \param   config      The instance of ConfigManager to set the scope and priorities to save.
     * \param   parentPath  The path name of the parent node. The nodes contain '_' at the end.
     * \return  Returns the number of nodes that have been saved.
     **/
    virtual unsigned int updateConfigNode(ConfigManager& config, const String & parentPath ) const;

    /**
     * \brief   Recursively groups child nodes. The grouping starts from the last node in the
     *          node tree to group leafs until the node that first triggered the method.
     *          Normally,the grouping starts from root node.
     * \return  Returns the number of nodes that it totally grouped.
     **/
    virtual unsigned int groupRecursive( void );

    /**
     * \brief   Removes the priority nodes of specified log priority.
     *          It loops all child nodes and removes all entries that match
     *          the priority.
     * \return  Returns the number of nodes that are removed.
     **/
    virtual unsigned int removePriorityNodesRecursive( unsigned int prioRemove );

    /**
     * \brief   Returns true if a node has no children nodes.
     **/
    virtual bool isEmpty( void ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns node name.
     **/
    inline const String & getNodeName( void ) const;

    /**
     * \brief   Sets node name.
     **/
    inline void setNodeName( const String newName );

    /**
     * \brief   Returns the node priority flag.
     **/
    inline unsigned int getPriority( void ) const;

    /**
     * \brief   Sets the node priority flag.
     **/
    inline void setPriority( unsigned int prio );

    /**
     * \brief   Adds log priority bits.
     **/
    inline void addPriority( unsigned int prio );

    /**
     * \brief   Returns true if the object is the root. The root does not have a parent
     **/
    inline bool isRoot( void ) const;

    /**
     * \brief   Returns true if the object is a node. Nodes have a parent, it may have child leafs and child nodes.
     **/
    inline bool isNode( void ) const;

    /**
     * \brief   Returns true if the object is a leaf. Leafs have parent, but cannot have children nodes.
     **/
    inline bool isLeaf( void ) const;

    /**
     * \brief   Returns true if the node is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns true if the logging priority has debug priority bit set.
     **/
    inline bool hasPrioDebug( void ) const;

    /**
     * \brief   Returns true if the logging priority has info priority bit set.
     **/
    inline bool hasPrioInfo( void ) const;

    /**
     * \brief   Returns true if the logging priority has warning priority bit set.
     **/
    inline bool hasPrioWarning( void ) const;

    /**
     * \brief   Returns true if the logging priority has error priority bit set.
     **/
    inline bool hasPrioError( void ) const;

    /**
     * \brief   Returns true if the logging priority has fatal error priority bit set.
     **/
    inline bool hasPrioFatal( void ) const;

    /**
     * \brief   Returns true if any logging priority bit is set.
     **/
    inline bool hasLogsEneabled( void ) const;

    /**
     * \brief   Returns true if the logging scopes priority bit set.
     **/
    inline bool hasLogScopes( void ) const;

    /**
     * \brief   Recursively creates a child node out of scopePath and adds in the list.
     *          The recursion continues until leaf. The new entry is added only if it is
     *          not existing in the child list.
     * \param   scopePath   The path name of the scope to generate and add child nodes.
     * \param   prioState   The log priority bitwise flag to add to nodes.
     * \return  Returns the number of nodes created or updated.
     **/
    unsigned int addChildRecursive( String & scopePath, unsigned int prioStates );

    /**
     * \brief   Recursively creates a child node out of Scope object, which contains
     *          the scope name and the scope priority, and adds in the list.
     *          The recursion continues until leaf. The new entry is added only if it is
     *          not existing in the child list.
     * \param   traceScope  The logging scope, which contains the scope name and the log priority.
     * \param   prioState   The log priority bitwise flag to add to nodes.
     * \return  Returns the number of nodes created or updated.
     **/
    unsigned int addChildRecursive( const TraceScope & traceScope );

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns invalid node object.
     **/
    static ScopeNodeBase & invalidNode( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The type of the node. Cannot be changed.
    const ScopeNodeBase::eNode  mNodeType;
    
    //!< The priority flags set bitwise.
    unsigned int                mPrioStates;

    //!< The name of the node.
    String                      mNodeName;

    //!< Bitwise flag, indicating the groupings of nodes.
    unsigned int                mGroupping;

//////////////////////////////////////////////////////////////////////////
// Hidden, only for internal use
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is inaccessible. Used only internally
     **/
    ScopeNodeBase( void );
};

//////////////////////////////////////////////////////////////////////////
// ScopeNodeBase inline methods
//////////////////////////////////////////////////////////////////////////

inline const String & ScopeNodeBase::getNodeName( void ) const
{
    return mNodeName;
}

inline void ScopeNodeBase::setNodeName( const String newName )
{
    mNodeName = newName;
}

inline unsigned int ScopeNodeBase::getPriority( void ) const
{
    return mPrioStates;
}

inline void ScopeNodeBase::setPriority( unsigned int prio )
{
    mPrioStates = prio;
}

inline void ScopeNodeBase::addPriority( unsigned int prio )
{
    mPrioStates |= prio;
}

inline bool ScopeNodeBase::isRoot( void ) const
{
    return (mNodeType == ScopeNodeBase::eNode::Root);
}

inline bool ScopeNodeBase::isNode( void ) const
{
    return (mNodeType == ScopeNodeBase::eNode::Node);
}

inline bool ScopeNodeBase::isLeaf( void ) const
{
    return (mNodeType == ScopeNodeBase::eNode::Leaf);
}

inline bool ScopeNodeBase::isValid( void ) const
{
    return (mNodeType != ScopeNodeBase::eNode::Invalid);
}

inline bool ScopeNodeBase::hasPrioDebug( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioDebug)) != 0;
}

inline bool ScopeNodeBase::hasPrioInfo( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioInfo)) != 0;
}

inline bool ScopeNodeBase::hasPrioWarning( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioWarning)) != 0;
}

inline bool ScopeNodeBase::hasPrioError( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioError)) != 0;
}

inline bool ScopeNodeBase::hasPrioFatal( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioFatal)) != 0;
}

inline bool ScopeNodeBase::hasLogsEneabled( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioLogs)) != 0;
}

inline bool ScopeNodeBase::hasLogScopes( void ) const
{
    return (mPrioStates & static_cast<uint32_t>(NETrace::eLogPriority::PrioScope)) != 0;
}

#endif  // AREG_TRACE_PRIVATE_SCOPENODEBASE_HPP
