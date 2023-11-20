#ifndef AREG_BASE_IDENTIFIER_HPP
#define AREG_BASE_IDENTIFIER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/Identifier.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Object Identifier class.
 *              The Identifier is binding string and integer values to
 *              provide conversion from string to integer and from 
 *              integer to string.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/String.hpp"

#include <string_view>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Identifier class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Identifier is binding string and integer values to
 *              provide conversion from string to integer and from 
 *              integer to string. It is used to pass enum values as strings.
 **/
class AREG_API Identifier
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Identifier::BAD_IDENTIFIER_VALUE
     *          Bad Identifier integer value.
     *          The values is reserved and should not be used
     *          at least together with BAD_IDENTIFIER_NAME
     **/
    static constexpr unsigned int       BAD_IDENTIFIER_VALUE    { static_cast<unsigned int>(0x80000000) };

    /**
     * \brief   Identifier::BAD_IDENTIFIER_NAME
     *          Bad Identifier name. The name is reserved and
     *          should not be used at leas together with BAD_IDENTIFIER_VALUE
     **/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    static constexpr std::string_view   BAD_IDENTIFIER_NAME     { "_BAD_IDENTIFIER" };
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Identifier::BAD_IDENTIFIER
     *          Bad Identifier object, which integer value is BAD_IDENTIFIER_VALUE
     *          and string value is BAD_IDENTIFIER_NAME
     **/
    static const Identifier             BAD_IDENTIFIER          /* = {BAD_IDENTIFIER_VALUE, BAD_IDENTIFIER_NAME} */;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. 
     *          By default, initial values are equal to BAD_DENTIFIER
     *          and the identifier object is invalid
     **/
    Identifier( void );

    /**
     * \brief   Initialization constructor. Sets integer value and name
     * \param   idValue     The integer value of identifier
     * \param   idName      The name of identifier associated with integer value.
     **/
    Identifier(unsigned int idValue, const char* idName);
    Identifier(unsigned int idValue, const std::string_view& idName);
    Identifier(unsigned int idValue, const String& idName);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    Identifier(const Identifier & src);

    /**
     * \brief   Move constructor.
     * \param   src     The source to move data.
     **/
    Identifier( Identifier && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~Identifier( void ) = default;

//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts given integer value of the identifier into the string.
     *          It uses lookup table to make conversion.
     * \param   idValue     The digital value of the identifier to convert.
     * \param   lookupList  The lookup table to search identifier to return string value.
     * \param   defIndex    The index of the value to return in case if did not find the identifier.
     *                      Pass NECommon::INVALID_POSITION value to ignore and return empty string
     *                      if no identifier entry exists.
     * \return  Returns the string value of the identifier it found.
     *          Return the string value of the given default identifier entry index if identifier not found and default index is valid.
     *          Returns empty string if identifier not found and the index is invalid.
     **/
    inline static const String& convToString(unsigned int idValue, const std::vector<Identifier>& lookupList, unsigned int defIndex);

    /**
     * \brief   Converts given string value of the identifier into the digital value.
     *          It uses lookup table make conversion.
     * \param   idName      The string value of the identifier to convert.
     * \param   lookupList  The lookup table to search identifier to return integer value.
     * \param   defIndex    The index of the value to return in case if did not find the identifier.
     *                      Pass NECommon::INVALID_POSITION value to ignore and return 0xFFFFFFFF
     *                      if no identifier entry exists.
     * \return  Returns the integer value of the identifier it found.
     *          Return the integer value of the given default identifier entry index if identifier not found and default index is valid.
     *          Returns 0xFFFF'FFFF if identifier not found and the index is invalid.
     **/
    inline static unsigned int convFromString(const String& idName, const std::vector<Identifier>& lookupList, unsigned int defIndex);

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigning operator. Copies identifier data from given source
     **/
    Identifier & operator = (const Identifier & src);

    /**
     * \brief   Move operator. Moves identifier data from given source
     **/
    Identifier & operator = ( Identifier && src ) noexcept;

    /**
     * \brief   Returns true if 2 identifier objects are equal.
     **/
    inline bool operator == (const Identifier & other) const;

    /**
     * \brief   Returns true if the name of identifier is equal.
     **/
    inline bool operator == (const char * rhs) const;

    /**
     * \brief   Returns true if the integer value of identifier is equal.
     **/
    inline bool operator == (unsigned int rhs) const;

    /**
     * \brief   Returns true if 2 identifier objects are not equal.
     **/
    inline bool operator != ( const Identifier & other ) const;

    /**
     * \brief   Returns true if the name of identifier is not equal.
     **/
    inline bool operator != ( const char * rhs ) const;

    /**
     * \brief   Returns true if the integer value of identifier is equal.
     **/
    inline bool operator != ( unsigned int rhs ) const;

/************************************************************************
 * Friend functions. Declare to make it streamable
 ************************************************************************/

    /**
     * \brief   Reads identifier value from stream.
     * \param   stream  Streaming object to read identifier value
     * \param   input   Identifier to write values
     * \return  Returns instance of read streaming object
     **/
    friend inline const IEInStream & operator >> (const IEInStream & stream, Identifier & input);

    /**
     * \brief   Writes identifier value to stream.
     * \param   stream  Streaming object to write identifier value
     * \param   input   Identifier to read values
     * \return  Returns instance of write streaming object
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const Identifier & output);

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if Identifier is valid. 
     *          Invalid Identifier has values of BAD_DENTIFIER.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Reset Identifier data. Set to BAD_DENTIFIER.
     **/
    inline void invalidate( void );

    /**
     * \brief   Returns Identifier string value
     **/
    inline const String & getName( void ) const;

    /**
     * \brief   Returns Identifier integer value
     **/
    inline unsigned int getValue( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Integer value of Identifier
     **/
    unsigned int    mValue;
    /**
     * \brief   String value of Identifier
     **/
    String          mName;
};

//////////////////////////////////////////////////////////////////////////
// Identifier class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const String& Identifier::convToString(unsigned int idValue, const std::vector<Identifier>& lookupList, unsigned int defIndex)
{
    ASSERT(defIndex < lookupList.size());
    for (const Identifier& entry : lookupList)
    {
        if (entry.mValue == idValue)
        {
            return entry.mName;
        }
    }

    return ((defIndex >= 0) && (defIndex < static_cast<uint32_t>(lookupList.size()))? lookupList[defIndex].mName : String::getEmptyString());
}

inline unsigned int Identifier::convFromString(const String& idName, const std::vector<Identifier>& lookupList, unsigned int defIndex)
{
    ASSERT(defIndex < lookupList.size());
    for (const Identifier& entry : lookupList)
    {
        if (entry.mName == idName)
        {
            return entry.mValue;
        }
    }

    return ((defIndex >= 0) && (defIndex < static_cast<uint32_t>(lookupList.size())) ? lookupList[defIndex].mValue : static_cast<uint32_t>(~0));
}

inline bool Identifier::operator == ( const Identifier & other ) const
{
    return (this == &other) || ((mValue == other.mValue) && (mName == other.mName));
}

inline bool Identifier::operator == ( const char * rhs ) const
{
    return (mName == rhs);
}

inline bool Identifier::operator == ( unsigned int rhs ) const
{
    return (mValue == rhs);
}

inline bool Identifier::operator != ( const Identifier & other ) const
{
    return (this != &other) && ((mValue != other.mValue) && (mName != other.mName));
}

inline bool Identifier::operator != ( const char * rhs ) const
{
    return (mName != rhs);
}

inline bool Identifier::operator != ( unsigned int rhs ) const
{
    return (mValue != rhs);
}

inline bool Identifier::isValid( void ) const
{
    return (*this != BAD_IDENTIFIER);
}

inline void Identifier::invalidate( void )
{
    mValue  = BAD_IDENTIFIER.mValue;
    mName   = BAD_IDENTIFIER.mName;
}

inline const String & Identifier::getName( void ) const
{
    return mName;
}

inline unsigned int Identifier::getValue( void ) const
{
    return mValue;
}

/************************************************************************
 * Friend functions. Declare to make it streamable
 * It is streamable object and the values can be streamed between different threads
 ************************************************************************/
inline const IEInStream& operator >> (const IEInStream& stream, Identifier& input)
{
    stream >> input.mValue;
    stream >> input.mName;
    return stream;
}

inline IEOutStream & operator << (IEOutStream& stream, const Identifier& output)
{
    stream << output.mValue;
    stream << output.mName;
    return stream;
}

#endif  // AREG_BASE_IDENTIFIER_HPP
