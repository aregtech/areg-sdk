#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/WideString.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Wide String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEString.hpp"
#include <stdio.h>
#include <stdarg.h>
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
 * \brief   Wide string class declaration. Has basic functionalities
 *          to handle null-terminated string operations.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
class AREG_API WideString : public TEString<wchar_t, TEStringImpl<wchar_t> >
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   String::EmptyString
     *          The empty string.
     **/
    static constexpr std::wstring_view   EmptyString      = {L""};   //!< Empty String

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    inline WideString( void );
    /**
     * \brief    Initialization constructor. Copies data from source
     * \param    source    The string data source. If nullptr, sets empty string.
     **/
    inline WideString( const wchar_t * source );
    /**
     * \brief    Initialization constructor. Copies carCount wide chars from source
     * \param    source       The string source
     * \param    charCount    The number of characters to copy.
     **/
    inline WideString( const wchar_t * source, int charCount );
    /**
     * \brief    Copies wchar_t as source
     * \param    ch    Char as string.
     **/
    inline WideString( wchar_t ch );
    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    inline WideString( const WideString & source );
    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    inline WideString( WideString && source ) noexcept;
    /**
     * \brief   Initialization constructor. Converts wide char and copies string
     * \param   source  The source of ASCII 8-bit char string
     **/
    inline WideString( const char * source );
    /**
     * \brief   Initialization constructor. Converts ASCII 8-bit char string and
     *          copied first charCount symbols.
     * \param   source      The source of 8-bit ASCII string
     * \param   charCount   The number of character to copy from 
     *                      given string source
     **/
    inline WideString( const char * source, int charCount );
    /**
     * \brief   Initialization constructor. Copies data from 8-bit ASCII string
     * \param   source  The source object of 8-bit ASCII string
     **/
    WideString( const String & source );
    /**
     * \brief   Constructor, initializes string from streaming object
     **/
    WideString( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~WideString( void ) = default;

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converting operator, converts object to unsigned int primitive
     **/
    explicit operator unsigned int ( void ) const;

    /**
     * \brief   Converting operator, converts object to string buffer
     **/
    inline operator const wchar_t * ( void ) const;

    /**
     * \brief   Assigning operator, copies data from given wide-string source
     * \param   src     The source of string to copy.
     * \return  Returns the string object.
     **/
    WideString & operator = ( const WideString & src );

    /**
     * \brief   Move operator, moves data from given wide string source
     * \param   src     The source of wide string to move.
     * \return  Returns the string object.
     **/
    WideString & operator = ( WideString && src ) noexcept;

    /**
     * \brief   Assigning operator, copies data from given null-terminated wide string source
     * \param   src     The source of null-terminated string to copy.
     * \return  Returns the string object.
     **/
    WideString & operator = ( const wchar_t * src );

    /**
     * \brief   Assigning operator, copies data from given null-terminated 8-bit string source
     * \param   src     The source of null-terminated 8-bit ASCII char string to copy.
     * \return  Returns the string object.
     **/
    WideString & operator = ( const char * src );

    /**
     * \brief   Assigning operator, copies data from given character source
     * \param   chSource    The source of character to copy.
     * \return  Returns the string object.
     **/
    WideString & operator = ( wchar_t chSource );

    /**
     * \brief   Assigning operator, copies data from given 8-bit string source
     * \param   src     The source of wide-string to copy.
     * \return  Returns the string object.
     **/
    WideString & operator = ( const String & src );

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const WideString & other) const;
           bool operator == (const String & other) const;

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const wchar_t * other) const;
    inline bool operator == (const char * other) const;

    /**
     * \brief   Determines whether the string is equal to character, i.e. it is one symbol
     *          string and this symbol is equal to specified character.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   ch      A character to compare.
     * \return  Returns true if the length of string is 1 and the single symbol is equal
     *          to specified character.
     * \see     Compare
     **/
    inline bool operator == (wchar_t ch) const;
    inline bool operator == (char ch) const;

    /**
     * \brief   Determines inequality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are unequal.
     * \see     Compare
     **/
    inline bool operator != (const WideString & other) const;

    /**
     * \brief   Determines inequality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if 2 strings are unequal.
     * \see     Compare
     **/
    inline bool operator != (const wchar_t * other) const;

    /**
     * \brief   Determines whether existing string is more than given strings.
     *          The operator is needed for sorting algorithms
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is more than given.
     * \see     Compare
     **/
    inline bool operator > ( const WideString & other ) const;

    /**
     * \brief   Determines whether existing string is less than given strings.
     *          The operator is needed for sorting algorithms
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is less than given.
     * \see     Compare
     **/
    inline bool operator < ( const WideString & other ) const;

    /**
     * \brief   Appends given string at the end of existing string.
     * \param   src     The source of string to append at the end
     * \return  Returns string object
     **/
    WideString & operator += ( const WideString & src );

    /**
     * \brief   Appends given null-terminated wide string at the end of existing string.
     * \param   src     The source of null-terminated string to append at the end
     * \return  Returns string object
     **/
    WideString & operator += ( const wchar_t * src );

    /**
     * \brief   Appends given null-terminated string at the end of existing string.
     * \param   src     The source of null-terminated wide-char string to append at the end
     * \return  Returns string object
     **/
    WideString & operator += ( const char * src );

    /**
     * \brief   Appends given wide string at the end of existing string.
     * \param   src     The source of 8-bit string to append at the end
     * \return  Returns string object
     **/
    WideString & operator += ( const String & src );

    /**
     * \brief   Appends given character at the end of existing string.
     * \param   chSource   The source of character to append at the end
     * \return  Returns string object
     **/
    WideString & operator += ( wchar_t chSource );

    /**
     * \brief   Returns character at given position. The position should be valid.
     * \param   atPos   The valid position in string buffer.
     * \return  Returns character at given position.
     **/
    wchar_t operator [ ] (int atPos) const;

