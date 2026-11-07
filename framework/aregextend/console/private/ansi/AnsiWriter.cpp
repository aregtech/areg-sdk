/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/ansi/AnsiWriter.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the output buffer of the ANSI console.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/console/private/ansi/AnsiWriter.hpp"

#include <algorithm>
#include <limits>
#include <string.h>

namespace {
    constexpr char      ESC         { '\x1B' };
    constexpr char      BEL         { '\x07' };
    constexpr std::string_view  CMD_CLEAR_EOL   { "\x1B[K" };
    constexpr std::string_view  CMD_CURSOR_HIDE { "\x1B[?25l" };
    constexpr std::string_view  CMD_CURSOR_SHOW { "\x1B[?25h" };

    inline bool _is_control(unsigned char ch) noexcept
    {
        return (ch < 0x20u) || (ch == 0x7Fu);
    }

    inline bool _is_utf8_tail(unsigned char ch) noexcept
    {
        return (ch & 0xC0u) == 0x80u;
    }

    //!< Returns the text without the new lines at the start and at the end.
    std::string_view _trim_new_lines(std::string_view text) noexcept
    {
        while (!text.empty() && ((text.front() == '\n') || (text.front() == '\r')))
        {
            text.remove_prefix(1u);
        }

        while (!text.empty() && ((text.back() == '\n') || (text.back() == '\r')))
        {
            text.remove_suffix(1u);
        }

        return text;
    }
} // namespace

