#ifndef AREG_LOGGING_LOGSCOPE_HPP
#define AREG_LOGGING_LOGSCOPE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/LogScope.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log scope object definition.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/IOStream.hpp"
#include <atomic>

//////////////////////////////////////////////////////////////////////////////
// LogScope class declaration
//////////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Log Scope class is used to control the activation and priority of log messages.
 *
 *          Before logging any messages, log scopes must be defined. Each log scope
 *          is automatically registered in the system and has a unique, immutable name.
 *          A log scope has an activation state and a log priority level, which can be
 *          modified at runtime. These attributes filter messages locally on the host
 *          before they are sent to the log collector, allowing for dynamic logging control.
 **/
class AREG_API LogScope
{
//////////////////////////////////////////////////////////////////////////////
// friend class declaration to access internals
//////////////////////////////////////////////////////////////////////////////
    friend class ScopeMessage;
//////////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////////
    using session   = std::atomic_uint32_t;

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the name of scope, which should be unique and cannot be changed,
     *          and sets message priority. By default, the message priority areg::PrioNotset,
     *          which means the logging of message is disabled.
     * \param   scopeName   The unique name of the log scope.
     * \param   priority    The message priority of log scope.
     **/
    LogScope( const char * scopeName, areg::LogPriority priority = areg::LogPriority::PrioNotset );

    /**
     * \brief   Initializes the logging scope object from the stream.
     * \param   stream  The streaming object that contains the scope attributes.
     **/
    LogScope(const areg::InStream & stream);

    /**
     * \brief   Destructor.
     **/
    ~LogScope();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns 32-bit integer value of the scope.
     **/
    inline operator uint32_t () const;

    /**
     * \brief   Writes the scope data into the stream.
     * \note    The log scope class does not have operator to deserialize
     *          the scope object, because of 2 reasons:
     *              1.  LogScope does not have default constructor. Because it
     *                  sets empty name, which is not unique;
     *              2.  The name of the log scope cannot be changed.
     * \param   stream  The stream object to write data.
     * \param   output  The scope to write into the stream object.
     **/
    inline friend areg::OutStream & operator << ( areg::OutStream & stream, const LogScope & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets scope log message priority level by integer value.
     * \param   newPrio     Scope log message priority level to set.
     **/
    inline void setPriority( uint32_t newPrio );

    /**
     * \brief   Sets scope log message priority level by name.
     * \param   newPrio     The name of the log priority level to add.
     **/
    inline void setPriority( const char * newPrio );
    inline void setPriority( const areg::String & newPrio );

    /**
     * \brief   Adds priority level to the existing priority level of the scope.
     *          The priority level is added bitwise.
     * \param   addPrio     The log message priority level to add.
     **/
    inline void addPriority( areg::LogPriority addPrio );

    /**
     * \brief   Adds priority level to the existing priority level of the scope,
     *          where the priority level is added by name.
     *          The priority level is added bitwise.
     * \param   addPrio     The name of the log priority level to add.
     **/
    inline void addPriority( const char * addPrio );
    inline void addPriority( const areg::String & addPrio );

    /**
     * \brief   Removes priority level from the existing priority level of the scope.
     *          The operation is made bitwise.
     * \param   remPrio     The log priority level to remove.
     **/
    inline void removePriority( areg::LogPriority remPrio );

    /**
     * \brief   Removes priority level from the existing priority level of the scope,
     *          where the priority level is passed by name.
     *          The operation is made bitwise.
     * \param   remPrio     The name of the log priority level to remove.
     **/
    inline void removePriority( const char * remPrio );
    inline void removePriority( const areg::String & remPrio );

    /**
     * \brief   Returns the value of log message priority.
     **/
    inline uint32_t getPriority() const;

    /**
     * \brief   Returns the ID of log scope.
     **/
    inline uint32_t getScopeId() const;

    /**
     * \brief   Returns the name of the log scope.
     **/
    inline const areg::String & getScopeName() const;

    /**
     * \brief   Returns the session ID of the log scope, used to identify the scope in the session.
     **/
    inline uint32_t getSessionId() const;

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The ID of log scope. It cannot be changed
     **/
    const uint32_t  mScopeId;
    /**
     * \brief   The log message priority of the scope.
     **/
    uint32_t        mScopePrio;
    /**
     * \brief   The name of log scope. It cannot be changed
     **/
    const areg::String        mScopeName;
    /**
     * \brief   The log scope is active or not.
     **/
     const bool         mIsRegistered;
     /**
      * \brief   The session ID of the log scope, used to identify the scope in the session.
      **/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
     mutable session    mSessionId;
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Increases the session ID and returns the value to use in log messages.
     **/
    inline uint32_t nextSession() const;
    /**
     * \brief   Returns LogScope object
     **/
    inline LogScope & self();

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    LogScope() = delete;
    AREG_NOCOPY_NOMOVE( LogScope );
};

//////////////////////////////////////////////////////////////////////////
// Hasher of LogScope class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the LogScope.
 */
namespace std
{
    template<>
    struct hash<LogScope>
    {
        //! A function to convert LogScope object to uint32_t.
        inline uint32_t operator()(const LogScope& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////////
// inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline areg::OutStream & operator << ( areg::OutStream & stream, const LogScope & output )
{
    stream << output.mScopeId;
    stream << output.mScopePrio;
    stream << output.mScopeName;

    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// LogScope class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline uint32_t LogScope::nextSession() const
{
    return mSessionId.fetch_add(1);
}

inline LogScope & LogScope::self()
{
    return (*this);
}

inline LogScope::operator uint32_t () const
{
    return mScopeId;
}

inline void LogScope::setPriority( uint32_t newPrio )
{
    mScopePrio  = newPrio;
}

inline void LogScope::addPriority( areg::LogPriority addPrio )
{
    mScopePrio  |= static_cast<uint32_t>(addPrio);
}

void LogScope::addPriority( const char * addPrio )
{
    addPriority( areg::stringToLogPrio(addPrio) );
}

void LogScope::addPriority( const areg::String & addPrio )
{
    addPriority( areg::stringToLogPrio(addPrio) );
}

inline void LogScope::removePriority( areg::LogPriority remPrio )
{
    mScopePrio  &= ~static_cast<uint32_t>(remPrio);
}

void LogScope::removePriority( const char * remPrio )
{
    removePriority( areg::stringToLogPrio(remPrio) );
}

void LogScope::removePriority( const areg::String & remPrio )
{
    removePriority( areg::stringToLogPrio(remPrio) );
}

inline uint32_t LogScope::getPriority() const
{
    return mScopePrio;
}

inline uint32_t LogScope::getScopeId() const
{
    return mScopeId;
}

inline const areg::String & LogScope::getScopeName() const
{
    return mScopeName;
}

inline uint32_t LogScope::getSessionId() const
{
    return mSessionId;
}

#endif  // AREG_LOGGING_LOGSCOPE_HPP
