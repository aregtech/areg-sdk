#ifndef AREG_BASE_IDENTIFIER_HPP
#define AREG_BASE_IDENTIFIER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Identifier.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Object Identifier class.
 *              The Identifier is binding string and integer values to
 *              provide conversion from string to integer and from 
 *              integer to string.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"

#include <string_view>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Identifier class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Binds string and integer values to provide bidirectional conversion between string and
 *          integer representations, used for passing enum values as strings.
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
    static constexpr uint32_t       BAD_IDENTIFIER_VALUE    { static_cast<uint32_t>(0x80000000) };

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
     * \brief   Creates an invalid identifier with default values.
     **/
    Identifier();

    /**
     * \brief   Initializes the identifier with an integer value and name.
     *
     * \param   idValue     The integer value of the identifier.
     * \param   idName      The name associated with the integer value.
     **/
    Identifier(uint32_t idValue, const char* idName);
    /**
     * \brief   Initializes the identifier with an integer value and name.
     *
     * \param   idValue     The integer value of the identifier.
     * \param   idName      The name associated with the integer value.
     **/
    Identifier(uint32_t idValue, const std::string_view& idName);
    /**
     * \brief   Initializes the identifier with an integer value and name.
     *
     * \param   idValue     The integer value of the identifier.
     * \param   idName      The name associated with the integer value.
     **/
    Identifier(uint32_t idValue, const String& idName);

    /**
     * \brief
     *
     * \param   src     The source identifier to copy.
     **/
    Identifier(const Identifier & src);

    /**
     * \brief
     *
     * \param   src     The source identifier to move.
     * \note    Move constructor.
     **/
    Identifier( Identifier && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    ~Identifier() = default;

//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts an integer value to its corresponding string representation using a lookup
     *          table.
     *
     * \param   idValue         The integer value to convert.
     * \param   lookupList      The lookup table to search for the identifier.
     * \param   defIndex        The index of the default string to return if the identifier is not
     *                          found; pass NECommon::INVALID_POSITION to return an empty string if
     *                          not found.
     * \return  Returns the string representation of the identifier if found; returns the default
     *          string if the identifier is not found and defIndex is valid; returns an empty string
     *          if not found and defIndex is invalid.
     **/
    inline static const String& to_string(uint32_t idValue, const std::vector<Identifier>& lookupList, uint32_t defIndex);

    /**
     * \brief   Converts a string value to its corresponding integer representation using a lookup
     *          table.
     *
     * \param   idName          The string value to convert.
     * \param   lookupList      The lookup table to search for the identifier.
     * \param   defIndex        The index of the default integer to return if the identifier is not
     *                          found; pass NECommon::INVALID_POSITION to return 0xFFFFFFFF if not
     *                          found.
     * \return  Returns the integer representation of the identifier if found; returns the default
     *          integer if the identifier is not found and defIndex is valid; returns 0xFFFFFFFF if
     *          not found and defIndex is invalid.
     **/
    inline static uint32_t conv_from_string(const String& idName, const std::vector<Identifier>& lookupList, uint32_t defIndex);

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies identifier data from the source.
     *
     * \param   src     The source identifier to copy from.
     * \return  Reference to this identifier.
     **/
    Identifier & operator = (const Identifier & src);

    /**
     * \brief   Moves identifier data from the source.
     *
     * \param   src     The source identifier to move from.
     * \return  Reference to this identifier.
     **/
    Identifier & operator = ( Identifier && src ) noexcept;

    /**
     * \brief   Returns true if two identifiers are equal.
     *
     * \param   other       The identifier to compare.
     * \return  Returns true if both identifiers are equal; false otherwise.
     **/
    inline bool operator == (const Identifier & other) const;

    /**
     * \brief   Returns true if the identifier name equals the given string.
     *
     * \param   rhs     The string to compare with the identifier name.
     * \return  Returns true if the identifier name matches the string; false otherwise.
     **/
    inline bool operator == (const char * rhs) const;

    /**
     * \brief   Returns true if the identifier value equals the given integer.
     *
     * \param   rhs     The integer to compare with the identifier value.
     * \return  Returns true if the identifier value matches the integer; false otherwise.
     **/
    inline bool operator == (uint32_t rhs) const;

    /**
     * \brief   Returns true if two identifiers are not equal.
     *
     * \param   other       The identifier to compare.
     * \return  Returns true if the identifiers are not equal; false otherwise.
     **/
    inline bool operator != ( const Identifier & other ) const;

    /**
     * \brief   Returns true if the identifier name does not equal the given string.
     *
     * \param   rhs     The string to compare with the identifier name.
     * \return  Returns true if the identifier name does not match the string; false otherwise.
     **/
    inline bool operator != ( const char * rhs ) const;

    /**
     * \brief   Returns true if the identifier value does not equal the given integer.
     *
     * \param   rhs     The integer to compare with the identifier value.
     * \return  Returns true if the identifier value does not match the integer; false otherwise.
     **/
    inline bool operator != ( uint32_t rhs ) const;

/************************************************************************
 * Friend functions. Declare to make it streamable
 ************************************************************************/

    /**
     * \brief   Reads identifier data from a stream.
     *
     * \param   stream      The input stream to read from.
     * \param[out] input       The identifier to write the values into.
     * \return  Reference to the input stream.
     **/
    friend inline const InStream & operator >> (const InStream & stream, Identifier & input);

    /**
     * \brief   Writes identifier data to a stream.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The identifier to read the values from.
     * \return  Reference to the output stream.
     **/
    friend inline OutStream & operator << (OutStream & stream, const Identifier & output);

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if the identifier is valid.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Resets the identifier to an invalid state.
     **/
    inline void invalidate();

    /**
     * \brief   Returns the identifier's string name.
     **/
    inline const String & name() const;

    /**
     * \brief   Returns the identifier's integer value.
     **/
    inline uint32_t value() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Integer value of Identifier
     **/
    uint32_t    mValue;
    /**
     * \brief   String value of Identifier
     **/
    String          mName;
};

