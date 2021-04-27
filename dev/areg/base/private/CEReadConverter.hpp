#ifndef AREG_BASE_PRIVATE_CEREADCONVERTER_HPP
#define AREG_BASE_PRIVATE_CEREADCONVERTER_HPP
/************************************************************************
 * \file        areg/base/private/CEReadConverter.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
class CEString;
class CEWideString;
class IEInStream;
class IECursorPosition;

//////////////////////////////////////////////////////////////////////////
// CEReadConverter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This helper class is reading data from a stream and converts
 *          to basic types.
 **/
class AREG_API CEReadConverter
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
    CEReadConverter( IEInStream & readStream, IECursorPosition & readPosition );

    /**
     * \brief   Destructor.
     **/
    ~CEReadConverter( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads data from streaming object and converts to boolean.
     *          On output, the out_value contains boolean value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted boolean value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetBool( bool & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to character.
     *          On output, the out_value contains character value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted character value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetChar( char & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to wide-character.
     *          On output, the out_value contains wide-character value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted wide-character value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetChar( wchar_t & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to short integer.
     *          On output, the out_value contains short integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted short integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetShort(short & out_value) const;

    /**
     * \brief   Reads data from streaming object and converts to integer.
     *          On output, the out_value contains integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetInt( int & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to long integer.
     *          On output, the out_value contains long integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted long integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetLong( long & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to 64-bit integer.
     *          On output, the out_value contains 64-bit integer value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted 64-bit integer value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetInt64( int64_t & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to number with floating point.
     *          On output, the out_value contains number with floating point value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted number with floating point value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetFloat( float & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to number with floating point.
     *          On output, the out_value contains number with floating point value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted number with floating point value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetDouble( double & out_value ) const;

    /**
     * \brief   Reads data from streaming object and converts to null-terminated ASCII string.
     *          On output, the out_value contains null-terminated ASCII string value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted null-terminated ASCII string value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetString(CEString & out_value) const;

    /**
     * \brief   Reads data from streaming object and converts to null-terminated wide-char string.
     *          On output, the out_value contains null-terminated wide-char string value.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains converted null-terminated wide-char string value.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool GetString(CEWideString & out_value) const;

    /**
     * \brief   Reads from streaming object single line of null-terminated ASCII string.
     *          On output, the out_value contains single line of null-terminated ASCII string value.
     *          The new line indicator character is not included in the string.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains single line of converted null-terminated ASCII string value.
     *                      The new line indicator character is not included in the string.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool ReadLine(CEString & out_value) const;

    /**
     * \brief   Reads from streaming object single line of null-terminated wide-char string.
     *          On output, the out_value contains single line of null-terminated wide-char string value.
     *          The new line indicator character is not included in the string.
     *          Function returns true if reading succeeded.
     * \param   out_value   On output, this parameter contains single line of converted null-terminated wide-char string value.
     *                      The new line indicator character is not included in the string.
     * \return  If reading succeeded, function returns true and the output parameter is valid.
     *          Otherwise, the value of output parameter should be ignored.
     **/
    bool ReadLine(CEWideString & out_value) const;

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
    CEReadConverter( void );
    CEReadConverter( const CEReadConverter & /*src*/ );
    const CEReadConverter & operator = ( const CEReadConverter & /*src*/ );
};

#endif  // AREG_BASE_PRIVATE_CEREADCONVERTER_HPP
