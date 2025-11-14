#ifndef AREG_LOGGING_LOGSCOPE_HPP
#define AREG_LOGGING_LOGSCOPE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/LogScope.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log scope object definition.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/logging/NELogging.hpp"
#include "areg/base/String.hpp"
#include "areg/base/IEIOStream.hpp"
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
     *          and sets message priority. By default, the message priority NELogging::PrioNotset,
     *          which means the logging of message is disabled.
     * \param   scopeName   The unique name of the log scope.
     * \param   priority    The message priority of log scope.
     **/
    LogScope( const char * scopeName, NELogging::eLogPriority priority = NELogging::eLogPriority::PrioNotset );

    /**
     * \brief   Initializes the logging scope object from the stream.
     * \param   stream  The streaming object that contains the scope attributes.
     **/
    LogScope(const IEInStream & stream);

    /**
     * \brief   Destructor.
     **/
    ~LogScope( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns 32-bit integer value of the scope.
     **/
    inline operator unsigned int ( void ) const;

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
    inline friend IEOutStream & operator << ( IEOutStream & stream, const LogScope & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets scope log message priority level by integer value.
     * \param   newPrio     Scope log message priority level to set.
     **/
    inline void setPriority( unsigned int newPrio );

    /**
     * \brief   Sets scope log message priority level by name.
     * \param   newPrio     The name of the log priority level to add.
     **/
    inline void setPriority( const char * newPrio );
    inline void setPriority( const String & newPrio );

    /**
     * \brief   Adds priority level to the existing priority level of the scope.
     *          The priority level is added bitwise.
     * \param   addPrio     The log message priority level to add.
     **/
    inline void addPriority( NELogging::eLogPriority addPrio );

    /**
     * \brief   Adds priority level to the existing priority level of the scope,
     *          where the priority level is added by name.
     *          The priority level is added bitwise.
     * \param   addPrio     The name of the log priority level to add.
     **/
    inline void addPriority( const char * addPrio );
    inline void addPriority( const String & addPrio );

    /**
     * \brief   Removes priority level from the existing priority level of the scope.
     *          The operation is made bitwise.
     * \param   remPrio     The log priority level to remove.
     **/
    inline void removePriority( NELogging::eLogPriority remPrio );

    /**
     * \brief   Removes priority level from the existing priority level of the scope,
     *          where the priority level is passed by name.
     *          The operation is made bitwise.
     * \param   remPrio     The name of the log priority level to remove.
     **/
    inline void removePriority( const char * remPrio );
    inline void removePriority( const String & remPrio );

    /**
     * \brief   Returns the value of log message priority.
     **/
    inline unsigned int getPriority( void ) const;

    /**
     * \brief   Returns the ID of log scope.
     **/
    inline unsigned int getScopeId( void ) const;

    /**
     * \brief   Returns the name of the log scope.
     **/
    inline const String & getScopeName( void ) const;

    /**
     * \brief   Returns the session ID of the log scope, used to identify the scope in the session.
     **/
    inline uint32_t getSessionId(void) const;

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The ID of log scope. It cannot be changed
     **/
    const unsigned int  mScopeId;
    /**
     * \brief   The log message priority of the scope.
     **/
    unsigned int        mScopePrio;
    /**
     * \brief   The name of log scope. It cannot be changed
     **/
    const String        mScopeName;
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
    inline uint32_t nextSession(void) const;
    /**
     * \brief   Returns LogScope object
     **/
    inline LogScope & self( void );

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    LogScope( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( LogScope );
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
        //! A function to convert LogScope object to unsigned int.
        inline unsigned int operator()(const LogScope& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

//////////////////////////////////////////////////////////////////////////////
// inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline IEOutStream & operator << ( IEOutStream & stream, const LogScope & output )
{
    stream << output.mScopeId;
    stream << output.mScopePrio;
    stream << output.mScopeName;

    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// LogScope class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline uint32_t LogScope::nextSession(void) const
{
    return mSessionId.fetch_add(1);
}

inline LogScope & LogScope::self( void )
{
    return (*this);
}

inline LogScope::operator unsigned int ( void ) const
{
    return mScopeId;
}

inline void LogScope::setPriority( unsigned int newPrio )
{
    mScopePrio  = newPrio;
}

inline void LogScope::addPriority( NELogging::eLogPriority addPrio )
{
    mScopePrio  |= static_cast<unsigned int>(addPrio);
}

void LogScope::addPriority( const char * addPrio )
{
    addPriority( NELogging::stringToLogPrio(addPrio) );
}

void LogScope::addPriority( const String & addPrio )
{
    addPriority( NELogging::stringToLogPrio(addPrio) );
}

inline void LogScope::removePriority( NELogging::eLogPriority remPrio )
{
    mScopePrio  &= ~static_cast<unsigned int>(remPrio);
}

void LogScope::removePriority( const char * remPrio )
{
    removePriority( NELogging::stringToLogPrio(remPrio) );
}

void LogScope::removePriority( const String & remPrio )
{
    removePriority( NELogging::stringToLogPrio(remPrio) );
}

inline unsigned int LogScope::getPriority( void ) const
{
    return mScopePrio;
}

inline unsigned int LogScope::getScopeId( void ) const
{
    return mScopeId;
}

inline const String & LogScope::getScopeName( void ) const
{
    return mScopeName;
}

inline uint32_t LogScope::getSessionId(void) const
{
    return mSessionId;
}

#endif  // AREG_LOGGING_LOGSCOPE_HPP
