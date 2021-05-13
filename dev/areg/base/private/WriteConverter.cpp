/************************************************************************
 * \file        areg/base/private/WriteConverter.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/
#include "areg/base/private/WriteConverter.hpp"

#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// WriteConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
WriteConverter::WriteConverter( IEOutStream & writeStream, IECursorPosition & writePosition )
    : mWriteStream  (writeStream)
    , mWritePosition(writePosition)
{
    ; // do nothing
}

WriteConverter::~WriteConverter( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool WriteConverter::setString( const WideString & in_value )
{
    return setString( in_value.getString() );
}

bool WriteConverter::setString( const String & in_value )
{
    return setString( in_value.getString() );
}

bool WriteConverter::setBool( bool in_value )
{
    static const unsigned int size = sizeof(bool);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setChar( char in_value )
{
    static const unsigned int size = sizeof(char);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(& in_value), size) == size);
}

bool WriteConverter::setChar( wchar_t in_value )
{
    static const unsigned int size = sizeof(wchar_t);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setShort( short in_value )
{
    static const unsigned int size = sizeof(short);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setInt( int in_value )
{
    static const unsigned int size = sizeof(int);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setLong( long in_value )
{
    static const unsigned int size = sizeof(long);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setInt64( int64_t in_value )
{
    static const unsigned int size = sizeof(int64_t);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setFloat( float in_value )
{
    static const unsigned int size = sizeof(float);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setDouble( double in_value )
{
    static const unsigned int size = sizeof(double);
    return (mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool WriteConverter::setString( const char * in_value )
{
    const unsigned int size = in_value != NULL ? (NEString::getStringLength<char>(in_value) + 1) * sizeof(char) : 0;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}

bool WriteConverter::setString( const wchar_t * in_value )
{
    const unsigned int size = in_value != NULL ? (NEString::getStringLength<wchar_t>(in_value) + 1) * sizeof(wchar_t) : 0;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const unsigned char *>(&in_value), size) == size : false);
}

bool WriteConverter::appendString( const char * in_value )
{
    const unsigned int size = in_value != NULL ? NEString::getStringLength<char>(in_value) * sizeof(char) : 0;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}

bool WriteConverter::appendString( const wchar_t * in_value )
{
    const unsigned int size = in_value != NULL ? NEString::getStringLength<wchar_t>(in_value) * sizeof(wchar_t) : 0;
    return (size != 0 ? mWriteStream.write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}
