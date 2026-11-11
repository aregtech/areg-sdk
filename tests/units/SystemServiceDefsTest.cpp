/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SystemServiceDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the system service definitions: names of the
 *              service options and phases, the option table and the held socket helpers.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"

#include <cstring>
#include <iterator>

namespace
{
    using areg::ext::ServiceOption;
    using areg::ext::ServicePhase;

    //!< A pair of an option and its expected name.
    struct OptionName
    {
        ServiceOption   value;
        const char *    name;
    };

    //!< A pair of a phase and its expected name.
    struct PhaseName
    {
        ServicePhase    value;
        const char *    name;
    };

    //!< Expected row of the option table.
    struct OptionRow
    {
        const char *    shortName;
        const char *    longName;
        ServiceOption   command;
        uint32_t        field;
    };
}

TEST(SystemServiceDefsTest, service_option_names)
{
    static const OptionName table[] =
    {
          { ServiceOption::CMD_Undefined, "areg::ext::CMD_Undefined" }
        , { ServiceOption::CMD_Console  , "areg::ext::CMD_Console" }
        , { ServiceOption::CMD_Help     , "areg::ext::CMD_Help" }
        , { ServiceOption::CMD_Load     , "areg::ext::CMD_Load" }
        , { ServiceOption::CMD_Install  , "areg::ext::CMD_Install" }
        , { ServiceOption::CMD_Service  , "areg::ext::CMD_Service" }
        , { ServiceOption::CMD_Uninstall, "areg::ext::ServiceOption::CMD_Uninstall" }
        , { ServiceOption::CMD_Verbose  , "areg::ext::ServiceOption::CMD_Verbose" }
        , { ServiceOption::CMD_Custom   , "areg::ext::ServiceOption::CMD_Custom" }
    };

    for (const OptionName & row : table)
    {
        EXPECT_STREQ(areg::ext::as_string(row.value), row.name);
    }
}

TEST(SystemServiceDefsTest, service_phase_names)
{
    static const PhaseName table[] =
    {
          { ServicePhase::Stopped   , "areg::ext::ServiceStopped" }
        , { ServicePhase::Starting  , "areg::ext::ServiceStarting" }
        , { ServicePhase::Stopping  , "areg::ext::ServiceStopping" }
        , { ServicePhase::Running   , "areg::ext::ServiceRunning" }
        , { ServicePhase::Continuing, "areg::ext::ServiceContinuing" }
        , { ServicePhase::Pausing   , "areg::ext::ServicePausing" }
        , { ServicePhase::Paused    , "areg::ext::ServicePaused" }
    };

    for (const PhaseName & row : table)
    {
        EXPECT_STREQ(areg::ext::as_string(row.value), row.name);
    }
}

TEST(SystemServiceDefsTest, service_names_are_distinct)
{
    const ServiceOption options[] =
    {
          ServiceOption::CMD_Undefined, ServiceOption::CMD_Help, ServiceOption::CMD_Load
        , ServiceOption::CMD_Verbose  , ServiceOption::CMD_Uninstall, ServiceOption::CMD_Install
        , ServiceOption::CMD_Service  , ServiceOption::CMD_Console  , ServiceOption::CMD_Custom
    };

    for (size_t i = 0; i < std::size(options); ++i)
    {
        for (size_t j = i + 1u; j < std::size(options); ++j)
        {
            EXPECT_NE(std::strcmp(areg::ext::as_string(options[i]), areg::ext::as_string(options[j])), 0);
        }
    }
}

TEST(SystemServiceDefsTest, option_table_maps_names_to_commands)
{
    static const OptionRow rows[] =
    {
          { ""  , ""            , ServiceOption::CMD_Console  , areg::ext::OptionParser::NO_DATA }
        , { "-c", "--console"   , ServiceOption::CMD_Console  , areg::ext::OptionParser::NO_DATA }
        , { "-h", "--help"      , ServiceOption::CMD_Help     , areg::ext::OptionParser::NO_DATA }
        , { "-l", "--load"      , ServiceOption::CMD_Load     , areg::ext::OptionParser::STRING_NO_RANGE }
        , { "-i", "--install"   , ServiceOption::CMD_Install  , areg::ext::OptionParser::NO_DATA }
        , { "-s", "--service"   , ServiceOption::CMD_Service  , areg::ext::OptionParser::NO_DATA }
        , { "-u", "--uninstall" , ServiceOption::CMD_Uninstall, areg::ext::OptionParser::NO_DATA }
        , { "-v", "--verbose"   , ServiceOption::CMD_Verbose  , areg::ext::OptionParser::NO_DATA }
    };

    ASSERT_EQ(std::size(areg::ext::ServiceOptionSetup), std::size(rows));
    for (size_t i = 0; i < std::size(rows); ++i)
    {
        const areg::ext::OptionParser::OptionSetup & setup = areg::ext::ServiceOptionSetup[i];
        EXPECT_EQ(setup.optShort, rows[i].shortName);
        EXPECT_EQ(setup.optLong, rows[i].longName);
        EXPECT_EQ(setup.optCmmand, static_cast<int32_t>(rows[i].command));
        EXPECT_EQ(setup.optField, rows[i].field);
    }
}

