/************************************************************************
 * \file        areg/base/private/WideString.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, String Class to handle basic
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

#define _MAX_BINARY_BUFFER    72
#define _MIN_BUF_SIZE        128
#define _BUF_SIZE            256
#define _MAX_BUF_SIZE        512
#define _EXTRA_BUF_SIZE     1024

static const wchar_t _FormatRadixBinary[] = { L'0', L'1', L'\0' };

template<typename DigitType>
static inline int32_t _formatBinary( WideString & result, DigitType number )
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
    result = WideString::EmptyChar;
    if ( isNegative )
    {
        result = L'-';
        ++ count;
    }
    result  += buffer;
    return count;
}

template<typename DigitType, int const CharCount>
static inline int32_t _formatDigit( WideString & result, const wchar_t * format, DigitType number )
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
static inline int32_t _formatStringList( WideString & result, const wchar_t * format, va_list argptr )
{
    
    wchar_t buffer[CharCount];
    int32_t count = _formatStringList(buffer, CharCount, format, argptr);
    result = buffer;
    return count;
}

template<int const CharCount>
static inline int32_t _formatString( WideString & result, const wchar_t * format, ... )
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

AREG_API WideString operator + (const WideString & lhs, const WideString & rhs)
{
    WideString result(lhs);
    result += rhs;
    return result;
}

AREG_API WideString operator + (const WideString & lhs, const wchar_t * rhs)
{
    WideString result(lhs);
    result += rhs;
    return result;
}

AREG_API WideString operator + (const wchar_t * lhs, const WideString & rhs)
{
    WideString result(lhs);
    result += rhs;
    return result;
}

AREG_API WideString operator + (const WideString & lhs, wchar_t chRhs)
{
    WideString result(lhs);
    result += chRhs;
    return result;
}

AREG_API WideString operator + (wchar_t chLhs, const WideString & rhs)
{
    WideString result(chLhs);
    result += rhs;
    return result;
}

AREG_API const IEInStream & operator >> (const IEInStream & stream, WideString & input)
{
    input.readStream(stream);
    return stream;
}

AREG_API IEOutStream & operator << (IEOutStream & stream, const WideString & output)
{
    output.writeStream(stream);
    return stream;
}

//////////////////////////////////////////////////////////////////////////
// WideString class implementation
//////////////////////////////////////////////////////////////////////////
const wchar_t * const   WideString::EmptyString   = L"";
const wchar_t * const   WideString::BOOLEAN_TRUE  = L"true";
const wchar_t * const   WideString::BOOLEAN_FALSE = L"false";
const WideString		WideString::InvalidString;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
WideString::WideString(const String & source)
    : TEString<wchar_t>(static_cast<const wchar_t *>(NULL), source.getLength(), NEString::EncodeWide)
{
    NEString::copyString<wchar_t, char>(getDataString(), source.getString(), NEString::StartPos, source.getLength());
}

WideString::WideString( const IEInStream & stream )
    : TEString<wchar_t>( NEString::EncodeWide )
{
    readStream(stream);
}

WideString::~WideString( void )
{
    ;
}

//////////////////////////////////////////////////////////////////////////
// operators / operations
//////////////////////////////////////////////////////////////////////////
const WideString & WideString::operator = (const WideString & strSource)
{
    if (this != &strSource)
    {
        release();
        mData = NEString::initString<wchar_t, wchar_t>(strSource.getString(), strSource.getLength(), NEString::EncodeAscii);

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }

    return (*this);
}

const WideString & WideString::operator = (const wchar_t * strSource)
{
    if (getString() != strSource)
    {
        NEString::SString<wchar_t> * temp = mData;
        mData = NEString::initString<wchar_t, wchar_t>( strSource, NEString::CountAll, NEString::EncodeAscii );
        NEString::releaseSpace<wchar_t>(temp);

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }

    return (*this);
}

const WideString & WideString::operator = (wchar_t chSource)
{
    release( );
    mData = NEString::initString<wchar_t, wchar_t>( &chSource, 1, NEString::EncodeWide );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

const WideString & WideString::operator = ( const char * strSource )
{
    release( );
    mData = NEString::initString<wchar_t, char>( strSource, NEString::CountAll, NEString::EncodeWide );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

const WideString & WideString::operator = (const String & strSource)
{
    release();
    mData = NEString::initString<wchar_t, char>(strSource.getString(), strSource.getLength(), NEString::EncodeWide);

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG

    return (*this);
}

bool WideString::operator == (const String & other) const
{
    return (NEString::compareFast<wchar_t, char>( getString( ), other.getString( ) ) == 0);
}

WideString & WideString::operator += (const WideString & strSource)
{
    append(strSource.getString(), strSource.getLength());
    return (*this);
}

WideString & WideString::operator += (const wchar_t * strSource)
{
    append( strSource, NEString::getStringLength<wchar_t>( strSource ) );
    return (*this);
}

WideString & WideString::operator += (const char * strSource)
{
    NEString::CharCount charCount = NEString::getStringLength<char>( strSource );
    resize( getLength( ) + charCount );
    if ( isValid( ) )
    {
        NEString::appendString<wchar_t, char>( *mData, strSource, charCount );
    }

    return (*this);
}

WideString & WideString::operator += (const String & strSource)
{
    NEString::CharCount charCount = strSource.getLength();
    resize(getLength() + charCount);
    if (isValid())
    {
        NEString::appendString<wchar_t, char>(*mData, strSource.getString(), charCount);
    }

    return (*this);
}

WideString & WideString::operator += (wchar_t chSource)
{
    append(&chSource, 1);
    return (*this);
}

WideString WideString::getSubstring(const wchar_t * strSource, const wchar_t * strPhrase, const wchar_t ** out_next /*= static_cast<const wchar_t **>(NULL) */)
{
    WideString result;
    if ( out_next != static_cast<const wchar_t **>(NULL))
        *out_next = NULL_STRING_W;

    if (NEString::isEmpty<wchar_t>(strSource) == false)
    {
        NEString::CharPos pos = NEString::findFirstOf<wchar_t>(strPhrase, strSource, NEString::StartPos, out_next);
        result.copy(strSource, pos != NEString::InvalidPos ? pos : NEString::CountAll);
    }

    return result;
}

