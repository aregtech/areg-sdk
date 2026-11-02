/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/private/LayoutManager.hpp"
#include "areg/logging/private/Layouts.hpp"

#include <cstring>
#include <string>
#include <utility>

#if AREG_LOGGING
namespace
{
    areg::LogEntry fixed_entry()
    {
        areg::LogEntry entry;
        entry.logDataType = areg::LogDataType::Remote;
        entry.logCookie = 42u;
        entry.logModuleId = 0xABCDEFu;
        entry.logThreadId = 123u;
        entry.logScopeId = 456u;
        entry.logMessagePrio = areg::LogPriority::PrioWarning;
        std::strcpy(entry.logModule, "remote_module");
        entry.logModuleLen = 13u;
        std::strcpy(entry.logThread, "worker");
        entry.logThreadLen = 6u;
        std::strcpy(entry.logMessage, "payload");
        entry.logMessageLen = 7u;
        return entry;
    }

    std::string output(const areg::SharedBuffer& buffer)
    {
        return buffer.size_used() == 0u ? std::string()
            : std::string(reinterpret_cast<const char*>(buffer.buffer()), buffer.size_used());
    }

    std::string render(const char* format, const areg::LogEntry& entry)
    {
        areg::LayoutManager manager;
        EXPECT_TRUE(manager.create_layouts(format));
        areg::SharedBuffer buffer;
        manager.log_message(entry, buffer);
        return output(buffer);
    }

    template<typename Layout>
    void check_copies(areg::LayoutToken token, const areg::LogEntry& entry, const std::string& expected)
    {
        Layout original;
        Layout copied{ original };
        Layout moved{ std::move(copied) };
        for (const Layout* layout : { &original, &moved })
        {
            EXPECT_EQ(layout->layout_type(), token);
            areg::SharedBuffer buffer;
            layout->log_message(entry, buffer);
            EXPECT_EQ(output(buffer), expected);
        }
    }
}

// Layout specifiers render the fixed message's fields in the requested literal order.
TEST(LayoutTest, fixed_message_specifiers_preserve_fields_and_literal_order)
{
    EXPECT_EQ(render("prefix|%a|%e|%t|%s|%x|%y|%p|%m|%n", fixed_entry())
              , "prefix|042|0xABCDEF|000123|456|remote_module|worker|WARN|payload|\n");
}

// Scope-name output uses the declared length rather than trailing message bytes.
TEST(LayoutTest, scope_name_uses_the_declared_message_length)
{
    areg::LogEntry entry{ fixed_entry() };
    entry.logMsgType = areg::LogMessageType::ScopeEnter;
    entry.logMessageLen = 4u;
    EXPECT_EQ(render("[%z]", entry), "[payl]");
}

// Missing identifiers and names produce the documented empty or fallback output.
TEST(LayoutTest, missing_identifiers_and_names_have_defined_output)
{
    areg::LogEntry entry{ fixed_entry() };
    entry.logModuleId = 0u;
    entry.logThreadId = 0u;
    entry.logScopeId = 0u;
    entry.logModuleLen = 0u;
    entry.logThreadLen = 0u;
    entry.logMessagePrio = areg::LogPriority::PrioInvalid;
    entry.logMessage[0] = '\0';
    entry.logMessageLen = 0u;
    EXPECT_EQ(render("%e|%t|%s|%x|%y|%p|%m", entry), "|||Unknown_Module|Unknown_Thread||");
}

// Local module fields are resolved from the current process.
TEST(LayoutTest, local_module_name_and_identifier_are_resolved_from_the_process)
{
    areg::LogEntry entry{ fixed_entry() };
    entry.logDataType = areg::LogDataType::Local;
    entry.logModuleId = static_cast<ITEM_ID>(areg::Process::instance().id());
#ifdef _BIT64
    const areg::String expectedId{ areg::String::make_string(static_cast<uint64_t>(entry.logModuleId), areg::Radix::Hexadecimal) };
#else   // _BIT32
    const areg::String expectedId{ areg::String::make_string(static_cast<uint32_t>(entry.logModuleId), areg::Radix::Hexadecimal) };
#endif  // _BIT64
    EXPECT_EQ(render("%e", entry), expectedId.as_string());
    EXPECT_EQ(render("%x", entry), areg::Process::instance().app_name().as_string());
    entry.logDataType = areg::LogDataType::Remote;
    entry.logCookie = areg::COOKIE_LOCAL;
    EXPECT_EQ(render("%x", entry), areg::Process::instance().app_name().as_string());
}

