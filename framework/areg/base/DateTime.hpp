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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

    //!< Initializes to invalid (zero) time.
    inline constexpr DateTime() noexcept;

    inline constexpr DateTime( const DateTime & dateTime ) noexcept = default;

    ~DateTime() = default;

    /**
     * \brief   Initializes date and time from microseconds since Unix epoch (January 1, 1970).
     **/
    inline constexpr DateTime( const TIME64 & dateTime ) noexcept;

    /**
     * \brief   Initializes date and time from system calendar time structure.
     **/
    inline constexpr explicit DateTime( const areg::CalendarTime & sysTime ) noexcept;

    /**
     * \brief   Initializes date and time by deserializing from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     **/
    DateTime( const InStream & stream );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline constexpr DateTime& operator = (const DateTime& src) noexcept;

    inline constexpr DateTime& operator = (DateTime&& src) noexcept;

    [[nodiscard]]
    inline constexpr bool operator == (const DateTime& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator != (const DateTime& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator > (const DateTime& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator < (const DateTime& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator >= (const DateTime& other) const noexcept;

    [[nodiscard]]
    inline constexpr bool operator <= (const DateTime& other) const noexcept;

    /**
     * \brief   Converts to 64-bit unsigned integer representing microseconds since Unix epoch
     *          (January 1, 1970).
     **/
    [[nodiscard]]
    inline constexpr operator TIME64 () const noexcept;

    /**
     * \brief   Converts to std::chrono::microseconds duration since Unix epoch.
     **/
    [[nodiscard]]
    inline operator std::chrono::microseconds() const noexcept;

    /**
     * \brief   Converts to std::chrono::milliseconds duration since Unix epoch.
     **/
    [[nodiscard]]
    inline operator std::chrono::milliseconds() const noexcept;

    /**
     * \brief   Converts to std::chrono::seconds duration since Unix epoch.
     **/
    [[nodiscard]]
    inline operator std::chrono::seconds() const noexcept;

    /**
     * \brief   Deserializes date and time value from stream.
     *
     * \param   stream      Streaming object containing serialized date and time.
     * \param[out] input    DateTime object initialized from deserialized stream data.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, DateTime & input );

    /**
     * \brief   Serializes date and time value to stream.
     *
     * \param[out] stream   Streaming object where date and time will be serialized.
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
     **/
    [[nodiscard]]
    static inline DateTime now() noexcept;

    /**
     * \brief   Returns current system time in UTC as a timestamp.
     **/
    [[nodiscard]]
    static inline TIME64 timestamp() noexcept;

    /**
     * \brief   Fills calendar time structure with current system time in UTC or local time.
     *
     * \param[out] timeData     Calendar time structure to fill with current time values.
     * \param   localTime       If true, timeData is filled with local time; if false, UTC time.
     **/
    static inline void now( areg::CalendarTime & timeData, bool localTime ) noexcept;

    /**
     * \brief   Returns milliseconds elapsed since system startup.
     **/
    [[nodiscard]]
    static inline uint64_t system_tick_count() noexcept;

    /**
     * \brief   Returns milliseconds elapsed since current process started.
     **/
    [[nodiscard]]
    static inline uint64_t process_tick_count() noexcept;

    /**
     * \brief   Formats DateTime as a string using the specified format.
     *
     * \param   dateTime        DateTime object to format.
     * \param[out] result       String to receive the formatted DateTime.
     * \param   formatName      Format specification name (defaults to DEFAULT_TIME_FORMAT_OUTPUT).
     **/
    static void format_time(const DateTime &dateTime, String& result, std::string_view formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT);

/************************************************************************/
// Non-static operations
/************************************************************************/

    /**
     * \brief   Returns DateTime formatted as a string using the specified format.
     *
     * \param   formatName      Format specification name (defaults to DEFAULT_TIME_FORMAT_OUTPUT).
     **/
    [[nodiscard]]
    String format_time( std::string_view formatName = areg::DEFAULT_TIME_FORMAT_OUTPUT) const;

    /**
     * \brief   Returns the time value as microseconds since Unix epoch.
     **/
    [[nodiscard]]
    inline constexpr const TIME64 & time() const noexcept;

    /**
     * \brief   Sets the date and time value in microseconds since Unix epoch.
     *
     * \param   newTime     New time value in microseconds since Unix epoch (January 1, 1970).
     **/
    inline constexpr void set_time(const TIME64& newTime) noexcept;

    /**
     * \brief   Returns true if time value is non-zero.
     **/
    [[nodiscard]]
    inline constexpr bool is_valid() const noexcept;

    /**
     * \brief   Returns the year component extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t year() const noexcept;

    /**
     * \brief   Returns the month component (1-12) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t month() const noexcept;

    /**
     * \brief   Returns the day component (1-31) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t day() const noexcept;

    /**
     * \brief   Returns the hour component (0-23) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t hours() const noexcept;

    /**
     * \brief   Returns the minute component (0-59) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t minutes() const noexcept;

    /**
     * \brief   Returns the second component (0-59) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t seconds() const noexcept;

    /**
     * \brief   Returns the millisecond component (0-999) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t milliseconds() const noexcept;

    /**
     * \brief   Returns the microsecond component (0-999) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t microseconds() const noexcept;

    /**
     * \brief   Returns the day of year component (1-365/366) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t day_of_year() const noexcept;

    /**
     * \brief   Returns the day of week component (0-6, 0=Monday) extracted from the date-time value.
     **/
    [[nodiscard]]
    inline constexpr uint32_t day_of_week() const noexcept;

    /**
     * \brief   Converts date and time value to calendar time structure with broken-down components.
     *
     * \param[out] sysTime     Calendar time structure to receive broken-down date and time values.
     **/
    inline void date_time(areg::CalendarTime& sysTime) noexcept;

    /**
     * \brief   Sets date and time from calendar time structure, converting to microseconds since Unix epoch.
     *
     * \param   sysTime     Calendar time structure to convert.
     **/
    inline void set_date_time(const areg::CalendarTime& sysTime) noexcept;

    /**
     * \brief   Converts date and time value to tm structure with broken-down components.
     *
     * \param[out] time    tm structure to receive broken-down date and time values.
     **/
    inline void date_time(struct tm& time) noexcept;

    /**
     * \brief   Sets date and time from tm structure, converting to microseconds since Unix epoch.
     *
     * \param   time    tm structure to convert.
     **/
    inline void set_date_time(const struct tm& time) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Broken-down date components for shared use by calendar extraction methods.
     **/
    struct DateParts { uint32_t years; uint32_t month; uint32_t day; };

    /**
     * \brief   Decomposes the stored microsecond value into broken-down date components.
     * \return  DateParts with approximate years since Unix epoch, month (1-12) and day of month (1-31).
     **/
    [[nodiscard]]
    inline constexpr DateParts _decompose() const noexcept;

    /**
     * \brief   Date and time in microseconds passed since Unix epoch (January 1, 1970).
     **/
    TIME64  mDateTime;
};

//////////////////////////////////////////////////////////////////////////
// DateTime class inline / constexpr implementations
//////////////////////////////////////////////////////////////////////////

inline constexpr DateTime::DateTime() noexcept
    : mDateTime( INVALID_TIME )
{
}

inline constexpr DateTime::DateTime( const TIME64 & dateTime ) noexcept
    : mDateTime( dateTime )
{
}

inline constexpr DateTime::DateTime( const areg::CalendarTime & sysTime ) noexcept
    : mDateTime( areg::to_time(sysTime) )
{
}

inline constexpr DateTime::operator TIME64 () const noexcept
{
    return mDateTime;
}

inline DateTime::operator std::chrono::microseconds() const noexcept
{
    return std::chrono::microseconds(mDateTime);
}

inline DateTime::operator std::chrono::milliseconds() const noexcept
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::microseconds(mDateTime));
}

inline DateTime::operator std::chrono::seconds() const noexcept
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::microseconds(mDateTime));
}

