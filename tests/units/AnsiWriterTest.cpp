/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/AnsiWriterTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The output buffer of the ANSI console: clipping, rows, escapes, the input field.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/VtScreen.hpp"
#include "aregextend/console/private/AnsiWriter.hpp"

#include <string>
#include <memory>

using areg::ext::AnsiWriter;

namespace
{
    void _append(const char* data, uint32_t size, void* context)
    {
        static_cast<std::string*>(context)->append(data, size);
    }

    //!< A writer of a given screen size that collects its output.
    struct Output
    {
        std::string bytes;
        std::unique_ptr<AnsiWriter> writer;

        Output(int32_t columns, int32_t rows)
            : bytes()
            , writer(std::make_unique<AnsiWriter>(&_append, &bytes))
        {
            writer->set_screen(columns, rows);
        }

        //!< Flushes and renders the output on a screen of the given size.
        VtScreen render(int columns, int rows)
        {
            writer->flush();
            VtScreen screen(columns, rows);
            screen.feed(bytes);
            return screen;
        }
    };
}

TEST(AnsiWriterTest, move_to_writes_a_one_based_cursor_position)
{
    Output out(80, 24);
    out.writer->move_to({ 0, 0 });
    out.writer->move_to({ 4, 9 });
    out.writer->flush();
    EXPECT_EQ(out.bytes, "\x1B[1;1H\x1B[10;5H");
}

TEST(AnsiWriterTest, text_is_clipped_at_the_right_edge)
{
    Output out(10, 5);
    EXPECT_EQ(out.writer->text_at({ 6, 1 }, "ABCDEFGH"), 1);
    const VtScreen screen{ out.render(10, 5) };
    EXPECT_EQ(screen.row(1), "      ABCD");
    EXPECT_EQ(screen.wrapped(), 0);
    EXPECT_EQ(screen.scrolled(), 0);
}

// Reproduces the defect: a long text on the bottom row of a small terminal scrolled the screen.
TEST(AnsiWriterTest, bottom_row_never_scrolls_the_screen)
{
    Output out(10, 5);
    out.writer->text_at({ 0, 0 }, "title");
    EXPECT_EQ(out.writer->text_at({ 0, 4 }, "0123456789ABCDEF"), 4);
    const VtScreen screen{ out.render(10, 5) };
    EXPECT_EQ(screen.row(0), "title");
    EXPECT_EQ(screen.row(4), "012345678");
    EXPECT_EQ(screen.scrolled(), 0);
    EXPECT_EQ(screen.wrapped(), 0);
}

TEST(AnsiWriterTest, rows_below_the_screen_are_not_drawn)
{
    Output out(10, 5);
    EXPECT_EQ(out.writer->text_at({ 0, 3 }, "a\nb\nc\nd"), 4);
    EXPECT_EQ(out.writer->text_at({ 0, 5 }, "below"), -1);
    EXPECT_EQ(out.writer->text_at({ -1, 0 }, "left"), -1);
    const VtScreen screen{ out.render(10, 5) };
    EXPECT_EQ(screen.row(3), "a");
    EXPECT_EQ(screen.row(4), "b");
    EXPECT_FALSE(screen.shows("below"));
    EXPECT_FALSE(screen.shows("left"));
    EXPECT_EQ(screen.scrolled(), 0);
}

TEST(AnsiWriterTest, new_line_continues_on_the_next_row_at_the_same_column)
{
    Output out(20, 5);
    EXPECT_EQ(out.writer->text_at({ 3, 1 }, "ab\ncd\n"), 3);
    const VtScreen screen{ out.render(20, 5) };
    EXPECT_EQ(screen.row(1), "   ab");
    EXPECT_EQ(screen.row(2), "   cd");
    EXPECT_EQ(screen.row(3), "");
    EXPECT_EQ(screen.scrolled(), 0);
}

TEST(AnsiWriterTest, each_row_is_cleared_from_the_column)
{
    Output out(20, 5);
    out.writer->text_at({ 0, 1 }, "old text that is long");
    out.writer->text_at({ 4, 1 }, "new");
    const VtScreen screen{ out.render(20, 5) };
    EXPECT_EQ(screen.row(1), "old new");
}

