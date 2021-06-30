#ifndef AREG_BASE_STRING_HPP
#define AREG_BASE_STRING_HPP
/************************************************************************
 * \file        areg/base/String.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEString.hpp"
#include <stdio.h>
#include <stdarg.h>

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
 *          to handle null-terminated string operations. It does not 
 *          have any special encoding.
 *
 *          Use this class to declare member variables. This class also
 *          supports streaming and used in Hash Map since has operator
 *          to covert string value to integer.
 **/
class AREG_API String : public TEString<char, TEStringImpl<char>>
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   String::EmptyString
     *          The empty string.
     **/
    static const char * const   EmptyString     /*= ""*/    ;   //!< Empty String
    /**
     * \brief   String::BOOLEAN_TRUE
     *          Boolean 'true' value as a string.
     **/
    static const char * const   BOOLEAN_TRUE    /*= "true"*/;   //!< Boolean value 'true' as string
    /**
     * \brief   String::BOOLEAN_FALSE
     *          Boolean 'false' value as a string.
     **/
    static const char * const   BOOLEAN_FALSE   /*= "false"*/;  //!< Boolean value 'false' as string
    /**
     * \brief	Empty string object
     **/
    static const String 		InvalidString;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    inline String( void );
    /**
     * \brief    Initialization constructor. Copies data from source
     * \param    source    The string data source. If NULL, sets empty string.
     **/
    inline String( const char * source );
    /**
     * \brief    Initialization constructor. Copies carCount chars from source
     * \param    source       The string source
     * \param    charCount    The number of characters to copy.
     **/
    inline String( const char * source, int charCount );
    /**
     * \brief    Copies char as source
     * \param    ch    Char as string.
     **/
    inline String( char ch );
    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    inline String( const String & source );
    /**
     * \brief   Initialization constructor. Converts wide char and copies string
     * \param   source  The source of wide char string
     **/
    inline String( const wchar_t* source );
    /**
     * \brief   Initialization constructor. Converts wide char string and
     *          copied first charCount symbols.
     * \param   source      The source of wide char string
     * \param   charCount   The number of character to copy from 
     *                      given string source
     **/
    inline String( const wchar_t* source, int charCount );
    /**
    * \brief   Initialization constructor. Copies data from wide char string
    * \param   source  The source object of wide char string
    **/
    String( const WideString & source );
    /**
     * \brief   Constructor, initializes string from streaming object
     **/
    String( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~String( void );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converting operator, converts object to unsigned int primitive
     **/
    operator unsigned int ( void ) const;

    /**
     * \brief   Converting operator, converts object to string buffer
     **/
    inline operator const char * ( void ) const;

    /**
     * \brief   Assigning operator, copies data from given string source
     * \param   strSource   The source of string to copy.
     * \return  Returns the string object.
     **/
    const String & operator = ( const String & strSource );

    /**
     * \brief   Assigning operator, copies data from given null-terminated string source
     * \param   strSource   The source of null-terminated string to copy.
     * \return  Returns the string object.
     **/
    const String & operator = ( const char * strSource );

    /**
     * \brief   Assigning operator, copies data from given null-terminated wide-char string source
     * \param   strSource   The source of null-terminated wide-char string to copy.
     * \return  Returns the string object.
     **/
    const String & operator = ( const wchar_t * strSource );

    /**
     * \brief   Assigning operator, copies data from given character source
     * \param   chSource    The source of character to copy.
     * \return  Returns the string object.
     **/
    const String & operator = ( char chSource );

    /**
     * \brief   Assigning operator, copies data from given wide-string source
     * \param   strSource   The source of wide-string to copy.
     * \return  Returns the string object.
     **/
    const String & operator = ( const WideString & strSource );

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const String & other) const;
           bool operator == (const WideString & other) const;

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const char * other) const;
    inline bool operator == (const wchar_t * other) const;

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
    inline bool operator == (char ch) const;
    inline bool operator == (wchar_t ch) const;

    /**
     * \brief   Determines inequality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are unequal.
     * \see     Compare
     **/
    inline bool operator != (const String & other) const;

    /**
     * \brief   Determines inequality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if 2 strings are unequal.
     * \see     Compare
     **/
    inline bool operator != (const char * other) const;

    /**
     * \brief   Determines whether existing string is more than given strings.
     *          The operator is needed for sorting algorithms
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is more than given.
     * \see     Compare
     **/
    inline bool operator > ( const String & other ) const;

    /**
     * \brief   Determines whether existing string is less than given strings.
     *          The operator is needed for sorting algorithms
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is less than given.
     * \see     Compare
     **/
    inline bool operator < ( const String & other ) const;

    /**
     * \brief   Appends given string at the end of existing string.
     * \param   strSource   The source of string to append at the end
     * \return  Returns string object
     **/
    String & operator += ( const String & strSource );

    /**
     * \brief   Appends given null-terminated string at the end of existing string.
     * \param   strSource   The source of null-terminated string to append at the end
     * \return  Returns string object
     **/
    String & operator += ( const char * strSource );

    /**
     * \brief   Appends given null-terminated wide-char string at the end of existing string.
     * \param   strSource   The source of null-terminated wide-char string to append at the end
     * \return  Returns string object
     **/
    String & operator += ( const wchar_t * strSource );

    /**
     * \brief   Appends given wide string at the end of existing string.
     * \param   strSource   The source of wide-string to append at the end
     * \return  Returns string object
     **/
    String & operator += ( const WideString & strSource );

    /**
     * \brief   Appends given character at the end of existing string.
     * \param   chSource   The source of character to append at the end
     * \return  Returns string object
     **/
    String & operator += ( char chSource );

    /**
     * \brief   Returns character at given position. The position should be valid.
     * \param   atPos   The valid position in string buffer.
     * \return  Returns character at given position.
     **/
    char operator [ ] (int atPos) const;