// Timestamp output uses the entry's timestamp and stays empty for zero.
TEST(LayoutTest, timestamp_is_formatted_from_the_entry_and_zero_is_empty)
{
    areg::LogEntry entry{ fixed_entry() };
    entry.logTimestamp = 1'700'000'000'123'000ULL;
    const areg::String expected{ areg::DateTime(entry.logTimestamp).format_time(areg::TIME_FORMAT_ISO8601_OUTPUT) };
    EXPECT_EQ(render("[%d]", entry), "[" + std::string(expected.as_string()) + "]");
    entry.logTimestamp = 0u;
    EXPECT_EQ(render("[%d]", entry), "[]");
}

// The rendered tick count falls between the observations before and after rendering.
TEST(LayoutTest, tick_count_is_monotonic_and_inside_the_observed_interval)
{
    const uint64_t before{ areg::DateTime::process_tick_count() };
    const std::string text{ render("%c", fixed_entry()) };
    const uint64_t after{ areg::DateTime::process_tick_count() };
    ASSERT_FALSE(text.empty());
    uint64_t value{ 0u };
    for (char digit : text)
    {
        ASSERT_GE(digit, '0');
        ASSERT_LE(digit, '9');
        value = value * 10u + static_cast<uint64_t>(digit - '0');
    }
    EXPECT_GE(value, before);
    EXPECT_LE(value, after);
}

// An empty or null format clears any previously configured layouts.
TEST(LayoutTest, empty_and_null_formats_clear_previous_layouts)
{
    areg::LayoutManager manager;
    EXPECT_FALSE(manager.is_valid());
    ASSERT_TRUE(manager.create_layouts("old"));
    EXPECT_FALSE(manager.create_layouts(static_cast<const char*>(nullptr)));
    EXPECT_FALSE(manager.is_valid());
    ASSERT_TRUE(manager.create_layouts(areg::String("%m")));
    EXPECT_FALSE(manager.create_layouts(areg::String()));
    areg::SharedBuffer buffer;
    manager.log_message(fixed_entry(), buffer);
    EXPECT_EQ(buffer.size_used(), 0u);
    manager.delete_layouts();
    manager.delete_layouts();
    EXPECT_FALSE(manager.is_valid());
}

// Unknown and incomplete format specifiers are preserved as literal text.
TEST(LayoutTest, unknown_and_unfinished_specifiers_remain_literal)
{
    EXPECT_EQ(render("a%qb%", fixed_entry()), "a%qb%");
    EXPECT_EQ(render("%%", fixed_entry()), "%");
}

// Ignoring the layout writes only the message bytes within its declared length.
TEST(LayoutTest, ignore_layout_writes_only_the_bounded_message)
{
    areg::LogEntry entry{ fixed_entry() };
    entry.logMessagePrio = areg::LogPriority::PrioIgnoreLayout;
    entry.logMessageLen = 3u;
    EXPECT_EQ(render("prefix%p%m%n", entry), "pay");
    areg::LayoutManager empty;
    areg::SharedBuffer buffer;
    empty.log_message(entry, buffer);
    EXPECT_EQ(output(buffer), "pay");
}

// Copied and moved layouts retain their token type and rendered output.
TEST(LayoutTest, copied_and_moved_layouts_keep_their_type_and_output)
{
    const areg::LogEntry entry{ fixed_entry() };
    check_copies<areg::DayTimeLayout>(areg::LayoutToken::DayTime, entry, "");
    check_copies<areg::ModuleIdLayout>(areg::LayoutToken::ExecutableId, entry, "0xABCDEF");
    check_copies<areg::MessageLayout>(areg::LayoutToken::Message, entry, "payload");
    check_copies<areg::EndOfLineLayout>(areg::LayoutToken::EndOfLine, entry, "\n");
    check_copies<areg::PriorityLayout>(areg::LayoutToken::Priority, entry, "WARN");
    check_copies<areg::ScopeIdLayout>(areg::LayoutToken::ScopeId, entry, "456");
    check_copies<areg::ThreadIdLayout>(areg::LayoutToken::ThreadId, entry, "000123");
    check_copies<areg::ModuleNameLayout>(areg::LayoutToken::ExecutableName, entry, "remote_module");
    check_copies<areg::ThreadNameLayout>(areg::LayoutToken::ThreadName, entry, "worker");
    check_copies<areg::ScopeNameLayout>(areg::LayoutToken::ScopeName, entry, "payload");
    check_copies<areg::CookieIdLayout>(areg::LayoutToken::CookieId, entry, "042");
    areg::TickCountLayout tick;
    areg::TickCountLayout copied{ tick };
    areg::TickCountLayout moved{ std::move(copied) };
    EXPECT_EQ(moved.layout_type(), areg::LayoutToken::TickCount);
}

