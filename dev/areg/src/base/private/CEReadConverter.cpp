/************************************************************************
 * \file        areg/src/base/private/CEReadConverter.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Read converter helper object
 *              Reads data from readable streaming object and converts
 *              data to basic types.
 *
 ************************************************************************/
#include "areg/src/base/private/CEReadConverter.hpp"

#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/IECursorPosition.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEReadConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEReadConverter::CEReadConverter( IEInStream & readStream, IECursorPosition & readPosition )
    : mReadStream   (readStream)
    , mReadPosition (readPosition)
{
    ; // do nothing
}

CEReadConverter::~CEReadConverter( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool CEReadConverter::GetBool( bool & out_value ) const
{
    static const unsigned int size = sizeof(bool);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetChar( char & out_value ) const
{
    static const unsigned int size = sizeof(char);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetChar( wchar_t & out_value ) const
{
    static const unsigned int size = sizeof(wchar_t);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetShort( short & out_value ) const
{
    static const unsigned int size = sizeof(short);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetInt( int & out_value ) const
{
    static const unsigned int size = sizeof(int);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetLong(long & out_value) const
{
    static const unsigned int size = sizeof(long);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetInt64( int64_t & out_value ) const
{
    static const unsigned int size = sizeof(int64_t);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetFloat( float & out_value ) const
{
    static const unsigned int size = sizeof(float);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetDouble( double & out_value ) const
{
    static const unsigned int size = sizeof(double);
    return (mReadStream.Read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool CEReadConverter::GetString( CEString & out_value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    out_value.Clear();

    while ( GetChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString<char>(ch) )
            break;
        out_value += ch;
    }
    return result;
}

bool CEReadConverter::GetString( CEWideString & out_value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    out_value.Clear();

    while ( GetChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString(ch) )
            break;

        out_value += ch;
    }

    return result;
}

bool CEReadConverter::ReadLine( CEString & out_value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    while ( GetChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfLine<char>(ch) )
        {
            if (GetChar(ch) && (NEString::isCarriageReturn<char>(ch) == false) && (NEString::isEndOfString<char>(ch) == false))
            {
                mReadPosition.SetPosition(-1 * static_cast<int>(sizeof(char)), IECursorPosition::POSITION_CURRENT);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}

bool CEReadConverter::ReadLine( CEWideString & out_value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    while (GetChar(ch))
    {
        result = true;
        if (NEString::isEndOfLine<wchar_t>(ch))
        {
            if ( GetChar( ch ) && (NEString::isCarriageReturn<wchar_t>( ch ) == false) && (NEString::isEndOfString<wchar_t>( ch ) == false) )
            {
                mReadPosition.SetPosition(-1 * static_cast<int>(sizeof(wchar_t)), IECursorPosition::POSITION_CURRENT);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}
