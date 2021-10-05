/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
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

#define _MAX_BINARY_BUFFER    72
#define _MIN_BUF_SIZE        128
#define _BUF_SIZE            256
#define _MAX_BUF_SIZE        512
#define _EXTRA_BUF_SIZE     1024

namespace 
{
    constexpr char const _formatRadixBinary[] = { '0', '1', '\0' };

//////////////////////////////////////////////////////////////////////////
// String class implementation
//////////////////////////////////////////////////////////////////////////

    template<typename DigitType>
    inline int32_t _formatBinary( String & result, DigitType number )
    {
        char buffer[_MAX_BINARY_BUFFER];
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

    template<typename DigitType, int const CharCount>
    inline int32_t _formatDigit( String & result, const char * format, DigitType number )
    {
        char buffer[CharCount];
        buffer[0] = static_cast<char>(NEString::EndOfString);

        int32_t count = -1;
#ifdef _WIN32
        count = ::sprintf_s(buffer, CharCount, format, number);
#else   // _WIN32
        count = ::snprintf( buffer, CharCount, format, number);
#endif  // _WIN32
        result = buffer;
        return count;
    }

    inline int _formatStringList( char * buffer, int count, const char * format, va_list argptr )
    {

        int result = -1;
        if ( buffer != NULL_STRING )
        {
            *buffer = static_cast<char>(NEString::EndOfString);
#ifdef  WIN32
            result = vsprintf_s( buffer, static_cast<size_t>(count), format, argptr );
#else   // !WIN32
            result = vsnprintf( buffer, count, format, argptr );
#endif  // !WIN32
        }
        return result;
    }

    template<int const CharCount>
    inline int32_t _formatStringList( String & result, const char * format, va_list argptr )
    {
    
        char buffer[CharCount];
        int32_t count = _formatStringList(buffer, CharCount, format, argptr);
        result = buffer;
        return count;
    }

    template<int const CharCount>
    inline int32_t _formatString( String & result, const char * format, ... )
    {
        va_list argptr;
        va_start( argptr, format );
        int32_t count = _formatStringList<CharCount>(result, format, argptr );
        va_end( argptr );
        return count;
    }

} // namespace

//////////////////////////////////////////////////////////////////////////
// Friend methods
//////////////////////////////////////////////////////////////////////////

AREG_API String operator + (const String & lhs, const String & rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

AREG_API String operator + (const String & lhs, const char * rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

AREG_API String operator + (const char * lhs, const String & rhs)
{
    String result(lhs);
    result += rhs;
    return result;
}

AREG_API String operator + (const String & lhs, char chRhs)
{
    String result(lhs);
    result += chRhs;
    return result;
}

AREG_API String operator + (char chLhs, const String & rhs)
{
    String result(chLhs);
    result += rhs;
    return result;
}

AREG_API const IEInStream & operator >> (const IEInStream & stream, String & input)
{
    input.readStream(stream);
    return stream;
}

AREG_API IEOutStream & operator << (IEOutStream & stream, const String & output)
{
    output.writeStream(stream);
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
String::String(const WideString & source)
    : TEString<char>(NULL_STRING, source.getLength())
{
    NEString::copyString<char, wchar_t>(getDataString(), source.getString(), NEString::START_POS, source.getLength());
#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
}

String::String( const IEInStream & stream )
    : TEString<char>( )
{
    readStream(stream);
#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////
String & String::operator = (const String & src)
{
    if (this != &src)
    {
        release();
        if ( src.isEmpty() == false)
        {
            mData = NEString::initString<char, char>(src.getString(), src.getLength());
        }

#ifdef DEBUG
        mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
    }

    return (*this);
}

String & String::operator = ( String && src ) noexcept
{
    if ( this != &src )
    {
        release( );
        mData = src.mData;
        src.mData = nullptr;

#ifdef DEBUG
        mString = mData != nullptr ? mData->strBuffer : nullptr;
        src.mString = nullptr;
#endif // DEBUG
    }

    return (*this);
}

String & String::operator = (const char * src)
{
    if (getString() != src)
    {
        NEString::SString<char> * temp = mData;
        mData = NEString::initString<char, char>( src, NEString::COUNT_ALL );
        NEString::releaseSpace<char>(temp);

#ifdef DEBUG
        mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
    }

    return (*this);
}

String & String::operator = (char chSource)
{
    release( );
    mData = NEString::initString<char, char>( &chSource, 1 );

#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG

    return (*this);
}

String & String::operator = ( const wchar_t * src )
{
    release( );
    mData = NEString::initString<char, wchar_t>( src, NEString::COUNT_ALL );

#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG

    return (*this);
}

String & String::operator = (const WideString & src)
{
    release();
    mData = NEString::initString<char, wchar_t>(src.getString(), src.getLength());

#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG

    return (*this);
}

bool String::operator == (const WideString & other) const
{
    int len = getLength();
    if ( len == other.getLength() )
    {
        return (NEString::compareFast<char, wchar_t>( getString( ), other.getString( ) ) == 0);
    }
    else
    {
        return false;
    }
}

String & String::operator += (const String & src)
{
    append(src.getString(), src.getLength());
    return (*this);
}

String & String::operator += (const char * src)
{
    append( src, NEString::getStringLength<char>( src ) );
    return (*this);
}

String & String::operator += (const wchar_t * src)
{
    NEString::CharCount charCount = NEString::getStringLength<wchar_t>( src );
    resize( getLength( ) + charCount );
    if ( isValid( ) )
    {
        NEString::appendString<char, wchar_t>( *mData, src, charCount );
    }

#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG

    return (*this);
}

String & String::operator += (const WideString & src)
{
    NEString::CharCount charCount = src.getLength();
    resize(getLength() + charCount);
    if (isValid())
    {
        NEString::appendString<char, wchar_t>(*mData, src.getString(), charCount);
    }

#ifdef DEBUG
    mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG

    return (*this);
}

String & String::operator += (char chSource)
{
    append(&chSource, 1);
    return (*this);
}

String String::getSubstring(const char * src, const char * strPhrase, const char ** out_next /*= nullptr*/)
{
    String result;
    if ( out_next != nullptr )
    {
        *out_next = NULL_STRING;
    }

    if (NEString::isEmpty<char>(src) == false)
    {
        NEString::CharPos pos = NEString::findFirstOf<char>(strPhrase, src, NEString::START_POS, out_next);
        result.copy(src, pos != NEString::INVALID_POS ? pos : NEString::COUNT_ALL);
    }

    return result;
}

void String::readStream(const IEInStream & stream)
{
    stream.read( *this );
}

void String::writeStream(IEOutStream & stream) const
{
    stream.write(*this);
}

char String::operator [ ] (int atPos) const
{
    ASSERT(isValid());
    ASSERT(canRead(atPos));
    return mData->strBuffer[atPos];
}

String::operator unsigned int (void) const
{
    const char * str = getBuffer();
    if ( getLength( ) > 4 )
    {
        const unsigned int * buf = reinterpret_cast<const unsigned int *>(str);
        return (*buf);
    }
    else if ( isValid() )
    {
        return NEMath::crc32Calculate( str );
    }
    else
    {
        return NEMath::CHECKSUM_IGNORE;
    }
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
    if ( NEString::compareStrings<char, char>(strBoolean, NECommon::BOOLEAN_TRUE.data(), lenTrue, false) == 0)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compareStrings<char, char>(strBoolean, NECommon::BOOLEAN_FALSE.data(), lenFalse, false) == 0)
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

String String::int32ToString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        if ( number < 0)
            _formatDigit<int32_t, 24>( result, "-%0.11o", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, "%0.11o", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t, 24>( result, "-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, "0x%.8X", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int32_t, 24>( result, "%d", number );
        break;
    }
    return result;
}

String String::uint32ToString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint32_t, 24>(result, "%0.11o", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint32_t, 24>(result, "0x%.8X", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint32_t, 24>( result, "%u", number );
        break;
    }
    return result;
}

String String::int64ToString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch (radix)
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::eRadix::RadixOctal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, "-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, "%0.22llo", number);
        break;

    case NEString::eRadix::RadixHexadecimal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, "-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, "0x%.16llX", number);
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<int64_t, 32>(result, "%lld", number);
        break;
    }
    return result;
}

String String::uint64ToString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    String result;

    switch ( radix )
    {
    case NEString::eRadix::RadixBinary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::eRadix::RadixOctal:
        _formatDigit<uint64_t, 32>( result, "%.22llo", number );
        break;

    case NEString::eRadix::RadixHexadecimal:
        _formatDigit<uint64_t, 32>( result, "0x%.16llX", number );
        break;

    case NEString::eRadix::RadixDecimal:    // fall through
    case NEString::eRadix::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint64_t, 32>( result, "%llu", number );
        break;
    }
    return result;
}

String String::floatToString(float number)
{
    String result;
    _formatDigit<float, 32>( result, "%f", number );
    return result;
}

String String::doubleToString(double number)
{
    String result;
    _formatDigit<double, 64>( result, "%g", number );
    return result;
}

String String::boolToString( bool value )
{
    return (value ? String(NECommon::BOOLEAN_TRUE.data(),  static_cast<int>(NECommon::BOOLEAN_TRUE.length() )) :
                    String(NECommon::BOOLEAN_FALSE.data(), static_cast<int>(NECommon::BOOLEAN_FALSE.length())) );
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

const String & String::formatString(const char * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    formatList(format, argptr);
    
    va_end(argptr);

    return self();
}

const String & String::formatList(const char * format, va_list argptr)
{
    clear();
    if (format != nullptr)
    {
        if ( _formatStringList<_MIN_BUF_SIZE>( self( ), format, argptr ) < 0 )
        {
            if ( _formatStringList<_BUF_SIZE>( self( ), format, argptr ) < 0 )
            {
                if ( _formatStringList<_MAX_BUF_SIZE>( self( ), format, argptr ) < 0 )
                {
                    _formatStringList<_EXTRA_BUF_SIZE>( self( ), format, argptr );
                }
            }
        }
    }
    return self( );
}

int String::setString( const unsigned char * buffer )
{
    (*this) = reinterpret_cast<const char *>(buffer);
    return getLength();
}
