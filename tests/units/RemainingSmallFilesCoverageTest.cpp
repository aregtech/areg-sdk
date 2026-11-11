#include "units/GUnitTest.hpp"
#include "areg/base/FatalError.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/component/ComponentAddress.hpp"
#include "areg/component/DispatcherThread.hpp"

#include <array>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#if defined(POSIX)
    #include <sys/resource.h>
#endif

namespace
{
    using OpenMode = areg::FileBase::OpenMode;
    constexpr uint32_t BINARY_MODE
    {
        static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary)
    };
    constexpr uint32_t TEXT_MODE
    {
        static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Text)
    };

    void reset_stream(const areg::InStream& stream)
    {
        stream.reset();
    }

    std::wstring supplementary_text()
    {
        if constexpr (sizeof(wchar_t) == 4u)
        {
            return std::wstring(1u, static_cast<wchar_t>(0x1F600u));
        }
        else
        {
            return std::wstring{ static_cast<wchar_t>(0xD83Du), static_cast<wchar_t>(0xDE00u) };
        }
    }

    bool set_lock_failure_mode(const char* value)
    {
#if defined(WINDOWS)
        return ::_putenv_s("AREG_LOCK_FAILURE_FATAL", value) == 0;
#else
        return ::setenv("AREG_LOCK_FAILURE_FATAL", value, 1) == 0;
#endif
    }

    void disable_core_dump()
    {
        static_cast<void>(std::signal(SIGABRT, SIG_DFL));
#if defined(POSIX)
        const struct rlimit limit{ 0, 0 };
        static_cast<void>(::setrlimit(RLIMIT_CORE, &limit));
#endif
    }

    [[noreturn]] void exiting_fatal_handler(const char* reason) noexcept
    {
        std::fprintf(stderr, "fatal callback received: %s\n", reason != nullptr ? reason : "null");
        std::exit(73);
    }
}

class RemainingSmallFilesCoverageTest : public ::testing::Test
{
protected:
    RemainingSmallFilesCoverageTest()
        : mFile(BINARY_MODE)
        , mWriter(mFile, mFile)
        , mReader(mFile, mFile)
    {
    }

    void SetUp() override
    {
        ASSERT_TRUE(mFile.open());
    }

    areg::FileBuffer mFile;
    areg::WriteConverter mWriter;
    areg::ReadConverter mReader;
};

