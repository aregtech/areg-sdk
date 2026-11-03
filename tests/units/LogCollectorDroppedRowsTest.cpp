/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogCollectorDroppedRowsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the log rows the log collector drops for held
 *              observers and of the warning rows that report them.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "logcollector/service/private/LogCollectorDroppedRows.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace
{
    constexpr ITEM_ID   OBSERVER_A  { 300u };
    constexpr ITEM_ID   OBSERVER_B  { 301u };
    constexpr ITEM_ID   SOURCE_1    { 400u };
    constexpr ITEM_ID   SOURCE_2    { 401u };
    constexpr uint32_t  SCOPE_ID    { 7u };

    areg::MessageEnvelope make_row(ITEM_ID source, uint32_t scopeId, const char * text, areg::LogMessageType type = areg::LogMessageType::MessageText)
    {
        const areg::LogEntry entry(type, scopeId, 0u, 0u, areg::LogPriority::PrioError, text, static_cast<uint32_t>(std::strlen(text)));
        return areg::create_log_message(entry, areg::LogDataType::Remote, source);
    }

    areg::MessageEnvelope data_row(ITEM_ID source)
    {
        return make_row(source, SCOPE_ID, "a row of the source");
    }

    areg::MessageEnvelope queue_notice(ITEM_ID source, unsigned long long count)
    {
        char text[areg::LOG_MSG_SIZE];
        std::snprintf(text, sizeof(text), "Dropped [ %llu ] log messages, the logging queue was full.", count);
        return make_row(source, areg::LOG_SCOPE_ID_NONE, text);
    }

    struct Sent
    {
        ITEM_ID             target  { 0u };
        ITEM_ID             source  { 0u };
        ITEM_ID             logSource{ 0u };
        ITEM_ID             cookie  { 0u };
        areg::LogPriority   prio    { areg::LogPriority::PrioInvalid };
        std::string         text    { };
    };

    struct Collect
    {
        std::vector<Sent> & mSent;

        void operator () (areg::MessageEnvelope && msg) const
        {
            const areg::LogEntry & entry{ *reinterpret_cast<const areg::LogEntry *>(msg.buffer()) };
            mSent.push_back(Sent{ static_cast<ITEM_ID>(msg.target()), static_cast<ITEM_ID>(msg.source()), entry.logSource, entry.logCookie, entry.logMessagePrio, entry.logMessage });
        }
    };

    // The number of rows the warning rows report: N of 'Dropped [ N ]' plus the rows of the log collector.
    uint64_t reported(const std::vector<Sent> & sent)
    {
        uint64_t sum{ 0u };
        for (const Sent & row : sent)
        {
            unsigned long long count{ 0u };
#if defined(_MSC_VER)
            if ((::sscanf_s(row.text.c_str(), "Dropped [ %llu ]", &count) == 1) ||
                (::sscanf_s(row.text.c_str(), "The log collector dropped %llu", &count) == 1))
#else
            if ((std::sscanf(row.text.c_str(), "Dropped [ %llu ]", &count) == 1) ||
                (std::sscanf(row.text.c_str(), "The log collector dropped %llu", &count) == 1))
#endif
            {
                sum += count;
            }
        }

        return sum;
    }
}

/**
 * \brief   N is read from the 'Dropped [ N ]' rows that a log source writes, up to 64 bits.
 **/
TEST(LogCollectorDroppedRowsTest, read_dropped_notice_accepts_source_notices)
{
    uint64_t count{ 0u };
    EXPECT_TRUE(LogCollectorDroppedRows::read_dropped_notice(queue_notice(SOURCE_1, 803u), count));
    EXPECT_EQ(count, 803u);

    const areg::MessageEnvelope ring{ make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ 12 ] log messages, the application was not connected to the log collector.") };
    EXPECT_TRUE(LogCollectorDroppedRows::read_dropped_notice(ring, count));
    EXPECT_EQ(count, 12u);

    EXPECT_TRUE(LogCollectorDroppedRows::read_dropped_notice(queue_notice(SOURCE_1, 18446744073709551615ull), count));
    EXPECT_EQ(count, 18446744073709551615ull);
}

/**
 * \brief   A row that is not a 'Dropped [ N ]' notice without a scope is not read as one.
 **/
TEST(LogCollectorDroppedRowsTest, read_dropped_notice_rejects_other_rows)
{
    uint64_t count{ 99u };
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, SCOPE_ID, "Dropped [ 5 ] log messages."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [5 ] log messages."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [  ] log messages."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ 12"), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ "), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, ""), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "The log collector dropped 5 log messages for this observer."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ 5 ] rows", areg::LogMessageType::ScopeEnter), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ 123456789012345678901 ] log messages."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(make_row(SOURCE_1, areg::LOG_SCOPE_ID_NONE, "Dropped [ 18446744073709551616 ] log messages."), count));
    EXPECT_FALSE(LogCollectorDroppedRows::read_dropped_notice(areg::MessageEnvelope(), count));
    EXPECT_EQ(count, 99u);
}

/**
 * \brief   Reproduces the defect: a source's 'Dropped [ N ]' notice dropped for a held observer
 *          was counted as one row, so its N rows went missing without a notice. The observer
 *          gets the N of the source in a row of that source, and the rows of the log collector.
 **/
