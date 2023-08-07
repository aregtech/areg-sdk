/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/String.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/IEIOStream.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define strtoll     _strtoi64
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define strtoull    _strtoui64
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define strtof      strtod
#endif

namespace 
{
    //!< A formate chars to generate human readable binary text.
    constexpr char const _formatRadixBinary[] = { '0', '1', '\0' };

//////////////////////////////////////////////////////////////////////////
// String class implementation
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Format to create binary text containing '0' and '1'.
     * \tparam  DigitType   The time of digital number to convert. Must be a primitive.
     * \param   result      On output, this contains the conversion result.
     * \param   number      The digit to convert.
     * \return  Returns the number of characters copied into the string.
     **/
    template<typename DigitType>
    inline int32_t _formatBinary( String & OUT result, DigitType number )
    {
        char buffer[ NEString::MSG_MIN_BUF_SIZE ]{ 0 };
        char * dst  = buffer;
        DigitType base = static_cast<DigitType>(NEString::eRadix::RadixBinary);
        bool isNegative = number < 0;

        number = MACRO_ABS( number );
        short idx = 0;
        do
        {
            idx = static_cast<short>(number % base);

            *dst ++ = _formatRadixBinary[idx < 0 ? 1 : idx];
            number /= base;
        } while ( number != 0 );

        *dst    = static_cast<char>(NEString::EndOfString);
        int32_t count = static_cast<int32_t>(dst - buffer);
        NEString::swapString<char>(buffer, count);
        result = String::EmptyChar;
        if ( isNegative )
        {
            result = '-';
            ++ count;
        }

        result  += buffer;
        return count;
    }

    /**
     * \brief   Formats the digit using predefined formatting.
     *          The buffer to format is allocated in the stack and
     *          the length of buffer is passed as a type argument.
     * \tparam  DigitType   The type of digit to format.
     * \tparam  CharCount   The size of buffer to allocated in the stack.
     *                      By default, the length of buffer in '_MIN_BUF_SIZE'
     * \param   result      On output, contains the conversion result.
     * \param   format      The format to convert the digit.
     * \param   number      The digit to convert.
     * \return  Returns number of characters in the string.
     *          In case of error, the return is negative.
     **/
    template<typename DigitType, int const CharCount = NEString::MSG_MIN_BUF_SIZE>
    inline int32_t _formatDigit( String & result, const char * format, DigitType number )
    {
        char buffer[ CharCount ]{ 0 };

        int32_t count = -1;
#ifdef WINDOWS
        count = ::sprintf_s(buffer, CharCount, format, number);
#else   // !WINDOWS
        count = ::snprintf( buffer, CharCount, format, number);
#endif  // WINDOWS
        result = buffer;
        return count;
    }

    /**
     * \brief   Formats the list of arguments and copies the result into the buffer.
     * \param   buffer  The buffer to copy formatted values.
     * \param   count   The size of the buffer to allocated.
     * \param   format  The format to convert.
     * \param   argptr  The list of arguments to convert.
     * \return  Returns the number of characters in the buffer, not including null-character.
     **/
    inline int _formatStringList( char * buffer, int count, const char * format, va_list argptr )
    {
        int result { -1 };
        if ( buffer != nullptr )
        {
            *buffer = String::EmptyChar;
#ifdef  WINDOWS
            result = vsprintf_s( buffer, static_cast<size_t>(count), format, argptr );
#else   // !WINDOWS
            result = vsnprintf( buffer, count, format, argptr );
#endif  // WINDOWS
        }

        return result;
    }

    /**
     * \brief   Formats the list of arguments and copies the result into the buffer.
     *          The buffer is allocated in the stack.
     * \tparam  CharCount   The size of the buffer to allocate in the stack.
     * \param   result      On output, this contain the result of conversion.
     * \param   format      The format to convert.
     * \param   argptr      The list of arguments to convert.
     * \return  Returns the number of characters in the buffer, not including null-character.
     **/
    template<int const CharCount = NEString::MSG_BUF_SIZE>
    inline int32_t _formatStringList( String & result, const char * format, va_list argptr )
    {
        char buffer[ CharCount ] { 0 };
        int32_t count = _formatStringList( buffer, CharCount, format, argptr );
        result.assign( buffer, count > 0 ? count : 0 );
        return count;
    }

