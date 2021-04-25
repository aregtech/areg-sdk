/************************************************************************
 * \file        areg/src/base/private/CEString.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/IEIOStream.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

static const char _FormatRadixBinary[] = { '0', '1', '\0' };

template<typename DigitType>
static inline int32_t _formatBinary( CEString & result, DigitType number )
{
    char buffer[_MAX_BINARY_BUFFER];
    char * dst  = buffer;
    DigitType base = static_cast<DigitType>(NEString::RadixBinary);
    bool isNegative = number < 0;
    // if ( number < 0 )
    //    number = NEMath::makeAbsolute<DigitType>(number);
    number = MACRO_ABS( number );
    short idx = 0;
    do
    {
        idx = static_cast<uint32_t>(number % base);
        *dst ++ = _FormatRadixBinary[idx < 0 ? 1 : idx];
        number /= base;
    } while ( number != 0 );
    *dst    = static_cast<char>(NEString::EndOfString);
    int32_t count = static_cast<int32_t>(dst - buffer);
    NEString::swapString<char>(buffer, count);
    result = CEString::EmptyChar;
    if ( isNegative )
    {
        result = '-';
        ++ count;
    }
    result  += buffer;
    return count;
}

template<typename DigitType, int const CharCount>
static inline int32_t _formatDigit( CEString & result, const char * format, DigitType number )
{
    char buffer[CharCount];
    buffer[0] = static_cast<char>(NEString::EndOfString);

    int32_t count = -1;
#ifdef _WIN32
    count = sprintf_s(buffer, CharCount, format, number);
#else   // _WIN32
    count = snprintf( buffer, CharCount, format, number);
#endif  // _WIN32
    result = buffer;
    return count;
}

static inline int32_t _formatStringList( char * buffer, int32_t count, const char * format, va_list argptr )
{

    int32_t result = -1;
    if ( buffer != NULL_STRING )
    {
        *buffer = static_cast<char>(NEString::EndOfString);
#ifdef  WIN32
        result = vsprintf_s( buffer, count, format, argptr );
#else
        result = vsnprintf( buffer, count, format, argptr );
#endif
    }
    return result;
}

template<int const CharCount>
static inline int32_t _formatStringList( CEString & result, const char * format, va_list argptr )
{
    
    char buffer[CharCount];
    int32_t count = _formatStringList(buffer, CharCount, format, argptr);
    result = buffer;
    return count;
}

template<int const CharCount>
static inline int32_t _formatString( CEString & result, const char * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int32_t count = _formatStringList<CharCount>(result, format, argptr );
    va_end( argptr );
    return count;
}

//////////////////////////////////////////////////////////////////////////
// Friend methods
//////////////////////////////////////////////////////////////////////////

AREG_API CEString operator + (const CEString & lhs, const CEString & rhs)
{
    CEString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEString operator + (const CEString & lhs, const char * rhs)
{
    CEString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEString operator + (const char * lhs, const CEString & rhs)
{
    CEString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEString operator + (const CEString & lhs, char chRhs)
{
    CEString result(lhs);
    result += chRhs;
    return result;
}

AREG_API CEString operator + (char chLhs, const CEString & rhs)
{
    CEString result(chLhs);
    result += rhs;
    return result;
}

AREG_API const IEInStream & operator >> (const IEInStream & stream, CEString & input)
{
    input.readStream(stream);
    return stream;
}

AREG_API IEOutStream & operator << (IEOutStream & stream, const CEString & output)
{
    output.writeStream(stream);
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// CEString class implementation
//////////////////////////////////////////////////////////////////////////
const char * const   CEString::EmptyString      = "";
const char * const   CEString::BOOLEAN_TRUE     = "true";
const char * const   CEString::BOOLEAN_FALSE    = "false";

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEString::CEString(const CEWideString & source)
    : TEString<char>(NULL_STRING, source.GetLength(), NEString::EncodeAscii)
{
    NEString::copyString<char, wchar_t>(getString(), source.GetBuffer(), NEString::StartPos, source.GetLength());
}

CEString::CEString( const IEInStream & stream )
    : TEString<char>( NEString::EncodeAscii )
{
    readStream(stream);
}

CEString::~CEString( void )
{
    ;
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////
const CEString & CEString::operator = (const CEString & strSource)
{
    if (this != &strSource)
    {
        release();
        mData = NEString::initString<char, char>(strSource.GetBuffer(), strSource.GetLength(), NEString::EncodeAscii);

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }
    return (*this);
}

const CEString & CEString::operator = (const char * strSource)
{
    if (GetBuffer() != strSource)
    {
        NEString::SString<char> * temp = mData;
        mData = NEString::initString<char, char>( strSource, NEString::CountAll, NEString::EncodeAscii );
        NEString::releaseSpace<char>(temp);

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }

    return (*this);
}

const CEString & CEString::operator = (char chSource)
{
    release( );
    mData = NEString::initString<char, char>( &chSource, 1, NEString::EncodeAscii );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

const CEString & CEString::operator = ( const wchar_t * strSource )
{
    release( );
    mData = NEString::initString<char, wchar_t>( strSource, NEString::CountAll, NEString::EncodeAscii );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

const CEString & CEString::operator = (const CEWideString & strSource)
{
    release();
    mData = NEString::initString<char, wchar_t>(strSource.GetBuffer(), strSource.GetLength(), NEString::EncodeAscii);

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

bool CEString::operator == (const CEWideString & other) const
{
    return (NEString::compareFast<char, wchar_t>( GetBuffer( ), other.GetBuffer( ) ) == 0);
}

CEString & CEString::operator += (const CEString & strSource)
{
    AppendString(strSource.GetBuffer(), strSource.GetLength());
    return (*this);
}

CEString & CEString::operator += (const char * strSource)
{
    AppendString( strSource, NEString::getStringLength<char>( strSource ) );
    return (*this);
}

CEString & CEString::operator += (const wchar_t * strSource)
{
    NEString::CharCount charCount = NEString::getStringLength<wchar_t>( strSource );
    EnsureSpace( GetLength( ) + charCount );
    if ( IsValid( ) )
    {
        NEString::appendString<char, wchar_t>( *mData, strSource, charCount );
    }
    return (*this);
}

CEString & CEString::operator += (const CEWideString & strSource)
{
    NEString::CharCount charCount = strSource.GetLength();
    EnsureSpace(GetLength() + charCount);
    if (IsValid())
    {
        NEString::appendString<char, wchar_t>(*mData, strSource.GetBuffer(), charCount);
    }
    return (*this);
}

CEString & CEString::operator += (char chSource)
{
    AppendString(&chSource, 1);
    return (*this);
}

CEString CEString::GetSubstring(const char * strSource, const char * strPhrase, const char ** out_next /*= static_cast<const char **>(NULL) */)
{
    CEString result;
    if ( out_next != static_cast<const char **>(NULL) )
        *out_next = NULL_STRING;

    if (NEString::isEmpty<char>(strSource) == false)
    {
        NEString::CharPos pos = NEString::searchFirstOf<char>(strPhrase, strSource, NEString::StartPos, out_next);
        result.CopyString(strSource, pos != NEString::InvalidPos ? pos : NEString::CountAll);
    }
    return result;
}

