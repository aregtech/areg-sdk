/************************************************************************
 * \file        areg/base/private/ReadConverter.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Read converter helper object
 *              Reads data from readable streaming object and converts
 *              data to basic types.
 *
 ************************************************************************/
#include "areg/base/private/ReadConverter.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/IECursorPosition.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// ReadConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ReadConverter::ReadConverter( IEInStream & readStream, IECursorPosition & readPosition )
    : mReadStream   (readStream)
    , mReadPosition (readPosition)
{
    ; // do nothing
}

ReadConverter::~ReadConverter( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool ReadConverter::getBool( bool & out_value ) const
{
    static const unsigned int size = sizeof(bool);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getChar( char & out_value ) const
{
    static const unsigned int size = sizeof(char);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getChar( wchar_t & out_value ) const
{
    static const unsigned int size = sizeof(wchar_t);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getShort( short & out_value ) const
{
    static const unsigned int size = sizeof(short);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getInt( int & out_value ) const
{
    static const unsigned int size = sizeof(int);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getLong(long & out_value) const
{
    static const unsigned int size = sizeof(long);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getInt64( int64_t & out_value ) const
{
    static const unsigned int size = sizeof(int64_t);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getFloat( float & out_value ) const
{
    static const unsigned int size = sizeof(float);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getDouble( double & out_value ) const
{
    static const unsigned int size = sizeof(double);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getString( String & out_value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    out_value.clear();

    while ( getChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString<char>(ch) )
            break;
        out_value += ch;
    }
    return result;
}

bool ReadConverter::getString( WideString & out_value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    out_value.clear();

    while ( getChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString(ch) )
            break;

        out_value += ch;
    }

    return result;
}

bool ReadConverter::readLine( String & out_value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    while ( getChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfLine<char>(ch) )
        {
            if (getChar(ch) && (NEString::isCarriageReturn<char>(ch) == false) && (NEString::isEndOfString<char>(ch) == false))
            {
                mReadPosition.setPosition(-1 * static_cast<int>(sizeof(char)), IECursorPosition::POSITION_CURRENT);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}

bool ReadConverter::readLine( WideString & out_value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    while (getChar(ch))
    {
        result = true;
        if (NEString::isEndOfLine<wchar_t>(ch))
        {
            if ( getChar( ch ) && (NEString::isCarriageReturn<wchar_t>( ch ) == false) && (NEString::isEndOfString<wchar_t>( ch ) == false) )
            {
                mReadPosition.setPosition(-1 * static_cast<int>(sizeof(wchar_t)), IECursorPosition::POSITION_CURRENT);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}