// Literal layouts preserve their text through copies and moves and accept null input.
TEST(LayoutTest, literal_layouts_copy_move_and_accept_null_text)
{
    const areg::LogEntry entry{ fixed_entry() };
    areg::AnyTextLayout empty;
    areg::AnyTextLayout nullText{ static_cast<const char*>(nullptr) };
    areg::AnyTextLayout original{ areg::String("literal") };
    areg::AnyTextLayout copied{ original };
    areg::AnyTextLayout moved{ std::move(copied) };
    areg::SharedBuffer buffer;
    empty.log_message(entry, buffer);
    nullText.log_message(entry, buffer);
    EXPECT_EQ(buffer.size_used(), 0u);
    original.log_message(entry, buffer);
    moved.log_message(entry, buffer);
    EXPECT_EQ(output(buffer), "literalliteral");
}

// A repeated exclusive specifier is consumed and ignored; the text around it is kept.
TEST(LayoutTest, repeated_message_specifier_does_not_hang)
{
    EXPECT_EQ(render("%m%m", fixed_entry()), "payload");
}

// An escaped percent sign does not duplicate the literal text before it.
TEST(LayoutTest, escaped_percent_does_not_duplicate_preceding_literal)
{
    EXPECT_EQ(render("left%%right", fixed_entry()), "left%right");
}

// A format at the stack-buffer boundary must preserve all text without overrunning storage.
TEST(LayoutTest, string_layout_at_stack_buffer_boundary_preserves_all_text)
{
    const std::string text(512u, 'x');
    areg::LayoutManager manager;
    ASSERT_TRUE(manager.create_layouts(areg::String(text)));
    areg::SharedBuffer buffer;
    manager.log_message(fixed_entry(), buffer);
    EXPECT_EQ(output(buffer), text);
}

// An escaped percent sign renders once in every position, and other literal percent signs are kept.
TEST(LayoutTest, escaped_percent_renders_once_in_every_position)
{
    const areg::LogEntry entry{ fixed_entry() };
    EXPECT_EQ(render("%%", entry), "%");
    EXPECT_EQ(render("%%left", entry), "%left");
    EXPECT_EQ(render("right%%", entry), "right%");
    EXPECT_EQ(render("a%%b%%c", entry), "a%b%c");
    EXPECT_EQ(render("%%%%", entry), "%%");
    EXPECT_EQ(render("a%%%m", entry), "a%payload");
    EXPECT_EQ(render("%m%%b", entry), "payload%b");
    EXPECT_EQ(render("%%m", entry), "%m");
    EXPECT_EQ(render("100%", entry), "100%");
    EXPECT_EQ(render("a%qb", entry), "a%qb");
}

// Only the first exclusive specifier creates a layout; later ones are dropped in place.
TEST(LayoutTest, later_exclusive_specifiers_are_dropped_and_text_is_kept)
{
    EXPECT_EQ(render("a%mb%mc", fixed_entry()), "apayloadbc");
    EXPECT_EQ(render("%m%z", fixed_entry()), "payload");
    EXPECT_EQ(render("<%z>%m", fixed_entry()), "<payload>");
}

// Layouts longer than the stack buffer and below it render the same through both overloads.
TEST(LayoutTest, long_layouts_render_all_text_through_both_overloads)
{
    for (const std::size_t length : { std::size_t{ 511u }, std::size_t{ 4096u } })
    {
        const std::string text(length, 'y');
        const std::string format{ text + "%m" };
        areg::LayoutManager byString;
        areg::LayoutManager byPointer;
        ASSERT_TRUE(byString.create_layouts(areg::String(format)));
        ASSERT_TRUE(byPointer.create_layouts(format.c_str()));
        areg::SharedBuffer first;
        areg::SharedBuffer second;
        byString.log_message(fixed_entry(), first);
        byPointer.log_message(fixed_entry(), second);
        EXPECT_EQ(output(first), text + "payload");
        EXPECT_EQ(output(second), text + "payload");
    }

    areg::LayoutManager empty;
    EXPECT_FALSE(empty.create_layouts(areg::String()));
    EXPECT_FALSE(empty.create_layouts(static_cast<const char*>(nullptr)));
}
#endif
