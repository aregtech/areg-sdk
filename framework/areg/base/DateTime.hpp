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
#include "areg/base/GEGlobal.h"

#include "areg/base/IOStream.hpp"
#include "areg/base/String.hpp"
#include "areg/base/UtilityDefs.hpp"

#include <chrono>
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
struct tm;

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // DateTime class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Date and Time class. It saves date and time value as 
     *          64-bit value and makes basic operations with date and time value.
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
         * \brief   Default constructor. Sets zero value.
         **/
        DateTime();

        /**
         * \brief   Sets date and time value in microseconds passed since January 1 1970.
         * \param   dateTime    The time in microseconds passed since January 1 1970.
         **/
        DateTime( const TIME64 & dateTime );

        /**
         * \brief   Sets date and time value from given system time structure.
         * \param   sysTime     System time structure to get date and time values.
         **/
        explicit DateTime( const CalendarTime & sysTime );

        /**
         * \brief   Copies data and time data from given source.
         * \param   dateTime    The source to copy data.
         **/
        DateTime( const DateTime & dateTime );

        /**
         * \brief   Moves data and time data from given source.
         * \param   dateTime    The source to move data.
         **/
        DateTime( DateTime && dateTime ) noexcept;

        /**
         * \brief   Initializes data from streaming object
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
         * \brief   Converts and returns date and time value as 64-bit unsigned integer value.
         *          The returned value is microseconds passed since Unix epoch (January 1, 1970).
         **/
        inline operator TIME64 () const;

        /**
         * \brief   Converts and returns date and time value in microseconds passed since Unix epoch (Unix epoch, since January 1, 1970).
         **/
        inline operator std::chrono::microseconds() const;

        /**
         * \brief   Converts and returns date and time value in milliseconds passed since Unix epoch (Unix epoch, since January 1, 1970).
         **/
        inline operator std::chrono::milliseconds() const;

        /**
         * \brief   Converts and returns date and time value in seconds passed since Unix epoch (Unix epoch, since January 1, 1970).
         **/
        inline operator std::chrono::seconds() const;

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
         * \brief   Reads (deserializes) date and time value from streaming object.
         * \param   stream  Streaming object, which contains serialized date and time value
         * \param   input   Date and time object, which is initialized by deserializing 
         *                  date and time value from stream
         **/
        friend inline const InStream & operator >> ( const InStream & stream, DateTime & input );

        /**
         * \brief   Writes (serializes) date and time value to streaming object.
         * \param   stream  Streaming object to serialized date and time value
         * \param   output  Date and time object, which is contains date and time value and
         *                  should be serialized to streaming object
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
         * \brief   Retrieves and returns current system time in UTC (Coordinated Universal Time).
         *          The value in DateTime object is in microseconds passed since Unix epoch (January 1, 1970).
         * \return  Returns current date and time value.
         **/
        static DateTime getNow();

        /**
         * \brief   Retrieves and returns current time either in UTC or local time.
         * \param[out]  timeData    On output, it will contain the time values.
         * \param[in]   localTime   If true, timeData is converted to local time.
         **/
        static void getNow( CalendarTime & timeData, bool localTime );

        /**
         * \brief   Retrieves the number of milliseconds that have elapsed since the system was started.
         **/
        static uint64_t getSystemTickCount();

        /**
         * \brief   Retrieves the number of milliseconds that have elapsed since current process has been started.
         **/
        static uint64_t getProcessTickCount();

        /**
         * \brief   Formats time and outputs as a string. The caller should specify the
         *          the time format name for output.
         * \param[in]   dateTime    The DateTime object to convert to string.
         * \param[out]  result      On output this contains formated string of DateTime.
         * \param[in]   formatName  The formating to convert DateTime.
         **/
        static void formatTime(const DateTime &dateTime, String& result, const std::string_view& formatName = DEFAULT_TIME_FORMAT_OUTPUT);

    /************************************************************************/
    // Non-static operations
    /************************************************************************/

        /**
         * \brief   Formats time and outputs as a string. The caller should specify the
         *          the time format name for output
         **/
        String formatTime( const std::string_view & formatName = DEFAULT_TIME_FORMAT_OUTPUT) const;

        /**
         * \brief   Returns the time data.
         **/
        inline const TIME64 & getTime() const;

        /**
         * \brief   Set the date time value. The new value should be set in microseconds since Unix epoch.
         * \param   newTime     The new time in microseconds since Unix epoch (January 1, 1970).
         **/
        inline void setTime(const TIME64& newTime);

        /**
         * \brief   Returns true, if time value is not zero
         **/
        inline bool isValid() const;

        /**
         * \brief   Breaks the date-time value and returns the calculated year.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getYear() const;

        /**
         * \brief   Breaks the date-time value and returns the calculated month within the year.
         *          The first month is January and the last month is the December.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getMonth() const;

        /**
         * \brief   Breaks the date-time value and returns the day within the month.
         *          The first day is 1 and the last day depends on month.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getDay() const;

        /**
         * \brief   Breaks the date-time value and returns the hour within the day.
         *          The returned value is between 0 and 23.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getHours() const;

        /**
         * \brief   Breaks the date-time value and returns the minutes within the hour.
         *          The returned value is between 0 and 59.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getMinutex() const;

        /**
         * \brief   Breaks the date-time value and returns the seconds within the minute.
         *          The returned value is between 0 and 59.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getSecons() const;

        /**
         * \brief   Breaks the date-time value and returns the milliseconds within the second.
         *          The returned value is between 0 and 999.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getMilliscones() const;

        /**
         * \brief   Breaks the date-time value and returns the microseconds within the millisecond.
         *          The returned value is between 0 and 999.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getMicroseconds() const;

        /**
         * \brief   Breaks the date-time value and returns the day of the year.
         *          The returned value is between 1 and 365 / 366.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getDayOfTheYear() const;

        /**
         * \brief   Breaks the date-time value and returns the day of the week.
         *          The returned value is between 1 and 7.
         *          The date-time value is in microseconds passed since Unix epoch.
         **/
        uint32_t getDayOfTheWeek() const;

        /**
         * \brief   Converts existing date and time value in microseconds passed since Unix epoch to system time structure.
         *          On output, the sysTime contains converted and broken date and time values.
         * \param[out]  sysTime     The System Time structure to break the data.
         **/
        void getDateTime(CalendarTime& sysTime);

        /**
         * \brief   Calculates and sets the value in microseconds passed since Unix epoch.
         *          The date and time information is in 'sysTime' parameter.
         * \param[in]   sysTime     The System Time structure as a source of data.
         **/
        void setDateTime(const CalendarTime& sysTime);

        /**
         * \brief   Converts existing date and time value in microseconds passed since Unix epoch to tm structure.
         *          On output, the 'time' contains converted and broken date and time values.
         * \param[out]  time    The tm structure object to break the data.
         **/
        void getDateTime(struct tm& time);

        /**
         * \brief   Calculates and sets the value in microseconds passed since Unix epoch.
         *          The date and time information is in 'time' parameter.
         * \param[in]   time    The tm structure as a source of data.
         **/
        void setDateTime(const struct tm& time);

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

    inline const TIME64 & DateTime::getTime() const
    {
        return mDateTime;
    }

    inline void DateTime::setTime(const TIME64& newTime)
    {
        mDateTime = newTime;
    }

    inline bool DateTime::isValid() const
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
