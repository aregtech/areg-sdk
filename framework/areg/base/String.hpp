#ifndef AREG_BASE_STRING_HPP
#define AREG_BASE_STRING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/String.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, String Class to handle basic
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
class WideString;

//////////////////////////////////////////////////////////////////////////
// String class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   ASCII string class declaration. Has basic functionalities
 *          to handle null-terminated string operations.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
class AREG_API String : public TEString<char>
 {
    friend class BufferStreamBase;

    using Base = TEString<char>;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   String::getEmptyString()
     *          The empty string.
     **/
    static const String & getEmptyString( void );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor.
     **/
    String( void ) = default;

    /**
     * \brief   Destructor.
     **/
    ~String(void) = default;

    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    String(const String& source) = default;

    /**
     * \brief   Move constructor.
     * \param   source  The source to copy data.
     **/
    String(String&& source) noexcept = default;

    /**
     * \brief    Initialization constructor. Copies data from source
     * \param    source    The string data source. If nullptr, sets empty string.
     **/
    inline String(const char* source);
    inline String(const std::string& source);
    inline String(const std::string_view& source);
    inline String(std::string&& source) noexcept;
    inline String(const WideString& source);
    inline String(const std::wstring& source);
    inline String(const wchar_t* source);

    /**
     * \brief    Initialization constructor. Copies carCount chars from source
     * \param    source       The string source
     * \param    charCount    The number of characters to copy.
     **/
    inline String(const char* source, uint32_t charCount);
    inline String(const wchar_t* source, uint32_t charCount);

    /**
     * \brief    Copies char as source
     * \param    ch    Char as string.
     **/
    inline explicit String( char ch );

    /**
     * \brief   Initializes string and reserves a space for specified amount of characters.
     *          Note, the size of string remains zero.
     *
     * \param   count   The space to reserve.
     */
    inline explicit String(uint32_t count);

    /**
     * \brief   Constructor, initializes string from streaming object
     **/
    explicit String( const IEInStream & stream );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converting operator, converts object to unsigned int primitive
     **/
    explicit inline operator unsigned int(void) const;

    /**
     * \brief   Assigning operator, copies data from given null-terminated wide-char string source
     * \param   src     The source of null-terminated string to copy.
     * \return  Returns the string object.
     **/
    inline String & operator = (const String & src);
    inline String & operator = (const std::string& src);
    inline String & operator = (const std::string_view& src);
    inline String & operator = (const char* src);
    inline String & operator = (const char src);
    inline String & operator = (const std::wstring& src);
    inline String & operator = (const wchar_t * src );
    inline String & operator = (const wchar_t src );
    inline String & operator = (String && src) noexcept;
    String & operator = ( const WideString & src );

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const String& other) const;
    inline bool operator == (const std::string& other) const;
    inline bool operator == (const std::string_view& other) const;
    inline bool operator == (const char* other) const;
    inline bool operator == (const char ch) const;
    bool operator == (const wchar_t* other) const;
    bool operator == (const std::wstring& other) const;
    bool operator == (const WideString& other) const;

    inline bool operator != (const String& other) const;
    inline bool operator != (const std::string& other) const;
    inline bool operator != (const std::string_view& other) const;
    inline bool operator != (const char* other) const;
    inline bool operator != ( const char ch ) const;
    bool operator != (const wchar_t* other) const;
    bool operator != (const std::wstring& other) const;
    bool operator != (const WideString& other) const;

    /**
     * \brief   Appends given null-terminated wide-char string at the end of existing string.
     * \param   src     The source of null-terminated wide-char string to append at the end
     * \return  Returns string object
     **/
    inline String& operator += (const String& src);
    inline String& operator += (const std::string& src);
    inline String& operator += (const std::string_view& src);
    inline String& operator += (const char* src);
    inline String& operator += (const char chSource);
    inline String& operator += (const wchar_t* src);
    inline String& operator += (const wchar_t src);
    inline String& operator += (const std::wstring& src);
    String & operator += ( const WideString & src );

/************************************************************************/
// Friend global operators to operate String
/************************************************************************/

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline String operator + (const String& lhs, const String& rhs);
    friend inline String operator + (const String& lhs, const std::string& rhs);
    friend inline String operator + (const String& lhs, const std::string_view& rhs);
    friend inline String operator + (const String& lhs, const char* rhs);
    friend inline String operator + (const String& lhs, const char rhs);
    friend inline String operator + (const std::string& lhs, const String& rhs);
    friend inline String operator + (const std::string_view& lhs, const String& rhs);
    friend inline String operator + (const char* lhs, const String& rhs);
    friend inline String operator + (const char lhs, const String& rhs);

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side null-terminated string
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline String operator + (const String & lhs, const wchar_t * rhs);

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side null-terminated string
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend inline  String operator + (const wchar_t * lhs, const String & rhs);

