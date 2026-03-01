#ifndef AREG_BASE_DATETIME_HPP
#define AREG_BASE_DATETIME_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/DateTime.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Date and Time class.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <chrono>
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
struct tm;

namespace areg {
//////////////////////////////////////////////////////////////////////////
// DateTime class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Date and time value stored as 64-bit microseconds since Unix epoch, with conversion and
 *          component-extraction operations.
 **/
class AREG_API DateTime
{
public:
/************************************************************************/
// Public constants
/************************************************************************/

/**
     * \brief   Invalid time.
     **/
    static constexpr TIME64             INVALID_TIME                        { 0 };

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes date and time value to zero.
     **/
    DateTime();

    /**
     * \brief   Initializes date and time from microseconds since Unix epoch (January 1, 1970).
     *
     * \param   dateTime    Time in microseconds since Unix epoch.
     **/
    DateTime( const TIME64 & dateTime );

    /**
     * \brief   Initializes date and time from system calendar time structure.
     *
     * \param   sysTime     System time structure to extract date and time values.
     **/
    explicit DateTime( const areg::CalendarTime & sysTime );

    /**
     * \brief   Copies date and time data from another DateTime object.
     *
     * \param   dateTime    Source DateTime object to copy from.
     **/
    DateTime( const DateTime & dateTime );

    /**
     * \brief   Moves date and time data from another DateTime object.
     *
     * \param   dateTime    Source DateTime object to move from.
     **/
    DateTime( DateTime && dateTime ) noexcept;

    /**
     * \brief   Initializes date and time by deserializing from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     **/
    DateTime( const InStream & stream );

    /**
     * \brief   Destructor.
     **/
    ~DateTime() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts to 64-bit unsigned integer representing microseconds since Unix epoch
     *          (January 1, 1970).
     **/
    inline operator TIME64 () const;

    /**
     * \brief   Converts to std::chrono::microseconds duration since Unix epoch.
     **/
    inline operator std::chrono::microseconds() const;

    /**
     * \brief   Converts to std::chrono::milliseconds duration since Unix epoch.
     **/
    inline operator std::chrono::milliseconds() const;

    /**
     * \brief   Converts to std::chrono::seconds duration since Unix epoch.
     **/
    inline operator std::chrono::seconds() const;

    /**
     * \brief   Assigns date and time value from another DateTime object.
     *
     * \param   src     Source DateTime object.
     **/
    inline DateTime & operator = ( const DateTime & src );

    /**
     * \brief   Move-assigns date and time value from another DateTime object.
     *
     * \param   src     Source DateTime object to move from.
     **/
    inline DateTime & operator = ( DateTime && src ) noexcept;

    /**
     * \brief   Returns true if two DateTime objects are equal.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if both DateTime objects have equal time values; false otherwise.
     **/
    bool operator == ( const DateTime & other ) const;

    /**
     * \brief   Returns true if two DateTime objects are not equal.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if DateTime objects have different time values; false otherwise.
     **/
    bool operator != ( const DateTime & other ) const;

    /**
     * \brief   Returns true if this DateTime is greater than the other.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if this DateTime value is greater than other; false otherwise.
     **/
    bool operator > ( const DateTime & other ) const;

    /**
     * \brief   Returns true if this DateTime is less than the other.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if this DateTime value is less than other; false otherwise.
     **/
    bool operator < ( const DateTime & other ) const;

    /**
     * \brief   Returns true if this DateTime is greater than or equal to the other.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if this DateTime value is greater than or equal to other; false
     *          otherwise.
     **/
    bool operator >= ( const DateTime & other ) const;

    /**
     * \brief   Returns true if this DateTime is less than or equal to the other.
     *
     * \param   other       DateTime object to compare.
     * \return  Returns true if this DateTime value is less than or equal to other; false otherwise.
     **/
    bool operator <= ( const DateTime & other ) const;

    /**
     * \brief   Deserializes date and time value from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     * \param[out] input       DateTime object initialized from deserialized stream data.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, DateTime & input );

    /**
     * \brief   Serializes date and time value to stream.
     *
     * \param[out] stream      Streaming object where date and time will be serialized.
     * \param   output      DateTime object to serialize.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const DateTime & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Static operations
/************************************************************************/
    /**
     * \brief   Returns current system time in UTC as a DateTime object.
     *
     * \return  DateTime representing current UTC time in microseconds since Unix epoch.
     **/
    static DateTime now();

    /**
     * \brief   Fills calendar time structure with current system time in UTC or local time.
     *
     * \param[out] timeData        Calendar time structure to fill with current time values.
     * \param   localTime       If true, timeData is filled with local time; if false, UTC time.
     **/
    static void now( areg::CalendarTime & timeData, bool localTime );

