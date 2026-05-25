#ifndef AREG_BASE_RUNTIMECLASSID_HPP
#define AREG_BASE_RUNTIMECLASSID_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/RuntimeClassID.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Runtime Class ID.
 *              Identifies runtime objects hierarchically.  The magic number
 *              (CRC32 of the class name) is computed at compile time when the
 *              class name is a string literal, enabling zero-cost static
 *              initialization and single-integer runtime type checks.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MathDefs.hpp"

#include "areg/base/String.hpp"
#include <string_view>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Compile-time–constructible runtime class identifier.
 *
 *          Each runtime class owns a static constexpr RuntimeClassID created
 *          from its string-literal class name via AREG_IMPLEMENT_RUNTIME.
 *          The CRC32 magic number is evaluated entirely at compile time for
 *          literal-constructed objects, making hierarchy checks a single
 *          32-bit integer comparison at runtime.
 *
 *          Lifetime rule:
 *          The character data pointed to by the internal string_view must
 *          outlive this object.  For string literals (the normal use case)
 *          this is always satisfied.  Constructing from a temporary String
 *          is unsafe; use the const String& overload only when the String
 *          has static or sufficiently long lifetime.
 **/
class AREG_API RuntimeClassID
{
/************************************************************************
 * friend classes to access private default constructor.
 ************************************************************************/
    template < typename KEY, typename VALUE >
    friend class HashMap;
    template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, typename SYNCOBJECT, class MapContainer, class Implement>
    friend class ResourceMapBase;

//////////////////////////////////////////////////////////////////////////
// Public constants
//////////////////////////////////////////////////////////////////////////
public:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    /**
     * \brief   Sentinel name used by the default-constructed (invalid) class ID.
     **/
    static constexpr std::string_view BAD_CLASS_ID { "_BAD_RUNTIME_CLASS_ID_" };
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Static helpers
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a reference to the shared invalid RuntimeClassID singleton.
     *          Used as a default value in hash-map blocks.
     **/
    [[nodiscard]]
    static RuntimeClassID & empty_id() noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructs a RuntimeClassID from a null-terminated string literal.
     *          The magic number is computed at compile time when className is
     *          a string literal constant expression.
     *
     * \param   className   Null-terminated class name.  Must outlive this object.
     **/
    explicit inline constexpr RuntimeClassID( const char * className ) noexcept;

    /**
     * \brief   Constructs a RuntimeClassID from a string_view.
     *          The magic number is computed at compile time when className is
     *          a constexpr string_view.
     *
     * \param   className   Class name view.  Pointed-to data must outlive this object.
     **/
    explicit inline constexpr RuntimeClassID( std::string_view className ) noexcept;

    inline constexpr RuntimeClassID( const RuntimeClassID& src ) noexcept;

    inline constexpr RuntimeClassID(RuntimeClassID&& src) noexcept;

    ~RuntimeClassID() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline constexpr RuntimeClassID& operator = (const RuntimeClassID& src) noexcept;

    inline constexpr RuntimeClassID& operator = (RuntimeClassID&& src) noexcept;

