/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ReadConverter.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Read converter helper object
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
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool ReadConverter::getBool( bool & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(bool);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getChar( char & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(char);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getChar( wchar_t & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(wchar_t);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getShort( short & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(short);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getInt( int & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(int);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getInt64( int64_t & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(int64_t);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getFloat( float & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(float);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getDouble( double & OUT out_value ) const
{
    static constexpr unsigned int size = sizeof(double);
    return (mReadStream.read(reinterpret_cast<unsigned char *>(&out_value), size) == size);
}

bool ReadConverter::getString( String & OUT out_value ) const
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

bool ReadConverter::getString( WideString & OUT out_value ) const
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

bool ReadConverter::readLine( String & OUT out_value ) const
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
                mReadPosition.setPosition(-1 * static_cast<int>(sizeof(char)), IECursorPosition::eCursorPosition::PositionCurrent);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}

bool ReadConverter::readLine( WideString & OUT out_value ) const
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
                mReadPosition.setPosition(-1 * static_cast<int>(sizeof(wchar_t)), IECursorPosition::eCursorPosition::PositionCurrent);
            }

            break;
        }

        out_value += ch;
    }

    return result;
}
