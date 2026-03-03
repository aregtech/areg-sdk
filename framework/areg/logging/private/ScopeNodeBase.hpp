#ifndef AREG_LOGGING_PRIVATE_SCOPENODEBASE_HPP
#define AREG_LOGGING_PRIVATE_SCOPENODEBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/ScopeNodeBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, The base class of scope nodes.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/String.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if AREG_LOGGING

 /************************************************************************
  * Dependencies.
  ************************************************************************/
namespace areg {

    class ConfigManager;
    class LogScope;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ScopeNodeBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for hierarchical scope nodes used to organize and manage logging priorities.
 *          Supports grouping by priority level and persistence to configuration files. Not
 *          thread-safe; intended for single-threaded use.
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
    enum class NodeType : uint8_t
    {
          Invalid   = 0 // The node is invalid,             bits: 0000 0000
        , Leaf      = 2 // The node is root, has no parent, bits: 0000 0010
        , Node      = 4 // The node that has children,      bits: 0000 0100
        , Root      = 12// The node does not have children, bits: 0000 1100
    };

    /**
     * \brief   The grouping flags to set bitwise.
     **/
    enum class Grouping   : uint8_t
    {
          None      = 0 // Bits: 0000 0000
        , Partial   = 4 // Bits: 0000 0100
        , Leaves    = 5 // Bits: 0000 0101
        , Nodes     = 6 // Bits: 0000 0110
        , All       = 7 // Bits: 0000 0111
    };

//////////////////////////////////////////////////////////////////////////
// Protected constructors and destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes an empty node with no priority.
     *
     * \param   nodeType    The type of node to set when initialized. Cannot be changed.
     **/
    explicit ScopeNodeBase( ScopeNodeBase::NodeType nodeType );

    /**
     * \brief   Initializes a node with type, name, and priority flags.
     *
     * \param   nodeType    The type of the node. Cannot be changed after initialization.
     * \param   nodeName    The name of the node.
     * \param   prio        The logging priority flags set bitwise.
     **/
    ScopeNodeBase( ScopeNodeBase::NodeType nodeType, const String & nodeName, uint32_t prio = static_cast<uint32_t>(areg::LogPriority::PrioNotset) );

    //!< Copies, moves and destroys the node object.
    /**
     * \brief   Copy constructor.
     **/
    ScopeNodeBase( const ScopeNodeBase & src );
    /**
     * \brief   Move constructor.
     **/
    ScopeNodeBase( ScopeNodeBase && src ) noexcept;
    virtual ~ScopeNodeBase() = default;

/************************************************************************
 * ScopeNodeBase statics
 ************************************************************************/

    /**
     * \brief   Extracts the next node name from a scope path. Each node is separated by '_';
     *          consecutive underscores ('__') preserve the underscore in the node name. On return,
     *          scopeName contains the remaining path after extraction.
     *
     * \param[in,out] scopeName       The scope name to extract from; contains the remaining path
     *                                after extraction.
     * \return  The extracted node name.
     **/
    static String extract_node_name( String & scopeName );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Copy assignment operator.
     **/
    ScopeNodeBase & operator = ( const ScopeNodeBase & src );
    /**
     * \brief   Move assignment operator.
     **/
    ScopeNodeBase & operator = ( ScopeNodeBase && src ) noexcept;

