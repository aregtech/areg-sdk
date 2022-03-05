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
 * \file        areg/base/DateTime.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Date and Time class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"

#include <string>
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
struct tm;
class IEInStream;
class IEOutStream;
//////////////////////////////////////////////////////////////////////////
// DateTime class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Date and Time class. It saves date and time value as 
 *          64-bit value and makes basic operations with date and time value.
 **/
class AREG_API DateTime
{
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

public:
/************************************************************************/
// Public constants
/************************************************************************/

    /**
     * \brief   ISO8601 format of timestamp to display logs
     **/
    static constexpr std::string_view   TIME_FORMAT_ISO8601_OUTPUT          { "%Y-%m-%d %H:%M:%S,%l" };

    /**
     * \brief   Absolute time format of timestamp
     **/
    static constexpr std::string_view   TIME_FORMAT_ABSOLUTE_OUTPUT         { "%H:%M:%S,%l" };

    /**
     * \brief   Format only data of timestamp
     **/
    static constexpr std::string_view   TIME_FORMAT_DATE_OUTPUT             { "%d %b %Y %H:%M:%S,%l" };

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets zero value.
     **/
    DateTime( void );

    /**
     * \brief   Sets date and time value in microseconds passed since January 1 1970.
     * \param   dateTime    The time in microseconds passed since January 1 1970.
     **/
    DateTime( const TIME64 & dateTime );

    /**
     * \brief   Sets date and time value from given system time structure.
     * \param   sysTime     System time structure to get date and time values.
     **/
    explicit DateTime( const NEUtilities::sSystemTime & sysTime );

    /**
     * \brief   Sets date and time value from given file time structure.
     * \param   fileTime    File time structure to get date and time values.
     **/
    explicit DateTime( const NEUtilities::sFileTime & fileTime );

    /**
     * \brief   Copies data and time data from given source.
     * \param   src     The source to copy data.
     **/
    DateTime( const DateTime & dateTime );

    /**
     * \brief   Moves data and time data from given source.
     * \param   src     The source to move data.
     **/
    DateTime( DateTime && dateTime ) noexcept;

    /**
     * \brief   Initializes data from streaming object
     **/
    DateTime( const IEInStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~DateTime(void) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts and returns date and time value as 64-bit unsigned integer value
     **/
    inline operator TIME64 ( void ) const;

    /**
     * \brief   Sets date and time value from given source.
     * \param   src     The source of date and time value.
     **/
    inline DateTime & operator = ( const DateTime & src );

    /**
     * \brief   Move operator.
     * \param   src     The source of date and time value.
     **/
    inline DateTime & operator = ( DateTime && src ) noexcept;

    /**
     * \brief   Returns true, if 2 date and time objects are equal. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if 2 date and time objects are equal.
     **/
    bool operator == ( const DateTime & other ) const;

    /**
     * \brief   Returns true, if 2 date and time objects are not equal. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if 2 date and time objects are not equal.
     **/
    bool operator != ( const DateTime & other ) const;

    /**
     * \brief   Returns true, if one date and time object is greater than other. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if date and time value is greater than other.
     **/
    bool operator > ( const DateTime & other ) const;

    /**
     * \brief   Returns true, if one date and time object is smaller than other. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if date and time value is smaller than other.
     **/
    bool operator < ( const DateTime & other ) const;

    /**
     * \brief   Returns true, if one date and time object is greater or equal than other. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if date and time value is greater or equal than other.
     **/
    bool operator >= ( const DateTime & other ) const;

    /**
     * \brief   Returns true, if one date and time object is smaller or equal than other. Otherwise, returns false.
     * \param   other   The date and time object to compare.
     * \return  Returns true if date and time value is smaller or equal than other.
     **/
    bool operator <= ( const DateTime & other ) const;

    /**
     * \brief   Reads (de-serializes) date and time value from streaming object.
     * \param   stream  Streaming object, which contains serialized date and time value
     * \param   input   Date and time object, which is initialized by de-serializing 
     *                  date and time value from stream
     **/
    friend AREG_API const IEInStream & operator >> ( const IEInStream & stream, DateTime & input );

    /**
     * \brief   Writes (serializes) date and time value to streaming object.
     * \param   stream  Streaming object to serialized date and time value
     * \param   input   Date and time object, which is contains date and time value and
     *                  should be serialized to streaming object
     **/
    friend AREG_API IEOutStream & operator << ( IEOutStream & stream, const DateTime & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Static operations
/************************************************************************/
    /**
     * \brief   Retrieves and returns current system time in UTC (Coordinated Universal Time).
     * \return  Returns current date and time value.
     **/
    static DateTime getNow( void );

    /**
     * \brief   Retrieves and returns current local time.
     * \param   timeData    On output, it will contain the time values.
     * \param   localTime   If true, timeData is converted to local time.
     * \return  Returns current date and time value.
     **/
    static void getNow( NEUtilities::sSystemTime & OUT timeData, bool localTime );

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed since the system was started.
     **/
    static uint64_t getSystemTickCount( void );

    /**
     * \brief   Retrieves the number of milliseconds that have elapsed since current process has been started.
     **/
    static uint64_t getProcessTickCount( void );

/************************************************************************/
// Non-static operations
/************************************************************************/
    /**
     * \brief   Converts existing date and time value to system time structure
     * \param   out_sysTime The System Time structure to feel data
     **/
    void convToSystemTime( NEUtilities::sSystemTime & out_sysTime ) const;

    /**
     * \brief   Retrieves date and time value from system time structure and sets value
     * \param   sysTime     The System Time structure as a source of data.
     **/
    void convFromSystemTime( const NEUtilities::sSystemTime & sysTime );

    /**
     * \brief   Formats time and outputs as a string. The caller should specify the
     *          the time format name for output
     **/
    std::string formatTime( const std::string_view & formatName = DateTime::TIME_FORMAT_ISO8601_OUTPUT ) const;

    /**
     * \brief   Returns the time data.
     **/
    inline const TIME64 & getTime( void ) const;

    /**
     * \brief   Returns true, if time value is not zero
     **/
    inline bool isValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Date and time value
     **/
    TIME64  mDateTime;
};

//////////////////////////////////////////////////////////////////////////
// DateTime class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DateTime::operator TIME64 ( void ) const
{
    return mDateTime;
}

inline DateTime & DateTime::operator = ( const DateTime & src )
{
    mDateTime = src.mDateTime;
    return (*this);
}

inline DateTime & DateTime::operator = ( DateTime && src ) noexcept
{
    mDateTime = src.mDateTime;
    return (*this);
}

inline const TIME64 & DateTime::getTime( void ) const
{
    return mDateTime;
}

inline bool DateTime::isValid( void ) const
{
    return (mDateTime != 0);
}