void WideString::readStream(const IEInStream & stream)
{
    stream.read( *this );
}

void WideString::writeStream(IEOutStream & stream) const
{
    stream.write(*this);
}

wchar_t WideString::operator [ ] (int atPos) const
{
    ASSERT(isValid());
    ASSERT(canRead(atPos));
    return mData->strBuffer[atPos];
}

WideString::operator unsigned int(void) const
{
    return (isValid() ? NEMath::crc32Calculate(getString()) : NEMath::CHECKSUM_IGNORE);
}

int32_t WideString::makeInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    int result = static_cast<int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstol( strDigit, &temp, static_cast<int>(radix) ) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

uint32_t WideString::makeUInt32( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    unsigned int result = static_cast<unsigned int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoul(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

int64_t WideString::makeInt64( const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    long long int result = static_cast<long long int>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoll(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

uint64_t WideString::makeUInt64(const wchar_t * strDigit, NEString::eRadix radix /*= NEString::RadixDecimal*/, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/)
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    uint64_t result = static_cast<uint64_t>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstoull(strDigit, &temp, static_cast<int>(radix)) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

float WideString::makeFloat(const wchar_t * strDigit, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    float result = static_cast<float>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstof(strDigit, &temp) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

double WideString::makeDouble(const wchar_t * strDigit, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    wchar_t * temp = static_cast<wchar_t *>(NULL);
    double result = static_cast<double>(NEString::isEmpty<wchar_t>(strDigit) == false ? wcstod(strDigit, &temp) : 0);
    if (end != static_cast<const wchar_t **>(NULL))
        *end = temp != static_cast<wchar_t *>(NULL) ? temp : strDigit;

    return result;
}

bool WideString::makeBool( const wchar_t * strBoolean, const wchar_t ** end /*= static_cast<const wchar_t **>(NULL)*/ )
{
    bool result = false;
    int lenSkip = 0;
    int lenTrue = NEString::getStringLength<wchar_t>(BOOLEAN_TRUE);
    int lenFalse= NEString::getStringLength<wchar_t>(BOOLEAN_FALSE);
    if ( NEString::compareStrings<wchar_t, wchar_t>(strBoolean, BOOLEAN_TRUE, lenTrue, false ) == 0 )
    {
        result = true;
        lenSkip= lenTrue;
    }
    else if ( NEString::compareStrings<wchar_t, wchar_t>( strBoolean, BOOLEAN_FALSE, lenFalse, false ) == 0 )
    {
        result = false;
        lenSkip= lenFalse;
    }

    if ( end != static_cast<const wchar_t **>(NULL) )
        *end = (strBoolean + lenSkip);

    return result;
}

WideString WideString::int32ToString(int32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

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

WideString WideString::uint32ToString(uint32_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

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

WideString WideString::int64ToString(int64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

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

WideString WideString::uint64ToString(uint64_t number, NEString::eRadix radix /*= NEString::RadixDecimal */)
{
    WideString result;

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

WideString WideString::floatToString(float number)
{
    WideString result;
    _formatDigit<float, 32>( result, L"%f", number );
    return result;
}

WideString WideString::doubleToString(double number)
{
    WideString result;
    _formatDigit<double, 64>( result, L"%g", number );
    return result;
}

WideString WideString::boolToString( bool value )
{
    return WideString( value ? BOOLEAN_TRUE : BOOLEAN_FALSE );
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

const WideString & WideString::formatString(const wchar_t * format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    formatList(format, argptr);
    
    va_end(argptr);

    return self();
}

const WideString & WideString::formatList(const wchar_t * format, va_list argptr)
{
    clear();
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
