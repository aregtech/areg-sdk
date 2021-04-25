#ifndef AREG_BASE_PRIVATE_CEWRITECONVERTER_HPP
#define AREG_BASE_PRIVATE_CEWRITECONVERTER_HPP
/************************************************************************
 * \file        areg/src/base/private/CEWriteConverter.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Write converter helper object
 *              Writes data to writable streaming object by converting
 *              basic types.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEOutStream;
class IECursorPosition;
class CEString;
class CEWideString;

//////////////////////////////////////////////////////////////////////////
// CEWriteConverter class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   This helper class is writing data to a stream by converting
 *          basic types.
 **/
class AREG_API CEWriteConverter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initialization constructor.
     *          Initialize converting helper class and hold
     *          references of streaming buffer and its position.
     * \param   writeStream     The writable streaming object
     * \param   writePosition   The position object of writable stream.
     **/
    CEWriteConverter( IEOutStream & writeStream, IECursorPosition & writePosition );

    /**
     * \brief   Destructor.
     **/
    ~CEWriteConverter( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Writes boolean value to the writable streaming object
     * \param   in_value    The boolean value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetBool( bool in_value );

    /**
     * \brief   Writes 8-bit character value to the writable streaming object
     * \param   in_value    The 8-bit character value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetChar( char in_value );

    /**
     * \brief   Writes 16-bit wide-character value to the writable streaming object
     * \param   in_value    The 16-bit wide-character value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetChar( wchar_t in_value );

    /**
     * \brief   Writes short integer value to the writable streaming object
     * \param   in_value    The short integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetShort( short in_value );

    /**
     * \brief   Writes integer value to the writable streaming object
     * \param   in_value    The integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetInt( int in_value );

    /**
     * \brief   Writes long integer value to the writable streaming object
     * \param   in_value    The long integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetLong( long in_value );

    /**
     * \brief   Writes 64-bit integer value to the writable streaming object
     * \param   in_value    The 64-bit integer value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetInt64( int64_t in_value );

    /**
     * \brief   Writes number with floating point value to the writable streaming object
     * \param   in_value    The number with floating point value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetFloat( float in_value );

    /**
     * \brief   Writes number with floating point value to the writable streaming object
     * \param   in_value    The number with floating point value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetDouble( double in_value );

    /**
     * \brief   Writes ASCII string value to the writable streaming object
     * \param   in_value    The ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetString( const CEString & in_value );
 
    /**
     * \brief   Writes null-terminated ASCII string value to the writable streaming object
     * \param   in_value    The null-terminated ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetString( const char * in_value );

    /**
     * \brief   Writes wide-char string value to the writable streaming object
     * \param   in_value    The wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetString( const CEWideString & in_value );

    /**
     * \brief   Writes null-terminated wide-char string value to the writable streaming object
     * \param   in_value    The null-terminated wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool SetString( const wchar_t * in_value );

    /**
     * \brief   Appends null-terminated ASCII string to the writable streaming object.
     *          Unlike SetString() function, no end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'false', to set ASCII end of string value
     *          when operation is completed.
     * \param   in_value    The null-terminated ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool AppendString( const char * in_value );

    /**
     * \brief   Appends null-terminated wide-char (unicode) string to the writable streaming object.
     *          Unlike SetString() function, no end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'true', to set wide-char (unicode) end of string value
     *          when operation is completed.
     * \param   in_value    The null-terminated wide-char (unicode) string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool AppendString( const wchar_t * in_value );

    /**
     * \brief   Writes null-terminated ASCII string value to the writable streaming object as a single line.
     *          The new line symbol automatically will be added to the end.
     *          No end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'false', to set ASCII end of string value
     *          when operation is completed.
     * \param   in_value    The ASCII string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool WriteLine( const char * in_value );

    /**
     * \brief   Adds null-terminated wide-char string value to the writable streaming object as a single line.
     *          The new line symbol automatically will be added to the end.
     *          No end of string value will be added to the end.
     *          Call SetEndOfString with parameter 'true', to set wide-char (Unicode) end of string value
     *          when operation is completed.
     * \param   in_value    The null-terminated wide-char string value to write to writable streaming object
     * \return  Returns true if operation succeeded
     **/
    bool WriteLine( const wchar_t* in_value );

    /**
     * \brief   Sets end of string (null termination) at the current position of writable buffer 
     *          to indicate end of string value
     * \param   isWideChar  Flag, indicating whether ASCII or wide-char (Unicode) character should be added.
     * \return  Returns true if operation succeeded
     **/
    bool SetEndOfString( bool isWideChar );

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
    CEWriteConverter( void );
    CEWriteConverter( const CEWriteConverter & /*src*/ );
    const CEWriteConverter & operator = ( const CEWriteConverter & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEWriteConverter class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline bool CEWriteConverter::WriteLine( const char * in_value )
{   return (AppendString(in_value) && SetChar(static_cast<char>('\n')));    }

inline bool CEWriteConverter::WriteLine( const wchar_t * in_value )
{   return (AppendString(in_value) && SetChar(static_cast<wchar_t>('\n'))); }

inline bool CEWriteConverter::SetEndOfString( bool isWideChar )
{   return ( isWideChar ? SetChar(static_cast<wchar_t>('\0')) : SetChar(static_cast<char>('\0')) ); }

#endif  // AREG_BASE_PRIVATE_CEWRITECONVERTER_HPP
