/************************************************************************
 * \file        areg/base/private/CEWideString.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, String Class to handle basic
 *              null-terminated string operations.
 ************************************************************************/
#include "areg/base/CEWideString.hpp"
#include "areg/base/CEString.hpp"
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

#define _MAX_BINARY_BUFFER    72
#define _MIN_BUF_SIZE        128
#define _BUF_SIZE            256
#define _MAX_BUF_SIZE        512
#define _EXTRA_BUF_SIZE     1024

static const wchar_t _FormatRadixBinary[] = { L'0', L'1', L'\0' };

template<typename DigitType>
static inline int32_t _formatBinary( CEWideString & result, DigitType number )
{
    wchar_t buffer[_MAX_BINARY_BUFFER];
    wchar_t * dst  = buffer;
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
    NEString::swapString<wchar_t>(buffer, count);
    result = CEWideString::EmptyChar;
    if ( isNegative )
    {
        result = L'-';
        ++ count;
    }
    result  += buffer;
    return count;
}

template<typename DigitType, int const CharCount>
static inline int32_t _formatDigit( CEWideString & result, const wchar_t * format, DigitType number )
{
    wchar_t buffer[CharCount + 1];
    buffer[0] = static_cast<char>(NEString::EndOfString);

    int32_t count = -1;
#ifdef _WIN32
    count = swprintf_s(buffer, CharCount, format, number);
#else   // _WIN32
    count = swprintf( buffer, CharCount, format, number);
#endif  // _WIN32
    result = buffer;
    return count;
}

static inline int32_t _formatStringList( wchar_t * buffer, int32_t count, const wchar_t * format, va_list argptr )
{

    int32_t result = -1;
    if ( buffer != NULL_STRING_W )
    {
        *buffer = static_cast<wchar_t>(NEString::EndOfString);
#ifdef  WIN32
        result = _vsnwprintf_s( buffer, count * sizeof(wchar_t), count - 1, format, argptr );
#else
        result = vswprintf( buffer, count, format, argptr );
#endif
    }
    return result;
}

template<int const CharCount>
static inline int32_t _formatStringList( CEWideString & result, const wchar_t * format, va_list argptr )
{
    
    wchar_t buffer[CharCount];
    int32_t count = _formatStringList(buffer, CharCount, format, argptr);
    result = buffer;
    return count;
}

template<int const CharCount>
static inline int32_t _formatString( CEWideString & result, const wchar_t * format, ... )
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

