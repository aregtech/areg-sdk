#ifndef AREG_BASE_STRING_HPP
#define AREG_BASE_STRING_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
    class WideString;
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
 *          to covert string value to integer.
 **/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

class AREG_API String : public StringBase<char>
 {
    friend class BufferStreamBase;

    using Base = StringBase<char>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the empty string singleton.
     **/
    static const String & empty_string();

    /**
     * \brief   String::EmptyString
     *          The empty string.
     **/
    static constexpr char EmptyString[ ]{ "" };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    String() = default;

    ~String() = default;

    String(const String& source) = default;

    String(String&& source) noexcept = default;

    inline String(const char* source);

    inline String(const std::string& source);

    inline String(std::string_view source);

    inline String(std::string&& source) noexcept;

    inline String(const WideString& source);

    inline String(const std::wstring& source);

    inline String(const wchar_t* source);

    inline String(const char* source, uint32_t charCount);

    inline String(const wchar_t* source, uint32_t charCount);

    inline explicit String( char ch );

    /**
     * \brief   Reserves space for a specified number of characters without modifying the size.
     *
     * \param   count       The number of characters to reserve. Size remains zero.
     **/
    inline explicit String(uint32_t count);

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

    /**
     * \brief   Converts string to a 32-bit unsigned integer hash value.
     **/
    explicit inline operator uint32_t() const;

    inline String & operator = (const String & src);
    inline String & operator = (const std::string& src);
    inline String & operator = (std::string_view src);
    inline String & operator = (const char* src);
    inline String & operator = (const char src);
    inline String & operator = (const std::wstring& src);
    inline String & operator = (const wchar_t * src );
    inline String & operator = (const wchar_t src);
    inline String & operator = (String && src) noexcept;
    inline String & operator = (std::string && src) noexcept;
    String & operator = ( const WideString & src );

    inline bool operator == (const String& other) const;
    inline bool operator == (const std::string& other) const;
    inline bool operator == (std::string_view other) const;
    inline bool operator == (const char* other) const;
    inline bool operator == (const char ch) const;
    bool operator == (const wchar_t* other) const;
    bool operator == (const std::wstring& other) const;
    bool operator == (const WideString& other) const;

    inline bool operator != (const String& other) const;
    inline bool operator != (const std::string& other) const;
    inline bool operator != (std::string_view other) const;
    inline bool operator != (const char* other) const;
    inline bool operator != (const char ch) const;
    bool operator != (const wchar_t* other) const;
    bool operator != (const std::wstring& other) const;
    bool operator != (const WideString& other) const;

    inline String& operator += (const String& src);
    inline String& operator += (const std::string& src);
    inline String& operator += (std::string_view src);
    inline String& operator += (const char* src);
    inline String& operator += (const char chSource);
    inline String& operator += (const wchar_t* src);
    inline String& operator += (const wchar_t src);
    inline String& operator += (const std::wstring& src);
    String & operator += ( const WideString & src );

/************************************************************************/
// Friend global operators to operate String
/************************************************************************/

    friend inline String operator + (const String& lhs, const String& rhs);
    friend inline String operator + (const String& lhs, const std::string& rhs);
    friend inline String operator + (const String& lhs, std::string_view rhs);
    friend inline String operator + (const String& lhs, const char* rhs);
    friend inline String operator + (const String& lhs, const char rhs);
    friend inline String operator + (const std::string& lhs, const String& rhs);
    friend inline String operator + (std::string_view lhs, const String& rhs);
    friend inline String operator + (const char* lhs, const String& rhs);
    friend inline String operator + (const char lhs, const String& rhs);
    friend inline String operator + (const String & lhs, const wchar_t * rhs);
    friend inline  String operator + (const wchar_t * lhs, const String & rhs);

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
     *  String test("0123 456 789 0123");
     *
     *  const char * next = static_cast<const char *>(test);          // next == "0123 456 789 0123"
     *  String result1 = String::substr( next, "0123", &next);        // result1 == ""          , next == " 456 789 0123"
     *  String result2 = String::substr( next, "0123", &next);        // result2 == " 456 789 " , next == ""
     *  String result3 = String::substr( next, "0123", &next);        // result3 == ""          , next == nullptr;
     *
     *  next = static_cast<const char *>(test);                       // next == "0123 456 789 0123"
     *  String result4 = String::substr( next, " ", &next);           // result4 == "0123"      , next == "456 789 0123"
     *  String result5 = String::substr( next, " ", &next);           // result5 == "456"       , next == "789 0123"
     *  String result6 = String::substr( next, " ", &next);           // result6 == "789"       , next == "0123";
     *  String result7 = String::substr( next, " ", &next);           // result7 == "0123"      , next == nullptr;
     **/
    static String substr( const char * src, const char * strPhrase, const char ** out_next = nullptr );

    /**
     * \brief   Converts a string of digits to a 32-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit signed integer.
     **/
    static int32_t make_int32( const char * strDigit, areg::Radix radix = areg::Radix::Decimal, const char ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit unsigned integer.
     **/
    static uint32_t make_uint32( const char * strDigit, areg::Radix radix = areg::Radix::Decimal, const char ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit signed integer.
     **/
    static int64_t make_int64( const char * strDigit, areg::Radix radix = areg::Radix::Decimal, const char ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit unsigned integer.
     **/
    static uint64_t make_uint64( const char * strDigit, areg::Radix radix = areg::Radix::Decimal, const char ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 32-bit floating-point number.
     **/
    static float make_float( const char * strDigit, const char ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end         If not nullptr, receives pointer to the next character after the
     *                         numerical value.
     * \return  Returns the 64-bit floating-point number.
     **/
    static double make_double( const char * strDigit, const char ** end = nullptr );
    /**
     * \brief   Converts a string to a boolean value.
     *
     * \param   strBoolean      The string to convert. Recognized as true if equal to "true"
     *                          (case-insensitive).
     * \param[out] end             If not nullptr, receives pointer to the next character after the
     *                             value.
     * \return  Returns true if string matches "true"; false otherwise.
     **/
    static bool make_bool( const char * strBoolean, const char ** end = nullptr );

    /**
     * \brief   Converts a 32-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static String make_string( int32_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static String make_string( uint32_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static String make_string( int64_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the converted string.
     **/
    static String make_string( uint64_t number, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit floating-point number to a string.
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static String make_string( float number );
    /**
     * \brief   Converts a 64-bit floating-point number to a string.
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static String make_string( double number );
    /**
     * \brief   Converts a boolean value to a string.
     *
     * \param   value       The boolean value to convert.
     * \return  Returns "true" or "false".
     **/
    static String make_string( bool value );

    /**
     * \brief   Formats a string using printf-like formatting rules.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string, followed by arguments to format.
     * \return  Returns the number of characters copied; zero if nothing copied; negative if error
     *          occurred.
     **/
    static int32_t format_string( char * strDst, int32_t count, const char * format, ... );

    /**
     * \brief   Formats a string using printf-like formatting rules and a va_list.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     * \return  Returns the number of characters copied; zero if nothing copied; negative if error occurred.
     **/
    static int32_t format_string_list( char * strDst, int32_t count, const char * format, va_list argptr );

/************************************************************************/
// Non-static methods
/************************************************************************/

    /**
     * \brief   Formats this string using printf-like formatting rules.
     *
     * \param   format      The format string, followed by arguments to format.
     * \note    Allocates 128 characters initially; retries with 512 characters if formatting fails.
     **/
    String& format(const char* format, ...);

    /**
     * \brief   Formats this string using printf-like formatting rules and a va_list.
     *
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     * \note    Allocates 128 characters initially; retries with 512 characters if formatting fails.
     **/
    String& format_list(const char* format, va_list argptr);

    /**
     * \brief   Assigns a wide-character string after converting to narrow characters.
     *
     * \param   source      The source wide-character string.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    String& assign(const wchar_t* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a C-string.
     *
     * \param   source      The source C-string.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline String& assign(const char* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a std::string from an optional starting position.
     *
     * \param   source      The source std::string.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline String& assign(const std::string& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a std::string_view from an optional starting position.
     *
     * \param   source      The source string_view.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline String& assign(std::string_view source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a String from an optional starting position.
     *
     * \param   source      The source String.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline String& assign(const String& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Assigns a single character.
     **/
    inline String& assign(const char ch);

    /**
     * \brief   Appends a wide-character string after converting to narrow characters.
     *
     * \param   source      The source wide-character string.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    String& append(const wchar_t* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a C-string.
     *
     * \param   source      The source C-string.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline String& append(const char* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a std::string from an optional starting position.
     *
     * \param   source      The source std::string.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline String& append(const std::string& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a string_view from an optional starting position.
     *
     * \param   source      The source string_view.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline String& append(std::string_view source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a String from an optional starting position.
     *
     * \param   source      The source String.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline String& append(const String& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends a single character.
     **/
    inline String& append(const char ch);

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
    inline String & from_int32( int32_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit unsigned integer to this string.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    For hexadecimal, prepends "0x"; total length is 10 characters ("0x" + 8 hex digits).
     **/
    inline String & from_uint32( uint32_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit signed integer to this string.
     *
     * \param   value       The integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    Negative values include a minus sign; for non-decimal bases, use from_uint64() for
     *          unsigned representation.
     **/
    inline String & from_int64( int64_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 64-bit unsigned integer to this string.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     * \note    For hexadecimal, prepends "0x"; total length is 18 characters ("0x" + 16 hex digits).
     **/
    inline String & from_uint64( uint64_t value, areg::Radix radix = areg::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit floating-point number to this string.
     **/
    inline String & from_float( float value );
    /**
     * \brief   Converts a 64-bit floating-point number to this string.
     **/
    inline String & from_double( double value );
    /**
     * \brief   Converts a boolean value to this string.
     **/
    inline String & from_bool( bool value );

/************************************************************************/
// String protected
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
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// String class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline String::String(const char* source)
    : StringBase<char>(source)
{
}

inline String::String(const std::string& source)
    : StringBase<char>(source)
{
}

inline String::String(std::string_view source)
    : StringBase<char>(source)
{
}

inline String::String(std::string&& source) noexcept
    : StringBase<char>(std::move(source))
{
}

inline String::String(const std::wstring& source)
    : StringBase<char>()
{
    assign(source.c_str(), static_cast<areg::CharCount>(source.length()));
}

inline String::String(const wchar_t* source)
    : StringBase<char>()
{
    assign(source, areg::COUNT_ALL);
}

inline String::String(const char* source, uint32_t charCount)
    : StringBase<char>(source, static_cast<areg::CharCount>(charCount))
{
}

inline String::String(const wchar_t* source, uint32_t charCount)
    : StringBase<char>()
{
    assign(source, static_cast<areg::CharCount>(charCount));
}

inline String::String( char ch )
    : StringBase<char>( ch )
{
}

inline String::String(uint32_t count)
    : StringBase<char>( static_cast<uint32_t>(count) )
{
}

inline String::operator uint32_t() const
{
    return static_cast<uint32_t>(std::hash<std::string>{}(mData));
}

inline String& String::operator = (const wchar_t* src)
{
    assign(src, areg::COUNT_ALL);
    return (*this);
}

inline String& String::operator = (const wchar_t src)
{
    (*this) = static_cast<char>(src);
    return (*this);
}

inline String& String::operator = (const String& src)
{
    Base::operator = (static_cast<const Base&>(src));
    return (*this);
}

inline String& String::operator = (const std::string& src)
{
    Base::operator = (src);
    return (*this);
}

inline String& String::operator = (const std::wstring& src)
{
    assign(src.c_str(), static_cast<areg::CharCount>(src.length()));
    return (*this);
}

inline String& String::operator = (std::string_view src)
{
    Base::operator = (src);
    return (*this);
}

inline String& String::operator = (const char* src)
{
    Base::operator = (src);
    return (*this);
}

inline String& String::operator = (String&& src) noexcept
{
    Base::operator = (std::move(src));
    return (*this);
}

inline String& String::operator = (std::string&& src) noexcept
{
    Base::mData = std::move(src);
    return (*this);
}

inline String& String::operator = (const char src)
{
    Base::operator = (src);
    return (*this);
}

inline bool String::operator == (const String& other) const
{
    return Base::operator==(static_cast<const Base&>(other));
}

inline bool String::operator == (const std::string& other) const
{
    return Base::operator==(other);
}

inline bool String::operator == (std::string_view other) const
{
    return Base::operator==(other);
}

inline bool String::operator == (const char* other) const
{
    return Base::operator==(other);
}

inline bool String::operator == (const char ch) const
{
    return Base::operator==(ch);
}

inline bool String::operator != (const String& other) const
{
    return Base::operator!=(static_cast<const Base&>(other));
}

inline bool String::operator != (const std::string& other) const
{
    return Base::operator!=(other);
}

inline bool String::operator != (std::string_view other) const
{
    return Base::operator!=(other);
}

inline bool String::operator != (const char* other) const
{
    return Base::operator!=(other);
}

inline bool String::operator != (const char ch) const
{
    return Base::operator != (ch);
}

inline String& String::operator += (const String& src)
{
    Base::operator+=(static_cast<const Base&>(src));
    return (*this);
}

inline String& String::operator += (const std::string& src)
{
    Base::operator+=(src);
    return (*this);
}

inline String& String::operator += (std::string_view src)
{
    Base::operator+=(src);
    return (*this);
}

inline String& String::operator += (const char* src)
{
    Base::operator+=(src);
    return (*this);
}

inline String& String::operator += (const char chSource)
{
    Base::operator+=(chSource);
    return (*this);
}

inline String& String::operator += (const wchar_t* src)
{
    append(src, areg::COUNT_ALL);
    return (*this);
}

inline String& String::operator += (const wchar_t chSource)
{
    append(&chSource, 1);
    return (*this);
}

inline String& String::operator += (const std::wstring& src)
{
    append(src.c_str(), static_cast<areg::CharCount>(src.length()));
    return (*this);
}

inline String operator + (const String& lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const String& lhs, const std::string& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const String& lhs, std::string_view rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const String& lhs, const char* rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const String& lhs, const char rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

inline String operator + (const std::string& lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (std::string_view lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const char* lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const char lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const String& lhs, const wchar_t* rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline String operator + (const wchar_t* lhs, const String& rhs)
{
    String result(lhs);
    result.append(rhs);
    return result;
}

inline const InStream& operator >> (const InStream& stream, String& input)
{
    input.read_stream(stream);
    return stream;
}

inline OutStream& operator << (OutStream& stream, const String& output)
{
    output.write_stream(stream);
    return stream;
}

inline int32_t String::to_int32( areg::Radix radix /*= areg::Decimal */ ) const
{
    return String::make_int32(as_string(), radix, nullptr );
}

inline uint32_t String::to_uint32( areg::Radix radix /*= areg::Decimal */ ) const
{
    return String::make_uint32(as_string(), radix, nullptr );
}

inline int64_t String::to_int64( areg::Radix radix /*= areg::Decimal */ ) const
{
    return String::make_int64(as_string(), radix, nullptr );
}

inline uint64_t String::to_uint64( areg::Radix radix /*= areg::Decimal */ ) const
{
    return String::make_uint64(as_string(), radix, nullptr );
}

inline float String::to_float() const
{
    return String::make_float(as_string(), nullptr );
}

inline double String::to_double() const
{
    return String::make_double(as_string(), nullptr );
}

inline bool String::to_bool() const
{
    return (is_empty() || areg::compare_ignore_case<char, char>(as_string(), areg::BOOLEAN_FALSE.data()) == areg::Ordering::Equal ? false : true);
}

inline String & String::from_int32( int32_t value, areg::Radix radix /*= areg::Decimal */ )
{
    *this = String::make_string(value, radix);
    return (*this);
}

inline String & String::from_uint32( uint32_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = String::make_string( value, radix );
    return (*this);
}

inline String & String::from_int64( int64_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = String::make_string( value, radix );
    return (*this);
}

inline String & String::from_uint64( uint64_t value, areg::Radix radix /*= areg::Decimal */ )
{
    (*this) = String::make_string( value, radix );
    return (*this);
}

inline String & String::from_float( float value )
{
    (*this) = String::make_string( value );
    return (*this);
}

inline String & String::from_double( double value )
{
    (*this) = String::make_string( value );
    return (*this);
}

inline String & String::from_bool( bool value )
{
    (*this) = String::make_string( value );
    return (*this);
}

inline String& String::assign(const char* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(source, count);
    return (*this);
}

inline String& String::assign(const std::string& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string<char> &>(source), pos, count);
    return (*this);
}

inline String& String::assign(std::string_view source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string_view<char> &>(source), pos, count);
    return (*this);
}

inline String& String::assign(const String& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::assign(source, pos, count);
    return (*this);
}

inline String & String::assign(const char ch)
{
    Base::assign(ch);
    return (*this);
}

inline String& String::append(const char* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(source, count);
    return (*this);
}

inline String& String::append(const std::string& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string<char>&>(source), pos, count);
    return (*this);
}

inline String& String::append(std::string_view source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string_view<char>&>(source), pos, count);
    return (*this);
}

inline String& String::append(const String& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    Base::append(static_cast<const Base&>(source), pos, count);
    return (*this);
}

inline String & String::append(const char ch)
{
    Base::append(ch);
    return (*this);
}

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
            return static_cast<uint32_t>(std::hash<std::string>{}(key.data()));
        }
    };
} // namespace std

#endif  // AREG_BASE_STRING_HPP