// Reproduces the defect: the output of a string_view read past its size, up to the next zero byte.
TEST(AnsiWriterTest, string_view_output_stops_at_its_size)
{
    Output out(20, 5);
    out.writer->text_at({ 0, 0 }, std::string_view("ABCDEFG", 3));
    out.writer->flush();
    EXPECT_EQ(out.bytes.find('D'), std::string::npos);
    EXPECT_EQ(out.render(20, 5).row(0), "ABC");
}

TEST(AnsiWriterTest, escape_sequences_take_no_column_and_survive_the_clip)
{
    Output out(6, 3);
    out.writer->text_at({ 0, 0 }, "\x1B[31mABCDEFGH\x1B[0m");
    out.writer->flush();
    EXPECT_NE(out.bytes.find("\x1B[31m"), std::string::npos);
    EXPECT_NE(out.bytes.find("\x1B[0m"), std::string::npos);
    EXPECT_EQ(out.render(6, 3).row(0), "ABCDEF");
}

TEST(AnsiWriterTest, control_characters_are_dropped_and_a_tab_is_expanded)
{
    Output out(40, 3);
    out.writer->text_at({ 2, 0 }, "a\tb\x07\x01" "c\r");
    const VtScreen screen{ out.render(40, 3) };
    EXPECT_EQ(screen.row(0), "  a     bc");
}

TEST(AnsiWriterTest, utf8_character_is_never_split)
{
    Output out(3, 2);
    out.writer->text_at({ 0, 0 }, "ab\xC3\xA9\xC3\xA9");
    out.writer->flush();
    EXPECT_EQ(out.bytes.substr(out.bytes.size() - 4u), "ab\xC3\xA9");
}

TEST(AnsiWriterTest, unknown_size_clips_nothing)
{
    Output out(0, 0);
    const std::string text(300u, 'x');
    EXPECT_EQ(out.writer->text_at({ 0, 100 }, text), 100);
    out.writer->flush();
    EXPECT_NE(out.bytes.find(text), std::string::npos);
}

TEST(AnsiWriterTest, clear_at_skips_a_position_outside_the_screen)
{
    Output out(10, 5);
    out.writer->clear_at({ 10, 0 });
    out.writer->clear_at({ 0, 5 });
    out.writer->flush();
    EXPECT_TRUE(out.bytes.empty());
    out.writer->clear_at({ 2, 1 });
    out.writer->flush();
    EXPECT_EQ(out.bytes, "\x1B[2;3H\x1B[K");
}

TEST(AnsiWriterTest, field_keeps_the_caret_on_the_screen)
{
    Output out(10, 3);
    const areg::Coord atEnd{ out.writer->field({ 5, 0 }, "abcdefgh", 8u) };
    EXPECT_EQ(atEnd.posX, 9);
    EXPECT_EQ(atEnd.posY, 0);
    EXPECT_EQ(out.render(10, 3).row(0), "     efgh");

    Output start(10, 3);
    const areg::Coord atStart{ start.writer->field({ 5, 0 }, "abcdefgh", 2u) };
    EXPECT_EQ(atStart.posX, 7);
    EXPECT_EQ(start.render(10, 3).row(0), "     abcde");
}

TEST(AnsiWriterTest, field_on_the_bottom_row_leaves_the_last_cell)
{
    Output out(10, 3);
    const areg::Coord caret{ out.writer->field({ 5, 2 }, "abcdefgh", 8u) };
    EXPECT_EQ(caret.posX, 8);
    const VtScreen screen{ out.render(10, 3) };
    EXPECT_EQ(screen.row(2), "     fgh");
    EXPECT_EQ(screen.scrolled(), 0);
}

TEST(AnsiWriterTest, plain_line_strips_escapes_and_trims_new_lines)
{
    Output out(0, 0);
    out.writer->plain_line("\n\x1B[1mQuit\x1B[0m the application\n");
    out.writer->plain_line("");
    out.writer->plain_line("\n\n");
    out.writer->plain_line("two\nrows");
    out.writer->flush();
    EXPECT_EQ(out.bytes, "Quit the application\ntwo\nrows\n");
}

