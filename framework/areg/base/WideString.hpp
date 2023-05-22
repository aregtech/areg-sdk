#ifndef AREG_BASE_WIDESTRING_HPP
#define AREG_BASE_WIDESTRING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/WideString.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, WideString Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEString.hpp"

#include <functional>
#include <stdarg.h>
#include <stdio.h>
#include <string_view>

/************************************************************************
 * Dependencies.
 ************************************************************************/
class IEInStream;
class IEOutStream;
class String;

//////////////////////////////////////////////////////////////////////////
// WideString class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   2 bytes wide string class declaration. Has basic functionalities
 *          to handle null-terminated string operations.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
class AREG_API WideString : public TEString<wchar_t>
 {
    friend class BufferStreamBase;

    using Base = TEString<wchar_t>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   WideString::EmptyString
     *          The empty wide string.
     **/
    static const WideString EmptyString;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor.
     **/
    WideString( void ) = default;

    /**
     * \brief   Destructor.
     **/
    ~WideString(void) = default;

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    WideString(const WideString& source) = default;

    /**
     * \brief   Move constructor.
     * \param   source  The source to copy data.
     **/
    WideString(WideString&& source) noexcept = default;

    /**
     * \brief    Initialization constructor. Copies data from source
     * \param    source    The string data source. If nullptr, sets empty string.
     **/
    inline WideString(const wchar_t* source);
    inline WideString(const std::wstring& source);
    inline WideString(const std::wstring_view& source);
    inline WideString(std::wstring&& source) noexcept;
    inline WideString(const String& source);
    inline WideString(const std::string& source);
    inline WideString(const char* source);

    /**
     * \brief    Initialization constructor. Copies carCount chars from source
     * \param    source       The string source
     * \param    charCount    The number of characters to copy.
     **/
    inline WideString(const wchar_t* source, uint32_t charCount);
    inline WideString(const char* source, uint32_t charCount);

    /**
     * \brief    Copies wchar_t as source
     * \param    ch    Char as string.
     **/
    inline explicit WideString( wchar_t ch );

    /**
     * \brief   Initializes string and reserves a space for specified amount of characters.
     *          Note, the size of string remains zero.
     *
     * \param   count   The space to reserve.
     */
    inline explicit WideString(uint32_t count);

    /**
     * \brief   Constructor, initializes string from streaming object
     **/
    explicit WideString( const IEInStream & stream );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converting operator, converts object to unsigned int primitive
     **/
    explicit inline operator unsigned int(void) const;

    /**
     * \brief   Assigning operator, copies data from given null-terminated wide-wchar_t string source
     * \param   src     The source of null-terminated string to copy.
     * \return  Returns the string object.
     **/
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

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
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

    /**
     * \brief   Appends given null-terminated wide-wchar_t string at the end of existing string.
     * \param   src     The source of null-terminated wide-wchar_t string to append at the end
     * \return  Returns string object
     **/
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

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline WideString operator + (const WideString& lhs, const WideString& rhs);
    friend inline WideString operator + (const WideString& lhs, const std::wstring& rhs);
    friend inline WideString operator + (const WideString& lhs, const std::wstring_view& rhs);
    friend inline WideString operator + (const WideString& lhs, const wchar_t* rhs);
    friend inline WideString operator + (const WideString& lhs, const wchar_t rhs);
    friend inline WideString operator + (const std::wstring& lhs, const WideString& rhs);
    friend inline WideString operator + (const std::wstring_view& lhs, const WideString& rhs);
    friend inline WideString operator + (const wchar_t* lhs, const WideString& rhs);
    friend inline WideString operator + (const wchar_t lhs, const WideString& rhs);

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side null-terminated string
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline WideString operator + (const WideString & lhs, const char * rhs);

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side null-terminated string
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline  WideString operator + (const char * lhs, const WideString & rhs);

/************************************************************************/
// Friend global operators to stream WideString
/************************************************************************/
    /**
     * \brief   Streams to input object, i.e. reads data from streaming object to string,
     *          and initialize string data.
     * \param   stream  Streaming object to read string data
     * \param   input    WideString object to initialize and write string data.
     * \return  Reference to stream object.
     **/
    friend inline const IEInStream & operator >> (const IEInStream & stream, WideString & input);

    /**
     * \brief   Streams from output object, i.e. write data from string to streaming object.
     * \param   stream    Streaming object to write data.
     * \param   output    WideString object to read data from
     * \return  Reference to stream object.
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const WideString & output);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

public:
/************************************************************************/
// Static methods
/************************************************************************/
    /**
     * \brief   Searches given phrase in the string buffer until the end of string.
     *          If found, returns string with data until the found position.
     *          On output, if 'out_next' is not nullptr, contains pointer to the next position
     *          after phrase in given string buffer, or nullptr if phrase not found.
     * \param   src     The source of string
     * \param   strPhrase   The phrase to search in the string.
     * \param   out_next    If not nullptr, on return contains value of pointer next after phrase in string buffer,
     *                      or nullptr if could not find the string.
     * \return  Returns string copied until matched phrase.
     *
     * \example getSubstring
     *
     *  WideString test(L"0123 456 789 0123");
     *
     *  const wchar_t * next = static_cast<const wchar_t *>(test);            // next == L"0123 456 789 0123"
     *  WideString result1 = WideString::getSubstring( next, L"0123", &next); // results: result1 == L""           , next == L" 456 789 0123"
     *  WideString result2 = WideString::getSubstring( next, L"0123", &next); // results: result2 == L" 456 789 "  , next == L""
     *  WideString result3 = WideString::getSubstring( next, L"0123", &next); // results: result3 == L""           , next == nullptr;
     *
     *  next = static_cast<const wchar_t *>(test);                            // next == L"0123 456 789 0123"
     *  WideString result4 = WideString::getSubstring( next, L" ", &next);    // results: result4 == L"0123"       , next == L"456 789 0123"
     *  WideString result5 = WideString::getSubstring( next, L" ", &next);    // results: result5 == L"456 789"    , next == L"789 0123"
     *  WideString result6 = WideString::getSubstring( next, L" ", &next);    // results: result6 == L"789"        , next == L"0123";
     *  WideString result7 = WideString::getSubstring( next, L" ", &next);    // results: result7 == L"0123"       , next == nullptr;
     **/
    static WideString getSubstring( const wchar_t * src, const wchar_t * strPhrase, const wchar_t ** out_next = nullptr );

    /**
     * \brief   Converts given string of digits to 32-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit integer
     **/
    static int32_t makeInt32( const wchar_t * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 32-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit unsigned integer
     **/
    static uint32_t makeUInt32( const wchar_t * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit integer
     **/
    static int64_t makeInt64( const wchar_t * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit unsigned integer
     **/
    static uint64_t makeUInt64( const wchar_t * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 32-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit digit with floating point
     **/
    static float makeFloat( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static double makeDouble( const wchar_t * strDigit, const wchar_t ** end = nullptr );
    /**
     * \brief   Converts given string to boolean value
     * \param   strBoolean  The string to convert.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strBoolean buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static bool makeBool( const wchar_t * strBoolean, const wchar_t ** end = nullptr );

    /**
     * \brief   Converts given signed 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString toString( int32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString toString( uint32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given signed 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString toString( int64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString toString( uint64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static WideString toString( float number );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static WideString toString( double number );
    /**
     * \brief   Converts given boolean value to string.
     * \param   value   The boolean value to convert to string
     * \return  Returns converted string.
     **/
    static WideString toString( bool value );

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \return  Returns the number of characters copied in the buffer.
     *          Return zero if nothing is copied.
     *          Returns negative value if error occurred (for example, formating syntax error).
     **/
    static int formatString( wchar_t * strDst, int count, const wchar_t * format, ... );

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the number of characters copied in the buffer.
     *          Return zero if nothing is copied.
     *          Returns negative value if error occurred (for example, formating syntax error).
     **/
    static int formatStringList( wchar_t * strDst, int count, const wchar_t * format, va_list argptr );

/************************************************************************/
// Non-static methods
/************************************************************************/

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const WideString& format(const wchar_t* format, ...);

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const WideString& formatList(const wchar_t* format, va_list argptr);

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     *
     * \param   source  The source of string to copy characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to copy. By default, it copies all characters.
     * \param   ch      A character to assign.
     * \return  Returns modified string.
     **/
    WideString& assign(const char* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& assign(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& assign(const std::wstring& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& assign(const std::wstring_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& assign(const WideString& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& assign(const wchar_t ch);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     *
     * \param   source  The source of string to append characters.
     * \param   pos     If specified the valid zero-based position in the given string to append.
     *                  Otherwise, it append starting from the beginning.
     * \param   count   If specified, the number of characters to append. By default, it appends all characters.
     * \param   ch      A character to append.
     * \return  Returns modified string.
     **/
    WideString& append(const char* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& append(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& append(const std::wstring& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& append(const std::wstring_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& append(const WideString& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline WideString& append(const wchar_t ch);

    /**
     * \brief   Converts string of digits to 32-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit integer
     **/
    inline int32_t toInt32( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit unsigned integer
     **/
    inline uint32_t toUInt32( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit integer
     **/
    inline int64_t toInt64( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit unsigned integer
     **/
    inline uint64_t toUInt64( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit digit with floating point
     * \return  Returns the 32-bit digit with floating point
     **/
    inline float toFloat( void ) const;
    /**
     * \brief   Converts string of digits to 64-bit digit with floating point
     * \return  Returns the 64-bit digit with floating point
     **/
    inline double toDouble( void ) const;
    /**
     * \brief   Converts string to boolean value. If value is "true", it returns true. Otherwise returns false.
     * \return  Returns boolean value.
     **/
    inline bool toBool( void ) const;

    /**
     * \brief   Converts and sets 32-bit signed digit in the string based on radix bases.
     * \param   value   The 32-bit signed integer value to set in the string.
     * \param   radix   The base of conversion. It set RadixAutomatic, it will use
     *                  RadixDecimal. By default, conversion is done in RadixDecimal.
     * \return  Returns string with value.
     * \note    If value is negative, the minus sign is added at begin.
     *          Except decimal (base 10), in other cases it might make less sense.
     *          If need to convert negative number to Hexadecimal or Octal,
     *          might make sense to use FromUInt32 method.
     **/
    inline WideString & fromInt32( int32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );

    /**
     * \brief   Converts and sets 32-bit unsigned digit in the string based on radix bases.
     * \param   value   The 32-bit unsigned integer value to set in the string.
     * \param   radix   The base of conversion. It set RadixAutomatic, it will use
     *                  RadixDecimal. By default, conversion is done in RadixDecimal.
     * \return  Returns string with value.
     * \note    In case of RadixHexadecimal, it adds "0x" at the begin and the total length
     *          of string is 10, where first 2 positions are "0x" and the rest 8 positions
     *          are filled with
     **/
    inline WideString & fromUInt32( uint32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts and sets 64-bit signed digit in the string based on radix bases.
     * \param   value   The 64-bit signed integer value to set in the string.
     * \param   radix   The base of conversion. It set RadixAutomatic, it will use
     *                  RadixDecimal. By default, conversion is done in RadixDecimal.
     * \return  Returns string with value.
     * \note    If value is negative, the minus sign is added at begin.
     *          Except decimal (base 10), in other cases it might make less sense.
     *          If need to convert negative number to Hexadecimal or Octal,
     *          might make sense to use FromUInt32 method.
     **/
    inline WideString & fromInt64( int64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts and sets 64-bit unsigned digit in the string based on radix bases.
     * \param   value   The 64-bit unsigned integer value to set in the string.
     * \param   radix   The base of conversion. It set RadixAutomatic, it will use
     *                  RadixDecimal. By default, conversion is done in RadixDecimal.
     * \return  Returns string with value.
     * \note    In case of RadixHexadecimal, it adds "0x" at the begin and the total length
     *          of string is 10, where first 2 positions are "0x" and the rest 8 positions
     *          are filled with
     **/
    inline WideString & fromUInt64( uint64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts and sets float digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & fromFloat( float value );
    /**
     * \brief   Converts and sets double digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & fromDouble( double value );
    /**
     * \brief   Converts and sets boolean value in the string.
     * \param   value   The boolean value to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & fromBool( bool value );

/************************************************************************/
// WideString protected
/************************************************************************/
protected:
    /**
     * \brief   Reads string data from streaming object.
     * \param   stream  The streaming object, which contains string source data
     **/
    void readStream(const IEInStream & stream);

    /**
     * \brief   Writes string data to streaming object.
     * \param   stream  The streaming object to write string data.
     **/
    void writeStream(IEOutStream & stream) const;
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
        //! An operator to convert String object to unsigned int.
        inline unsigned int operator()(const WideString& key) const
        {
            return static_cast<unsigned int>(std::hash<std::wstring>{}(key.getObject()));
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// WideString class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline WideString::WideString(const wchar_t* source)
    : TEString<wchar_t>(source)
{
}

inline WideString::WideString(const std::wstring& source)
    : TEString<wchar_t>(source)
{
}

inline WideString::WideString(const std::wstring_view& source)
    : TEString<wchar_t>(source)
{
}

inline WideString::WideString(std::wstring&& source) noexcept
    : TEString<wchar_t>( std::move( source ) )
{
}

inline WideString::WideString(const std::string& source)
    : TEString<wchar_t>()
{
    assign(source.c_str(), static_cast<NEString::CharCount>(source.length()));
}

inline WideString::WideString(const char* source)
    : TEString<wchar_t>()
{
    assign(source, NEString::COUNT_ALL);
}

inline WideString::WideString(const wchar_t* source, uint32_t charCount)
    : TEString<wchar_t>(source, static_cast<NEString::CharCount>(charCount))
{
}

inline WideString::WideString(const char* source, uint32_t charCount)
    : TEString<wchar_t>()
{
    assign(source, static_cast<NEString::CharCount>(charCount));
}

inline WideString::WideString( wchar_t ch )
    : TEString<wchar_t>( ch )
{
}

inline WideString::WideString(uint32_t count)
    : TEString<wchar_t>( static_cast<uint32_t>(count) )
{
}

inline WideString::operator unsigned int(void) const
{
    return static_cast<unsigned int>(std::hash<std::wstring>{}(mData));
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

inline const IEInStream& operator >> (const IEInStream& stream, WideString& input)
{
    input.readStream(stream);
    return stream;
}

inline IEOutStream& operator << (IEOutStream& stream, const WideString& output)
{
    output.writeStream(stream);
    return stream;
}

inline int32_t WideString::toInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeInt32(getString(), radix, nullptr );
}

inline uint32_t WideString::toUInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeUInt32(getString(), radix, nullptr );
}

inline int64_t WideString::toInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeInt64(getString(), radix, nullptr );
}

inline uint64_t WideString::toUInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeUInt64(getString(), radix, nullptr );
}

inline float WideString::toFloat( void ) const
{
    return WideString::makeFloat(getString(), nullptr );
}

inline double WideString::toDouble( void ) const
{
    return WideString::makeDouble(getString(), nullptr );
}

inline bool WideString::toBool( void ) const
{
    return (NEString::compareIgnoreCase<wchar_t, char>( getString(), NECommon::BOOLEAN_TRUE.data() ) == NEMath::eCompare::Equal);
}

inline WideString & WideString::fromInt32( int32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    *this = WideString::toString(value, radix);
    return (*this);
}

inline WideString & WideString::fromUInt32( uint32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = WideString::toString( value, radix );
    return (*this);
}

inline WideString & WideString::fromInt64( int64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = WideString::toString( value, radix );
    return (*this);
}

inline WideString & WideString::fromUInt64( uint64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = WideString::toString( value, radix );
    return (*this);
}

inline WideString & WideString::fromFloat( float value )
{
    (*this) = WideString::toString( value );
    return (*this);
}

inline WideString & WideString::fromDouble( double value )
{
    (*this) = WideString::toString( value );
    return (*this);
}

inline WideString & WideString::fromBool( bool value )
{
    (*this) = WideString::toString( value );
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
