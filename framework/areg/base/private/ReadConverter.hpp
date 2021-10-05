#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ReadConverter.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Read converter helper object
 *              Reads data from readable streaming object and converts
 *              data to basic types.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;
class WideString;
class IEInStream;
class IECursorPosition;

//////////////////////////////////////////////////////////////////////////
// ReadConverter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This helper class is reading data from a stream and converts
 *          to basic types.
 **/
class AREG_API ReadConverter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          Initialize converting helper class and holds
     *          references of streaming buffer and its position.
     * \param   readStream      The readable streaming object
     * \param   readPosition    The position object of readable stream.
     **/
    ReadConverter( IEInStream & readStream, IECursorPosition & readPosition );

    /**
     * \brief   Destructor.
     **/
    ~ReadConverter( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads data from streaming object and converts to boolean.
     *          On output, the out_value contains boolean value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted boolean value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getBool( bool & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to character.
     *          On output, the out_value contains character value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted character value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getChar( char & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to wide-character.
     *          On output, the out_value contains wide-character value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted wide-character value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getChar( wchar_t & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to short integer.
     *          On output, the out_value contains short integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted short integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getShort(short & OUT out_value) const;

    /**
     * \brief   Reads data from streaming object and converts to integer.
     *          On output, the out_value contains integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getInt( int & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to 64-bit integer.
     *          On output, the out_value contains 64-bit integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted 64-bit integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getInt64( int64_t & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to number with floating point.
     *          On output, the out_value contains number with floating point value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted number with floating point value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getFloat( float & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to number with floating point.
     *          On output, the out_value contains number with floating point value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted number with floating point value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getDouble( double & OUT out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to null-terminated ASCII string.
     *          On output, the out_value contains null-terminated ASCII string value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted null-terminated ASCII string value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getString(String & OUT out_value) const;

    /**
     * \brief   Reads data from streaming object and converts to null-terminated wide-char string.
     *          On output, the out_value contains null-terminated wide-char string value.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains converted null-terminated wide-char string value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool getString(WideString & OUT out_value) const;

    /**
     * \brief   Reads from streaming object single line of null-terminated ASCII string.
     *          On output, the out_value contains single line of null-terminated ASCII string value.
     *          The new line indicator character is not included in the string.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains single line of converted null-terminated ASCII string value.
     *                      The new line indicator character is not included in the string.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool readLine(String & OUT out_value) const;

    /**
     * \brief   Reads from streaming object single line of null-terminated wide-char string.
     *          On output, the out_value contains single line of null-terminated wide-char string value.
     *          The new line indicator character is not included in the string.
     *          Function returns true if reading succeeded.
     * \param   out_value[out]  On output, this parameter contains single line of converted null-terminated wide-char string value.
     *                      The new line indicator character is not included in the string.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool readLine(WideString & OUT out_value) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference of readable streaming object
     **/
    IEInStream  &      mReadStream;
    /**
     * \brief   Reference of cursor position object in readable stream.
     **/
    IECursorPosition & mReadPosition;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ReadConverter( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ReadConverter );
};
