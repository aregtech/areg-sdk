#ifndef AREG_BASE_PRIVATE_WRITECONVERTER_HPP
#define AREG_BASE_PRIVATE_WRITECONVERTER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/WriteConverter.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/StringDefs.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class OutStream;
class Cursor;
class String;
class WideString;

//////////////////////////////////////////////////////////////////////////
// WriteConverter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class for writing data to a stream by converting basic types to their binary
 *          representation.
 **/
class AREG_API WriteConverter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the converter with references to the output stream and write position.
     *
     * \param   write_stream        The output stream to write to.
     * \param   writePosition       The write position cursor for the stream.
     **/
    WriteConverter( OutStream & write_stream, Cursor & writePosition );

    ~WriteConverter() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Writes a boolean value to the output stream.
     *
     * \param   in_value    The boolean value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_bool( bool in_value );

    /**
     * \brief   Writes an 8-bit character value to the output stream.
     *
     * \param   in_value    The 8-bit character value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_char( char in_value );

    /**
     * \brief   Writes a 16-bit wide-character value to the output stream.
     *
     * \param   in_value    The 16-bit wide-character value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_char( wchar_t in_value );

    /**
     * \brief   Writes a 16-bit signed integer value to the output stream.
     *
     * \param   in_value    The int16_t value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_short( int16_t in_value );

    /**
     * \brief   Writes a 32-bit signed integer value to the output stream.
     *
     * \param   in_value    The int32_t value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_int( int32_t in_value );

    /**
     * \brief   Writes a 64-bit signed integer value to the output stream.
     *
     * \param   in_value    The int64_t value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_int64( int64_t in_value );

    /**
     * \brief   Writes a floating-point value to the output stream.
     *
     * \param   in_value    The floating-point value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_float( float in_value );

    /**
     * \brief   Writes a double-precision floating-point value to the output stream.
     *
     * \param   in_value    The double value to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_double( double in_value );

    /**
     * \brief   Writes an ASCII string value to the output stream.
     *
     * \param   in_value    The String object to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_string( const String & in_value );
 
    /**
     * \brief   Writes a null-terminated ASCII string to the output stream.
     *
     * \param   in_value    The null-terminated ASCII string to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_string( const char * in_value );

    /**
     * \brief   Writes a wide-character string value to the output stream.
     *
     * \param   in_value    The WideString object to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_string( const WideString & in_value );

    /**
     * \brief   Writes a null-terminated wide-character string to the output stream.
     *
     * \param   in_value    The null-terminated wide-character string to write.
     * \return  Returns true if the write succeeded.
     **/
    bool set_string( const wchar_t * in_value );

    /**
     * \brief   Appends a null-terminated ASCII string to the stream without adding a string
     *          terminator.
     *
     * \param   in_value    The null-terminated ASCII string to append.
     * \return  Returns true if the operation succeeded.
     **/
    bool append_string( const char * in_value );

    /**
     * \brief   Appends a null-terminated wide-character (Unicode) string to the stream without
     *          adding a string terminator.
     *
     * \param   in_value    The null-terminated wide-character string to append.
     * \return  Returns true if the operation succeeded.
     **/
    bool append_string( const wchar_t * in_value );

    /**
     * \brief   Writes a null-terminated ASCII string as a line with automatic newline terminator.
     *
     * \param   in_value    The null-terminated ASCII string to write.
     * \return  Returns true if the operation succeeded.
     **/
    bool write_line( const char * in_value );

    /**
     * \brief   Writes a null-terminated wide-character string as a line with automatic newline terminator.
     *
     * \param   in_value    The null-terminated wide-character string to write.
     * \return  Returns true if the operation succeeded.
     **/
    bool write_line( const wchar_t* in_value );

    /**
     * \brief   Writes a null-terminator at the current position to indicate end of string.
     *
     * \param   isWideChar      If true, writes wide-character (Unicode) null-terminator;
     *                          otherwise writes ASCII.
     * \return  Returns true if the operation succeeded.
     **/
    bool set_string_end( bool isWideChar );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    OutStream &      mWriteStream;    //!< Reference of a writable streaming object

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    WriteConverter() = delete;
    AREG_NOCOPY_NOMOVE( WriteConverter );
};

//////////////////////////////////////////////////////////////////////////
// WriteConverter class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool WriteConverter::write_line( const char * in_value )
{
    return (append_string(in_value) && set_char(static_cast<char>(areg::EndOfLine)));
}

inline bool WriteConverter::write_line( const wchar_t * in_value )
{
    return (append_string(in_value) && set_char(static_cast<wchar_t>(areg::EndOfLine)));
}

inline bool WriteConverter::set_string_end( bool isWideChar )
{
    return ( isWideChar ? set_char(static_cast<wchar_t>('\0')) : set_char(static_cast<char>('\0')) );
}

} // namespace areg
#endif  // AREG_BASE_PRIVATE_WRITECONVERTER_HPP
