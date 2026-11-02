/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

#include <cstring>

namespace areg {

namespace {
    /**
     * \brief   Reads exactly sizeof(T) bytes into the value; on a short read restores the cursor
     *          and leaves the value unchanged.
     **/
    template<typename T>
    inline bool _read_value(InStream& stream, Cursor& cursor, T& value) noexcept
    {
        uint8_t bytes[sizeof(T)];
        const uint32_t count{ stream.read(bytes, static_cast<uint32_t>(sizeof(T))) };
        if (count == static_cast<uint32_t>(sizeof(T)))
        {
            std::memcpy(&value, bytes, sizeof(T));
            return true;
        }

        if (count != 0u)
        {
            cursor.set_position(-static_cast<int32_t>(count), Cursor::SeekOrigin::Current);
        }

        return false;
    }

    /**
     * \brief   Reads one character of UTF-16LE text, combining a surrogate pair for a 4-byte wchar_t.
     *          Returns the number of bytes read; 0 at the end of data.
     **/
    inline uint32_t _read_wide(InStream& stream, Cursor& cursor, wchar_t& value) noexcept
    {
        uint16_t unit{ 0u };
        if (!_read_value(stream, cursor, unit))
            return 0u;

        if constexpr (sizeof(wchar_t) == 4u)
        {
            uint16_t low{ 0u };
            if (((unit & 0xFC00u) == 0xD800u) && _read_value(stream, cursor, low))
            {
                if ((low & 0xFC00u) == 0xDC00u)
                {
                    value = static_cast<wchar_t>(0x10000u + ((static_cast<uint32_t>(unit) - 0xD800u) << 10u) + (static_cast<uint32_t>(low) - 0xDC00u));
                    return 4u;
                }

                cursor.set_position(-2, Cursor::SeekOrigin::Current);
            }
        }

        value = static_cast<wchar_t>(unit);
        return 2u;
    }
}

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
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::as_char( char & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::as_char( wchar_t & Value ) const
{
    uint32_t code{ 0u };
    if (!_read_value(mReadStream, mReadPosition, code))
        return false;

    Value = ((sizeof(wchar_t) == 2u) && (code > 0xFFFFu)) ? static_cast<wchar_t>(0xFFFDu) : static_cast<wchar_t>(code);
    return true;
}

bool ReadConverter::as_short( int16_t & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::as_int( int32_t & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::int64( int64_t & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::as_float( float & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
}

bool ReadConverter::as_double( double & Value ) const
{
    return _read_value(mReadStream, mReadPosition, Value);
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

    while ( _read_wide(mReadStream, mReadPosition, ch) != 0u )
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
            char next{ '\0' };
            if (areg::is_carriage_return<char>(ch) && as_char(next) && (next != '\n'))
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
    while (_read_wide(mReadStream, mReadPosition, ch) != 0u)
    {
        result = true;
        if (areg::is_eol<wchar_t>(ch))
        {
            wchar_t next{ L'\0' };
            const uint32_t size{ areg::is_carriage_return<wchar_t>(ch) ? _read_wide(mReadStream, mReadPosition, next) : 0u };
            if ((size != 0u) && (next != L'\n'))
            {
                mReadPosition.set_position(-1 * static_cast<int32_t>(size), Cursor::SeekOrigin::Current);
            }

            break;
        }

        Value += ch;
    }

    return result;
}

} // namespace areg
