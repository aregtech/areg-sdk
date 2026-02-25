/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/String.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/IOStream.hpp"

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
    inline int32_t _formatBinary( String & result, DigitType number )
    {
        char buffer[ NEString::MSG_MIN_BUF_SIZE ]{ 0 };
        char * dst  = buffer;
        DigitType base = static_cast<DigitType>(NEString::Radix::Binary);
        bool isNegative = number < 0;

        number = NEMath::abs<DigitType>(number);
        int16_t idx = 0;
        do
        {
            idx = static_cast<int16_t>(number % base);

            *dst ++ = _formatRadixBinary[idx < 0 ? 1 : idx];
            number /= base;
        } while ( number != 0 );

        *dst    = static_cast<char>(NEString::EndOfString);
        int32_t count = static_cast<int32_t>(dst - buffer);
        NEString::revert_string<char>(buffer, count);
        if ( isNegative )
        {
            result = '-';
            ++ count;
        }
        else
        {
            result = String::EmptyChar;
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
    template<typename DigitType, int32_t const CharCount = NEString::MSG_MIN_BUF_SIZE>
    inline int32_t _formatDigit( String & result, const char * format, DigitType number )
    {
        char buffer[ CharCount ]{ 0 };

        int32_t count{ -1 };
#ifdef _WIN32
        count = ::sprintf_s(buffer, CharCount, format, number);
#else   // !_WIN32
        count = ::snprintf( buffer, CharCount, format, number);
#endif  // _WIN32
        result.assign(buffer, count > 0 ? count : 0);
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
    inline int32_t _formatStringList( char * buffer, int32_t count, const char * format, va_list argptr )
    {
#ifdef  _WIN32
        return vsprintf_s( buffer, static_cast<size_t>(count), format, argptr );
#else   // !_WIN32
        return vsnprintf( buffer, count, format, argptr );
#endif  // _WIN32
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
    template<int32_t const CharCount = NEString::MSG_BUF_SIZE>
    inline int32_t _formatStringList( String & result, const char * format, va_list argptr )
    {
        char buffer[ CharCount ] { 0 };
        int32_t count = _formatStringList( buffer, CharCount, format, argptr );
        result.assign( buffer, count > 0 ? count : 0 );
        return count;
    }

    /**
     * \brief   Compare 2 strings of different char-set and returns true if they are equal.
     **/
    inline bool _isEqual(const char* str, const wchar_t* wstr)
    {
        while ((*str != StringBase<char>::EmptyChar) && (*wstr++ == static_cast<wchar_t>(*str++)))
            ;

        return (*str == StringBase<char>::EmptyChar);
    }

} // namespace


// the static empty string
const String & String::empty_string()
{
    static const String _emptyString{ "" };
    return _emptyString;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
String::String(const WideString & source)
    : StringBase<char>( )
{
    assign(source.as_string(), source.length());
}

String::String( const InStream & stream )
    : StringBase<char>( )
{
    read_stream(stream);
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////

String & String::operator = (const WideString & src)
{
    assign(src.as_string(), src.length());
    return (*this);
}

bool String::operator == (const WideString& other) const
{
    bool result = false;
    if (length() == other.length())
    {
        result = _isEqual(as_string(), other.as_string());
    }

    return result;
}

bool String::operator == (const std::wstring& other) const
{
    bool result = false;
    if (length() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqual(as_string(), other.c_str());
    }

    return result;
}

bool String::operator == (const wchar_t* other) const
{
    bool result = false;
    if (length() == static_cast<NEString::CharCount>(wcslen(other)))
    {
        result = _isEqual(as_string(), other);
    }

    return result;
}

bool String::operator != (const wchar_t* other) const
{
    bool result = true;
    if (length() == static_cast<NEString::CharCount>(wcslen(other)))
    {
        result = _isEqual(as_string(), other) == false;
    }

    return result;
}

bool String::operator != (const std::wstring& other) const
{
    bool result = true;
    if (length() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqual(as_string(), other.c_str()) == false;
    }

    return result;
}

bool String::operator != (const WideString& other) const
{
    bool result = true;
    if (length() == other.length())
    {
        result = _isEqual(as_string(), other.as_string()) == false;
    }

    return result;
}

String & String::operator += (const WideString & src)
{
    append(src.as_string(), src.length());
    return (*this);
}

String String::substr(const char * src, const char * strPhrase, const char ** out_next /*= nullptr*/)
{
    String result;
    if ( out_next != nullptr )
    {
        *out_next = nullptr;
    }

    if (NEString::is_empty<char>(src) == false)
    {
        NEString::CharPos pos = NEString::find_first<char>(strPhrase, src, NEString::START_POS, true, out_next);
        result.assign(src, NEString::is_position_valid(pos) ? pos : NEString::COUNT_ALL);
    }

    return result;
}

int32_t String::make_int32( const char * strDigit, NEString::Radix radix /*= NEString::Decimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    int32_t result  = static_cast<int32_t>(NEString::is_empty<char>(strDigit) == false ? strtol( strDigit, &temp, static_cast<int32_t>(radix) ) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint32_t String::make_uint32( const char * strDigit, NEString::Radix radix /*= NEString::Decimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    uint32_t result = static_cast<uint32_t>(NEString::is_empty<char>(strDigit) == false ? strtoul(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

int64_t String::make_int64( const char * strDigit, NEString::Radix radix /*= NEString::Decimal*/, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    int64_t result = static_cast<int64_t>(NEString::is_empty<char>(strDigit) == false ? strtoll(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint64_t String::make_uint64(const char * strDigit, NEString::Radix radix /*= NEString::Decimal*/, const char ** end /*= nullptr*/)
{
    char * temp = nullptr;
    uint64_t result = static_cast<uint64_t>(NEString::is_empty<char>(strDigit) == false ? strtoull(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

float String::make_float(const char * strDigit, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    float result = static_cast<float>(NEString::is_empty<char>(strDigit) == false ? strtof(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

double String::make_double(const char * strDigit, const char ** end /*= nullptr*/ )
{
    char * temp = nullptr;
    double result = static_cast<double>(NEString::is_empty<char>(strDigit) == false ? strtod(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

bool String::make_bool( const char * strBoolean, const char ** end /*= nullptr*/ )
{
    bool result = false;
    int32_t lenSkip = 0;
    int32_t lenTrue = static_cast<int32_t>(NECommon::BOOLEAN_TRUE.length());
    int32_t lenFalse= static_cast<int32_t>(NECommon::BOOLEAN_FALSE.length());
    if ( NEString::compare_strings<char, char>(strBoolean, NECommon::BOOLEAN_TRUE.data(), lenTrue, false) == NEMath::Ordering::Equal)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compare_strings<char, char>(strBoolean, NECommon::BOOLEAN_FALSE.data(), lenFalse, false) == NEMath::Ordering::Equal)
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

String String::make_string(int32_t number, NEString::Radix radix /*= NEString::Decimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::Radix::Binary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::Radix::Octal:
        if ( number < 0)
            _formatDigit<int32_t>( result, "-%0.11o", -1 * number );
        else
            _formatDigit<int32_t>( result, "%0.11o", number );
        break;

    case NEString::Radix::Hexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t>( result, "-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t>( result, "0x%.8X", number );
        break;

    case NEString::Radix::Decimal:    // fall through
    case NEString::Radix::Automatic:  // fall through
    default:
        _formatDigit<int32_t>( result, "%d", number );
        break;
    }

    return result;
}

String String::make_string(uint32_t number, NEString::Radix radix /*= NEString::Decimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::Radix::Binary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::Radix::Octal:
        _formatDigit<uint32_t>(result, "%0.11o", number);
        break;

    case NEString::Radix::Hexadecimal:
        _formatDigit<uint32_t>(result, "0x%.8X", number);
        break;

    case NEString::Radix::Decimal:    // fall through
    case NEString::Radix::Automatic:  // fall through
    default:
        _formatDigit<uint32_t>( result, "%u", number );
        break;
    }

    return result;
}

String String::make_string(int64_t number, NEString::Radix radix /*= NEString::Decimal */)
{
    String result;

    switch (radix)
    {
    case NEString::Radix::Binary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::Radix::Octal:
        if (number < 0)
            _formatDigit<int64_t>(result, "-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t>(result, "%0.22llo", number);
        break;

    case NEString::Radix::Hexadecimal:
        if (number < 0)
            _formatDigit<int64_t>(result, "-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t>(result, "0x%.16llX", number);
        break;

    case NEString::Radix::Decimal:    // fall through
    case NEString::Radix::Automatic:  // fall through
    default:
        _formatDigit<int64_t>(result, "%lld", number);
        break;
    }

    return result;
}

String String::make_string(uint64_t number, NEString::Radix radix /*= NEString::Decimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::Radix::Binary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::Radix::Octal:
        _formatDigit<uint64_t>( result, "%.22llo", number );
        break;

    case NEString::Radix::Hexadecimal:
        _formatDigit<uint64_t>( result, "0x%.16llX", number );
        break;

    case NEString::Radix::Decimal:    // fall through
    case NEString::Radix::Automatic:  // fall through
    default:
        _formatDigit<uint64_t>( result, "%llu", number );
        break;
    }

    return result;
}

String String::make_string(float number)
{
    String result;
    _formatDigit<double>( result, "%f", static_cast<double>(number) );
    return result;
}

String String::make_string(double number)
{
    String result;
    _formatDigit<double>( result, "%g", number );
    return result;
}

String String::make_string( bool value )
{
    return String(value ? NECommon::BOOLEAN_TRUE : NECommon::BOOLEAN_FALSE);
}

int32_t String::format_string( char * strDst, int32_t count, const char * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int32_t result{ strDst != nullptr ? _formatStringList( strDst, count, format, argptr ) : -1};
    va_end( argptr );
    return result;
}

int32_t String::format_string_list( char * strDst, int32_t count, const char * format, va_list argptr )
{
    return (strDst != nullptr ? _formatStringList( strDst, count, format, argptr ) : -1);
}

String & String::format(const char * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    format_list(format, argptr);
    
    va_end(argptr);

    return (*this);
}

String & String::format_list(const char * format, va_list argptr)
{
    clear();
    if (format != nullptr)
    {
        int32_t count = NEString::required_buffer_size(format, argptr);

        switch ( count )
        {
        case NEString::MSG_MIN_BUF_SIZE:
            _formatStringList<NEString::MSG_MIN_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_BUF_SIZE:
            _formatStringList<NEString::MSG_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_BIG_BUF_SIZE:
            _formatStringList<NEString::MSG_BIG_BUF_SIZE>( *this, format, argptr );
            break;

        case NEString::MSG_EXTRA_BUF_SIZE:
            _formatStringList<NEString::MSG_EXTRA_BUF_SIZE>( *this, format, argptr );
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

    if (NEString::is_empty<wchar_t>(source) == false)
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(wcslen(source)) : count;
        mData.resize(static_cast<uint32_t>(count));
        char* dst = mData.data();
        while (--count >= 0)
        {
            *dst++ = static_cast<char>(*source++);
        }

        *dst = EmptyChar;
    }

    return (*this);
}

String& String::append(const wchar_t* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (NEString::is_empty<wchar_t>(source) == false)
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

void String::read_stream(const InStream& stream)
{
    stream.read(*this);
}

void String::write_stream(OutStream& stream) const
{
    stream.write(*this);
}
