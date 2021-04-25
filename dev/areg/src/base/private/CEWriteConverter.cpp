/************************************************************************
 * \file        areg/src/base/private/CEWriteConverter.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/
#include "areg/src/base/private/CEWriteConverter.hpp"

#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEWideString.hpp"
#include "areg/src/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWriteConverter class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEWriteConverter::CEWriteConverter( IEOutStream & writeStream, IECursorPosition & writePosition )
    : mWriteStream  (writeStream)
    , mWritePosition(writePosition)
{
    ; // do nothing
}

CEWriteConverter::~CEWriteConverter( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool CEWriteConverter::SetString( const CEWideString & in_value )
{
    return SetString( in_value.GetBuffer() );
}

bool CEWriteConverter::SetString( const CEString & in_value )
{
    return SetString( in_value.GetBuffer() );
}

bool CEWriteConverter::SetBool( bool in_value )
{
    static const unsigned int size = sizeof(bool);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetChar( char in_value )
{
    static const unsigned int size = sizeof(char);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(& in_value), size) == size);
}

bool CEWriteConverter::SetChar( wchar_t in_value )
{
    static const unsigned int size = sizeof(wchar_t);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetShort( short in_value )
{
    static const unsigned int size = sizeof(short);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetInt( int in_value )
{
    static const unsigned int size = sizeof(int);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetLong( long in_value )
{
    static const unsigned int size = sizeof(long);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetInt64( int64_t in_value )
{
    static const unsigned int size = sizeof(int64_t);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetFloat( float in_value )
{
    static const unsigned int size = sizeof(float);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetDouble( double in_value )
{
    static const unsigned int size = sizeof(double);
    return (mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size);
}

bool CEWriteConverter::SetString( const char * in_value )
{
    const unsigned int size = in_value != NULL ? (NEString::getStringLength<char>(in_value) + 1) * sizeof(char) : 0;
    return (size != 0 ? mWriteStream.Write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}

bool CEWriteConverter::SetString( const wchar_t * in_value )
{
    const unsigned int size = in_value != NULL ? (NEString::getStringLength<wchar_t>(in_value) + 1) * sizeof(wchar_t) : 0;
    return (size != 0 ? mWriteStream.Write(reinterpret_cast<const unsigned char *>(&in_value), size) == size : false);
}

bool CEWriteConverter::AppendString( const char * in_value )
{
    const unsigned int size = in_value != NULL ? NEString::getStringLength<char>(in_value) * sizeof(char) : 0;
    return (size != 0 ? mWriteStream.Write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}

bool CEWriteConverter::AppendString( const wchar_t * in_value )
{
    const unsigned int size = in_value != NULL ? NEString::getStringLength<wchar_t>(in_value) * sizeof(wchar_t) : 0;
    return (size != 0 ? mWriteStream.Write(reinterpret_cast<const unsigned char *>(in_value), size) == size : false);
}
