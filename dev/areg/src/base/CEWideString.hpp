#ifndef AREG_BASE_CEWIDESTRING_HPP
#define AREG_BASE_CEWIDESTRING_HPP
/************************************************************************
 * \file        areg/src/base/CEWideString.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Wide String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEString.hpp"
#include <stdio.h>
#include <stdarg.h>

/************************************************************************
 * Dependencies.
 ************************************************************************/
class IEInStream;
class IEOutStream;
class CEString;

/**
 * \brief       ASCII string class declaration. Has basic functionalities
 *              to handle null-terminated string operations. It does not 
 *              have any special encoding.
 *
 * \details     Use this class to declare member variables if it will 
 *              be used to support streaming. This class also used
 *              in hash map since has operator to covert string value
 *              into hash integer value.
 **/

//////////////////////////////////////////////////////////////////////////
// CEWideString class declaration.
//////////////////////////////////////////////////////////////////////////
class AREG_API CEWideString : public TEString<wchar_t>
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    static const wchar_t * const   EmptyString     /*= L""*/    ; //!< Empty String
    static const wchar_t * const   BOOLEAN_TRUE    /*= "true"*/ ; //!< Boolean value 'true' as string
    static const wchar_t * const   BOOLEAN_FALSE   /*= "false"*/; //!< Boolean value 'false' as string

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor.
     **/
    inline CEWideString( void );
    /**
     * \brief	Initialization constructor. Copies data from source
     * \param	source	The string data source. If NULL, sets empty string.
     **/
    inline CEWideString( const wchar_t * source );
    /**
     * \brief    Initialization constructor. Copies carCount wide chars from source
     * \param    source       The string source
     * \param    charCount    The number of characters to copy.
     **/
    inline CEWideString( const wchar_t * source, int charCount );
    /**
     * \brief    Initialization constructor. Copies wchar_t as source
     * \param    ch    Char as string.
     **/
    inline CEWideString( wchar_t ch );
    /**
     * \brief   Copy constructor.
     * \param   source  The source to copy data.
     **/
    inline CEWideString( const CEWideString & source );
    /**
     * \brief   Initialization constructor. Converts wide wchar_t and copies string
     * \param   source  The source of 8-bit ASCII string
     **/
    inline CEWideString( const char * source );
    /**
     * \brief   Initialization constructor. Converts wide wchar_t string and
     *          copied first charCount symbols.
     * \param   source      The source of 8-bit ASCII string
     * \param   charCount   The number of character to copy from 
     *                      given string source
     **/
    inline CEWideString( const char * source, int charCount );
    /**
     * \brief   Initialization constructor. Copies data from 8-bit ASCII string
     * \param   source  The source object of 8-bit ASCII string
     **/
    CEWideString( const CEString & source );
    /**
     * \brief   Constructor, initializes string from streaming object
     **/
    CEWideString( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEWideString( void );

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
    inline operator const wchar_t * ( void ) const;

    /**
     * \brief   Assigning operator, copies data from given wide-string source
     * \param   strSource   The source of string to copy.
     * \return  Returns the string object.
     **/
    const CEWideString & operator = ( const CEWideString & strSource );

    /**
     * \brief   Assigning operator, copies data from given null-terminated wide string source
     * \param   strSource   The source of null-terminated string to copy.
     * \return  Returns the string object.
     **/
    const CEWideString & operator = ( const wchar_t * strSource );

    /**
     * \brief   Assigning operator, copies data from given null-terminated string source
     * \param   strSource   The source of null-terminated wide-char string to copy.
     * \return  Returns the string object.
     **/
    const CEWideString & operator = ( const char * strSource );

    /**
     * \brief   Assigning operator, copies data from given character source
     * \param   chSource    The source of character to copy.
     * \return  Returns the string object.
     **/
    const CEWideString & operator = ( wchar_t chSource );

    /**
     * \brief   Assigning operator, copies data from given string source
     * \param   strSource   The source of wide-string to copy.
     * \return  Returns the string object.
     **/
    const CEWideString & operator = ( const CEString & strSource );

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const CEWideString & other) const;

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The null-terminated string to compare
     * \return  Returns true if 2 strings are equal.
     * \see     Compare
     **/
    inline bool operator == (const wchar_t * other) const;

    /**
     * \brief   Determines inequality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are unequal.
     * \see     Compare
     **/
    inline bool operator != (const CEWideString & other) const;

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
    inline bool operator > ( const CEWideString & other ) const;

    /**
     * \brief   Determines whether existing string is less than given strings.
     *          The operator is needed for sorting algorithms
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is less than given.
     * \see     Compare
     **/
    inline bool operator < ( const CEWideString & other ) const;

    /**
     * \brief   Appends given string at the end of existing string.
     * \param   strSource   The source of string to append at the end
     * \return  Returns string object
     **/
    CEWideString & operator += ( const CEWideString & strSource );

    /**
     * \brief   Appends given null-terminated wide string at the end of existing string.
     * \param   strSource   The source of null-terminated string to append at the end
     * \return  Returns string object
     **/
    CEWideString & operator += ( const wchar_t * strSource );

    /**
     * \brief   Appends given null-terminated string at the end of existing string.
     * \param   strSource   The source of null-terminated wide-char string to append at the end
     * \return  Returns string object
     **/
    CEWideString & operator += ( const char * strSource );

    /**
     * \brief   Appends given wide string at the end of existing string.
     * \param   strSource   The source of string to append at the end
     * \return  Returns string object
     **/
    CEWideString & operator += (const CEString & strSource);

    /**
     * \brief   Appends given character at the end of existing string.
     * \param   chSource   The source of wide 16-bit character to append at the end
     * \return  Returns string object
     **/
    CEWideString & operator += ( wchar_t chSource );

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
    friend AREG_API CEWideString operator + (const CEWideString & lhs, const CEWideString & rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side null-terminated wide string
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API CEWideString operator + (const CEWideString & lhs, const wchar_t * rhs);
    
    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side null-terminated wide string
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    friend AREG_API CEWideString operator + (const wchar_t * lhs, const CEWideString & rhs);
    
    /**
     * \brief   Operator to concatenate string and 16-bit character.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side 16-bit character
     * \return  Returns newly constructed string object by concatenating string and char
     **/
    friend AREG_API CEWideString operator + (const CEWideString & lhs, wchar_t chRhs);
    
    /**
     * \brief   Operator to concatenate 16-bit character and string.
     * \param   lhs     Left-hand side 16-bit character
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating char and string
     **/
    friend AREG_API CEWideString operator + (const wchar_t chLhs, const CEWideString & rhs);

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
    friend AREG_API const IEInStream & operator >> (const IEInStream & stream, CEWideString & input);
    
    /**
     * \brief    Streams from output object, i.e. write data from string to streaming object.
     * \param    stream    Streaming object to write data.
     * \param    output    String object to read data from
     * \return    Reference to stream object.
     **/
    friend AREG_API IEOutStream & operator << (IEOutStream & stream, const CEWideString & output);

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
     * \example CEWideString::GetSubstring()
     * 
     *  CEWideString test(L"0123 456 789 0123");
     * 
     *  const wchar_t * next = static_cast<const wchar_t *>(test);                  // next == "0123 456 789 0123"
     *  CEWideString result1 = CEWideString::GetSubstring( next, L"0123", &next);   // results: result1 == ""           , next == " 456 789 0123"
     *  CEWideString result2 = CEWideString::GetSubstring( next, L"0123", &next);   // results: result2 == " 456 789 "  , next == ""
     *  CEWideString result3 = CEWideString::GetSubstring( next, L"0123", &next);   // results: result3 == ""           , next == NULL;
     *  
     *  next = static_cast<const wchar_t *>(test);                                  // next == "0123 456 789 0123"
     *  CEWideString result4 = CEWideString::GetSubstring( next, L" ", &next);      // results: result4 == "0123"          , next == "456 789 0123"
     *  CEWideString result5 = CEWideString::GetSubstring( next, L" ", &next);      // results: result5 == "456 789"       , next == "789 0123"
     *  CEWideString result6 = CEWideString::GetSubstring( next, L" ", &next);      // results: result6 == "789"           , next == "0123";
     *  CEWideString result7 = CEWideString::GetSubstring( next, L" ", &next);      // results: result7 == "0123"          , next == NULL;
     **/
    static CEWideString GetSubstring( const wchar_t * strSource, const wchar_t * strPhrase, const wchar_t ** out_next = static_cast<const wchar_t **>(NULL) );

    /**
     * \brief   Converts given string of digits to 32-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit integer
     **/
    static int32_t StringToInt32( const wchar_t * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
     * \brief   Converts given string of digits to 32-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit unsigned integer
     **/
    static uint32_t StringToUInt32( const wchar_t * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
     * \brief   Converts given string of digits to 64-bit integer
     * \param   strDigit    The string with digits. Can contain negative or positive sign in front
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit integer
     **/
    static int64_t StringToInt64( const wchar_t * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
     * \brief   Converts given string of digits to 64-bit unsigned integer
     * \param   strDigit    The string with digits.
     * \param   radix       The base value when calculate integer.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 64-bit unsigned integer
     **/
    static uint64_t StringToUInt64( const wchar_t * strDigit, NEString::eRadix radix = NEString::RadixDecimal, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
     * \brief   Converts given string of digits to 32-bit digit with floating point
     * \param   strDigit    The string with digits.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
     * \return  Returns the 32-bit digit with floating point
     **/
    static float StringToFloat( const wchar_t * strDigit, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
    * \brief   Converts given string of digits to 64-bit digit with floating point
    * \param   strDigit    The string with digits.
    * \param   end [out]   If not null, on output this contains value of pointer to the next character in strDigit buffer after the numerical value.
    * \return  Returns the 64-bit digit with floating point
    **/
    static double StringToDouble( const wchar_t * strDigit, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );
    /**
     * \brief   Converts given string to boolean value
     * \param   strBoolean  The string to convert.
     * \param   end [out]   If not null, on output this contains value of pointer to the next character in strBoolean buffer after the numerical value.
     * \return  Returns the 64-bit digit with floating point
     **/
    static bool StringToBoolean( const wchar_t * strBoolean, const wchar_t ** end = static_cast<const wchar_t **>(NULL) );

    /**
     * \brief   Converts given signed 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static CEWideString Int32ToString( int32_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given unsigned 32-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static CEWideString UInt32ToString( uint32_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given signed 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static CEWideString Int64ToString( int64_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given unsigned 64-bit integer into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \param   radix       The base value to make conversion. The lowest is 2 (binary) and the highest is hexadecimal (16)
     * \return  Returns converted string.
     **/
    static CEWideString UInt64ToString( uint64_t number, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static CEWideString FloatToString( float number );
    /**
     * \brief   Converts given 32-bit digit with floating point into the string. The conversion is done on radix base, which by default is decimal (10).
     * \param   number      The number to convert to string
     * \return  Returns converted string.
     **/
    static CEWideString DoubleToString( double number );
    /**
     * \brief   Converts given boolean value to string.
     * \param   value   The boolean value to convert to string
     * \return  Returns converted string.
     **/
    static CEWideString BooleanToString( bool value );

    /**
     * \brief   Formats the string. The classic rules similar of 'swprintf' are applied.
     * \param   strDst  The destination where formated string is copied.
     * \param   count   The length of destination buffer.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
     * \return  Returns the number of characters copied in the buffer.
     *          Return zero if nothing is copied.
     *          Returns negative value if error occurred (for example, formating syntax error).
     **/
    static int PrintString( wchar_t * strDst, int count, const wchar_t * format, ... );

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
    static int PrintStringList( wchar_t * strDst, int count, const wchar_t * format, va_list argptr );

public:
    /**
     * \brief   Returns substring of existing string. The substring is formed starting at given position in string buffer
     *          and number of characters to get in string or until end of string if 'charCount' is NEString::CountAll
     * \param   startPos    The starting position in existing string buffer. Should be valid position.
     *                      By default, starting position is begin of string buffer, i.e. NEString::StartPos
     * \param   charCount   The number of characters to copy to substring. If equal to NEString::CountAll,
     *                      it copies characters until end of string.
     * \return  Returns created substring.
     **/
    inline CEWideString Substr( NEString::CharPos startPos = NEString::StartPos, NEString::CharCount charCount = NEString::CountAll ) const;

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
     * \example CEWideString::Substr
     *
     *  CEWideString test(L"0123 456 789 0123");
     *  CEWideString result;
     *  NEString::CharPos next = NEString::StartPos;
     *  next = test.Substr(result, L"0123", next);   // results: next == 4, result == L""
     *  next = test.Substr(result, L"0123", next);   // results: next == NEString::EndPos, result == L" 456 789 "
     *
     *  next = NEString::StartPos;
     *  next = test.Substr(result, L" ", next);      // results: next == 5, result == L"0123"
     *  next = test.Substr(result, L" ", next);      // results: next == 9, result == L"456"
     *  next = test.Substr(result, L" ", next);      // results: next == NEString::InvalidPos, result == L"0123"
     **/
    inline NEString::CharPos Substr( CEWideString & outResult, const wchar_t * strPhrase, NEString::CharPos startPos = NEString::StartPos ) const;

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
    inline NEString::CharPos Substr( CEWideString & outResult, wchar_t chSymbol, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the left side substring of length 'charCount' or empty string is string is empty.
     **/
    inline CEWideString LeftStr( NEString::CharCount charCount ) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the right side substring of length 'charCount' or empty string is string is empty.
     **/
    inline CEWideString RightStr( NEString::CharCount charCount ) const;

    /**
     * \brief   Formats the string. The classic rules similar of 'spintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   ...     Additional arguments
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const CEWideString & Format( const wchar_t * format, ... );

    /**
     * \brief   Formats the string. The classic rules similar of 'vsprintf' are applied.
     * \param   format  The format of string, then followed values to output if they exist.
     * \param   argptr  The list of arguments.
     * \return  Returns the resulting string.
     * \note    By default, it will be 128 character space allocated to format string.
     *          If fails, will try repeat operation with 512 chars
     **/
    const CEWideString & FormatList( const wchar_t * format, va_list argptr );

    /**
     * \brief   Converts string of digits to 32-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit integer
     **/
    inline int32_t ToInt32( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 32-bit unsigned integer
     **/
    inline uint32_t ToUInt32( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit integer
     **/
    inline int64_t ToInt64( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 64-bit unsigned integer
     * \param   radix       The base value when calculate integer.
     * \return  Returns the 64-bit unsigned integer
     **/
    inline uint64_t ToUInt64( NEString::eRadix radix = NEString::RadixDecimal ) const;
    /**
     * \brief   Converts string of digits to 32-bit digit with floating point
     * \return  Returns the 32-bit digit with floating point
     **/
    inline float ToFloat( void ) const;
    /**
     * \brief   Converts string of digits to 64-bit digit with floating point
     * \return  Returns the 64-bit digit with floating point
     **/
    inline double ToDouble( void ) const;
    /**
     * \brief   Converts string to boolean value. If value is "true", it returns true. Otherwise returns false.
     * \return  Returns boolean value.
     **/
    inline bool ToBoolean( void ) const;

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
    inline CEWideString & FromInt32( int32_t value, NEString::eRadix radix = NEString::RadixDecimal );

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
    inline CEWideString & FromUInt32( uint32_t value, NEString::eRadix radix = NEString::RadixDecimal );
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
    inline CEWideString & FromInt64( int64_t value, NEString::eRadix radix = NEString::RadixDecimal );
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
    inline CEWideString & FromUInt64( uint64_t value, NEString::eRadix radix = NEString::RadixDecimal );
    /**
     * \brief   Converts and sets float digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline CEWideString & FromFloat( float value );
    /**
     * \brief   Converts and sets double digit in the string.
     * \param   value   The value of number with floating point to set in the string.
     * \return  Returns string with value.
     **/
    inline CEWideString & FromDouble( double value );
    /**
     * \brief   Converts and sets boolean value in the string.
     * \param   value   The boolean value to set in the string.
     * \return  Returns string with value.
     **/
    inline CEWideString & FromBoolean( bool value );

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
    inline CEWideString & self( void );                 //!< Returns reference object of string itself
    inline const CEWideString & self( void ) const;     //!< Returns reference object of string itself
};

//////////////////////////////////////////////////////////////////////////
// CEWideString class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline CEWideString & CEWideString::self( void )
{   return (*this);     }

inline const CEWideString & CEWideString::self( void ) const
{   return (*this);     }

inline CEWideString::CEWideString( void )
    : TEString<wchar_t>( NEString::EncodeWide )
{   ;   }

inline CEWideString::CEWideString( const wchar_t * source )
    : TEString<wchar_t>( source, NEString::EncodeWide )
{   ;   }

inline CEWideString::CEWideString( wchar_t ch )
    : TEString<wchar_t>( ch, NEString::EncodeWide )
{   ;   }

inline CEWideString::CEWideString( const CEWideString & source )
    : TEString<wchar_t>( static_cast<const TEString<wchar_t> &>(source) )
{   ;   }

inline CEWideString::CEWideString( const wchar_t * source, int charCount )
    : TEString<wchar_t>( source, charCount, NEString::EncodeWide )
{   ;   }

inline CEWideString::CEWideString( const char* const source )
    : TEString<wchar_t>( NULL_STRING_W, NEString::getStringLength<char>( source ), NEString::EncodeWide )
{   NEString::copyString<wchar_t, char>( getString( ), source );                                                    }

inline CEWideString::CEWideString( const char * source, int charCount )
    : TEString<wchar_t>( NULL_STRING_W, charCount, NEString::EncodeWide )
{   NEString::copyString<wchar_t, char>( getString( ), source, NEString::StartPos, charCount );                     }

inline CEWideString::operator const wchar_t *(void) const
{   return String();                                                                                                }

inline bool CEWideString::operator == (const CEWideString & other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other.GetBuffer( ) ) == 0);                      }

inline bool CEWideString::operator == (const wchar_t * other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other ) == 0);                                   }

inline bool CEWideString::operator != (const CEWideString & other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other.GetBuffer( ) ) != 0);                      }

inline bool CEWideString::operator != (const wchar_t * other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other ) != 0);                                   }

inline bool CEWideString::operator > (const CEWideString & other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other ) < 0);                                    }