TEST_F(RemainingSmallFilesCoverageTest, converter_narrow_lines_add_one_newline_without_a_terminator)
{
    ASSERT_TRUE(mWriter.write_line("alpha"));
    ASSERT_TRUE(mWriter.write_line(""));
    ASSERT_EQ(mFile.length(), 7u);
    reset_stream(mFile);
    std::array<uint8_t, 7u> bytes{};
    ASSERT_EQ(mFile.read(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    EXPECT_EQ(std::memcmp(bytes.data(), "alpha\n\n", bytes.size()), 0);
}

TEST_F(RemainingSmallFilesCoverageTest, converter_wide_lines_add_utf16_newlines_without_a_terminator)
{
    ASSERT_TRUE(mWriter.write_line(L"A"));
    ASSERT_TRUE(mWriter.write_line(L""));
    ASSERT_EQ(mFile.length(), 6u);
    reset_stream(mFile);
    std::array<uint8_t, 6u> bytes{};
    ASSERT_EQ(mFile.read(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    const std::array<uint8_t, 6u> expected{ 'A', 0u, '\n', 0u, '\n', 0u };
    EXPECT_EQ(bytes, expected);
}

TEST_F(RemainingSmallFilesCoverageTest, converter_null_lines_fail_without_writing)
{
    ASSERT_TRUE(mWriter.set_int(123));
    const uint32_t before{ mFile.position() };
    EXPECT_FALSE(mWriter.write_line(static_cast<const char*>(nullptr)));
    EXPECT_FALSE(mWriter.write_line(static_cast<const wchar_t*>(nullptr)));
    EXPECT_EQ(mFile.position(), before);
    EXPECT_EQ(mFile.length(), before);
}

TEST(RemainingSmallFilesTest, converter_line_and_terminator_writes_report_closed_stream_failure)
{
    areg::FileBuffer closed(BINARY_MODE);
    areg::WriteConverter writer(closed, closed);
    EXPECT_FALSE(writer.write_line("text"));
    EXPECT_FALSE(writer.write_line(""));
    EXPECT_FALSE(writer.write_line(L"text"));
    EXPECT_FALSE(writer.write_line(L""));
    EXPECT_FALSE(writer.set_string_end(false));
    EXPECT_FALSE(writer.set_string_end(true));
    EXPECT_FALSE(closed.is_opened());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_explicit_terminators_separate_narrow_and_wide_strings)
{
    ASSERT_TRUE(mWriter.append_string("first"));
    ASSERT_TRUE(mWriter.set_string_end(false));
    ASSERT_TRUE(mWriter.append_string(L"second"));
    ASSERT_TRUE(mWriter.set_string_end(true));
    ASSERT_TRUE(mWriter.set_int(99));
    EXPECT_EQ(mFile.length(), 24u);
    reset_stream(mFile);
    areg::String narrow;
    areg::WideString wide;
    int32_t tail{ 0 };
    ASSERT_TRUE(mReader.as_string(narrow));
    EXPECT_EQ(narrow, areg::String("first"));
    ASSERT_TRUE(mReader.as_string(wide));
    EXPECT_EQ(wide, areg::WideString(L"second"));
    ASSERT_TRUE(mReader.as_int(tail));
    EXPECT_EQ(tail, 99);
    EXPECT_TRUE(mFile.is_eof());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_narrow_cr_lookahead_preserves_the_next_line)
{
    ASSERT_TRUE(mWriter.append_string("one\rtwo\r"));
    reset_stream(mFile);
    areg::String line;
    ASSERT_TRUE(mReader.read_line(line));
    EXPECT_EQ(line, areg::String("one"));
    EXPECT_EQ(mFile.position(), 4u);
    line.clear();
    ASSERT_TRUE(mReader.read_line(line));
    EXPECT_EQ(line, areg::String("two"));
    EXPECT_TRUE(mFile.is_eof());
    line.clear();
    EXPECT_FALSE(mReader.read_line(line));
    EXPECT_TRUE(line.is_empty());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_wide_cr_lookahead_preserves_a_surrogate_pair)
{
    const std::wstring tail{ supplementary_text() + L"tail" };
    ASSERT_TRUE(mWriter.append_string((std::wstring(L"one\r") + tail + L"\r").c_str()));
    reset_stream(mFile);
    areg::WideString line;
    ASSERT_TRUE(mReader.read_line(line));
    EXPECT_EQ(line, areg::WideString(L"one"));
    EXPECT_EQ(mFile.position(), 8u);
    line.clear();
    ASSERT_TRUE(mReader.read_line(line));
    EXPECT_EQ(line, areg::WideString(tail.c_str()));
    EXPECT_TRUE(mFile.is_eof());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_supplementary_strings_round_trip_with_exact_wire_size)
{
    const std::wstring source{ std::wstring(L"A") + supplementary_text() + L"Z" };
    ASSERT_TRUE(mWriter.set_string(source.c_str()));
    ASSERT_EQ(mFile.length(), 10u);
    reset_stream(mFile);
    const std::array<uint8_t, 10u> expected{ 'A', 0u, 0x3Du, 0xD8u, 0x00u, 0xDEu, 'Z', 0u, 0u, 0u };
    std::array<uint8_t, 10u> bytes{};
    ASSERT_EQ(mFile.read(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    EXPECT_EQ(bytes, expected);
    reset_stream(mFile);
    areg::WideString result;
    ASSERT_TRUE(mReader.as_string(result));
    EXPECT_EQ(result, areg::WideString(source.c_str()));
    EXPECT_TRUE(mFile.is_eof());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_unpaired_high_surrogate_does_not_consume_the_following_unit)
{
    const std::array<uint8_t, 6u> bytes{ 0x3Du, 0xD8u, 'Z', 0u, 0u, 0u };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    reset_stream(mFile);
    areg::WideString result;
    ASSERT_TRUE(mReader.as_string(result));
    ASSERT_EQ(result.length(), 2u);
    EXPECT_EQ(result[0], static_cast<wchar_t>(0xD83Du));
    EXPECT_EQ(result[1], L'Z');
    EXPECT_TRUE(mFile.is_eof());
}

TEST_F(RemainingSmallFilesCoverageTest, converter_partial_utf16_unit_keeps_the_unread_byte)
{
    const std::array<uint8_t, 3u> bytes{ 'A', 0u, 0x7Fu };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    reset_stream(mFile);
    areg::WideString result;
    ASSERT_TRUE(mReader.as_string(result));
    EXPECT_EQ(result, areg::WideString(L"A"));
    EXPECT_EQ(mFile.position(), 2u);
    char tail{ 0 };
    ASSERT_TRUE(mReader.as_char(tail));
    EXPECT_EQ(tail, '\x7F');
}

TEST_F(RemainingSmallFilesCoverageTest, converter_terminal_high_surrogate_is_returned_without_reading_past_eof)
{
    const std::array<uint8_t, 2u> bytes{ 0x3Du, 0xD8u };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    reset_stream(mFile);
    areg::WideString result;
    ASSERT_TRUE(mReader.as_string(result));
    ASSERT_EQ(result.length(), 1u);
    EXPECT_EQ(result[0], static_cast<wchar_t>(0xD83Du));
    EXPECT_TRUE(mFile.is_eof());
    EXPECT_FALSE(mReader.as_string(result));
    EXPECT_TRUE(result.is_empty());
}

TEST_F(RemainingSmallFilesCoverageTest, readable_byte_bound_tracks_cursor_and_closed_state)
{
    const areg::InStream& input{ mFile };
    EXPECT_EQ(input.bounded_count(100u), 0u);
    ASSERT_TRUE(mFile.write_int(123));
    reset_stream(mFile);
    EXPECT_EQ(input.bounded_count(100u), 4u);
    EXPECT_EQ(input.bounded_count(2u), 2u);
    EXPECT_EQ(mFile.set_position(1, areg::Cursor::SeekOrigin::Begin), 1u);
    EXPECT_EQ(input.bounded_count(100u), 3u);
    mFile.close();
    EXPECT_EQ(input.bounded_count(100u), 0u);
}

TEST_F(RemainingSmallFilesCoverageTest, reserve_shrink_truncates_data_and_clamps_the_cursor)
{
    const std::array<uint8_t, 6u> bytes{ 1u, 2u, 3u, 4u, 5u, 6u };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    EXPECT_EQ(mFile.reserve(3u), 3u);
    EXPECT_EQ(mFile.length(), 3u);
    EXPECT_EQ(mFile.position(), 3u);
    reset_stream(mFile);
    std::array<uint8_t, 6u> result{};
    EXPECT_EQ(mFile.read(result.data(), static_cast<uint32_t>(result.size())), 3u);
    EXPECT_EQ(result[0], 1u);
    EXPECT_EQ(result[1], 2u);
    EXPECT_EQ(result[2], 3u);
    EXPECT_EQ(result[3], 0u);
    EXPECT_EQ(mFile.reserve(0u), 0u);
    EXPECT_EQ(mFile.length(), 0u);
}

TEST(RemainingSmallFilesTest, create_mode_allocates_an_empty_writable_memory_file)
{
    areg::FileBuffer file(BINARY_MODE | static_cast<uint32_t>(OpenMode::Create));
    ASSERT_TRUE(file.open());
    EXPECT_TRUE(file.is_valid());
    EXPECT_EQ(file.length(), 0u);
    EXPECT_EQ(file.position(), 0u);
    EXPECT_TRUE(file.can_write());
    ASSERT_TRUE(file.write_int(7));
    EXPECT_EQ(file.length(), 4u);
}

TEST(RemainingSmallFilesTest, conflicting_create_exist_flags_normalize_to_open_always)
{
    const uint32_t mode{ static_cast<uint32_t>(OpenMode::Create) | static_cast<uint32_t>(OpenMode::Exist)
        | static_cast<uint32_t>(OpenMode::Truncate) | static_cast<uint32_t>(OpenMode::Binary) };
    areg::FileBuffer file(mode);
    ASSERT_TRUE(file.open());
    const uint32_t normalized{ file.mode() };
    EXPECT_EQ(normalized & static_cast<uint32_t>(OpenMode::Create), 0u);
    EXPECT_EQ(normalized & static_cast<uint32_t>(OpenMode::Exist), 0u);
    EXPECT_NE(normalized & static_cast<uint32_t>(OpenMode::OpenAlways), 0u);
    EXPECT_TRUE(file.can_write());
    EXPECT_TRUE(file.can_read());
    ASSERT_TRUE(file.write_int(19));
    reset_stream(file);
    int32_t value{ 0 };
    ASSERT_TRUE(file.read_int(value));
    EXPECT_EQ(value, 19);
}

TEST(RemainingSmallFilesTest, direct_memory_file_mode_retains_read_and_write_access)
{
    areg::FileBuffer file(TEXT_MODE | static_cast<uint32_t>(OpenMode::WriteDirect));
    ASSERT_TRUE(file.open());
    EXPECT_EQ(file.mode() & static_cast<uint32_t>(OpenMode::WriteDirect), static_cast<uint32_t>(OpenMode::WriteDirect));
    ASSERT_TRUE(file.write("direct"));
    reset_stream(file);
    areg::String result;
    EXPECT_EQ(file.read_string(result), 6);
    EXPECT_EQ(result, areg::String("direct"));
}

TEST_F(RemainingSmallFilesCoverageTest, empty_serialized_buffer_consumes_only_its_size_prefix)
{
    ASSERT_TRUE(mFile.write_int(0));
    ASSERT_TRUE(mFile.write_int(42));
    areg::SharedBuffer destination;
    ASSERT_EQ(destination.write(reinterpret_cast<const uint8_t*>("old"), 3u), 3u);
    reset_stream(mFile);
    EXPECT_EQ(mFile.read(destination), sizeof(int32_t));
    EXPECT_EQ(destination.size_used(), 0u);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(mFile.position(), sizeof(int32_t));
    int32_t tail{ 0 };
    ASSERT_TRUE(mFile.read_int(tail));
    EXPECT_EQ(tail, 42);
}

TEST_F(RemainingSmallFilesCoverageTest, negative_serialized_buffer_size_rolls_back_and_invalidates_destination)
{
    ASSERT_TRUE(mFile.write_int(-1));
    areg::SharedBuffer destination;
    ASSERT_EQ(destination.write(reinterpret_cast<const uint8_t*>("old"), 3u), 3u);
    reset_stream(mFile);
    EXPECT_EQ(mFile.read(destination), 0u);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(mFile.position(), 0u);
    int32_t prefix{ 0 };
    ASSERT_TRUE(mFile.read_int(prefix));
    EXPECT_EQ(prefix, -1);
}

TEST(RemainingSmallFilesTest, resize_and_fill_rejects_a_read_only_shared_buffer_without_mutation)
{
    areg::SharedBuffer origin;
    const std::array<uint8_t, 4u> bytes{ 1u, 2u, 3u, 4u };
    ASSERT_EQ(origin.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    areg::FileBuffer file(origin);
    ASSERT_TRUE(file.open());
    ASSERT_FALSE(file.can_write());
    ASSERT_EQ(file.set_position(1, areg::Cursor::SeekOrigin::Begin), 1u);
    EXPECT_EQ(file.resize_and_fill(8u, 0xFFu), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_EQ(file.position(), 1u);
    EXPECT_EQ(file.length(), bytes.size());
    EXPECT_EQ(origin.size_used(), bytes.size());
    EXPECT_EQ(std::memcmp(origin.buffer(), bytes.data(), bytes.size()), 0);
}

TEST_F(RemainingSmallFilesCoverageTest, byte_search_skips_false_prefixes_without_losing_an_overlapping_match)
{
    const std::array<uint8_t, 5u> bytes{ 'a', 'a', 'a', 'a', 'b' };
    const std::array<uint8_t, 3u> needle{ 'a', 'a', 'b' };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    EXPECT_EQ(mFile.search_data(0u, needle.data(), static_cast<uint32_t>(needle.size())), 2u);
}

TEST_F(RemainingSmallFilesCoverageTest, absent_searches_terminate_with_a_full_final_window)
{
    const std::array<uint8_t, 6u> bytes{ 'a', 'a', 'a', 'a', 'a', 'a' };
    const std::array<uint8_t, 3u> needle{ 'a', 'a', 'b' };
    ASSERT_EQ(mFile.write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    EXPECT_EQ(mFile.search_data(0u, needle.data(), static_cast<uint32_t>(needle.size())), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_EQ(mFile.search_text(0u, "aab", true), areg::Cursor::INVALID_CURSOR_POSITION);
}

TEST(RemainingSmallFilesTest, byte_search_on_an_open_empty_file_reports_invalid_position)
{
    areg::FileBuffer file(BINARY_MODE);
    ASSERT_TRUE(file.open());
    ASSERT_TRUE(file.can_read());
    ASSERT_EQ(file.length(), 0u);
    const std::array<uint8_t, 2u> needle{ 'a', 'b' };
    EXPECT_EQ(file.search_data(0u, needle.data(), static_cast<uint32_t>(needle.size())), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_TRUE(file.is_opened());
    EXPECT_EQ(file.position(), 0u);
    EXPECT_EQ(file.length(), 0u);
}

/**
 * \brief   Reproduces the defect: searching a closed file asserted instead of failing.
 **/
TEST(RemainingSmallFilesTest, search_on_a_closed_file_fails_without_touching_it)
{
    areg::FileBuffer file(BINARY_MODE);
    ASSERT_FALSE(file.is_opened());
    const std::array<uint8_t, 2u> needle{ 'a', 'b' };
    EXPECT_EQ(file.search_data(0u, needle.data(), static_cast<uint32_t>(needle.size())), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_EQ(file.search_text(0u, "ab", true), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_EQ(file.search_text(0u, L"ab", false), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_FALSE(file.is_opened());
}

TEST(RemainingSmallFilesTest, string_write_overloads_preserve_binary_terminators)
{
    areg::FileBuffer file(BINARY_MODE);
    ASSERT_TRUE(file.open());
    ASSERT_TRUE(file.write("A"));
    ASSERT_TRUE(file.write(L"B"));
    EXPECT_EQ(file.length(), 6u);
    reset_stream(file);
    std::array<uint8_t, 6u> bytes{};
    ASSERT_EQ(file.read(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
    const std::array<uint8_t, 6u> expected{ 'A', 0u, 'B', 0u, 0u, 0u };
    EXPECT_EQ(bytes, expected);
}

/**
 * \brief   Reproduces the defect: the wide reader stopped at a valid character above U+00FF.
 **/
TEST(RemainingSmallFilesTest, wide_text_surrogate_pair_across_a_read_chunk_is_not_split)
{
    areg::FileBuffer file(TEXT_MODE);
    ASSERT_TRUE(file.open());
    const std::wstring source{ std::wstring(255u, L'a') + supplementary_text() + L"z" };
    ASSERT_TRUE(file.write_string(source.c_str()));
    reset_stream(file);
    areg::WideString result;
    EXPECT_EQ(file.read_string(result), static_cast<int32_t>(source.size()));
    EXPECT_EQ(result, areg::WideString(source.c_str()));
    EXPECT_TRUE(file.is_eof());
}

TEST(RemainingSmallFilesTest, wide_text_and_lines_keep_characters_above_latin1)
{
    areg::FileBuffer file(TEXT_MODE);
    ASSERT_TRUE(file.open());
    const std::wstring first{ L"\x65e5\x672c\x0410\x20ac" + supplementary_text() + L"\x00e9" };
    const std::wstring second{ L"\x4e2d\x6587" };
    ASSERT_TRUE(file.write_string((first + L"\n" + second).c_str()));
    reset_stream(file);
    areg::WideString line;
    EXPECT_EQ(file.read_line(line), static_cast<int32_t>(first.size() + 1u));
    EXPECT_EQ(line, areg::WideString(first.c_str()));
    wchar_t buffer[16]{};
    EXPECT_EQ(file.read_line(buffer, 16), static_cast<int32_t>(second.size()));
    EXPECT_EQ(std::wstring(buffer), second);
    EXPECT_TRUE(file.is_eof());
}

TEST(RemainingSmallFilesTest, wide_text_still_stops_at_a_control_character)
{
    areg::FileBuffer file(TEXT_MODE);
    ASSERT_TRUE(file.open());
    ASSERT_TRUE(file.write_string(L"\x65e5\x0001\x672c"));
    reset_stream(file);
    areg::WideString text;
    EXPECT_EQ(file.read_string(text), 2);
    EXPECT_EQ(text, areg::WideString(L"\x65e5"));
    EXPECT_EQ(file.read_string(text), 1);
    EXPECT_EQ(text, areg::WideString(L"\x672c"));
}

TEST(RemainingSmallFilesTest, printable_classification_covers_wide_characters_above_latin1)
{
    EXPECT_TRUE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x0100)));
    EXPECT_TRUE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x65e5)));
    EXPECT_TRUE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0xD83D)));
    EXPECT_TRUE(areg::is_printable<wchar_t>(L'A'));
    EXPECT_TRUE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x00e9)));
    EXPECT_FALSE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x0001)));
    EXPECT_FALSE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x007F)));
    EXPECT_FALSE(areg::is_printable<wchar_t>(static_cast<wchar_t>(0x0000)));
    EXPECT_FALSE(areg::is_printable<char>('\x01'));

    wchar_t text[]{ L"\x65e5\x672c\nnext" };
    wchar_t * next{ nullptr };
    const wchar_t * first{ areg::line<wchar_t>(text, areg::COUNT_ALL, &next) };
    EXPECT_EQ(std::wstring(first), std::wstring(L"\x65e5\x672c"));
    ASSERT_NE(next, nullptr);
    EXPECT_EQ(std::wstring(next), std::wstring(L"next"));
}

