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
#include "areg/base/areg_global.h"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/IOStream.hpp"
#include <atomic>
#include <string_view>
namespace areg {

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
    using session_id    = std::atomic_uint32_t;

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Constructs a log scope from a string literal, computing the scope ID at
     *          compile time and storing the name as a zero-allocation string_view.
     *          A compile-time check enforces the maximum scope name length.
     *
     * \tparam  N           Length of the string literal including the null terminator.
     *                      Deduced automatically from the literal.
     * \param   scopeName   The string literal name of the scope. Must have static storage
     *                      duration for the lifetime of this LogScope object.
     * \param   priority    The initial log message priority. Defaults to PrioNotset.
     **/
    template<uint32_t N>
    explicit LogScope( const char (&scopeName)[N]
                     , areg::LogPriority priority = areg::LogPriority::PrioNotset ) noexcept;

    ~LogScope();

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns 32-bit integer value of the scope.
     **/
    [[nodiscard]]
    inline explicit constexpr operator uint32_t () const noexcept;

    /**
     * \brief   Writes the scope data into the stream.
     *
     * \param   stream      The stream object to write data.
     * \param   output      The scope to write into the stream object.
     * \note    The log scope class does not have operator to deserialize the scope object, because
     *          of 2 reasons: 1. LogScope does not have default constructor. Because it sets empty
     *          name, which is not unique; 2. The name of the log scope cannot be changed.
     **/
    inline friend OutStream & operator << ( OutStream & stream, const LogScope & output );

//////////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Sets scope log message priority level by integer value.
     *
     * \param   newPrio     Scope log message priority level to set.
     **/
    inline void set_priority( uint32_t newPrio ) noexcept;

    /**
     * \brief   Sets scope log message priority level by name.
     *
     * \param   newPrio     The name of the log priority level to add.
     **/
    inline void set_priority( const char * newPrio ) noexcept;
    /**
     * \brief   Sets scope log message priority level by name.
     *
     * \param   newPrio     The name of the log priority level to add.
     **/
    inline void set_priority( const String & newPrio ) noexcept;

    /**
     * \brief   Adds priority level to the existing priority level of the scope. The priority level is added bitwise.
     *
     * \param   addPrio     The log message priority level to add.
     **/
    inline void add_priority( areg::LogPriority addPrio ) noexcept;

    /**
     * \brief   Adds priority level to the existing priority level of the scope, where the priority
     *          level is added by name. The priority level is added bitwise.
     *
     * \param   addPrio     The name of the log priority level to add.
     **/
    inline void add_priority( const char * addPrio ) noexcept;
    /**
     * \brief   Adds priority level to the existing priority level of the scope, where the priority
     *          level is added by name. The priority level is added bitwise.
     *
     * \param   addPrio     The name of the log priority level to add.
     * \note    Move overload. Takes ownership of the string.
     **/
    inline void add_priority( const String & addPrio ) noexcept;

    /**
     * \brief   Removes priority level from the existing priority level of the scope. The operation is made bitwise.
     *
     * \param   remPrio     The log priority level to remove.
     **/
    inline void remove_priority( areg::LogPriority remPrio ) noexcept;

    /**
     * \brief   Removes priority level from the existing priority level of the scope, where the
     *          priority level is passed by name. The operation is made bitwise.
     *
     * \param   remPrio     The name of the log priority level to remove.
     **/
    inline void remove_priority( const char * remPrio ) noexcept;
    /**
     * \brief   Removes priority level from the existing priority level of the scope, where the
     *          priority level is passed by name. The operation is made bitwise.
     *
     * \param   remPrio     The name of the log priority level to remove.
     * \note    Move overload. Takes ownership of the string.
     **/
    inline void remove_priority( const String & remPrio ) noexcept;

    /**
     * \brief   Returns the value of log message priority.
     **/
    [[nodiscard]]
    inline uint32_t priority() const noexcept;

    /**
     * \brief   Returns the ID of log scope.
     **/
    [[nodiscard]]
    inline constexpr uint32_t id() const noexcept;

    /**
     * \brief   Returns the name of the log scope.
     **/
    [[nodiscard]]
    inline constexpr std::string_view name() const noexcept;