void CEString::readStream(const IEInStream & stream)
{
    stream.Read( *this );
}

void CEString::writeStream(IEOutStream & stream) const
{
    stream.Write(*this);
}

char CEString::operator [ ] (int atPos) const
{
    ASSERT(IsValid());
    ASSERT(canRead(atPos));
    return mData->strBuffer[atPos];
}

CEString::operator unsigned int(void) const
{
    return (IsValid() ? NEMath::Crc32Calculate(GetBuffer()) : 0);
}

int32_t CEString::StringToInt32( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    char * temp = static_cast<char *>(NULL);
    int result = static_cast<int>(NEString::isEmpty<char>(strDigit) == false ? strtol( strDigit, &temp, static_cast<int>(radix) ) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

uint32_t CEString::StringToUInt32( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    char * temp = static_cast<char *>(NULL);
    unsigned int result = static_cast<unsigned int>(NEString::isEmpty<char>(strDigit) == false ? strtoul(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

int64_t CEString::StringToInt64( const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    char * temp = static_cast<char *>(NULL);
    long long int result = static_cast<long long int>(NEString::isEmpty<char>(strDigit) == false ? strtoll(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

uint64_t CEString::StringToUInt64(const char * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const char ** end /*= static_cast<const char **>(NULL)*/)
{
    char * temp = static_cast<char *>(NULL);
    uint64_t result = static_cast<uint64_t>(NEString::isEmpty<char>(strDigit) == false ? strtoull(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

float CEString::StringToFloat(const char * strDigit, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    char * temp = static_cast<char *>(NULL);
    float result = static_cast<float>(NEString::isEmpty<char>(strDigit) == false ? strtof(strDigit, &temp) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

double CEString::StringToDouble(const char * strDigit, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    char * temp = static_cast<char *>(NULL);
    double result = static_cast<double>(NEString::isEmpty<char>(strDigit) == false ? strtod(strDigit, &temp) : 0);
    if (end != static_cast<const char **>(NULL))
        *end = temp != static_cast<char *>(NULL) ? temp : strDigit;
    return result;
}

bool CEString::StringToBoolean( const char * strBoolean, const char ** end /*= static_cast<const char **>(NULL)*/ )
{
    bool result = false;
    int lenSkip = 0;
    int lenTrue = NEString::getStringLength<char>(BOOLEAN_TRUE);
    int lenFalse= NEString::getStringLength<char>(BOOLEAN_FALSE);
    if ( NEString::compareStrings<char, char>(strBoolean, BOOLEAN_TRUE, lenTrue, false) == 0)
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compareStrings<char, char>(strBoolean, BOOLEAN_FALSE, lenFalse, false) == 0)
    {
        result = false;
        lenSkip = lenFalse;
    }

    if ( end != static_cast<const char **>(NULL) )
        *end = (strBoolean + lenSkip);
    return result;
}

CEString CEString::Int32ToString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::RadixOctal:
        if ( number < 0)
            _formatDigit<int32_t, 24>( result, "-%0.11o", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, "%0.11o", number );
        break;

    case NEString::RadixHexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t, 24>( result, "-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, "0x%.8X", number );
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<int32_t, 24>( result, "%d", number );
        break;
    }
    return result;
}

CEString CEString::UInt32ToString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::RadixOctal:
        _formatDigit<uint32_t, 24>(result, "%0.11o", number);
        break;

    case NEString::RadixHexadecimal:
        _formatDigit<uint32_t, 24>(result, "0x%.8X", number);
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint32_t, 24>( result, "%u", number );
        break;
    }
    return result;
}

CEString CEString::Int64ToString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEString result;

    switch (radix)
    {
    case NEString::RadixBinary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::RadixOctal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, "-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, "%0.22llo", number);
        break;

    case NEString::RadixHexadecimal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, "-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, "0x%.16llX", number);
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<int64_t, 32>(result, "%lld", number);
        break;
    }
    return result;
}

CEString CEString::UInt64ToString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::RadixOctal:
        _formatDigit<uint64_t, 32>( result, "%.22llo", number );
        break;

    case NEString::RadixHexadecimal:
        _formatDigit<uint64_t, 32>( result, "0x%.16llX", number );
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint64_t, 32>( result, "%llu", number );
        break;
    }
    return result;
}

CEString CEString::FloatToString(float number)
{
    CEString result;
    _formatDigit<float, 32>( result, "%f", number );
    return result;
}

CEString CEString::DoubleToString(double number)
{
    CEString result;
    _formatDigit<double, 64>( result, "%g", number );
    return result;
}

CEString CEString::BooleanToString( bool value )
{
    return CEString( value ? BOOLEAN_TRUE : BOOLEAN_FALSE);
}

int CEString::PrintString( char * strDst, int count, const char * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int result = CEString::PrintStringList( strDst, count, format, argptr );
    va_end( argptr );
    return result;
}

int CEString::PrintStringList( char * strDst, int count, const char * format, va_list argptr )
{
    return _formatStringList(strDst, count, format, argptr);
}

const CEString & CEString::Format(const char * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    FormatList(format, argptr);
    
    va_end(argptr);

    return self();
}

const CEString & CEString::FormatList(const char * format, va_list argptr)
{
    Clear();
    if (format != static_cast<const char *>(NULL))
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