TEST(RemainingSmallFilesTest, wide_search_returns_byte_offsets_for_supplementary_text)
{
    areg::FileBuffer file(TEXT_MODE);
    ASSERT_TRUE(file.open());
    const std::wstring needle{ supplementary_text() + L"z" };
    const std::wstring source{ std::wstring(L"prefix") + needle + L"suffix" };
    ASSERT_TRUE(file.write_string(source.c_str()));
    EXPECT_EQ(file.search_text(0u, needle.c_str(), true), 12u);
    EXPECT_EQ(file.search_text(14u, needle.c_str(), true), areg::Cursor::INVALID_CURSOR_POSITION);
}

TEST(RemainingSmallFilesTest, wide_search_normalizes_out_of_range_code_points_to_replacement)
{
    if constexpr (sizeof(wchar_t) == 4u)
    {
        areg::FileBuffer file(TEXT_MODE);
        ASSERT_TRUE(file.open());
        ASSERT_TRUE(file.write_string(L"A\uFFFDB"));
        const wchar_t invalid[]{ static_cast<wchar_t>(0x110000u), L'\0' };
        EXPECT_EQ(file.search_text(0u, invalid, true), 2u);
    }
    else
    {
        EXPECT_EQ(sizeof(wchar_t), 2u);
    }
}