TEST(AnsiWriterTest, output_larger_than_the_buffer_arrives_in_order)
{
    Output out(0, 0);
    std::string expected;
    for (uint32_t i = 0u; i < 3u * AnsiWriter::CAPACITY; ++i)
        expected += static_cast<char>('a' + (i % 26u));

    out.writer->raw(expected.substr(0u, 100u));
    out.writer->raw(expected.substr(100u));
    EXPECT_LT(out.writer->pending(), AnsiWriter::CAPACITY + 1u);
    out.writer->flush();
    EXPECT_EQ(out.writer->pending(), 0u);
    EXPECT_EQ(out.bytes, expected);
}

TEST(AnsiWriterTest, escape_length_of_each_kind)
{
    EXPECT_EQ(AnsiWriter::escape_length("\x1B[12;34H", 0u), 8u);
    EXPECT_EQ(AnsiWriter::escape_length("ab\x1B[m", 2u), 3u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B]0;title\x07", 0u), 10u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B]0;t\x1B\\", 0u), 7u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B(B", 0u), 3u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B" "7", 0u), 2u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B[12", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B[1\n", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B", 0u), 0u);
}

TEST(AnsiWriterTest, field_outside_the_screen_draws_nothing)
{
    Output out(20, 5);
    const areg::Coord caret{ out.writer->field({ 3, 5 }, "abc", 2u) };
    EXPECT_EQ(caret.posX, 3);
    EXPECT_EQ(caret.posY, 5);
    EXPECT_EQ(out.writer->pending(), 0u);
}

TEST(AnsiWriterTest, incomplete_escape_ends_the_text)
{
    Output plain(20, 5);
    plain.writer->plain("ab\x1B[12");
    plain.writer->flush();
    EXPECT_EQ(plain.bytes, "ab");

    Output row(20, 5);
    row.writer->text_at({ 0, 1 }, "cd\x1B[3");
    EXPECT_EQ(row.render(20, 5).row(1), "cd");
}

TEST(AnsiWriterTest, escape_with_a_bad_intermediate_byte_has_no_length)
{
    EXPECT_EQ(AnsiWriter::escape_length("\x1B(\x01", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B( B", 0u), 4u);
}

TEST(AnsiWriterTest, escape_length_rejects_every_malformed_sequence)
{
    EXPECT_EQ(AnsiWriter::escape_length("\x1B[", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B[1\x7F", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B]0;t", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B]0;t\x1B", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B]0;t\x1Bx\x07", 0u), 8u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B(", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B( ", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B(\x7F", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B\x01", 0u), 0u);
    EXPECT_EQ(AnsiWriter::escape_length("\x1B\x7F", 0u), 0u);
}

TEST(AnsiWriterTest, plain_text_keeps_tabs_and_new_lines_only)
{
    Output out(0, 0);
    out.writer->plain("a\tb\x7F\r\n");
    out.writer->plain_line("\r\nX\r\n");
    out.writer->flush();
    EXPECT_EQ(out.bytes, "a\tb\nX\n");
}

TEST(AnsiWriterTest, flush_without_a_sink_drops_the_output)
{
    AnsiWriter writer(nullptr, nullptr);
    writer.raw("lost");
    EXPECT_EQ(writer.pending(), 4u);
    writer.flush();
    EXPECT_EQ(writer.pending(), 0u);
}

TEST(AnsiWriterTest, tab_at_the_right_edge_is_cut_and_ends_the_row)
{
    Output out(10, 3);
    out.writer->text_at({ 4, 0 }, "ab\tX\tY\tZ");
    EXPECT_EQ(out.render(10, 3).row(0), "    ab  X");
}

TEST(AnsiWriterTest, unknown_rows_clip_only_the_columns)
{
    Output out(10, 0);
    out.writer->text_at({ 0, -1 }, "hidden");
    out.writer->flush();
    EXPECT_TRUE(out.bytes.empty());
    out.writer->text_at({ 2, 40 }, "abcdefghijkl");
    out.writer->flush();
    EXPECT_NE(out.bytes.find("abcdefgh"), std::string::npos);
    EXPECT_EQ(out.bytes.find("abcdefghi"), std::string::npos);
}
