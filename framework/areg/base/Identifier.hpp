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
 *              Binds string and integer values to provide bidirectional
 *              conversion between string and integer representations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"

#include <string_view>
#include <vector>

namespace areg {

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
    //!< Reserved invalid identifier integer value.
    static constexpr uint32_t           BAD_IDENTIFIER_VALUE    { static_cast<uint32_t>(0x80000000) };

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    //!< Reserved invalid identifier name.
    static constexpr std::string_view   BAD_IDENTIFIER_NAME     { "_BAD_IDENTIFIER" };
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    //!< Static invalid identifier instance.
    static const Identifier             BAD_IDENTIFIER;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Identifier();

    Identifier(const Identifier& src);

    Identifier(Identifier&& src) noexcept;

    ~Identifier() = default;

    /**
     * \brief   Initializes the identifier with an integer value and name.
     *
     * \param   idValue     The integer value of the identifier.
     * \param   idName      The name associated with the integer value.
     **/
    Identifier(uint32_t idValue, const char * idName);

    Identifier(uint32_t idValue, std::string_view idName);

    Identifier(uint32_t idValue, const String& idName);

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts an integer value to its corresponding string representation using a lookup
     *          table.
     *
     * \param   idValue         The integer value to convert.
     * \param   lookupList      The lookup table to search for the identifier.
     * \param   defIndex        The index of the default string to return if the identifier is not
     *                          found; pass areg::INVALID_POSITION to return an empty string if
     *                          not found.
     * \return  Returns the string representation of the identifier if found; returns the default
     *          string if the identifier is not found and defIndex is valid; returns an empty string
     *          if not found and defIndex is invalid.
     **/
    [[nodiscard]]
    inline static const String& to_string( uint32_t idValue
                                         , const std::vector<Identifier>& lookupList
                                         , uint32_t defIndex ) noexcept;

    /**
     * \brief   Converts a string value to its corresponding integer representation using a lookup
     *          table.
     *
     * \param   idName          The string value to convert.
     * \param   lookupList      The lookup table to search for the identifier.
     * \param   defIndex        The index of the default integer to return if the identifier is not
     *                          found; pass areg::INVALID_POSITION to return 0xFFFFFFFF if not
     *                          found.
     * \return  Returns the integer representation of the identifier if found; returns the default
     *          integer if the identifier is not found and defIndex is valid; returns 0xFFFFFFFF if
     *          not found and defIndex is invalid.
     **/
    [[nodiscard]]
    inline static uint32_t from_string( const String& idName
                                      , const std::vector<Identifier>& lookupList
                                      , uint32_t defIndex );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    Identifier& operator = (const Identifier& src);

    Identifier& operator = (Identifier&& src) noexcept;

    [[nodiscard]]
    inline bool operator == (const Identifier& other) const;

    [[nodiscard]]
    inline bool operator != (const Identifier& other) const;

    [[nodiscard]]
    inline bool operator == (const char * rhs) const;

    [[nodiscard]]
    inline bool operator == (const String& rhs) const;

    [[nodiscard]]
    inline constexpr bool operator == (uint32_t rhs) const noexcept;

    [[nodiscard]]
    inline bool operator != (const char * rhs) const;

    [[nodiscard]]
    inline bool operator != (const String& rhs) const;

    [[nodiscard]]
    inline constexpr bool operator != (uint32_t rhs) const noexcept;

/************************************************************************
 * Friend functions to make it streamable
 ************************************************************************/

    /**
     * \brief   Reads identifier data from a stream.
     *
     * \param       stream  The input stream to read from.
     * \param[out]  input   The identifier to write the values into.
     * \return  Reference to the input stream.
     **/
    friend inline const areg::InStream& operator >> (const areg::InStream& stream, areg::Identifier& input);

    /**
     * \brief   Writes identifier data to a stream.
     *
     * \param   stream      The output stream to write to.
     * \param   output      The identifier to read the values from.
     * \return  Reference to the output stream.
     **/
    friend inline areg::OutStream& operator << (areg::OutStream& stream, const areg::Identifier& output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    [[nodiscard]]
    inline bool is_valid() const;

    inline void invalidate();

    [[nodiscard]]
    inline const String& name() const noexcept;

    [[nodiscard]]
    constexpr uint32_t value() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    uint32_t    mValue;     //!< Integer value of the identifier.
    String      mName;      //!< String name of the identifier.
};

//////////////////////////////////////////////////////////////////////////
// Identifier class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const String& Identifier::to_string( uint32_t idValue
                                           , const std::vector<Identifier>& lookupList
                                           , uint32_t defIndex ) noexcept
{
    for (const Identifier& entry : lookupList)
    {
        if (entry.mValue == idValue)
        {
            return entry.mName;
        }
    }

    return (defIndex < static_cast<uint32_t>(lookupList.size()) ? lookupList[defIndex].mName : String::empty_string());
}

inline uint32_t Identifier::from_string( const String& idName
                                        , const std::vector<Identifier>& lookupList
                                        , uint32_t defIndex )
{
    for (const Identifier& entry : lookupList)
    {
        if (entry.mName == idName)
        {
            return entry.mValue;
        }
    }

    return (defIndex < static_cast<uint32_t>(lookupList.size()) ? lookupList[defIndex].mValue : static_cast<uint32_t>(~0));
}

inline bool Identifier::operator == (const Identifier& other) const
{
    return (this == &other) || ((mValue == other.mValue) && (mName == other.mName));
}

inline bool Identifier::operator != (const Identifier& other) const
{
    return !(*this == other);
}

inline bool Identifier::operator == (const char * rhs) const
{
    return (mName == rhs);
}

inline bool Identifier::operator == (const String& rhs) const
{
    return (mName == rhs);
}

inline constexpr bool Identifier::operator == (uint32_t rhs) const noexcept
{
    return (mValue == rhs);
}

inline bool Identifier::operator != (const char * rhs) const
{
    return (mName != rhs);
}

inline bool Identifier::operator != (const String& rhs) const
{
    return (mName != rhs);
}

inline constexpr bool Identifier::operator != (uint32_t rhs) const noexcept
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

inline const String& Identifier::name() const noexcept
{
    return mName;
}

constexpr uint32_t Identifier::value() const noexcept
{
    return mValue;
}

/************************************************************************
 * Friend functions to make it streamable
 ************************************************************************/
inline const areg::InStream& operator >> (const areg::InStream& stream, areg::Identifier& input)
{
    stream >> input.mValue;
    stream >> input.mName;
    return stream;
}

inline areg::OutStream& operator << (areg::OutStream& stream, const areg::Identifier& output)
{
    stream << output.mValue;
    stream << output.mName;
    return stream;
}

} // namespace areg

#endif  // AREG_BASE_IDENTIFIER_HPP