/************************************************************************/
// Friend global operators to operate String
/************************************************************************/
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API WideString operator + (const WideString & lhs, const WideString & rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side null-terminated wide string
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API WideString operator + (const WideString & lhs, const wchar_t * rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side null-terminated wide string
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API WideString operator + (const wchar_t * lhs, const WideString & rhs);
    
    /**
     * \brief   Operator to concatenate string and 16-bit character.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side 16-bit character
     * \return  Returns newly constructed string object by concatenating string and char
     **/
    friend AREG_API WideString operator + (const WideString & lhs, wchar_t chRhs);
    
    /**
     * \brief   Operator to concatenate 16-bit character and string.
     * \param   lhs     Left-hand side 16-bit character
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating char and string
     **/
    friend AREG_API WideString operator + (wchar_t chLhs, const WideString & rhs);

/************************************************************************/
// Friend global operators to stream String
/************************************************************************/
    /**
     * \brief   Streams to input object, i.e. reads data from streaming object to string,
     *          and initialize string data.
     * \param    stream  Streaming object to read string data
     * \param    input    String object to initialize and write string data.
     * \return    Reference to stream object.
     **/
    friend AREG_API const IEInStream & operator >> (const IEInStream & stream, WideString & input);
    
    /**
     * \brief    Streams from output object, i.e. write data from string to streaming object.
     * \param    stream    Streaming object to write data.
     * \param    output    String object to read data from
     * \return    Reference to stream object.
     **/
    friend AREG_API IEOutStream & operator << (IEOutStream & stream, const WideString & output);

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
     *  const wchar_t * next = static_cast<const wchar_t *>(test);              // next == L"0123 456 789 0123"
     *  WideString result1 = WideString::getSubstring( next, L"0123", &next);   // results: result1 == L""          , next == L" 456 789 0123"
     *  WideString result2 = WideString::getSubstring( next, L"0123", &next);   // results: result2 == L" 456 789 " , next == L""
     *  WideString result3 = WideString::getSubstring( next, L"0123", &next);   // results: result3 == L""          , next == nullptr;
     *  
     *  next = static_cast<const wchar_t *>(test);                              // next == L"0123 456 789 0123"
     *  WideString result4 = WideString::getSubstring( next, L" ", &next);      // results: result4 == L"0123"      , next == L"456 789 0123"
     *  WideString result5 = WideString::getSubstring( next, L" ", &next);      // results: result5 == L"456 789"   , next == L"789 0123"
     *  WideString result6 = WideString::getSubstring( next, L" ", &next);      // results: result6 == L"789"       , next == L"0123";
     *  WideString result7 = WideString::getSubstring( next, L" ", &next);      // results: result7 == L"0123"      , next == nullptr;
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
    static WideString int32ToString( int32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString uint32ToString( uint32_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given signed 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString int64ToString( int64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given unsigned 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static WideString uint64ToString( uint64_t number, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static WideString floatToString( float number );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static WideString doubleToString( double number );
    /**
     * \brief   Converts given boolean value to string.
     * \param   value   The boolean value to convert to string
     * \return  Returns converted string.
     **/
    static WideString boolToString( bool value );

    /**
     * \brief   Formats the string. The classic rules similar of 'swprintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \return  Returns the number of characters copied in the buffer.
     *          Return zero if nothing is copied.
     *          Returns negative value if error occurred (for example, formating syntax error).
     **/
    static int formatString( wchar_t * strDst, int count, const wchar_t * format, ... );

    /**
     * \brief   Formats the string. The classic rules similar of 'vswprintf' are applied.
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
     * \brief   Returns substring of existing string. The substring is formed starting at given position in string buffer
     *          and number of characters to get in string or until end of string if 'charCount' is NEString::COUNT_ALL
     * \param   startPos    The starting position in existing string buffer. Should be valid position.
     *                      By default, starting position is begin of string buffer, i.e. NEString::START_POS
     * \param   charCount   The number of characters to copy to substring. If equal to NEString::COUNT_ALL,
     *                      it copies characters until end of string.
     * \return  Returns created substring.
     **/
    inline WideString substring( NEString::CharPos startPos = NEString::START_POS, NEString::CharCount charCount = NEString::COUNT_ALL ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after phrase. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   strPhrase   The phrase to search in the string.
     * \param   startPos    The starting position to search the string.
     * \return  Returns following values:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the phrase is not at the end;
     *              -   NEString::END_POS if found the phrase at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     *
     * \example WideString::substring
     *
     *  WideString test(L"0123 456 789 0123");
     *  WideString result;
     *  NEString::CharPos next = NEString::START_POS;
     *  next = test.substring(result, L"0123", next);   // results: next == 4, result == L""
     *  next = test.substring(result, L"0123", next);   // results: next == NEString::END_POS, result == L" 456 789 "
     *
     *  next = NEString::START_POS;
     *  next = test.substring(result, L" ", next);      // results: next == 5, result == L"0123"
     *  next = test.substring(result, L" ", next);      // results: next == 9, result == L"456"
     *  next = test.substring(result, L" ", next);      // results: next == NEString::INVALID_POS, result == L"0123"
     **/
    inline NEString::CharPos substring( WideString & outResult, const wchar_t * strPhrase, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after symbol. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   chSymbol    The symbol to search in the string.
     * \param   startPos    The starting position to search the symbol.
     * \return  Returns next position after searched symbol and value are followings:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the symbol is not at the end;
     *              -   NEString::END_POS if found the symbol at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     **/
    inline NEString::CharPos substring( WideString & outResult, wchar_t chSymbol, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the left side substring of length 'charCount' or empty string is string is empty.
     **/
    inline WideString leftSide( NEString::CharCount charCount ) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the right side substring of length 'charCount' or empty string is string is empty.
     **/
    inline WideString rightSide( NEString::CharCount charCount ) const;

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const WideString & formatString( const wchar_t * format, ... );

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const WideString & formatList( const wchar_t * format, va_list argptr );

    /**
     * \brief   Converts string of digits to 32-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit integer
     **/
    inline int32_t convToInt32( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit unsigned integer
     **/
    inline uint32_t convToUInt32( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit integer
     **/
    inline int64_t convToInt64( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit unsigned integer
     **/
    inline uint64_t convToUInt64( NEString::eRadix radix = NEString::eRadix::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit digit with floating point
     * \return  Returns the 32-bit digit with floating point
     **/
    inline float convToFloat( void ) const;
    /**
     * \brief   Converts string of digits to 64-bit digit with floating point
     * \return  Returns the 64-bit digit with floating point
     **/
    inline double convToDouble( void ) const;
    /**
     * \brief   Converts string to boolean value. If value is "true", it returns true. Otherwise returns false.
     * \return  Returns boolean value.
     **/
    inline bool convToBool( void ) const;

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
    inline WideString & convFromInt32( int32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );

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
    inline WideString & convFromUInt32( uint32_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
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
    inline WideString & convFromInt64( int64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
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
    inline WideString & convFromUInt64( uint64_t value, NEString::eRadix radix = NEString::eRadix::RadixDecimal );
    /**
     * \brief   Converts and sets float digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & convFromFloat( float value );
    /**
     * \brief   Converts and sets double digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & convFromDouble( double value );
    /**
     * \brief   Converts and sets boolean value in the string.
     * \param   value   The boolean value to set in the string.
     * \return  Returns string with value.
     **/
    inline WideString & convFromBool( bool value );

/************************************************************************/
// String overrides
/************************************************************************/
protected:
    /**
    * \brief   Reads string data from streaming object.
    * \param   stream  The streaming object, which contains string source data
    **/
    virtual void readStream(const IEInStream & stream);

    /**
    * \brief   Writes string data to streaming object.
    * \param   stream  The streaming object to write string data.
    **/
    virtual void writeStream(IEOutStream & stream) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline WideString & self( void );                 //!< Returns reference object of string itself
    inline const WideString & self( void ) const;     //!< Returns reference object of string itself
};

//////////////////////////////////////////////////////////////////////////
// WideString class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline WideString & WideString::self( void )
{
    return (*this);
}

inline const WideString & WideString::self( void ) const
{
    return (*this);
}

inline WideString::WideString( void )
    : TEString<wchar_t>( )
{
}

inline WideString::WideString( const wchar_t * source )
    : TEString<wchar_t>( source )
{
}

inline WideString::WideString( wchar_t ch )
    : TEString<wchar_t>( ch )
{
}

inline WideString::WideString( const WideString & source )
    : TEString<wchar_t>( static_cast<const TEString<wchar_t> &>(source) )
{
}

inline WideString::WideString( WideString && source ) noexcept
    : TEString<wchar_t>( static_cast<TEString<wchar_t> &&>(source) )
{
}

inline WideString::WideString( const wchar_t * source, int charCount )
    : TEString<wchar_t>( source, static_cast<NEString::CharCount>(charCount) )
{
}

inline WideString::WideString( const char* const source )
    : TEString<wchar_t>( NULL_STRING_W, NEString::getStringLength<char>( source ) )
{
    NEString::copyString<wchar_t, char>( getDataString( ), source );
#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
}

inline WideString::WideString( const char * source, int charCount )
    : TEString<wchar_t>( NULL_STRING_W, static_cast<NEString::CharCount>(charCount) )
{
    NEString::copyString<wchar_t, char>( getDataString( ), source, NEString::START_POS, static_cast<NEString::CharCount>(charCount) );
#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
}

inline WideString::operator const wchar_t *(void) const
{
    return getString();
}

inline bool WideString::operator == (const WideString & other) const
{
    int len = getLength();
    if ( len == other.getLength() )
    {
        return (NEString::compareFast<wchar_t>( getString( ), other.getString( ), len ) == 0);
    }
    else
    {
        return false;
    }
}

inline bool WideString::operator == (const wchar_t * other) const
{
    return (NEString::compare<wchar_t>( getString( ), other ) == 0);
}

inline bool WideString::operator == (const char * other) const
{
    return (NEString::compareFast<wchar_t, char>( getString( ), other ) == 0);
}

inline bool WideString::operator == (const wchar_t ch) const
{
    return ((getLength() == static_cast<NEString::CharCount>(1)) && (getAt(0) == ch));
}

inline bool WideString::operator == (const char ch) const
{
    return ((getLength() == static_cast<NEString::CharCount>(1)) && (getAt(0) == ch));
}

inline bool WideString::operator != (const WideString & other) const
{
    int len = getLength();
    if ( len == other.getLength() )
    {
        return (NEString::compareFast<wchar_t>( getString( ), other.getString( ), len ) != 0);
    }
    else
    {
        return true; 
    }
}

inline bool WideString::operator != (const wchar_t * other) const
{
    return (NEString::compare<wchar_t>( getString( ), other ) != 0);
}

inline bool WideString::operator > (const WideString & other) const
{
    return (NEString::compare<wchar_t>( getString( ), other.getString() ) > 0);
}

inline bool WideString::operator < (const WideString & other) const
{
    return (NEString::compare<wchar_t>( getString( ), other.getString() ) < 0);
}

inline NEString::CharPos WideString::substring( WideString & outResult, const wchar_t * strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/ ) const
{
    return TEString<wchar_t>::substring( outResult, strPhrase, startPos );
}

inline NEString::CharPos WideString::substring( WideString & outResult, wchar_t chSymbol, NEString::CharPos startPos /*= NEString::START_POS */ ) const
{
    return TEString<wchar_t>::substring(outResult, chSymbol, startPos);
}

inline WideString WideString::substring( NEString::CharPos startPos /*= NEString::START_POS*/, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ ) const
{
    WideString result;
    TEString<wchar_t>::substring(result, startPos, charCount);
    return result;
}

inline WideString WideString::leftSide( NEString::CharCount charCount ) const
{
    WideString result;
    TEString<wchar_t>::substring( result, NEString::START_POS, charCount );
    return result;
}

inline WideString WideString::rightSide( NEString::CharCount charCount ) const
{
    WideString result;

    NEString::CharCount len = getLength();
    NEString::CharPos pos   = charCount < len ? len - charCount : NEString::START_POS;
    TEString<wchar_t>::substring( result, pos, NEString::COUNT_ALL );
    return result;
}

inline int32_t WideString::convToInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeInt32(getString(), radix, nullptr );
}

inline uint32_t WideString::convToUInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeUInt32(getString(), radix, nullptr );
}

inline int64_t WideString::convToInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeInt64(getString(), radix, nullptr );
}

inline uint64_t WideString::convToUInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return WideString::makeUInt64(getString(), radix, nullptr );
}

inline float WideString::convToFloat( void ) const
{
    return WideString::makeFloat(getString(), nullptr );
}

inline double WideString::convToDouble( void ) const
{
    return WideString::makeDouble(getString(), nullptr );
}

inline bool WideString::convToBool( void ) const
{
    return (NEString::compareIgnoreCase<wchar_t, char>( getString(), NECommon::BOOLEAN_TRUE.data() ) == 0);
}

inline WideString & WideString::convFromInt32( int32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self() = WideString::int32ToString(value, radix);
    return self();
}

inline WideString & WideString::convFromUInt32( uint32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = WideString::uint32ToString( value, radix );
    return self( );
}

inline WideString & WideString::convFromInt64( int64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = WideString::int64ToString( value, radix );
    return self( );
}

inline WideString & WideString::convFromUInt64( uint64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = WideString::uint64ToString( value, radix );
    return self( );
}

inline WideString & WideString::convFromFloat( float value )
{
    self( ) = WideString::floatToString( value );
    return self( );
}

inline WideString & WideString::convFromDouble( double value )
{
    self( ) = WideString::doubleToString( value );
    return self( );
}

inline WideString & WideString::convFromBool( bool value )
{
    self( ) = WideString::boolToString( value );
    return self( );
}
