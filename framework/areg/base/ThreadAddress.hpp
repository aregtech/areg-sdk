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

#include <string_view>
#include <utility>

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;

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
     * \brief   Converts a ThreadAddress object to a path string with format "<process ID>::<thread
     *          name>::".
     *
     * \param   threadAddress       The thread address object to convert.
     * \return  Returns the thread path string.
     **/
    static String to_path( const ThreadAddress & threadAddress );

    /**
     * \brief   Parses a path string and converts it to a ThreadAddress object. Returns remaining
     *          string after the address part.
     *
     * \param   threadPath      The path string to parse in format "<process ID>::<thread
     *                          name>::...".
     * \param[out] nextPart        If not nullptr, receives the pointer to the remaining path after
     *                             the address. If parsing fails, contains the same address as
     *                             threadPath.
     * \return  Returns the ThreadAddress object created from the path.
     **/
    static ThreadAddress from_path( const char* const threadPath, const char** nextPart = nullptr );

    /**
     * \brief   Returns the invalid thread address singleton.
     **/
    static const ThreadAddress & invalid_thread_address();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets the current process ID and invalid thread name.
     **/
    ThreadAddress();
    /**
     * \brief   Initializes with the current process ID and the specified thread name.
     *
     * \param   threadName      The null-terminated thread name string.
     **/
    explicit ThreadAddress( const char * threadName );
    /**
     * \brief   Initializes with the current process ID and the specified thread name.
     *
     * \param   threadName      The String object containing the thread name.
     **/
    explicit ThreadAddress( const String & threadName );
    ThreadAddress( const ThreadAddress & src );
    /**
     * \brief
     * \note    Move overload.
     **/
    ThreadAddress( ThreadAddress && src ) noexcept;
    /**
     * \brief   Initializes the ThreadAddress from data read from an input stream.
     *
     * \param   stream      The input stream containing ThreadAddress data.
     **/
    ThreadAddress( const InStream & stream );
    /**
     * \brief   Destructor
     **/
    ~ThreadAddress() = default;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts the thread address to a uint32_t hash value.
     **/
    inline explicit operator uint32_t () const;
    /**
     * \brief   Copies data from the given source ThreadAddress.
     **/
    inline ThreadAddress & operator = (const ThreadAddress & src);
    /**
     * \brief   Moves data from the given source ThreadAddress.
     **/
    inline ThreadAddress & operator = ( ThreadAddress && src ) noexcept;
    /**
     * \brief   Compares two ThreadAddress objects for equality.
     **/
    inline bool operator == (const ThreadAddress & other) const;
    /**
     * \brief   Compares two ThreadAddress objects for inequality.
     **/
    inline bool operator != (const ThreadAddress & other) const;

    /**
     * \brief   Returns true if this thread address is greater than the other. Supports sorted
     *          containers.
     **/
    inline bool operator > (const ThreadAddress& other) const;

    /**
     * \brief   Returns true if this thread address is less than the other. Supports sorted
     *          containers.
     **/
    inline bool operator < (const ThreadAddress& other) const;

/************************************************************************/
// Friend global operators to make thread address streamable
/************************************************************************/
    /**
     * \brief   Reads ThreadAddress data from an input stream.
     *
     * \param   stream      The input stream to read from.
     * \param   input       The ThreadAddress object to initialize from the stream.
     * \return  Returns a reference to the input stream.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ThreadAddress & input );

    /**
     * \brief   Writes ThreadAddress data to an output stream.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The ThreadAddress object to write to the stream.
     * \return  Returns a reference to the output stream.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ThreadAddress & output);

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the thread name.
     **/
    inline const String & thread_name() const;

    /**
     * \brief   Returns true if the thread address is valid.
     **/
    bool is_valid() const;

    /**
     * \brief   Converts the ThreadAddress to a path string with format "<process ID>::<thread
     *          name>::".
     *
     * \return  Returns the thread path string.
     **/
    inline String to_string() const;

    /**
     * \brief   Parses a path string to extract thread address data. Returns remaining string after
     *          address.
     *
     * \param   threadPath      The path string containing thread address data.
     * \param[out] nextPart        If not nullptr, receives the pointer to the remaining path after
     *                             the address.
     **/
    void conv_from_string(const char * threadPath, const char** nextPart = nullptr );

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
    static uint32_t _magic_number( const ThreadAddress & addrThread );

//////////////////////////////////////////////////////////////////////////
// ThreadAddress member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The thread name.
     **/
    String          mThreadName;
    /**
     * \brief   The calculated number of thread address.
     **/
    uint32_t    mMagicNum;
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

inline const String & ThreadAddress::thread_name() const
{
    return mThreadName;
}

inline bool ThreadAddress::operator == ( const ThreadAddress & other ) const
{
    return (mMagicNum == other.mMagicNum);
}

inline bool ThreadAddress::operator != ( const ThreadAddress & other ) const
{
    return (mMagicNum != other.mMagicNum);
}

inline bool ThreadAddress::operator > (const ThreadAddress& other) const
{
    return (mMagicNum > other.mMagicNum);
}

inline bool ThreadAddress::operator < (const ThreadAddress& other) const
{
    return (mMagicNum < other.mMagicNum);
}

inline ThreadAddress::operator uint32_t() const
{
    return mMagicNum;
}

inline String ThreadAddress::to_string() const
{
    return mThreadName;
}

//////////////////////////////////////////////////////////////////////////
// Global operators for ThreadAddress class
//////////////////////////////////////////////////////////////////////////
inline const InStream & operator >> (const InStream & stream, ThreadAddress & input)
{
    return ( stream >> input.mThreadName );
}

inline OutStream & operator << (OutStream & stream, const ThreadAddress & output)
{
    return ( stream << output.mThreadName );
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
        //! A function to convert ThreadAddress object to uint32_t.
        inline uint32_t operator()(const areg::ThreadAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_BASE_THREADADDRESS_HPP