inline bool CEWideString::operator < (const CEWideString & other) const
{   return (NEString::compareFast<wchar_t, wchar_t>( GetBuffer( ), other ) > 0);                                    }

inline NEString::CharPos CEWideString::Substr( CEWideString & outResult, const wchar_t * strPhrase, NEString::CharPos startPos /*= NEString::StartPos*/ ) const
{   return TEString<wchar_t>::Substr( outResult, strPhrase, startPos );             }

inline NEString::CharPos CEWideString::Substr( CEWideString & outResult, const wchar_t chSymbol, NEString::CharPos startPos /*= NEString::StartPos */ ) const
{   return TEString<wchar_t>::Substr(outResult, chSymbol, startPos);                }

inline CEWideString CEWideString::Substr( NEString::CharPos startPos /*= NEString::StartPos*/, NEString::CharCount charCount /*= NEString::CountAll*/ ) const
{
    CEWideString result(getInitEncoding());
    TEString<wchar_t>::Substr(result, startPos, charCount);
    return result;
}

inline CEWideString CEWideString::LeftStr( NEString::CharCount charCount ) const
{
    CEWideString result( getInitEncoding( ) );
    TEString<wchar_t>::Substr( result, NEString::StartPos, charCount );
    return result;
}

inline CEWideString CEWideString::RightStr( NEString::CharCount charCount ) const
{
    CEWideString result( getInitEncoding( ) );
    NEString::CharPos pos = charCount < GetLength( ) ? GetLength( ) - charCount : NEString::StartPos;
    TEString<wchar_t>::Substr( result, pos, NEString::CountAll );
    return result;
}