//////////////////////////////////////////////////////////////////////////
// Identifier class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const String& Identifier::to_string(uint32_t idValue, const std::vector<Identifier>& lookupList, uint32_t defIndex)
{
    ASSERT(defIndex < lookupList.size());
    for (const Identifier& entry : lookupList)
    {
        if (entry.mValue == idValue)
        {
            return entry.mName;
        }
    }

    return (defIndex < static_cast<uint32_t>(lookupList.size())? lookupList[defIndex].mName : String::empty_string());
}

inline uint32_t Identifier::conv_from_string(const String& idName, const std::vector<Identifier>& lookupList, uint32_t defIndex)
{
    ASSERT(defIndex < lookupList.size());
    for (const Identifier& entry : lookupList)
    {
        if (entry.mName == idName)
        {
            return entry.mValue;
        }
    }

    return (defIndex < static_cast<uint32_t>(lookupList.size()) ? lookupList[defIndex].mValue : static_cast<uint32_t>(~0));
}

inline bool Identifier::operator == ( const Identifier & other ) const
{
    return (this == &other) || ((mValue == other.mValue) && (mName == other.mName));
}

inline bool Identifier::operator == ( const char * rhs ) const
{
    return (mName == rhs);
}

inline bool Identifier::operator == ( uint32_t rhs ) const
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

inline bool Identifier::operator != ( uint32_t rhs ) const
{
    return (mValue != rhs);
}

inline bool Identifier::is_valid() const
{
    return (*this != BAD_IDENTIFIER);
}

inline void Identifier::invalidate()
{
    mValue  = BAD_IDENTIFIER.mValue;
    mName   = BAD_IDENTIFIER.mName;
}

inline const String & Identifier::name() const
{
    return mName;
}

inline uint32_t Identifier::value() const
{
    return mValue;
}

/************************************************************************
 * Friend functions. Declare to make it streamable
 * It is streamable object and the values can be streamed between different threads
 ************************************************************************/
inline const InStream& operator >> (const InStream& stream, Identifier& input)
{
    stream >> input.mValue;
    stream >> input.mName;
    return stream;
}

inline OutStream & operator << (OutStream& stream, const Identifier& output)
{
    stream << output.mValue;
    stream << output.mName;
    return stream;
}

#endif  // AREG_BASE_IDENTIFIER_HPP