/************************************************************************/
// Friend global operators to stream String
/************************************************************************/
    /**
     * \brief   Streams to input object, i.e. reads data from streaming object to string,
     *          and initialize string data.
     * \param   stream  Streaming object to read string data
     * \param   input    String object to initialize and write string data.
     * \return  Reference to stream object.
     **/
    friend inline const IEInStream & operator >> (const IEInStream & stream, String & input);

    /**
     * \brief   Streams from output object, i.e. write data from string to streaming object.
     * \param   stream    Streaming object to write data.
     * \param   output    String object to read data from
     * \return  Reference to stream object.
     **/
    friend inline IEOutStream & operator << (IEOutStream & stream, const String & output);

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
     *  String test("0123 456 789 0123");
     *
     *  const char * next = static_cast<const char *>(test);            // next == "0123 456 789 0123"
     *  String result1 = String::getSubstring( next, "0123", &next);    // results: result1 == ""           , next == " 456 789 0123"
     *  String result2 = String::getSubstring( next, "0123", &next);    // results: result2 == " 456 789 "  , next == ""
     *  String result3 = String::getSubstring( next, "0123", &next);    // results: result3 == ""           , next == nullptr;
     *
     *  next = static_cast<const char *>(test);                         // next == "0123 456 789 0123"
     *  String result4 = String::getSubstring( next, " ", &next);       // results: result4 == "0123"       , next == "456 789 0123"
     *  String result5 = String::getSubstring( next, " ", &next);       // results: result5 == "456 789"    , next == "789 0123"
     *  String result6 = String::getSubstring( next, " ", &next);       // results: result6 == "789"        , next == "0123";
     *  String result7 = String::getSubstring( next, " ", &next);       // results: result7 == "0123"       , next == nullptr;
     **/
    static String getSubstring( const char * src, const char * strPhrase, const char ** out_next = nullptr );

    /**
     * \brief   Converts given string of digits to 32-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit integer
     **/
    static int32_t makeInt32( const char * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const char ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 32-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit unsigned integer
     **/
    static uint32_t makeUInt32( const char * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const char ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit integer
     **/
    static int64_t makeInt64( const char * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const char ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit unsigned integer
     **/
    static uint64_t makeUInt64( const char * strDigit, NEString::eRadix radix = NEString::eRadix::RadixDecimal, const char ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 32-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit digit with floating point
     **/
    static float makeFloat( const char * strDigit, const char ** end = nullptr );
    /**
     * \brief   Converts given string of digits to 64-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static double makeDouble( const char * strDigit, const char ** end = nullptr );
    /**
     * \brief   Converts given string to boolean value
     * \param   strBoolean  The string to convert.
     * \param   end [out]   If not nullptr, on output this contains value of pointer to the next character in strBoolean buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static bool makeBool( const char * strBoolean, const char ** end = nullptr );

    /**
     * \brief   Converts given signed 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String toString( int32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String toString( uint32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given signed 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String toString( int64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String toString( uint64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static String toString( float number );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static String toString( double number );
    /**
     * \brief   Converts given boolean value to string.
     * \param   value   The boolean value to convert to string
     * \return  Returns converted string.
     **/
    static String toString( bool value );

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \return  Returns the number of characters copied in the buffer.
     *          Return zero if nothing is copied.
     *          Returns negative value if error occurred (for example, formating syntax error).
     **/
    static int formatString( char * strDst, int count, const char * format, ... );

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
    static int formatStringList( char * strDst, int count, const char * format, va_list argptr );

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
    const String& format(const char* format, ...);

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const String& formatList(const char* format, va_list argptr);

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
    String& assign(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& assign(const char* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& assign(const std::string& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& assign(const std::string_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& assign(const String& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& assign(const char ch);

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
    String& append(const wchar_t* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& append(const char* source, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& append(const std::string& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& append(const std::string_view& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& append(const String& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline String& append(const char ch);

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
    inline String & fromInt32( int32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );

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
    inline String & fromUInt32( uint32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
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
    inline String & fromInt64( int64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
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
    inline String & fromUInt64( uint64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts and sets float digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline String & fromFloat( float value );
    /**
     * \brief   Converts and sets double digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline String & fromDouble( double value );
    /**
     * \brief   Converts and sets boolean value in the string.
     * \param   value   The boolean value to set in the string.
     * \return  Returns string with value.
     **/
    inline String & fromBool( bool value );

/************************************************************************/
// String protected
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
// Hasher of String class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A template to calculate hash value of the String.
 */
namespace std
{
    template<>
    struct hash<String>
    {
        //! An operator to convert String object to unsigned int.
        inline unsigned int operator()(const String& key) const
        {
            return static_cast<unsigned int>(std::hash<std::string>{}(key.getObject()));
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// String class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline String::String(const char* source)
    : TEString<char>(source)
{
}

inline String::String(const std::string& source)
    : TEString<char>(source)
{
}

inline String::String(const std::string_view& source)
    : TEString<char>(source)
{
}

inline String::String(std::string&& source) noexcept
    : TEString<char>(std::move(source))
{
}

inline String::String(const std::wstring& source)
    : TEString<char>()
{
    assign(source.c_str(), static_cast<NEString::CharCount>(source.length()));
}

inline String::String(const wchar_t* source)
    : TEString<char>()
{
    assign(source, NEString::COUNT_ALL);
}

inline String::String(const char* source, uint32_t charCount)
    : TEString<char>(source, static_cast<NEString::CharCount>(charCount))
{
}

inline String::String(const wchar_t* source, uint32_t charCount)
    : TEString<char>()
{
    assign(source, static_cast<NEString::CharCount>(charCount));
}

inline String::String( char ch )
    : TEString<char>( ch )
{
}

inline String::String(uint32_t count)
    : TEString<char>( static_cast<uint32_t>(count) )
{
}

inline String::operator unsigned int(void) const
{
    return static_cast<unsigned int>(std::hash<std::string>{}(mData));
}

inline String& String::operator = (const wchar_t* src)
{
    assign(src, NEString::COUNT_ALL);
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
    assign(src.c_str(), static_cast<NEString::CharCount>(src.length()));
    return (*this);
}

inline String& String::operator = (const std::string_view& src)
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

inline bool String::operator == (const std::string_view& other) const
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

inline bool String::operator != (const std::string_view& other) const
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

inline String& String::operator += (const std::string_view& src)
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
    append(src, NEString::COUNT_ALL);
    return (*this);
}

String& String::operator += (const wchar_t chSource)
{
    append(&chSource, 1);
    return (*this);
}

inline String& String::operator += (const std::wstring& src)
{
    append(src.c_str(), static_cast<NEString::CharCount>(src.length()));
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

inline String operator + (const String& lhs, const std::string_view& rhs)
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

inline String operator + (const std::string_view& lhs, const String& rhs)
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

inline const IEInStream& operator >> (const IEInStream& stream, String& input)
{
    input.readStream(stream);
    return stream;
}

inline IEOutStream& operator << (IEOutStream& stream, const String& output)
{
    output.writeStream(stream);
    return stream;
}

inline int32_t String::toInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeInt32(getString(), radix, nullptr );
}

inline uint32_t String::toUInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeUInt32(getString(), radix, nullptr );
}

inline int64_t String::toInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeInt64(getString(), radix, nullptr );
}

inline uint64_t String::toUInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeUInt64(getString(), radix, nullptr );
}

inline float String::toFloat( void ) const
{
    return String::makeFloat(getString(), nullptr );
}

inline double String::toDouble( void ) const
{
    return String::makeDouble(getString(), nullptr );
}

inline bool String::toBool( void ) const
{
    return (NEString::compareIgnoreCase<char, char>( getString(), NECommon::BOOLEAN_TRUE.data() ) == NEMath::eCompare::Equal);
}

inline String & String::fromInt32( int32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    *this = String::toString(value, radix);
    return (*this);
}

inline String & String::fromUInt32( uint32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = String::toString( value, radix );
    return (*this);
}

inline String & String::fromInt64( int64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = String::toString( value, radix );
    return (*this);
}

inline String & String::fromUInt64( uint64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    (*this) = String::toString( value, radix );
    return (*this);
}

inline String & String::fromFloat( float value )
{
    (*this) = String::toString( value );
    return (*this);
}

inline String & String::fromDouble( double value )
{
    (*this) = String::toString( value );
    return (*this);
}

inline String & String::fromBool( bool value )
{
    (*this) = String::toString( value );
    return (*this);
}

inline String& String::assign(const char* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(source, count);
    return (*this);
}

inline String& String::assign(const std::string& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string<char> &>(source), pos, count);
    return (*this);
}

inline String& String::assign(const std::string_view& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(static_cast<const std::basic_string_view<char> &>(source), pos, count);
    return (*this);
}

inline String& String::assign(const String& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::assign(source, pos, count);
    return (*this);
}

inline String & String::assign(const char ch)
{
    Base::assign(ch);
    return (*this);
}

inline String& String::append(const char* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(source, count);
    return (*this);
}

inline String& String::append(const std::string& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string<char>&>(source), pos, count);
    return (*this);
}

inline String& String::append(const std::string_view& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const std::basic_string_view<char>&>(source), pos, count);
    return (*this);
}

inline String& String::append(const String& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    Base::append(static_cast<const Base&>(source), pos, count);
    return (*this);
}

inline String & String::append(const char ch)
{
    Base::append(ch);
    return (*this);
}

#endif  // AREG_BASE_STRING_HPP
