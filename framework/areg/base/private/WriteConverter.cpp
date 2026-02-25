/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/WriteConverter.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/
#include "areg/base/private/WriteConverter.hpp"

#include "areg/base/IOStream.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// WriteConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
WriteConverter::WriteConverter( OutStream & write_stream, Cursor & /*writePosition*/ )
    : mWriteStream  (write_stream)
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool WriteConverter::set_string( const WideString & in_value )
{
    return set_string( in_value.as_string() );
}

bool WriteConverter::set_string( const String & in_value )
{
    return set_string( in_value.as_string() );
}

bool WriteConverter::set_bool( bool in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(bool));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_char( char in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(char));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(& in_value), size) == size);
}

bool WriteConverter::set_char( wchar_t in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(wchar_t));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_short( int16_t in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(int16_t));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_int( int32_t in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(int32_t));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_int64( int64_t in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(int64_t));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_float( float in_value )
{
    static constexpr uint32_t size = static_cast<uint32_t>(sizeof(float));
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_double( double in_value )
{
    static constexpr uint32_t size = sizeof(double);
    return (mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size);
}

bool WriteConverter::set_string( const char * in_value )
{
    const uint32_t size = in_value != nullptr ? (static_cast<uint32_t>(NEString::string_length<char>(in_value)) + 1u) * sizeof(char) : 0u;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const uint8_t *>(in_value), size) == size : false);
}

bool WriteConverter::set_string( const wchar_t * in_value )
{
    const uint32_t size = in_value != nullptr ? (static_cast<uint32_t>(NEString::string_length<wchar_t>(in_value)) + 1u) * sizeof(wchar_t) : 0u;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const uint8_t *>(&in_value), size) == size : false);
}

bool WriteConverter::append_string( const char * in_value )
{
    const uint32_t size = in_value != nullptr ? static_cast<uint32_t>(NEString::string_length<char>(in_value)) * sizeof(char) : 0u;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const uint8_t *>(in_value), size) == size : false);
}

bool WriteConverter::append_string( const wchar_t * in_value )
{
    const uint32_t size = in_value != nullptr ? static_cast<uint32_t>(NEString::string_length<wchar_t>(in_value)) * sizeof(wchar_t) : 0u;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const uint8_t *>(in_value), size) == size : false);
}
