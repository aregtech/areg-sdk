#ifndef AREG_BASE_STRING_HPP
#define AREG_BASE_STRING_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/String.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/StringBase.hpp"

#include <functional>
#include <stdarg.h>
#include <stdio.h>
#include <string_view>

/************************************************************************
 * Dependencies.
 ************************************************************************/
namespace areg {
    class InStream;
    class OutStream;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// String class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   8-bit string class declaration. Has basic functionalities
 *          to handle null-terminated string operations.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer. The operations are
 *          implemented in StringBase.
 **/
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

class AREG_API String : public StringBase<char, String>
 {
    using Base = StringBase<char, String>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the empty string singleton.
     **/
    static const String & empty_string();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    using Base::Base;

    String() noexcept = default;

    ~String() = default;

    String(const String& source) = default;

    String(String&& source) noexcept = default;

    inline explicit String( char ch );

    /**
     * \brief   Initializes string by reading from a stream object.
     *
     * \param   stream      The input stream to read string data from.
     **/
    explicit String( const InStream & stream );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:
    using Base::operator =;

    String & operator = (const String & src) = default;

    String & operator = (String && src) noexcept = default;

/************************************************************************/
// Friend global operators to stream String
/************************************************************************/
    /**
     * \brief   Reads string data from a stream.
     *
     * \param   stream      The input stream to read string data from.
     * \param[in,out] input       The String to initialize with data read from the stream.
     **/
    friend inline const InStream & operator >> (const InStream & stream, String & input);

    /**
     * \brief   Writes string data to a stream.
     *
     * \param[in,out] stream   The output stream to write string data to.
     * \param   output         The String to read data from.
     **/
    friend inline OutStream & operator << (OutStream & stream, const String & output);

/************************************************************************/
// String protected
/************************************************************************/
protected:
    /**
     * \brief   Reads string data from a stream object.
     *
     * \param   stream      The input stream to read string data from.
     **/
    void from_stream(const InStream & stream);

    /**
     * \brief   Writes string data to a stream object.
     *
     * \param[in,out] stream      The output stream to write string data to.
     **/
    void to_stream(OutStream & stream) const;
};
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// String class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline String::String( char ch )
    : Base( ch )
{
}

inline const InStream& operator >> (const InStream& stream, String& input)
{
    input.from_stream(stream);
    return stream;
}

inline OutStream& operator << (OutStream& stream, const String& output)
{
    output.to_stream(stream);
    return stream;
}

template<>
struct required_size <areg::String>
{
    [[nodiscard]]
    inline uint32_t operator ()(const areg::String& str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(char) * (str.length() + 1)));
    }
};

template<>
struct required_size <std::string>
{
    [[nodiscard]]
    inline uint32_t operator ()(const std::string& str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(char) * (str.length() + 1)));
    }
};

template<>
struct required_size <std::string_view>
{
    [[nodiscard]]
    inline uint32_t operator ()(const std::string_view str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(char) * (str.length() + 1)));
    }
};

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// Hasher of WideString class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the WideString.
 */
namespace std {
    template<>
    struct hash<areg::String>
    {
        //! An operator to convert String object to uint32_t.
        inline uint32_t operator()(const areg::String& key) const
        {
            return static_cast<uint32_t>(key);
            // return static_cast<uint32_t>(std::hash<std::string>{}(key.data()));
        }
    };
} // namespace std

#endif  // AREG_BASE_STRING_HPP
