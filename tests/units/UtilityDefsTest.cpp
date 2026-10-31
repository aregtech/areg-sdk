/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/UtilityDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the time, name, size and duration utilities
 *              of UtilityDefs.hpp, of the helpers of MathDefs.hpp and of DateTime formatting.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <string_view>

namespace
{
    //!< 2001-09-09 01:46:40 UTC, which is 1'000'000'000 seconds after the Unix epoch, plus 123 ms and 456 us.
    constexpr TIME64    FIXED_TIME  { 1'000'000'000ULL * 1'000'000ULL + 123'456ULL };

    //!< The largest offset between the UTC and a local time zone in microseconds, 15 hours.
    constexpr int64_t   MAX_ZONE_OFFSET { 15LL * 3600LL * 1'000'000LL };

    //!< The granularity of local time zone offsets in microseconds, 15 minutes.
    constexpr int64_t   ZONE_STEP   { 15LL * 60LL * 1'000'000LL };

    //!< Returns the value read at run time, so that a constexpr function using it is not folded by the compiler.
    template <typename T>
    T rt(T value)
    {
        volatile T copy{ value };
        return copy;
    }

    //!< Returns the content of the string as std::string.
    std::string str(const areg::String & text)
    {
        return std::string(text.as_string(), text.length());
    }
}

//////////////////////////////////////////////////////////////////////////
// Time constants and conversion
//////////////////////////////////////////////////////////////////////////