TEST(RemainingSmallFilesTest, role_only_address_uses_current_thread_address_without_requiring_a_running_dispatcher)
{
    const areg::String role("RemainingSmallFilesMissingRole");
    const areg::DispatcherThread& dispatcher{ areg::DispatcherThread::current_dispatcher_thread() };
    EXPECT_FALSE(dispatcher.is_running());
    const areg::ComponentAddress address(role);
    EXPECT_EQ(address.role_name(), role);
    EXPECT_EQ(address.thread_address(), dispatcher.address());
    ASSERT_TRUE(dispatcher.address().is_valid());
    EXPECT_TRUE(address.is_valid());
    EXPECT_EQ(address, areg::ComponentAddress(dispatcher.address(), role));
}

TEST(RemainingSmallFilesTest, role_only_address_normalizes_empty_and_truncates_long_roles)
{
    const areg::ComponentAddress empty{ areg::String() };
    EXPECT_EQ(empty.role_name(), areg::ComponentAddress::invalid_component_address().role_name());
    EXPECT_FALSE(empty.is_valid());
    const std::string source(areg::ITEM_NAMES_MAX_LENGTH + 1u, 'x');
#if defined(DEBUG) && GTEST_HAS_DEATH_TEST
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    EXPECT_DEATH(static_cast<void>(areg::ComponentAddress{ areg::String(source.c_str()) }), "");
#elif !defined(DEBUG)
    const areg::ComponentAddress longRole{ areg::String(source.c_str()) };
    EXPECT_EQ(longRole.role_name().length(), areg::ITEM_NAMES_MAX_LENGTH);
    EXPECT_EQ(longRole.role_name(), areg::String(std::string(areg::ITEM_NAMES_MAX_LENGTH, 'x').c_str()));
    const areg::String truncated(std::string(areg::ITEM_NAMES_MAX_LENGTH, 'x').c_str());
    EXPECT_TRUE(longRole.is_valid());
    EXPECT_EQ(longRole, areg::ComponentAddress(areg::DispatcherThread::current_dispatcher_thread().address(), truncated));
#endif
}

