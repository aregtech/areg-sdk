/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/WideString.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, WideString Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/IOStream.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define wcstoll     _wcstoi64
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define wcstoull    _wcstoui64
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    #define wcstof      wcstod
#endif

namespace {
    //!< A formate chars to generate human readable binary text.
    constexpr wchar_t const _formatRadixBinary[] = { L'0', L'1', L'\0' };

//////////////////////////////////////////////////////////////////////////
// WideString class implementation
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Format to create binary text containing '0' and '1'.
     * \tparam  DigitType   The time of digital number to convert. Must be a primitive.
     * \param   result      On output, this contains the conversion result.
     * \param   number      The digit to convert.
     * \return  Returns the number of characters copied into the string.
     **/
    template<typename DigitType>
    inline int32_t _format_binary( areg::WideString & result, DigitType number )
    {
        wchar_t buffer[ areg::MSG_MIN_BUF_SIZE ]{ 0 };
        wchar_t * dst  = buffer;
        DigitType base = static_cast<DigitType>(areg::Radix::Binary);
        bool isNegative = number < 0;

        number = areg::abs<DigitType>(number);
        int16_t idx = 0;
        do
        {
            idx = static_cast<int16_t>(number % base);

            *dst ++ = _formatRadixBinary[idx < 0 ? 1 : idx];
            number /= base;
        } while ( number != 0 );

        *dst    = static_cast<wchar_t>(areg::EndOfString);
        int32_t count = static_cast<int32_t>(dst - buffer);
        areg::revert_string<wchar_t>(buffer, count);
        if ( isNegative )
        {
            result = L'-';
            ++ count;
        }
        else
        {
            result = areg::WideString::EmptyChar;
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
    template<typename DigitType, int32_t const CharCount = areg::MSG_MIN_BUF_SIZE>
    inline int32_t _format_digit( areg::WideString & result, const wchar_t * format, DigitType number )
    {
        wchar_t buffer[ CharCount ] { 0 };

        int32_t count{ -1 };
#ifdef _WIN32
        count = ::swprintf_s(buffer, CharCount, format, number);
#else   // _WIN32
        count = ::swprintf( buffer, CharCount, format, number);
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
    inline int32_t _format_string_list( wchar_t * buffer, int32_t count, const wchar_t * format, va_list argptr )
    {
#ifdef  _WIN32
        return vswprintf_s( buffer, static_cast<size_t>(count), format, argptr );
#else   // !_WIN32
        return vswprintf( buffer, count, format, argptr );
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
    template<int32_t const CharCount = areg::MSG_MIN_BUF_SIZE>
    inline int32_t _format_string_list( areg::WideString & result, const wchar_t * format, va_list argptr )
    {
        wchar_t buffer[ CharCount ] { 0 };
        int32_t count = _format_string_list( buffer, CharCount, format, argptr );
        result.assign( buffer, count > 0 ? count : 0 );
        return count;
    }

    /**
     * \brief   Compare 2 strings of different char-set and returns true if they are equal.
     **/
    inline bool _is_equal(const wchar_t* str, const char* wstr)
    {
        while ((*str != areg::StringBase<wchar_t>::EmptyChar) && (*wstr++ == static_cast<char>(*str++)))
            ;

        return (*str == areg::StringBase<wchar_t>::EmptyChar);
    }

} // namespace

namespace areg {

const WideString & WideString::empty_string()
{
    static const WideString _emptyString{ L"" };
    return _emptyString;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
WideString::WideString(const String & source)
    : StringBase<wchar_t>( )
{
    assign(source.as_string(), source.length());
}

WideString::WideString( const InStream & stream )
    : StringBase<wchar_t>( )
{
    from_stream(stream);
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////

WideString & WideString::operator = (const String & src)
{
    assign(src.as_string(), src.length());
    return (*this);
}

bool WideString::operator == (const String& other) const
{
    bool result = false;
    if (length() == other.length())
    {
        result = _is_equal(as_string(), other.as_string());
    }

    return result;
}

bool WideString::operator == (const std::string& other) const
{
    bool result = false;
    if (length() == static_cast<areg::CharCount>(other.length()))
    {
        result = _is_equal(as_string(), other.c_str());
    }

    return result;
}

bool WideString::operator == (const char* other) const
{
    if (other == nullptr)
        return is_empty();

    bool result = false;
    if (length() == static_cast<areg::CharCount>(strlen(other)))
    {
        result = _is_equal(as_string(), other);
    }

    return result;
}

bool WideString::operator != (const char* other) const
{
    if (other == nullptr)
        return !is_empty();

    bool result = true;
    if (length() == static_cast<areg::CharCount>(strlen(other)))
    {
        result = _is_equal(as_string(), other) == false;
    }

    return result;
}

bool WideString::operator != (const std::string& other) const
{
    bool result = true;
    if (length() == static_cast<areg::CharCount>(other.length()))
    {
        result = _is_equal(as_string(), other.c_str()) == false;
    }

    return result;
}

bool WideString::operator != (const String& other) const
{
    bool result = true;
    if (length() == other.length())
    {
        result = _is_equal(as_string(), other.as_string()) == false;
    }

    return result;
}

WideString & WideString::operator += (const String & src)
{
    append(src.as_string(), src.length());
    return (*this);
}

WideString WideString::substr(const wchar_t * src, const wchar_t * strPhrase, const wchar_t ** out_next /*= nullptr*/)
{
    WideString result;
    if ( out_next != nullptr )
    {
        *out_next = nullptr;
    }

    if (areg::is_empty<wchar_t>(src) == false)
    {
        areg::CharPos pos = areg::find_first<wchar_t>(strPhrase, src, areg::START_POS, true, out_next);
        result.assign(src, areg::is_position_valid(pos) ? pos : areg::COUNT_ALL);
    }

    return result;
}

int32_t WideString::make_int32( const wchar_t * strDigit, areg::Radix radix /*= areg::Decimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    int32_t result  = static_cast<int32_t>(areg::is_empty<wchar_t>(strDigit) == false ? wcstol( strDigit, &temp, static_cast<int32_t>(radix) ) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint32_t WideString::make_uint32( const wchar_t * strDigit, areg::Radix radix /*= areg::Decimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    uint32_t result = static_cast<uint32_t>(areg::is_empty<wchar_t>(strDigit) == false ? wcstoul(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

int64_t WideString::make_int64( const wchar_t * strDigit, areg::Radix radix /*= areg::Decimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    int64_t result = static_cast<int64_t>(areg::is_empty<wchar_t>(strDigit) == false ? wcstoll(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint64_t WideString::make_uint64(const wchar_t * strDigit, areg::Radix radix /*= areg::Decimal*/, const wchar_t ** end /*= nullptr*/)
{
    wchar_t * temp = nullptr;
    uint64_t result = static_cast<uint64_t>(areg::is_empty<wchar_t>(strDigit) == false ? wcstoull(strDigit, &temp, static_cast<int32_t>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

float WideString::make_float(const wchar_t * strDigit, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    float result = static_cast<float>(areg::is_empty<wchar_t>(strDigit) == false ? wcstof(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

double WideString::make_double(const wchar_t * strDigit, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    double result = static_cast<double>(areg::is_empty<wchar_t>(strDigit) == false ? wcstod(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

bool WideString::make_bool( const wchar_t * strBoolean, const wchar_t ** end /*= nullptr*/ )
{
    bool result = false;
    int32_t lenSkip = 0;
    int32_t lenTrue = static_cast<int32_t>(areg::BOOLEAN_TRUE.length());
    int32_t lenFalse= static_cast<int32_t>(areg::BOOLEAN_FALSE.length());
    if ( areg::compare_strings<wchar_t, char>(strBoolean, areg::BOOLEAN_TRUE.data(), lenTrue, false) == areg::Ordering::Equal)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( areg::compare_strings<wchar_t, char>(strBoolean, areg::BOOLEAN_FALSE.data(), lenFalse, false) == areg::Ordering::Equal)
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

WideString WideString::make_string(int32_t number, areg::Radix radix /*= areg::Decimal */)
{
    WideString result;

    switch ( radix )
    {
    case areg::Radix::Binary:
        _format_binary<int32_t>( result, number );
        break;

    case areg::Radix::Octal:
        if ( number < 0)
            _format_digit<int32_t>( result, L"-%0.11o", -1 * number );
        else
            _format_digit<int32_t>( result, L"%0.11o", number );
        break;

    case areg::Radix::Hexadecimal:
        if ( number < 0 )
            _format_digit<int32_t>( result, L"-0x%.8X", -1 * number );
        else
            _format_digit<int32_t>( result, L"0x%.8X", number );
        break;

    case areg::Radix::Decimal:    // fall through
    case areg::Radix::Automatic:  // fall through
    default:
        _format_digit<int32_t>( result, L"%d", number );
        break;
    }

    return result;
}

WideString WideString::make_string(uint32_t number, areg::Radix radix /*= areg::Decimal */)
{
    WideString result;

    switch ( radix )
    {
    case areg::Radix::Binary:
        _format_binary<uint32_t>(result, number);
        break;

    case areg::Radix::Octal:
        _format_digit<uint32_t>(result, L"%0.11o", number);
        break;

    case areg::Radix::Hexadecimal:
        _format_digit<uint32_t>(result, L"0x%.8X", number);
        break;

    case areg::Radix::Decimal:    // fall through
    case areg::Radix::Automatic:  // fall through
    default:
        _format_digit<uint32_t>( result, L"%u", number );
        break;
    }

    return result;
}

WideString WideString::make_string(int64_t number, areg::Radix radix /*= areg::Decimal */)
{
    WideString result;

    switch (radix)
    {
    case areg::Radix::Binary:
        _format_binary<int64_t>(result, number);
        break;

    case areg::Radix::Octal:
        if (number < 0)
            _format_digit<int64_t>(result, L"-%0.22llo", -1 * number);
        else
            _format_digit<int64_t>(result, L"%0.22llo", number);
        break;

    case areg::Radix::Hexadecimal:
        if (number < 0)
            _format_digit<int64_t>(result, L"-0x%.16llX", -1 * number);
        else
            _format_digit<int64_t>(result, L"0x%.16llX", number);
        break;

    case areg::Radix::Decimal:    // fall through
    case areg::Radix::Automatic:  // fall through
    default:
        _format_digit<int64_t>(result, L"%lld", number);
        break;
    }

    return result;
}

WideString WideString::make_string(uint64_t number, areg::Radix radix /*= areg::Decimal */)
{
    WideString result;

    switch ( radix )
    {
    case areg::Radix::Binary:
        _format_binary<uint64_t>( result, number );
        break;

    case areg::Radix::Octal:
        _format_digit<uint64_t>( result, L"%.22llo", number );
        break;

    case areg::Radix::Hexadecimal:
        _format_digit<uint64_t>( result, L"0x%.16llX", number );
        break;

    case areg::Radix::Decimal:    // fall through
    case areg::Radix::Automatic:  // fall through
    default:
        _format_digit<uint64_t>( result, L"%llu", number );
        break;
    }

    return result;
}

WideString WideString::make_string(float number)
{
    WideString result;
    _format_digit<double>( result, L"%f", static_cast<double>(number) );
    return result;
}

WideString WideString::make_string(double number)
{
    WideString result;
    _format_digit<double>( result, L"%g", number );
    return result;
}

WideString WideString::make_string( bool value )
{
    return WideString(value ? areg::BOOLEAN_TRUE : areg::BOOLEAN_FALSE);
}

int32_t WideString::format_string( wchar_t * strDst, int32_t count, const wchar_t * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int32_t result{ strDst != nullptr ? _format_string_list( strDst, count, format, argptr ) : -1};
    va_end( argptr );
    return result;
}

int32_t WideString::format_string_list( wchar_t * strDst, int32_t count, const wchar_t * format, va_list argptr )
{
    return (strDst != nullptr ? _format_string_list( strDst, count, format, argptr ) : -1);
}

WideString & WideString::format(const wchar_t * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    format_list(format, argptr);
    
    va_end(argptr);

    return (*this);
}

WideString & WideString::format_list(const wchar_t * format, va_list argptr)
{
    clear();
    if (format != nullptr)
    {
        int32_t count = areg::required_char_count(format, argptr);

        switch ( count )
        {
        case areg::MSG_MIN_BUF_SIZE:
            _format_string_list<areg::MSG_MIN_BUF_SIZE>( *this, format, argptr );
            break;

        case areg::MSG_BUF_SIZE:
            _format_string_list<areg::MSG_BUF_SIZE>( *this, format, argptr );
            break;

        case areg::MSG_BIG_BUF_SIZE:
            _format_string_list<areg::MSG_BIG_BUF_SIZE>( *this, format, argptr );
            break;

        case areg::MSG_EXTRA_BUF_SIZE:
            _format_string_list<areg::MSG_EXTRA_BUF_SIZE>( *this, format, argptr );
            break;

        default:
            ASSERT( false ); // put assertion to catch assertion.
            break;
        }
    }

    return (*this);
}

WideString& WideString::assign(const char* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    mData.clear();

    if (areg::is_empty<char>(source) == false)
    {
        count = count == areg::COUNT_ALL ? static_cast<areg::CharCount>(strlen(source)) : count;
        mData.resize(static_cast<uint32_t>(count));
        wchar_t* dst = mData.data();
        while (--count >= 0)
        {
            *dst++ = static_cast<wchar_t>(*source++);
        }

        *dst = EmptyChar;
    }

    return (*this);
}

WideString& WideString::append(const char* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (areg::is_empty<char>(source) == false)
    {
        uint32_t len = static_cast<uint32_t>(mData.length());
        count = count == areg::COUNT_ALL ? static_cast<areg::CharCount>(strlen(source)) : count;
        uint32_t newSize = len + static_cast<uint32_t>(count);
        mData.resize(newSize);
        wchar_t* dst = mData.data() + len;
        while (--count >= 0)
        {
            *dst++ = static_cast<wchar_t>(*source++);
        }

        *dst = EmptyChar;
    }

    return (*this);
}

void WideString::from_stream(const InStream& stream)
{
    stream.read(*this);
}

void WideString::to_stream(OutStream& stream) const
{
    stream.write(*this);
}

} // namespace areg