    /**
     * \brief   Returns the session ID of the log scope, used to identify the scope in the session.
     **/
    [[nodiscard]]
    inline uint32_t session() const noexcept;

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

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   The name of log scope. It cannot be changed
     **/
    const std::string_view  mScopeName;
     /**
      * \brief   The session ID of the log scope, used to identify the scope in the session.
      **/
     mutable session_id mSessionId;
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Increments the session ID and returns the new value to use in log messages.
     **/
    [[nodiscard]]
    inline uint32_t next_session() const noexcept;

    [[nodiscard]]
    inline LogScope & self() noexcept;

    /**
     * \brief   Registers this scope with LogManager. Separated from the constructor body
     *          so that the template constructor does not need to include LogManager.hpp.
     **/
    void _register() noexcept;

//////////////////////////////////////////////////////////////////////////////
// Forbidden methods
//////////////////////////////////////////////////////////////////////////////
private:
    LogScope() = delete;
    AREG_NOCOPY_NOMOVE( LogScope );
};

//////////////////////////////////////////////////////////////////////////////
// inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline OutStream & operator << ( OutStream & stream, const LogScope & output )
{
    stream << output.mScopeId;
    stream << output.mScopePrio;
    stream << String{ output.mScopeName.data() };

    return stream;
}

//////////////////////////////////////////////////////////////////////////////
// LogScope class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline uint32_t LogScope::next_session() const noexcept
{
    return mSessionId.fetch_add(1, std::memory_order_relaxed);
}

inline LogScope & LogScope::self() noexcept
{
    return (*this);
}

inline constexpr LogScope::operator uint32_t () const noexcept
{
    return mScopeId;
}

inline void LogScope::set_priority( uint32_t newPrio ) noexcept
{
    mScopePrio  = newPrio;
}

inline void LogScope::add_priority( areg::LogPriority addPrio ) noexcept
{
    mScopePrio  |= static_cast<uint32_t>(addPrio);
}

void LogScope::add_priority( const char * addPrio ) noexcept
{
    add_priority( areg::string_to_priority(addPrio) );
}

void LogScope::add_priority( const String & addPrio ) noexcept
{
    add_priority( areg::string_to_priority(addPrio) );
}

inline void LogScope::remove_priority( areg::LogPriority remPrio ) noexcept
{
    mScopePrio  &= ~static_cast<uint32_t>(remPrio);
}

void LogScope::remove_priority( const char * remPrio ) noexcept
{
    remove_priority( areg::string_to_priority(remPrio) );
}

void LogScope::remove_priority( const String & remPrio ) noexcept
{
    remove_priority( areg::string_to_priority(remPrio) );
}

inline uint32_t LogScope::priority() const noexcept
{
    return mScopePrio;
}

inline constexpr uint32_t LogScope::id() const noexcept
{
    return mScopeId;
}

inline constexpr std::string_view LogScope::name() const noexcept
{
    return mScopeName;
}

inline uint32_t LogScope::session() const noexcept
{
    return mSessionId;
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////////
// LogScope template constructor implementation
//////////////////////////////////////////////////////////////////////////////

#if AREG_LOGGING

template<uint32_t N>
inline areg::LogScope::LogScope( const char (&scopeName)[N]
                                , areg::LogPriority priority ) noexcept
    : mScopeId      ( areg::make_id(scopeName) )
    , mScopePrio    ( static_cast<uint32_t>(priority) )
    , mScopeName    ( scopeName, N - 1u )
    , mSessionId    ( 0u )
{
    static_assert(N <= 256u, "Scope name exceeds maximum allowed length (255 chars)");
    _register();
}

#else   // AREG_LOGGING

template<uint32_t N>
inline areg::LogScope::LogScope( const char (&)[N]
                                , areg::LogPriority ) noexcept
    : mScopeId      ( 0u )
    , mScopePrio    ( static_cast<uint32_t>(areg::LogPriority::PrioInvalid) )
    , mScopeName    ( )
    , mSessionId    ( 0u )
{
    static_assert(N <= 256u, "Scope name exceeds maximum allowed length (255 chars)");
}

#endif  // AREG_LOGGING

//////////////////////////////////////////////////////////////////////////
// Hasher of LogScope class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the LogScope.
 */
namespace std {
    template<>
    struct hash<areg::LogScope>
    {
        //! A function to convert LogScope object to uint32_t.
        [[nodiscard]]
        inline uint32_t operator()(const areg::LogScope& key) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_LOGGING_LOGSCOPE_HPP
