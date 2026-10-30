#ifndef AREG_BASE_WIDESTRING_HPP
#define AREG_BASE_WIDESTRING_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/WideString.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, WideString Class to handle basic
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
// WideString class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Wide-character string class declaration. Has basic functionalities
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

class AREG_API WideString : public StringBase<wchar_t, WideString>
 {
    using Base = StringBase<wchar_t, WideString>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the empty wide string singleton.
     **/
    static const WideString & empty_string();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    using Base::Base;

    WideString() noexcept = default;

    ~WideString() = default;

    WideString(const WideString& source) = default;

    WideString(WideString&& source) noexcept = default;

    inline explicit WideString( wchar_t ch );

    /**
     * \brief   Initializes string by reading from a stream object.
     *
     * \param   stream      The input stream to read string data from.
     **/
    explicit WideString( const InStream & stream );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:
    using Base::operator =;

    WideString & operator = (const WideString & src) = default;

    WideString & operator = (WideString && src) noexcept = default;

/************************************************************************/
// Friend global operators to stream WideString
/************************************************************************/
    /**
     * \brief   Reads string data from a stream.
     *
     * \param   stream      The input stream to read string data from.
     * \param[in,out] input       The WideString to initialize with data read from the stream.
     **/
    friend inline const InStream & operator >> (const InStream & stream, WideString & input);

    /**
     * \brief   Writes string data to a stream.
     *
     * \param[in,out] stream   The output stream to write string data to.
     * \param   output         The WideString to read data from.
     **/
    friend inline OutStream & operator << (OutStream & stream, const WideString & output);

/************************************************************************/
// WideString protected
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
// WideString class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline WideString::WideString( wchar_t ch )
    : Base( ch )
{
}

inline const InStream& operator >> (const InStream& stream, WideString& input)
{
    input.from_stream(stream);
    return stream;
}

inline OutStream& operator << (OutStream& stream, const WideString& output)
{
    output.to_stream(stream);
    return stream;
}

template<>
struct required_size <areg::WideString>
{
    [[nodiscard]]
    inline uint32_t operator ()(const areg::WideString& str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(wchar_t) * (str.length() + 1)));
    }
};

template<>
struct required_size <std::wstring>
{
    [[nodiscard]]
    inline uint32_t operator ()(const std::wstring& str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(wchar_t) * (str.length() + 1)));
    }
};

template<>
struct required_size <std::wstring_view>
{
    [[nodiscard]]
    inline uint32_t operator ()(const std::wstring_view str) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(wchar_t) * (str.length() + 1)));
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
    struct hash<areg::WideString>
    {
        //! An operator to convert String object to uint32_t.
        inline uint32_t operator()(const areg::WideString& key) const
        {
            return static_cast<uint32_t>(key);
            // return static_cast<uint32_t>(std::hash<std::wstring>{}(key.data()));
        }
    };
} // namespace std

#endif  // AREG_BASE_WIDESTRING_HPP
