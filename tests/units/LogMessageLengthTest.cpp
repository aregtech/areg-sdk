/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogMessageLengthTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the log message length contract.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <cstdarg>
#include <string>

namespace
{
    //! Calls the va_list form under test, which is the one the logging path uses.
    int32_t _format(char* buffer, int32_t count, int32_t& required, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        const int32_t result{ areg::String::format_string_list(buffer, count, format, args, required) };
        va_end(args);
        return result;
    }
}

/**
 * \brief   The buffer must never be written past its end, whatever the text length.
 **/
TEST(LogMessageLengthTest, test_formatNeverOverruns)
{
    constexpr int count{ 16 };
    char buffer[count + 4]{ 0 };

    // Fill the guard area, so an overrun is visible.
    for (int i = count; i < (count + 4); ++i)
    {
        buffer[i] = 'X';
    }

    const std::string longText(64, 'a');
    int32_t required{ 0 };
    const int32_t written{ _format(buffer, count, required, "%s", longText.c_str()) };

    EXPECT_LT(written, count);                  // never fills the last byte
    EXPECT_EQ(buffer[written], '\0');           // always terminated
    EXPECT_EQ(required, 64);                    // reports what the text needed
    for (int i = count; i < (count + 4); ++i)
    {
        EXPECT_EQ(buffer[i], 'X') << "the guard byte at " << i << " was overwritten";
    }
}

/**
 * \brief   A message that fits reports its own length and is not marked as cut.
 **/
TEST(LogMessageLengthTest, test_shortMessageIsNotCut)
{
    areg::LogEntry entry;
    const std::string text{ "a short message" };
    areg::mem_copy(entry.logMessage, areg::LOG_MSG_SIZE - 1u, text.c_str(), static_cast<uint32_t>(text.length()));
    entry.logMessage[text.length()] = '\0';
    entry.logMessageLen = static_cast<uint32_t>(text.length());

    EXPECT_FALSE(areg::is_log_message_cut(entry));
    EXPECT_EQ(areg::log_message_size(entry), static_cast<uint32_t>(text.length()));
}

/**
 * \brief   A message longer than the buffer is marked as cut, and the clamped length
 *          stays inside the buffer so that every reader is safe.
 **/
TEST(LogMessageLengthTest, test_longMessageIsCutAndClamped)
{
    constexpr uint32_t maxLen{ areg::LOG_MSG_SIZE - 1u };

    areg::LogEntry entry;
    const std::string text(maxLen + 312u, 'b');
    areg::mem_copy(entry.logMessage, maxLen, text.c_str(), static_cast<uint32_t>(text.length()));
    entry.logMessage[maxLen] = '\0';
    entry.logMessageLen = static_cast<uint32_t>(text.length());   // the length before the cut

    EXPECT_TRUE(areg::is_log_message_cut(entry));
    EXPECT_EQ(areg::log_message_size(entry), maxLen);
    EXPECT_EQ(entry.logMessageLen - areg::log_message_size(entry), 312u);
}

/**
 * \brief   The clamped length never exceeds the buffer, whatever the field holds.
 *          This is the guarantee every consumer relies on.
 **/
TEST(LogMessageLengthTest, test_clampedLengthAlwaysFits)
{
    constexpr uint32_t maxLen{ areg::LOG_MSG_SIZE - 1u };

    areg::LogEntry entry;
    const uint32_t lengths[]{ 0u, 1u, maxLen - 1u, maxLen, maxLen + 1u, 100000u, 0xFFFFFFFFu };
    for (uint32_t value : lengths)
    {
        entry.logMessageLen = value;
        EXPECT_LE(areg::log_message_size(entry), maxLen) << "clamped length escaped the buffer for " << value;
    }
}