inline constexpr DateTime & DateTime::operator = ( const DateTime & src ) noexcept
{
    mDateTime = src.mDateTime;
    return (*this);
}

inline constexpr DateTime & DateTime::operator = ( DateTime && src ) noexcept
{
    mDateTime = src.mDateTime;
    return (*this);
}

inline constexpr bool DateTime::operator == (const DateTime& other) const noexcept
{
    return (mDateTime == other.mDateTime);
}

inline constexpr bool DateTime::operator != (const DateTime& other) const noexcept
{
    return (mDateTime != other.mDateTime);
}

inline constexpr bool DateTime::operator > (const DateTime& other) const noexcept
{
    return (mDateTime > other.mDateTime);
}

inline constexpr bool DateTime::operator < (const DateTime& other) const noexcept
{
    return (mDateTime < other.mDateTime);
}

inline constexpr bool DateTime::operator >= (const DateTime& other) const noexcept
{
    return (mDateTime >= other.mDateTime);
}

inline constexpr bool DateTime::operator <= (const DateTime& other) const noexcept
{
    return (mDateTime <= other.mDateTime);
}

inline DateTime DateTime::now() noexcept
{
    return DateTime(areg::system_time_now());
}

inline TIME64 DateTime::timestamp() noexcept
{
    return areg::system_time_now();
}

inline void DateTime::now(areg::CalendarTime& timeData, bool localTime) noexcept
{
    areg::system_time_now(timeData, localTime);
}

