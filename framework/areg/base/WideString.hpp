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
#include "areg/base/GEGlobal.h"
#include "areg/base/StringBase.hpp"

#include <functional>
#include <stdarg.h>
#include <stdio.h>
#include <string_view>

/************************************************************************
 * Dependencies.
 ************************************************************************/
class InStream;
class OutStream;
class String;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
//////////////////////////////////////////////////////////////////////////
// WideString class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Wide character string (UTF-16) class supporting null-terminated string operations,
 *          streaming, and type conversions.
 * 
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
class AREG_API WideString : public StringBase<wchar_t>
 {
    friend class BufferStreamBase;

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

    /**
     * \brief   Destructor.
     **/
    ~WideString() = default;

    WideString(const WideString& source) = default;

    WideString(WideString&& source) noexcept = default;

    /**
     * \brief   Initializes string from a null-terminated wide character pointer.
     *
     * \param   source      The string data source. If nullptr, initializes an empty string.
     **/
    inline WideString(const wchar_t* source);
    /**
     * \brief   Initializes string from a std::wstring object.
     *
     * \param   source      The wide string to copy.
     **/
    inline WideString(const std::wstring& source);
    /**
     * \brief   Initializes string from a std::wstring_view.
     *
     * \param   source      The wide string view to copy.
     **/
    inline WideString(const std::wstring_view& source);
    /**
     * \brief   Initializes string from an rvalue std::wstring.
     *
     * \param   source      The wide string to move.
     * \note    Move constructor. Takes ownership of the string.
     **/
    inline WideString(std::wstring&& source) noexcept;
    /**
     * \brief   Initializes string from an AREG String object.
     *
     * \param   source      The String object to copy.
     **/
    inline WideString(const String& source);
    /**
     * \brief   Initializes string from a std::string object.
     *
     * \param   source      The string to copy.
     **/
    inline WideString(const std::string& source);
    /**
     * \brief   Initializes string from a null-terminated character pointer.
     *
     * \param   source      The string data source. If nullptr, initializes an empty string.
     **/
    inline WideString(const char* source);

    /**
     * \brief   Initializes string from a wide character pointer with character count.
     *
     * \param   source          The string source.
     * \param   charCount       The number of characters to copy.
     **/
    inline WideString(const wchar_t* source, uint32_t charCount);
    /**
     * \brief   Initializes string from a character pointer with character count.
     *
     * \param   source          The string source.
     * \param   charCount       The number of characters to copy.
     **/
    inline WideString(const char* source, uint32_t charCount);

    /**
     * \brief   Initializes string from a single wide character.
     *
     * \param   ch      The character to initialize the string with.
     **/
    inline explicit WideString( wchar_t ch );

    /**
     * \brief   Initializes string and reserves space for specified number of characters.
     *
     * \param   count       The number of characters to reserve. The size of the string remains
     *                      zero.
     **/
    inline explicit WideString(uint32_t count);

    /**
     * \brief   Initializes string from a streaming object.
     *
     * \param   stream      The input stream to read string data from.
     **/
    explicit WideString( const InStream & stream );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts the string to a uint32_t hash value.
     **/
    explicit inline operator uint32_t() const;

    /**
     * \brief   Assigns from another WideString.
     *
     * \param   src     The source WideString to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const WideString & src);
    /**
     * \brief   Assigns from a std::wstring.
     *
     * \param   src     The std::wstring to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const std::wstring& src);
    /**
     * \brief   Assigns from a std::wstring_view.
     *
     * \param   src     The wide string view to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const std::wstring_view& src);
    /**
     * \brief   Assigns from a null-terminated wide character pointer.
     *
     * \param   src     The null-terminated wide character string to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const wchar_t* src);
    /**
     * \brief   Assigns from a single wide character.
     *
     * \param   src     The wide character to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const wchar_t src);
    /**
     * \brief   Assigns from a std::string.
     *
     * \param   src     The std::string to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const std::string& src);
    /**
     * \brief   Assigns from a null-terminated character pointer.
     *
     * \param   src     The null-terminated character string to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const char * src );
    /**
     * \brief   Assigns from a single character.
     *
     * \param   src     The character to assign.
     * \return  A reference to this string object.
     **/
    inline WideString & operator = (const char src );
    /**
     * \brief   Move-assigns from another WideString.
     *
     * \param   src     The source WideString to move-assign.
     * \return  A reference to this string object.
     * \note    Move assignment operator.
     **/
    inline WideString & operator = (WideString && src) noexcept;
    /**
     * \brief   Move-assigns from an rvalue std::wstring.
     *
     * \param   src     The std::wstring to move-assign.
     * \return  A reference to this string object.
     * \note    Move assignment operator.
     **/
    inline WideString & operator = (std::wstring && src) noexcept;
    /**
     * \brief   Assigns from an AREG String object.
     *
     * \param   src     The String object to assign.
     * \return  A reference to this string object.
     **/
    WideString & operator = ( const String & src );

    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The string to compare.
     * \return  True if equal; false otherwise.
     **/
    inline bool operator == (const WideString& other) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The std::wstring to compare.
     * \return  True if equal; false otherwise.
     **/
    inline bool operator == (const std::wstring& other) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The wide string view to compare.
     * \return  True if equal; false otherwise.
     **/
    inline bool operator == (const std::wstring_view& other) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The wide character string to compare.
     * \return  True if equal; false otherwise.
     **/
    inline bool operator == (const wchar_t* other) const;
    /**
     * \brief   Returns true if the string equals a single character.
     *
     * \param   ch      The wide character to compare.
     * \return  True if equal; false otherwise.
     **/
    inline bool operator == (const wchar_t ch) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The character string to compare.
     * \return  True if equal; false otherwise.
     **/
    bool operator == (const char* other) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The std::string to compare.
     * \return  True if equal; false otherwise.
     **/
    bool operator == (const std::string& other) const;
    /**
     * \brief   Returns true if the strings are equal.
     *
     * \param   other       The AREG String to compare.
     * \return  True if equal; false otherwise.
     **/
    bool operator == (const String& other) const;

    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The string to compare.
     * \return  True if not equal; false otherwise.
     **/
    inline bool operator != (const WideString& other) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The std::wstring to compare.
     * \return  True if not equal; false otherwise.
     **/
    inline bool operator != (const std::wstring& other) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The wide string view to compare.
     * \return  True if not equal; false otherwise.
     **/
    inline bool operator != (const std::wstring_view& other) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The wide character string to compare.
     * \return  True if not equal; false otherwise.
     **/
    inline bool operator != (const wchar_t* other) const;
    /**
     * \brief   Returns true if the string does not equal a single character.
     *
     * \param   ch      The wide character to compare.
     * \return  True if not equal; false otherwise.
     **/
    inline bool operator != (const wchar_t ch) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The character string to compare.
     * \return  True if not equal; false otherwise.
     **/
    bool operator != (const char* other) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The std::string to compare.
     * \return  True if not equal; false otherwise.
     **/
    bool operator != (const std::string& other) const;
    /**
     * \brief   Returns true if the strings are not equal.
     *
     * \param   other       The AREG String to compare.
     * \return  True if not equal; false otherwise.
     **/
    bool operator != (const String& other) const;

    /**
     * \brief   Appends a WideString to this string.
     *
     * \param   src     The WideString to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const WideString& src);
    /**
     * \brief   Appends a std::wstring to this string.
     *
     * \param   src     The std::wstring to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const std::wstring& src);
    /**
     * \brief   Appends a std::wstring_view to this string.
     *
     * \param   src     The wide string view to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const std::wstring_view& src);
    /**
     * \brief   Appends a null-terminated wide character string to this string.
     *
     * \param   src     The wide character string to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const wchar_t* src);
    /**
     * \brief   Appends a single wide character to this string.
     *
     * \param   chSource    The wide character to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const wchar_t chSource);
    /**
     * \brief   Appends a null-terminated character string to this string.
     *
     * \param   src     The character string to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const char* src);
    /**
     * \brief   Appends a single character to this string.
     *
     * \param   src     The character to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const char src);
    /**
     * \brief   Appends a std::string to this string.
     *
     * \param   src     The std::string to append.
     * \return  A reference to this string object.
     **/
    inline WideString& operator += (const std::string& src);
    /**
     * \brief   Appends an AREG String to this string.
     *
     * \param   src     The AREG String to append.
     * \return  A reference to this string object.
     **/
    WideString & operator += ( const String & src );

/************************************************************************/
// Friend global operators to operate WideString
/************************************************************************/

    /**
     * \brief   Returns a new string that is the concatenation of two WideStrings.
     *
     * \param   lhs     The left-hand side string.
     * \param   rhs     The right-hand side string.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString& lhs, const WideString& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a WideString and a std::wstring.
     *
     * \param   lhs     The left-hand side WideString.
     * \param   rhs     The right-hand side std::wstring.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString& lhs, const std::wstring& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a WideString and a
     *          std::wstring_view.
     *
     * \param   lhs     The left-hand side WideString.
     * \param   rhs     The right-hand side wide string view.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString& lhs, const std::wstring_view& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a WideString and a wide character
     *          pointer.
     *
     * \param   lhs     The left-hand side WideString.
     * \param   rhs     The right-hand side null-terminated wide character string.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString& lhs, const wchar_t* rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a WideString and a single wide
     *          character.
     *
     * \param   lhs     The left-hand side WideString.
     * \param   rhs     The right-hand side wide character.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString& lhs, const wchar_t rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a std::wstring and a WideString.
     *
     * \param   lhs     The left-hand side std::wstring.
     * \param   rhs     The right-hand side WideString.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const std::wstring& lhs, const WideString& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a std::wstring_view and a
     *          WideString.
     *
     * \param   lhs     The left-hand side wide string view.
     * \param   rhs     The right-hand side WideString.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const std::wstring_view& lhs, const WideString& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a wide character pointer and a
     *          WideString.
     *
     * \param   lhs     The left-hand side null-terminated wide character string.
     * \param   rhs     The right-hand side WideString.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const wchar_t* lhs, const WideString& rhs);
    /**
     * \brief   Returns a new string that is the concatenation of a single wide character and a
     *          WideString.
     *
     * \param   lhs     The left-hand side wide character.
     * \param   rhs     The right-hand side WideString.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const wchar_t lhs, const WideString& rhs);

    /**
     * \brief   Returns a new string that is the concatenation of a WideString and a character
     *          pointer.
     *
     * \param   lhs     The left-hand side WideString.
     * \param   rhs     The right-hand side null-terminated character string.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline WideString operator + (const WideString & lhs, const char * rhs);

    /**
     * \brief   Returns a new string that is the concatenation of a character pointer and a
     *          WideString.
     *
     * \param   lhs     The left-hand side null-terminated character string.
     * \param   rhs     The right-hand side WideString.
     * \return  A newly constructed string object with concatenated content.
     **/
    friend inline  WideString operator + (const char * lhs, const WideString & rhs);

/************************************************************************/
// Friend global operators to stream WideString
/************************************************************************/
    /**
     * \brief   Reads string data from a streaming object.
     *
     * \param   stream      The input stream to read string data from.
     * \param[out] input       The WideString to initialize with data from the stream.
     * \return  A reference to the stream object.
     **/
    friend inline const InStream & operator >> (const InStream & stream, WideString & input);

    /**
     * \brief   Writes string data to a streaming object.
     *
     * \param[out] stream      The output stream to write string data to.
     * \param   output      The WideString to write to the stream.
     * \return  A reference to the stream object.
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
     * \brief   Searches for a phrase in a string buffer and returns the substring up to the found
     *          position.
     *
     * \param   src             The source string to search in.
     * \param   strPhrase       The phrase to search for.
     * \param[out] out_next        If not nullptr, receives a pointer to the position after the
     *                             found phrase, or nullptr if phrase not found.
     * \return  A string containing data up to the matched phrase.
      *
      * \example substr
      *
      *  WideString test(L"0123 456 789 0123");
      *
      *  const wchar_t * next = static_cast<const wchar_t *>(test);              // next == L"0123 456 789 0123"
      *  WideString result1 = WideString::substr( next, L"0123", &next);         // result1 == L""          , next == L" 456 789 0123"
      *  WideString result2 = WideString::substr( next, L"0123", &next);         // result2 == L" 456 789 " , next == L""
      *  WideString result3 = WideString::substr( next, L"0123", &next);         // result3 == L""          , next == nullptr;
      *
      *  next = static_cast<const wchar_t *>(test);                              // next == L"0123 456 789 0123"
      *  WideString result4 = WideString::substr( next, L" ", &next);            // result4 == L"0123"      , next == L"456 789 0123"
      *  WideString result5 = WideString::substr( next, L" ", &next);            // result5 == L"456"       , next == L"789 0123"
      *  WideString result6 = WideString::substr( next, L" ", &next);            // result6 == L"789"       , next == L"0123";
      *  WideString result7 = WideString::substr( next, L" ", &next);            // result7 == L"0123"      , next == nullptr;
     **/
    static WideString substr( const wchar_t * src, const wchar_t * strPhrase, const wchar_t ** out_next = nullptr );

    /**
     * \brief   Converts a string of digits to a 32-bit signed integer.
     *
     * \param   strDigit    The string containing digits. Can include a leading sign (+ or -).
     * \param   radix       The base for conversion (e.g., Decimal = 10, Hexadecimal = 16).
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 32-bit signed integer.
     **/
    static int32_t make_int32( const wchar_t * strDigit, NEString::Radix radix = NEString::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit unsigned integer.
     *
     * \param   strDigit    The string containing digits.
     * \param   radix       The base for conversion (e.g., Decimal = 10, Hexadecimal = 16).
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 32-bit unsigned integer.
     **/
    static uint32_t make_uint32( const wchar_t * strDigit, NEString::Radix radix = NEString::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit signed integer.
     *
     * \param   strDigit    The string containing digits. Can include a leading sign (+ or -).
     * \param   radix       The base for conversion (e.g., Decimal = 10, Hexadecimal = 16).
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 64-bit signed integer.
     **/
    static int64_t make_int64( const wchar_t * strDigit, NEString::Radix radix = NEString::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit unsigned integer.
     *
     * \param   strDigit    The string containing digits.
     * \param   radix       The base for conversion (e.g., Decimal = 10, Hexadecimal = 16).
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 64-bit unsigned integer.
     **/
    static uint64_t make_uint64( const wchar_t * strDigit, NEString::Radix radix = NEString::Radix::Decimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 32-bit floating-point number.
     *
     * \param   strDigit    The string containing digits.
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 32-bit floating-point number.
     **/
    static float make_float( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string of digits to a 64-bit floating-point number.
     *
     * \param   strDigit    The string containing digits.
     * \param[out] end         If not nullptr, receives a pointer to the first character after the
     *                         converted value.
     * \return  The 64-bit floating-point number.
     **/
    static double make_double( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts a string to a boolean value.
     *
     * \param   strBoolean      The string to convert (e.g., "true" or "false").
     * \param[out] end             If not nullptr, receives a pointer to the first character after
     *                             the converted value.
     * \return  The boolean value.
     **/
    static bool make_bool( const wchar_t * strBoolean, const wchar_t ** end = nullptr );

    /**
     * \brief   Converts a signed 32-bit integer to a string using the specified radix.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion (minimum 2 (binary), maximum 16 (hexadecimal)).
     * \return  The converted string.
     **/
    static WideString make_string( int32_t number, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Converts an unsigned 32-bit integer to a string using the specified radix.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion (minimum 2 (binary), maximum 16 (hexadecimal)).
     * \return  The converted string.
     **/
    static WideString make_string( uint32_t number, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Converts a signed 64-bit integer to a string using the specified radix.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion (minimum 2 (binary), maximum 16 (hexadecimal)).
     * \return  The converted string.
     **/
    static WideString make_string( int64_t number, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Converts an unsigned 64-bit integer to a string using the specified radix.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion (minimum 2 (binary), maximum 16 (hexadecimal)).
     * \return  The converted string.
     **/
    static WideString make_string( uint64_t number, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Converts a 32-bit floating-point number to a string.
     *
     * \param   number      The number to convert.
     * \return  The converted string.
     **/
    static WideString make_string( float number );
    /**
     * \brief   Converts a 64-bit floating-point number to a string.
     *
     * \param   number      The number to convert.
     * \return  The converted string.
     **/
    static WideString make_string( double number );
    /**
     * \brief   Converts a boolean value to a string.
     *
     * \param   value       The boolean value to convert.
     * \return  The converted string.
     **/
    static WideString make_string( bool value );

    /**
     * \brief   Formats a string using printf-style format specifiers.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string, followed by the arguments to format.
     * \return  The number of characters written; zero if nothing was written; negative if an error
     *          occurred.
     **/
    static int32_t format_string( wchar_t * strDst, int32_t count, const wchar_t * format, ... );

    /**
     * \brief   Formats a string using printf-style format specifiers with a va_list.
     *
     * \param[out] strDst      The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer.
     * \param   format      The format string.
     * \param   argptr      The variable argument list with values to format.
     * \return  The number of characters written; zero if nothing was written; negative if an error
     *          occurred.
     **/
    static int32_t format_string_list( wchar_t * strDst, int32_t count, const wchar_t * format, va_list argptr );

/************************************************************************/
// Non-static methods
/************************************************************************/

    /**
     * \brief   Formats this string using printf-style format specifiers.
     *
     * \param   format      The format string, followed by the arguments to format.
     * \return  A reference to this string object.
     * \note    Allocates 128 characters by default; retries with 512 characters on overflow.
     **/
    WideString& format(const wchar_t* format, ...);

    /**
     * \brief   Formats this string using printf-style format specifiers with a va_list.
     *
     * \param   format      The format string.
     * \param   argptr      The variable argument list with values to format.
     * \return  A reference to this string object.
     * \note    Allocates 128 characters by default; retries with 512 characters on overflow.
     **/
    WideString& format_list(const wchar_t* format, va_list argptr);

    /**
     * \brief   Assigns characters from a character string.
     *
     * \param   source      The source string to copy from.
     * \param   count       The number of characters to copy; defaults to all characters.
     * \return  A reference to this string object.
     **/
    WideString& assign(const char* source, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Assigns characters from a wide character string.
     *
     * \param   source      The source wide character string to copy from.
     * \param   count       The number of characters to copy; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& assign(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Assigns characters from a std::wstring starting at a position.
     *
     * \param   source      The source wide string to copy from.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& assign(const std::wstring& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Assigns characters from a std::wstring_view starting at a position.
     *
     * \param   source      The source wide string view to copy from.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& assign(const std::wstring_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Assigns characters from another WideString starting at a position.
     *
     * \param   source      The source WideString to copy from.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to copy; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& assign(const WideString& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Assigns a single wide character.
     *
     * \param   ch      The wide character to assign.
     * \return  A reference to this string object.
     **/
    inline WideString& assign(const wchar_t ch);

    /**
     * \brief   Appends characters from a character string.
     *
     * \param   source      The source string to append.
     * \param   count       The number of characters to append; defaults to all characters.
     * \return  A reference to this string object.
     **/
    WideString& append(const char* source, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends characters from a wide character string.
     *
     * \param   source      The source wide character string to append.
     * \param   count       The number of characters to append; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& append(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends characters from a std::wstring starting at a position.
     *
     * \param   source      The source wide string to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& append(const std::wstring& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends characters from a std::wstring_view starting at a position.
     *
     * \param   source      The source wide string view to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& append(const std::wstring_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends characters from another WideString starting at a position.
     *
     * \param   source      The source WideString to append.
     * \param   pos         The starting position in the source; defaults to the beginning.
     * \param   count       The number of characters to append; defaults to all characters.
     * \return  A reference to this string object.
     **/
    inline WideString& append(const WideString& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends a single wide character.
     *
     * \param   ch      The wide character to append.
     * \return  A reference to this string object.
     **/
    inline WideString& append(const wchar_t ch);

    /**
     * \brief   Converts the string to a 32-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  The 32-bit signed integer.
     **/
    inline int32_t to_int32( NEString::Radix radix = NEString::Radix::Decimal ) const;
    /**
     * \brief   Converts the string to a 32-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  The 32-bit unsigned integer.
     **/
    inline uint32_t to_uint32( NEString::Radix radix = NEString::Radix::Decimal ) const;
    /**
     * \brief   Converts the string to a 64-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  The 64-bit signed integer.
     **/
    inline int64_t to_int64( NEString::Radix radix = NEString::Radix::Decimal ) const;
    /**
     * \brief   Converts the string to a 64-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  The 64-bit unsigned integer.
     **/
    inline uint64_t to_uint64( NEString::Radix radix = NEString::Radix::Decimal ) const;
    /**
     * \brief   Converts the string to a 32-bit floating-point number.
     *
     * \return  The 32-bit floating-point number.
     **/
    inline float to_float() const;
    /**
     * \brief   Converts the string to a 64-bit floating-point number.
     *
     * \return  The 64-bit floating-point number.
     **/
    inline double to_double() const;
    /**
     * \brief   Converts the string to a boolean value ("true" -> true, otherwise false).
     *
     * \return  The boolean value.
     **/
    inline bool to_bool() const;

    /**
     * \brief   Sets the string to the representation of a 32-bit signed integer using the specified
     *          radix.
     *
     * \param   value       The 32-bit signed integer to convert.
     * \param   radix       The base for conversion; defaults to decimal. Negative numbers are
     *                      prefixed with a minus sign.
     * \return  A reference to this string object.
     **/
    inline WideString & from_int32( int32_t value, NEString::Radix radix = NEString::Radix::Decimal );

    /**
     * \brief   Sets the string to the representation of a 32-bit unsigned integer using the
     *          specified radix.
     *
     * \param   value       The 32-bit unsigned integer to convert.
     * \param   radix       The base for conversion; defaults to decimal. Hexadecimal values are
     *                      prefixed with "0x".
     * \return  A reference to this string object.
     **/
    inline WideString & from_uint32( uint32_t value, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Sets the string to the representation of a 64-bit signed integer using the specified
     *          radix.
     *
     * \param   value       The 64-bit signed integer to convert.
     * \param   radix       The base for conversion; defaults to decimal. Negative numbers are
     *                      prefixed with a minus sign.
     * \return  A reference to this string object.
     **/
    inline WideString & from_int64( int64_t value, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Sets the string to the representation of a 64-bit unsigned integer using the
     *          specified radix.
     *
     * \param   value       The 64-bit unsigned integer to convert.
     * \param   radix       The base for conversion; defaults to decimal. Hexadecimal values are
     *                      prefixed with "0x".
     * \return  A reference to this string object.
     **/
    inline WideString & from_uint64( uint64_t value, NEString::Radix radix = NEString::Radix::Decimal );
    /**
     * \brief   Sets the string to the representation of a 32-bit floating-point number.
     *
     * \param   value       The 32-bit floating-point number to convert.
     * \return  A reference to this string object.
     **/
    inline WideString & from_float( float value );
    /**
     * \brief   Sets the string to the representation of a 64-bit floating-point number.
     *
     * \param   value       The 64-bit floating-point number to convert.
     * \return  A reference to this string object.
     **/
    inline WideString & from_double( double value );
    /**
     * \brief   Sets the string to the representation of a boolean value ("true" or "false").
     *
     * \param   value       The boolean value to convert.
     * \return  A reference to this string object.
     **/
    inline WideString & from_bool( bool value );

/************************************************************************/
// WideString protected
/************************************************************************/
protected:
    /**
     * \brief   Reads string data from an input stream.
     *
     * \param   stream      The input stream to read from.
     **/
    void read_stream(const InStream & stream);

    /**
     * \brief   Writes string data to an output stream.
     *
     * \param[out] stream      The output stream to write to.
     **/
    void write_stream(OutStream & stream) const;
};
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hasher of WideString class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the WideString.
 */
namespace std
{
    template<>
    struct hash<WideString>
    {
        //! An operator to convert String object to uint32_t.
        inline uint32_t operator()(const WideString& key) const
        {
            return static_cast<uint32_t>(std::hash<std::wstring>{}(key.data()));
        }
    };
}

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
    assign(source.c_str(), static_cast<NEString::CharCount>(source.length()));
}

inline WideString::WideString(const char* source)
    : StringBase<wchar_t>()
{
    assign(source, NEString::COUNT_ALL);
}

inline WideString::WideString(const wchar_t* source, uint32_t charCount)
    : StringBase<wchar_t>(source, static_cast<NEString::CharCount>(charCount))
{
}

inline WideString::WideString(const char* source, uint32_t charCount)
    : StringBase<wchar_t>()
{
    assign(source, static_cast<NEString::CharCount>(charCount));
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
    return static_cast<uint32_t>(std::hash<std::wstring>{}(mData));
}

inline WideString& WideString::operator = (const char* src)
{
    assign(src, NEString::COUNT_ALL);
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
    assign(src.c_str(), static_cast<NEString::CharCount>(src.length()));
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
    append(src, NEString::COUNT_ALL);
    return (*this);
}

WideString& WideString::operator += (const char chSource)
{
    append(&chSource, 1);
    return (*this);
}

inline WideString& WideString::operator += (const std::string& src)
{
    append(src.c_str(), static_cast<NEString::CharCount>(src.length()));
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

inline int32_t WideString::to_int32( NEString::Radix radix /*= NEString::Decimal */ ) const
{
    return WideString::make_int32(as_string(), radix, nullptr );
}

inline uint32_t WideString::to_uint32( NEString::Radix radix /*= NEString::Decimal */ ) const
{
    return WideString::make_uint32(as_string(), radix, nullptr );
}

inline int64_t WideString::to_int64( NEString::Radix radix /*= NEString::Decimal */ ) const
{
    return WideString::make_int64(as_string(), radix, nullptr );
}

inline uint64_t WideString::to_uint64( NEString::Radix radix /*= NEString::Decimal */ ) const
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
    return (is_empty() || NEString::compare_ignore_case<wchar_t, char>(as_string(), NECommon::BOOLEAN_FALSE.data()) == NEMath::Ordering::Equal ? false : true);
}

inline WideString & WideString::from_int32( int32_t value, NEString::Radix radix /*= NEString::Decimal */ )
{
    *this = WideString::make_string(value, radix);
    return (*this);
}

inline WideString & WideString::from_uint32( uint32_t value, NEString::Radix radix /*= NEString::Decimal */ )
{
    (*this) = WideString::make_string( value, radix );
    return (*this);
}

inline WideString & WideString::from_int64( int64_t value, NEString::Radix radix /*= NEString::Decimal */ )
{
    (*this) = WideString::make_string( value, radix );
    return (*this);
}

inline WideString & WideString::from_uint64( uint64_t value, NEString::Radix radix /*= NEString::Decimal */ )
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

inline WideString& WideString::assign(const wchar_t* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(source, count);
    return (*this);
}

inline WideString& WideString::assign(const std::wstring& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string<wchar_t> &>(source), pos, count);
    return (*this);
}

inline WideString& WideString::assign(const std::wstring_view& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string_view<wchar_t> &>(source), pos, count);
    return (*this);
}

inline WideString& WideString::assign(const WideString& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(source, pos, count);
    return (*this);
}

inline WideString & WideString::assign(const wchar_t ch)
{
    Base::assign(ch);
    return (*this);
}

inline WideString& WideString::append(const wchar_t* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(source, count);
    return (*this);
}

inline WideString& WideString::append(const std::wstring& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string<wchar_t>&>(source), pos, count);
    return (*this);
}

inline WideString& WideString::append(const std::wstring_view& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string_view<wchar_t>&>(source), pos, count);
    return (*this);
}

inline WideString& WideString::append(const WideString& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const Base&>(source), pos, count);
    return (*this);
}

inline WideString & WideString::append(const wchar_t ch)
{
    Base::append(ch);
    return (*this);
}

#endif  // AREG_BASE_WIDESTRING_HPP