TEST(LogCollectorDroppedRowsTest, held_source_notice_keeps_its_count)
{
    LogCollectorDroppedRows rows;
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_1, 803u));
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));

    std::vector<Sent> sent;
    ASSERT_TRUE(rows.release(OBSERVER_A, Collect{ sent }));
    ASSERT_EQ(sent.size(), 2u);
    EXPECT_EQ(reported(sent), 3u + 803u);

    EXPECT_EQ(sent[0].text.rfind("Dropped [ 803 ] log messages", 0), 0u);
    EXPECT_EQ(sent[0].target, OBSERVER_A);
    EXPECT_EQ(sent[0].source, SOURCE_1);
    EXPECT_EQ(sent[0].logSource, SOURCE_1);
    EXPECT_EQ(sent[0].cookie, SOURCE_1);
    EXPECT_EQ(sent[0].prio, areg::LogPriority::PrioError);

    EXPECT_EQ(sent[1].text.rfind("The log collector dropped 3 log messages", 0), 0u);
    EXPECT_EQ(sent[1].target, OBSERVER_A);
    EXPECT_EQ(sent[1].source, areg::COOKIE_LOGGER);
    EXPECT_EQ(sent[1].prio, areg::LogPriority::PrioWarning);
    EXPECT_TRUE(rows.is_empty());
}

/**
 * \brief   The notices of one source are summed into one row; sources and observers are kept apart.
 **/
TEST(LogCollectorDroppedRowsTest, notices_are_summed_per_source_and_observer)
{
    LogCollectorDroppedRows rows;
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_1, 10u));
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_2, 5u));
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_1, 20u));
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_2));
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_B, queue_notice(SOURCE_1, 7u));

    std::vector<Sent> sent;
    ASSERT_TRUE(rows.release(OBSERVER_A, Collect{ sent }));
    ASSERT_EQ(sent.size(), 3u);
    EXPECT_EQ(sent[0].source, SOURCE_1);
    EXPECT_EQ(sent[0].text.rfind("Dropped [ 30 ]", 0), 0u);
    EXPECT_EQ(sent[1].source, SOURCE_2);
    EXPECT_EQ(sent[1].text.rfind("Dropped [ 5 ]", 0), 0u);
    EXPECT_EQ(sent[2].text.rfind("The log collector dropped 2 ", 0), 0u);
    EXPECT_EQ(reported(sent), 37u);

    EXPECT_FALSE(rows.is_empty());
    EXPECT_TRUE(rows.contains(OBSERVER_B));
    sent.clear();
    ASSERT_TRUE(rows.release(OBSERVER_B, Collect{ sent }));
    ASSERT_EQ(sent.size(), 1u);
    EXPECT_EQ(sent[0].target, OBSERVER_B);
    EXPECT_EQ(sent[0].text.rfind("Dropped [ 7 ]", 0), 0u);
}

/**
 * \brief   An observer with only dropped notices gets no row of the log collector.
 **/
TEST(LogCollectorDroppedRowsTest, notices_only_send_no_collector_row)
{
    LogCollectorDroppedRows rows;
    EXPECT_TRUE(rows.is_empty());
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_1, 4u));
    EXPECT_FALSE(rows.is_empty());
    EXPECT_TRUE(rows.contains(OBSERVER_A));

    std::vector<Sent> sent;
    ASSERT_TRUE(rows.release(OBSERVER_A, Collect{ sent }));
    ASSERT_EQ(sent.size(), 1u);
    EXPECT_EQ(sent[0].text.rfind("Dropped [ 4 ]", 0), 0u);
    EXPECT_TRUE(rows.is_empty());
}

/**
 * \brief   release_all() releases only the observers that are no longer held and reports whether
 *          one is left.
 **/
TEST(LogCollectorDroppedRowsTest, release_all_keeps_held_observers)
{
    LogCollectorDroppedRows rows;
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_B, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_B, queue_notice(SOURCE_2, 6u));

    std::vector<Sent> sent;
    EXPECT_TRUE(rows.release_all([](const ITEM_ID & observer) { return observer == OBSERVER_A; }, Collect{ sent }));
    ASSERT_EQ(sent.size(), 2u);
    EXPECT_EQ(sent[0].target, OBSERVER_B);
    EXPECT_EQ(sent[1].target, OBSERVER_B);
    EXPECT_EQ(reported(sent), 7u);
    EXPECT_TRUE(rows.contains(OBSERVER_A));
    EXPECT_FALSE(rows.contains(OBSERVER_B));

    sent.clear();
    EXPECT_FALSE(rows.release_all([](const ITEM_ID &) { return false; }, Collect{ sent }));
    ASSERT_EQ(sent.size(), 1u);
    EXPECT_EQ(sent[0].target, OBSERVER_A);
    EXPECT_TRUE(rows.is_empty());
}

/**
 * \brief   A disconnected observer is forgotten with its notices; an unknown observer gets nothing.
 **/
TEST(LogCollectorDroppedRowsTest, remove_forgets_the_observer)
{
    LogCollectorDroppedRows rows;
    rows.count_dropped(OBSERVER_A, data_row(SOURCE_1));
    rows.count_dropped(OBSERVER_A, queue_notice(SOURCE_1, 9u));
    rows.count_dropped(OBSERVER_B, queue_notice(SOURCE_1, 3u));
    rows.remove(OBSERVER_A);

    std::vector<Sent> sent;
    EXPECT_FALSE(rows.release(OBSERVER_A, Collect{ sent }));
    EXPECT_TRUE(sent.empty());
    ASSERT_TRUE(rows.release(OBSERVER_B, Collect{ sent }));
    ASSERT_EQ(sent.size(), 1u);
    EXPECT_EQ(sent[0].text.rfind("Dropped [ 3 ]", 0), 0u);
    EXPECT_FALSE(rows.release(OBSERVER_B, Collect{ sent }));
    EXPECT_EQ(sent.size(), 1u);
}