inline uint64_t DateTime::system_tick_count() noexcept
{
    return areg::tick_count();
}

inline uint64_t DateTime::process_tick_count() noexcept
{
    return areg::tick_count();
}

inline constexpr const TIME64 & DateTime::time() const noexcept
{
    return mDateTime;
}

inline constexpr void DateTime::set_time(const TIME64& newTime) noexcept
{
    mDateTime = newTime;
}

inline constexpr bool DateTime::is_valid() const noexcept
{
    return (mDateTime != INVALID_TIME);
}

inline constexpr uint32_t DateTime::year() const noexcept
{
    constexpr double   _secsInYear{ 60.0 * 60.0 * 24.0 * 365.25 };
    constexpr uint32_t _unixEpoch{ 1970 };

    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    return static_cast<uint32_t>(static_cast<double>(secs) / _secsInYear) + _unixEpoch;
}

inline constexpr DateTime::DateParts DateTime::_decompose() const noexcept
{
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    uint32_t days       = static_cast<uint32_t>(mDateTime / areg::DAY_TO_MICROSECS);
    uint32_t years      = days / 365u;
    uint32_t remainDays = (days % 365u) - ((years + 1u) / 4u);
    uint32_t month      = 1u;
    while (remainDays >= _DAYS_IN_MONTH[month - 1u])
    {
        remainDays -= _DAYS_IN_MONTH[month - 1u];
        ++month;
    }

    return { years, month, remainDays + 1u };
}

inline constexpr uint32_t DateTime::month() const noexcept
{
    return _decompose().month;
}

inline constexpr uint32_t DateTime::day() const noexcept
{
    return _decompose().day;
}

inline constexpr uint32_t DateTime::hours() const noexcept
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    return static_cast<uint32_t>(remainSecs / areg::HOUR_TO_SECS);
}

inline constexpr uint32_t DateTime::minutes() const noexcept
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    return static_cast<uint32_t>((remainSecs % areg::HOUR_TO_SECS) / areg::MIN_TO_SECS);
}

inline constexpr uint32_t DateTime::seconds() const noexcept
{
    uint64_t secs{ mDateTime / areg::SEC_TO_MICROSECS };
    uint32_t remainSecs{ static_cast<uint32_t>(secs % areg::DAY_TO_SECS) };
    return static_cast<uint32_t>((remainSecs % areg::HOUR_TO_SECS) % areg::MIN_TO_SECS);
}

inline constexpr uint32_t DateTime::milliseconds() const noexcept
{
    return static_cast<uint32_t>( (mDateTime / areg::MILLISEC_TO_MICROSECS) % areg::MILLISEC_TO_MICROSECS );
}

inline constexpr uint32_t DateTime::microseconds() const noexcept
{
    return static_cast<uint32_t>(mDateTime % areg::MILLISEC_TO_MICROSECS);
}

inline constexpr uint32_t DateTime::day_of_year() const noexcept
{
    constexpr uint32_t _DAYS_IN_MONTH[]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const DateParts p = _decompose();
    uint32_t dayOfYear = 0u;
    for (uint32_t i = 0u; i < p.month - 1u; ++i)
        dayOfYear += _DAYS_IN_MONTH[i];

    return (dayOfYear + p.day);
}

inline constexpr uint32_t DateTime::day_of_week() const noexcept
{
    const DateParts p   = _decompose();
    const uint32_t  dow = (p.day + ((13u * (p.month + 1u)) / 5u) + p.years
                        + (p.years / 4u) - (p.years / 100u) + (p.years / 400u)) % 7u;
    return (dow + 6u) % 7u;
}

inline void DateTime::date_time(areg::CalendarTime& sysTime) noexcept
{
    areg::to_system_time(mDateTime, sysTime);
}

inline void DateTime::set_date_time(const areg::CalendarTime& sysTime) noexcept
{
    mDateTime = areg::to_time(sysTime);
}

inline void DateTime::date_time(tm& time) noexcept
{
    areg::to_tm(mDateTime, time);
}

inline void DateTime::set_date_time(const tm& time) noexcept
{
    mDateTime = areg::to_time(time);
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

template<>
struct required_size<areg::DateTime>
{
    [[nodiscard]]
    inline constexpr uint32_t operator()() const noexcept
    {
        return static_cast<uint32_t>(sizeof(TIME64));
    }

    [[nodiscard]]
    inline constexpr uint32_t operator()(const areg::DateTime& /*dt*/) const noexcept
    {
        return static_cast<uint32_t>(sizeof(TIME64));
    }

    [[nodiscard]]
    inline constexpr operator uint32_t () const noexcept
    {
        return static_cast<uint32_t>(sizeof(TIME64));
    }
};

} // namespace areg
#endif  // AREG_BASE_DATETIME_HPP
