#ifndef AREG_COMPONENT_COMPONENTADDRESS_HPP
#define AREG_COMPONENT_COMPONENTADDRESS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/ComponentAddress.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Address class.
 *              This class defines address object of component.
 *              Every created component has address to be able to find
 *              in system and use for event dispatching.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/ThreadAddress.hpp"
#include "areg/base/String.hpp"

#include <utility>
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class OutStream;

//////////////////////////////////////////////////////////////////////////
// ComponentAddress class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Unique component address in the system, composed of a master thread address and role
 *          name. Supports conversion to/from string paths and validates component identity.
 **/
class AREG_API ComponentAddress
{
//////////////////////////////////////////////////////////////////////////
// Predefined types and constants.
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Static variables
/************************************************************************/
public:
    /**
     * \brief   Returns the constant invalid component address. The invalid address is excluded from
     *          event dispatching and registration.
     **/
    static const ComponentAddress & invalid_component_address();

/************************************************************************/
// Static methods
/************************************************************************/

    /**
     * \brief   Converts component address to a path string with special syntax.
     *
     * \param   componentAddress    The component address to convert.
     * \return  Returns the component address as a string path.
     **/
    static String to_path( const ComponentAddress & componentAddress );

    /**
     * \brief   Parses component path string to component address.
     *
     * \param   componentPath       The component path string.
     * \param[in,out] out_nextPart        If not nullptr, on output contains the remaining path
     *                                    after the component address.
     * \return  Returns the parsed component address. Validate before use.
     **/
    static ComponentAddress from_path( const char* componentPath, const char ** out_nextPart = nullptr );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes from thread address. Role name must be set separately.
     *
     * \param   threadAddress       The master thread address.
     **/
    ComponentAddress( const ThreadAddress & threadAddress );
    
    /**
     * \brief   Initializes from thread address and role name.
     *
     * \param   threadAddress       The master thread address.
     * \param   roleName            The component role name.
     **/
    ComponentAddress( const ThreadAddress & threadAddress, const String & roleName );

    /**
     * \brief   Initializes from role name using the current thread as master.
     *
     * \param   roleName    The component role name.
     **/
    ComponentAddress( const String & roleName );

    /**
     * \brief   Initializes from role name and thread name.
     *
     * \param   roleName        The component role name.
     * \param   nameThread      The master thread name.
     **/
    ComponentAddress( const String & roleName, const String & nameThread );

    /**
     * \brief
     *
     * \param   src     The source to copy.
     **/
    ComponentAddress( const ComponentAddress & src );

    /**
     * \brief
     *
     * \param   src     The source to move.
     * \note    Move overload.
     **/
    ComponentAddress( ComponentAddress && src ) noexcept;