    /**
     * \brief   Returns true if nodes are equal.
     **/
    bool operator == ( const ScopeNodeBase & other ) const;
    /**
     * \brief   Returns true if nodes are not equal.
     **/
    bool operator != ( const ScopeNodeBase & other ) const;
    /**
     * \brief   Returns true if this node is greater than the other.
     **/
    bool operator > ( const ScopeNodeBase & other ) const;
    /**
     * \brief   Returns true if this node is less than the other.
     **/
    bool operator < ( const ScopeNodeBase & other ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************
 * ScopeNodeBase override
 ************************************************************************/

    /**
     * \brief   Creates a child node from a scope path and sets priority flags. The remaining path
     *          is returned in scopePath; empty if a leaf was created.
     *
     * \param[in,out] scopePath       The scope path to process; contains the remaining path after
     *                                creating a child node.
     * \param   prioStates      Bitwise logging priority flags to apply.
     * \return  The created child node (either a node or leaf).
     * \note    Only root and nodes can create children; leafs return invalid node.
     **/
    virtual const ScopeNodeBase & make_child_node( String & scopePath, uint32_t prioStates ) const;

    /**
     * \brief   Adds or updates a child node. Returns the node and a flag indicating whether it was
     *          created (true) or updated (false).
     *
     * \param   child       The child node to add or update.
     * \return  A pair containing the child node and a boolean: true if created, false if updated.
     **/
    virtual std::pair<ScopeNodeBase &, bool> add_child_node( const ScopeNodeBase & child );

    /**
     * \brief   Creates and adds a child node from a scope path, or updates an existing child.
     *          Returns the node and a flag indicating whether it was created (true) or updated
     *          (false).
     *
     * \param[in,out] scopePath       The scope path to process; contains the remaining path after
     *                                operation.
     * \param   prioStates      Logging priority flags to apply.
     * \return  A pair containing the child node and a boolean: true if created, false if updated.
     **/
    virtual std::pair<ScopeNodeBase &, bool> add_child_node( String & scopePath, uint32_t prioStates );

    /**
     * \brief   Generates a scope path by combining the prefix with the node name, appending '_' for
     *          non-leaf nodes.
     *
     * \param   prefix      The prefix to prepend; empty for root, parent-generated path for nodes.
     * \return  The generated scope path.
     **/
    virtual String make_scope_path( const String & prefix ) const;

    /**
     * \brief   Generates a scope configuration string with priority settings for persistence.
     *
     * \param   parent      The parent scope name (prefix) to include in the generated string.
     * \return  The generated configuration string with priority settings.
     **/
    virtual String make_config_string( const String & parent ) const;

    /**
     * \brief   Groups and removes child nodes with identical priority flags, treating leafs and
     *          nodes separately.
     *
     * \return  The number of nodes removed.
     **/
    virtual uint32_t group_child_nodes();

    /**
     * \brief   Saves scope priority settings to configuration. Saves leaf nodes as individual
     *          entries and grouped nodes as scope groups.
     *
     * \param   config          The ConfigManager instance to update with scope settings.
     * \param   parentPath      The parent scope path; nodes end with '_'.
     * \return  The number of nodes saved.
     **/
    virtual uint32_t update_config_node(ConfigManager& config, const String & parentPath ) const;

    /**
     * \brief   Recursively groups child nodes from leaf to root.
     *
     * \return  The total number of nodes grouped.
     **/
    virtual uint32_t group_recursive();

    /**
     * \brief   Recursively removes child nodes matching the specified priority.
     *
     * \return  The number of nodes removed.
     **/
    virtual uint32_t remove_priority_nodes( uint32_t prioRemove );

    /**
     * \brief   Returns true if the node has no children.
     **/
    virtual bool is_empty() const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the node's name.
     **/
    inline const String & node_name() const;

    /**
     * \brief   Sets the node's name.
     **/
    inline void set_node_name( const String newName );

    /**
     * \brief   Returns the node's priority flags.
     **/
    inline uint32_t priority() const;

    /**
     * \brief   Sets the node's priority flags.
     **/
    inline void set_priority( uint32_t prio );

    /**
     * \brief   Adds priority bits to the node.
     **/
    inline void add_priority( uint32_t prio );

    /**
     * \brief   Returns true if this is the root node.
     **/
    inline bool is_root() const;

    /**
     * \brief   Returns true if this is a node (has parent and may have children).
     **/
    inline bool is_node() const;

    /**
     * \brief   Returns true if this is a leaf node (has parent but no children).
     **/
    inline bool is_leaf() const;

    /**
     * \brief   Returns true if the node is valid.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns true if the Debug priority bit is set.
     **/
    inline bool has_prio_debug() const;

    /**
     * \brief   Returns true if the Information priority bit is set.
     **/
    inline bool has_prio_info() const;

    /**
     * \brief   Returns true if the Warning priority bit is set.
     **/
    inline bool has_prio_warning() const;

    /**
     * \brief   Returns true if the Error priority bit is set.
     **/
    inline bool has_prio_error() const;

    /**
     * \brief   Returns true if the Fatal Error priority bit is set.
     **/
    inline bool has_prio_fatal() const;

    /**
     * \brief   Returns true if any logging priority bit is set.
     **/
    inline bool has_logs_eneabled() const;

    /**
     * \brief   Returns true if the logging scopes priority bit is set.
     **/
    inline bool has_log_scopes() const;

    /**
     * \brief   Recursively creates and adds child nodes from a scope path until a leaf is created.
     *
     * \param[in,out] scopePath       The scope path to process; consumed as nodes are created.
     * \param   prioStates      Logging priority flags to apply to created nodes.
     * \return  The number of nodes created or updated.
     **/
    uint32_t add_child_recursive( String & scopePath, uint32_t prioStates );

    /**
     * \brief   Recursively creates and adds child nodes from a LogScope until a leaf is created.
     *
     * \param   logScope    The LogScope containing name and priority to create nodes from.
     * \return  The number of nodes created or updated.
     **/
    uint32_t add_child_recursive( const LogScope & logScope );

//////////////////////////////////////////////////////////////////////////
// Protected members
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Returns the invalid node singleton.
     **/
    static ScopeNodeBase & invalid_node();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The type of the node. Cannot be changed.
    const ScopeNodeBase::NodeType  mNodeType;
    
    //!< The priority flags set bitwise.
    uint32_t                mPrioStates;

    //!< The name of the node.
    String                      mNodeName;

    //!< Bitwise flag, indicating the groupings of nodes.
    uint32_t                mGrouping;

//////////////////////////////////////////////////////////////////////////
// Hidden, only for internal use
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is inaccessible; for internal use only.
     **/
    ScopeNodeBase();
};

//////////////////////////////////////////////////////////////////////////
// ScopeNodeBase inline methods
//////////////////////////////////////////////////////////////////////////

inline const String & ScopeNodeBase::node_name() const
{
    return mNodeName;
}

inline void ScopeNodeBase::set_node_name( const String newName )
{
    mNodeName = newName;
}

inline uint32_t ScopeNodeBase::priority() const
{
    return mPrioStates;
}

inline void ScopeNodeBase::set_priority( uint32_t prio )
{
    mPrioStates = prio;
}

inline void ScopeNodeBase::add_priority( uint32_t prio )
{
    mPrioStates |= prio;
}

inline bool ScopeNodeBase::is_root() const
{
    return (mNodeType == ScopeNodeBase::NodeType::Root);
}

inline bool ScopeNodeBase::is_node() const
{
    return (mNodeType == ScopeNodeBase::NodeType::Node);
}

inline bool ScopeNodeBase::is_leaf() const
{
    return (mNodeType == ScopeNodeBase::NodeType::Leaf);
}

inline bool ScopeNodeBase::is_valid() const
{
    return (mNodeType != ScopeNodeBase::NodeType::Invalid);
}

inline bool ScopeNodeBase::has_prio_debug() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioDebug)) != 0;
}

inline bool ScopeNodeBase::has_prio_info() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioInfo)) != 0;
}

inline bool ScopeNodeBase::has_prio_warning() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioWarning)) != 0;
}

inline bool ScopeNodeBase::has_prio_error() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioError)) != 0;
}

inline bool ScopeNodeBase::has_prio_fatal() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioFatal)) != 0;
}

inline bool ScopeNodeBase::has_logs_eneabled() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioLogs)) != 0;
}

inline bool ScopeNodeBase::has_log_scopes() const
{
    return (mPrioStates & static_cast<uint32_t>(areg::LogPriority::PrioScope)) != 0;
}

} // namespace areg

#endif  // AREG_LOGGING
#endif  // AREG_LOGGING_PRIVATE_SCOPENODEBASE_HPP
