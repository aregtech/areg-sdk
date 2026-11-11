/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogProtocolTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the messages the log collector and the log observers
 *              exchange. They are built the same with and without logging in the build.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <cstring>

namespace
{
    constexpr ITEM_ID   SOURCE  { 1'001u };
    constexpr ITEM_ID   TARGET  { 1'002u };
    constexpr ITEM_ID   OBSERVER{ 1'003u };

    //!< Returns true if the message has the ID, the source and the target.
    bool is_message(const areg::MessageEnvelope & msg, areg::FuncIdRange id, ITEM_ID source, ITEM_ID target)
    {
        return msg.is_valid()
            && (msg.message_id() == static_cast<uint32_t>(id))
            && (msg.source() == static_cast<uint32_t>(source))
            && (msg.target() == static_cast<uint32_t>(target));
    }
}

// A log row built from a log entry is complete and carries the cookie of its sender.
TEST(LogProtocolTest, log_row_is_built)
{
    areg::LogEntry entry{ areg::LogMessageType::MessageText };
    entry.logMessagePrio = areg::LogPriority::PrioWarning;
    const char text[]{ "log protocol row" };
    entry.logMessageLen  = static_cast<uint32_t>(sizeof(text) - 1u);
    std::memcpy(entry.logMessage, text, sizeof(text));

    const areg::MessageEnvelope msg{ areg::create_log_message(entry, areg::LogDataType::Remote, SOURCE) };
    ASSERT_TRUE(msg.is_valid());
    EXPECT_EQ(msg.message_id(), static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage));
    EXPECT_EQ(msg.source(), static_cast<uint32_t>(SOURCE));
    ASSERT_TRUE(areg::is_log_entry_valid(msg));

    const areg::LogEntry * row{ reinterpret_cast<const areg::LogEntry *>(msg.buffer()) };
    EXPECT_EQ(row->logCookie, SOURCE);
    EXPECT_EQ(row->logDataType, areg::LogDataType::Remote);
    EXPECT_EQ(row->logMessagePrio, areg::LogPriority::PrioWarning);
    EXPECT_STREQ(row->logMessage, text);
}

// A scope update decodes as the scope entry the log source reads.
TEST(LogProtocolTest, scope_update_carries_the_scope)
{
    const areg::MessageEnvelope msg{ areg::message_update_scope(SOURCE, TARGET, areg::String("area_scope"), 42u, static_cast<uint32_t>(areg::LogPriority::PrioDebug)) };
    ASSERT_TRUE(is_message(msg, areg::FuncIdRange::ServiceLogUpdateScopes, SOURCE, TARGET));

    msg.move_to_begin();
    uint32_t count{ 0u };
    areg::ScopeEntry entry{};
    msg >> count >> entry;
    EXPECT_EQ(count, 1u);
    EXPECT_EQ(entry.scopeName, "area_scope");
    EXPECT_EQ(entry.scopeId, 42u);
    EXPECT_EQ(entry.scopePrio, static_cast<uint32_t>(areg::LogPriority::PrioDebug));
}

// The requests of an observer name their target.
TEST(LogProtocolTest, observer_requests_name_their_target)
{
    const struct
    {
        areg::MessageEnvelope   msg;
        areg::FuncIdRange       id;
    } requests[]
    {
          { areg::message_query_instances(SOURCE, TARGET)       , areg::FuncIdRange::SystemServiceQueryInstances    }
        , { areg::message_query_scopes(SOURCE, TARGET)          , areg::FuncIdRange::ServiceLogQueryScopes          }
        , { areg::message_save_configuration(SOURCE, TARGET)    , areg::FuncIdRange::ServiceSaveLogConfiguration    }
        , { areg::message_restore_configuration(SOURCE, TARGET) , areg::FuncIdRange::ServiceLogRestoreConfiguration }
    };

    for (const auto & request : requests)
    {
        ASSERT_TRUE(is_message(request.msg, request.id, SOURCE, TARGET)) << "message " << static_cast<uint32_t>(request.id);
        request.msg.move_to_begin();
        ITEM_ID named{ 0u };
        request.msg >> named;
        EXPECT_EQ(named, TARGET);
    }
}

// The source state messages carry the state and who asked for it.
TEST(LogProtocolTest, source_state_messages_carry_the_state)
{
    const areg::MessageEnvelope update{ areg::message_update_source_state(SOURCE, TARGET, areg::LogSourceState::Paused) };
    ASSERT_TRUE(is_message(update, areg::FuncIdRange::ServiceLogUpdateSourceState, SOURCE, TARGET));
    update.move_to_begin();
    ITEM_ID named{ 0u };
    uint8_t state{ 0u };
    update >> named >> state;
    EXPECT_EQ(named, TARGET);
    EXPECT_EQ(state, static_cast<uint8_t>(areg::LogSourceState::Paused));

    const areg::MessageEnvelope updated{ areg::message_source_state_updated(SOURCE, TARGET, areg::LogSourceState::Stopped, OBSERVER) };
    ASSERT_TRUE(is_message(updated, areg::FuncIdRange::ServiceLogSourceStateUpdated, SOURCE, TARGET));
    updated.move_to_begin();
    ITEM_ID source{ 0u };
    ITEM_ID byObserver{ 0u };
    updated >> source >> state >> byObserver;
    EXPECT_EQ(source, SOURCE);
    EXPECT_EQ(state, static_cast<uint8_t>(areg::LogSourceState::Stopped));
    EXPECT_EQ(byObserver, OBSERVER);
}

// The answers of a log source to a save and a restore go to the log collector.
TEST(LogProtocolTest, configuration_answers_go_to_the_collector)
{
    const areg::MessageEnvelope saved{ areg::message_configuration_saved() };
    ASSERT_TRUE(saved.is_valid());
    EXPECT_EQ(saved.message_id(), static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
    EXPECT_EQ(saved.target(), static_cast<uint32_t>(areg::COOKIE_LOGGER));

    const areg::MessageEnvelope restored{ areg::message_configuration_restored() };
    ASSERT_TRUE(restored.is_valid());
    EXPECT_EQ(restored.message_id(), static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationRestored));
    EXPECT_EQ(restored.target(), static_cast<uint32_t>(areg::COOKIE_LOGGER));
}

// Priority names and priority bits convert both ways.
TEST(LogProtocolTest, priority_names_convert_both_ways)
{
    const areg::String name{ areg::make_prio_string(static_cast<uint32_t>(areg::LogPriority::PrioWarning)) };
    EXPECT_EQ(name, areg::PRIO_WARNING_STR);
    EXPECT_EQ(areg::make_priorities(name), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(areg::make_priorities(areg::PRIO_DEBUG_STR), static_cast<uint32_t>(areg::LogPriority::PrioDebug));
}

#if AREG_LOGGING
// A request with an unknown source or target is not built.
TEST(LogProtocolTest, requests_with_an_unknown_end_are_not_built)
{
    const areg::ScopeNames names;
    const ITEM_ID ends[][2]{ { areg::COOKIE_UNKNOWN, TARGET }, { SOURCE, areg::COOKIE_UNKNOWN } };
    for (const auto & end : ends)
    {
        const ITEM_ID source{ end[0] };
        const ITEM_ID target{ end[1] };
        EXPECT_FALSE(areg::message_update_scopes(source, target, names).is_valid());
        EXPECT_FALSE(areg::message_update_scope(source, target, areg::String("scope"), 1u, 0u).is_valid());
        EXPECT_FALSE(areg::message_query_instances(source, target).is_valid());
        EXPECT_FALSE(areg::message_query_scopes(source, target).is_valid());
        EXPECT_FALSE(areg::message_save_configuration(source, target).is_valid());
        EXPECT_FALSE(areg::message_restore_configuration(source, target).is_valid());
        EXPECT_FALSE(areg::message_update_source_state(source, target, areg::LogSourceState::Paused).is_valid());
        EXPECT_FALSE(areg::message_source_state_updated(source, target, areg::LogSourceState::Paused, OBSERVER).is_valid());
    }
}

// A scope registration with unknown ends goes from this process to the log collector.
TEST(LogProtocolTest, scope_registration_defaults_its_ends)
{
    const areg::ScopeList scopes;
    const areg::MessageEnvelope byDefault{ areg::message_register_scopes(areg::COOKIE_UNKNOWN, areg::COOKIE_UNKNOWN, scopes) };
    ASSERT_TRUE(byDefault.is_valid());
    EXPECT_EQ(byDefault.target(), static_cast<uint32_t>(areg::COOKIE_LOGGER));
    EXPECT_EQ(byDefault.source(), static_cast<uint32_t>(areg::cookie()));

    const areg::MessageEnvelope named{ areg::message_register_scopes(SOURCE, TARGET, scopes) };
    EXPECT_TRUE(is_message(named, areg::FuncIdRange::ServiceLogRegisterScopes, SOURCE, TARGET));
}

// Finalizing an invalid message changes nothing; a log entry assigned to itself keeps its text.
TEST(LogProtocolTest, finalize_of_an_invalid_message_and_self_assignment_keep_state)
{
    areg::MessageEnvelope invalid;
    areg::finalize_log_message(invalid, areg::LogDataType::Remote, SOURCE);
    EXPECT_FALSE(invalid.is_valid());

    areg::LogEntry entry{ areg::LogMessageType::MessageText };
    entry.logMessageLen = 4u;
    std::memcpy(entry.logMessage, "text", 5u);
    areg::LogEntry & same{ entry };
    entry = same;
    EXPECT_STREQ(entry.logMessage, "text");
    EXPECT_EQ(entry.logMessageLen, 4u);
}
#endif  // AREG_LOGGING