    /**
     * \brief   Deserializes component address from stream.
     *
     * \param   stream      The stream to read.
     **/
    ComponentAddress( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~ComponentAddress() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Basic operators
/************************************************************************/

    /**
     * \brief   Converts component address to 32-bit unsigned integer.
     **/
    inline explicit operator uint32_t () const;

    /**
     * \brief   Copies address data from given source.
     *
     * \param   src     The source to copy.
     **/
    inline ComponentAddress & operator = ( const ComponentAddress & src );

    /**
     * \brief   Moves address data from given source.
     *
     * \param   src     The source to move.
     **/
    inline ComponentAddress & operator = ( ComponentAddress && src ) noexcept;

    /**
     * \brief   Returns true if addresses are equal.
     *
     * \param   other       The address to compare.
     **/
    inline bool operator == ( const ComponentAddress & other ) const;

    /**
     * \brief   Returns true if addresses are not equal.
     *
     * \param   other       The address to compare.
     **/
    inline bool operator != ( const ComponentAddress & other ) const;

/************************************************************************/
// Friend global operators to support streaming.
/************************************************************************/

    /**
     * \brief   Deserializes component address from stream.
     *
     * \param   stream      The stream to read.
     * \param[out] input       The address to initialize.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, ComponentAddress & input );

    /**
     * \brief   Serializes component address to stream.
     *
     * \param[out] stream      The stream to write.
     * \param   output      The address to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const ComponentAddress & output );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the master thread address.
     **/
    inline const ThreadAddress & thread_address() const;

    /**
     * \brief   Returns the component role name.
     **/
    inline const String & role_name() const;

    /**
     * \brief   Returns true if component address is valid.
     **/
    bool is_valid() const;

    /**
     * \brief   Converts component address to path string with special separators.
     *
     * \return  Returns the component address as a string path.
     **/
    String to_string() const;

    /**
     * \brief   Parses component path string and initializes address.
     *
     * \param   pathComponent       The component path string.
     * \param[in,out] out_nextPart        If not nullptr, on output contains the remaining path.
     **/
    void conv_from_string(const char * pathComponent, const char** out_nextPart = nullptr);

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private methods
/************************************************************************/
    /**
     * \brief   Default constructor (internal use only).
     **/
    ComponentAddress();
    /**
     * \brief   Returns the hash value of the given component address.
     *
     * \param   addrComp    The component address to hash.
     * \return  Returns the calculated hash value.
     **/
    static uint32_t _magic_number( const ComponentAddress & addrComp );

private:
/************************************************************************/
// Private member variables
/************************************************************************/
    /**
     * \brief   Component name. Or Role name of component
     **/
    String          mRoleName;
    /**
     * \brief   Thread address object.
     **/
    ThreadAddress   mThreadAddress;
    /**
     * \brief   The numeric value of Component Address object
     **/
    uint32_t    mMagicNum;
};

//////////////////////////////////////////////////////////////////////////
// ComponentAddress class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline ComponentAddress & ComponentAddress::operator = ( const ComponentAddress& src )
{
    mThreadAddress  = src.mThreadAddress;
    mRoleName       = src.mRoleName;
    mMagicNum       = src.mMagicNum;

    return (*this);
}

inline ComponentAddress & ComponentAddress::operator = ( ComponentAddress && src ) noexcept
{
    if (this != &src)
    {
        mThreadAddress  = std::move(src.mThreadAddress);
        mRoleName       = std::move(src.mRoleName);
        mMagicNum       = src.mMagicNum;
        src.mMagicNum   = areg::CHECKSUM_IGNORE;
    }

    return (*this);
}

inline bool ComponentAddress::operator == ( const ComponentAddress & other ) const
{
    return (mThreadAddress == other.mThreadAddress) && (mRoleName == other.mRoleName);
}

inline bool ComponentAddress::operator != ( const ComponentAddress& other ) const
{
    return (mThreadAddress != other.mThreadAddress) || (mRoleName != other.mRoleName);
}

ComponentAddress::operator uint32_t () const
{
    return mMagicNum;
}

inline const ThreadAddress& ComponentAddress::thread_address() const
{
    return mThreadAddress;
}

inline const String& ComponentAddress::role_name() const
{
    return mRoleName;
}

//////////////////////////////////////////////////////////////////////////
// Friend global operators to support streaming
//////////////////////////////////////////////////////////////////////////
inline const InStream & operator >> (const InStream & stream, ComponentAddress & input)
{
    stream >> input.mRoleName;
    stream >> input.mThreadAddress; 
    return stream;                                   }

inline OutStream & operator << (OutStream & stream, const ComponentAddress & output)
{
    stream << output.mRoleName;
    stream << output.mThreadAddress;
    return stream;
}

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of ComponentAddress class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the ComponentAddress.
 */
namespace std {
    template<>
    struct hash<areg::ComponentAddress>
    {
        //! A function to convert ComponentAddress object to uint32_t.
        inline uint32_t operator()(const areg::ComponentAddress& key) const
        {
            return static_cast<uint32_t>(key);
        }
    };
} // namespace std

#endif  // AREG_COMPONENT_COMPONENTADDRESS_HPP