TEST(UtilityDefsTest, time_constants_are_consistent)
{
    EXPECT_EQ(areg::MIN_TO_SECS, 60);
    EXPECT_EQ(areg::HOUR_TO_SECS, 3'600);
    EXPECT_EQ(areg::DAY_TO_SECS, 86'400);
    EXPECT_EQ(areg::YEAR_TO_SECS, 365LL * 86'400LL);
    EXPECT_EQ(areg::YEAR_TO_SECS_EX, areg::YEAR_TO_SECS + areg::DAY_TO_SECS);
    EXPECT_EQ(areg::DAY_TO_MICROSECS, areg::DAY_TO_SECS * areg::SEC_TO_MICROSECS);
    EXPECT_EQ(areg::HOUR_TO_MILLISECS, areg::HOUR_TO_SECS * areg::SEC_TO_MILLISECS);
    EXPECT_EQ(areg::SEC_TO_NS, areg::SEC_TO_MICROSECS * areg::MICROSEC_TO_NS);
    EXPECT_EQ(areg::MILLISEC_TO_NS, areg::MILLISEC_TO_MICROSECS * areg::MICROSEC_TO_NS);
}

TEST(UtilityDefsTest, to_seconds_of_microseconds)
{
    EXPECT_EQ(areg::to_seconds(static_cast<TIME64>(0)), static_cast<time_t>(0));
    EXPECT_EQ(areg::to_seconds(static_cast<TIME64>(999'999)), static_cast<time_t>(0));
    EXPECT_EQ(areg::to_seconds(static_cast<TIME64>(1'000'000)), static_cast<time_t>(1));
    EXPECT_EQ(areg::to_seconds(FIXED_TIME), static_cast<time_t>(1'000'000'000));
}

TEST(UtilityDefsTest, conv_microsecs_splits_seconds_millis_and_micros)
{
    time_t secs{ 0 };
    uint16_t milli{ 0u };
    uint16_t micro{ 0u };
    areg::conv_microsecs(FIXED_TIME, secs, milli, micro);
    EXPECT_EQ(secs, static_cast<time_t>(1'000'000'000));
    EXPECT_EQ(milli, 123u);
    EXPECT_EQ(micro, 456u);

    areg::conv_microsecs(static_cast<TIME64>(0), secs, milli, micro);
    EXPECT_EQ(secs, static_cast<time_t>(0));
    EXPECT_EQ(milli, 0u);
    EXPECT_EQ(micro, 0u);

    areg::conv_microsecs(static_cast<TIME64>(999'999), secs, milli, micro);
    EXPECT_EQ(secs, static_cast<time_t>(0));
    EXPECT_EQ(milli, 999u);
    EXPECT_EQ(micro, 999u);
}

TEST(UtilityDefsTest, to_system_time_breaks_down_a_utc_time)
{
    areg::CalendarTime cal;
    areg::to_system_time(FIXED_TIME, cal);
    EXPECT_EQ(cal.stYear, 2001);
    EXPECT_EQ(cal.stMonth, 9);
    EXPECT_EQ(cal.stDay, 9);
    EXPECT_EQ(cal.stHour, 1);
    EXPECT_EQ(cal.stMinute, 46);
    EXPECT_EQ(cal.stSecond, 40);
    EXPECT_EQ(cal.stMillisecs, 123);
    EXPECT_EQ(cal.stMicrosecs, 456);
    EXPECT_EQ(cal.stDayOfWeek, 1);      // Sunday
    EXPECT_EQ(cal.stDayOfYear, 252);
}

TEST(UtilityDefsTest, to_system_time_of_epoch)
{
    areg::CalendarTime cal;
    areg::to_system_time(static_cast<TIME64>(0), cal);
    EXPECT_EQ(cal.stYear, 1970);
    EXPECT_EQ(cal.stMonth, 1);
    EXPECT_EQ(cal.stDay, 1);
    EXPECT_EQ(cal.stHour, 0);
    EXPECT_EQ(cal.stDayOfYear, 1);
    EXPECT_EQ(cal.stDayOfWeek, 5);      // Thursday
}

TEST(UtilityDefsTest, calendar_time_converts_back_to_the_same_microseconds)
{
    areg::CalendarTime cal;
    areg::to_system_time(FIXED_TIME, cal);
    EXPECT_EQ(areg::to_time(cal), FIXED_TIME);
    EXPECT_EQ(areg::to_seconds(cal), static_cast<time_t>(1'000'000'000));
}

TEST(UtilityDefsTest, to_tm_and_back_keep_the_fields)
{
    struct tm broken{};
    areg::to_tm(FIXED_TIME, broken);
    EXPECT_EQ(broken.tm_year, 101);
    EXPECT_EQ(broken.tm_mon, 8);
    EXPECT_EQ(broken.tm_mday, 9);
    EXPECT_EQ(broken.tm_hour, 1);
    EXPECT_EQ(broken.tm_min, 46);
    EXPECT_EQ(broken.tm_sec, 40);
    EXPECT_EQ(broken.tm_wday, 0);
    EXPECT_EQ(broken.tm_yday, 251);

    areg::CalendarTime cal;
    areg::to_system_time(broken, cal);
    EXPECT_EQ(cal.stYear, 2001);
    EXPECT_EQ(cal.stMonth, 9);
    EXPECT_EQ(cal.stDay, 9);
    EXPECT_EQ(cal.stDayOfWeek, 1);
    EXPECT_EQ(cal.stDayOfYear, 252);
    EXPECT_EQ(areg::to_time(broken), (FIXED_TIME / areg::SEC_TO_MICROSECS) * areg::SEC_TO_MICROSECS);
}

TEST(UtilityDefsTest, calendar_time_converts_to_tm)
{
    areg::CalendarTime cal;
    areg::to_system_time(FIXED_TIME, cal);

    struct tm broken{};
    areg::to_tm(cal, broken);
    EXPECT_EQ(broken.tm_year, 101);
    EXPECT_EQ(broken.tm_mon, 8);
    EXPECT_EQ(broken.tm_mday, 9);
    EXPECT_EQ(broken.tm_hour, 1);
    EXPECT_EQ(broken.tm_min, 46);
    EXPECT_EQ(broken.tm_sec, 40);
    EXPECT_EQ(broken.tm_wday, 0);
    EXPECT_EQ(broken.tm_yday, 251);
    EXPECT_EQ(broken.tm_isdst, -1);
}

TEST(UtilityDefsTest, compare_times_of_microseconds)
{
    EXPECT_EQ(areg::compare_times(static_cast<TIME64>(1), static_cast<TIME64>(2)), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_times(static_cast<TIME64>(2), static_cast<TIME64>(1)), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare_times(static_cast<TIME64>(2), static_cast<TIME64>(2)), areg::Ordering::Equal);
    EXPECT_EQ(areg::compare_times(static_cast<TIME64>(0xFFFFFFFFULL), static_cast<TIME64>(0x100000000ULL)), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_times(static_cast<TIME64>(0x100000000ULL), static_cast<TIME64>(0xFFFFFFFFULL)), areg::Ordering::Bigger);
}

TEST(UtilityDefsTest, compare_times_of_calendar_times)
{
    areg::CalendarTime early;
    areg::CalendarTime late;
    areg::to_system_time(FIXED_TIME, early);
    areg::to_system_time(FIXED_TIME + 1, late);
    EXPECT_EQ(areg::compare_times(early, late), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_times(late, early), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare_times(early, early), areg::Ordering::Equal);
}

TEST(UtilityDefsTest, system_time_now_in_utc_matches_the_microsecond_clock)
{
    const TIME64 before{ areg::system_time_now() };
    areg::CalendarTime cal;
    areg::system_time_now(cal, false);
    const TIME64 after{ areg::system_time_now() };

    const TIME64 converted{ areg::to_time(cal) };
    EXPECT_GE(converted + 2 * areg::SEC_TO_MICROSECS, before);
    EXPECT_LE(converted, after + 2 * areg::SEC_TO_MICROSECS);
    EXPECT_GE(cal.stYear, 2024);
    EXPECT_TRUE(areg::is_in_range(cal.stMonth, 1, 12));
    EXPECT_TRUE(areg::is_in_range(cal.stDay, 1, 31));
}

TEST(UtilityDefsTest, system_time_now_in_local_time_differs_by_a_zone_offset)
{
    areg::CalendarTime utc;
    areg::CalendarTime local;
    areg::system_time_now(utc, false);
    areg::system_time_now(local, true);

    const int64_t diff{ static_cast<int64_t>(areg::to_time(local)) - static_cast<int64_t>(areg::to_time(utc)) };
    const int64_t rest{ diff % ZONE_STEP };
    const int64_t zone{ diff - rest };
    EXPECT_LE(zone < 0 ? -zone : zone, MAX_ZONE_OFFSET);
    EXPECT_LE(rest < 0 ? -rest : rest, 5 * areg::SEC_TO_MICROSECS);
}

TEST(UtilityDefsTest, local_time_keeps_milliseconds_and_microseconds)
{
    areg::CalendarTime local;
    ASSERT_TRUE(areg::to_local_time(FIXED_TIME, local));
    EXPECT_EQ(local.stMillisecs, 123);
    EXPECT_EQ(local.stMicrosecs, 456);

    const int64_t diff{ static_cast<int64_t>(areg::to_time(local)) - static_cast<int64_t>(FIXED_TIME) };
    EXPECT_EQ(diff % ZONE_STEP, 0);
    EXPECT_LE(diff < 0 ? -diff : diff, MAX_ZONE_OFFSET);
}

TEST(UtilityDefsTest, local_time_of_calendar_time_matches_local_time_of_microseconds)
{
    areg::CalendarTime utc;
    areg::to_system_time(FIXED_TIME, utc);

    areg::CalendarTime fromCal;
    areg::CalendarTime fromMicro;
    ASSERT_TRUE(areg::to_local_time(utc, fromCal));
    ASSERT_TRUE(areg::to_local_time(FIXED_TIME, fromMicro));
    EXPECT_EQ(areg::to_time(fromCal), areg::to_time(fromMicro));
}

TEST(UtilityDefsTest, days_since_epoch_counts_calendar_days)
{
    static_assert(areg::days_since_epoch(1970, 1, 1) == 0);
    static_assert(areg::days_since_epoch(1969, 12, 31) == -1);
    EXPECT_EQ(areg::days_since_epoch(1972, 2, 29), 789);
    EXPECT_EQ(areg::days_since_epoch(1972, 3, 1), 790);
    EXPECT_EQ(areg::days_since_epoch(2001, 9, 9), 11574);
    EXPECT_EQ(areg::days_since_epoch(2024, 13, 1), areg::days_since_epoch(2025, 1, 1));
    EXPECT_EQ(areg::days_since_epoch(2024, 0, 1), areg::days_since_epoch(2023, 12, 1));
}

TEST(UtilityDefsTest, to_time_reads_month_and_day_not_day_of_year)
{
    areg::CalendarTime cal;
    cal.stYear      = 2001;
    cal.stMonth     = 9;
    cal.stDay       = 9;
    cal.stHour      = 1;
    cal.stMinute    = 46;
    cal.stSecond    = 40;
    cal.stMicrosecs = 456;
    cal.stMillisecs = 123;
    EXPECT_EQ(areg::to_time(cal), FIXED_TIME);
    EXPECT_EQ(areg::to_seconds(cal), static_cast<time_t>(FIXED_TIME / areg::SEC_TO_MICROSECS));

    struct tm broken{};
    broken.tm_year  = 101;
    broken.tm_mon   = 8;
    broken.tm_mday  = 9;
    broken.tm_hour  = 1;
    broken.tm_min   = 46;
    broken.tm_sec   = 40;
    EXPECT_EQ(areg::to_time(broken), (FIXED_TIME / areg::SEC_TO_MICROSECS) * areg::SEC_TO_MICROSECS);
}

TEST(UtilityDefsTest, local_tm_matches_local_calendar_time)
{
    struct tm broken{};
    areg::CalendarTime local;
    ASSERT_TRUE(areg::to_local_tm(FIXED_TIME, broken));
    ASSERT_TRUE(areg::to_local_time(FIXED_TIME, local));
    EXPECT_EQ(broken.tm_year + 1900, local.stYear);
    EXPECT_EQ(broken.tm_mon + 1, local.stMonth);
    EXPECT_EQ(broken.tm_mday, local.stDay);
    EXPECT_EQ(broken.tm_hour, local.stHour);
    EXPECT_EQ(broken.tm_min, local.stMinute);
    EXPECT_EQ(broken.tm_sec, local.stSecond);
}

TEST(UtilityDefsTest, make_tm_local_converts_utc_fields_to_local_fields)
{
    struct tm expected{};
    ASSERT_TRUE(areg::to_local_tm(FIXED_TIME, expected));

    struct tm broken{};
    areg::to_tm(FIXED_TIME, broken);
    areg::make_tm_local(broken);
    EXPECT_EQ(broken.tm_year, expected.tm_year);
    EXPECT_EQ(broken.tm_mon, expected.tm_mon);
    EXPECT_EQ(broken.tm_mday, expected.tm_mday);
    EXPECT_EQ(broken.tm_hour, expected.tm_hour);
    EXPECT_EQ(broken.tm_min, expected.tm_min);
    EXPECT_EQ(broken.tm_sec, expected.tm_sec);
}

TEST(UtilityDefsTest, tick_count_does_not_go_back)
{
    const uint64_t first{ areg::tick_count() };
    const uint64_t second{ areg::tick_count() };
    EXPECT_GE(second, first);
    EXPECT_LT(second - first, 60'000u);
}

//////////////////////////////////////////////////////////////////////////
// Names
//////////////////////////////////////////////////////////////////////////

TEST(UtilityDefsTest, component_item_name_joins_with_separator)
{
    const areg::String name{ areg::create_component_item_name(areg::String("Comp"), areg::String("Item")) };
    EXPECT_EQ(name, "Comp:Item");
}

TEST(UtilityDefsTest, component_item_name_is_empty_when_a_part_is_empty)
{
    EXPECT_TRUE(areg::create_component_item_name(areg::String(), areg::String("Item")).is_empty());
    EXPECT_TRUE(areg::create_component_item_name(areg::String("Comp"), areg::String()).is_empty());
    EXPECT_TRUE(areg::create_component_item_name(areg::String(), areg::String()).is_empty());
}

TEST(UtilityDefsTest, component_item_name_is_cut_to_the_maximum_length)
{
    const areg::String comp(std::string(200, 'c').c_str());
    const areg::String item(std::string(200, 'i').c_str());
    const areg::String name{ areg::create_component_item_name(comp, item) };
    EXPECT_EQ(static_cast<int32_t>(name.length()), areg::MAX_GENERATED_NAME_BUFFER_SIZE);
    EXPECT_EQ(str(name).substr(0, 200), std::string(200, 'c'));
}

TEST(UtilityDefsTest, generate_name_has_prefix_and_two_hex_fields)
{
    char buffer[64]{};
    const char * result{ areg::generate_name("prefix", buffer, 64) };
    ASSERT_EQ(result, buffer);

    const std::string name(buffer);
    ASSERT_EQ(name.size(), std::string("prefix_").size() + 8u + 1u + 8u);
    EXPECT_EQ(name.substr(0, 7), "prefix_");
    EXPECT_EQ(name[15], '_');
    EXPECT_EQ(name.find_first_not_of("0123456789abcdef", 7), 15u);
    EXPECT_EQ(name.find_first_not_of("0123456789abcdef", 16), std::string::npos);
}

TEST(UtilityDefsTest, generate_name_uses_given_separator)
{
    char buffer[64]{};
    ASSERT_NE(areg::generate_name("p", buffer, 64, "-"), nullptr);
    const std::string name(buffer);
    ASSERT_EQ(name.size(), 1u + 1u + 8u + 1u + 8u);
    EXPECT_EQ(name[1], '-');
    EXPECT_EQ(name[10], '-');
}

TEST(UtilityDefsTest, generate_name_defaults_for_null_prefix_and_separator)
{
    char buffer[64]{};
    ASSERT_NE(areg::generate_name(nullptr, buffer, 64, nullptr), nullptr);
    const std::string name(buffer);
    ASSERT_EQ(name.size(), 4u + 1u + 8u + 1u + 8u);
    EXPECT_EQ(name.substr(0, 5), "name_");
}

TEST(UtilityDefsTest, generate_name_without_buffer_returns_null)
{
    EXPECT_EQ(areg::generate_name("prefix", nullptr, 64), nullptr);
}

TEST(UtilityDefsTest, generate_name_cuts_to_a_small_buffer)
{
    char buffer[8];
    std::memset(buffer, 'x', sizeof(buffer));
    ASSERT_EQ(areg::generate_name("prefix", buffer, 8), buffer);
    EXPECT_EQ(std::strlen(buffer), 7u);
    EXPECT_EQ(std::string(buffer), "prefix_");
}

TEST(UtilityDefsTest, generate_name_returns_a_string_with_the_prefix)
{
    const areg::String name{ areg::generate_name("unit") };
    EXPECT_EQ(name.length(), 4u + 1u + 8u + 1u + 8u);
    EXPECT_EQ(str(name).substr(0, 5), "unit_");
}

TEST(UtilityDefsTest, generate_unique_id_grows)
{
    const uint32_t first{ areg::generate_unique_id() };
    const uint32_t second{ areg::generate_unique_id() };
    const uint32_t third{ areg::generate_unique_id() };
    EXPECT_NE(first, 0u);
    EXPECT_GT(second, first);
    EXPECT_GT(third, second);
}

//////////////////////////////////////////////////////////////////////////
// Data size and duration literals
//////////////////////////////////////////////////////////////////////////

TEST(UtilityDefsTest, data_size_in_bytes)
{
    areg::DataLiteral zero{ areg::conv_data_size(0) };
    EXPECT_EQ(zero.first, 0.0);
    EXPECT_EQ(zero.second, areg::STR_ONE_BYTE);

    areg::DataLiteral last{ areg::conv_data_size(areg::ONE_KILOBYTE - 1u) };
    EXPECT_EQ(last.first, static_cast<double>(areg::ONE_KILOBYTE - 1u));
    EXPECT_EQ(last.second, areg::STR_ONE_BYTE);
}

TEST(UtilityDefsTest, data_size_in_kilobytes)
{
    areg::DataLiteral one{ areg::conv_data_size(areg::ONE_KILOBYTE) };
    EXPECT_EQ(one.first, 1.0);
    EXPECT_EQ(one.second, areg::STR_ONE_KILOBYTE);

    areg::DataLiteral half{ areg::conv_data_size(areg::ONE_KILOBYTE + areg::ONE_KILOBYTE / 2u) };
    EXPECT_EQ(half.first, 1.5);
    EXPECT_EQ(half.second, areg::STR_ONE_KILOBYTE);

    areg::DataLiteral last{ areg::conv_data_size(areg::ONE_MEGABYTE - 1u) };
    EXPECT_EQ(last.second, areg::STR_ONE_KILOBYTE);
}

TEST(UtilityDefsTest, data_size_in_megabytes)
{
    areg::DataLiteral one{ areg::conv_data_size(areg::ONE_MEGABYTE) };
    EXPECT_EQ(one.first, 1.0);
    EXPECT_EQ(one.second, areg::STR_ONE_MEGABYTE);

    areg::DataLiteral big{ areg::conv_data_size(10ULL * areg::ONE_GIGABYTE) };
    EXPECT_EQ(big.first, 10.0 * 1024.0);
    EXPECT_EQ(big.second, areg::STR_ONE_MEGABYTE);

    areg::DataLiteral max{ areg::conv_data_size(UINT64_MAX) };
    EXPECT_EQ(max.second, areg::STR_ONE_MEGABYTE);
    EXPECT_GT(max.first, 1.0e12);
}

TEST(UtilityDefsTest, duration_in_nanoseconds)
{
    areg::DataLiteral zero{ areg::conv_duration(0) };
    EXPECT_EQ(zero.first, 0.0);
    EXPECT_EQ(zero.second, areg::STR_1_NS_SHORT);

    areg::DataLiteral last{ areg::conv_duration(areg::DURATION_1_MICRO - 1u) };
    EXPECT_EQ(last.first, static_cast<double>(areg::DURATION_1_MICRO - 1u));
    EXPECT_EQ(last.second, areg::STR_1_NS_SHORT);
}

TEST(UtilityDefsTest, duration_in_microseconds)
{
    areg::DataLiteral one{ areg::conv_duration(areg::DURATION_1_MICRO) };
    EXPECT_EQ(one.first, 1.0);
    EXPECT_EQ(one.second, areg::STR_1_MICROSEC_SHORT);

    areg::DataLiteral last{ areg::conv_duration(areg::DURATION_1_MILLI - 1u) };
    EXPECT_EQ(last.second, areg::STR_1_MICROSEC_SHORT);
}

TEST(UtilityDefsTest, duration_in_milliseconds)
{
    areg::DataLiteral one{ areg::conv_duration(areg::DURATION_1_MILLI) };
    EXPECT_EQ(one.first, 1.0);
    EXPECT_EQ(one.second, areg::STR_1_MILLISEC_SHORT);

    areg::DataLiteral last{ areg::conv_duration(areg::DURATION_1_SEC - 1u) };
    EXPECT_EQ(last.second, areg::STR_1_MILLISEC_SHORT);
}

TEST(UtilityDefsTest, duration_in_seconds)
{
    areg::DataLiteral two{ areg::conv_duration(2ULL * areg::DURATION_1_SEC) };
    EXPECT_EQ(two.first, 2.0);
    EXPECT_EQ(two.second, areg::STR_1_SEC_SHORT);

    areg::DataLiteral max{ areg::conv_duration(UINT64_MAX) };
    EXPECT_EQ(max.second, areg::STR_1_SEC_SHORT);
    EXPECT_GT(max.first, 1.0e9);
}

//////////////////////////////////////////////////////////////////////////
// Duration
//////////////////////////////////////////////////////////////////////////

TEST(UtilityDefsTest, duration_units_are_derived_from_nanoseconds)
{
    areg::Duration duration;
    duration.mark_start();
    duration.mark_stop();
    EXPECT_GE(duration.stop(), duration.start());
    EXPECT_EQ(duration.passed_microseconds(), duration.passed_nanoseconds() / 1'000u);
    EXPECT_EQ(duration.passed_milliseconds(), duration.passed_nanoseconds() / 1'000'000u);
    EXPECT_EQ(duration.passed_seconds(), duration.passed_nanoseconds() / 1'000'000'000u);
    EXPECT_EQ(duration.passed_minutes(), duration.passed_seconds() / 60u);
}

TEST(UtilityDefsTest, duration_keeps_the_first_stop)
{
    areg::Duration duration;
    duration.mark_start();
    const TIME64 first{ duration.mark_stop() };
    const TIME64 second{ duration.mark_stop() };
    EXPECT_EQ(first, second);
    EXPECT_EQ(duration.stop(), first);
}

TEST(UtilityDefsTest, duration_is_empty_until_stopped)
{
    areg::Duration duration;
    const TIME64 start{ duration.mark_start() };
    EXPECT_EQ(duration.start(), start);
    EXPECT_EQ(duration.passed_nanoseconds(), 0u);
    EXPECT_LT(duration.duration_since_start(), 60ULL * 1'000'000'000ULL);
}

TEST(UtilityDefsTest, duration_since_start_of_a_stopped_duration_is_its_length)
{
    areg::Duration duration;
    duration.mark_start();
    duration.mark_stop();
    EXPECT_EQ(duration.duration_since_start(), duration.passed_nanoseconds());
}

TEST(UtilityDefsTest, pointer_and_number_conversions_round_trip)
{
    int value{ 0 };
    const std::uintptr_t number{ areg::to_num<std::uintptr_t>(&value) };
    EXPECT_EQ(number, reinterpret_cast<std::uintptr_t>(&value));
    EXPECT_EQ(areg::to_ptr<int *>(number), &value);
    EXPECT_EQ(areg::to_num<uint8_t>(static_cast<uint32_t>(0x1FFu)), static_cast<uint8_t>(0xFFu));
    EXPECT_EQ(areg::to_ptr<uint32_t>(static_cast<uint64_t>(7u)), 7u);
}

//////////////////////////////////////////////////////////////////////////
// MathDefs: scalar helpers
//////////////////////////////////////////////////////////////////////////

TEST(MathDefsTest, sign_of_numbers)
{
    EXPECT_EQ(areg::sign(-5), areg::NumericSign::Negative);
    EXPECT_EQ(areg::sign(0), areg::NumericSign::Undefined);
    EXPECT_EQ(areg::sign(5), areg::NumericSign::Positive);
    EXPECT_EQ(areg::sign(-0.5), areg::NumericSign::Negative);
    EXPECT_EQ(areg::sign(0.0), areg::NumericSign::Undefined);
    EXPECT_EQ(areg::sign(0.5), areg::NumericSign::Positive);
    EXPECT_EQ(areg::sign(static_cast<uint32_t>(3u)), areg::NumericSign::Positive);
    EXPECT_EQ(areg::sign(INT64_MIN), areg::NumericSign::Negative);
    EXPECT_EQ(areg::sign(INT64_MAX), areg::NumericSign::Positive);
}

TEST(MathDefsTest, sign_as_char)
{
    EXPECT_EQ(areg::as_char(areg::NumericSign::Negative), '-');
    EXPECT_EQ(areg::as_char(areg::NumericSign::Positive), '+');
    EXPECT_EQ(areg::as_char(areg::NumericSign::Undefined), '\0');
    EXPECT_EQ(areg::as_char(static_cast<areg::NumericSign>(7)), '\0');
}

TEST(MathDefsTest, absolute_value)
{
    EXPECT_EQ(areg::abs(-5), 5);
    EXPECT_EQ(areg::abs(0), 0);
    EXPECT_EQ(areg::abs(5), 5);
    EXPECT_EQ(areg::abs(-2.5), 2.5);
    EXPECT_EQ(areg::abs(static_cast<int64_t>(-INT32_MAX)), static_cast<int64_t>(INT32_MAX));
    EXPECT_EQ(areg::abs(INT64_MAX), INT64_MAX);
    EXPECT_EQ(areg::abs(-INT64_MAX), INT64_MAX);
}

TEST(MathDefsTest, round_to_nearest_away_from_zero)
{
    EXPECT_EQ(areg::round(0.0), 0.0);
    EXPECT_EQ(areg::round(2.4), 2.0);
    EXPECT_EQ(areg::round(2.5), 3.0);
    EXPECT_EQ(areg::round(2.6), 3.0);
    EXPECT_EQ(areg::round(-2.4), -2.0);
    EXPECT_EQ(areg::round(-2.5), -3.0);
    EXPECT_EQ(areg::round(-2.6), -3.0);
    EXPECT_EQ(areg::round(0.49), 0.0);
}

TEST(MathDefsTest, compare_values)
{
    EXPECT_EQ(areg::compare(1, 2), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare(2, 1), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare(2, 2), areg::Ordering::Equal);
    EXPECT_EQ(areg::compare(-1, 1), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare(1.5, -1.5), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare(INT32_MIN, INT32_MAX), areg::Ordering::Smaller);
}

TEST(MathDefsTest, compare_two_part_integers)
{
    areg::Int64Parts small{};
    areg::Int64Parts big{};
    small.u.highPart = 1u;
    small.u.lowPart = 0xFFFFFFFFu;
    big.u.highPart = 2u;
    big.u.lowPart = 0u;
    EXPECT_EQ(areg::compare_parts(small, big), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_parts(big, small), areg::Ordering::Bigger);

    big.u.highPart = 1u;
    big.u.lowPart = 0u;
    EXPECT_EQ(areg::compare_parts(small, big), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare_parts(big, small), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_parts(small, small), areg::Ordering::Equal);
}

TEST(MathDefsTest, range_check_includes_both_ends)
{
    EXPECT_TRUE(areg::is_in_range(5, 1, 10));
    EXPECT_TRUE(areg::is_in_range(1, 1, 10));
    EXPECT_TRUE(areg::is_in_range(10, 1, 10));
    EXPECT_FALSE(areg::is_in_range(0, 1, 10));
    EXPECT_FALSE(areg::is_in_range(11, 1, 10));
    EXPECT_TRUE(areg::is_in_range(-3, -5, -1));
    EXPECT_FALSE(areg::is_in_range(5, 10, 1));
    EXPECT_TRUE(areg::is_in_range(UINT32_MAX, 0u, UINT32_MAX));
}

TEST(MathDefsTest, align_size_up_to_the_block)
{
    EXPECT_EQ(areg::align_size(0u, 8u), 0u);
    EXPECT_EQ(areg::align_size(1u, 8u), 8u);
    EXPECT_EQ(areg::align_size(8u, 8u), 8u);
    EXPECT_EQ(areg::align_size(9u, 8u), 16u);
    EXPECT_EQ(areg::align_size(13u, 1u), 13u);
    EXPECT_EQ(areg::align_size(static_cast<uint64_t>(4097u), static_cast<uint64_t>(4096u)), static_cast<uint64_t>(8192u));
}

TEST(MathDefsTest, delta_is_a_distance)
{
    EXPECT_EQ(areg::delta(3u, 10u), 7u);
    EXPECT_EQ(areg::delta(10u, 3u), 7u);
    EXPECT_EQ(areg::delta(5u, 5u), 0u);
    EXPECT_EQ(areg::delta(-5, 5), 10);
    EXPECT_EQ(areg::delta(5, -5), 10);
    EXPECT_EQ(areg::delta(1.5, 4.0), 2.5);
    EXPECT_EQ(areg::delta(static_cast<uint64_t>(0), UINT64_MAX), UINT64_MAX);
}

//////////////////////////////////////////////////////////////////////////
// MathDefs: bytes, words and swapping
//////////////////////////////////////////////////////////////////////////

TEST(MathDefsTest, low_and_high_byte)
{
    EXPECT_EQ(areg::lo_byte(static_cast<uint32_t>(0x12345678u)), 0x78u);
    EXPECT_EQ(areg::hi_byte(static_cast<uint32_t>(0x12345678u)), 0x12u);
    EXPECT_EQ(areg::lo_byte(static_cast<uint16_t>(0xABCDu)), 0xCDu);
    EXPECT_EQ(areg::hi_byte(static_cast<uint16_t>(0xABCDu)), 0xABu);
    EXPECT_EQ(areg::lo_byte(static_cast<int16_t>(-2)), 0xFEu);
    EXPECT_EQ(areg::hi_byte(static_cast<int16_t>(-2)), 0xFFu);
    EXPECT_EQ(areg::lo_byte(static_cast<uint8_t>(0x5Au)), 0x5Au);
    EXPECT_EQ(areg::hi_byte(static_cast<uint8_t>(0x5Au)), 0x5Au);
    EXPECT_EQ(areg::hi_byte(static_cast<uint64_t>(0x1122334455667788ULL)), 0x11u);
    EXPECT_EQ(areg::lo_byte(static_cast<uint64_t>(0)), 0u);
}

TEST(MathDefsTest, words_and_double_words)
{
    EXPECT_EQ(areg::lo_word(0x12345678u), 0x5678u);
    EXPECT_EQ(areg::hi_word(0x12345678u), 0x1234u);
    EXPECT_EQ(areg::lo_word(0u), 0u);
    EXPECT_EQ(areg::hi_word(UINT32_MAX), 0xFFFFu);
    EXPECT_EQ(areg::lo_dword(0x1122334455667788ULL), 0x55667788u);
    EXPECT_EQ(areg::hi_dword(0x1122334455667788ULL), 0x11223344u);
    EXPECT_EQ(areg::lo_dword(UINT64_MAX), UINT32_MAX);
    EXPECT_EQ(areg::hi_dword(0), 0u);
}

TEST(MathDefsTest, make_numbers_from_parts)
{
    EXPECT_EQ(areg::make16(0x12u, 0x34u), 0x1234u);
    EXPECT_EQ(areg::make32(0x1234u, 0x5678u), 0x12345678u);
    EXPECT_EQ(areg::make64(0x11223344u, 0x55667788u), 0x1122334455667788ULL);
    EXPECT_EQ(areg::make16(0xFFu, 0xFFu), 0xFFFFu);
    EXPECT_EQ(areg::make32(0u, 0u), 0u);
    EXPECT_EQ(areg::make64(UINT32_MAX, UINT32_MAX), UINT64_MAX);
}

TEST(MathDefsTest, construct_numbers_from_bytes)
{
    EXPECT_EQ(areg::construct32(0x12u, 0x34u, 0x56u, 0x78u), 0x12345678u);
    EXPECT_EQ(areg::construct64(0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u, 0x77u, 0x88u), 0x1122334455667788ULL);
    EXPECT_EQ(areg::construct32(0u, 0u, 0u, 0u), 0u);
    EXPECT_EQ(areg::construct64(0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu), UINT64_MAX);
}

TEST(MathDefsTest, make_and_split_are_inverse)
{
    constexpr uint64_t value{ 0x0123456789ABCDEFULL };
    EXPECT_EQ(areg::make64(areg::hi_dword(value), areg::lo_dword(value)), value);
    EXPECT_EQ(areg::make32(areg::hi_word(0xCAFEBABEu), areg::lo_word(0xCAFEBABEu)), 0xCAFEBABEu);
}

TEST(MathDefsTest, swap_bytes_of_16_and_32_bits)
{
    EXPECT_EQ(areg::swap_bytes(static_cast<uint16_t>(0x1234u)), static_cast<uint16_t>(0x3412u));
    EXPECT_EQ(areg::swap_bytes(static_cast<uint32_t>(0x12345678u)), static_cast<uint32_t>(0x78563412u));
    EXPECT_EQ(areg::swap_bytes(static_cast<uint16_t>(0u)), static_cast<uint16_t>(0u));
    EXPECT_EQ(areg::swap_bytes(static_cast<uint32_t>(UINT32_MAX)), static_cast<uint32_t>(UINT32_MAX));
    EXPECT_EQ(areg::swap_bytes(static_cast<int16_t>(-2)), static_cast<int16_t>(-257));
    EXPECT_EQ(areg::swap_bytes(areg::swap_bytes(static_cast<uint32_t>(0xA1B2C3D4u))), static_cast<uint32_t>(0xA1B2C3D4u));
}

//////////////////////////////////////////////////////////////////////////
// MathDefs: CRC32
//////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr std::string_view  CRC_TEXT    { "123456789" };

    //!< The CRC-32C check value of the text "123456789".
    constexpr uint32_t          CRC_CHECK   { 0xE3069283u };
}

TEST(MathDefsTest, crc32_of_the_standard_check_text)
{
    EXPECT_EQ(areg::crc32_calculate(CRC_TEXT), CRC_CHECK);
    EXPECT_EQ(areg::crc32_calculate("123456789"), CRC_CHECK);
    EXPECT_EQ(areg::crc32_calculate(reinterpret_cast<const uint8_t *>("123456789"), 9), CRC_CHECK);
    EXPECT_EQ(areg::crc32_calculate(L"123456789"), CRC_CHECK);
}

TEST(MathDefsTest, crc32_of_nothing_is_zero)
{
    EXPECT_EQ(areg::crc32_calculate(static_cast<const char *>(nullptr)), 0u);
    EXPECT_EQ(areg::crc32_calculate(static_cast<const wchar_t *>(nullptr)), 0u);
    EXPECT_EQ(areg::crc32_calculate(""), 0u);
    EXPECT_EQ(areg::crc32_calculate(std::string_view()), 0u);
    EXPECT_EQ(areg::crc32_calculate(reinterpret_cast<const uint8_t *>("abc"), 0), 0u);
    EXPECT_EQ(areg::crc32_finish(areg::crc32_init()), 0u);
}

TEST(MathDefsTest, crc32_of_wide_characters_uses_both_bytes)
{
    const uint8_t bytes[]{ 0x00u, 0x01u };
    EXPECT_EQ(areg::crc32_calculate(L"\x0100"), areg::crc32_calculate(bytes, 2));
    EXPECT_NE(areg::crc32_calculate(L"\x0100"), areg::crc32_calculate(L"\x0001"));
}

TEST(MathDefsTest, crc32_can_be_calculated_in_pieces)
{
    uint32_t crc{ areg::crc32_init() };
    crc = areg::crc32_start(crc, "1234");
    crc = areg::crc32_start(crc, std::string_view("56"));
    crc = areg::crc32_start(crc, reinterpret_cast<const uint8_t *>("789"), 3);
    EXPECT_EQ(areg::crc32_finish(crc), CRC_CHECK);

    uint32_t bytewise{ areg::crc32_init() };
    for (char ch : CRC_TEXT)
    {
        bytewise = areg::crc32_start(bytewise, static_cast<uint8_t>(ch));
    }
    EXPECT_EQ(areg::crc32_finish(bytewise), CRC_CHECK);
}

TEST(MathDefsTest, crc32_start_ignores_null_data)
{
    const uint32_t init{ areg::crc32_init() };
    EXPECT_EQ(areg::crc32_start(init, static_cast<const char *>(nullptr)), init);
    EXPECT_EQ(areg::crc32_start(init, static_cast<const uint8_t *>(nullptr), 10), init);
    EXPECT_EQ(areg::crc32_start(init, std::string_view()), init);
}

TEST(MathDefsTest, crc32_hardware_equals_software_for_every_tail_length)
{
    uint8_t data[67]{};
    for (uint32_t i = 0u; i < sizeof(data); ++i)
    {
        data[i] = static_cast<uint8_t>(i * 37u + 11u);
    }

    for (int32_t size = 0; size <= static_cast<int32_t>(sizeof(data)); ++size)
    {
        EXPECT_EQ(areg::crc32_hardware(data, size), areg::crc32_calculate(data, size)) << "size " << size;
    }

    EXPECT_EQ(areg::crc32_hardware(reinterpret_cast<const uint8_t *>("123456789"), 9), CRC_CHECK);
}

TEST(MathDefsTest, crc32_validity)
{
    EXPECT_FALSE(areg::crc32_valid(areg::CHECKSUM_IGNORE));
    EXPECT_FALSE(areg::crc32_valid(areg::CHECKSUM_INVALID));
    EXPECT_TRUE(areg::crc32_valid(1u));
    EXPECT_TRUE(areg::crc32_valid(CRC_CHECK));
}

TEST(MathDefsTest, runtime_class_names_do_not_collide)
{
    constexpr std::string_view names[]{ "Alpha", "Beta", "Gamma" };
    EXPECT_EQ(areg::crc32_first_collision(names), 3u);

    constexpr std::string_view same[]{ "Alpha", "Beta", "Alpha" };
    EXPECT_EQ(areg::crc32_first_collision(same), 0u);
}

//////////////////////////////////////////////////////////////////////////
// MathDefs: LargeInteger
//////////////////////////////////////////////////////////////////////////

TEST(LargeIntegerTest, default_is_zero)
{
    const areg::LargeInteger number;
    EXPECT_EQ(number.hiBits, 0u);
    EXPECT_EQ(number.loBits, 0u);
}

TEST(LargeIntegerTest, construct_from_parts)
{
    const areg::LargeInteger number(rt(0x11223344u), rt(0x55667788u));
    EXPECT_EQ(areg::high_bits(number), 0x11223344u);
    EXPECT_EQ(areg::low_bits(number), 0x55667788u);
}

TEST(LargeIntegerTest, construct_from_64_bit_number)
{
    const areg::LargeInteger number(rt(static_cast<uint64_t>(0x0000000100000002ULL)));
    EXPECT_EQ(number.hiBits, 1u);
    EXPECT_EQ(number.loBits, 2u);
}

TEST(LargeIntegerTest, convert_to_64_bit_number)
{
    const areg::LargeInteger number(rt(0x00000001u), rt(0x00000002u));
    EXPECT_EQ(static_cast<uint64_t>(number), 0x0000000100000002ULL);
}

TEST(LargeIntegerTest, converting_a_number_back_keeps_it)
{
    constexpr uint64_t values[]{ 0ULL, 1ULL, 0xFFFFFFFFULL, 0x100000000ULL, 0x0123456789ABCDEFULL, UINT64_MAX };
    for (uint64_t value : values)
    {
        const areg::LargeInteger number(value);
        EXPECT_EQ(static_cast<uint64_t>(number), value);
    }
}

TEST(LargeIntegerTest, assign_a_64_bit_number)
{
    areg::LargeInteger number;
    [[maybe_unused]] const areg::LargeInteger & ref{ number = rt(static_cast<uint64_t>(0xAABBCCDD11223344ULL)) };
    EXPECT_EQ(number.hiBits, 0xAABBCCDDu);
    EXPECT_EQ(number.loBits, 0x11223344u);
}

TEST(LargeIntegerTest, set_bits_replaces_both_parts)
{
    areg::LargeInteger number(rt(1u), rt(2u));
    areg::set_bits(number, 7u, 8u);
    EXPECT_EQ(number.hiBits, 7u);
    EXPECT_EQ(number.loBits, 8u);
}

TEST(LargeIntegerTest, equality)
{
    const areg::LargeInteger one(rt(1u), rt(2u));
    const areg::LargeInteger same(rt(1u), rt(2u));
    const areg::LargeInteger other(rt(1u), rt(3u));
    EXPECT_TRUE(one == same);
    EXPECT_FALSE(one != same);
    EXPECT_TRUE(one != other);
    EXPECT_FALSE(one == other);
    EXPECT_TRUE(one == one);
    EXPECT_FALSE(one != one);
}

TEST(LargeIntegerTest, equality_with_a_64_bit_number)
{
    const areg::LargeInteger one(rt(1u), rt(2u));
    EXPECT_TRUE(one == rt(static_cast<uint64_t>(0x0000000100000002ULL)));
    EXPECT_TRUE(one != rt(static_cast<uint64_t>(0x0000000200000001ULL)));
}

TEST(LargeIntegerTest, add_and_subtract_numbers)
{
    const areg::LargeInteger number(rt(static_cast<uint64_t>(0x00000001FFFFFFFFULL)));
    EXPECT_EQ(number + rt(static_cast<uint64_t>(1u)), 0x0000000200000000ULL);
    EXPECT_EQ(number - rt(static_cast<uint64_t>(1u)), 0x00000001FFFFFFFEULL);
    EXPECT_EQ(number + areg::LargeInteger(rt(static_cast<uint64_t>(0x0000000100000001ULL))), 0x0000000300000000ULL);
    EXPECT_EQ(number - areg::LargeInteger(rt(static_cast<uint64_t>(0x00000001FFFFFFFFULL))), 0ULL);
}

TEST(LargeIntegerTest, compound_add_and_subtract_a_64_bit_number)
{
    areg::LargeInteger number(rt(0x00000001u), rt(0xFFFFFFFFu));
    [[maybe_unused]] const areg::LargeInteger & added{ number += rt(static_cast<uint64_t>(1u)) };
    EXPECT_EQ(number.hiBits, 2u);
    EXPECT_EQ(number.loBits, 0u);

    [[maybe_unused]] const areg::LargeInteger & subtracted{ number -= rt(static_cast<uint64_t>(2u)) };
    EXPECT_EQ(number.hiBits, 1u);
    EXPECT_EQ(number.loBits, 0xFFFFFFFEu);
}

TEST(LargeIntegerTest, compound_add_a_large_integer)
{
    areg::LargeInteger number(rt(0x00000001u), rt(0xFFFFFFFFu));
    [[maybe_unused]] const areg::LargeInteger & added{ number += areg::LargeInteger(rt(0x00000001u), rt(0x00000001u)) };
    EXPECT_EQ(number.hiBits, 3u);
    EXPECT_EQ(number.loBits, 0u);
}

TEST(LargeIntegerTest, compound_subtract_a_large_integer)
{
    areg::LargeInteger number(rt(0x00000003u), rt(0x00000000u));
    [[maybe_unused]] const areg::LargeInteger & subtracted{ number -= areg::LargeInteger(rt(0x00000001u), rt(0x00000001u)) };
    EXPECT_EQ(number.hiBits, 1u);
    EXPECT_EQ(number.loBits, 0xFFFFFFFFu);
}

TEST(LargeIntegerTest, stream_round_trip)
{
    const areg::LargeInteger original(rt(0x01020304u), rt(0x05060708u));
    areg::SharedBuffer buffer;
    buffer << original;
    EXPECT_EQ(buffer.size_used(), 2u * sizeof(uint32_t));

    buffer.reset();
    areg::LargeInteger read;
    buffer >> read;
    EXPECT_TRUE(read == original);
}

//////////////////////////////////////////////////////////////////////////
// DateTime: stream and text
//////////////////////////////////////////////////////////////////////////

TEST(DateTimeFormatTest, stream_round_trip_keeps_the_time)
{
    const areg::DateTime original(FIXED_TIME);
    areg::SharedBuffer buffer;
    buffer << original;

    buffer.reset();
    const areg::DateTime read(buffer);
    EXPECT_TRUE(read == original);
    EXPECT_EQ(read.time(), FIXED_TIME);

    buffer.reset();
    areg::DateTime assigned;
    buffer >> assigned;
    EXPECT_TRUE(assigned == original);
}

TEST(DateTimeFormatTest, invalid_time_formats_as_empty_text)
{
    const areg::DateTime invalid;
    EXPECT_FALSE(invalid.is_valid());
    EXPECT_TRUE(invalid.format_time().is_empty());

    areg::String text("old");
    areg::DateTime::format_time(invalid, text);
    EXPECT_TRUE(text.is_empty());
}

TEST(DateTimeFormatTest, milliseconds_are_printed_with_three_digits)
{
    const areg::DateTime date(FIXED_TIME);
    EXPECT_EQ(date.format_time("%l"), "123");

    const areg::DateTime tenMillis(FIXED_TIME - 113'000);
    EXPECT_EQ(tenMillis.format_time("%l"), "010");
}

TEST(DateTimeFormatTest, empty_format_name_gives_the_default_format)
{
    const areg::DateTime date(FIXED_TIME);
    const areg::String text{ date.format_time(std::string_view()) };
    EXPECT_EQ(text.length(), 23u);
    EXPECT_EQ(str(text).substr(0, 4), "2001");
    EXPECT_EQ(str(text).substr(19, 4), ",123");
    EXPECT_EQ(text, date.format_time(areg::DEFAULT_TIME_FORMAT_OUTPUT));
}

TEST(DateTimeFormatTest, absolute_format_has_time_and_milliseconds_only)
{
    const areg::DateTime date(FIXED_TIME);
    const areg::String text{ date.format_time(areg::TIME_FORMAT_ABSOLUTE_OUTPUT) };
    EXPECT_EQ(text.length(), 12u);
    EXPECT_EQ(str(text).substr(2, 1), ":");
    EXPECT_EQ(str(text).substr(5, 1), ":");
    EXPECT_EQ(str(text).substr(8, 4), ",123");
}

TEST(DateTimeFormatTest, date_format_names_the_month)
{
    const areg::DateTime date(FIXED_TIME);
    const areg::String text{ date.format_time(areg::TIME_FORMAT_DATE_OUTPUT) };
    EXPECT_TRUE(text.is_valid_position(text.find_first("Sep")));
    EXPECT_TRUE(text.is_valid_position(text.find_first("2001")));
    EXPECT_TRUE(text.is_valid_position(text.find_first(",123")));
}

TEST(DateTimeFormatTest, calendar_components_of_a_time_read_at_run_time)
{
    const areg::DateTime date(rt(FIXED_TIME));
    EXPECT_EQ(date.year(), 2001u);
    EXPECT_EQ(date.month(), 9u);
    EXPECT_EQ(date.day(), 9u);
    EXPECT_EQ(date.hours(), 1u);
    EXPECT_EQ(date.minutes(), 46u);
    EXPECT_EQ(date.seconds(), 40u);
    EXPECT_EQ(date.milliseconds(), 123u);
    EXPECT_EQ(date.microseconds(), 456u);
    EXPECT_EQ(date.day_of_year(), 252u);
}

TEST(DateTimeFormatTest, calendar_components_match_the_utc_calendar_on_every_day)
{
    uint32_t wrong{ 0u };
    for (TIME64 day = 0u; day < 29'000u; ++day)
    {
        const TIME64 time{ rt(day * static_cast<TIME64>(areg::DAY_TO_MICROSECS) + 12ULL * static_cast<TIME64>(areg::HOUR_TO_MICROSECS)) };
        areg::CalendarTime cal;
        areg::to_system_time(time, cal);

        const areg::DateTime date(time);
        if ((date.year() != static_cast<uint32_t>(cal.stYear)) || (date.month() != static_cast<uint32_t>(cal.stMonth))
            || (date.day() != static_cast<uint32_t>(cal.stDay)) || (date.day_of_year() != static_cast<uint32_t>(cal.stDayOfYear)))
        {
            if (++wrong <= 3u)
            {
                ADD_FAILURE() << "day " << day << ": " << cal.stYear << "-" << cal.stMonth << "-" << cal.stDay << " (day of year " << cal.stDayOfYear
                              << ") but DateTime reports " << date.year() << "-" << date.month() << "-" << date.day() << " (day of year " << date.day_of_year() << ")";
            }
        }
    }

    EXPECT_EQ(wrong, 0u);
}

TEST(DateTimeFormatTest, day_of_week_counts_from_monday)
{
    const areg::DateTime sunday(rt(FIXED_TIME));
    EXPECT_EQ(sunday.day_of_week(), 6u);

    const areg::DateTime monday(rt(FIXED_TIME + 86'400ULL * 1'000'000ULL));
    EXPECT_EQ(monday.day_of_week(), 0u);

    const areg::DateTime thursday(rt(static_cast<TIME64>(0)));
    EXPECT_EQ(thursday.day_of_week(), 3u);
}
