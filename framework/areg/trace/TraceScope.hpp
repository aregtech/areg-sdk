#ifndef AREG_TRACE_TRACESCOPE_HPP
#define AREG_TRACE_TRACESCOPE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/TraceScope.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Trace messaging scope object definition.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/NETrace.hpp"
#include "areg/base/String.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////////
// TraceScope class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   Tracing scope object is used to enable or disable logging messages.
 * 
 *          Before message output, the trace scopes should be defined.
 *          Depending on logging enable and priority flags of the scope,
 *          the system decides whether the message should be logged or not.
 *          Each created tracing scope is automatically registered in the system.
 *          Each tracing scope should have fixed unique name, which is used to
 *          generate scope ID. Neither name, nor scope ID can be changed. The scope
 *          priority can be changed during runtime and to enable dynamically certain
 *          logs and associated priorities.
 * 
 *          This makes logging dynamic. To see certain logs in output target,
 *          there is no need to stop and start application, it is enough
 *          to enable or disable certain logging priorities and the system
 *          automatically filters logs.
 * 
 *          The initial activation of scopes, enabling of priorities of scope or 
 *          group of scopes can be done in configuration file, which sets up the
 *          initial configuration of logging service and the scopes.
 * 
 * \note    In this version all logs are written as a text in the log file. At the
 *          moment, there is no tool to change scope priorities. So that, there is
 *          a need to restart application if need to filter scopes and this filtering
 *          should be done in configuration file. In the next versions there will be
 *          a tool to configure and view logs during runtime.
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
     * \brief   Initializes the trace scope object from the stream.
     *          Unlike the previous initializer, this trace scope object is not inserted into the map.
     * \param   stream  The streaming object that contains the information of name, ID and priority.
     **/
    TraceScope(const IEInStream & stream);

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

    /**
     * \brief   Writes the scope data into the stream.
     *          Note, the class does not have operator to deserialize
     *          the scope object, because of 2 reasons:
     *              1.  The default of TraceScope is deleted, so that there is
     *                  no possibility to create scope with empty name;
     *              2.  The name and the ID of the scope are declared as a constants
     *                  and cannot be changed, because they are inserted in the map
     *                  when they are created, the key of the TraceScope object in the
     *                  map is the ID and the location in the map is fixed.
     * \param   stream  The stream object to write data.
     * \param   output  The scope to write into the stream object.
     **/
    inline friend IEOutStream & operator << ( IEOutStream & stream, const TraceScope & output );

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
    inline void setPriority( const String & newPrio );

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
    inline void addPriority( const String & addPrio );

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
    inline void removePriority( const String & remPrio );

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
    inline const String & getScopeName( void ) const;

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
    TraceScope( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( TraceScope );
};

//////////////////////////////////////////////////////////////////////////
// Hasher of TraceScope class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the TraceScope.
 */
namespace std
{
    template<>
    struct hash<TraceScope>
    {
        //! A function to convert TraceScope object to unsigned int.
        inline unsigned int operator()(const TraceScope& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////////
// inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline IEOutStream & operator << ( IEOutStream & stream, const TraceScope & output )
{
    stream << output.mScopeName;
    stream << output.mScopeId;
    stream << output.mScopePrio;

    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// TraceScope class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline TraceScope & TraceScope::self( void )
{
    return (*this);
}

inline TraceScope::operator unsigned int ( void ) const
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

inline void TraceScope::setPriority( const String & newPrio )
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

inline void TraceScope::addPriority( const String & addPrio )
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

inline void TraceScope::removePriority( const String & remPrio )
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

inline const String & TraceScope::getScopeName( void ) const
{
    return mScopeName;
}

#endif  // AREG_TRACE_TRACESCOPE_HPP