/************************************************************************/
// Friend global operators to operate String
/************************************************************************/
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API String operator + (const String & lhs, const String & rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side null-terminated string
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API String operator + (const String & lhs, const char * rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side null-terminated string
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API String operator + (const char * lhs, const String & rhs);
    
    /**
     * \brief   Operator to concatenate string and 8-bit character.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side 8-bit character
     * \return  Returns newly constructed string object by concatenating string and char
     **/
    friend AREG_API String operator + (const String & lhs, char chRhs);
    
    /**
     * \brief   Operator to concatenate 8-bit character and string.
     * \param   lhs     Left-hand side 8-bit character
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating char and string
     **/
    friend AREG_API String operator + (char chLhs, const String & rhs);

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
    friend AREG_API const IEInStream & operator >> (const IEInStream & stream, String & input);
    
    /**
     * \brief    Streams from output object, i.e. write data from string to streaming object.
     * \param    stream    Streaming object to write data.
     * \param    output    String object to read data from
     * \return    Reference to stream object.
     **/
    friend AREG_API IEOutStream & operator << (IEOutStream & stream, const String & output);

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
     *          On output, if 'out_next' is not NULL, contains pointer to the next position
     *          after phrase in given string buffer, or NULL if phrase not found.
     * \param   strSource   The source of string
     * \param   strPhrase   The phrase to search in the string.
     * \param   out_next    If not NULL, on return contains value of pointer next after phrase in string buffer,
     *                      or NULL if could not find the string.
     * \return  Returns string copied until matched phrase.
     *
     * \example String::getSubstring
     * 
     *  String test("0123 456 789 0123");
     * 
     *  const char * next = static_cast<const char *>(test);            // next == "0123 456 789 0123"
     *  String result1 = String::getSubstring( next, "0123", &next);    // results: result1 == ""           , next == " 456 789 0123"
     *  String result2 = String::getSubstring( next, "0123", &next);    // results: result2 == " 456 789 "  , next == ""
     *  String result3 = String::getSubstring( next, "0123", &next);    // results: result3 == ""           , next == NULL;
     *  
     *  next = static_cast<const char *>(test);                         // next == "0123 456 789 0123"
     *  String result4 = String::getSubstring( next, " ", &next);       // results: result4 == "0123"       , next == "456 789 0123"
     *  String result5 = String::getSubstring( next, " ", &next);       // results: result5 == "456 789"    , next == "789 0123"
     *  String result6 = String::getSubstring( next, " ", &next);       // results: result6 == "789"        , next == "0123";
     *  String result7 = String::getSubstring( next, " ", &next);       // results: result7 == "0123"       , next == NULL;
     **/
    static String getSubstring( const char * strSource, const char * strPhrase, const char ** out_next = static_cast<const char **>(NULL) );

    /**
     * \brief   Converts given string of digits to 32-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit integer
     **/
    static int32_t makeInt32( const char * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string of digits to 32-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit unsigned integer
     **/
    static uint32_t makeUInt32( const char * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string of digits to 64-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit integer
     **/
    static int64_t makeInt64( const char * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string of digits to 64-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit unsigned integer
     **/
    static uint64_t makeUInt64( const char * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string of digits to 32-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit digit with floating point
     **/
    static float makeFloat( const char * strDigit, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string of digits to 64-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static double makeDouble( const char * strDigit, const char ** end = static_cast<const char **>(NULL) );
    /**
     * \brief   Converts given string to boolean value
     * \param   strBoolean  The string to convert.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strBoolean buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static bool makeBool( const char * strBoolean, const char ** end = static_cast<const char **>(NULL) );

    /**
     * \brief   Converts given signed 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String int32ToString( int32_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given unsigned 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String uint32ToString( uint32_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given signed 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String int64ToString( int64_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given unsigned 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static String uint64ToString( uint64_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static String floatToString( float number );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static String doubleToString( double number );
    /**
     * \brief   Converts given boolean value to string.
     * \param   value   The boolean value to convert to string
     * \return  Returns converted string.
     **/
    static String boolToString( bool value );

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
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
     * \brief   Returns substring of existing string. The substring is formed starting at given position in string buffer
     *          and number of characters to get in string or until end of string if 'charCount' is NEString::CountAll
     * \param   startPos    The starting position in existing string buffer. Should be valid position.
     *                      By default, starting position is begin of string buffer, i.e. NEString::StartPos
     * \param   charCount   The number of characters to copy to substring. If equal to NEString::CountAll,
     *                      it copies characters until end of string.
     * \return  Returns created substring.
     **/
    inline String substring( NEString::CharPos startPos = NEString::StartPos, NEString::CharCount charCount = NEString::CountAll ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after phrase. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   strPhrase   The phrase to search in the string.
     * \param   startPos    The starting position to search the string.
     * \return  Returns following values:
     *              -   Valid string position not equal to NEString::EndPos,
     *                  if found phrase and the phrase is not at the end;
     *              -   NEString::EndPos if found the phrase at end of string;
     *              -   NEString::InvalidPos if could not find the phrase.
     *
     * \example String::substring
     *
     *  String test("0123 456 789 0123");
     *  String result;
     *  NEString::CharPos next = NEString::StartPos;
     *  next = test.substring(result, "0123", next);   // results: next == 4, result == ""
     *  next = test.substring(result, "0123", next);   // results: next == NEString::EndPos, result == " 456 789 "
     *
     *  next = NEString::StartPos;
     *  next = test.substring(result, " ", next);      // results: next == 5, result == "0123"
     *  next = test.substring(result, " ", next);      // results: next == 9, result == "456"
     *  next = test.substring(result, " ", next);      // results: next == NEString::InvalidPos, result == "0123"
     **/
    inline NEString::CharPos substring( String & outResult, const char * strPhrase, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after symbol. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   chSymbol    The symbol to search in the string.
     * \param   startPos    The starting position to search the symbol.
     * \return  Returns next position after searched symbol and value are followings:
     *              -   Valid string position not equal to NEString::EndPos,
     *                  if found phrase and the symbol is not at the end;
     *              -   NEString::EndPos if found the symbol at end of string;
     *              -   NEString::InvalidPos if could not find the phrase.
     **/
    inline NEString::CharPos substring( String & outResult, char chSymbol, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the left side substring of length 'charCount' or empty string is string is empty.
     **/
    inline String leftSide( NEString::CharCount charCount ) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the right side substring of length 'charCount' or empty string is string is empty.
     **/
    inline String rightSide( NEString::CharCount charCount ) const;

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const String & formatString( const char * format, ... );

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const String & formatList( const char * format, va_list argptr );

    /**
     * \brief   Converts string of digits to 32-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit integer
     **/
    inline int32_t convToInt32( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit unsigned integer
     **/
    inline uint32_t convToUInt32( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit integer
     **/
    inline int64_t convToInt64( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit unsigned integer
     **/
    inline uint64_t convToUInt64( NEString::eRadix radix = NEString::RadixDecimal ) const;
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
    inline String & convFromInt32( int32_t value, NEString::eRadix radix = NEString::RadixDecimal );

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
    inline String & convFromUInt32( uint32_t value, NEString::eRadix radix = NEString::RadixDecimal );
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
    inline String & convFromInt64( int64_t value, NEString::eRadix radix = NEString::RadixDecimal );
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
    inline String & convFromUInt64( uint64_t value, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts and sets float digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline String & convFromFloat( float value );
    /**
     * \brief   Converts and sets double digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline String & convFromDouble( double value );
    /**
     * \brief   Converts and sets boolean value in the string.
     * \param   value   The boolean value to set in the string.
     * \return  Returns string with value.
     **/
    inline String & convFromBool( bool value );

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
    inline String & self( void );                 //!< Returns reference object of string itself
    inline const String & self( void ) const;     //!< Returns reference object of string itself
};

//////////////////////////////////////////////////////////////////////////
// String class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline String & String::self( void )
{
    return (*this);
}

inline const String & String::self( void ) const
{
    return (*this);
}

inline String::String( void )
    : TEString<char>( NEString::EncodeAscii )
{   ;   }

inline String::String( const char * source )
    : TEString<char>( source, NEString::EncodeAscii )
{   ;   }

inline String::String( char ch )
    : TEString<char>( ch, NEString::EncodeAscii )
{   ;   }

inline String::String( const String & source )
    : TEString<char>( static_cast<const TEString<char> &>(source) )
{   ;   }

inline String::String( const char * source, int charCount )
    : TEString<char>( source, charCount, NEString::EncodeAscii )
{   ;   }

inline String::String( const wchar_t* source )
    : TEString<char>( NULL_STRING, NEString::getStringLength<wchar_t>( source ), NEString::EncodeAscii )
{
    NEString::copyString<char, wchar_t>( getDataString( ), source );
}

inline String::String( const wchar_t* source, int charCount )
    : TEString<char>( NULL_STRING, charCount, NEString::EncodeAscii )
{
    NEString::copyString<char, wchar_t>( getDataString( ), source, NEString::StartPos, charCount );
}

inline String::operator const char *(void) const
{
    return getString();
}

inline bool String::operator == (const String & other) const
{
    return (NEString::compareFast<char, char>( getString( ), other.getString( ) ) == 0);
}

inline bool String::operator == (const char * other) const
{
    return (NEString::compareFast<char, char>( getString( ), other ) == 0);
}

inline bool String::operator == (const wchar_t * other) const
{
    return (NEString::compareFast<char, wchar_t>( getString( ), other ) == 0);
}

inline bool String::operator == (const char ch) const
{
    return ((getLength() == 1) && (getAt(0) == ch));
}

inline bool String::operator == (const wchar_t ch) const
{
    return ((getLength() == 1) && (static_cast<wchar_t>(getAt(0)) == ch));
}

inline bool String::operator != (const String & other) const
{
    return (NEString::compareFast<char, char>( getString( ), other.getString( ) ) != 0);
}

inline bool String::operator != (const char * other) const
{
    return (NEString::compareFast<char, char>( getString( ), other ) != 0);
}

inline bool String::operator > (const String & other) const
{
    return (NEString::compareFast<char, char>( getString( ), other.getString() ) < 0);
}

inline bool String::operator < (const String & other) const
{
    return (NEString::compareFast<char, char>( getString( ), other.getString() ) > 0);
}

inline NEString::CharPos String::substring( String & outResult, const char * strPhrase, NEString::CharPos startPos /*= NEString::StartPos*/ ) const
{
    return TEString<char>::substring( outResult, strPhrase, startPos );
}

inline NEString::CharPos String::substring( String & outResult, char chSymbol, NEString::CharPos startPos /*= NEString::StartPos */ ) const
{
    return TEString<char>::substring(outResult, chSymbol, startPos);
}

inline String String::substring( NEString::CharPos startPos /*= NEString::StartPos*/, NEString::CharCount charCount /*= NEString::CountAll*/ ) const
{
    String result(getInitEncoding());
    TEString<char>::substring(result, startPos, charCount);
    return result;
}

inline String String::leftSide( NEString::CharCount charCount ) const
{
    String result( getInitEncoding( ) );
    TEString<char>::substring( result, NEString::StartPos, charCount );
    return result;
}

inline String String::rightSide( NEString::CharCount charCount ) const
{
    String result( getInitEncoding( ) );
    NEString::CharPos pos = charCount < getLength() ? getLength() - charCount : NEString::StartPos;
    TEString<char>::substring( result, pos, NEString::CountAll );
    return result;
}

inline int32_t String::convToInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeInt32(getString(), radix, static_cast<const char **>(NULL) );
}

inline uint32_t String::convToUInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeUInt32(getString(), radix, static_cast<const char **>(NULL) );
}

inline int64_t String::convToInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeInt64(getString(), radix, static_cast<const char **>(NULL) );
}

inline uint64_t String::convToUInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{
    return String::makeUInt64(getString(), radix, static_cast<const char **>(NULL) );
}

inline float String::convToFloat( void ) const
{
    return String::makeFloat(getString(), static_cast<const char **>(NULL) );
}

inline double String::convToDouble( void ) const
{
    return String::makeDouble(getString(), static_cast<const char **>(NULL) );
}

inline bool String::convToBool( void ) const
{
    return (NEString::compareIgnoreCase<char, char>( getString(), BOOLEAN_TRUE ) == 0);
}

inline String & String::convFromInt32( int32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self() = String::int32ToString(value, radix);
    return self();
}

inline String & String::convFromUInt32( uint32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = String::uint32ToString( value, radix );
    return self( );
}

inline String & String::convFromInt64( int64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = String::int64ToString( value, radix );
    return self( );
}

inline String & String::convFromUInt64( uint64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = String::uint64ToString( value, radix );
    return self( );
}

inline String & String::convFromFloat( float value )
{
    self( ) = String::floatToString( value );
    return self( );
}

inline String & String::convFromDouble( double value )
{
    self( ) = String::doubleToString( value );
    return self( );
}

inline String & String::convFromBool( bool value )
{
    self( ) = String::boolToString( value );
    return self( );
}

#endif  // AREG_BASE_STRING_HPP
