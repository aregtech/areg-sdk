/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/WideString.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, WideString Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/IEIOStream.hpp"

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

namespace 
{
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
    inline int32_t _formatBinary( WideString & OUT result, DigitType number )
    {
        wchar_t buffer[ NEString::MSG_MIN_BUF_SIZE ]{ 0 };
        wchar_t * dst  = buffer;
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

        *dst    = static_cast<wchar_t>(NEString::EndOfString);
        int32_t count = static_cast<int32_t>(dst - buffer);
        NEString::swapString<wchar_t>(buffer, count);
        result = WideString::EmptyChar;
        if ( isNegative )
        {
            result = L'-';
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
    inline int32_t _formatDigit( WideString & result, const wchar_t * format, DigitType number )
    {
        wchar_t buffer[ CharCount ] { 0 };

        int32_t count = -1;
#ifdef WINDOWS
        count = ::swprintf_s(buffer, CharCount, format, number);
#else   // WINDOWS
        count = ::swprintf( buffer, CharCount, format, number);
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
    inline int _formatStringList( wchar_t * buffer, int count, const wchar_t * format, va_list argptr )
    {
        int result { -1 };
        if ( buffer != nullptr )
        {
            *buffer = WideString::EmptyChar;
#ifdef  WINDOWS
            result = vswprintf_s( buffer, static_cast<size_t>(count), format, argptr );
#else   // !WINDOWS
            result = vswprintf( buffer, count, format, argptr );
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
    template<int const CharCount = NEString::MSG_MIN_BUF_SIZE>
    inline int32_t _formatStringList( WideString & result, const wchar_t * format, va_list argptr )
    {
        wchar_t buffer[ CharCount ] { 0 };
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
    inline int32_t _formatString( WideString & result, const wchar_t * format, ... )
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
    inline bool _isEqua(const wchar_t* str, const char* wstr)
    {
        while ((*str != TEString<wchar_t>::EmptyChar) && (*wstr++ == static_cast<char>(*str++)))
            ;

        return (*str == TEString<wchar_t>::EmptyChar);
    }

} // namespace


// the static empty string
const WideString & WideString::getEmptyString(void)
{
    static const WideString _emptyString{ L"" };
    return _emptyString;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
WideString::WideString(const String & source)
    : TEString<wchar_t>( )
{
    assign(source.getString(), source.getLength());
}

WideString::WideString( const IEInStream & stream )
    : TEString<wchar_t>( )
{
    readStream(stream);
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////

WideString & WideString::operator = (const String & src)
{
    assign(src.getString(), src.getLength());
    return (*this);
}

bool WideString::operator == (const String& other) const
{
    bool result = false;
    if (getLength() == other.getLength())
    {
        result = _isEqua(getString(), other.getString());
    }

    return result;
}

bool WideString::operator == (const std::string& other) const
{
    bool result = false;
    if (getLength() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqua(getString(), other.c_str());
    }

    return result;
}

bool WideString::operator == (const char* other) const
{
    bool result = false;
    if (getLength() == static_cast<NEString::CharCount>(strlen(other)))
    {
        result = _isEqua(getString(), other);
    }

    return result;
}

bool WideString::operator != (const char* other) const
{
    bool result = true;
    if (getLength() == static_cast<NEString::CharCount>(strlen(other)))
    {
        result = _isEqua(getString(), other) == false;
    }

    return result;
}

bool WideString::operator != (const std::string& other) const
{
    bool result = true;
    if (getLength() == static_cast<NEString::CharCount>(other.length()))
    {
        result = _isEqua(getString(), other.c_str()) == false;
    }

    return result;
}

bool WideString::operator != (const String& other) const
{
    bool result = true;
    if (getLength() == other.getLength())
    {
        result = _isEqua(getString(), other.getString()) == false;
    }

    return result;
}

WideString & WideString::operator += (const String & src)
{
    append(src.getString(), src.getLength());
    return (*this);
}

WideString WideString::getSubstring(const wchar_t * src, const wchar_t * strPhrase, const wchar_t ** out_next /*= nullptr*/)
{
    WideString result;
    if ( out_next != nullptr )
    {
        *out_next = nullptr;
    }

    if (NEString::isEmpty<wchar_t>(src) == false)
    {
        NEString::CharPos pos = NEString::findFirst<wchar_t>(strPhrase, src, NEString::START_POS, out_next);
        result.assign(src, NEString::isPositionValid(pos) ? pos : NEString::COUNT_ALL);
    }

    return result;
}

int32_t WideString::makeInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    int result  = static_cast<int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstol( strDigit, &temp, static_cast<int>(radix) ) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint32_t WideString::makeUInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    unsigned int result = static_cast<unsigned int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoul(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

int64_t WideString::makeInt64( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    int64_t result = static_cast<long long int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoll(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

uint64_t WideString::makeUInt64(const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= nullptr*/)
{
    wchar_t * temp = nullptr;
    uint64_t result = static_cast<uint64_t>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoull(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

float WideString::makeFloat(const wchar_t * strDigit, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    float result = static_cast<float>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstof(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

double WideString::makeDouble(const wchar_t * strDigit, const wchar_t ** end /*= nullptr*/ )
{
    wchar_t * temp = nullptr;
    double result = static_cast<double>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstod(strDigit, &temp) : 0);
    if (end != nullptr)
    {
        *end = temp != nullptr ? temp : strDigit;
    }

    return result;
}

bool WideString::makeBool( const wchar_t * strBoolean, const wchar_t ** end /*= nullptr*/ )
{
    bool result = false;
    int lenSkip = 0;
    int lenTrue = static_cast<int>(NECommon::BOOLEAN_TRUE.length());
    int lenFalse= static_cast<int>(NECommon::BOOLEAN_FALSE.length());
    if ( NEString::compareStrings<wchar_t, char>(strBoolean, NECommon::BOOLEAN_TRUE.data(), lenTrue, false) == NEMath::eCompare::Equal)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compareStrings<wchar_t, char>(strBoolean, NECommon::BOOLEAN_FALSE.data(), lenFalse, false) == NEMath::eCompare::Equal)
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

WideString WideString::toString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        if ( number < 0)
            _formatDigit<int32_t>( result, L"-%0.11o", -1 * number );
        else
            _formatDigit<int32_t>( result, L"%0.11o", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t>( result, L"-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t>( result, L"0x%.8X", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int32_t>( result, L"%d", number );
        break;
    }
    return result;
}

WideString WideString::toString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint32_t>(result, L"%0.11o", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint32_t>(result, L"0x%.8X", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint32_t>( result, L"%u", number );
        break;
    }
    return result;
}

WideString WideString::toString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

    switch (radix)
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        if (number < 0)
            _formatDigit<int64_t>(result, L"-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t>(result, L"%0.22llo", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        if (number < 0)
            _formatDigit<int64_t>(result, L"-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t>(result, L"0x%.16llX", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int64_t>(result, L"%lld", number);
        break;
    }

    return result;
}

WideString WideString::toString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint64_t>( result, L"%.22llo", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint64_t>( result, L"0x%.16llX", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint64_t>( result, L"%llu", number );
        break;
    }

    return result;
}

WideString WideString::toString(float number)
{
    WideString result;
    _formatDigit<float>( result, L"%f", number );
    return result;
}

WideString WideString::toString(double number)
{
    WideString result;
    _formatDigit<double>( result, L"%g", number );
    return result;
}

WideString WideString::toString( bool value )
{
    return WideString(value ? NECommon::BOOLEAN_TRUE : NECommon::BOOLEAN_FALSE);
}

int WideString::formatString( wchar_t * strDst, int count, const wchar_t * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int result = WideString::formatStringList( strDst, count, format, argptr );
    va_end( argptr );
    return result;
}

int WideString::formatStringList( wchar_t * strDst, int count, const wchar_t * format, va_list argptr )
{
    return _formatStringList(strDst, count, format, argptr);
}

const WideString & WideString::format(const wchar_t * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    formatList(format, argptr);
    
    va_end(argptr);

    return (*this);
}

const WideString & WideString::formatList(const wchar_t * format, va_list argptr)
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

WideString& WideString::assign(const char* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    mData.clear();

    if (NEString::isEmpty<char>(source) == false)
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(strlen(source)) : count;
        mData.resize(count);
        wchar_t* dst = mData.data();
        while (--count >= 0)
            *dst++ = static_cast<wchar_t>(*source++);

        *dst = EmptyChar;
    }

    return (*this);
}

WideString& WideString::append(const char* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (NEString::isEmpty<char>(source) == false)
    {
        uint32_t len = static_cast<uint32_t>(mData.length());
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(strlen(source)) : count;
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

void WideString::readStream(const IEInStream& stream)
{
    stream.read(*this);
}

void WideString::writeStream(IEOutStream& stream) const
{
    stream.write(*this);
}