    /**
     * \brief   Returns milliseconds elapsed since system startup.
     **/
    static uint64_t system_tick_count();

    /**
     * \brief   Returns milliseconds elapsed since current process started.
     **/
    static uint64_t process_tick_count();

    /**
     * \brief   Formats DateTime as a string using the specified format.
     *
     * \param   dateTime        DateTime object to format.
     * \param[out] result          String to receive the formatted DateTime.
     * \param   formatName      Format specification name (defaults to DEFAULT_TIME_FORMAT_OUTPUT).
     **/
    static void format_time(const DateTime &dateTime, String& result, const std::string_view& formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT);

/************************************************************************/
// Non-static operations
/************************************************************************/

    /**
     * \brief   Returns DateTime formatted as a string using the specified format.
     *
     * \param   formatName      Format specification name (defaults to DEFAULT_TIME_FORMAT_OUTPUT).
     **/
    String format_time( const std::string_view & formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT) const;

    /**
     * \brief   Returns the time value as microseconds since Unix epoch.
     **/
    inline const TIME64 & time() const;

    /**
     * \brief   Sets the date and time value in microseconds since Unix epoch.
     *
     * \param   newTime     New time value in microseconds since Unix epoch (January 1, 1970).
     **/
    inline void set_time(const TIME64& newTime);

    /**
     * \brief   Returns true if time value is non-zero.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns the year component extracted from the date-time value.
     **/
    uint32_t year() const;

    /**
     * \brief   Returns the month component (1-12) extracted from the date-time value.
     **/
    uint32_t month() const;

    /**
     * \brief   Returns the day component (1-31) extracted from the date-time value.
     **/
    uint32_t day() const;

    /**
     * \brief   Returns the hour component (0-23) extracted from the date-time value.
     **/
    uint32_t hours() const;

    /**
     * \brief   Returns the minute component (0-59) extracted from the date-time value.
     **/
    uint32_t minutex() const;

    /**
     * \brief   Returns the second component (0-59) extracted from the date-time value.
     **/
    uint32_t secons() const;

    /**
     * \brief   Returns the millisecond component (0-999) extracted from the date-time value.
     **/
    uint32_t milliscones() const;

    /**
     * \brief   Returns the microsecond component (0-999) extracted from the date-time value.
     **/
    uint32_t microseconds() const;

    /**
     * \brief   Returns the day of year component (1-365/366) extracted from the date-time value.
     **/
    uint32_t day_of_year() const;

    /**
     * \brief   Returns the day of week component (1-7) extracted from the date-time value.
     **/
    uint32_t day_of_week() const;

    /**
     * \brief   Converts date and time value to calendar time structure with broken-down components.
     *
     * \param[out] sysTime     Calendar time structure to receive broken-down date and time values.
     **/
    void date_time(areg::CalendarTime& sysTime);

    /**
     * \brief   Sets date and time from calendar time structure, converting to microseconds since
     *          Unix epoch.
     *
     * \param   sysTime     Calendar time structure to convert.
     **/
    void set_date_time(const areg::CalendarTime& sysTime);

    /**
     * \brief   Converts date and time value to tm structure with broken-down components.
     *
     * \param[out] time    tm structure to receive broken-down date and time values.
     **/
    void date_time(struct tm& time);

    /**
     * \brief   Sets date and time from tm structure, converting to microseconds since Unix epoch.
     *
     * \param   time    tm structure to convert.
     **/
    void set_date_time(const struct tm& time);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Date and time in microseconds passed since Unix epoch (January 1, 1970).
     **/
    TIME64  mDateTime;
};

//////////////////////////////////////////////////////////////////////////
// DateTime class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline DateTime::operator TIME64 () const
{
    return mDateTime;
}

inline DateTime::operator std::chrono::microseconds() const
{
    return std::chrono::microseconds(mDateTime);
}

inline DateTime::operator std::chrono::milliseconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(mDateTime));
}

inline DateTime::operator std::chrono::seconds() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::microseconds(mDateTime));
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

inline const TIME64 & DateTime::time() const
{
    return mDateTime;
}

inline void DateTime::set_time(const TIME64& newTime)
{
    mDateTime = newTime;
}

inline bool DateTime::is_valid() const
{
    return (mDateTime != INVALID_TIME);
}

inline const InStream & operator >> ( const InStream & stream, DateTime & input )
{
    stream >> input.mDateTime;
    return stream;
}

inline OutStream & operator << ( OutStream & stream, const DateTime & output )
{
    stream << output.mDateTime;
    return stream;
}

} // namespace areg
#endif  // AREG_BASE_DATETIME_HPP