namespace areg::ext {

void AnsiWriter::set_screen(int32_t columns, int32_t rows) noexcept
{
    mColumns = (columns > 0) ? columns : 0;
    mRows    = (rows    > 0) ? rows    : 0;
}

void AnsiWriter::move_to(areg::Coord pos) noexcept
{
    _put(ESC);
    _put('[');
    _put_number((pos.posY > 0 ? pos.posY : 0) + 1);
    _put(';');
    _put_number((pos.posX > 0 ? pos.posX : 0) + 1);
    _put('H');
}

void AnsiWriter::clear_at(areg::Coord pos) noexcept
{
    if (_room(pos) > 0)
    {
        move_to(pos);
        _put(CMD_CLEAR_EOL.data(), static_cast<uint32_t>(CMD_CLEAR_EOL.size()));
    }
}

int32_t AnsiWriter::text_at(areg::Coord pos, std::string_view text) noexcept
{
    int32_t last{ -1 };
    areg::Coord at{ pos };
    size_t start{ 0u };
    while ((mRows <= 0) || (at.posY < mRows))
    {
        const size_t end{ text.find('\n', start) };
        const int32_t room{ _room(at) };
        if (room > 0)
        {
            move_to(at);
            _put(CMD_CLEAR_EOL.data(), static_cast<uint32_t>(CMD_CLEAR_EOL.size()));
            _put_row(text.substr(start, (end == std::string_view::npos) ? std::string_view::npos : end - start), at.posX, room);
            last = at.posY;
        }

        if (end == std::string_view::npos)
        {
            break;
        }

        start = end + 1u;
        ++at.posY;
    }

    return last;
}

areg::Coord AnsiWriter::field(areg::Coord pos, std::string_view text, uint32_t caret) noexcept
{
    const int32_t room{ _room(pos) };
    if (room <= 0)
    {
        return pos;
    }

    caret = std::min(caret, static_cast<uint32_t>(text.size()));
    const uint32_t width{ static_cast<uint32_t>(room) };
    const uint32_t first{ (caret >= width) ? caret - width + 1u : 0u };
    const uint32_t count{ std::min(static_cast<uint32_t>(text.size()) - first, width) };

    move_to(pos);
    _put(CMD_CLEAR_EOL.data(), static_cast<uint32_t>(CMD_CLEAR_EOL.size()));
    _put(text.data() + first, count);
    return areg::Coord{ pos.posX + static_cast<int32_t>(caret - first), pos.posY };
}

void AnsiWriter::raw(std::string_view text) noexcept
{
    _put(text.data(), static_cast<uint32_t>(text.size()));
}

void AnsiWriter::plain_line(std::string_view text) noexcept
{
    text = _trim_new_lines(text);
    const bool printable = std::any_of(text.begin(), text.end(), [](char ch) { return !_is_control(static_cast<unsigned char>(ch)); });
    if (printable)
    {
        plain(text);
        _put('\n');
    }
}

void AnsiWriter::plain(std::string_view text) noexcept
{
    const uint32_t size{ static_cast<uint32_t>(text.size()) };
    for (uint32_t i = 0u; i < size; ++i)
    {
        const unsigned char ch{ static_cast<unsigned char>(text[i]) };
        if (ch == static_cast<unsigned char>(ESC))
        {
            const uint32_t length{ escape_length(text, i) };
            if (length == 0u)
            {
                break;
            }

            i += length - 1u;
        }
        else if ((_is_control(ch) == false) || (ch == '\n') || (ch == '\t'))
        {
            _put(static_cast<char>(ch));
        }
    }
}

void AnsiWriter::hide_cursor() noexcept
{
    _put(CMD_CURSOR_HIDE.data(), static_cast<uint32_t>(CMD_CURSOR_HIDE.size()));
}

void AnsiWriter::show_cursor() noexcept
{
    _put(CMD_CURSOR_SHOW.data(), static_cast<uint32_t>(CMD_CURSOR_SHOW.size()));
}

void AnsiWriter::flush() noexcept
{
    if ((mUsed != 0u) && (mSink != nullptr))
    {
        mSink(mData, mUsed, mContext);
    }

    mUsed = 0u;
}

uint32_t AnsiWriter::escape_length(std::string_view text, uint32_t index) noexcept
{
    const uint32_t size{ static_cast<uint32_t>(text.size()) };
    if ((index + 1u) >= size)
    {
        return 0u;
    }

    const unsigned char kind{ static_cast<unsigned char>(text[index + 1u]) };
    uint32_t next{ index + 2u };
    if (kind == '[')
    {
        for (; next < size; ++next)
        {
            const unsigned char ch{ static_cast<unsigned char>(text[next]) };
            if ((ch >= 0x40u) && (ch <= 0x7Eu))
            {
                return next - index + 1u;
            }
            else if ((ch < 0x20u) || (ch > 0x3Fu))
            {
                return 0u;
            }
        }
    }
    else if (kind == ']')
    {
        for (; next < size; ++next)
        {
            if (text[next] == BEL)
            {
                return next - index + 1u;
            }
            else if ((text[next] == ESC) && ((next + 1u) < size) && (text[next + 1u] == '\\'))
            {
                return next - index + 2u;
            }
        }
    }
    else if ((kind >= 0x20u) && (kind <= 0x2Fu))
    {
        for (; next < size; ++next)
        {
            const unsigned char ch{ static_cast<unsigned char>(text[next]) };
            if ((ch >= 0x30u) && (ch <= 0x7Eu))
            {
                return next - index + 1u;
            }
            else if ((ch < 0x20u) || (ch > 0x2Fu))
            {
                return 0u;
            }
        }
    }
    else if ((kind >= 0x30u) && (kind <= 0x7Eu))
    {
        return 2u;
    }

    return 0u;
}

void AnsiWriter::_put(const char* data, uint32_t size) noexcept
{
    while (size != 0u)
    {
        if (mUsed == CAPACITY)
        {
            flush();
        }

        const uint32_t count{ std::min(size, CAPACITY - mUsed) };
        ::memcpy(mData + mUsed, data, count);
        mUsed += count;
        data  += count;
        size  -= count;
    }
}

void AnsiWriter::_put_number(int32_t value) noexcept
{
    char digits[12]{ };
    uint32_t count{ 0u };
    uint32_t number{ static_cast<uint32_t>(value > 0 ? value : 0) };
    do
    {
        digits[count++] = static_cast<char>('0' + (number % 10u));
        number /= 10u;
    } while (number != 0u);

    while (count != 0u)
    {
        _put(digits[--count]);
    }
}

void AnsiWriter::_put_row(std::string_view row, int32_t column, int32_t limit) noexcept
{
    const uint32_t size{ static_cast<uint32_t>(row.size()) };
    int32_t used{ 0 };
    bool full{ false };
    for (uint32_t i = 0u; i < size; ++i)
    {
        const unsigned char ch{ static_cast<unsigned char>(row[i]) };
        if (ch == static_cast<unsigned char>(ESC))
        {
            const uint32_t length{ escape_length(row, i) };
            if (length == 0u)
            {
                break;
            }

            _put(row.data() + i, length);
            i += length - 1u;
        }
        else if (ch == '\t')
        {
            int32_t spaces{ TAB_SIZE - ((column + used) % TAB_SIZE) };
            for (; (spaces > 0) && (used < limit); --spaces, ++used)
            {
                _put(' ');
            }

            full = full || (spaces > 0);
        }
        else if (_is_control(ch))
        {
            continue;
        }
        else if (_is_utf8_tail(ch))
        {
            if (full == false)
            {
                _put(static_cast<char>(ch));
            }
        }
        else if ((full == false) && (used < limit))
        {
            _put(static_cast<char>(ch));
            ++used;
        }
        else
        {
            full = true;
        }
    }
}

int32_t AnsiWriter::_room(areg::Coord pos) const noexcept
{
    if ((pos.posX < 0) || (pos.posY < 0) || ((mRows > 0) && (pos.posY >= mRows)))
    {
        return 0;
    }
    else if (mColumns <= 0)
    {
        return std::numeric_limits<int32_t>::max();
    }

    // The last cell of the bottom row stays empty.
    const int32_t room{ mColumns - pos.posX - (((mRows > 0) && (pos.posY == mRows - 1)) ? 1 : 0) };
    return (room > 0) ? room : 0;
}

} // namespace areg::ext