inline int32_t CEWideString::ToInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{   return CEWideString::StringToInt32(GetBuffer(), radix, static_cast<const wchar_t **>(NULL) );       }

inline uint32_t CEWideString::ToUInt32( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{   return CEWideString::StringToUInt32(GetBuffer(), radix, static_cast<const wchar_t **>(NULL) );      }

inline int64_t CEWideString::ToInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{   return CEWideString::StringToInt64(GetBuffer(), radix, static_cast<const wchar_t **>(NULL) );       }

inline uint64_t CEWideString::ToUInt64( NEString::eRadix radix /*= NEString::RadixDecimal */ ) const
{   return CEWideString::StringToUInt64(GetBuffer(), radix, static_cast<const wchar_t **>(NULL) );      }

inline float CEWideString::ToFloat( void ) const
{   return CEWideString::StringToFloat(GetBuffer(), static_cast<const wchar_t **>(NULL) );              }

inline double CEWideString::ToDouble( void ) const
{   return CEWideString::StringToDouble(GetBuffer(), static_cast<const wchar_t **>(NULL) );             }

inline bool CEWideString::ToBoolean( void ) const
{   return (NEString::compareIgnoreCase<wchar_t, wchar_t>( String(), BOOLEAN_TRUE ) == 0);              }

inline CEWideString & CEWideString::FromInt32( int32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self() = CEWideString::Int32ToString(value, radix);
    return self();
}

inline CEWideString & CEWideString::FromUInt32( uint32_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = CEWideString::UInt32ToString( value, radix );
    return self( );
}

inline CEWideString & CEWideString::FromInt64( int64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = CEWideString::Int64ToString( value, radix );
    return self( );
}

inline CEWideString & CEWideString::FromUInt64( uint64_t value, NEString::eRadix radix /*= NEString::RadixDecimal */ )
{
    self( ) = CEWideString::UInt64ToString( value, radix );
    return self( );
}

inline CEWideString & CEWideString::FromFloat( float value )
{
    self( ) = CEWideString::FloatToString( value );
    return self( );
}

inline CEWideString & CEWideString::FromDouble( double value )
{
    self( ) = CEWideString::DoubleToString( value );
    return self( );
}

inline CEWideString & CEWideString::FromBoolean( bool value )
{
    self( ) = CEWideString::BooleanToString( value );
    return self( );
}

#endif  // AREG_BASE_CEWideString_HPP
