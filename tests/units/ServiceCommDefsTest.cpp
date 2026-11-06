/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceCommDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the definitions of the system service: the names of
 *              the options and phases, the list of held sockets and the data rate literals.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/Socket.hpp"
#include "aregextend/service/DataRateHelper.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"

#include <string>

namespace
{
    using areg::ext::DataRateHelper;

    //!< Returns true if \a text contains \a part.
    bool contains(const char * text, const char * part)
    {
        return (std::string(text).find(part) != std::string::npos);
    }
}

// Every service option has its own name, and the name tells the option.
TEST(ServiceCommDefsTest, service_option_names_tell_the_option)
{
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Undefined), "CMD_Undefined"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Console), "CMD_Console"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Help), "CMD_Help"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Load), "CMD_Load"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Install), "CMD_Install"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Service), "CMD_Service"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Uninstall), "CMD_Uninstall"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Verbose), "CMD_Verbose"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServiceOption::CMD_Custom), "CMD_Custom"));
}

// Every service phase has its own name, and the name tells the phase.
TEST(ServiceCommDefsTest, service_phase_names_tell_the_phase)
{
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Stopped), "Stopped"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Starting), "Starting"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Stopping), "Stopping"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Running), "Running"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Continuing), "Continuing"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Pausing), "Pausing"));
    EXPECT_TRUE(contains(areg::ext::as_string(areg::ext::ServicePhase::Paused), "Paused"));
}

// A socket is listed once, however often it is held, and found by its handle.
TEST(ServiceCommDefsTest, held_socket_is_listed_once_and_found_by_its_handle)
{
    areg::ext::HeldSocket holds[3]{ };
    uint32_t count{ 0u };
    const areg::Socket::SocketHandle hold{ };
    const SOCKETHANDLE first{ static_cast<SOCKETHANDLE>(5) };
    const SOCKETHANDLE second{ static_cast<SOCKETHANDLE>(6) };
    const SOCKETHANDLE unknown{ static_cast<SOCKETHANDLE>(7) };

    EXPECT_EQ(areg::ext::find_hold(first, holds, count), nullptr);

    areg::ext::hold_socket(first, hold, holds, count);
    EXPECT_EQ(count, 1u);
    areg::ext::hold_socket(first, hold, holds, count);
    EXPECT_EQ(count, 1u);
    areg::ext::hold_socket(second, hold, holds, count);
    EXPECT_EQ(count, 2u);

    EXPECT_EQ(areg::ext::find_hold(first, holds, count), &holds[0].hold);
    EXPECT_EQ(areg::ext::find_hold(second, holds, count), &holds[1].hold);
    EXPECT_EQ(areg::ext::find_hold(unknown, holds, count), nullptr);
}

// Below a kilobyte the rate is in bytes, from a kilobyte in kilobytes, from a megabyte in
// megabytes, and the value is the size in that unit.
TEST(ServiceCommDefsTest, data_rate_literals_pick_the_unit_by_size)
{
    DataRateHelper::DataRate rate{ DataRateHelper::convert_data_rate_literals(0u) };
    EXPECT_FLOAT_EQ(rate.first, 0.0f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_BYTES));

    rate = DataRateHelper::convert_data_rate_literals(DataRateHelper::ONE_KILOBYTE - 1u);
    EXPECT_FLOAT_EQ(rate.first, static_cast<float>(DataRateHelper::ONE_KILOBYTE - 1u));
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_BYTES));

    rate = DataRateHelper::convert_data_rate_literals(DataRateHelper::ONE_KILOBYTE);
    EXPECT_FLOAT_EQ(rate.first, 1.0f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_KILOBYTES));

    rate = DataRateHelper::convert_data_rate_literals(DataRateHelper::ONE_KILOBYTE + DataRateHelper::ONE_KILOBYTE / 2u);
    EXPECT_FLOAT_EQ(rate.first, 1.5f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_KILOBYTES));

    rate = DataRateHelper::convert_data_rate_literals(DataRateHelper::ONE_MEGABYTE - 1u);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_KILOBYTES));

    rate = DataRateHelper::convert_data_rate_literals(DataRateHelper::ONE_MEGABYTE);
    EXPECT_FLOAT_EQ(rate.first, 1.0f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_MEGABYTES));

    rate = DataRateHelper::convert_data_rate_literals(static_cast<uint64_t>(DataRateHelper::ONE_MEGABYTE) * 5u / 2u);
    EXPECT_FLOAT_EQ(rate.first, 2.5f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_MEGABYTES));
}

// A rate far above the megabyte stays in megabytes.
TEST(ServiceCommDefsTest, data_rate_literals_of_a_gigabyte_stay_in_megabytes)
{
    const DataRateHelper::DataRate rate{ DataRateHelper::convert_data_rate_literals(static_cast<uint64_t>(DataRateHelper::ONE_MEGABYTE) * 1024u) };
    EXPECT_FLOAT_EQ(rate.first, 1024.0f);
    EXPECT_EQ(rate.second, std::string(DataRateHelper::MSG_MEGABYTES));
}
