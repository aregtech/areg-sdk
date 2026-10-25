/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
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
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <cstdarg>
#include <future>
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

namespace
{
    //! Builds a log message as a log source sends it.
    areg::MessageEnvelope _make_sent(const std::string & text)
    {
        areg::MessageEnvelope msg{ areg::make_log_message( areg::LogMessageType::MessageText, 1u, 0u, 0u
                                                         , areg::LogPriority::PrioDebug, text.c_str()
                                                         , static_cast<uint32_t>(text.length())) };
        areg::finalize_log_message(msg, areg::LogDataType::Remote, areg::COOKIE_REMOTE_SERVICE);
        return msg;
    }

    areg::LogEntry & _entry(areg::MessageEnvelope & msg)
    {
        return *reinterpret_cast<areg::LogEntry *>(msg.buffer());
    }
}

/**
 * \brief   A message built by a log source is a whole log entry, short or cut.
 **/
TEST(LogMessageLengthTest, test_sentMessageIsValid)
{
    EXPECT_TRUE(areg::is_log_entry_valid(_make_sent("")));
    EXPECT_TRUE(areg::is_log_entry_valid(_make_sent("a short message")));
    EXPECT_TRUE(areg::is_log_entry_valid(_make_sent(std::string(areg::LOG_MSG_SIZE + 100u, 'c'))));
}

/**
 * \brief   A message whose size is not the size of its entry is not a log entry.
 **/
TEST(LogMessageLengthTest, test_sizeMustMatchTheEntry)
{
    areg::MessageEnvelope msg{ _make_sent("a short message") };
    const uint32_t size{ msg.size_used() };

    msg.set_size_used(size - 1u);
    EXPECT_FALSE(areg::is_log_entry_valid(msg));

    msg.set_size_used(areg::log_entry_head() - 1u);
    EXPECT_FALSE(areg::is_log_entry_valid(msg));

    msg.set_size_used(size);
    _entry(msg).logMessageLen += 1u;
    EXPECT_FALSE(areg::is_log_entry_valid(msg));
}

/**
 * \brief   The text and the names must end where the entry says they end.
 **/
TEST(LogMessageLengthTest, test_stringsMustEnd)
{
    {
        areg::MessageEnvelope msg{ _make_sent("text") };
        _entry(msg).logMessage[areg::log_message_size(_entry(msg))] = 'x';
        EXPECT_FALSE(areg::is_log_entry_valid(msg));
    }

    {
        areg::MessageEnvelope msg{ _make_sent("text") };
        _entry(msg).logThreadLen = areg::LOG_NAME_SIZE;
        EXPECT_FALSE(areg::is_log_entry_valid(msg));
    }

    {
        areg::MessageEnvelope msg{ _make_sent("text") };
        areg::LogEntry & entry{ _entry(msg) };
        entry.logModule[entry.logModuleLen] = 'x';
        EXPECT_FALSE(areg::is_log_entry_valid(msg));
    }
}

namespace
{
    //! A thread that builds one log message while it runs.
    class NamedLogThread final : public areg::Thread, protected areg::ThreadConsumer
    {
    public:
        NamedLogThread()
            : areg::Thread  ( *this, "NamedLogThread" )
            , mMessage      ( )
            , mBuilt        ( )
        {
        }

        //! Waits until the thread built the message, then returns it.
        areg::MessageEnvelope & message()
        {
            mBuilt.get_future().wait();
            return mMessage;
        }

    protected:
        void on_run() override
        {
            mMessage = _make_sent("from a named thread");
            mBuilt.set_value();
        }

    private:
        areg::MessageEnvelope   mMessage;   //!< The message built in the thread.
        std::promise<void>      mBuilt;     //!< Set when the message is built.
    };
}

/**
 * \brief   A message carries the name of the thread that built it.
 **/
TEST(LogMessageLengthTest, test_messageCarriesThreadName)
{
    NamedLogThread thread;
    ASSERT_TRUE(thread.start(areg::WAIT_INFINITE));
    areg::MessageEnvelope & msg{ thread.message() };
    static_cast<void>(thread.shutdown(areg::WAIT_INFINITE));

    ASSERT_TRUE(areg::is_log_entry_valid(msg));
    EXPECT_STREQ(_entry(msg).logThread, "NamedLogThread");
    EXPECT_EQ(_entry(msg).logThreadLen, 14u);
}
