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
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ReadConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ReadConverter::ReadConverter( InStream & read_stream, Cursor & readPosition )
    : mReadStream   (read_stream)
    , mReadPosition (readPosition)
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool ReadConverter::as_bool( bool & Value ) const
{
    static constexpr uint32_t size = sizeof(bool);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_char( char & Value ) const
{
    static constexpr uint32_t size = sizeof(char);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_char( wchar_t & Value ) const
{
    static constexpr uint32_t size = sizeof(wchar_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_short( int16_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int16_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_int( int32_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int32_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::int64( int64_t & Value ) const
{
    static constexpr uint32_t size = sizeof(int64_t);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_float( float & Value ) const
{
    static constexpr uint32_t size = sizeof(float);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_double( double & Value ) const
{
    static constexpr uint32_t size = sizeof(double);
    return (mReadStream.read(reinterpret_cast<uint8_t *>(&Value), size) == size);
}

bool ReadConverter::as_string( String & Value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    Value.clear();

    while ( as_char(ch) )
    {
        result = true;
        if ( areg::is_eos<char>(ch) )
            break;
        Value += ch;
    }
    return result;
}

bool ReadConverter::as_string( WideString & Value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    Value.clear();

    while ( as_char(ch) )
    {
        result = true;
        if ( areg::is_eos(ch) )
            break;

        Value += ch;
    }

    return result;
}

bool ReadConverter::read_line( String & Value ) const
{
    bool result = false;
    char ch     = static_cast<char>('\0');
    while ( as_char(ch) )
    {
        result = true;
        if ( areg::is_eol<char>(ch) )
        {
            if (as_char(ch) && (areg::is_carriage_return<char>(ch) == false) && (areg::is_eos<char>(ch) == false))
            {
                mReadPosition.set_position(-1 * static_cast<int32_t>(sizeof(char)), Cursor::SeekOrigin::Current);
            }

            break;
        }

        Value += ch;
    }

    return result;
}

bool ReadConverter::read_line( WideString & Value ) const
{
    bool result = false;
    wchar_t ch  = static_cast<wchar_t>('\0');
    while (as_char(ch))
    {
        result = true;
        if (areg::is_eol<wchar_t>(ch))
        {
            if ( as_char( ch ) && (areg::is_carriage_return<wchar_t>( ch ) == false) && (areg::is_eos<wchar_t>( ch ) == false) )
            {
                mReadPosition.set_position(-1 * static_cast<int32_t>(sizeof(wchar_t)), Cursor::SeekOrigin::Current);
            }

            break;
        }

        Value += ch;
    }

    return result;
}

} // namespace areg
