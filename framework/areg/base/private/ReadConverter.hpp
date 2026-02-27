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
#include "areg/base/GEGlobal.h"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class String; }
namespace areg { class WideString; }
namespace areg { class InStream; }
namespace areg { class Cursor; }

namespace areg
{
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
        ReadConverter( areg::InStream & readStream, areg::Cursor & readPosition );

        /**
         * \brief   Destructor.
         **/
        ~ReadConverter() = default;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Reads data from streaming object and converts to boolean.
         *          On output, the Value contains boolean value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted boolean value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getBool( bool & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to character.
         *          On output, the Value contains character value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted character value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getChar( char & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to wide-character.
         *          On output, the Value contains wide-character value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted wide-character value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getChar( wchar_t & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to int16_t integer.
         *          On output, the Value contains int16_t integer value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted int16_t integer value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getShort(int16_t & Value) const;

        /**
         * \brief   Reads data from streaming object and converts to integer.
         *          On output, the Value contains integer value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted integer value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getInt( int32_t & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to 64-bit integer.
         *          On output, the Value contains 64-bit integer value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted 64-bit integer value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getInt64( int64_t & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to number with floating point.
         *          On output, the Value contains number with floating point value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted number with floating point value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getFloat( float & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to number with floating point.
         *          On output, the Value contains number with floating point value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted number with floating point value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getDouble( double & Value ) const;

        /**
         * \brief   Reads data from streaming object and converts to null-terminated ASCII string.
         *          On output, the Value contains null-terminated ASCII string value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted null-terminated ASCII string value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getString(areg::String & Value) const;

        /**
         * \brief   Reads data from streaming object and converts to null-terminated wide-char string.
         *          On output, the Value contains null-terminated wide-char string value.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains converted null-terminated wide-char string value.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool getString(areg::WideString & Value) const;

        /**
         * \brief   Reads from streaming object single line of null-terminated ASCII string.
         *          On output, the Value contains single line of null-terminated ASCII string value.
         *          The new line indicator character is not included in the string.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains single line of converted null-terminated ASCII string value.
         *                      The new line indicator character is not included in the string.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool readLine(areg::String & Value) const;

        /**
         * \brief   Reads from streaming object single line of null-terminated wide-char string.
         *          On output, the Value contains single line of null-terminated wide-char string value.
         *          The new line indicator character is not included in the string.
         *          Function returns true if reading succeeded.
         * \param[out]  Value   On output, this parameter contains single line of converted null-terminated wide-char string value.
         *                          The new line indicator character is not included in the string.
         * \return  If reading succeeded, function returns true and the output parameter is valid.
         *          Otherwise, the value of output parameter should be ignored.
         **/
        bool readLine(areg::WideString & Value) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Reference of readable streaming object
         **/
        areg::InStream  &      mReadStream;
        /**
         * \brief   Reference of cursor position object in readable stream.
         **/
        areg::Cursor & mReadPosition;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
        ReadConverter() = delete;
        AREG_NOCOPY_NOMOVE( ReadConverter );
    };

} // namespace areg
#endif  // AREG_BASE_PRIVATE_READCONVERTER_HPP
