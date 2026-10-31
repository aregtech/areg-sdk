/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/DebugDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the debug output helpers of DebugDefs.hpp: priority prefixes and the console output calls.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/private/DebugDefs.hpp"

#include <string>
#include <string_view>

TEST(DebugDefsTest, priority_prefixes)
{
    EXPECT_EQ(areg::prio_prefix(areg::DebugPriority::PrioNone), "");
    EXPECT_EQ(areg::prio_prefix(areg::DebugPriority::PrioErr), "ERR: ");
    EXPECT_EQ(areg::prio_prefix(areg::DebugPriority::PrioWarn), "WRN: ");
    EXPECT_EQ(areg::prio_prefix(areg::DebugPriority::PrioInfo), "INF: ");
    EXPECT_EQ(areg::prio_prefix(areg::DebugPriority::PrioDbg), "DBG: ");
}

TEST(DebugDefsTest, output_console_with_priority_accepts_every_priority)
{
    areg::output_console(areg::DebugPriority::PrioErr, "error %d", 1);
    areg::output_console(areg::DebugPriority::PrioWarn, "warning %s", "text");
    areg::output_console(areg::DebugPriority::PrioInfo, "info %u\n", 2u);
    areg::output_console(areg::DebugPriority::PrioDbg, "debug");
    areg::output_console(areg::DebugPriority::PrioNone, "none %d", 3);
    SUCCEED();
}

TEST(DebugDefsTest, output_console_with_priority_ignores_a_null_message)
{
    areg::output_console(areg::DebugPriority::PrioErr, nullptr);
    SUCCEED();
}

TEST(DebugDefsTest, output_console_with_priority_cuts_a_too_long_message)
{
    const std::string longText(areg::MAX_DEBUG_BUFFER_SIZE * 2u, 'x');
    areg::output_console(areg::DebugPriority::PrioInfo, "%s", longText.c_str());
    SUCCEED();
}

TEST(DebugDefsTest, output_console_without_priority_formats_and_ignores_null)
{
    areg::output_console("plain %d %s\n", 4, "text");
    areg::output_console("no arguments");
    areg::output_console(static_cast<const char *>(nullptr));
    SUCCEED();
}

TEST(DebugDefsTest, output_console_without_priority_cuts_a_too_long_message)
{
    const std::string longText(areg::MAX_DEBUG_BUFFER_SIZE * 2u, 'y');
    areg::output_console("%s", longText.c_str());
    SUCCEED();
}

TEST(DebugDefsTest, output_message_os_accepts_empty_and_null_text)
{
    areg::output_message_os(nullptr);
    areg::output_message_os("");
    areg::output_message_os("message\n");
    SUCCEED();
}