AREG_API CEWideString operator + (const CEWideString & lhs, const CEWideString & rhs)
{
    CEWideString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEWideString operator + (const CEWideString & lhs, const wchar_t * rhs)
{
    CEWideString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEWideString operator + (const wchar_t * lhs, const CEWideString & rhs)
{
    CEWideString result(lhs);
    result += rhs;
    return result;
}

AREG_API CEWideString operator + (const CEWideString & lhs, wchar_t chRhs)
{
    CEWideString result(lhs);
    result += chRhs;
    return result;
}

AREG_API CEWideString operator + (wchar_t chLhs, const CEWideString & rhs)
{
    CEWideString result(chLhs);
    result += rhs;
    return result;
}

AREG_API const IEInStream & operator >> (const IEInStream & stream, CEWideString & input)
{
    input.readStream(stream);
    return stream;
}

AREG_API IEOutStream & operator << (IEOutStream & stream, const CEWideString & output)
{
    output.writeStream(stream);
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// CEWideString class implementation
//////////////////////////////////////////////////////////////////////////
const wchar_t * const   CEWideString::EmptyString   = L"";
const wchar_t * const   CEWideString::BOOLEAN_TRUE  = L"true";
const wchar_t * const   CEWideString::BOOLEAN_FALSE = L"false";

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEWideString::CEWideString(const CEString & source)
    : TEString<wchar_t>(static_cast<const wchar_t *>(NULL), source.GetLength(), NEString::EncodeWide)
{
    NEString::copyString<wchar_t, char>(getString(), source.GetBuffer(), NEString::StartPos, source.GetLength());
}

CEWideString::CEWideString( const IEInStream & stream )
    : TEString<wchar_t>( NEString::EncodeWide )
{
    readStream(stream);
}

CEWideString::~CEWideString( void )
{
    ;
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////
const CEWideString & CEWideString::operator = (const CEWideString & strSource)
{
    if (this != &strSource)
    {
        release();
        mData = NEString::initString<wchar_t, wchar_t>(strSource.GetBuffer(), strSource.GetLength(), NEString::EncodeAscii);
    }
    return (*this);
}

const CEWideString & CEWideString::operator = (const wchar_t * strSource)
{
    if (GetBuffer() != strSource)
    {
        NEString::SString<wchar_t> * temp = mData;
        mData = NEString::initString<wchar_t, wchar_t>( strSource, NEString::CountAll, NEString::EncodeAscii );
        NEString::releaseSpace<wchar_t>(temp);
    }
    return (*this);
}

const CEWideString & CEWideString::operator = (wchar_t chSource)
{
    release( );
    mData = NEString::initString<wchar_t, wchar_t>( &chSource, 1, NEString::EncodeWide );
    return (*this);
}

const CEWideString & CEWideString::operator = ( const char * strSource )
{
    release( );
    mData = NEString::initString<wchar_t, char>( strSource, NEString::CountAll, NEString::EncodeWide );
    return (*this);
}

const CEWideString & CEWideString::operator = (const CEString & strSource)
{
    release();
    mData = NEString::initString<wchar_t, char>(strSource.GetBuffer(), strSource.GetLength(), NEString::EncodeWide);
    return (*this);
}

CEWideString & CEWideString::operator += (const CEWideString & strSource)
{
    AppendString(strSource.GetBuffer(), strSource.GetLength());
    return (*this);
}

CEWideString & CEWideString::operator += (const wchar_t * strSource)
{
    AppendString( strSource, NEString::getStringLength<wchar_t>( strSource ) );
    return (*this);
}

CEWideString & CEWideString::operator += (const char * strSource)
{
    NEString::CharCount charCount = NEString::getStringLength<char>( strSource );
    EnsureSpace( GetLength( ) + charCount );
    if ( IsValid( ) )
    {
        NEString::appendString<wchar_t, char>( *mData, strSource, charCount );
    }
    return (*this);
}

CEWideString & CEWideString::operator += (const CEString & strSource)
{
    NEString::CharCount charCount = strSource.GetLength();
    EnsureSpace(GetLength() + charCount);
    if (IsValid())
    {
        NEString::appendString<wchar_t, char>(*mData, strSource.GetBuffer(), charCount);
    }
    return (*this);
}

CEWideString & CEWideString::operator += (wchar_t chSource)
{
    AppendString(&chSource, 1);
    return (*this);
}

CEWideString CEWideString::GetSubstring(const wchar_t * strSource, const wchar_t * strPhrase, const wchar_t ** out_next /*= static_cast<const wchar_t **>(NULL) */)
{
    CEWideString result;
    if (out_next != static_cast<const wchar_t **>(NULL))
        *out_next = NULL_STRING_W;

    if (NEString::isEmpty<wchar_t>(strSource) == false)
    {
        NEString::CharPos pos = NEString::searchFirstOf<wchar_t>(strPhrase, strSource, NEString::StartPos, out_next);
        result.CopyString(strSource, pos != NEString::InvalidPos ? pos : NEString::CountAll);
    }
    return result;
}

void CEWideString::readStream(const IEInStream & stream)
{
    stream.Read( *this );
}

void CEWideString::writeStream(IEOutStream & stream) const
{
    stream.Write( *this );
}

wchar_t CEWideString::operator [ ] (int atPos) const
{
    ASSERT(IsValid());
    ASSERT(canRead(atPos));
    return mData->strBuffer[atPos];
}

CEWideString::operator unsigned int(void) const
{
    return (IsValid() ? NEMath::Crc32Calculate(GetBuffer()) : 0);
}

int32_t CEWideString::StringToInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    int result = static_cast<int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstol( strDigit, &temp, static_cast<int>(radix) ) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

uint32_t CEWideString::StringToUInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    unsigned int result = static_cast<unsigned int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoul(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

int64_t CEWideString::StringToInt64( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    long long int result = static_cast<long long int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoll(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

uint64_t CEWideString::StringToUInt64(const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/)
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    uint64_t result = static_cast<uint64_t>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoull(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

float CEWideString::StringToFloat(const wchar_t * strDigit, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    float result = static_cast<float>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstof(strDigit, &temp) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

double CEWideString::StringToDouble(const wchar_t * strDigit, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    double result = static_cast<double>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstod(strDigit, &temp) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;
    return result;
}

bool CEWideString::StringToBoolean( const wchar_t * strBoolean, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    bool result = false;
    int lenSkip = 0;
    int lenTrue = NEString::getStringLength<wchar_t>( BOOLEAN_TRUE );
    int lenFalse = NEString::getStringLength<wchar_t>( BOOLEAN_FALSE );
    if ( NEString::compareStrings<wchar_t, wchar_t>( strBoolean, BOOLEAN_TRUE, lenTrue, false ) == 0 )
    {
        result = true;
        lenSkip = lenTrue;
    }
    else if ( NEString::compareStrings<wchar_t, wchar_t>( strBoolean, BOOLEAN_FALSE, lenFalse, false ) == 0 )
    {
        result = false;
        lenSkip = lenFalse;
    }

    if ( end != static_cast<const wchar_t **>(NULL) )
        *end = (strBoolean + lenSkip);
    return result;
}

CEWideString CEWideString::Int32ToString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEWideString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<int32_t>( result, number );
        break;

    case NEString::RadixOctal:
        if ( number < 0)
            _formatDigit<int32_t, 24>( result, L"-%0.11o", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, L"%0.11o", number );
        break;

    case NEString::RadixHexadecimal:
        if ( number < 0 )
            _formatDigit<int32_t, 24>( result, L"-0x%.8X", -1 * number );
        else
            _formatDigit<int32_t, 24>( result, L"0x%.8X", number );
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<int32_t, 24>( result, L"%d", number );
        break;
    }
    return result;
}

CEWideString CEWideString::UInt32ToString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEWideString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<uint32_t>(result, number);
        break;

    case NEString::RadixOctal:
        _formatDigit<uint32_t, 24>(result, L"%0.11o", number);
        break;

    case NEString::RadixHexadecimal:
        _formatDigit<uint32_t, 24>(result, L"0x%.8X", number);
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint32_t, 24>( result, L"%u", number );
        break;
    }
    return result;
}

CEWideString CEWideString::Int64ToString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEWideString result;

    switch (radix)
    {
    case NEString::RadixBinary:
        _formatBinary<int64_t>(result, number);
        break;

    case NEString::RadixOctal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, L"-%0.22llo", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, L"%0.22llo", number);
        break;

    case NEString::RadixHexadecimal:
        if (number < 0)
            _formatDigit<int64_t, 32>(result, L"-0x%.16llX", -1 * number);
        else
            _formatDigit<int64_t, 32>(result, L"0x%.16llX", number);
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<int64_t, 32>(result, L"%lld", number);
        break;
    }
    return result;
}

CEWideString CEWideString::UInt64ToString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    CEWideString result;

    switch ( radix )
    {
    case NEString::RadixBinary:
        _formatBinary<uint64_t>( result, number );
        break;

    case NEString::RadixOctal:
        _formatDigit<uint64_t, 32>( result, L"%.22llo", number );
        break;

    case NEString::RadixHexadecimal:
        _formatDigit<uint64_t, 32>( result, L"0x%.16llX", number );
        break;

    case NEString::RadixDecimal:    // fall through
    case NEString::RadixAutomatic:  // fall through
    default:
        _formatDigit<uint64_t, 32>( result, L"%llu", number );
        break;
    }
    return result;
}

CEWideString CEWideString::FloatToString(float number)
{
    CEWideString result;
    _formatDigit<float, 32>( result, L"%f", number );
    return result;
}

CEWideString CEWideString::DoubleToString(double number)
{
    CEWideString result;
    _formatDigit<double, 64>( result, L"%g", number );
    return result;
}

CEWideString CEWideString::BooleanToString( bool value )
{
    return CEWideString( value ? BOOLEAN_TRUE : BOOLEAN_FALSE );
}

int CEWideString::PrintString( wchar_t * strDst, int count, const wchar_t * format, ... )
{
    va_list argptr;
    va_start( argptr, format );
    int result = CEWideString::PrintStringList( strDst, count, format, argptr );
    va_end( argptr );
    return result;
}

int CEWideString::PrintStringList( wchar_t * strDst, int count, const wchar_t * format, va_list argptr )
{
    return _formatStringList(strDst, count, format, argptr);
}

const CEWideString & CEWideString::Format(const wchar_t * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    FormatList(format, argptr);
    
    va_end(argptr);

    return self();
}

const CEWideString & CEWideString::FormatList(const wchar_t * format, va_list argptr)
{
    Clear();
    if (format != static_cast<const wchar_t *>(NULL))
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