    friend inline constexpr bool operator == ( const char * lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline constexpr bool operator == ( std::string_view lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline bool operator == ( const String & lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline constexpr bool operator != ( const char * lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline constexpr bool operator != ( std::string_view lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline bool operator != ( const String & lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline constexpr bool operator == ( uint32_t lhs, const RuntimeClassID & rhs ) noexcept;

    friend inline constexpr bool operator != ( uint32_t lhs, const RuntimeClassID & rhs ) noexcept;

    [[nodiscard]]
    inline constexpr bool operator == ( const RuntimeClassID & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator == ( const char * other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator == ( std::string_view other ) const noexcept;

    [[nodiscard]]
    inline bool operator == ( const String & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( const RuntimeClassID & other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( const char * other ) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != ( std::string_view other ) const noexcept;

    [[nodiscard]]
    inline bool operator != ( const String & other ) const noexcept;

    /**
     * \brief   Returns the CRC32 magic number for use as a hash-map key.
     **/
    [[nodiscard]]
    inline explicit constexpr operator uint32_t () const noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if this is a valid (non-sentinel) class identifier.
     **/
    [[nodiscard]]
    inline constexpr bool is_valid() const noexcept;

    /**
     * \brief   Returns the class name as a non-owning string_view.
     *          Valid for the lifetime of the underlying string data.
     **/
    [[nodiscard]]
    inline constexpr std::string_view name() const noexcept;

    /**
     * \brief   Returns the CRC32 magic number of the class name.
     **/
    [[nodiscard]]
    inline constexpr uint32_t magic() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Private default constructor (for HashMap / ResourceMapBase)
//////////////////////////////////////////////////////////////////////////
private:
    constexpr RuntimeClassID() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    std::string_view    mClassName;     //!< Non-owning view of the class name.
    uint32_t            mMagicNum;      //!< CRC32 of mClassName; compile-time for literal construction.
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
};

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID constexpr constructor implementations
//////////////////////////////////////////////////////////////////////////

inline constexpr RuntimeClassID::RuntimeClassID() noexcept
    : mClassName { BAD_CLASS_ID }
    , mMagicNum  { areg::CHECKSUM_IGNORE }
{
}

inline constexpr RuntimeClassID::RuntimeClassID( const char * className ) noexcept
    : mClassName { areg::is_empty<char>(className) ? BAD_CLASS_ID : std::string_view{ className } }
    , mMagicNum  { areg::is_empty<char>(className) ? areg::CHECKSUM_IGNORE : areg::crc32_calculate(className) } 
{
}

inline constexpr RuntimeClassID::RuntimeClassID( std::string_view className ) noexcept
    : mClassName { className.empty() ? BAD_CLASS_ID : className }
    , mMagicNum  { className.empty() ? areg::CHECKSUM_IGNORE : areg::crc32_calculate(className) }
{
}

inline constexpr RuntimeClassID::RuntimeClassID( const RuntimeClassID & src ) noexcept
    : mClassName { src.mClassName }
    , mMagicNum  { src.mMagicNum }
{
}

inline constexpr RuntimeClassID::RuntimeClassID(RuntimeClassID&& src) noexcept
    : mClassName { std::move(src.mClassName) }
    , mMagicNum  { src.mMagicNum }
{
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID singleton
//////////////////////////////////////////////////////////////////////////

inline RuntimeClassID & RuntimeClassID::empty_id() noexcept
{
    static RuntimeClassID _runtime;
    return _runtime;
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID member operator implementations
//////////////////////////////////////////////////////////////////////////

inline constexpr RuntimeClassID& RuntimeClassID::operator = (const RuntimeClassID& src) noexcept
{
    if (this != &src)
    {
        mClassName  = src.mClassName;
        mMagicNum   = src.mMagicNum;
    }

    return (*this);
}

inline constexpr RuntimeClassID& RuntimeClassID::operator = (RuntimeClassID&& src) noexcept
{
    if (this != &src)
    {
        mClassName  = std::move(src.mClassName);
        mMagicNum   = src.mMagicNum;
    }

    return (*this);
}


inline constexpr bool RuntimeClassID::operator == ( const RuntimeClassID & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum);
}

inline constexpr bool RuntimeClassID::operator == ( const char * other ) const noexcept
{
    return (mClassName == other);
}

inline constexpr bool RuntimeClassID::operator == ( std::string_view other ) const noexcept
{
    return (mClassName == other);
}

inline bool RuntimeClassID::operator == ( const String & other ) const noexcept
{
    return (other == mClassName);
}

inline constexpr bool RuntimeClassID::operator != ( const RuntimeClassID & other ) const noexcept
{
    return (mMagicNum != other.mMagicNum);
}

inline constexpr bool RuntimeClassID::operator != ( const char * other ) const noexcept
{
    return (mClassName != other);
}

inline constexpr bool RuntimeClassID::operator != ( std::string_view other ) const noexcept
{
    return (mClassName != other);
}

inline bool RuntimeClassID::operator != ( const String & other ) const noexcept
{
    return (other != mClassName);
}

inline constexpr RuntimeClassID::operator uint32_t () const noexcept
{
    return mMagicNum;
}

inline constexpr bool RuntimeClassID::is_valid() const noexcept
{
    return (mMagicNum != areg::CHECKSUM_IGNORE);
}

inline constexpr std::string_view RuntimeClassID::name() const noexcept
{
    return mClassName;
}

inline constexpr uint32_t RuntimeClassID::magic() const noexcept
{
    return mMagicNum;
}

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID global friend operator implementations
//////////////////////////////////////////////////////////////////////////

[[nodiscard]]
inline constexpr bool operator == ( const char * lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mClassName == lhs);
}

[[nodiscard]]
inline constexpr bool operator == ( std::string_view lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mClassName == lhs);
}

[[nodiscard]]
inline bool operator == ( const String & lhs, const RuntimeClassID & rhs ) noexcept
{
    return (lhs == rhs.mClassName);
}

[[nodiscard]]
inline constexpr bool operator != ( const char * lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mClassName != lhs);
}

[[nodiscard]]
inline constexpr bool operator != ( std::string_view lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mClassName != lhs);
}

[[nodiscard]]
inline bool operator != ( const String & lhs, const RuntimeClassID & rhs ) noexcept
{
    return (lhs != rhs.mClassName);
}

[[nodiscard]]
inline constexpr bool operator == ( uint32_t lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mMagicNum == lhs);
} 

[[nodiscard]]
inline constexpr bool operator != ( uint32_t lhs, const RuntimeClassID & rhs ) noexcept
{
    return (rhs.mMagicNum != lhs);
}

template<>
struct required_size<areg::RuntimeClassID>
{
    [[nodiscard]]
    inline constexpr uint32_t operator()() const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }

    [[nodiscard]]
    inline constexpr uint32_t operator()(const areg::RuntimeClassID& /*classId*/) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }

    [[nodiscard]]
    inline constexpr operator uint32_t() const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t));
    }
};


} // namespace areg

//////////////////////////////////////////////////////////////////////////
// std::hash specialization for RuntimeClassID
//////////////////////////////////////////////////////////////////////////

namespace std {
    template<>
    struct hash<areg::RuntimeClassID>
    {
        [[nodiscard]]
        inline uint32_t operator()( const areg::RuntimeClassID & key ) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_BASE_RUNTIMECLASSID_HPP