TEST(SystemServiceDefsTest, defaults_start_as_console_with_data_rate)
{
    EXPECT_EQ(areg::ext::DEFAULT_OPTION, ServiceOption::CMD_Console);
    EXPECT_TRUE(areg::ext::DEFAULT_VERBOSE);
    EXPECT_EQ(areg::ext::DEFAULT_COMMUNICATION_PAIR_COUNT, 0u);
}

TEST(SystemServiceDefsTest, hold_socket_keeps_one_entry_per_socket)
{
    areg::ext::HeldSocket holds[4];
    uint32_t count{ 0u };
    const areg::Socket::SocketHandle hold{};
    const SOCKETHANDLE first{ static_cast<SOCKETHANDLE>(5) };
    const SOCKETHANDLE second{ static_cast<SOCKETHANDLE>(6) };

    areg::ext::hold_socket(first, hold, holds, count);
    EXPECT_EQ(count, 1u);
    areg::ext::hold_socket(first, hold, holds, count);
    EXPECT_EQ(count, 1u);
    areg::ext::hold_socket(second, hold, holds, count);
    EXPECT_EQ(count, 2u);
    EXPECT_EQ(holds[0].socket, first);
    EXPECT_EQ(holds[1].socket, second);
}

TEST(SystemServiceDefsTest, find_hold_returns_the_entry_of_the_socket)
{
    areg::ext::HeldSocket holds[4];
    uint32_t count{ 0u };
    const areg::Socket::SocketHandle hold{};
    const SOCKETHANDLE first{ static_cast<SOCKETHANDLE>(5) };
    const SOCKETHANDLE second{ static_cast<SOCKETHANDLE>(6) };
    const SOCKETHANDLE missing{ static_cast<SOCKETHANDLE>(7) };

    areg::ext::hold_socket(first, hold, holds, count);
    areg::ext::hold_socket(second, hold, holds, count);

    EXPECT_EQ(areg::ext::find_hold(first, holds, count), &holds[0].hold);
    EXPECT_EQ(areg::ext::find_hold(second, holds, count), &holds[1].hold);
    EXPECT_EQ(areg::ext::find_hold(missing, holds, count), nullptr);
}

TEST(SystemServiceDefsTest, find_hold_ignores_entries_beyond_the_count)
{
    areg::ext::HeldSocket holds[2];
    const SOCKETHANDLE socket{ static_cast<SOCKETHANDLE>(9) };
    holds[0].socket = socket;

    EXPECT_EQ(areg::ext::find_hold(socket, holds, 0u), nullptr);
    EXPECT_EQ(areg::ext::find_hold(socket, holds, 1u), &holds[0].hold);
}

TEST(SystemServiceDefsTest, convert_arguments_copies_narrow_arguments)
{
    char first[]  { "app" };
    char second[] { "--console" };
    char empty[]  { "" };
    char* argv[] { first, second, empty };
    constexpr int32_t argc{ static_cast<int32_t>(std::size(argv)) };

    char** result = areg::ext::convert_arguments<char>(argv, argc);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result[0], "app");
    EXPECT_STREQ(result[1], "--console");
    EXPECT_STREQ(result[2], "");
    EXPECT_NE(result[1], second);
    areg::ext::delete_arguments(result, argc);
}

TEST(SystemServiceDefsTest, convert_arguments_narrows_wide_arguments)
{
    wchar_t first[]  { L"router" };
    wchar_t second[] { L"-s" };
    wchar_t* argv[] { first, second };
    constexpr int32_t argc{ static_cast<int32_t>(std::size(argv)) };

    char** result = areg::ext::convert_arguments<wchar_t>(argv, argc);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result[0], "router");
    EXPECT_STREQ(result[1], "-s");
    areg::ext::delete_arguments(result, argc);
}

TEST(SystemServiceDefsTest, convert_arguments_returns_null_for_no_arguments)
{
    char first[] { "app" };
    char* argv[] { first };

    EXPECT_EQ(areg::ext::convert_arguments<char>(argv, 0), nullptr);
    EXPECT_EQ(areg::ext::convert_arguments<char>(argv, -1), nullptr);
    EXPECT_EQ(areg::ext::convert_arguments<char>(static_cast<char**>(nullptr), 1), nullptr);
    areg::ext::delete_arguments(nullptr, 3);
}

TEST(SystemServiceDefsTest, out_of_range_values_have_error_names)
{
    EXPECT_EQ(std::strncmp(areg::ext::as_string(static_cast<ServiceOption>(0x7FFF)), "ERR:", 4), 0);
    EXPECT_EQ(std::strncmp(areg::ext::as_string(static_cast<ServicePhase>(0x7FFF)), "ERR:", 4), 0);
}
