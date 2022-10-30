#ifndef AREG_BASE_PRIVATE_WRITECONVERTER_HPP
#define AREG_BASE_PRIVATE_WRITECONVERTER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/WriteConverter.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEOutStream;
class IECursorPosition;
class String;
class WideString;

//////////////////////////////////////////////////////////////////////////
// WriteConverter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This helper class is writing data to a stream by converting basic types.
 **/
class AREG_API WriteConverter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialize converting helper class and hold references of streaming buffer and its position.
     * \param   writeStream     The writable streaming object
     * \param   writePosition   The position object of writable stream.
     **/
    WriteConverter( IEOutStream & writeStream, IECursorPosition & writePosition );

    /**
     * \brief   Destructor.
     **/
    ~WriteConverter( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Writes boolean value to the writable streaming object
     * \param   in_value[in]    The boolean value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setBool( bool in_value );

    /**
     * \brief   Writes 8-bit character value to the writable streaming object
     * \param   in_value[in]    The 8-bit character value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setChar( char in_value );

    /**
     * \brief   Writes 16-bit wide-character value to the writable streaming object
     * \param   in_value[in]    The 16-bit wide-character value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setChar( wchar_t in_value );

    /**
     * \brief   Writes short integer value to the writable streaming object
     * \param   in_value[in]    The short integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setShort( short in_value );

    /**
     * \brief   Writes integer value to the writable streaming object
     * \param   in_value[in]    The integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setInt( int in_value );

    /**
     * \brief   Writes 64-bit integer value to the writable streaming object
     * \param   in_value[in]    The 64-bit integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setInt64( int64_t in_value );

    /**
     * \brief   Writes number with floating point value to the writable streaming object
     * \param   in_value[in]    The number with floating point value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setFloat( float in_value );

    /**
     * \brief   Writes number with floating point value to the writable streaming object
     * \param   in_value[in]    The number with floating point value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setDouble( double in_value );

    /**
     * \brief   Writes ASCII string value to the writable streaming object
     * \param   in_value[in]    The ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setString( const String & in_value );
 
    /**
     * \brief   Writes null-terminated ASCII string value to the writable streaming object
     * \param   in_value[in]    The null-terminated ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setString( const char * in_value );

    /**
     * \brief   Writes wide-char string value to the writable streaming object
     * \param   in_value[in]    The wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setString( const WideString & in_value );

    /**
     * \brief   Writes null-terminated wide-char string value to the writable streaming object
     * \param   in_value[in]    The null-terminated wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool setString( const wchar_t * in_value );

    /**
     * \brief   Appends null-terminated ASCII string to the writable streaming object.
     *          Unlike SetString() function, no end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'false', to set ASCII end of string value
     *          when operation is completed.
     * \param   in_value[in]    The null-terminated ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool appendString( const char * in_value );

    /**
     * \brief   Appends null-terminated wide-char (Unicode) string to the writable streaming object.
     *          Unlike SetString() function, no end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'true', to set wide-char (Unicode) end of string value
     *          when operation is completed.
     * \param   in_value[in]    The null-terminated wide-char (Unicode) string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool appendString( const wchar_t * in_value );

    /**
     * \brief   Writes null-terminated ASCII string value to the writable streaming object as a single line.
     *          The new line symbol automatically will be added to the end.
     *          No end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'false', to set ASCII end of string value
     *          when operation is completed.
     * \param   in_value[in]    The ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool writeLine( const char * in_value );

    /**
     * \brief   Adds null-terminated wide-char string value to the writable streaming object as a single line.
     *          The new line symbol automatically will be added to the end.
     *          No end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'true', to set wide-char (Unicode) end of string value
     *          when operation is completed.
     * \param   in_value[in]    The null-terminated wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool writeLine( const wchar_t* in_value );

    /**
     * \brief   Sets end of string (null-termination) at the current position of writable buffer 
     *          to indicate end of string value
     * \param   isWideChar  Flag, indicating whether ASCII or wide-char (Unicode) character should be added.
     * \return  Returns true if operation succeeded
     **/
    bool setEndOfString( bool isWideChar );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference of a writable streaming object
     **/
    IEOutStream &      mWriteStream;
    /**
     * \brief   Reference of cursor position object in writable stream.
     **/
    IECursorPosition & mWritePosition;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    WriteConverter( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( WriteConverter );
};

//////////////////////////////////////////////////////////////////////////
// WriteConverter class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool WriteConverter::writeLine( const char * in_value )
{
    return (appendString(in_value) && setChar(static_cast<char>('\n')));
}

inline bool WriteConverter::writeLine( const wchar_t * in_value )
{
    return (appendString(in_value) && setChar(static_cast<wchar_t>('\n')));
}

inline bool WriteConverter::setEndOfString( bool isWideChar )
{
    return ( isWideChar ? setChar(static_cast<wchar_t>('\0')) : setChar(static_cast<char>('\0')) );
}

#endif  // AREG_BASE_PRIVATE_WRITECONVERTER_HPP
