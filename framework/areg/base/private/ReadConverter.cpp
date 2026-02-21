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

#include "areg/base/IOStream.hpp"
#include "areg/base/Cursor.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// ReadConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ReadConverter::ReadConverter( InStream & readStream, Cursor & readPosition )
    : mReadStream   (readStream)
    , mReadPosition (readPosition)
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool ReadConverter::getBool( bool & Value ) const
{
    static constexpr uint32_t size = sizeof(bool);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getChar( char & Value ) const
{
    static constexpr uint32_t size = sizeof(char);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getChar( wchar_t & Value ) const
{
    static constexpr uint32_t size = sizeof(wchar_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getShort( int16_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int16_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getInt( int32_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int32_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getInt64( int64_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int64_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getFloat( float & Value ) const
{
    static constexpr uint32_t size = sizeof(float);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getDouble( double & Value ) const
{
    static constexpr uint32_t size = sizeof(double);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::getString( String & Value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    Value.clear();

    while ( getChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString<char>(ch) )
            break;
        Value += ch;
    }
    return result;
}

bool ReadConverter::getString( WideString & Value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    Value.clear();

    while ( getChar(ch) )
    {
        result = true;
        if ( NEString::isEndOfString(ch) )
            break;

        Value += ch;
    }

    return result;
}

bool ReadConverter::readLine( String & Value ) const
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
                mReadPosition.setPosition(-1 * static_cast<int32_t>(sizeof(char)), Cursor::SeekOrigin::Current);
            }

            break;
        }

        Value += ch;
    }

    return result;
}

bool ReadConverter::readLine( WideString & Value ) const
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
                mReadPosition.setPosition(-1 * static_cast<int32_t>(sizeof(wchar_t)), Cursor::SeekOrigin::Current);
            }

            break;
        }

        Value += ch;
    }

    return result;
}