#if GTEST_HAS_DEATH_TEST
TEST(RemainingSmallFilesDeathTest, null_fatal_reason_is_reported_as_unknown)
{
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    EXPECT_DEATH(
        {
            disable_core_dump();
            static_cast<void>(areg::set_fatal_handler(nullptr));
            areg::fatal_error(nullptr);
        }
        , "areg fatal error: unknown");
}

TEST(RemainingSmallFilesDeathTest, fatal_callback_receives_the_original_reason)
{
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    EXPECT_EXIT(
        {
            static_cast<void>(areg::set_fatal_handler(&exiting_fatal_handler));
            areg::fatal_error("remaining-small-files");
        }
        , ::testing::ExitedWithCode(73), "fatal callback received: remaining-small-files");
}

TEST(RemainingSmallFilesDeathTest, nonfatal_environment_values_report_null_reason_and_return)
{
    ::testing::GTEST_FLAG(death_test_style) = "threadsafe";
    EXPECT_EXIT(
        {
            for (const char* value : { "", "0", "false" })
            {
                if (!set_lock_failure_mode(value))
                {
                    std::exit(74);
                }
                areg::report_lock_failure(nullptr);
            }
            std::exit(0);
        }
        , ::testing::ExitedWithCode(0), "areg lock failure: unknown\nareg lock failure: unknown\nareg lock failure: unknown");
}
#endif
