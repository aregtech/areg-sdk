#ifndef AREG_BASE_PRIVATE_READCONVERTER_HPP
#define AREG_BASE_PRIVATE_READCONVERTER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/ReadConverter.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Read converter helper object
 *              Reads data from readable streaming object and converts
 *              data to basic types.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class String;
class WideString;
class InStream;
class Cursor;

//////////////////////////////////////////////////////////////////////////
// ReadConverter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class for reading data from a stream and converting to basic types.
 **/
class AREG_API ReadConverter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the converter with a readable stream and position cursor.
     *
     * \param   read_stream     The readable stream to read from.
     * \param   readPosition    The position cursor in the readable stream.
     **/
    ReadConverter( InStream & read_stream, Cursor & readPosition );

    /**
     * \brief   Destructor.
     **/
    ~ReadConverter() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads and converts stream data to a boolean value.
     *
     * \param[out] Value       On return, contains the converted boolean value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_bool( bool & Value ) const;

    /**
     * \brief   Reads and converts stream data to a character value.
     *
     * \param[out] Value       On return, contains the converted character value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_char( char & Value ) const;

    /**
     * \brief   Reads and converts stream data to a wide-character value.
     *
     * \param[out] Value       On return, contains the converted wide-character value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_char( wchar_t & Value ) const;

    /**
     * \brief   Reads and converts stream data to a 16-bit signed integer value.
     *
     * \param[out] Value       On return, contains the converted 16-bit signed integer value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_short(int16_t & Value) const;

    /**
     * \brief   Reads and converts stream data to a 32-bit signed integer value.
     *
     * \param[out] Value       On return, contains the converted 32-bit signed integer value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_int( int32_t & Value ) const;

    /**
     * \brief   Reads and converts stream data to a 64-bit signed integer value.
     *
     * \param[out] Value       On return, contains the converted 64-bit signed integer value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool int64( int64_t & Value ) const;

    /**
     * \brief   Reads and converts stream data to a floating-point value.
     *
     * \param[out] Value       On return, contains the converted floating-point value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_float( float & Value ) const;

    /**
     * \brief   Reads and converts stream data to a double-precision floating-point value.
     *
     * \param[out] Value       On return, contains the converted double-precision floating-point
     *                         value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_double( double & Value ) const;

    /**
     * \brief   Reads and converts stream data to an ASCII string value.
     *
     * \param[out] Value       On return, contains the converted ASCII string value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_string(String & Value) const;

    /**
     * \brief   Reads and converts stream data to a wide-character string value.
     *
     * \param[out] Value       On return, contains the converted wide-character string value.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool as_string(WideString & Value) const;

    /**
     * \brief   Reads a single line from the stream as an ASCII string, excluding the newline
     *          character.
     *
     * \param[out] Value       On return, contains a single line of ASCII string without the newline
     *                         character.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool read_line(String & Value) const;

    /**
     * \brief   Reads a single line from the stream as a wide-character string, excluding the
     *          newline character.
     *
     * \param[out] Value       On return, contains a single line of wide-character string without
     *                         the newline character.
     * \return  Returns true if reading succeeded; false otherwise.
     **/
    bool read_line(WideString & Value) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference of readable streaming object
     **/
    InStream  &      mReadStream;
    /**
     * \brief   Reference of cursor position object in readable stream.
     **/
    Cursor & mReadPosition;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    ReadConverter() = delete;
    AREG_NOCOPY_NOMOVE( ReadConverter );
};

} // namespace areg
#endif  // AREG_BASE_PRIVATE_READCONVERTER_HPP
