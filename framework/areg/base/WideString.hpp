#ifndef AREG_BASE_WIDESTRING_HPP
#define AREG_BASE_WIDESTRING_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
    class String;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// WideString class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   16-bit string class declaration. Has basic functionalities
 *          to handle null-terminated string operations.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

class AREG_API WideString : public StringBase<wchar_t>
 {
    using Base = StringBase<wchar_t>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the empty wide string constant.
     **/
    static const WideString & empty_string();

    /**
     * \brief   WideString::EmptyString
     *          The empty wide string.
     **/
    static constexpr wchar_t EmptyString[ ]{ L"" };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    WideString() = default;

    ~WideString() = default;

    WideString(const WideString& source) = default;

    WideString(WideString&& source) noexcept = default;

    inline WideString(const wchar_t* source);

    inline WideString(const std::wstring& source);

    inline WideString(const std::wstring_view& source);

    inline WideString(std::wstring&& source) noexcept;

    inline WideString(const String& source);

    inline WideString(const std::string& source);

    inline WideString(const char* source);

    inline WideString(const wchar_t* source, uint32_t charCount);

    inline WideString(const char* source, uint32_t charCount);

    inline explicit WideString( wchar_t ch );

    /**
     * \brief   Reserves space for a specified number of characters without modifying the size.
     *
     * \param   count       The number of characters to reserve. Size remains zero.
     **/
    inline explicit WideString(uint32_t count);

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

    /**
     * \brief   Converts string to a 32-bit unsigned integer hash value.
     **/
    explicit inline operator uint32_t() const;

    inline WideString & operator = (const WideString & src);
    inline WideString & operator = (const std::wstring& src);
    inline WideString & operator = (const std::wstring_view& src);
    inline WideString & operator = (const wchar_t* src);
    inline WideString & operator = (const wchar_t src);
    inline WideString & operator = (const std::string& src);
    inline WideString & operator = (const char * src );
    inline WideString & operator = (const char src );
    inline WideString & operator = (WideString && src) noexcept;
    inline WideString & operator = (std::wstring && src) noexcept;
    WideString & operator = ( const String & src );

    inline bool operator == (const WideString& other) const;
    inline bool operator == (const std::wstring& other) const;
    inline bool operator == (const std::wstring_view& other) const;
    inline bool operator == (const wchar_t* other) const;
    inline bool operator == (const wchar_t ch) const;
    bool operator == (const char* other) const;
    bool operator == (const std::string& other) const;
    bool operator == (const String& other) const;

    inline bool operator != (const WideString& other) const;
    inline bool operator != (const std::wstring& other) const;
    inline bool operator != (const std::wstring_view& other) const;
    inline bool operator != (const wchar_t* other) const;
    inline bool operator != (const wchar_t ch) const;
    bool operator != (const char* other) const;
    bool operator != (const std::string& other) const;
    bool operator != (const String& other) const;

    inline WideString& operator += (const WideString& src);
    inline WideString& operator += (const std::wstring& src);
    inline WideString& operator += (const std::wstring_view& src);
    inline WideString& operator += (const wchar_t* src);
    inline WideString& operator += (const wchar_t chSource);
    inline WideString& operator += (const char* src);
    inline WideString& operator += (const char src);
    inline WideString& operator += (const std::string& src);
    WideString & operator += ( const String & src );

/************************************************************************/
// Friend global operators to operate WideString
/************************************************************************/

    friend inline WideString operator + (const WideString& lhs, const WideString& rhs);
    friend inline WideString operator + (const WideString& lhs, const std::wstring& rhs);
    friend inline WideString operator + (const WideString& lhs, const std::wstring_view& rhs);
    friend inline WideString operator + (const WideString& lhs, const wchar_t* rhs);
    friend inline WideString operator + (const WideString& lhs, const wchar_t rhs);
    friend inline WideString operator + (const std::wstring& lhs, const WideString& rhs);
    friend inline WideString operator + (const std::wstring_view& lhs, const WideString& rhs);
    friend inline WideString operator + (const wchar_t* lhs, const WideString& rhs);
    friend inline WideString operator + (const wchar_t lhs, const WideString& rhs);
    friend inline WideString operator + (const WideString & lhs, const char * rhs);
    friend inline  WideString operator + (const char * lhs, const WideString & rhs);

/************************************************************************/
// Friend global operators to stream WideString
/************************************************************************/
    /**
     * \brief   Reads string data from a stream.
     *
     * \param   stream      The input stream to read string data from.
     * \param[out] input       The WideString to initialize with data read from the stream.
     **/
    friend inline const InStream & operator >> (const InStream & stream, WideString & input);

    /**
     * \brief   Writes wide-string data to a stream.
     *
     * \param[in,out] stream   The output stream to write string data to.
     * \param   output         The String to read data from.
     **/
    friend inline OutStream & operator << (OutStream & stream, const WideString & output);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

public:
/************************************************************************/
// Static methods
/************************************************************************/
    /**
     * \brief   Extracts substring up to the specified phrase in a string buffer.
     *
     * \param   src             The source string buffer to search.
     * \param   strPhrase       The phrase to search for in the string.
     * \param[out] out_next        If not nullptr, receives pointer to the position after the phrase
     *                             in the source string, or nullptr if phrase not found.
     * \return  Returns the substring up to the matched phrase.
     *
     * \example substr
     *
     *  WideString test(L"0123 456 789 0123");
     *
     *  const wchar_t * next = static_cast<const wchar_t *>(test);             // next == L"0123 456 789 0123"
     *  WideString result1 = WideString::substr( next, L"0123", &next);        // result1 == L""          , next == L" 456 789 0123"
     *  WideString result2 = WideString::substr( next, L"0123", &next);        // result2 == L" 456 789 " , next == L""
     *  WideString result3 = WideString::substr( next, L"0123", &next);        // result3 == L""          , next == nullptr;
     *
     *  next = static_cast<const wchar_t *>(test);                             // next == L"0123 456 789 0123"
     *  WideString result4 = WideString::substr( next, L" ", &next);           // result4 == L"0123"      , next == L"456 789 0123"
     *  WideString result5 = WideString::substr( next, L" ", &next);           // result5 == L"456"       , next == L"789 0123"
     *  WideString result6 = WideString::substr( next, L" ", &next);           // result6 == L"789"       , next == L"0123";
     *  WideString result7 = WideString::substr( next, L" ", &next);           // result7 == L"0123"      , next == nullptr;
     **/
    static WideString substr( const wchar_t * src, const wchar_t * strPhrase, const wchar_t ** out_next = nullptr );

    /**
     * \brief   Converts a string of digits to a 32-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit signed integer.
     **/
    static int32_t make_int32( const wchar_t * strDigit, areg::Radix radix = areg::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit unsigned integer.
     **/
    static uint32_t make_uint32( const wchar_t * strDigit, areg::Radix radix = areg::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit signed integer.
     **/
    static int64_t make_int64( const wchar_t * strDigit, areg::Radix radix = areg::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit unsigned integer.
     **/
    static uint64_t make_uint64( const wchar_t * strDigit, areg::Radix radix = areg::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit floating-point number.
     **/
    static float make_float( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit floating-point number.
     **/
    static double make_double( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string to a boolean value.
     *
     * \param   strBoolean      The string to convert. Recognized as true if equal to "true"
     *                          (case-insensitive).
     * \param[out] end             If not nullptr, receives pointer to the next character after the
     *                             value.
     * \return  Returns true if string matches "true"; false otherwise.
     **/
    static bool make_bool( const wchar_t * strBoolean, const wchar_t ** end = nullptr );

    /**
     * \brief   Converts a 32-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static WideString make_string( int32_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static WideString make_string( uint32_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static WideString make_string( int64_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static WideString make_string( uint64_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit floating-point number to a string.
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static WideString make_string( float number );
    /**
     * \brief   Converts a 64-bit floating-point number to a string.
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static WideString make_string( double number );
    /**
     * \brief   Converts a boolean value to a string.
     *
     * \param   value       The boolean value to convert.
     * \return  Returns "true" or "false".
     **/
    static WideString make_string( bool value );

    /**
     * \brief   Formats a string using printf-like formatting rules.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string, followed by arguments to format.
     * \return  Returns the number of characters copied; zero if nothing copied; negative if error
     *          occurred.
     **/
    static int32_t format_string( wchar_t * strDst, int32_t count, const wchar_t * format, ... );

    /**
     * \brief   Formats a string using printf-like formatting rules and a va_list.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     * \return  Returns the number of characters copied; zero if nothing copied; negative if error occurred.
     **/
    static int32_t format_string_list( wchar_t * strDst, int32_t count, const wchar_t * format, va_list argptr );

/************************************************************************/
// Non-static methods
/************************************************************************/

    /**
     * \brief   Formats this string using printf-like formatting rules.
     *
     * \param   format      The format string, followed by arguments to format.
     * \note    Allocates 128 characters initially; retries with 512 characters if formatting fails.
     **/
    WideString& format(const wchar_t* format, ...);

    /**
     * \brief   Formats this string using printf-like formatting rules and a va_list.
     *
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     * \note    Allocates 128 characters initially; retries with 512 characters if formatting fails.
     **/
    WideString& format_list(const wchar_t* format, va_list argptr);

    /**
     * \brief   Assigns a ASCII-character string after converting to narrow characters.
     *
     * \param   source      The source wide-character string.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    WideString& assign(const char* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a wide-character string.
     *
     * \param   source      The source wide-character string.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline WideString& assign(const wchar_t* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a std::wstring from an optional starting position.
     *
     * \param   source      The source std::wstring.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline WideString& assign(const std::wstring& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a std::wstring_view from an optional starting position.
     *
     * \param   source      The source wstring_view.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline WideString& assign(const std::wstring_view& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a WideString from an optional starting position.
     *
     * \param   source      The source WideString.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline WideString& assign(const WideString& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a single wide character.
     **/
    inline WideString& assign(const wchar_t ch);

    /**
     * \brief   Appends a C-string after converting to narrow characters.
     *
     * \param   source      The source C-string.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    WideString& append(const char* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a wide character string.
     *
     * \param   source      The source wide character string.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline WideString& append(const wchar_t* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends characters from a std::wstring starting at a position.
     *
     * \param   source      The source wide string to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline WideString& append(const std::wstring& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends characters from a std::wstring_view starting at a position.
     *
     * \param   source      The source wide string view to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline WideString& append(const std::wstring_view& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends characters from another WideString starting at a position.
     *
     * \param   source      The source WideString to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline WideString& append(const WideString& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a single wide character.
     **/
    inline WideString& append(const wchar_t ch);

    /**
     * \brief   Converts this string to a 32-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 32-bit signed integer.
     **/
    [[nodiscard]]
    inline int32_t to_int32( areg::Radix radix = areg::Radix::Decimal ) const;
    /**
     * \brief   Converts string to a 32-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 32-bit unsigned integer.
     **/
    [[nodiscard]]
    inline uint32_t to_uint32( areg::Radix radix = areg::Radix::Decimal ) const;
    /**
     * \brief   Converts string to a 64-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 64-bit signed integer.
     **/
    [[nodiscard]]
    inline int64_t to_int64( areg::Radix radix = areg::Radix::Decimal ) const;
    /**
     * \brief   Converts string to a 64-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 64-bit unsigned integer.
     **/
    [[nodiscard]]
    inline uint64_t to_uint64( areg::Radix radix = areg::Radix::Decimal ) const;
    /**
     * \brief   Converts string to a 32-bit floating-point number.
     *
     * \return  Returns the 32-bit floating-point number.
     **/
    [[nodiscard]]
    inline float to_float() const;
    /**
     * \brief   Converts string to a 64-bit floating-point number.
     *
     * \return  Returns the 64-bit floating-point number.
     **/
    [[nodiscard]]
    inline double to_double() const;
    /**
     * \brief   Converts string to a boolean value.
     *
     * \return  Returns true if string equals "true" (case-insensitive); false otherwise.
     **/
    [[nodiscard]]
    inline bool to_bool() const;

    /**
     * \brief   Converts a 32-bit signed integer to this string.
     *
     * \param   value       The integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    Negative values include a minus sign; for non-decimal bases, use from_uint32() for
     *          unsigned representation.
     **/
    inline WideString & from_int32( int32_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit unsigned integer to this string.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    For hexadecimal, prepends "0x"; total length is 10 characters ("0x" + 8 hex digits).
     **/
    inline WideString & from_uint32( uint32_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit signed integer to this string.
     *
     * \param   value       The integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    Negative values include a minus sign; for non-decimal bases, use from_uint64() for
     *          unsigned representation.
     **/
    inline WideString & from_int64( int64_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit unsigned integer to this string.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    For hexadecimal, prepends "0x"; total length is 18 characters ("0x" + 16 hex digits).
     **/
    inline WideString & from_uint64( uint64_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit floating-point number to this string.
     **/
    inline WideString & from_float( float value );
    /**
     * \brief   Converts a 64-bit floating-point number to this string.
     **/
    inline WideString & from_double( double value );
    /**
     * \brief   Converts a boolean value to this string.
     **/
    inline WideString & from_bool( bool value );

/************************************************************************/
// WideString protected
/************************************************************************/
protected:
    /**
     * \brief   Reads string data from a stream object.
     *
     * \param   stream      The input stream to read string data from.
     **/
    void read_stream(const InStream & stream);

    /**
     * \brief   Writes string data to a stream object.
     *
     * \param[in,out] stream      The output stream to write string data to.
     **/
    void write_stream(OutStream & stream) const;
};
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// WideString class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline WideString::WideString(const wchar_t* source)
    : StringBase<wchar_t>(source)
{
}

inline WideString::WideString(const std::wstring& source)
    : StringBase<wchar_t>(source)
{
}

inline WideString::WideString(const std::wstring_view& source)
    : StringBase<wchar_t>(source)
{
}

inline WideString::WideString(std::wstring&& source) noexcept
    : StringBase<wchar_t>( std::move( source ) )
{
}

inline WideString::WideString(const std::string& source)
    : StringBase<wchar_t>()
{
    assign(source.c_str(), static_cast<areg::CharCount>(source.length()));
}

inline WideString::WideString(const char* source)
    : StringBase<wchar_t>()
{
    assign(source, areg::COUNT_ALL);
}

inline WideString::WideString(const wchar_t* source, uint32_t charCount)
    : StringBase<wchar_t>(source, static_cast<areg::CharCount>(charCount))
{
}

inline WideString::WideString(const char* source, uint32_t charCount)
    : StringBase<wchar_t>()
{
    assign(source, static_cast<areg::CharCount>(charCount));
}

inline WideString::WideString( wchar_t ch )
    : StringBase<wchar_t>( ch )
{
}

inline WideString::WideString(uint32_t count)
    : StringBase<wchar_t>( static_cast<uint32_t>(count) )
{
}

inline WideString::operator uint32_t() const
{
    return areg::crc32_calculate(mData.c_str());
}

inline WideString& WideString::operator = (const char* src)
{
    assign(src, areg::COUNT_ALL);
    return (*this);
}

inline WideString& WideString::operator = (const char src)
{
    (*this) = static_cast<wchar_t>(src);
    return (*this);
}

inline WideString& WideString::operator = (const WideString& src)
{
    Base::operator = (static_cast<const Base&>(src));
    return (*this);
}

inline WideString& WideString::operator = (const std::wstring& src)
{
    Base::operator = (src);
    return (*this);
}

inline WideString& WideString::operator = (const std::string& src)
{
    assign(src.c_str(), static_cast<areg::CharCount>(src.length()));
    return (*this);
}

inline WideString& WideString::operator = (const std::wstring_view& src)
{
    Base::operator = (src);
    return (*this);
}

inline WideString& WideString::operator = (const wchar_t* src)
{
    Base::operator = (src);
    return (*this);
}

inline WideString& WideString::operator = (WideString&& src) noexcept
{
    Base::operator = (std::move(src));
    return (*this);
}

inline WideString& WideString::operator = (std::wstring&& src) noexcept
{
    Base::mData = std::move(src);
    return (*this);
}

inline WideString& WideString::operator = (const wchar_t src)
{
    Base::operator = (src);
    return (*this);
}

inline bool WideString::operator == (const WideString& other) const
{
    return Base::operator==(static_cast<const Base&>(other));
}

inline bool WideString::operator == (const std::wstring& other) const
{
    return Base::operator==(other);
}

inline bool WideString::operator == (const std::wstring_view& other) const
{
    return Base::operator==(other);
}

inline bool WideString::operator == (const wchar_t* other) const
{
    return Base::operator==(other);
}

inline bool WideString::operator == (const wchar_t ch) const
{
    return Base::operator==(ch);
}

inline bool WideString::operator != (const WideString& other) const
{
    return Base::operator!=(static_cast<const Base&>(other));
}

inline bool WideString::operator != (const std::wstring& other) const
{
    return Base::operator!=(other);
}

inline bool WideString::operator != (const std::wstring_view& other) const
{
    return Base::operator!=(other);
}

inline bool WideString::operator != (const wchar_t* other) const
{
    return Base::operator!=(other);
}

inline bool WideString::operator != (const wchar_t ch) const
{
    return Base::operator != (ch);
}

inline WideString& WideString::operator += (const WideString& src)
{
    Base::operator+=(static_cast<const Base&>(src));
    return (*this);
}

inline WideString& WideString::operator += (const std::wstring& src)
{
    Base::operator+=(src);
    return (*this);
}

inline WideString& WideString::operator += (const std::wstring_view& src)
{
    Base::operator+=(src);
    return (*this);
}

inline WideString& WideString::operator += (const wchar_t* src)
{
    Base::operator+=(src);
    return (*this);
}

inline WideString& WideString::operator += (const wchar_t chSource)
{
    Base::operator+=(chSource);
    return (*this);
}

inline WideString& WideString::operator += (const char* src)
{
    append(src, areg::COUNT_ALL);
    return (*this);
}

inline WideString& WideString::operator += (const char chSource)
{
    append(&chSource, 1);
    return (*this);
}

inline WideString& WideString::operator += (const std::string& src)
{
    append(src.c_str(), static_cast<areg::CharCount>(src.length()));
    return (*this);
}

inline WideString operator + (const WideString& lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const WideString& lhs, const std::wstring& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const WideString& lhs, const std::wstring_view& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const WideString& lhs, const wchar_t* rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const WideString& lhs, const wchar_t rhs)
{
    WideString result(lhs);
    result += rhs;
    return result;
}

inline WideString operator + (const std::wstring& lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const std::wstring_view& lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const wchar_t* lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const wchar_t lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const WideString& lhs, const char* rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline WideString operator + (const char* lhs, const WideString& rhs)
{
    WideString result(lhs);
    result.append(rhs);
    return result;
}

inline const InStream& operator >> (const InStream& stream, WideString& input)
{
    input.read_stream(stream);
    return stream;
}

inline OutStream& operator << (OutStream& stream, const WideString& output)
{
    output.write_stream(stream);
    return stream;
}

inline int32_t WideString::to_int32( areg::Radix radix /*= areg::Decimal */ ) const
{
    return WideString::make_int32(as_string(), radix, nullptr );
}

inline uint32_t WideString::to_uint32( areg::Radix radix /*= areg::Decimal */ ) const
{
    return WideString::make_uint32(as_string(), radix, nullptr );
}

inline int64_t WideString::to_int64( areg::Radix radix /*= areg::Decimal */ ) const
{
    return WideString::make_int64(as_string(), radix, nullptr );
}

inline uint64_t WideString::to_uint64( areg::Radix radix /*= areg::Decimal */ ) const
{
    return WideString::make_uint64(as_string(), radix, nullptr );
}

inline float WideString::to_float() const
{
    return WideString::make_float(as_string(), nullptr );
}

inline double WideString::to_double() const
{
    return WideString::make_double(as_string(), nullptr );
}

inline bool WideString::to_bool() const
{
    return (is_empty() || areg::compare_ignore_case<wchar_t, char>(as_string(), areg::BOOLEAN_FALSE.data()) == areg::Ordering::Equal ? false : true);
}

inline WideString & WideString::from_int32( int32_t value, areg::Radix radix /*= areg::Decimal */ )
{
    *this = WideString::make_string(value, radix);
    return (*this);
}

inline WideString & WideString::from_uint32( uint32_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = WideString::make_string( value, radix );
    return (*this);
}

inline WideString & WideString::from_int64( int64_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = WideString::make_string( value, radix );
    return (*this);
}

inline WideString & WideString::from_uint64( uint64_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = WideString::make_string( value, radix );
    return (*this);
}

inline WideString & WideString::from_float( float value )
{
    (*this) = WideString::make_string( value );
    return (*this);
}

inline WideString & WideString::from_double( double value )
{
    (*this) = WideString::make_string( value );
    return (*this);
}

inline WideString & WideString::from_bool( bool value )
{
    (*this) = WideString::make_string( value );
    return (*this);
}

inline WideString& WideString::assign(const wchar_t* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(source, count);
    return (*this);
}

inline WideString& WideString::assign(const std::wstring& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string<wchar_t> &>(source), pos, count);
    return (*this);
}

inline WideString& WideString::assign(const std::wstring_view& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string_view<wchar_t> &>(source), pos, count);
    return (*this);
}

inline WideString& WideString::assign(const WideString& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(source, pos, count);
    return (*this);
}

inline WideString & WideString::assign(const wchar_t ch)
{
    Base::assign(ch);
    return (*this);
}

inline WideString& WideString::append(const wchar_t* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(source, count);
    return (*this);
}

inline WideString& WideString::append(const std::wstring& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string<wchar_t>&>(source), pos, count);
    return (*this);
}

inline WideString& WideString::append(const std::wstring_view& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string_view<wchar_t>&>(source), pos, count);
    return (*this);
}

inline WideString& WideString::append(const WideString& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const Base&>(source), pos, count);
    return (*this);
}

inline WideString & WideString::append(const wchar_t ch)
{
    Base::append(ch);
    return (*this);
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
