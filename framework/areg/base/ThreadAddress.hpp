#ifndef AREG_BASE_THREADADDRESS_HPP
#define AREG_BASE_THREADADDRESS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/ThreadAddress.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread Address class
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

#include <string_view>
#include <utility>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   a class, containing thread address information.
 *          The thread address contains Process ID and Thread ID.
 *          Both of these parameters are unique and used for resource mapping.
 **/
class AREG_API ThreadAddress
{
//////////////////////////////////////////////////////////////////////////
// ThreadAddress Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   ThreadAddress::INVALID_THREAD_ADDRESS
     *          Invalid thread address constant.
     **/
    static const ThreadAddress    INVALID_THREAD_ADDRESS;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets current process ID and invalid thread name.
     **/
    ThreadAddress( void );
    /**
     * \brief   Initialization constructor. Sets current process ID and the thread name.
     * \param   threadID    Thread ID to set.
     **/
    explicit ThreadAddress( const char * threadName );
    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    ThreadAddress( const ThreadAddress & src );
    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    ThreadAddress( ThreadAddress && src ) noexcept;
    /**
     * \brief   Initialization constructor. Initialize variables from given stream
     * \param   stream  Input Streaming object that contains Thread Address data.
     **/
    ThreadAddress( const IEInStream & stream );
    /**
     * \brief   Destructor
     **/
    ~ThreadAddress( void ) = default;

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   This operator converts thread address data to unsigned int value
     **/
    inline explicit operator unsigned int ( void ) const;
    /**
     * \brief   Assigning operator to copy data from given source. 
     **/
    inline ThreadAddress & operator = (const ThreadAddress & src);
    /**
     * \brief   Move operator to move data from given source.
     **/
    inline ThreadAddress & operator = ( ThreadAddress && src ) noexcept;
    /**
     * \brief   Compares 2 thread address objects and if they are
     *          identical, returns true
     **/
    inline bool operator == (const ThreadAddress & other) const;
    /**
     * \brief   Compares 2 thread address objects and if they are not
     *          identical, returns true
     **/
    inline bool operator != (const ThreadAddress & other) const;

/************************************************************************/
// Friend global operators to make thread address streamable
/************************************************************************/
    /**
     * \brief	Read data from streaming object and initialize Thread Address object.
     * \param	stream	The streaming object to read data.
     * \param	input	The reference to Thread Address object to initialize data.
     * \return	Reference to streaming object.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, ThreadAddress & input );

    /**
     * \brief	Write data to streaming object and copies Thread Address object.
     * \param	stream	The streaming object to write data
     * \param	output	The reference to Thread Address object to get data
     * \return	Reference to streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const ThreadAddress & output);

//////////////////////////////////////////////////////////////////////////
// ThreadAddress operations and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return thread name.
     **/
    inline const String & getThreadName( void ) const;

    /**
     * \brief   Return true if current object is not equal to INVALID_THREAD_ADDRESS
     **/
    bool isValid( void ) const;

    /**
     * \brief	Converts Thread Address object to the thread path string value.
     *          the thread path has following format: "<process ID>::<thread name>::",
     *          where <process ID> is an integer value of process and
     *          <string name> is the name of string.
     * \param	threadAddress	The thread address object to convert.
     * \return	Returns string of thread path
     **/
    static String convAddressToPath( const ThreadAddress & threadAddress );

    /**
     * \brief	Parses passed path and converts passed path to the Thread Address object.
     *          The path should contain at least following format: "<process ID>::<thread name>".
     *          The first item should be integer.
     *          If Thread Address was parsed and properly retrieved, on output, parameter 'out_nextPart' 
     *          will contains address of string in 'threadPath' after Thread Address part.
     *          For example: if path is "<process ID>::<thread name>::<component name>::<service name>::",
     *          on output, out_nextPart will contain "<component name>::<service name>::" it is not nullptr
     * \param	threadPath	    The path of thread to convert.
     * \param   out_nextPart    If successfully retrieved thread address and it is not nullptr, 
     *                          on output will contain address of string after retrieving Thread address.
     *                          If parsing fails, it will contain same address as 'threadPart'.
     *                          If parameter is nullptr, it will be ignored.
     * \return	Return created from path thread address object.
     **/
    static ThreadAddress convPathToAddress( const char* const threadPath, const char** OUT out_nextPart = nullptr );

    /**
     * \brief	Converts Thread Address object to the thread path string value.
     *          the thread path has following format: "<process ID>::<thread name>::",
     *          where <process ID> is an integer value of process and
     *          <string name> is the name of string.
     * \return  Returns converted path of thread as string.
     **/
    inline String convToString( void ) const;

    /**
     * \brief	Parse string and retrieves thread address data from path.
     * \param	pathProxy	    The thread path as a string.
     * \param	out_nextPart	If not a nullptr, on output this will contain remaining
     *                          part after getting thread path.
     **/
    void convFromString(const char * threadPath, const char** OUT out_nextPart = nullptr );

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the calculated hash-key value of specified thread address object.
     **/
    static unsigned int _magicNumber( const ThreadAddress & addrThread );

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
    unsigned int    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// Hasher of ThreadAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ThreadAddress.
 */
namespace std
{
    template<> struct hash<ThreadAddress>
    {
        //! A function to convert ThreadAddress object to unsigned int.
        inline unsigned int operator()(const ThreadAddress& key) const
        {
            return static_cast<unsigned int>(key);
        }
    };
}

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
        src.mMagicNum   = NEMath::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline const String & ThreadAddress::getThreadName( void ) const
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

inline ThreadAddress::operator unsigned int( void ) const
{
    return mMagicNum;
}

inline String ThreadAddress::convToString(void) const
{
    return mThreadName;
}

//////////////////////////////////////////////////////////////////////////
// Global operators for ThreadAddress class
//////////////////////////////////////////////////////////////////////////
inline const IEInStream & operator >> (const IEInStream & stream, ThreadAddress & input)
{
    return ( stream >> input.mThreadName );
}

inline IEOutStream & operator << (IEOutStream & stream, const ThreadAddress & output)
{
    return ( stream << output.mThreadName );
}

#endif  // AREG_BASE_THREADADDRESS_HPP
