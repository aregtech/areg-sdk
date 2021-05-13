#ifndef AREG_TRACE_TRACESCOPE_HPP
#define AREG_TRACE_TRACESCOPE_HPP
/************************************************************************
 * \file        areg/trace/TraceScope.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace messaging scope object definition.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////////
// TraceScope class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   Tracing scope object is used to enable or disable logging messages.
 *          Before message output, the trace scopes should be defined.
 *          Depending on logging enabling and priority bit flags in scope,
 *          the system decides whether the message should be logged or not.
 *          Each created tracing scope is automatically registered in the system.
 *          Each tracing scope should have unique name. Out of name, it is generating
 *          scope ID. Neither name, nor scope ID can be changed anymore.
 *          Instead, the scope priority can be changed during runtime and
 *          by this the certain logs can be dynamically enabled, disabled or filtered.
 *          This approach makes logging dynamic. To see certain logs in output target,
 *          there is no more need to stop and start application, it is enough
 *          to enable or disable certain logging priorities and the system
 *          will automatically start logging message for enabled priorities.
 *          The activation of scopes and enabling of certain priorities to
 *          certain scope or group of scopes is possible to do via configuration file.
 *          These scope are automatically enabled or disable during each system start.
 *          By changing logging configuration, changes are automatically applied to
 *          application after next start.
 **/
class AREG_API TraceScope
{
//////////////////////////////////////////////////////////////////////////////
// friend class declaration to access internals
//////////////////////////////////////////////////////////////////////////////
    friend class TraceMessage;

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the name of scope and sets message priority. Out of given scope name,
     *          from it creates the scope ID. By default, the message priority of 
     *          trace scope is NETrace::PrioNotset, which means the logging of message is disabled.
     *          When the trace scope is created, it is automatically registered in the system.
     *          Neither trace scope name, nor ID cannot be changed anymore. Instead, the scope
     *          logging priority is possible to change during runtime.
     * \param   scopeName   The name of trace scope, which should be unique within process.
     *                      From given name it creates scope ID.
     * \param   priority    The logging message priority of trace scope
     **/
    TraceScope( const char * scopeName, NETrace::eLogPriority priority = NETrace::PrioNotset );

    /**
     * \brief   Destructor.
     **/
    ~TraceScope( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns 32-bit integer value of scope, which
     *          is same as scope ID.
     **/
    inline operator unsigned int ( void ) const;

//////////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets the message priority of scope by given priority value.
     *          It will reset old value of scope priority
     * \param   newPrio     The value of message priority to set
     **/
    inline void setPriority( unsigned int newPrio );

    /**
     * \brief   Sets the message priority of scope by given name of priority.
     *          It will reset old value of scope priority
     * \param   newPrio     The name of message priority to set
     **/
    inline void setPriority( const char * newPrio );

    /**
     * \brief   Adds the message priority of scope by given value of priority.
     *          If scope already has specified priority, no priority value will be changed.
     * \param   addPrio     The value of message priority to add
     **/
    inline void addPriority( NETrace::eLogPriority addPrio );

    /**
     * \brief   Adds the message priority of scope by given name of priority.
     *          If scope already has specified priority, no priority value will be changed.
     * \param   addPrio     The name of message priority to add
     **/
    inline void addPriority( const char * addPrio );

    /**
     * \brief   Removes the message priority of scope by given value of priority.
     *          If scope does not have specified priority, no priority value will be changed.
     * \param   remPrio     The value of message priority to remove
     **/
    inline void removePriority( NETrace::eLogPriority remPrio );

    /**
     * \brief   Removes the message priority of scope by given name of priority.
     *          If scope does not have specified priority, no priority value will be changed.
     * \param   remPrio     The name of message priority to remove
     **/
    inline void removePriority( const char * remPrio );

    /**
     * \brief   Returns value of trace scope message priority
     **/
    inline unsigned int getPriority( void ) const;

    /**
     * \brief   Returns the ID of trace scope.
     *          The ID of trace scope is set from scope name and it cannot be changed
     **/
    inline unsigned int getScopeId( void ) const;

    /**
     * \brief   Returns name of trace scope. The name cannot be changed
     **/
    inline const char * getScopeName( void ) const;

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The name of trace scope. It cannot be changed
     **/
    const String        mScopeName;
    /**
     * \brief   The ID of trace scope. It cannot be changed
     **/
    const unsigned int  mScopeId;
    /**
     * \brief   The trace message priority of scope.
     **/
    unsigned int        mScopePrio;

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns TraceScope object
     **/
    inline TraceScope & self( void );

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    TraceScope( void );
    TraceScope( const TraceScope & /*src*/ );
    const TraceScope & operator = ( const TraceScope & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////////
// TraceScope class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline TraceScope & TraceScope::self( void )
{
    return (*this);
}

inline TraceScope::operator unsigned int( void ) const
{
    return mScopeId;
}

inline void TraceScope::setPriority( unsigned int newPrio )
{
    mScopePrio  = newPrio;
}

inline void TraceScope::setPriority( const char * newPrio )
{
    setPriority( NETrace::convFromString(newPrio) );
}

inline void TraceScope::addPriority( NETrace::eLogPriority addPrio )
{
    mScopePrio  |= static_cast<unsigned int>(addPrio);
}

inline void TraceScope::addPriority( const char * addPrio )
{
    addPriority( NETrace::convFromString(addPrio) );
}

inline void TraceScope::removePriority( NETrace::eLogPriority remPrio )
{
    mScopePrio  &= ~static_cast<unsigned int>(remPrio);
}

inline void TraceScope::removePriority( const char * remPrio )
{
    removePriority( NETrace::convFromString(remPrio) );
}

inline unsigned int TraceScope::getPriority( void ) const
{
    return mScopePrio;
}

inline unsigned int TraceScope::getScopeId( void ) const
{
    return mScopeId;
}

inline const char * TraceScope::getScopeName( void ) const
{
    return mScopeName.getString();
}

#endif  // AREG_TRACE_TRASCOPE_HPP