    /**
     * \brief   Formats the string. The buffer is allocated in the stack.
     * \tparam  CharCount   The size of the buffer to allocate in the stack.
     * \param   result      On output, this contain the result of conversion.
     * \param   format      The format to convert.
     * \return  Returns the number of characters in the buffer, not including null-character.
     **/
    template<int const CharCount = NEString::MSG_BUF_SIZE>
    inline int32_t _formatString( String & result, const char * format, ... )
    {
        va_list argptr;
        va_start( argptr, format );
        int32_t count = _formatStringList<CharCount>(result, format, argptr );
        va_end( argptr );
        return count;
    }

    /**
     * \brief   Compare 2 strings of different char-set and returns true if they are equal.
     **/
    inline bool _isEqual(const char* str, const wchar_t* wstr)
    {
        while ((*str != TEString<char>::EmptyChar) && (*wstr++ == static_cast<wchar_t>(*str++)))
            ;

        return (*str == TEString<char>::EmptyChar);
    }

} // namespace


// the static empty string
const String & String::getEmptyString(void)
{
    static const String _emptyString{ "" };
    return _emptyString;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
String::String(const WideString & source)
    : TEString<char>( )
{
    assign(source.getString(), source.getLength());
}

String::String( const IEInStream & stream )
    : TEString<char>( )
{
    readStream(stream);
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////

String & String::operator = (const WideString & src)
{
    assign(src.getString(), src.getLength());
    return (*this);
}

bool String::operator == (const WideString& other) const
{
    bool result = false;
    if (getLength() == other.getLength())
    {
        result = _isEqual(getString(), other.getString());
    }

    return result;
}

bool String::operator == (const std::wstring& other) const
{
    bool result = false;
    if (getLength() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqual(getString(), other.c_str());
    }

    return result;
}

bool String::operator == (const wchar_t* other) const
{
    bool result = false;
    if (getLength() == static_cast<NEString::CharCount>(wcslen(other)))
    {
        result = _isEqual(getString(), other);
    }

    return result;
}

bool String::operator != (const wchar_t* other) const
{
    bool result = true;
    if (getLength() == static_cast<NEString::CharCount>(wcslen(other)))
    {
        result = _isEqual(getString(), other) == false;
    }

    return result;
}

bool String::operator != (const std::wstring& other) const
{
    bool result = true;
    if (getLength() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqual(getString(), other.c_str()) == false;
    }

    return result;
}

bool String::operator != (const WideString& other) const
{
    bool result = true;
    if (getLength() == other.getLength())
    {
        result = _isEqual(getString(), other.getString()) == false;
    }

    return result;
}

String & String::operator += (const WideString & src)
{
    append(src.getString(), src.getLength());
    return (*this);
}

String String::getSubstring(const char * src, const char * strPhrase, const char ** out_next /*= nullptr*/)
{
    String result;
    if ( out_next != nullptr )
    {
        *out_next = nullptr;
    }

    if (NEString::isEmpty<char>(src) == false)
    {
        NEString::CharPos pos = NEString::findFirst<char>(strPhrase, src, NEString::START_POS, out_next);
        result.assign(src, NEString::isPositionValid(pos) ? pos : NEString::COUNT_ALL);
    }

    return result;
}

int32_t String::makeInt32( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    int result  = static_cast<int>(NEString::isEmpty<char>(strDigit) == false ? strtol( strDigit, &temp, static_cast<int>(radix) ) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint32_t String::makeUInt32( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    unsigned int result = static_cast<unsigned int>(NEString::isEmpty<char>(strDigit) == false ? strtoul(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

int64_t String::makeInt64( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    int64_t result = static_cast<long long int>(NEString::isEmpty<char>(strDigit) == false ? strtoll(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint64_t String::makeUInt64(const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= nullptr*/)
{
    char * temp = nullptr;
    uint64_t result = static_cast<uint64_t>(NEString::isEmpty<char>(strDigit) == false ? strtoull(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

float String::makeFloat(const char * strDigit, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    float result = static_cast<float>(NEString::isEmpty<char>(strDigit) == false ? strtof(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

double String::makeDouble(const char * strDigit, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    double result = static_cast<double>(NEString::isEmpty<char>(strDigit) == false ? strtod(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

bool String::makeBool( const char * strBoolean, const char ** end /*= nullptr*/ )
{
    bool result = false;
    int lenSkip = 0;
    int lenTrue = static_cast<int>(NECommon::BOOLEAN_TRUE.length());
    int lenFalse= static_cast<int>(NECommon::BOOLEAN_FALSE.length());
    if ( NEString::compareStrings<char, char>(strBoolean, NECommon::BOOLEAN_TRUE.data(), lenTrue, false) == NEMath::eCompare::Equal)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compareStrings<char, char>(strBoolean, NECommon::BOOLEAN_FALSE.data(), lenFalse, false) == NEMath::eCompare::Equal)
    {
        result = false;
        lenSkip= lenFalse;
    }

    if ( end != nullptr )
    {
        *end = (strBoolean + lenSkip);
    }

    return result;
}

String String::toString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        if ( number < 0)
            _formatDigit<int32_t>( result, "-%0.11o", -1 * number );
        else
            _formatDigit<int32_t>( result, "%0.11o", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t>( result, "-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t>( result, "0x%.8X", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int32_t>( result, "%d", number );
        break;
    }
    return result;
}

String String::toString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint32_t>(result, "%0.11o", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint32_t>(result, "0x%.8X", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint32_t>( result, "%u", number );
        break;
    }
    return result;
}

String String::toString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch (radix)
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        if (number < 0)
            _formatDigit<int64_t>(result, "-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t>(result, "%0.22llo", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        if (number < 0)
            _formatDigit<int64_t>(result, "-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t>(result, "0x%.16llX", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int64_t>(result, "%lld", number);
        break;
    }

    return result;
}

String String::toString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint64_t>( result, "%.22llo", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint64_t>( result, "0x%.16llX", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint64_t>( result, "%llu", number );
        break;
    }

    return result;
}

String String::toString(float number)
{
    String result;
    _formatDigit<float>( result, "%f", number );
    return result;
}

String String::toString(double number)
{
    String result;
    _formatDigit<double>( result, "%g", number );
    return result;
}

String String::toString( bool value )
{
    return String(value ? NECommon::BOOLEAN_TRUE : NECommon::BOOLEAN_FALSE);
}

int String::formatString( char * strDst, int count, const char * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int result = String::formatStringList( strDst, count, format, argptr );
    va_end( argptr );
    return result;
}

int String::formatStringList( char * strDst, int count, const char * format, va_list argptr )
{
    return _formatStringList(strDst, count, format, argptr);
}

const String & String::format(const char * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    formatList(format, argptr);
    
    va_end(argptr);

    return (*this);
}

const String & String::formatList(const char * format, va_list argptr)
{
    clear();
    if (format != nullptr)
    {
        int count = NEString::requiredBufferSize(format, argptr);

        switch ( count )
        {
        case NEString::MSG_MIN_BUF_SIZE:
            _formatStringList< NEString::MSG_MIN_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_BUF_SIZE:
            _formatStringList< NEString::MSG_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_BIG_BUF_SIZE:
            _formatStringList< NEString::MSG_BIG_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_EXTRA_BUF_SIZE:
            _formatStringList< NEString::MSG_EXTRA_BUF_SIZE>( *this, format, argptr );
            break;

        default:
            ASSERT( false ); // put assertion to catch assertion.
            break;
        }
    }

    return (*this);
}

String& String::assign(const wchar_t* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    mData.clear();

    if (NEString::isEmpty<wchar_t>(source) == false)
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(wcslen(source)) : count;
        mData.resize(count);
        char* dst = mData.data();
        while (--count >= 0)
            *dst++ = static_cast<char>(*source++);

        *dst = EmptyChar;
    }

    return (*this);
}

String& String::append(const wchar_t* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (NEString::isEmpty<wchar_t>(source) == false)
    {
        uint32_t len = static_cast<uint32_t>(mData.length());
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(wcslen(source)) : count;
        uint32_t newSize = len + static_cast<uint32_t>(count);
        mData.resize(newSize);
        char* dst = mData.data() + len;
        while (--count >= 0)
        {
            *dst++ = static_cast<char>(*source++);
        }

        *dst = EmptyChar;
    }

    return (*this);
}

void String::readStream(const IEInStream& stream)
{
    stream.read(*this);
}

void String::writeStream(IEOutStream& stream) const
{
    stream.write(*this);
}
