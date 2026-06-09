#ifndef AREG_BASE_THREADADDRESS_HPP
#define AREG_BASE_THREADADDRESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ThreadAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread Address class
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <string_view>
#include <utility>


/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {

class InStream;
class OutStream;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Encapsulates thread identification information including process ID and thread name for
 *          resource mapping and routing.
 **/
class AREG_API ThreadAddress
{
//////////////////////////////////////////////////////////////////////////
// ThreadAddress Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
//////////////////////////////////////////////////////////////////////////
// Public static methods
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Converts a ThreadAddress object to a path string with format "<process ID>::<thread name>::".
     *
     * \param   threadAddress       The thread address object to convert.
     * \return  Returns the thread path string.
     **/
    [[nodiscard]]
    static String to_path( const ThreadAddress & threadAddress );

    /**
     * \brief   Parses a path string and converts it to a ThreadAddress object. Returns remaining
     *          string after the address part.
     *
     * \param   threadPath      The path string to parse in format "<process ID>::<thread name>::...".
     * \param[out] nextPart     If not nullptr, receives the pointer to the remaining path after
     *                          the address. If parsing fails, contains the same address as threadPath.
     * \return  Returns the ThreadAddress object created from the path.
     **/
    [[nodiscard]]
    static ThreadAddress from_path( const char* const threadPath, const char** nextPart = nullptr );

    /**
     * \brief   Returns the invalid thread address singleton.
     **/
    static const ThreadAddress & invalid_thread_address();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    ThreadAddress();
    ThreadAddress(const areg::Endpoint & endPoint);
    ThreadAddress(const ThreadAddress& src);
    ThreadAddress(ThreadAddress&& src) noexcept;
    ~ThreadAddress() = default;

    /**
     * \brief   Initializes with the specified thread name.
     *
     * \param   threadName      The null-terminated thread name string.
     **/
    explicit ThreadAddress( const char * threadName );
    explicit ThreadAddress( const String & threadName );

    /**
     * \brief   Initializes with specified thread unique number.
     *
     * \param   threadName      The null-terminated thread name string.
     **/
    explicit ThreadAddress(const UniqueNumber numThread) noexcept;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts the thread address to a uint32_t hash value.
     **/
    [[nodiscard]]
    inline explicit operator uint32_t () const noexcept;

    inline ThreadAddress & operator = (const ThreadAddress & src);
    inline ThreadAddress & operator = ( ThreadAddress && src ) noexcept;

    [[nodiscard]]
    inline constexpr bool operator == (const ThreadAddress & other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != (const ThreadAddress & other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator > (const ThreadAddress& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator < (const ThreadAddress& other) const noexcept;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the thread name.
     **/
    [[nodiscard]]
    inline const String & name() const noexcept;

    /**
     * \brief   Returns true if the thread address is valid.
     **/
    [[nodiscard]]
    bool is_valid() const noexcept;

    /**
     * \brief   Converts the ThreadAddress to a path string with format "<process ID>::<thread name>::".
     *
     * \return  Returns the thread path string.
     **/
    [[nodiscard]]
    inline String to_string() const noexcept;

    /**
     * \brief   Parses a path string to extract thread address data. Returns remaining string after address.
     *
     * \param   threadPath      The path string containing thread address data.
     * \param[out] nextPart     If not nullptr, receives the pointer to the remaining path after the address.
     **/
    void from_string(const char * threadPath, const char** nextPart = nullptr );

    /**
     * \brief   Initialize data from areg::Endpoint structure.
     **/
    inline void from_endpoint(const areg::Endpoint& endPoint) noexcept;

    /**
     * \brief   Fill areg::Endpoint structure that matches data
     **/
    inline void to_endpoint(areg::Endpoint& endPoint) const noexcept;

    /**
     * \brief   Reads ThreadAddress data from an input stream.
     *
     * \param   stream      The input stream to read from.
     **/
    inline const InStream & from_stream( const InStream & stream );

    /**
     * \brief   Writes ThreadAddress data to an output stream.
     *
     * \param   stream      The output stream to write to.
     **/
    inline OutStream & to_stream( OutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the calculated hash value of the thread address.
     *
     * \param   addrThread      The thread address to hash.
     * \return  Returns the hash value.
     **/
    [[nodiscard]]
    static uint32_t _magic_number( const ThreadAddress & addrThread );

//////////////////////////////////////////////////////////////////////////
// ThreadAddress member variables
//////////////////////////////////////////////////////////////////////////
private:
    String      mThreadName;    //!< The thread name.
    uint32_t    mMagicNum;      //!< The calculated hash of this thread address.
};

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class inline function implementation
//////////////////////////////////////////////////////////////////////////
inline ThreadAddress & ThreadAddress::operator = ( const ThreadAddress & src )
{
    mThreadName = src.mThreadName;
    mMagicNum   = src.mMagicNum;

    return (*this);
}

inline ThreadAddress & ThreadAddress::operator = ( ThreadAddress && src ) noexcept
{
    if ( static_cast<const ThreadAddress *>(this) != &src )
    {
        mThreadName = std::move(src.mThreadName);
        mMagicNum   = src.mMagicNum;
        src.mMagicNum   = areg::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline const String & ThreadAddress::name() const noexcept
{
    return mThreadName;
}

inline constexpr bool ThreadAddress::operator == ( const ThreadAddress & other ) const noexcept
{
    return (mMagicNum == other.mMagicNum);
}

inline constexpr bool ThreadAddress::operator != ( const ThreadAddress & other ) const noexcept
{
    return (mMagicNum != other.mMagicNum);
}

inline constexpr bool ThreadAddress::operator > (const ThreadAddress& other) const noexcept
{
    return (mMagicNum > other.mMagicNum);
}

inline constexpr bool ThreadAddress::operator < (const ThreadAddress& other) const noexcept
{
    return (mMagicNum < other.mMagicNum);
}

inline ThreadAddress::operator uint32_t() const noexcept
{
    return mMagicNum;
}

inline String ThreadAddress::to_string() const noexcept
{
    return mThreadName;
}

inline void ThreadAddress::from_endpoint(const areg::Endpoint& endPoint) noexcept
{
    mMagicNum = endPoint.thread;
    mThreadName.clear();
}

inline void ThreadAddress::to_endpoint(areg::Endpoint& endPoint) const noexcept
{
    endPoint.thread = mMagicNum;
}

inline const InStream& ThreadAddress::from_stream(const InStream& stream) 
{
    stream >> mMagicNum;
    return stream;
}

inline OutStream& ThreadAddress::to_stream(OutStream& stream) const
{
    stream << mMagicNum;
    return stream;
}

inline const InStream & operator >> ( const InStream & stream, ThreadAddress & addr )
{
    return addr.from_stream(stream);
}

inline OutStream & operator << ( OutStream & stream, const ThreadAddress & addr )
{
    return addr.to_stream(stream);
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ThreadAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ThreadAddress.
 */

namespace std {
    template<> struct hash<areg::ThreadAddress>
    {
        [[nodiscard]]
        inline uint32_t operator()(const areg::ThreadAddress& key) const noexcept
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_BASE_THREADADDRESS_HPP
